/*
 * common_data.h
 *
 *  Created on: 07.04.2014
 *      Author: Andrey
 */

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_


#include <stdint.h>


#define ALLDATA_BUFF_SIZE	16384			// ����� ��������� ������ ���� ������, ������������ � ������� ������� ��������� (������ ��� + ����. ������ + ���������� +...)
#define ALLDATA_BUFF_LEN	4096			// ������ ������� ������ (�������������� data_sum � ������ data_nmr)
#define NMR_DATA_LEN		2048			// ������������ ����� ������� �������� ������ ��� (������ ���� float ��� int16_t)
#define DATA_MAX_LEN		2048			// ������������ ����� �������� �� UPP ����������� ����� � ��� (�������� ������)
#define CMPLX_DATA_MAX_LEN	DATA_MAX_LEN/2
#define UPP_BUFF_SIZE		2*sizeof(unsigned char)*DATA_MAX_LEN
#define UPP_DATA_SIZE		sizeof(float)*(DATA_MAX_LEN + 2*PAD)

#define OUTCMD_BUFF_SIZE	16				// ����� �������� ������

#define DATA_HEAP_COUNT		1000			// ���������� �������� � ���� ��� ������������� �������� ������


#define XX_LEN				4				// ����� ����� ��� �������� ������ (�� ��������)


#define DIELECTR_DATA_LEN	64				// ����� ������ ���������������� �������
#define DIELECTR_MSG_LEN	20				// ����� ������ ������ ���������������� �������
#define DIELECTR_MSG_BYTES_TO_CHECK_CRC_LEN	16 // ����� ������ ������ ���������������� ������� �� ������� ��������� CRC (� ���� ������� �����)
#define DIELECTR_MSG_BYTES_TO_CHECK_CRC_OFS	2 // ������� ������� ����� ������ �� ������� ��������� CRC (��������� ���������� � ����)
#define DIELECTR_DATA_COUNT	5				// ������� ��� ������ ��������� ��������� � ������ ����������� � ���������

#define TELEMETRIC_DATA_LEN 9				// ���������� ���� � ����� ���������
#define TELEMETRIC_UART_BUF_LEN 9+9+9		// ���������� ���������� ���������� ( ��� ������������� (x3 bytes), ��� ���� �� ��� �������� (x2 bytes), ��� ���������� �� ��� �������� (x2 bytes) )


static float ADC_to_mV = 0.3398;			// ����������� �������� �������� ��� � [��]


#define NaN					0xFFFFFFFF		// Not a Number
#define mINF				0xFFFFFFFF		// -Infinity
#define pINF				0x7FFFFFFF		// +Infinity



// Device Addresses
#define PC_MAIN             0x01        	// User's PC (����� ������� ������� ���������)
#define NMR_TOOL            0x02        	// NMR logging tool (����� ������� ���)
#define COMM_TOOL           0x03        	// Communication board
#define SDSP_TOOL			0xE0			// dielectric device (SDSP)
#define GAMMA_TOOL			0x05			// Gamma tool
#define TELEMETRY_DU_TOOL	0x06			// Telemetric board "DU"
#define DUMMY_TOOL			0xC8			// ��������� ����������



// --------------------- UART Commands -------------------------

// ����������� ������� � �����, ������������ ����������/��������� (Service Msg / Short Msg) �����������
/*#define NONE                0x00      	// No data or no command
#define GET_RELAX_DATA      0x58        	// Get only relaxation data
#define NMRTOOL_IS_READY	0x95			// NMR Tool is ready to receive data/commands from PC_Main
#define NMRTOOL_ISNOT_READY 0x1A			// NMR Tool is not ready to receive data/commands from PC_Main
#define GET_DATA			0x44			// Get actual data
#define DATA_IS_OK			0xB7			// Incoming data is OK
#define DATA_IS_FAILED		0x8B			// Incoming data is failed*/
#define NONE                0x00        	// No data or no command
#define DATA_OK				0x01			// Incoming data is OK ( 2^1 Galois)
#define DATA_FAILED			0x02			// Incoming data is failed ( 2^2 Galois)
#define NMRTOOL_IS_READY	0x04			// NMR Tool is ready to get data/commands from PC_Main (2^3 Galois)
#define NMRTOOL_ISNOT_READY 0x08			// NMR Tool is not ready to receive data/commands from PC_Main (2^4 Galois)
#define HEADER_OK			0x10			// Header of Multypacket message was received and decoded successfully
#define GET_DATA			0x74			// Get actual data (2^10 of Galois)
#define GET_RELAX_DATA      0xE8        	// Get only relaxation data	(2^11 of Galois)
#define NMRTOOL_CONNECT		0x13			// Connect to NMR Tool (2^14 of Galois)
#define NMRTOOL_START		0x26			// Stop FPGA program (2^15 of Galois)
#define NMRTOOL_STOP		0x4C			// Start FPGA program (2^16 of Galois)
#define NMRTOOL_DATA		0x35			// NMR Tool data (any) (2^39 of Galois)
#define REPEAT_CMD			0xBB			// Request to Repeat data (2^83 of Galois)
#define DIEL_ADJUST_START	0xB1			// Request to Repeat data (2^85 of Galois)
#define DIEL_DATA_READY		0xE1			// SDSP data ready to send (2^89 of Galois)
// Format of command:
// Command (1 byte)


// Send data from c6x to PC
#define ORG_NMR_SIGNAL		0x87			// �������� ������, ���������� � ��� (2^13 of Galois)
#define ECHO_DATA           0x4F
#define RELAX_DATA			0xCD			// �������� �������������� ������ (2^12 of Galois)
#define SPEC_DATA           0x5A
#define SET_WIN_PARAMS		0xB9			// Set parameters of apodizing and window functions (2^60 of Galois)
#define SDSP_DATA			0x6B			// �������� ���������� ��������� � ��������������� ������ (2^84 of Galois)
#define SDSP_REQUEST_C8		0x7F			// �������� ������� 0xC8 �� ������ � ��������������� ������ (2^87 of Galois)
#define SDSP_REQUEST_88		0xFE			// �������� ������� 0x88 �� ������ � ��������������� ������ (2^88 of Galois)
#define LOG_TOOL_SETTINGS	0x11			// �������� �������� ������� ����������� ������� (2^100 of Galois)
#define NMRTOOL_CONNECT_DEF	0x1A		// first connect to LoggingTool (2^105 of Galois)

											// ������: cmd (1 byte) -- len (2 bytes) -- func_type(time domain) -- x0(time domain) -- sigma (time domain) -- func_type(freq domain) -- x0(freq domain) -- sigma (freq domain)
// Format of commands:
// Command    Data Length (bytes)               Data
//(1 byte)   (2 bytes - uint16_t)      (up to ~56000 bytes) Note: Max. data length ~ 255 packets * (255 - recovery bytes - 4)



// send data from PC to c6x
#define DATA_PROC			0x98			// ��������� ��� ��������� ������� ��� � ��������� ������, ���������� ���������� ����������� (2^17 of Galois)
#define FPGA_PRG			0x2D			// ��������� ��������� ������ ���, ���������� ���������� ����������� (2^18 of Galois)
// Format of command:
// Command (1 byte)


#define SET_COMM_PARAMS		0x78			// ��������� ��������: ����� ������ ����-��������, ���-�� ������������ ������ � �.�.
// Format of commands:
// Command    Data Length (bytes)               Data
//(1 byte)   (2 bytes - uint16_t)      (up to ~56000 bytes) Note: Max. data length ~ 255 packets * (255 - recovery bytes - 4)



#define NUM_CLOCKERS		6				// ���������� ��������-���������



typedef enum {  							// �����, ����������������� � �������, � ������� ��������� ����������� �� UART ���������
	NOT_DEFINED,							// �������������� ��������� (�������� ���������)
	STARTED,								// ������� ����� ���������
	FINISHED,								// ����� ��������� �������� (� ��������� ������� ������������)
	FAILED,									// ��������� �������, �� �� ����� ���� ������� ������������
	PACKS_STARTED,							// ������� ����� ������� �������������� ���������
	PACKS_FINISHED,							// ���������� ����� ������ �������������� ���������
	TIMED_OUT								// ������� �����, ���������� �� ����� ���������
} MultyStateIn;

typedef enum {								// �����, ����������������� � �������, � ������� ��������� ������������ �� UART ���������
	NOT_BUILT,								// �������������� ��������� (�������� ��������)
	HEADER_SENT,							// ��������� ��������� �������������� ���������
	MESSAGE_SENT							// ��������� ����������
} MultyStateOut;


typedef enum {								// �����, ����������������� � ���������� ��������������� ��������� ������� ��� ��������� ��������� �� UART
	BUSY,									// ������ ����� (��������� ������� ���, ��������� �������)
	READY,									// ��������� ������� ��� � ��� ��������� �����������
	FREE,									// ������ ��� ����� ��������� ��������� �� UART � ���������� ������
	NOT_READY,								// ������ �������� ����� ��������� �� UART
	UNKNOWN_STATE							// ����������� ���������
} NMRToolState;


typedef enum {
	MSG_OK = 0,								// ������������� ��������� ���� ������� ������� � �������������
	MSG_NO_PACKS = 1,						// ������ �������������� ��������� �� ���������
	MSG_DATA_NOT_ALL = 2,					// ������ (������) �� ��������� � ������ ������ ��� �� �����������
	MSG_DECODE_ERR = 3,						// ��������� ������ ��� �������������� �������� �������
	MSG_BAD_PACKETS = 4,					// ������ �� ������ �������� �� crc � �.�.
	MSG_EXTRACT_ERR = 5						// ������ ��� ���������� ������ �� �������
} MultiPackMsg_Err;


typedef struct GetDataReq					// ��� ������� � �� ��������, ������������ ��� ������������� ������� �������� ��������� ������
{
	uint8_t cmd;							// �������
	uint16_t param;							// ��������
} GetDataReq;


typedef enum {								// �����, ����������������� � ���������� ������ ����������
	TELE_NOT_READY = 0,						// ������ ���������� �� ������
	TELE_READY = 1							// ������ ���������� ������
} TelemetryState;

typedef enum {								// �����, ����������������� � ���������� ������ ���� (��������������� ������)
	SDSP_NOT_READY = 0,						// ������ SDSP �� ������
	SDSP_READY = 1							// ������ SDSP ������
} SDSPState;



// ������� ��������� ��������� ��� �������� ���������� ���������.
//static unsigned char NMRTool_Ready_PowerLow[14] =	{ 170, 145, 18, 0, 4, 0, 0, 0, 120, 90,  24, 137, 58, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ��� ����������
//static unsigned char NMRTool_Ready_PowerOK[14] = 	{ 170, 145, 18, 0, 4, 1, 0, 0, 121, 109, 144, 126, 230, 85 };	// ��� �� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ��� ����������
//static unsigned char NMRTool_Ready_PowerLow_T[14] =	{ 170, 145, 18, 0, 4, 2, 0, 0, 122, 52, 21, 122, 159, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ���������� ������
//static unsigned char NMRTool_Ready_PowerOK_T[14] = 	{ 170, 145, 18, 0, 4, 3, 0, 0, 123, 3, 157, 141, 67, 85 };		// ��� �� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ���������� ������
//static unsigned char NMRTool_NotReady[14] = 		{ 170, 145, 18, 0, 8, 0, 0, 0, 116, 89, 198, 235, 229, 85 };	// ��� �� ����� ������� �������/������ �� PC_Main

// ���� ��������� ������� � 5-�� ����� ���������� ��������� � ���������� ������� � ������ �����:
// ��� 0 - ���������� �������: 							0/1 - �������� / ����������
// ��� 1 - ���������� ����������: 						0/1 - �� ������ / ������
// ��� 2 - ��������� ������������� ����: 				0/1 - ������� �������� proger_start() / ���������� �������� proger_stop()
// ��� 3 - ��������� ��������� � ������������� ����: 	0/1 - ����������� / �����������

static unsigned char NMRTool_Ready0[14] =	{ 170, 145, 18, 0, 4, 0, 0, 0, 120, 90,  24, 137, 58, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ��� ����������, ������������ �� ����������, ����� ������������������ �� ���������
static unsigned char NMRTool_Ready1[14] = 	{ 170, 145, 18, 0, 4, 1, 0, 0, 121, 109, 144, 126, 230, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ��� ����������, ������������ �� ����������, ����� ������������������ �� ���������
static unsigned char NMRTool_Ready2[14] =	{ 170, 145, 18, 0, 4, 2, 0, 0, 122, 52, 21, 122, 159, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ���������� ������, ������������ �� ����������, ����� ������������������ �� ���������
static unsigned char NMRTool_Ready3[14] = 	{ 170, 145, 18, 0, 4, 3, 0, 0, 123, 3, 157, 141, 67, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ���������� ������, ������������ �� ����������, ����� ������������������ �� ���������
static unsigned char NMRTool_Ready4[14] = 	{ 170, 145, 18, 0, 4, 4, 0, 0, 124, 134, 2, 114, 109, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ��� ����������, ������������ ������� (proger_start()), ����� ������������������ �� ���������
static unsigned char NMRTool_Ready5[14] = 	{ 170, 145, 18, 0, 4, 5, 0, 0, 125, 177, 138, 133, 177, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ��� ����������, ������������ ������� (proger_start()), ����� ������������������ �� ���������
static unsigned char NMRTool_Ready6[14] = 	{ 170, 145, 18, 0, 4, 6, 0, 0, 126, 232, 15, 129, 200, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ���������� ������, ������������ ������� (proger_start()), ����� ������������������ �� ���������
static unsigned char NMRTool_Ready7[14] = 	{ 170, 145, 18, 0, 4, 7, 0, 0, 127, 223, 135, 118, 20, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ���������� ������, ������������ ������� (proger_start()), ����� ������������������ �� ���������
static unsigned char NMRTool_Ready8[14] =	{ 170, 145, 18, 0, 4, 8, 0, 0, 112, 255, 44, 98, 148, 85 };		// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ��� ����������, ������������ �� ����������, ����� ������������������ ���������
static unsigned char NMRTool_Ready9[14] = 	{ 170, 145, 18, 0, 4, 9, 0, 0, 113, 200, 164, 149, 72, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ��� ����������, ������������ �� ����������, ����� ������������������ ���������
static unsigned char NMRTool_Ready10[14] =	{ 170, 145, 18, 0, 4, 10, 0, 0, 114, 145, 33, 145, 49, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ���������� ������, ������������ �� ����������, ����� ������������������ ���������
static unsigned char NMRTool_Ready11[14] = 	{ 170, 145, 18, 0, 4, 11, 0, 0, 115, 166, 169, 102, 237, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ���������� ������, ������������ �� ����������, ����� ������������������ ���������
static unsigned char NMRTool_Ready12[14] = 	{ 170, 145, 18, 0, 4, 12, 0, 0, 116, 35, 54, 153, 195, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ��� ����������, ������������ ������� (proger_start()), ����� ������������������ ���������
static unsigned char NMRTool_Ready13[14] = 	{ 170, 145, 18, 0, 4, 13, 0, 0, 117, 20, 190, 110, 31, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ��� ����������, ������������ ������� (proger_start()), ����� ������������������ ���������
static unsigned char NMRTool_Ready14[14] = 	{ 170, 145, 18, 0, 4, 14, 0, 0, 118, 77, 59, 106, 102, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��������, ���������� ������, ������������ ������� (proger_start()), ����� ������������������ ���������
static unsigned char NMRTool_Ready15[14] = 	{ 170, 145, 18, 0, 4, 15, 0, 0, 119, 122, 179, 157, 186, 85 };	// ��� ����� ������� �������/������ �� PC_Main, ���������� ������� ��, ���������� ������, ������������ ������� (proger_start()), ����� ������������������ ���������

static unsigned char *NMRTool_Ready[] = { &NMRTool_Ready0[0], &NMRTool_Ready1[0], &NMRTool_Ready2[0], &NMRTool_Ready3[0], &NMRTool_Ready4[0], &NMRTool_Ready5[0], &NMRTool_Ready6[0], &NMRTool_Ready7[0],
										  &NMRTool_Ready8[0], &NMRTool_Ready9[0], &NMRTool_Ready10[0], &NMRTool_Ready11[0], &NMRTool_Ready12[0], &NMRTool_Ready13[0], &NMRTool_Ready14[0], &NMRTool_Ready15[0] };


static unsigned char NMRTool_NotReady[14] = { 170, 145, 18, 0, 8, 0, 0, 0, 116, 89, 198, 235, 229, 85 };	// ��� �� ����� ������� �������/������ �� PC_Main


//static unsigned char DielTool_AdjustmentOff[8] = 	{ 0x84, 0xB2, 0x0F, 0x98, 0xB1, 0x09, 0xDE, 0xBA };				// ��� ������ �� ������ ������ ����������� ���������������� ������� � ���������� ��������� �� PC_Main



// Tool Channel types
#define NMR_CHANNEL			0x01			// ������ ������ ���
//#define NMR_CHANNEL_2		0x02			// ������ ������ ���
#define GK_CHANNEL			0x03			// �����-������
#define SDSP_CHANNEL		0x04			// ��������������� ������
#define AFR_CHANNEL			0x05			// ����������-��������� �������������� ������� ������� ���
//#define AFR_CHANNEL_2		0x06			// ����������-��������� �������������� ������� ������� ���
#define RF_PULSE_CHANNEL	0x07			// �����-��������� ������� ������� ������� ���
//#define RF_PULSE_CHANNEL_2	0x08			// �����-��������� ������� ������� ������� ���


typedef struct ToolChannel
{
	uint32_t type;							// ��� ������
	uint32_t channel_id;					// ����� ������
	uint32_t freq_set_num;
	uint32_t frq1;							// ������� #1
	uint32_t frq2;							// ������� #2
	uint32_t frq3;							// ������� #3
	uint32_t frq4;							// ������� #4
	uint32_t frq5;							// ������� #5
	uint32_t frq6;							// ������� #6
	uint32_t addr_rx;						// ����� ���������
	uint32_t addr_tx;						// ����� �����������
} ToolChannel;


#endif /* COMMON_DATA_H_ */
