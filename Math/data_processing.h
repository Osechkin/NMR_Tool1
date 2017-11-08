/*
 * data_processing.h
 *
 *  Created on: 18.05.2015
 *      Author: Admin
 */

#ifndef DATA_PROCESSING_H_
#define DATA_PROCESSING_H_

#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "../io_containers.h"
#include "../common_data.h"


typedef struct OutBuffer // ��������� ��� �������� ������
{
	float *out_data; 						// ����� ��� �������� ������
	int outdata_counter; 					// ������� �������� ������ � �������� ������
	uint8_t data_id[OUTCMD_BUFF_SIZE]; 		// ������ ����� ������ (�������� ������ � ���, �����������-�������������� �������� ������ � �.�.) � �������� ������
	int group_index[OUTCMD_BUFF_SIZE];		// ������ ������� ����� ������
	int outdata_len[OUTCMD_BUFF_SIZE]; 		// ������ ���� �������� ������ � �������� ������
	uint8_t channel_id[OUTCMD_BUFF_SIZE];	// ������ ������� ������� ������ (������ ���1, ������ ���2 � �.�.)
	int full_size; 							// ����� ����� ������ � ������
} OutBuffer;

void OutBuffer_Init(OutBuffer *out_buff, float *data);
void OutBuffer_AddData(OutBuffer *out_buff, float *src, int src_len, uint8_t src_data_id, int src_gr_index);
void OutBuffer_ClearAll(OutBuffer *out_buff);


typedef struct SummationBuffer // ��������� ��� ����������� ���������� ������ (��������, ��������� �� ���)
{
	float *sum_data; 	// ������
	float *xx;			// ��������� �� ������ ����� XX (XX[0..3] = X0, X1, X2 � X3)
	int max_size; 		// ����� ���������� ��������� ���� float ��� ���������� sum_data
	int pos; 			// ������� ������� � ������� sum_data
	int group_index;	// ����� ������ ������ (��������, ����� �������� ������� ������ ��������� �� �����-��������)
	uint8_t data_id; 	// ��� ������
	uint8_t channel_id;	// ����� ������ ������ (������ ���1, ������ ���2 � �.�.)
} SummationBuffer;

void SummationBuffer_Init(SummationBuffer *sum_buf, float *data, int max_len, float *xx);
void SummationBuffer_ClearAll(SummationBuffer *sum_buf);


#define	MAX_PROCS			32
#define MAX_PROC_LEN		1024


#define MAX_TIMINGS			200
typedef struct TimingData
{
	unsigned int 	time[MAX_TIMINGS];					// ������� ������� � ������ ���������� (1 ���� = 1/300000000 ��� )
	int 			hard_echo_counter[MAX_TIMINGS];		// �������� hard_echo_counter (����� �������� ��� �� ����������� �������� GPIO2)
	int 			device_id[MAX_TIMINGS];				// id ����������, � ������� � ������ ������ ���� ������ (��. NMR_TOOL, GAMMA_TOOL � �.�.)
	int 			pack_id[MAX_TIMINGS];				// ����� ������ ����������, �������������� � ������ ������ (���������� ����� GPIO5-12)
	int 			count;								// ������� ������� � �������� time, hard_echo_counter, device_id, pack_id
	int 			target_loop;						// ����� "�����" ���������, �� ������� ���������� ����������� ����������
	int				loop;								// ����� �������� "�����" ���������
} TimingData;

void TimingData_ClearAll(TimingData *tim_data);
void TimingData_Clear(TimingData *tim_data);
void TimingData_Print(TimingData *tim_data);


#define MAX_TIMING_BUFF_LEN			4096
typedef struct TimingProc_Buffer
{
	uint32_t time[MAX_TIMING_BUFF_LEN];			// ������ ������� (���)
	uint32_t proc_id[MAX_TIMING_BUFF_LEN];		// ��� ��������, ����������� � ������ ������� time
	uint16_t len;								// ����� ������
	uint64_t tsc_init;							// �������� �������� �������� DSP CPU (TSCH | TSCL)
} TimingProc_Buffer;
// proc_id:
// |-- H.byte 4 --|--- byte 3 ---|--- byte 2 ---|-- 1 byte 1 --|
// H.byte 4 (highest byte) - �� ������������
// byte 3 - id ����������, � ������� � ������ ������ ���� ������ (��. NMR_TOOL, GAMMA_TOOL � �.�.)
// byte 2 - hard_echo_counter
// L.byte 1 (lowest byte) - GPIO 1-3 state: 4 h.bits - GPIO status, 4 l.bits - GPIO number (1,2 or 3)
//
// out_cond:
// 0 - ����� ����������� ������ � ������ ���������� ��� � ��������� �������
// 1 - ���� �� ���������� GPIO3 ����� ��� �� ��������� max_echo

void TimingProc_Buffer_Init(TimingProc_Buffer *timproc_buff, uint32_t tsch, uint32_t tscl);
Bool TimingProc_Buffer_Add(TimingProc_Buffer *timproc_buff, uint16_t prid, uint32_t tsch, uint32_t tscl);
uint32_t TimingProc_Buffer_ProcId_At(TimingProc_Buffer *timproc_buff, uint16_t index);
uint32_t TimingProc_Buffer_Time_At(TimingProc_Buffer *timproc_buff, uint16_t index);
uint16_t TimingProc_Buffer_Count(TimingProc_Buffer *timproc_buff);
void TimingProc_Buffer_Print1(TimingProc_Buffer *timproc_buff, uint16_t echo_index);	// echo_index - ����� "�����" ���������, �� ������� ���������� ����������� ����������
void TimingProc_Buffer_Print2(TimingProc_Buffer *timproc_buff);							// ����������� ����� �����������



typedef unsigned short Bool;
#define False           	0
#define True            	1


// ���� ������������ �������� ��� ��������� ��� ������� INS_DO_OPER
#define OPER_ADD			1	// ��� �������� ��� ������� - ��������
#define OPER_SUB			2	// ��� �������� ��� ������� - ���������
#define OPER_MUL			3	// ��� �������� ��� ������� - ���������
#define OPER_VD1			4	// ��������� �.�. �1
#define OPER_DIV			5	// ��� �������� ��� ������� - �������


// ���� ��������� ���������� ��������� ������ ��������� INS_DO_XX_OPER, INS_ADD_TO_XX
#define GR_SUM_BUFF			1	// group index � ������ ���� SummationBuffer
#define X0					2	// ������ X0
#define X1					3	// ������ X1
#define X2					4	// ������ X2
#define X3					5	// ������ X3


// ���������� ��� ��������� ������ ���������� �����������
#define INS_NO_OP			4	// "������" ���������� ("��� ��������")
#define INS_IF_DATA_NUM		5	// ���������� ��������� ����� ����������, ���� ����������� ������� ��������� ������ ������� ������ (���, ���) ��������, ����� - ������� ���� ����������
//#define INS_OPER_FID		8	// ���������� ������� ����� �� � upp_buffer � DATA_MAX_LEN ����������� �����. ��������� ����������� � i16_buffer
//#define INS_OPER_SE		9	// ���������� ������� ����� ��� � upp_buffer � DATA_MAX_LEN ����������� �����. ��������� ����������� � i16_buffer

#define INS_OPER_FID_ORG	6	// ���������� ������� ��� ����� ��� � ������ ��� � ����� ui16_buffer
#define INS_OPER_SE_ORG		7	// ���������� ������� ��� ����� �� � ������ ��� � ����� ui16_buffer
#define INS_OPER_FID		8	// ���������� ������� ��� ����� ���
#define INS_OPER_SE			9	// ���������� ������� ��� ����� ��
#define INS_QD_FID			10	// ������������ �������������� ����������� ������� ���
#define INS_QD_SE			11	// ������������ �������������� ����������� ������� ��
#define INS_WIN_TIME		12	// ������� ���������� ������� ������� ��� ������ �� ��������� �������������
#define INS_WIN_FREQ		13	// ������� ���������� ������� ������� ��� ������ � ��������� �������������
#define INS_OPER_FID_D		14	// ���������� ������� ��� ����� ��� � ����������� ������ � 2 ���� � ������� � �������� ������� ����� 2048 (Re = ������ ���, Im = 2048)
#define INS_OPER_SE_D		15	// ���������� ������� ��� ����� �� � ����������� ������ � 2 ���� � ������� � �������� ������� ����� 2048 (Re = ������ ���, Im = 2048)
#define INS_MV_ST_OUTBUF	16	// ������� ������� ������ � ������� ����� � �������� �����. ��� ������ ������������ ������� ������ ������� (��. ������ �������)
#define INS_WR_XX_SUMBUF	17	// ����������� ����� �� ������ XX (XX = X0, X1, X2 ��� X3) � ����� ������������
#define INS_IF_COND			19	// ���������� ���������� ����� ������ �������, ���� ��������� ���������� �����, ����� - ������� �� �������� ����� ���������� (��. ������ �������)
#define INS_WR_X0			20	// ������ ����� ���� float � ������ X0
#define INS_WR_X1			21	// ������ ����� ���� float � ������ X1
#define INS_WR_X2			22	// ������ ����� ���� float � ������ X2
#define INS_WR_X3			23	// ������ ����� ���� float � ������ X3
#define INS_IF_COND_X0		24	// ���������� ���������� ����� ������ �������, ���� X0 ������������� ��������� �������, ����� - ������� ��������� ���������� ���������� (��. ������ �������)
#define INS_IF_COND_X1		25	// ���������� ���������� ����� ������ �������, ���� X1 ������������� ��������� �������, ����� - ������� ��������� ���������� ���������� (��. ������ �������)
#define INS_IF_COND_X2		26	// ���������� ���������� ����� ������ �������, ���� X2 ������������� ��������� �������, ����� - ������� ��������� ���������� ���������� (��. ������ �������)
#define INS_IF_COND_X3		27	// ���������� ���������� ����� ������ �������, ���� X3 ������������� ��������� �������, ����� - ������� ��������� ���������� ���������� (��. ������ �������)
#define INS_APP_WIN_TIME	28	// ���������� ������� ������� ��� ��������� ������� � ������ � �����
#define INS_APP_WIN_FREQ	29	// ���������� ������� ������� ��� ��������� ������� � ������ � �����

#define INS_DO_XX_OPER		30	// �������������� �������� ��� ������� XX
#define INS_ADD_TO_XX		31	// �������� ����� � �������� ������ XX
#define INS_WR_ACC_GRIX		32	// ������ ����� ���� int32_t � ���� group_index �������������� �������

#define INS_CL_ST			34	// ������� �����
#define INS_ZERO_ST			35	// ��������� ������ �����, ����������� �� ������� �����
#define INS_NAN_ST			36	// ��������� ������ NaN (0xFFFFFFFF) �����, ����������� �� ������� �����
#define INS_WR_D0_ST		37	// ��������� ������ �� ������ data_org (D0) � ���� ������
#define INS_WR_D1_ST		38	// ��������� ������ �� ������ data1 (D1) � ���� ������
#define INS_WR_D2_ST		39	// ��������� ������ �� ������ data2 (D2) � ���� ������
#define INS_WR_D3_ST		40	// ��������� ������ �� ������ data3 (D3) � ���� ������
#define INS_WR_DX_ST		56	// ��������� ������ �� ������ dataX (DX = D0 - D7) � ���� ������
#define INS_CP_ST_D0		41	// ����������� ������ �� ����� ������ � ����� data_org (D0) c ����������� �����
#define INS_CP_ST_D1		42	// ����������� ������ �� ����� ������ � ����� data1 (D1) c ����������� �����
#define INS_CP_ST_D2		43	// ����������� ������ �� ����� ������ � ����� data2 (D2) c ����������� �����
#define INS_CP_ST_D3		44	// ����������� ������ �� ����� ������ � ����� data3 (D3) c ����������� �����
#define INS_CP_ST_DX		55	// ����������� ������ �� ����� ������ � ����� dataX (DX = D0 - D7) c ����������� �����
#define INS_MV_ST_D0		45	// ������� ������ �� ����� ������ � ����� data_org (D0)
#define INS_MV_ST_D1		46	// ������� ������ �� ����� ������ � ����� data1 (D1)
#define INS_MV_ST_D2		47	// ������� ������ �� ����� ������ � ����� data2 (D2)
#define INS_MV_ST_D3		51	// ������� ������ �� ����� ������ � ����� data3 (D3)
#define INS_MV_ST_DX		54	// ������� ������ �� ����� ������ � ����� dataX (DX = D0 - D7)
#define INS_ST_SWAP			52	// ������������ � ����� �������� �� ������� ����� � ����������� ���������
#define INS_FFT				48	// ������� �����-��������������
#define INS_FPW				49	// ���������� ������� ��������
#define INS_FAMPL			53	// ���������� ������������ �������

#define INS_GO_TO			50	// ����������� ������� �� �������� ���������� ����������

#define INS_WR_HX_ST		57	// ��������� ������ �� ���������� data_heapX (data_heap = H0 - H127) � ���� ������
#define INS_CP_ST_HX		58	// ����������� ������ �� ����� ������ � ��������� data_heapX (data_heap = H0 - H127) c ����������� �����
#define INS_MV_ST_HX		59	// ������� ������ �� ����� ������ � ��������� data_heapX (data_heap = H0 - H127)

#define INS_GET_GAMMA		60	// �������� ��������� �����-������� � �������� ��������� � ���� �� ����� XX. ��� ������XX - �������� ����������

#define INS_START_SDSP		61	// ��������� ��������� SDSP
#define INS_GET_SDSP		62	// �������� ��������� SDSP � �������� ��������� � �������� �����

#define INS_XX_TO_OUTBUF	63	// ������� ����� �� ������ XX � �������� ������. ��� ������ - �������� ����������, ��� ������, ������������ � XX, - ������ ���� ����������
#define INS_ACC_TO_OUTBUF	64	// ������� ������ �� �������������� ������� � �������� ������
#define INS_DO_OPER1		65	// ���������� ������������ �������� �������� ��� ������� � ����� � ������ � ������ XX. ��� �������� � ��� ������XX - ��������� ����������
#define INS_DO_OPER2		66	// ���������� ������������ �������� �������� ��� ����� �������� � �����. ��� �������� - �������� ����������
#define INS_ACC_DAT			67	// ���������� ������ � ������
#define INS_SUM_DAT			68  // ������� ��� �������� �������
#define INS_SUM_REL_DAT		69  // ������� ��� �������� ������� ��� ������ � �������� ����� ��������������� ����� (������� �� ���������� ���������� � ������� ������������ ������)
#define INS_AMP1			70	// ������ ��������� ������� �� ����������� �������
#define INS_SPEC_MAX		71	// ������ ��������� ��������� ������� � ���������� ���������� ��������
#define INS_ST_AVER			72	// ��������� ������ � ����� ������ � ��������� ��������� � ���� �� ����� X0, X1, X2 ��� X3
#define INS_SMOOTH_DAT		73	// ���������� ������ � ������ � ��������������� ������������
#define INS_CL_SUMBUFF		74	// �������� ����� ��������������� �����

#define INS_STOP_SDSP		75	// ��������� ������ SDSP (��������� �������, ��������� ���������� � �.�.)

#define INS_EMUL_SE_NS		76	// ����������� �������� ��� + ��� � �������� ���������� "������/���" � ��������� �� ������ � ������ UPP (upp_buffer)
#define INS_EMUL_FID_NS		77	// ����������� ��� + ��� � �������� ���������� "������/���" � ��������� �� ������ � ������ UPP (upp_buffer)
#define INS_EMUL_NS			78	// ����������� ��� � ��������� �� ������ � ������ UPP (upp_buffer)
#define INS_EMUL_FID		79	// ����������� ������ ��� � ��������� �� ������ � ������ UPP (upp_buffer)
#define INS_EMUL_SE			80	// ����������� ������ ��������� ��� � ��������� �� ������ � ������ UPP (upp_buffer)

#define INS_START_SDSP_ADC	81	// ��������� ��� ���������������� ������� SDSP
#define INS_GET_SDSP_ADC	82	// ������� ����� � ��� ���������������� ������� SDSP

#define INS_ST_DEC_OUTBUF	83	// ��������� ������ � ����� � ��������� �� �����
#define INS_ST_QUAD			84	// �������� ���������� ���������� �������

#define INS_CLEAR_HX		89	// ������� �������� ���������� data_heapX (data_heap = H0 - H127)
#define INS_NOISE_PROC1		90	// ����� #1 ��������� ������ ����
#define INS_SGN_PROC1		91	// ����� #1 ��������� ������� ��������� ���
#define INS_NOISE_UPP_PRE1	92	// ����� #1 ���������� � ��������������� ��������� ������ ���� � ������ ���
#define INS_SGN_UPP_PRE1	93	// ����� #1 ���������� � ��������������� ��������� ������ ������� � ������ ���
#define INS_NOISE_PROC2		94	// ����� #2 ��������� ������ ����
#define INS_SGN_PROC2		95	// ����� #2 ��������� ������� ��������� ���
#define INS_NS_SGN_UPP_PRE3	96	// ����� #3 ���������� � ��������������� ��������� ������ ���� � ������� � ������ ���
#define INS_SGN_PROC3		97	// ����� #3 ��������� ������� ��������� ���


#define DT_NS_FID_ORG		0x5A	// �������� ������ ���� ��� (uint16_t) (2^19 of Galois)
#define DT_NS_SE_ORG		0x4B	// �������� ������ ���� �� (uint16_t) (2^20 of Galois)
#define DT_SGN_FID_ORG		0x75	// �������� ������ ��� (uint16_t) (2^21 of Galois)
#define DT_SGN_SE_ORG		0xEA	// �������� ������ �� (uint16_t) (2^22 of Galois)
#define DT_NS_FID 			0xC9	// �������� ������ ���� ��� (float) (2^23 of Galois)
#define DT_NS_SE			0x8F	// �������� ������ ���� �� (float) (2^24 of Galois)
#define DT_SGN_FID			0x03	// �������� ������ ��� (float) (2^25 of Galois)
#define DT_SGN_SE			0x06	// �������� ������ �� (float) (2^26 of Galois)
#define DT_NS_QUAD_FID_RE	0x0C	// ����������� ��������������� �������� ������ ���� ��� - �������� ����� (float) (2^27 of Galois)
#define DT_NS_QUAD_FID_IM	0x18	// ����������� ��������������� �������� ������ ���� ��� - ������ ����� (float) (2^28 of Galois)
#define DT_NS_QUAD_SE_RE	0x30	// ����������� ��������������� �������� ������ ���� �� - �������� ����� (float) (2^29 of Galois)
#define DT_NS_QUAD_SE_IM	0x60	// ����������� ��������������� �������� ������ ���� �� - ������ ����� (float) (2^30 of Galois)
#define DT_SGN_QUAD_FID_RE	0xC0	// ����������� ��������������� �������� ������ ��� - �������� ����� (float) (2^31 of Galois)
#define DT_SGN_QUAD_FID_IM	0x9D	// ����������� ��������������� �������� ������ ��� - ������ ����� (float) (2^32 of Galois)
#define DT_SGN_QUAD_SE_RE	0x27	// ����������� ��������������� �������� ������ �� - �������� ����� (float) (2^33 of Galois)
#define DT_SGN_QUAD_SE_IM	0x4E	// ����������� ��������������� �������� ������ �� - ������ ����� (float) (2^34 of Galois)
#define DT_NS_QUAD_FID		0x9C	// ����������� ��������������� �������� ������ ���� ��� (float) (2^35 of Galois)
#define DT_NS_QUAD_SE		0x25	// ����������� ��������������� �������� ������ ���� �� (float) (2^36 of Galois)
#define DT_SGN_QUAD_FID		0x4A	// ����������� ��������������� �������� ������ ��� (float) (2^37 of Galois)
#define DT_SGN_QUAD_SE		0x94	// ����������� ��������������� �������� ������ �� (2^38 of Galois)
#define DT_NS_FFT_FID		0xD4	// ������ ���� ��� ��������� ��� (float) (2^41 of Galois)
#define DT_NS_FFT_SE		0xB5	// ������ ���� ��� ��������� �� (float) (2^42 of Galois)
#define DT_SGN_FFT_FID		0x77	// ������ ������� ��� ��������� ��� (float) (2^43 of Galois)
#define DT_SGN_FFT_SE		0xEE	// ������ ������� ��� ��������� �� (float) (2^44 of Galois)
#define DT_NS_FFT_FID_RE	0xC1	// ������ ���� ��� ��������� ��� - �������� ����� (float) (2^45 of Galois)
#define DT_NS_FFT_SE_RE		0x9F	// ������ ���� ��� ��������� �� - �������� ����� (float) (2^46 of Galois)
#define DT_SGN_FFT_FID_RE	0x23	// ������ ������� ��� ��������� ��� - �������� ����� (float) (2^47 of Galois)
#define DT_SGN_FFT_SE_RE	0x46	// ������ ������� ��� ��������� �� - �������� ����� (float) (2^48 of Galois)
#define DT_NS_FFT_FID_IM	0x8C	// ������ ���� ��� ��������� ��� - ������ ����� (float) (2^49 of Galois)
#define DT_NS_FFT_SE_IM		0x05	// ������ ���� ��� ��������� �� - ������ ����� (float) (2^50 of Galois)
#define DT_SGN_FFT_FID_IM	0x0A	// ������ ������� ��� ��������� ��� - ������ ����� (float) (2^51 of Galois)
#define DT_SGN_FFT_SE_IM	0x14	// ������ ������� ��� ��������� �� - ������ ����� (float) (2^52 of Galois)
#define DT_SGN_RELAX		0x28	// ������ �� ����� ������� ��������������� ��������� ������� (float) (2^53 of Galois)
#define DT_SGN_POWER_SE		0x50	// ������ �� ������� �������� ������� �� (float) (2^54 of Galois)
#define DT_SGN_POWER_FID	0xA0	// ������ �� ������� �������� ������� ��� (float) (2^55 of Galois)
#define DT_NS_POWER_SE		0x5D	// ������ �� ������� �������� ���� �� (float) (2^56 of Galois)
#define DT_NS_POWER_FID		0xBA	// ������ �� ������� �������� ���� ��� (float) (2^57 of Galois)
#define DT_DIEL				0x69	// ������ ���������������� �������� (float) (2^58 of Galois)
#define DT_GAMMA			0xD2	// ������ �����-�������� (float) (2^59 of Galois)
#define DT_SGN_FFT_FID_AM	0x6F	// ����������� ������ ����� ������� ��� ��������� ��� (float) (2^61 of Galois)
#define DT_NS_FFT_FID_AM	0xDE	// ����������� ������ ����� ���� ��� ��������� ��� (float) (2^62 of Galois)
#define DT_SGN_FFT_SE_AM	0xA1	// ����������� ������ ����� ������� ��� ��������� �� (float) (2^63 of Galois)
#define DT_NS_FFT_SE_AM		0x5F	// ����������� ������ ����� ���� ��� ��������� �� (float) (2^64 of Galois)
#define DT_AFR1_RX			0xBE	// ��� ��������� ������� 1 (float) (2^65 of Galois)
#define DT_AFR2_RX			0x61	// ��� ��������� ������� 2 (float) (2^66 of Galois)
#define DT_AFR3_RX			0xC2	// ��� ��������� ������� 3 (float) (2^67 of Galois)
#define DT_DU				0x99	// ���������� �� ���������� DU (27*uint8_t) (2^68 of Galois)
#define DT_DU_T				0x2F	// ����������� ���������� DU (3*uint8_t) (2^69 of Galois)
#define DT_DU_I				0x5E	// ���� ���� ���������� DU (3*uint8_t) (2^70 of Galois)
#define DT_DU_U				0xBC	// ���������� ���������� DU (3*uint8_t) (2^71 of Galois)
#define DT_PU				0x65	// ���������� �� ���������� PU (27*uint8_t) (2^72 of Galois)
#define DT_PU_T				0xCA	// ����������� ���������� PU (3*uint8_t) (2^73 of Galois)
#define DT_AVER_RELAX		0x89	// ����������� ������ ������ ���������� ��������������� (2^74 of Galois)
#define DT_TU				0x0F	// ���������� �� ���������� TU (27*uint8_t) (2^75 of Galois)
#define DT_TU_T				0x1E	// ����������� ���������� TU (9*uint8_t) (2^76 of Galois)
#define DT_RFP				0x3C	// ������ �� ��������������� �������� (float) (2^77 of Galois)
#define DT_RFP2				0x6A	// ������ �� ��������������� �������� (float) (2^40 of Galois)
#define DT_DIEL_ADJUST		0xD6	// ������ ��������� ������� ���������������� �������� (2^85 of Galois)
#define DT_SGN_RELAX2		0xDF	// �������������� ������ �� ������� ������� (2^90 of Galois)
#define DT_AFR1_TX			0xA3	// ��� ����������� ������� 1 (float) (2^91 of Galois)
#define DT_AFR2_TX			0x5B	// ��� ����������� ������� 2 (float) (2^92 of Galois)
#define DT_AFR3_TX			0xB6	// ��� ����������� ������� 3 (float) (2^93 of Galois)
#define DT_SGN_RELAX3		0x71	// �������������� ������ � �������� ������� (float) (2^94 of Galois)
#define DT_SGN_SPEC1		0xE2	// ������ ������ �2 �� ����������� ����� ���������� ��������������� �������� 1 (2^95 of Galois)
#define DT_SGN_SPEC2		0xD9	// ������ ������ �2 �� ����������� ����� ���������� ��������������� �������� 2 (2^96 of Galois)
#define DT_SGN_SPEC3		0xAF	// ������ ������ �2 �� ����������� ����� ���������� ��������������� �������� 3 (2^97 of Galois)
#define DT_AVER_RELAX2		0x43	// ����������� ������ ������ ���������� ��������������� � ������� 2 (2^98 of Galois)
#define DT_AVER_RELAX3		0x86	// ����������� ������ ������ ���������� ��������������� � ������� 3 (2^99 of Galois)
#define DT_SOLID_ECHO1		0x22	// �����-��� � ������� 1 (2^101 of Galois)
#define DT_SOLID_ECHO2		0x44	// �����-��� � ������� 2 (2^102 of Galois)
#define DT_SOLID_ECHO3		0x88	// �����-��� � ������� 3 (2^103 of Galois)



typedef struct Data_Proc
{
	uint8_t *procs[MAX_PROCS]; // ������ ������ ���������� (������ MAX_PROCS)
	int proc_lens[MAX_PROCS]; // ������ ���� ������ procs
	int index[MAX_PROCS]; // ������ ������� ������� "�������" � ������ ����������
} Data_Proc;


typedef struct Data_Cmd
{
	uint8_t cmd; 				// ��� �������
	float *params; 				// ��������� �������
	int count; 					// ���������� ���������� �������
	uint8_t type; 				// ��� ������, ��� ������� ������������� �������
} Data_Cmd;


#define PARAM_DATA_LEN		16384			// ������������ ����� ������, ����������� �� ������ �� ������� ������� ���������
typedef struct CMDData
{
	uint8_t cmd; // ��� ��������
	uint8_t param_count; // ���������� ���������� �������
	uint16_t param_data_len; // ����� ��������� ������� ������ param_data
	uint8_t param_data[PARAM_DATA_LEN]; // �������� ������ ������ (��������� ������� cmd)
} CMDData;


#define NoFunc			0
#define Func_Rect		1
#define Func_Gauss		2
#define Func_BiGauss	3
typedef struct Processing_Params
{
	uint8_t echo_func; 			// ������� ������� ��� ������ ��� �� ��������� ������� (0 - ��� �������, 1 - �������������, 2 - ��������, 3 - ��-��������)
	uint8_t spectr_func; 		// ������� ������� ��� ������ ��� � ��������� ������� (0 - ��� �������, 1 - �������������, 2 - ��������, 3 - ��-��������)

	int echo_sigma; 			// ������ ������� ������� ��� ��������� ������� (� ������)
	int spectr_sigma; 			// ������ ������� ������� ��� ��������� ������� (� ������)

	int echo_x0; 				// ����� ������� ������� ��� ��������� ������� (� ������)
	int spectr_x0; 				// ����� ������� ������� ��� ��������� ������� (� ������)

	float zero_level; 			// "������� ����" ��� ������������ ������ ��� (������ ���� ����� 2048)

	int points_count; 			// ���������� ������������ "�����" (�����������) � ������� ������, �������� �� ������ UPP

	int current_echo; 			// ����� �������� ��� � ������������������ �����-��������
	int echo_count; 			// ���������� �������� ���

	int group_index;			// "��������� ������" - ����� ������� ������ ��� (������������ ��� ����������, ��������, ������ �� ������ ������������������� �����-��������, ���������� � ����� ����� ���������)
	int channel_id;				// ����� ������ ������

	uint8_t tag; 				// unused field
} Processing_Params;


void setDefaultProcParams(Processing_Params *params); // ���������� �������� (�� ���������) ��������� ��������� ������

void init_DataProc(Data_Proc *d_proc);
void clear_AllDataProc(Data_Proc *d_proc);
Bool empty_DataProc(Data_Proc *d_proc);
int count_DataProc(Data_Proc *d_proc);
Bool empty_DataProcCmd(int index, Data_Proc *d_proc);
void fill_DataProc(Data_Proc *d_proc, uint8_t *src, int len);
void free_DataProc(Data_Proc *d_proc);
void init_DataProcCmd(Data_Cmd *ptr_cmd);
void create_DataProcCmd(uint8_t cmd, uint8_t count, uint8_t type, float *params, Data_Cmd *ptr_cmd);
void clear_DataProcCmd(Data_Proc *d_proc, int index);
Bool next_DataProcCmd(int index, Data_Proc *d_proc, Data_Cmd *ptr_cmd);
void pass_DataProcCmds(int index, Data_Proc *d_proc, int count);
void move_ToFirstDataProcCmd(int index, Data_Proc *d_proc);
void free_DataProcCmd(Data_Cmd *ptr_cmd);

//inline float Q_rsqrt(float number);
float Q_rsqrt(float number);

void fill_ByValue(STACKPtrF *stack, int src_len, uint8_t byte);
void cast_UPPDataToFID_U16(uint8_t *src, int src_len, uint16_t *dst);
void cast_UPPDataToSE_U16(uint8_t *src, int src_len, uint16_t *dst);
void cast_UPPDataToFID(uint8_t *src, int src_len, float *dst);
void cast_UPPDataToFID2(uint8_t *src, Processing_Params *proc_params, float *dst);
void cast_UPPDataToSE(uint8_t *src, int src_len, float *dst);
void cast_UPPDataToSE2(uint8_t *src, Processing_Params *proc_params, float *dst);
void copy_DataTo(STACKPtrF *stack, int src_len, float *dst);
void move_FromStack(STACKPtrF *stack, int src_len, float *dst);
void define_ZeroLevel(float *src, int src_len, Processing_Params *proc_params);
void do_QuadDetect(STACKPtrF *stack);
void apply_WinFunc(int step_id, Processing_Params *proc_params, STACKPtrF *stack);
void calc_AmplSpec(STACKPtrF *stack, int src_len);
void calc_PowerSpec(STACKPtrF *stack, int src_len);
void estimate_SignalAmp1(STACKPtrF *stack, int src_len);
void estimate_MaxSpectrum1(STACKPtrF *stack, int src_len, Processing_Params *proc_params);
void do_MathOperationVal(STACKPtrF *stack, int src_len,  SummationBuffer *sum_buff, Data_Cmd *instr);
void do_MathOperationBin(STACKPtrF *stack, int src_len, Data_Cmd *instr);
void do_MathOperationXX(SummationBuffer *sum_buff, Data_Cmd *instr);
void add_ValueToXX(SummationBuffer *sum_buff, Data_Cmd *instr);
void accumulate_Data(STACKPtrF *stack, int src_len, Processing_Params *proc_params);
void accsmooth_Data(STACKPtrF *stack, int src_len, Processing_Params *proc_params, Data_Cmd *instr);
Bool decimateDataInOutputbuffer(STACKPtrF *stack, OutBuffer *out_buff, Processing_Params *proc_params, Data_Cmd *instr);
Bool summarize_Data(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Data_Cmd *instr);
Bool summarize_DataForRelax(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Processing_Params *proc_params, Data_Cmd *instr);
Bool average_Data(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Data_Cmd *instr);
//Bool move_ToNMRBuffer(STACKPtrF *stack, int src_len, float *dst, int *dst_len, Processing_Params *proc_params, Data_Cmd *instr);
Bool move_ToOutputBuffer(STACKPtrF *stack, OutBuffer *out_buff, Processing_Params *proc_params, uint8_t data_type);
Bool move_AccToOutputBuffer(STACKPtrF *stack, SummationBuffer *sum_buff, OutBuffer *out_buff, Processing_Params *proc_params);
Bool move_XXToOutBuffer(SummationBuffer *sum_buff, OutBuffer *out_buff, Processing_Params *proc_params, Data_Cmd *instr);
Bool write_ValueToSummationBuffer(SummationBuffer *sum_buff, Data_Cmd *instr);


#endif /* DATA_PROCESSING_H_ */
