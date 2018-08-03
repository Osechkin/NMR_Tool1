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
//#include "c:\TI\bios_5_41_11_38\packages\ti\bios\include\tistdtypes.h"
#include "tistdtypes.h"

#include "../io_containers.h"
#include "../Galois/gf_data.h"


/* Структура принимаемых по UART сообщений, отправленных рабочей станцией оператора на поверхности:
 *
 * |-----------|	|-----------|	|----------|	...		|-----------|
 *   заголовок		   пакет 1		  пакет 2				   пакет N
 *
 *
 *  Заголовок сообщений:
 *
 *  1 байт	1 байт	1 байт	1 байт	1 байт	1 байт	1 байт	1 байт	   ( 4 байта )
 *  |-----|	|-----|	|-----|	|-----|	|-----|	|-----|	|-----|	|-----|	|-----|	...	|-----|
 *	 MARK	 ADDR	  ID	 NPACK	 LPACK	LBLOCK	 NERRS	 CRC8		 RSBYTES
 *
 *	 MARK	- стартовый маркер заголовка сообщения. Также кодирует тип сообщения (см. MTYPES)
 *	 ADDR	- адреса устройства-отправителя и устройства-адресата сообщения. Формат: биты 0-3 - адрес отправителя, биты 4-7 - адрес приемника
 *	 ID		- номер сообщения (всего от 0 до 255, т.е. номер сообщения не обязан быть уникальным в течение всей работы прибора ЯМК)
 *	 NPACK	- число пакетов в сообщении
 *	 LPACK	- длина пакетов в сообщении
 *	 LBLOCK	- длина блоков, на которые разбиваются пакеты для блочного кодирования Рида-Соломона
 *	 NERRS	- число ошибок, которые могут быть восстановлены в каждом блоке Рида-Соломона
 *	 CRC8	- контрольная сумма, которая вычисляется по байтам с MARK по NERRS
 *	 RSBYTES- проверочные байты, предназначенные для устранения ошибок в заголовке сообщения. Обычно число проверочных байт = 4
 *
 * *
 *  Пакеты сообщений (класс MsgPacket):
 *
 *  1 байт  1 байт  1 байт        N байт           1 байт
 *  |-----| |-----| |-----| |-------...--------| |-----|
 *   MARK     ID      NUM           Data           CRC8
 *
 *  MARK	- маркер пакета сообщения.
 *  ID		- номер сообщения (всего от 0 до 255, т.е. номер сообщения не обязан быть уникальным в течение всей работы прибора ЯМК)
 *  NUM		- номер пакета
 *  CRC8	- контрольная сумма по всему пакету, которая вычисляется с позиции MARK включительно
 *
 *
 *  После формирования пакета, производится кодирование по методу Рида-Соломона.
 *  В результате закодированный пакет представляет собой последовательность РС-блоков:
 *
 *  LBLOCK байт   LBLOCK байт       ...    LBLOCK байт
 *  |----------|  |----------|             |----------|
 *    данные        данные                   данные
 *
 *  LBLOCK	- длина блоков, на которые разбиваются пакеты для блочного кодирования Рида-Соломона (см. выше описание структуры MsgHeader)
 *
 *
 *  При этом структура каждого блока:
 *
 *     M байт    K байт
 *  |---------| |-------|			M + K = LBLOCK
 *     DATA      RSBYTES
 *
 *  DATA	- кодируемые данные. Здесь M = LBLOCK - K = LBLOCK - 2*NERRS (про NERRS см. описание MsgHeader выше)
 *  RSBYTES - проверочные байты, предназначенные для устранения ошибок в текущем блоке сообщения. Здесь K = 2*NERRS
 *
 */

#define HEAD_REC_LEN		4			// длина проверочной части заголовка сообщения
#define HEAD_INFO_LEN		8			// длина информационной части заголовка сообщения
#define HEADER_LEN			HEAD_REC_LEN + HEAD_INFO_LEN	// длина заголовка сообщения (msg_header)
#define SRV_MSG_LEN			HEAD_REC_LEN + HEAD_INFO_LEN	// длина служебного сообщения = длине заголовка msg_header
#define SHORT_MSG_LEN		HEAD_REC_LEN + HEAD_INFO_LEN	// длина короткого сообщения = длине заголовка msg_header
#define MAX_REC_ERRS		32			// максимальное число корректируемых ошибок в сообщениях
#define SRV_DATA_LEN		4			// длина данных в служебном сообщении

#define MAX_PACK_LEN		254			// максимальная длина пакета UART-сообщения
#define PACK_SRV_LEN		4			// длина служебных данных в пакете (маркер начала, id сообщения, номер пакета и crc)
#define PACK_HEAD_LEN		3			// длина заголовка пакета (маркер начала пакета, id сообщения и номер пакета)
#define MAX_PACK_CNT		255			// максимальное число пакетов в UART-сообщении
#define MAX_BODY_LEN		16384		// максимальная длина передаваемых данных в теле многопакетного сообщения (макс. длина всех пакетов)

#define True				1
#define False				0

#define E_MSG_OK			0			// result = OK
#define E_MSG_FAIL			1			// ошибка в сообщении
#define E_MSG_TYPE			2			// неверный тип сообщения
#define E_MSG_READER		3			// неверный адрес устройства-адресата
#define E_MSG_WRITER		4			// неверный адрес устройства-отправителя
#define E_MSG_LEN			5			// неверная длина тела сообщения
#define E_MSG_REC_LEN		6			// неверное число корректируемых ошибок
#define E_MSG_CRC			7			// ошибка контрольной суммы
#define E_MSG_ID			8			// неверный идентификатор сообщения (например, id сообщения в заголовке MsgHeader не совпадает с id сообщения в пакете и т.д.)


#define START_BYTE			0xAA		// стартовый байт перед сообщением/пакетом
#define STOP_BYTE			0x55		// стоповый байт после сообщения/пакета

typedef enum { 							// Маркеры начала UART сообщений, определяющие также тип сообщений
	MTYPE_MULTYPACK = 		0x45,		// маркер начала заголовка многопакетного сообщения
	MTYPE_PACKET = 			0x63,		// маркер пакета данных
	MTYPE_SERVICE = 		0x91,		// маркер служебного сообщения
	MTYPE_SHORT = 			0xB7		// маркер короткого сообщения (по структуре аналогично служебному сообщению)
} MTYPE;


// Header of UART message
typedef struct MsgHeader
{
	uint8_t msg_type;					// маркер начала сообщения. Определяет также тип сообщения.
	uint8_t reader;						// адрес устройства-адресата
	uint8_t writer;						// адрес устройства-отправителя	uint8_t
	uint8_t id;							// id сообщения
	uint8_t pack_count;					// число пакетов в сообщении
	uint8_t pack_len;					// длина пакетов в сообщении (все пакеты имеют одинаковую длину)
	uint8_t block_len;					// длина блоков Рида-Соломона, на которые разбивается пакет
	uint8_t rec_errs;					// число восстанавливаемых ошибок в каждом блоке пакета
	uint8_t data[SRV_DATA_LEN];			// служебные данные (только для служебных сообщений типа MTYPE_SERVICE ! )
	uint8_t crc;						// контрольная сумма
	uint8_t rec_data[HEAD_REC_LEN];		// массив проверочных байт
} MsgHeader;


// Body of UART message
typedef struct MsgBody
{
	uint8_t  data_len;					// длина массива данных в теле сообщения
	uint8_t* data;						// массив данных вместе с массивом проверочных байт в теле сообщения
	uint8_t  crc;						// контрольная сумма по массиву данных в теле сообщения
	uint8_t  rec_data_len;				// длина массива проверочных байт
} MsgBody;


// Packet for UART Message
typedef struct MsgPacket
{
	uint8_t start_marker;				// маркер начала пакета
	uint8_t msg_id;						// id сообщения, к которому относится пакет
	uint8_t packet_number;				// номер пакета
	uint8_t crc;						// контрольная сумма по всем данным в пакете

	uint8_t data[MAX_PACK_LEN];			// массив данных, содержащийся в пакете
	uint8_t data_len;					// длина массива данных в пакете

	uint8_t block_len;					// длина блоков Рида-Соломона, на которые разбивается пакет
	uint8_t rec_errs;					// число восстанавливаемых ошибок в каждом блоке пакета
	uint8_t pack_len;					// длина пакета (pack_len должно начело делиться на block_len)
} MsgPacket;


typedef struct MsgCommSettings
{
	uint8_t block_len;					// длина блоков Рида-Соломона, на которые разбивается пакет
	uint8_t rec_errs;					// число восстанавливаемых ошибок в каждом блоке пакета
	uint8_t pack_len;					// длина пакетов
	uint8_t pack_delay;					// задержка между пакетами
	Bool antinoise_coding;				// применять/не применять помехоустойчивое кодирование
	Bool packlen_autoadjust;			// оптимизация длины пакетов (вкл./выкл.)
	Bool interleaving;					// применение/не применение алгоритма "интерливинга" (перемешивания байтов в пакетах)
} MsgCommSettings;

// UART Message
typedef struct UART_Message
{
	MsgHeader *msg_header;				// заголовок сообщения
	MsgPacket *msg_packs[MAX_PACK_CNT]; // пакеты с данными сообщения
	uint16_t pack_cnt;					// количество пакетов в сообщении
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
