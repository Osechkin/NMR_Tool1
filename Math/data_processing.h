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


typedef struct OutBuffer // контейнер для выходных данных
{
	float *out_data; 						// буфер для выходных данных
	int outdata_counter; 					// счетчик массивов данных в выходном буфере
	uint8_t data_id[OUTCMD_BUFF_SIZE]; 		// массив типов данных (исходные данные с АЦП, квадратурно-детектрованные исходные данные и т.д.) в выходном буфере
	int group_index[OUTCMD_BUFF_SIZE];		// массив номеров групп данных
	int outdata_len[OUTCMD_BUFF_SIZE]; 		// массив длин массивов данных в выходном буфере
	uint8_t channel_id[OUTCMD_BUFF_SIZE];	// массив номеров каналов данных (датчик ЯМР1, датчик ЯМР2 и т.д.)
	int full_size; 							// общая длина данных в буфере
} OutBuffer;

void OutBuffer_Init(OutBuffer *out_buff, float *data);
void OutBuffer_AddData(OutBuffer *out_buff, float *src, int src_len, uint8_t src_data_id, int src_gr_index);
void OutBuffer_ClearAll(OutBuffer *out_buff);


typedef struct SummationBuffer // контейнер для поточечного накопления данных (например, интегралы от эхо)
{
	float *sum_data; 	// данные
	float *xx;			// указатель на массив ячеек XX (XX[0..3] = X0, X1, X2 и X3)
	int max_size; 		// число выделенных элементов типа float под переменную sum_data
	int pos; 			// текущая позиция в массиве sum_data
	int group_index;	// номер группы данных (например, номер текущего массива данных измерений по Карру-Парселлу)
	uint8_t data_id; 	// тип данных
	uint8_t channel_id;	// номер канала данных (датчик ЯМР1, датчик ЯМР2 и т.д.)
} SummationBuffer;

void SummationBuffer_Init(SummationBuffer *sum_buf, float *data, int max_len, float *xx);
void SummationBuffer_ClearAll(SummationBuffer *sum_buf);


#define	MAX_PROCS			32
#define MAX_PROC_LEN		1024


#define MAX_TIMINGS			200
typedef struct TimingData
{
	unsigned int 	time[MAX_TIMINGS];					// отсчеты времени в тактах процессора (1 такт = 1/300000000 сек )
	int 			hard_echo_counter[MAX_TIMINGS];		// значения hard_echo_counter (номер текущего эхо из аппаратного счетчика GPIO2)
	int 			device_id[MAX_TIMINGS];				// id устройства, с которым в данный момент идет работа (см. NMR_TOOL, GAMMA_TOOL и т.д.)
	int 			pack_id[MAX_TIMINGS];				// номер пакета инструкций, выполняющегося в данный момент (передается через GPIO5-12)
	int 			count;								// текущая позиция в массивах time, hard_echo_counter, device_id, pack_id
	int 			target_loop;						// номер "цикла" измерений, на котором необходимо распечатать результаты
	int				loop;								// номер текущего "цикла" измерений
} TimingData;

void TimingData_ClearAll(TimingData *tim_data);
void TimingData_Clear(TimingData *tim_data);
void TimingData_Print(TimingData *tim_data);


#define MAX_TIMING_BUFF_LEN			4096
typedef struct TimingProc_Buffer
{
	uint32_t time[MAX_TIMING_BUFF_LEN];			// момент времени (мкс)
	uint32_t proc_id[MAX_TIMING_BUFF_LEN];		// код операции, выполняемой в момент времени time
	uint16_t len;								// длина буфера
	uint64_t tsc_init;							// исходное значение отсчетов DSP CPU (TSCH | TSCL)
} TimingProc_Buffer;
// proc_id:
// |-- H.byte 4 --|--- byte 3 ---|--- byte 2 ---|-- 1 byte 1 --|
// H.byte 4 (highest byte) - не используется
// byte 3 - id устройства, с которым в данный момент идет работа (см. NMR_TOOL, GAMMA_TOOL и т.д.)
// byte 2 - hard_echo_counter
// L.byte 1 (lowest byte) - GPIO 1-3 state: 4 h.bits - GPIO status, 4 l.bits - GPIO number (1,2 or 3)
//
// out_cond:
// 0 - вывод накопленных данных в случае достижения эхо с требуемым номером
// 1 - если по достижении GPIO3 число эхо не превышает max_echo

void TimingProc_Buffer_Init(TimingProc_Buffer *timproc_buff, uint32_t tsch, uint32_t tscl);
Bool TimingProc_Buffer_Add(TimingProc_Buffer *timproc_buff, uint16_t prid, uint32_t tsch, uint32_t tscl);
uint32_t TimingProc_Buffer_ProcId_At(TimingProc_Buffer *timproc_buff, uint16_t index);
uint32_t TimingProc_Buffer_Time_At(TimingProc_Buffer *timproc_buff, uint16_t index);
uint16_t TimingProc_Buffer_Count(TimingProc_Buffer *timproc_buff);
void TimingProc_Buffer_Print1(TimingProc_Buffer *timproc_buff, uint16_t echo_index);	// echo_index - номер "цикла" измерений, на котором необходимо распечатать результаты
void TimingProc_Buffer_Print2(TimingProc_Buffer *timproc_buff);							// безусловный вывод результатов



typedef unsigned short Bool;
#define False           	0
#define True            	1


// коды поэлементных операций над массивами для команды INS_DO_OPER
#define OPER_ADD			1	// код операции над данными - сложение
#define OPER_SUB			2	// код операции над данными - вычитание
#define OPER_MUL			3	// код операции над данными - умножеине
#define OPER_VD1			4	// обработка В.Д. №1
#define OPER_DIV			5	// код операции над данными - деление


// коды приемника результата обработки данных командами INS_DO_XX_OPER, INS_ADD_TO_XX
#define GR_SUM_BUFF			1	// group index в буфере типа SummationBuffer
#define X0					2	// ячейка X0
#define X1					3	// ячейка X1
#define X2					4	// ячейка X2
#define X3					5	// ячейка X3


// инструкции для обработки данных сигнальным процессором
#define INS_NO_OP			4	// "пустая" инструкция ("нет операции")
#define INS_IF_DATA_NUM		5	// выполнение заданного числа инструкций, если выполняется условие равенства номера массива данных (эхо, ССИ) звдвному, иначе - пропуск этих инструкций
//#define INS_OPER_FID		8	// Приведение массива спада СЭ в upp_buffer к DATA_MAX_LEN двухбайтных чисел. Результат сохраняется в i16_buffer
//#define INS_OPER_SE		9	// Приведение массива спада ССИ в upp_buffer к DATA_MAX_LEN двухбайтных чисел. Результат сохраняется в i16_buffer

#define INS_OPER_FID_ORG	6	// Приведение массива как спада ССИ и запись его в буфер ui16_buffer
#define INS_OPER_SE_ORG		7	// Приведение массива как спада СЭ и запись его в буфер ui16_buffer
#define INS_OPER_FID		8	// Приведение массива как спада ССИ
#define INS_OPER_SE			9	// Приведение массива как спада СЭ
#define INS_QD_FID			10	// Квадратурное детектирование приведённого сигнала ССИ
#define INS_QD_SE			11	// Квадратурное детектирование приведённого сигнала СЭ
#define INS_WIN_TIME		12	// Задание параметров оконной функции для данных во временном представлении
#define INS_WIN_FREQ		13	// Задание параметров оконной функции для данных в частотном представлении
#define INS_OPER_FID_D		14	// Приведение массива как спада ССИ с расширением данных в 2 раза и записью в нечетные позиции числа 2048 (Re = отсчет АЦП, Im = 2048)
#define INS_OPER_SE_D		15	// Приведение массива как спада СЭ с расширением данных в 2 раза и записью в нечетные позиции числа 2048 (Re = отсчет АЦП, Im = 2048)
#define INS_MV_ST_OUTBUF	16	// Перенос массива данных с вершины стека в выходной буфер. Тип данных определяется третьим байтом команды (см. формат команды)
#define INS_WR_XX_SUMBUF	17	// скопировать число из ячейки XX (XX = X0, X1, X2 или X3) в буфер суммирования
#define INS_IF_COND			19	// Выполнение инструкций после данной команды, если параметры инструкции равны, иначе - переход на заданное число инструкций (см. формат команды)
#define INS_WR_X0			20	// Запись числа типа float в ячейку X0
#define INS_WR_X1			21	// Запись числа типа float в ячейку X1
#define INS_WR_X2			22	// Запись числа типа float в ячейку X2
#define INS_WR_X3			23	// Запись числа типа float в ячейку X3
#define INS_IF_COND_X0		24	// Выполнение инструкций после данной команды, если X0 удовлетворяет заданному условию, иначе - пропуск заданного количества инструкций (см. формат команды)
#define INS_IF_COND_X1		25	// Выполнение инструкций после данной команды, если X1 удовлетворяет заданному условию, иначе - пропуск заданного количества инструкций (см. формат команды)
#define INS_IF_COND_X2		26	// Выполнение инструкций после данной команды, если X2 удовлетворяет заданному условию, иначе - пропуск заданного количества инструкций (см. формат команды)
#define INS_IF_COND_X3		27	// Выполнение инструкций после данной команды, если X3 удовлетворяет заданному условию, иначе - пропуск заданного количества инструкций (см. формат команды)
#define INS_APP_WIN_TIME	28	// Применение оконной функции для временной области к данным в стеке
#define INS_APP_WIN_FREQ	29	// Применение оконной функции для частотной области к данным в стеке

#define INS_DO_XX_OPER		30	// Математические операции над ячейкой XX
#define INS_ADD_TO_XX		31	// Добавить число к значению ячейки XX
#define INS_WR_ACC_GRIX		32	// запись числа типа int32_t в поле group_index накопительного буффера

#define INS_CL_ST			34	// очистка стека
#define INS_ZERO_ST			35	// Заполнить нулями буфер, находящийся на вершине стека
#define INS_NAN_ST			36	// Заполнить числом NaN (0xFFFFFFFF) буфер, находящийся на вершине стека
#define INS_WR_D0_ST		37	// Помещение данных из буфера data_org (D0) в стек данных
#define INS_WR_D1_ST		38	// Помещение данных из буфера data1 (D1) в стек данных
#define INS_WR_D2_ST		39	// Помещение данных из буфера data2 (D2) в стек данных
#define INS_WR_D3_ST		40	// Помещение данных из буфера data3 (D3) в стек данных
#define INS_WR_DX_ST		56	// Помещение данных из буфера dataX (DX = D0 - D7) в стек данных
#define INS_CP_ST_D0		41	// Копирование данных из стека данных в буфер data_org (D0) c сохранением стека
#define INS_CP_ST_D1		42	// Копирование данных из стека данных в буфер data1 (D1) c сохранением стека
#define INS_CP_ST_D2		43	// Копирование данных из стека данных в буфер data2 (D2) c сохранением стека
#define INS_CP_ST_D3		44	// Копирование данных из стека данных в буфер data3 (D3) c сохранением стека
#define INS_CP_ST_DX		55	// Копирование данных из стека данных в буфер dataX (DX = D0 - D7) c сохранением стека
#define INS_MV_ST_D0		45	// Перенос данных из стека данных в буфер data_org (D0)
#define INS_MV_ST_D1		46	// Перенос данных из стека данных в буфер data1 (D1)
#define INS_MV_ST_D2		47	// Перенос данных из стека данных в буфер data2 (D2)
#define INS_MV_ST_D3		51	// Перенос данных из стека данных в буфер data3 (D3)
#define INS_MV_ST_DX		54	// Перенос данных из стека данных в буфер dataX (DX = D0 - D7)
#define INS_ST_SWAP			52	// перестановка в стеке элемента на вершине стека с последующим элементом
#define INS_FFT				48	// Быстрое Фурье-преобразование
#define INS_FPW				49	// Вычисление спектра мощности
#define INS_FAMPL			53	// Вычисление амплитудного спектра

#define INS_GO_TO			50	// Безусловный переход на заданное количество инструкций

#define INS_WR_HX_ST		57	// Помещение данных из контейнера data_heapX (data_heap = H0 - H127) в стек данных
#define INS_CP_ST_HX		58	// Копирование данных из стека данных в контейнер data_heapX (data_heap = H0 - H127) c сохранением стека
#define INS_MV_ST_HX		59	// Перенос данных из стека данных в контейнер data_heapX (data_heap = H0 - H127)

#define INS_GET_GAMMA		60	// Измерить показания гамма-датчика и положить результат в одну из ячеек XX. Код ячейкиXX - параметр инструкции

#define INS_START_SDSP		61	// Запустить измерения SDSP
#define INS_GET_SDSP		62	// Получить измерения SDSP и положить результат в выходной буфер

#define INS_XX_TO_OUTBUF	63	// Перенос числа из ячейки XX в выходной буффер. Код ячейки - параметр инструкции, тип данных, содержащихся в XX, - третий байт инструкции
#define INS_ACC_TO_OUTBUF	64	// Перенос данных из накопительного буффера в выходной буффер
#define INS_DO_OPER1		65	// Выполнение поэлементной бинарной операции над буфером в стеке и числом в ячейке XX. Код операции и код ячейкиXX - параметры инструкции
#define INS_DO_OPER2		66	// Выполнение поэлементной бинарной операции над двумя буферами в стеке. Код операции - параметр инструкции
#define INS_ACC_DAT			67	// Накопление данных в буфере
#define INS_SUM_DAT			68  // Сложить все элементы массива
#define INS_SUM_REL_DAT		69  // Сложить все элементы массива для записи в выходной буфер релаксационного спада (отличие от предыдущей инструкции в порядке расположения данных)
#define INS_AMP1			70	// Оценка амплитуды сигнала по мощностному спектру
#define INS_SPEC_MAX		71	// Оценка положения максимума спектра и применение найденного значения
#define INS_ST_AVER			72	// Усреднить массив в стеке данных и результат поместить в одну из ячеек X0, X1, X2 или X3
#define INS_SMOOTH_DAT		73	// Накопление данных в буфере с экспонециальным сглаживанием
#define INS_CL_SUMBUFF		74	// Очистить буфер релаксационного спада

#define INS_STOP_SDSP		75	// выключить прибор SDSP (выключить питание, выключить передатчик и т.д.)

#define INS_EMUL_SE_NS		76	// Эмулировать спиновое эхо + шум с заданным отношением "сигнал/шум" и заместить им данные в буфере UPP (upp_buffer)
#define INS_EMUL_FID_NS		77	// Эмулировать ССИ + шум с заданным отношением "сигнал/шум" и заместить им данные в буфере UPP (upp_buffer)
#define INS_EMUL_NS			78	// Эмулировать шум и заместить им данные в буфере UPP (upp_buffer)
#define INS_EMUL_FID		79	// Эмулировать сигнал ССИ и заместить им данные в буфере UPP (upp_buffer)
#define INS_EMUL_SE			80	// Эмулировать сигнал спинового эхо и заместить им данные в буфере UPP (upp_buffer)

#define INS_START_SDSP_ADC	81	// запустить АЦП диэлектрического прибора SDSP
#define INS_GET_SDSP_ADC	82	// считать даные с АЦП диэлектрического прибора SDSP

#define INS_ST_DEC_OUTBUF	83	// проредить массив в стеке и отправить на выход
#define INS_ST_QUAD			84	// получить квадратуру временнОго сигнала

#define INS_CLEAR_HX		89	// Очистка массивов контейнера data_heapX (data_heap = H0 - H127)
#define INS_NOISE_PROC1		90	// Метод #1 обработки данных шума
#define INS_SGN_PROC1		91	// Метод #1 обработки сигнала спинового эхо
#define INS_NOISE_UPP_PRE1	92	// Метод #1 накопления и предварительной обработки данных шума в буфере АЦП
#define INS_SGN_UPP_PRE1	93	// Метод #1 накопления и предварительной обработки данных сигнала в буфере АЦП
#define INS_NOISE_PROC2		94	// Метод #2 обработки данных шума
#define INS_SGN_PROC2		95	// Метод #2 обработки сигнала спинового эхо
#define INS_NS_SGN_UPP_PRE3	96	// Метод #3 накопления и предварительной обработки данных шума и сигнала в буфере АЦП
#define INS_SGN_PROC3		97	// Метод #3 обработки сигнала спинового эхо


#define DT_NS_FID_ORG		0x5A	// исходный сигнал шума ССИ (uint16_t) (2^19 of Galois)
#define DT_NS_SE_ORG		0x4B	// исходный сигнал шума СЭ (uint16_t) (2^20 of Galois)
#define DT_SGN_FID_ORG		0x75	// исходный сигнал ССИ (uint16_t) (2^21 of Galois)
#define DT_SGN_SE_ORG		0xEA	// исходный сигнал СЭ (uint16_t) (2^22 of Galois)
#define DT_NS_FID 			0xC9	// исходный сигнал шума ССИ (float) (2^23 of Galois)
#define DT_NS_SE			0x8F	// исходный сигнал шума СЭ (float) (2^24 of Galois)
#define DT_SGN_FID			0x03	// исходный сигнал ССИ (float) (2^25 of Galois)
#define DT_SGN_SE			0x06	// исходный сигнал СЭ (float) (2^26 of Galois)
#define DT_NS_QUAD_FID_RE	0x0C	// квадратурно детектированный исходный сигнал шума ССИ - реальная часть (float) (2^27 of Galois)
#define DT_NS_QUAD_FID_IM	0x18	// квадратурно детектированный исходный сигнал шума ССИ - мнимая часть (float) (2^28 of Galois)
#define DT_NS_QUAD_SE_RE	0x30	// квадратурно детектированный исходный сигнал шума СЭ - реальная часть (float) (2^29 of Galois)
#define DT_NS_QUAD_SE_IM	0x60	// квадратурно детектированный исходный сигнал шума СЭ - мнимая часть (float) (2^30 of Galois)
#define DT_SGN_QUAD_FID_RE	0xC0	// квадратурно детектированный исходный сигнал ССИ - реальная часть (float) (2^31 of Galois)
#define DT_SGN_QUAD_FID_IM	0x9D	// квадратурно детектированный исходный сигнал ССИ - мнимая часть (float) (2^32 of Galois)
#define DT_SGN_QUAD_SE_RE	0x27	// квадратурно детектированный исходный сигнал СЭ - реальная часть (float) (2^33 of Galois)
#define DT_SGN_QUAD_SE_IM	0x4E	// квадратурно детектированный исходный сигнал СЭ - мнимая часть (float) (2^34 of Galois)
#define DT_NS_QUAD_FID		0x9C	// квадратурно детектированный исходный сигнал шума ССИ (float) (2^35 of Galois)
#define DT_NS_QUAD_SE		0x25	// квадратурно детектированный исходный сигнал шума СЭ (float) (2^36 of Galois)
#define DT_SGN_QUAD_FID		0x4A	// квадратурно детектированный исходный сигнал ССИ (float) (2^37 of Galois)
#define DT_SGN_QUAD_SE		0x94	// квадратурно детектированный исходный сигнал СЭ (2^38 of Galois)
#define DT_NS_FFT_FID		0xD4	// спектр шума для интервала ССИ (float) (2^41 of Galois)
#define DT_NS_FFT_SE		0xB5	// спектр шума для интервала СЭ (float) (2^42 of Galois)
#define DT_SGN_FFT_FID		0x77	// спектр сигнала для интервала ССИ (float) (2^43 of Galois)
#define DT_SGN_FFT_SE		0xEE	// спектр сигнала для интервала СЭ (float) (2^44 of Galois)
#define DT_NS_FFT_FID_RE	0xC1	// спектр шума для интервала ССИ - реальная часть (float) (2^45 of Galois)
#define DT_NS_FFT_SE_RE		0x9F	// спектр шума для интервала СЭ - реальная часть (float) (2^46 of Galois)
#define DT_SGN_FFT_FID_RE	0x23	// спектр сигнала для интервала ССИ - реальная часть (float) (2^47 of Galois)
#define DT_SGN_FFT_SE_RE	0x46	// спектр сигнала для интервала СЭ - реальная часть (float) (2^48 of Galois)
#define DT_NS_FFT_FID_IM	0x8C	// спектр шума для интервала ССИ - мнимая часть (float) (2^49 of Galois)
#define DT_NS_FFT_SE_IM		0x05	// спектр шума для интервала СЭ - мнимая часть (float) (2^50 of Galois)
#define DT_SGN_FFT_FID_IM	0x0A	// спектр сигнала для интервала ССИ - мнимая часть (float) (2^51 of Galois)
#define DT_SGN_FFT_SE_IM	0x14	// спектр сигнала для интервала СЭ - мнимая часть (float) (2^52 of Galois)
#define DT_SGN_RELAX		0x28	// данные по спаду ядерной намагниченности полезного сигнала (float) (2^53 of Galois)
#define DT_SGN_POWER_SE		0x50	// данные по спектру мощности сигнала СЭ (float) (2^54 of Galois)
#define DT_SGN_POWER_FID	0xA0	// данные по спектру мощности сигнала ССИ (float) (2^55 of Galois)
#define DT_NS_POWER_SE		0x5D	// данные по спектру мощности шума СЭ (float) (2^56 of Galois)
#define DT_NS_POWER_FID		0xBA	// данные по спектру мощности шума ССИ (float) (2^57 of Galois)
#define DT_DIEL				0x69	// данные диэлектрического каротажа (float) (2^58 of Galois)
#define DT_GAMMA			0xD2	// данные Гамма-каротажа (float) (2^59 of Galois)
#define DT_SGN_FFT_FID_AM	0x6F	// амплитудный спектр Фурье сигнала для интервала ССИ (float) (2^61 of Galois)
#define DT_NS_FFT_FID_AM	0xDE	// амплитудный спектр Фурье шума для интервала ССИ (float) (2^62 of Galois)
#define DT_SGN_FFT_SE_AM	0xA1	// амплитудный спектр Фурье сигнала для интервала СЭ (float) (2^63 of Galois)
#define DT_NS_FFT_SE_AM		0x5F	// амплитудный спектр Фурье шума для интервала СЭ (float) (2^64 of Galois)
#define DT_AFR1_RX			0xBE	// АЧХ приемного контура 1 (float) (2^65 of Galois)
#define DT_AFR2_RX			0x61	// АЧХ приемного контура 2 (float) (2^66 of Galois)
#define DT_AFR3_RX			0xC2	// АЧХ приемного контура 3 (float) (2^67 of Galois)
#define DT_DU				0x99	// Телеметрия от устройства DU (27*uint8_t) (2^68 of Galois)
#define DT_DU_T				0x2F	// Температуры устройства DU (3*uint8_t) (2^69 of Galois)
#define DT_DU_I				0x5E	// Силы тока устройства DU (3*uint8_t) (2^70 of Galois)
#define DT_DU_U				0xBC	// Напряжения устройства DU (3*uint8_t) (2^71 of Galois)
#define DT_PU				0x65	// Телеметрия от устройства PU (27*uint8_t) (2^72 of Galois)
#define DT_PU_T				0xCA	// Температуры устройства PU (3*uint8_t) (2^73 of Galois)
#define DT_AVER_RELAX		0x89	// усредненные данные спадов поперечной намагниченности (2^74 of Galois)
#define DT_TU				0x0F	// Телеметрия от устройства TU (27*uint8_t) (2^75 of Galois)
#define DT_TU_T				0x1E	// Температуры устройства TU (9*uint8_t) (2^76 of Galois)
#define DT_RFP				0x3C	// данные по радиочастотному импульсу (float) (2^77 of Galois)
#define DT_RFP2				0x6A	// Данные по радиочастотному импульсу (float) (2^40 of Galois)
#define DT_DIEL_ADJUST		0xD6	// данные настройки прибора диэлектрического каротажа (2^85 of Galois)
#define DT_SGN_RELAX2		0xDF	// релаксационные данные со второго датчика (2^90 of Galois)
#define DT_AFR1_TX			0xA3	// АЧХ передающего контура 1 (float) (2^91 of Galois)
#define DT_AFR2_TX			0x5B	// АЧХ передающего контура 2 (float) (2^92 of Galois)
#define DT_AFR3_TX			0xB6	// АЧХ передающего контура 3 (float) (2^93 of Galois)
#define DT_SGN_RELAX3		0x71	// релаксационные данные с третьего датчика (float) (2^94 of Galois)
#define DT_SGN_SPEC1		0xE2	// спектр времен Т2 от измеренного спада поперечной намагниченности датчиком 1 (2^95 of Galois)
#define DT_SGN_SPEC2		0xD9	// спектр времен Т2 от измеренного спада поперечной намагниченности датчиком 2 (2^96 of Galois)
#define DT_SGN_SPEC3		0xAF	// спектр времен Т2 от измеренного спада поперечной намагниченности датчиком 3 (2^97 of Galois)
#define DT_AVER_RELAX2		0x43	// усредненные данные спадов поперечной намагниченности с датчика 2 (2^98 of Galois)
#define DT_AVER_RELAX3		0x86	// усредненные данные спадов поперечной намагниченности с датчика 3 (2^99 of Galois)
#define DT_SOLID_ECHO1		0x22	// солид-эхо с датчика 1 (2^101 of Galois)
#define DT_SOLID_ECHO2		0x44	// солид-эхо с датчика 2 (2^102 of Galois)
#define DT_SOLID_ECHO3		0x88	// солид-эхо с датчика 3 (2^103 of Galois)



typedef struct Data_Proc
{
	uint8_t *procs[MAX_PROCS]; // массив блоков инструкций (длиной MAX_PROCS)
	int proc_lens[MAX_PROCS]; // массив длин блоков procs
	int index[MAX_PROCS]; // массив текущих позиций "курсора" в блоках инструкций
} Data_Proc;


typedef struct Data_Cmd
{
	uint8_t cmd; 				// код команды
	float *params; 				// параметры команды
	int count; 					// количество параметров команды
	uint8_t type; 				// тип данных, для которых предназначена команда
} Data_Cmd;


#define PARAM_DATA_LEN		16384			// максимальная длина данных, принимаемых по кабелю от рабочей станции оператора
typedef struct CMDData
{
	uint8_t cmd; // код комманды
	uint8_t param_count; // количество параметров команды
	uint16_t param_data_len; // длина принятого массива данных param_data
	uint8_t param_data[PARAM_DATA_LEN]; // принятый массив данных (параметры команды cmd)
} CMDData;


#define NoFunc			0
#define Func_Rect		1
#define Func_Gauss		2
#define Func_BiGauss	3
typedef struct Processing_Params
{
	uint8_t echo_func; 			// оконная функция для данных АЦП во временной области (0 - нет функции, 1 - прямоугольная, 2 - Гауссова, 3 - би-Гауссова)
	uint8_t spectr_func; 		// оконная функция для данных АЦП в частотной области (0 - нет функции, 1 - прямоугольная, 2 - Гауссова, 3 - би-Гауссова)

	int echo_sigma; 			// ширина оконной функции для временной области (в точках)
	int spectr_sigma; 			// ширина оконной функции для частотной области (в точках)

	int echo_x0; 				// центр оконной функции для временной области (в точках)
	int spectr_x0; 				// центр оконной функции для частотной области (в точках)

	float zero_level; 			// "уровень нуля" для оцифрованных данных АЦП (должно быть около 2048)

	int points_count; 			// количество оцифрованных "точек" (двухбайтных) в массиве данных, принятом по каналу UPP

	int current_echo; 			// номер текущего эхо в последователньости Карра-Парселла
	int echo_count; 			// количество принятых эхо

	int group_index;			// "групповой индекс" - номер текущей группы эхо (используется для различения, например, данных из разных последовательностей Карра-Парселла, измеренных в одном цикле измерений)
	int channel_id;				// номер канала данных

	uint8_t tag; 				// unused field
} Processing_Params;


void setDefaultProcParams(Processing_Params *params); // установить исходные (по умолчанию) параметры обработки данных

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
