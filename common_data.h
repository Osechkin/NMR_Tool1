/*
 * common_data.h
 *
 *  Created on: 07.04.2014
 *      Author: Andrey
 */

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_


#include <stdint.h>


#define ALLDATA_BUFF_SIZE	16384			// длина байтового буфера всех данных, передаваемых в рабочую станцию оператора (данные ЯМР + диэл. данные + мониторинг +...)
#define ALLDATA_BUFF_LEN	4096			// размер буферов данных (накопительного data_sum и общего data_nmr)
#define NMR_DATA_LEN		2048			// максимальная длина массива выходных данных ЯМР (данные типа float или int16_t)
#define DATA_MAX_LEN		2048			// максимальная длина принятых по UPP двухбайтных чисел с АЦП (принятый сигнал)
#define CMPLX_DATA_MAX_LEN	DATA_MAX_LEN/2
#define UPP_BUFF_SIZE		2*sizeof(unsigned char)*DATA_MAX_LEN
#define UPP_DATA_SIZE		sizeof(float)*(DATA_MAX_LEN + 2*PAD)

#define OUTCMD_BUFF_SIZE	16				// буфер выходных команд

#define DATA_HEAP_COUNT		1000			// количество массивов в куяе для долгосрочного хранения данных


#define XX_LEN				4				// буфер ячеек для числовых данных (не массивов)


#define DIELECTR_DATA_LEN	64				// длина данных диэлектрического прибора
#define DIELECTR_MSG_LEN	20				// длина данных ответа диэлектрического прибора
#define DIELECTR_MSG_BYTES_TO_CHECK_CRC_LEN	16 // длина данных ответа диэлектрического прибора по которым считается CRC (в виде простой суммы)
#define DIELECTR_MSG_BYTES_TO_CHECK_CRC_OFS	2 // позиция первого байта данных по которым считается CRC (нумерация начинается с нуля)
#define DIELECTR_DATA_COUNT	5				// сколько раз подряд проводить измерения в режиме диагностики и настройки

#define TELEMETRIC_DATA_LEN 9				// количество байт в одном измерении
#define TELEMETRIC_UART_BUF_LEN 9+9+9		// количество измеренных параметров ( три сопротивления (x3 bytes), три тока по три значения (x2 bytes), три напряжения по три значения (x2 bytes) )


static float ADC_to_mV = 0.3398;			// коэффициент перевода отсчетов АЦП в [мВ]


#define NaN					0xFFFFFFFF		// Not a Number
#define mINF				0xFFFFFFFF		// -Infinity
#define pINF				0x7FFFFFFF		// +Infinity



// Device Addresses
#define PC_MAIN             0x01        	// User's PC (адрес рабочей станции оператора)
#define NMR_TOOL            0x02        	// NMR logging tool (адрес прибора ЯМК)
#define COMM_TOOL           0x03        	// Communication board
#define SDSP_TOOL			0xE0			// dielectric device (SDSP)
#define GAMMA_TOOL			0x05			// Gamma tool
#define TELEMETRY_DU_TOOL	0x06			// Telemetric board "DU"
#define DUMMY_TOOL			0xC8			// фиктивное устройство



// --------------------- UART Commands -------------------------

// Однобайтные команды и флаги, передаваемые служебными/короткими (Service Msg / Short Msg) сообщениями
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
#define ORG_NMR_SIGNAL		0x87			// Исходные данные, полученные с АЦП (2^13 of Galois)
#define ECHO_DATA           0x4F
#define RELAX_DATA			0xCD			// Отправка релаксационных данных (2^12 of Galois)
#define SPEC_DATA           0x5A
#define SET_WIN_PARAMS		0xB9			// Set parameters of apodizing and window functions (2^60 of Galois)
#define SDSP_DATA			0x6B			// отправка параметров настройки в диелектрический прибор (2^84 of Galois)
#define SDSP_REQUEST_C8		0x7F			// отправка запроса 0xC8 на данные в диелектрический прибор (2^87 of Galois)
#define SDSP_REQUEST_88		0xFE			// отправка запроса 0x88 на данные в диелектрический прибор (2^88 of Galois)
#define LOG_TOOL_SETTINGS	0x11			// отправка настроек каналов каротажного прибора (2^100 of Galois)
#define NMRTOOL_CONNECT_DEF	0x1A		// first connect to LoggingTool (2^105 of Galois)

											// Формат: cmd (1 byte) -- len (2 bytes) -- func_type(time domain) -- x0(time domain) -- sigma (time domain) -- func_type(freq domain) -- x0(freq domain) -- sigma (freq domain)
// Format of commands:
// Command    Data Length (bytes)               Data
//(1 byte)   (2 bytes - uint16_t)      (up to ~56000 bytes) Note: Max. data length ~ 255 packets * (255 - recovery bytes - 4)



// send data from PC to c6x
#define DATA_PROC			0x98			// программа для измерения сигнала ЯМР и обработки данных, получаемых сигнальным процессором (2^17 of Galois)
#define FPGA_PRG			0x2D			// программа обработки данных АЦП, получаемых сигнальным процессором (2^18 of Galois)
// Format of command:
// Command (1 byte)


#define SET_COMM_PARAMS		0x78			// параметры передачи: длина блоков Рида-Соломона, кол-во исправляемых ошибок и т.д.
// Format of commands:
// Command    Data Length (bytes)               Data
//(1 byte)   (2 bytes - uint16_t)      (up to ~56000 bytes) Note: Max. data length ~ 255 packets * (255 - recovery bytes - 4)



#define NUM_CLOCKERS		6				// количество объектов-счетчиков



typedef enum {  							// Флаги, свидетельствующие о статусе, в котором находится принимаемое по UART сообщение
	NOT_DEFINED,							// неопределенное состояние (ожидание сообщения)
	STARTED,								// начался прием сообщения
	FINISHED,								// прием сообщения завершен (и сообщение успешно декодировано)
	FAILED,									// сообщение принято, но не может быть успешно декодировано
	PACKS_STARTED,							// начался прием пакетов многопакетного сообщения
	PACKS_FINISHED,							// закончился прием пактов многопакетного сообщения
	TIMED_OUT								// истекло время, отведенное на прием сообщения
} MultyStateIn;

typedef enum {								// Флаги, свидетельствующие о статусе, в котором находится отправляемое по UART сообщение
	NOT_BUILT,								// неопределенное состояние (ожидание отправки)
	HEADER_SENT,							// отправлен заголовок многопакетного сообщения
	MESSAGE_SENT							// сообщение отправлено
} MultyStateOut;


typedef enum {								// флаги, свидетельствующие о готовности вычислительного комплекса прибора ЯМК принимать сообщения по UART
	BUSY,									// прибор занят (измерения сигнала ЯМР, обработка сигнала)
	READY,									// измерения сигнала ЯМР и его обработка завершились
	FREE,									// прибор ЯМК готов принимать сообщения по UART и отправлять данные
	NOT_READY,								// прибор закончил прием сообщений по UART
	UNKNOWN_STATE							// неизвестное состояние
} NMRToolState;


typedef enum {
	MSG_OK = 0,								// многопакетное сообщение было успешно принято и раскодировано
	MSG_NO_PACKS = 1,						// пакеты многопакетного сообщения не поступили
	MSG_DATA_NOT_ALL = 2,					// данные (пакеты) не поступили в полном обхеме для их расшифровки
	MSG_DECODE_ERR = 3,						// фатальная ошибка при раскодировании принятых пакетов
	MSG_BAD_PACKETS = 4,					// пакеты не прошли проверку на crc и т.п.
	MSG_EXTRACT_ERR = 5						// ошибка при извлечении данных из пакетов
} MultiPackMsg_Err;


typedef struct GetDataReq					// код команды и ее параметр, определяющие тип затребованных рабочей станцией оператора данных
{
	uint8_t cmd;							// команда
	uint16_t param;							// параметр
} GetDataReq;


typedef enum {								// флаги, свидетельствующие о готовности данных телеметрии
	TELE_NOT_READY = 0,						// данные телеметрии не готовы
	TELE_READY = 1							// данные телеметрии готовы
} TelemetryState;

typedef enum {								// флаги, свидетельствующие о готовности данных СДСП (диэлектрический прибор)
	SDSP_NOT_READY = 0,						// данные SDSP не готовы
	SDSP_READY = 1							// данные SDSP готовы
} SDSPState;



// Готовые служебные сообщения для отправки компьютеру оператора.
//static unsigned char NMRTool_Ready_PowerLow[14] =	{ 170, 145, 18, 0, 4, 0, 0, 0, 120, 90,  24, 137, 58, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, нет телеметрии
//static unsigned char NMRTool_Ready_PowerOK[14] = 	{ 170, 145, 18, 0, 4, 1, 0, 0, 121, 109, 144, 126, 230, 85 };	// ЯМК не готов принять команды/данные от PC_Main, напряжение питания ОК, нет телеметрии
//static unsigned char NMRTool_Ready_PowerLow_T[14] =	{ 170, 145, 18, 0, 4, 2, 0, 0, 122, 52, 21, 122, 159, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, телеметрия готова
//static unsigned char NMRTool_Ready_PowerOK_T[14] = 	{ 170, 145, 18, 0, 4, 3, 0, 0, 123, 3, 157, 141, 67, 85 };		// ЯМК не готов принять команды/данные от PC_Main, напряжение питания ОК, телеметрия готова
//static unsigned char NMRTool_NotReady[14] = 		{ 170, 145, 18, 0, 8, 0, 0, 0, 116, 89, 198, 235, 229, 85 };	// ЯМК не готов принять команды/данные от PC_Main

// Биты состояния прибора в 5-ом байте служебного сообщения о готовности прибора к сеансу связи:
// бит 0 - напряжение питания: 							0/1 - понижено / нормальное
// бит 1 - готовность телеметрии: 						0/1 - не готова / готова
// бит 2 - состояние программатора ПЛИС: 				0/1 - запущен командой proger_start() / остановлен командой proger_stop()
// бит 3 - состояние программы в программаторе ПЛИС: 	0/1 - исполняется / завершилась

static unsigned char NMRTool_Ready0[14] =	{ 170, 145, 18, 0, 4, 0, 0, 0, 120, 90,  24, 137, 58, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, нет телеметрии, программатор не запускался, конец последовательности не достигнут
static unsigned char NMRTool_Ready1[14] = 	{ 170, 145, 18, 0, 4, 1, 0, 0, 121, 109, 144, 126, 230, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, нет телеметрии, программатор не запускался, конец последовательности не достигнут
static unsigned char NMRTool_Ready2[14] =	{ 170, 145, 18, 0, 4, 2, 0, 0, 122, 52, 21, 122, 159, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, телеметрия готова, программатор не запускался, конец последовательности не достигнут
static unsigned char NMRTool_Ready3[14] = 	{ 170, 145, 18, 0, 4, 3, 0, 0, 123, 3, 157, 141, 67, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, телеметрия готова, программатор не запускался, конец последовательности не достигнут
static unsigned char NMRTool_Ready4[14] = 	{ 170, 145, 18, 0, 4, 4, 0, 0, 124, 134, 2, 114, 109, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, нет телеметрии, программатор запущен (proger_start()), конец последовательности не достигнут
static unsigned char NMRTool_Ready5[14] = 	{ 170, 145, 18, 0, 4, 5, 0, 0, 125, 177, 138, 133, 177, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, нет телеметрии, программатор запущен (proger_start()), конец последовательности не достигнут
static unsigned char NMRTool_Ready6[14] = 	{ 170, 145, 18, 0, 4, 6, 0, 0, 126, 232, 15, 129, 200, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, телеметрия готова, программатор запущен (proger_start()), конец последовательности не достигнут
static unsigned char NMRTool_Ready7[14] = 	{ 170, 145, 18, 0, 4, 7, 0, 0, 127, 223, 135, 118, 20, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, телеметрия готова, программатор запущен (proger_start()), конец последовательности не достигнут
static unsigned char NMRTool_Ready8[14] =	{ 170, 145, 18, 0, 4, 8, 0, 0, 112, 255, 44, 98, 148, 85 };		// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, нет телеметрии, программатор не запускался, конец последовательности достигнут
static unsigned char NMRTool_Ready9[14] = 	{ 170, 145, 18, 0, 4, 9, 0, 0, 113, 200, 164, 149, 72, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, нет телеметрии, программатор не запускался, конец последовательности достигнут
static unsigned char NMRTool_Ready10[14] =	{ 170, 145, 18, 0, 4, 10, 0, 0, 114, 145, 33, 145, 49, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, телеметрия готова, программатор не запускался, конец последовательности достигнут
static unsigned char NMRTool_Ready11[14] = 	{ 170, 145, 18, 0, 4, 11, 0, 0, 115, 166, 169, 102, 237, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, телеметрия готова, программатор не запускался, конец последовательности достигнут
static unsigned char NMRTool_Ready12[14] = 	{ 170, 145, 18, 0, 4, 12, 0, 0, 116, 35, 54, 153, 195, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, нет телеметрии, программатор запущен (proger_start()), конец последовательности достигнут
static unsigned char NMRTool_Ready13[14] = 	{ 170, 145, 18, 0, 4, 13, 0, 0, 117, 20, 190, 110, 31, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, нет телеметрии, программатор запущен (proger_start()), конец последовательности достигнут
static unsigned char NMRTool_Ready14[14] = 	{ 170, 145, 18, 0, 4, 14, 0, 0, 118, 77, 59, 106, 102, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания понижено, телеметрия готова, программатор запущен (proger_start()), конец последовательности достигнут
static unsigned char NMRTool_Ready15[14] = 	{ 170, 145, 18, 0, 4, 15, 0, 0, 119, 122, 179, 157, 186, 85 };	// ЯМК готов принять команды/данные от PC_Main, напряжение питания ОК, телеметрия готова, программатор запущен (proger_start()), конец последовательности достигнут

static unsigned char *NMRTool_Ready[] = { &NMRTool_Ready0[0], &NMRTool_Ready1[0], &NMRTool_Ready2[0], &NMRTool_Ready3[0], &NMRTool_Ready4[0], &NMRTool_Ready5[0], &NMRTool_Ready6[0], &NMRTool_Ready7[0],
										  &NMRTool_Ready8[0], &NMRTool_Ready9[0], &NMRTool_Ready10[0], &NMRTool_Ready11[0], &NMRTool_Ready12[0], &NMRTool_Ready13[0], &NMRTool_Ready14[0], &NMRTool_Ready15[0] };


static unsigned char NMRTool_NotReady[14] = { 170, 145, 18, 0, 8, 0, 0, 0, 116, 89, 198, 235, 229, 85 };	// ЯМК не готов принять команды/данные от PC_Main


//static unsigned char DielTool_AdjustmentOff[8] = 	{ 0x84, 0xB2, 0x0F, 0x98, 0xB1, 0x09, 0xDE, 0xBA };				// Код выхода из режима прямой комуникации диэлектрического прибора и программой оператора на PC_Main



// Tool Channel types
#define NMR_CHANNEL			0x01			// Первый датчик ЯМР
//#define NMR_CHANNEL_2		0x02			// Второй датчик ЯМР
#define GK_CHANNEL			0x03			// Гамма-датчик
#define SDSP_CHANNEL		0x04			// Диэлектрический прибор
#define AFR_CHANNEL			0x05			// Амплитудно-частотная характеристика первого датчика ЯМР
//#define AFR_CHANNEL_2		0x06			// Амплитудно-частотная характеристика второго датчика ЯМР
#define RF_PULSE_CHANNEL	0x07			// Радио-частотный импульс первого датчика ЯМР
//#define RF_PULSE_CHANNEL_2	0x08			// Радио-частотный импульс второго датчика ЯМР


typedef struct ToolChannel
{
	uint32_t type;							// тип канала
	uint32_t channel_id;					// номер канала
	uint32_t freq_set_num;
	uint32_t frq1;							// частота #1
	uint32_t frq2;							// частота #2
	uint32_t frq3;							// частота #3
	uint32_t frq4;							// частота #4
	uint32_t frq5;							// частота #5
	uint32_t frq6;							// частота #6
	uint32_t addr_rx;						// адрес приемника
	uint32_t addr_tx;						// адрес передатчика
} ToolChannel;


#endif /* COMMON_DATA_H_ */
