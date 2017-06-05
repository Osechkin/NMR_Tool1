/*
 * io_containers.h
 *
 *  Created on: 02.04.2014
 *      Author: Andrey
 */

#ifndef UART_MESSAGES_H_
#define UART_MESSAGES_H_


#include "stdint.h"
#include "stdlib.h"
#include "tistdtypes.h"

#include "../io_containers.h"
#include "../Galois/gf_data.h"


/* ��������� ����������� �� UART ���������, ������������ ������� �������� ��������� �� �����������:
 *
 * |-----------|	|-----------|	|----------|	...		|-----------|
 *   ���������		   ����� 1		  ����� 2				   ����� N
 *
 *
 *  ��������� ���������:
 *
 *  1 ����	1 ����	1 ����	1 ����	1 ����	1 ����	1 ����	1 ����	   ( 4 ����� )
 *  |-----|	|-----|	|-----|	|-----|	|-----|	|-----|	|-----|	|-----|	|-----|	...	|-----|
 *	 MARK	 ADDR	  ID	 NPACK	 LPACK	LBLOCK	 NERRS	 CRC8		 RSBYTES
 *
 *	 MARK	- ��������� ������ ��������� ���������. ����� �������� ��� ��������� (��. MTYPES)
 *	 ADDR	- ������ ����������-����������� � ����������-�������� ���������. ������: ���� 0-3 - ����� �����������, ���� 4-7 - ����� ���������
 *	 ID		- ����� ��������� (����� �� 0 �� 255, �.�. ����� ��������� �� ������ ���� ���������� � ������� ���� ������ ������� ���)
 *	 NPACK	- ����� ������� � ���������
 *	 LPACK	- ����� ������� � ���������
 *	 LBLOCK	- ����� ������, �� ������� ����������� ������ ��� �������� ����������� ����-��������
 *	 NERRS	- ����� ������, ������� ����� ���� ������������� � ������ ����� ����-��������
 *	 CRC8	- ����������� �����, ������� ����������� �� ������ � MARK �� NERRS
 *	 RSBYTES- ����������� �����, ��������������� ��� ���������� ������ � ��������� ���������. ������ ����� ����������� ���� = 4
 *
 * *
 *  ������ ��������� (����� MsgPacket):
 *
 *  1 ����  1 ����  1 ����        N ����           1 ����
 *  |-----| |-----| |-----| |-------...--------| |-----|
 *   MARK     ID      NUM           Data           CRC8
 *
 *  MARK	- ������ ������ ���������.
 *  ID		- ����� ��������� (����� �� 0 �� 255, �.�. ����� ��������� �� ������ ���� ���������� � ������� ���� ������ ������� ���)
 *  NUM		- ����� ������
 *  CRC8	- ����������� ����� �� ����� ������, ������� ����������� � ������� MARK ������������
 *
 *
 *  ����� ������������ ������, ������������ ����������� �� ������ ����-��������.
 *  � ���������� �������������� ����� ������������ ����� ������������������ ��-������:
 *
 *  LBLOCK ����   LBLOCK ����       ...    LBLOCK ����
 *  |----------|  |----------|             |----------|
 *    ������        ������                   ������
 *
 *  LBLOCK	- ����� ������, �� ������� ����������� ������ ��� �������� ����������� ����-�������� (��. ���� �������� ��������� MsgHeader)
 *
 *
 *  ��� ���� ��������� ������� �����:
 *
 *     M ����    K ����
 *  |---------| |-------|			M + K = LBLOCK
 *     DATA      RSBYTES
 *
 *  DATA	- ���������� ������. ����� M = LBLOCK - K = LBLOCK - 2*NERRS (��� NERRS ��. �������� MsgHeader ����)
 *  RSBYTES - ����������� �����, ��������������� ��� ���������� ������ � ������� ����� ���������. ����� K = 2*NERRS
 *
 */

#define HEAD_REC_LEN		4			// ����� ����������� ����� ��������� ���������
#define HEAD_INFO_LEN		8			// ����� �������������� ����� ��������� ���������
#define HEADER_LEN			HEAD_REC_LEN + HEAD_INFO_LEN	// ����� ��������� ��������� (msg_header)
#define SRV_MSG_LEN			HEAD_REC_LEN + HEAD_INFO_LEN	// ����� ���������� ��������� = ����� ��������� msg_header
#define SHORT_MSG_LEN		HEAD_REC_LEN + HEAD_INFO_LEN	// ����� ��������� ��������� = ����� ��������� msg_header
#define MAX_REC_ERRS		32			// ������������ ����� �������������� ������ � ����������
#define SRV_DATA_LEN		4			// ����� ������ � ��������� ���������

#define MAX_PACK_LEN		254			// ������������ ����� ������ UART-���������
#define PACK_SRV_LEN		4			// ����� ��������� ������ � ������ (������ ������, id ���������, ����� ������ � crc)
#define PACK_HEAD_LEN		3			// ����� ��������� ������ (������ ������ ������, id ��������� � ����� ������)
#define MAX_PACK_CNT		255			// ������������ ����� ������� � UART-���������
#define MAX_BODY_LEN		16384		// ������������ ����� ������������ ������ � ���� �������������� ��������� (����. ����� ���� �������)

#define True				1
#define False				0

#define E_MSG_OK			0			// result = OK
#define E_MSG_FAIL			1			// ������ � ���������
#define E_MSG_TYPE			2			// �������� ��� ���������
#define E_MSG_READER		3			// �������� ����� ����������-��������
#define E_MSG_WRITER		4			// �������� ����� ����������-�����������
#define E_MSG_LEN			5			// �������� ����� ���� ���������
#define E_MSG_REC_LEN		6			// �������� ����� �������������� ������
#define E_MSG_CRC			7			// ������ ����������� �����
#define E_MSG_ID			8			// �������� ������������� ��������� (��������, id ��������� � ��������� MsgHeader �� ��������� � id ��������� � ������ � �.�.)


#define START_BYTE			0xAA		// ��������� ���� ����� ����������/�������
#define STOP_BYTE			0x55		// �������� ���� ����� ���������/������

typedef enum { 							// ������� ������ UART ���������, ������������ ����� ��� ���������
	MTYPE_MULTYPACK = 		0x45,		// ������ ������ ��������� �������������� ���������
	MTYPE_PACKET = 			0x63,		// ������ ������ ������
	MTYPE_SERVICE = 		0x91,		// ������ ���������� ���������
	MTYPE_SHORT = 			0xB7		// ������ ��������� ��������� (�� ��������� ���������� ���������� ���������)
} MTYPE;


// Header of UART message
typedef struct MsgHeader
{
	uint8_t msg_type;					// ������ ������ ���������. ���������� ����� ��� ���������.
	uint8_t reader;						// ����� ����������-��������
	uint8_t writer;						// ����� ����������-�����������	uint8_t
	uint8_t id;							// id ���������
	uint8_t pack_count;					// ����� ������� � ���������
	uint8_t pack_len;					// ����� ������� � ��������� (��� ������ ����� ���������� �����)
	uint8_t block_len;					// ����� ������ ����-��������, �� ������� ����������� �����
	uint8_t rec_errs;					// ����� ����������������� ������ � ������ ����� ������
	uint8_t data[SRV_DATA_LEN];			// ��������� ������ (������ ��� ��������� ��������� ���� MTYPE_SERVICE ! )
	uint8_t crc;						// ����������� �����
	uint8_t rec_data[HEAD_REC_LEN];		// ������ ����������� ����
} MsgHeader;


// Body of UART message
typedef struct MsgBody
{
	uint8_t  data_len;					// ����� ������� ������ � ���� ���������
	uint8_t* data;						// ������ ������ ������ � �������� ����������� ���� � ���� ���������
	uint8_t  crc;						// ����������� ����� �� ������� ������ � ���� ���������
	uint8_t  rec_data_len;				// ����� ������� ����������� ����
} MsgBody;


// Packet for UART Message
typedef struct MsgPacket
{
	uint8_t start_marker;				// ������ ������ ������
	uint8_t msg_id;						// id ���������, � �������� ��������� �����
	uint8_t packet_number;				// ����� ������
	uint8_t crc;						// ����������� ����� �� ���� ������ � ������

	uint8_t data[MAX_PACK_LEN];			// ������ ������, ������������ � ������
	uint8_t data_len;					// ����� ������� ������ � ������

	uint8_t block_len;					// ����� ������ ����-��������, �� ������� ����������� �����
	uint8_t rec_errs;					// ����� ����������������� ������ � ������ ����� ������
	uint8_t pack_len;					// ����� ������ (pack_len ������ ������ �������� �� block_len)
} MsgPacket;


typedef struct MsgCommSettings
{
	uint8_t block_len;					// ����� ������ ����-��������, �� ������� ����������� �����
	uint8_t rec_errs;					// ����� ����������������� ������ � ������ ����� ������
	uint8_t pack_len;					// ����� �������
	uint8_t pack_delay;					// �������� ����� ��������
	Bool antinoise_coding;				// ���������/�� ��������� ���������������� �����������
	Bool packlen_autoadjust;			// ����������� ����� ������� (���./����.)
	Bool interleaving;					// ����������/�� ���������� ��������� "������������" (������������� ������ � �������)
} MsgCommSettings;

// UART Message
typedef struct UART_Message
{
	MsgHeader *msg_header;				// ��������� ���������
	MsgPacket *msg_packs[MAX_PACK_CNT]; // ������ � ������� ���������
	uint16_t pack_cnt;					// ���������� ������� � ���������
} UART_Message;



uint8_t Crc8(uint8_t *pcBlock, uint16_t len);
uint16_t Crc16(uint8_t *pcBlock, uint16_t len);

uint8_t mod8(uint8_t a, uint8_t b);


void initMsgHeader(MsgHeader *_msg_header);
void clearMsgHeader(MsgHeader *_msg_header);
void setMsgWriter(uint8_t addr, MsgHeader *_msg_header);
void setMsgReader(uint8_t addr, MsgHeader *_msg_header);
int  findMsgHeader(QUEUE8 *_queue, MsgHeader *_msg_header, GF_Data *_gf_data);
int  checkMsgHeader(MsgHeader *_msg_header);
void makeMsgHeader(MsgHeader *_msg_header, GF_Data *_gf_data);

void initMsgBody(uint8_t _data_len, uint8_t _rec_errs, MsgBody* _msg_body);
void clearMsgBody(MsgBody *_msg_body);
int  findMsgBody(QUEUE8 *_queue, MsgHeader *_msg_header, MsgBody *_msg_body, GF_Data *_gf_data);
int  checkMsgBody(MsgBody *_msg_body);

void initMsgPacket(MsgHeader *_msg_header, MsgPacket *_msg_pack, uint8_t pack_num);
void clearMsgPacket(MsgPacket *_msg_pack);
void deleteMsgPackets(UART_Message *_uart_msg);
void pushDataToMsgPacket(uint8_t *data, uint16_t data_len, uint16_t *pos, MsgPacket *_msg_pack, GF_Data *_gf_data);
void calcRecoveryPart(uint8_t *src, uint8_t *dst, MsgPacket *_msg_pack, GF_Data *_gf_data);
int findMsgPackets(QUEUE8 *_queue, UART_Message *_uart_msg, GF_Data *_gf_data);
int findMsgPackets2(BUFFER8 *_buff, UART_Message *_uart_msg, GF_Data *_gf_data);
int checkMsgPackets(UART_Message *_uart_msg);
int estimateBestPackLen(int data_len, int block_len, int rs_part_len);


#endif /* UART_MESSAGES_H_ */
