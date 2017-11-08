/*
 * data_processing.c
 *
 *  Created on: 20.05.2015
 *      Author: Osechkin
 */


#include "../Common/OMAPL138_global.h"
#include "../common_data.h"
#include "data_processing.h"



//fast sqrt(...)
//inline float Q_rsqrt( float number )
float Q_rsqrt( float number )
{
	long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    long ii = *(long*)&y;
    ii &= 0x7fffffff;
    y = *(float*)&ii;

    return y*number*1.00168579158972f;			// 1,0016857915897235564467551425010197 is number to correct systematic error
}



void OutBuffer_Init(OutBuffer *out_buff, float *data)
{
	out_buff->out_data = data;
	OutBuffer_ClearAll(out_buff);
}

void OutBuffer_AddData(OutBuffer *out_buff, float *src, int src_len, uint8_t src_data_id, int src_gr_index)
{
	int pos = out_buff->full_size;
	memcpy(out_buff->out_data + pos, src, src_len*sizeof(float));
	out_buff->full_size += src_len;

	int data_cnt = out_buff->outdata_counter;
	out_buff->data_id[data_cnt] = src_data_id;
	out_buff->outdata_len[data_cnt] = src_len;
	out_buff->group_index[data_cnt] = src_gr_index;
	out_buff->outdata_counter++;
}

void OutBuffer_ClearAll(OutBuffer *out_buff)
{
	out_buff->outdata_counter = 0;
	memset(&out_buff->data_id[0], 0x00, OUTCMD_BUFF_SIZE*sizeof(uint8_t));
	memset(&out_buff->outdata_len[0], 0x00, OUTCMD_BUFF_SIZE*sizeof(int));
	memset(&out_buff->group_index[0], 0x00, OUTCMD_BUFF_SIZE*sizeof(int));
	out_buff->full_size = 0;
}


void SummationBuffer_Init(SummationBuffer *sum_buf, float *data, int max_len, float *xx)
{
	sum_buf->sum_data = data;
	sum_buf->xx = xx;
	sum_buf->max_size = max_len;
	sum_buf->pos = 0;
	sum_buf->data_id = 0;
	sum_buf->group_index = 0;
}

void SummationBuffer_ClearAll(SummationBuffer *sum_buf)
{
	memset(sum_buf->sum_data, 0xFF, sum_buf->max_size*sizeof(float));
	sum_buf->pos = 0;
	sum_buf->data_id = 0;
	sum_buf->group_index = 0;
	memset(sum_buf->xx, 0x00, XX_LEN*sizeof(float));
}


void init_DataProc(Data_Proc *d_proc)
{
    int i;
    //uint8_t *p = (uint8_t*)malloc(MAX_PROCS*MAX_PROC_LEN*sizeof(uint8_t));
	for (i = 0; i < MAX_PROCS; i++)
	{
        d_proc->procs[i] = (uint8_t*)calloc(MAX_PROC_LEN, sizeof(uint8_t));
		//d_proc->procs[i] = p + i*MAX_PROC_LEN;
		d_proc->proc_lens[i] = 0;
		d_proc->index[i] = 0;
	}
}

void clear_AllDataProc(Data_Proc *d_proc)
{
	int i = 0;
	for (i = 0; i < MAX_PROCS; i++)
	{
		memset(d_proc->procs[i], 0x0, MAX_PROC_LEN);
		d_proc->proc_lens[i] = 0;
		d_proc->index[i] = 0;
	}
}

Bool empty_DataProc(Data_Proc *d_proc)
{
	int i;
	for (i = 0; i < MAX_PROCS; i++)
	{
        if (d_proc->proc_lens[i] > 0) return False;
	}

    return True;
}

int count_DataProc(Data_Proc *d_proc)
{
	int res = 0;
	int i;
	for (i = 0; i < MAX_PROCS; i++)
	{
		if (d_proc->proc_lens[i] > 0) res++;
	}

	return res;
}


Bool empty_DataProcCmd(int index, Data_Proc *d_proc)
{
    return (d_proc->proc_lens[index] == 0 ? True : False);
}

void fill_DataProc(Data_Proc *d_proc, uint8_t *src, int len)
{
	if (src[0] == 0x00 || src[0] == 0xFF) return;

	uint8_t tmp[1000];
	memcpy(&tmp[0], src, len);

	int index = 0;
	int loc_index = 0;							// счетчик байт внутри блока инструкций procs[...]
	int proc_num = 0;							// номер текущего блока инструкций procs[...]
	int fin = 1;								// индикатор того, что данные блока инструкций закончились

	while (index+4 <= len)
	{
		uint8_t byte = src[index];
		if (byte == 0x00) return;
		else if (byte != 0xFF)					// разделитель между блоками команд или конец всех данных не достигнут
		{
			if (fin > 0)
			{
				index++;
                proc_num = byte-1;
				fin = 0;
			}
			int loc_len = 0;					// длина данных текущей команды cmd
			uint8_t cmd = src[index];			// команда
			uint8_t param_cnt = src[index+1];	// число параметрров команды cmd
			uint8_t type = src[index+2];		// тип данных, над которыми выполняется cmd
			loc_len = 4*param_cnt + 3;

			uint8_t *dst = d_proc->procs[proc_num];
			memcpy(dst+loc_index, src+index, loc_len*sizeof(uint8_t));
			d_proc->proc_lens[proc_num] += loc_len;
			d_proc->index[proc_num] = 0;

			index += loc_len;
			loc_index += loc_len;
		}
		else
		{
			loc_index = 0;
			index++;
			fin = 1;
		}
	}
}

void free_DataProc(Data_Proc *d_proc)
{
	int i;
    for (i = 0; i < MAX_PROCS; i++)
    {
        free(d_proc->procs[i]);
    }	
	free(d_proc);
}


void init_DataProcCmd(Data_Cmd *ptr_cmd)
{
	ptr_cmd->cmd = 0;
	ptr_cmd->count = 0;
	ptr_cmd->type = 0;
	//ptr_cmd->params = 0;
	ptr_cmd->params = (float*)calloc(32, sizeof(float));
}

void create_DataProcCmd(uint8_t cmd, uint8_t count, uint8_t type, float *params, Data_Cmd *ptr_cmd)
{
	ptr_cmd->cmd = cmd;
	ptr_cmd->count = count;
	ptr_cmd->type = type;
	ptr_cmd->params = 0;

	if (count > 0)
	{
		ptr_cmd->params = (float*)calloc(count, sizeof(float));
		int i;
		for (i = 0; i < count; i++) ptr_cmd->params[i] = params[i];
	}
}

void clear_DataProcCmd(Data_Proc *d_proc, int index)
{
	memset(d_proc->procs[index], 0x0, MAX_PROC_LEN);
	d_proc->proc_lens[index] = 0;
	d_proc->index[index] = 0;
}

Bool next_DataProcCmd(int index, Data_Proc *d_proc, Data_Cmd *ptr_cmd)
{
    if (d_proc->proc_lens[index] == 0) return False;

	int pos = d_proc->index[index];
	if (pos >= d_proc->proc_lens[index])
	{
		d_proc->index[index] = d_proc->proc_lens[index];
		return False;
	}

	uint8_t cmd = d_proc->procs[index][pos];
	uint8_t count = d_proc->procs[index][pos+1];
	uint8_t type = d_proc->procs[index][pos+2];

    if ((d_proc->proc_lens[index] - pos) < (4*count+3))
    {
    	d_proc->index[index] = 0;
    	return False;
    }

	ptr_cmd->cmd = cmd;
	ptr_cmd->count = count;
	ptr_cmd->type = type;
	
	if (count > 0)
	{
		int i;
		for (i = 0; i < count; i++)
		{
			float param = 0;
			uint32_t byte0 = (uint32_t)(d_proc->procs[index][4*i+pos+3]);
			uint32_t byte1 = (uint32_t)(d_proc->procs[index][4*i+pos+4]);
			uint32_t byte2 = (uint32_t)(d_proc->procs[index][4*i+pos+5]);
			uint32_t byte3 = (uint32_t)(d_proc->procs[index][4*i+pos+6]);
			uint32_t pp = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
			param = (float)pp;
			ptr_cmd->params[i] = param;
		}
	}

	d_proc->index[index] += 4*count + 3;

    return True;
}

/* функция пропуска count инструкций в пакете под номером index
 * в программе обработки данных d_proc
 */
void pass_DataProcCmds(int index, Data_Proc *d_proc, int count)
{
	if (d_proc->proc_lens[index] == 0) return;

	int i;
	int proc_lens = d_proc->proc_lens[index];
	for (i = 0; i < count; i++)
	{
		int pos = d_proc->index[index];
		if (pos >= proc_lens)
		{
			d_proc->index[index] = proc_lens;
			return;
		}

		uint8_t byte_count = d_proc->procs[index][pos+1];
		d_proc->index[index] += 4*byte_count + 3;
	}
}

/* функция, устанавливающая маркер на первую инструкцию в пакете index
 * программы d_proc обработки данных
 */
void move_ToFirstDataProcCmd(int index, Data_Proc *d_proc)
{
	d_proc->index[index] = 0;
}

void free_DataProcCmd(Data_Cmd *ptr_cmd)
{
	if (ptr_cmd->count > 0) free(ptr_cmd->params);
	//free(ptr_cmd);
}


/* Функция, устанавливающая параметры обработки данных в исходные значения */
void setDefaultProcParams(Processing_Params *params)
{
	// Default Parameters for data processing
	params->echo_func = Func_Gauss;
	params->spectr_func = Func_Gauss;
	params->echo_x0 = 0;
	params->spectr_x0 = 0;
	params->echo_sigma = 1024;
	params->spectr_sigma = 1024;
	params->tag = 0;
	params->zero_level = 2048;
	params->points_count = 0;
	params->group_index = 0;
	params->channel_id = 0xFF;
}


/* Функция заполнения буфера данных, находящегося на вершине стека
 *  числом (байтом) byte
 *  src_len - длина массива на вершине стека (количество чисел типа float)
 */
void fill_ByValue(STACKPtrF *stack, int src_len, uint8_t byte)
{
	if (stack->cnt < 1) return; 			// стек пуст !

	float *src = STACKPtrF_first(stack);
	memset(src, byte, src_len*sizeof(float));
}


/* функция приведения исходных данных измеренного ССИ в буфере UPP
 *  к двухбайтному беззнаковому целому типу с обрезкой избыточных данных
 * src - источник даных (предполагается, что буффер upp_buffer)
 * src_len - длина массива src
 * dst - выходной массив двухбайтных беззнаковых целых чисел (предполагается, что ui16_buffer)
 */
void cast_UPPDataToFID_U16(uint8_t *src, int src_len, uint16_t *dst)
{
	if (src_len > DATA_MAX_LEN) src_len = DATA_MAX_LEN;
	memset(dst+src_len, 0x0, (DATA_MAX_LEN-src_len)*sizeof(uint16_t));

	int i;
	for (i = 0; i < src_len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i];
		uint16_t x2 = (uint16_t)src[2*i+1];
		uint16_t x = (x1 | (x2 << 8));
		dst[i] = x;
	}
}

/* функция приведения исходных данных измеренного спинового эхо в буфере UPP
 * к двухбайтному беззнаковому целому типу с обрезкой избыточных данных
 * src - источник даных (предполагается, что буффер upp_buffer)
 * src_len - длина массива src
 * dst - выходной массив двухбайтных беззнаковых целых чисел (предполагается, что ui16_buffer)
 */
void cast_UPPDataToSE_U16(uint8_t *src, int src_len, uint16_t *dst)
{
	int src_pre = (src_len - DATA_MAX_LEN)/2;
	if (src_len < DATA_MAX_LEN) src_pre = 0;
	int src_post = src_pre + DATA_MAX_LEN;
	if (src_len < DATA_MAX_LEN) src_post = src_len;

	int dst_pre = 0;
	if (src_len < DATA_MAX_LEN) dst_pre = (DATA_MAX_LEN - src_len)/2;
	int dst_post = DATA_MAX_LEN;
	if (src_len < DATA_MAX_LEN) dst_post = (DATA_MAX_LEN + src_len)/2;
	memset(dst, 0x0, dst_pre*sizeof(uint16_t));
	memset(dst+dst_post, 0x0, dst_pre*sizeof(uint16_t));

	int len = src_post - src_pre;
	int i;
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i + 2*src_pre];
		uint16_t x2 = (uint16_t)src[2*i+1 + 2*src_pre];
		uint16_t x = (x1 | (x2 << 8));
		dst[i+dst_pre] = x;
	}
}

/* функция приведения исходных данных измеренного ССИ в буфере UPP
 *  к массиву типа float с обрезкой избыточных данных
 * src - источник даных (предполагается, что буффер upp_buffer)
 * src_len - длина массива src
 * dst - выходной массив типа float
 */
void cast_UPPDataToFID(uint8_t *src, int src_len, float *dst)
{
	if (src_len > DATA_MAX_LEN) src_len = DATA_MAX_LEN;
	memset(dst+src_len, 0x0, (DATA_MAX_LEN-src_len)*sizeof(float));

	int i;
	for (i = 0; i < src_len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i];
		uint16_t x2 = (uint16_t)src[2*i+1];
		float x = (x1 | (x2 << 8));
		dst[i] = x;
	}
}

/* функция приведения исходных данных измеренного ССИ в буфере UPP
 *  к массиву типа float с обрезкой избыточных данных и
 * с расширением данных в 2 раза и добавлением числа 2048 в нечетные позиции,
 * тем самым формируя массив, в котором по четным позициям стоят измеренные при
 * помощи АЦП значения, а по нечетным - число 2048 (ноль величины сигнала).
 * src - источник даных (предполагается, что буффер upp_buffer)
 * src_len - длина массива src
 * dst - выходной массив типа float
 */
/*
void cast_UPPDataToFID2(uint8_t *src, Processing_Params *proc_params, float *dst)
{
	int src_len = proc_params->points_count;

	int i;
	if (src_len > DATA_MAX_LEN/2) src_len = DATA_MAX_LEN/2;
	for (i = 0; i < DATA_MAX_LEN/2-src_len; i++)
	{
		dst[i+2*src_len] = 2048.0;
	}

	int index = 0;
	for (i = 0; i < src_len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i];
		uint16_t x2 = (uint16_t)src[2*i+1];
		float x = (x1 | (x2 << 8));

		dst[index++] = x;
		dst[index++] = 2048.0;
	}

	proc_params->points_count = 2*src_len;
}
 */
void cast_UPPDataToFID2(uint8_t *src, Processing_Params *proc_params, float *dst)
{
	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	int i;
	if (src_len > DATA_MAX_LEN/2) src_len = DATA_MAX_LEN/2;

	float M = 0;
	int index = 0;
	for (i = 0; i < src_len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i];
		uint16_t x2 = (uint16_t)src[2*i+1];
		uint16_t x = (x1 | (x2 << 8));

		M += x;
		dst[index] = (float)x;
		index += 2;
	}
	M /= src_len;

	memset(dst+2*src_len, 0x0, (DATA_MAX_LEN/2-src_len)*sizeof(float));

	index = 0;
	for (i = 0; i < src_len; i++)
	{
		float x = dst[index];
		dst[index] = (x - M)*ADC_to_mV;
		index += 2;
	}

	proc_params->points_count = 2*src_len;
}

/* функция приведения исходных данных измеренного спинового эхо в буфере UPP
 * к массиву типа float с обрезкой избыточных данных
 * src - источник даных (предполагается, что буффер upp_buffer)
 * src_len - длина массива src
 * dst - выходной массив типа float
 */
void cast_UPPDataToSE(uint8_t *src, int src_len, float *dst)
{
	int i;

	int src_pre = (src_len - DATA_MAX_LEN)/2;
	if (src_len < DATA_MAX_LEN) src_pre = 0;
	int src_post = src_pre + DATA_MAX_LEN;
	if (src_len < DATA_MAX_LEN) src_post = src_len;

	int dst_pre = 0;
	if (src_len < DATA_MAX_LEN) dst_pre = (DATA_MAX_LEN - src_len)/2;
	int dst_post = DATA_MAX_LEN;
	if (src_len < DATA_MAX_LEN) dst_post = (DATA_MAX_LEN + src_len)/2;
	//memset(dst, 0x0, dst_pre*sizeof(uint16_t));
	//memset(dst+dst_post, 0x0, dst_pre*sizeof(uint16_t));
	/*for (i = 0; i < dst_pre; i++)
	{
		dst[i] = 2048.0;
	}
	for (i = 0; i < dst_pre; i++)
	{
		dst[i+dst_post] = 2048.0;
	}

	int len = src_post - src_pre;
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i + 2*src_pre];
		uint16_t x2 = (uint16_t)src[2*i+1 + 2*src_pre];
		float x = (x1 | (x2 << 8));
		dst[i+dst_pre] = x;
	}*/

	float M = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i + 2*src_pre];
		uint16_t x2 = (uint16_t)src[2*i+1 + 2*src_pre];
		float x = (x1 | (x2 << 8));
		dst[i+dst_pre] = x;

		M += x;

		//uint16_t x1 = (uint16_t)src[2*i + src_pre];
		//uint16_t x2 = (uint16_t)src[2*i+1 + src_pre];
		//uint16_t x = (x1 | (x2 << 8));
	}
	M /= len;

	memset(dst, 0x0, dst_pre*sizeof(float));
	memset(dst+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = dst[index];
		dst[index++] = (x - M)*ADC_to_mV;
	}
}

/* функция приведения исходных данных измеренного спинового эхо в буфере UPP
 * к массиву типа float с обрезкой избыточных данных и
 * с расширением данных в 2 раза и добавлением среднего по исходным данным в нечетные позиции,
 * тем самым формируя массив, в котором по четным позициям стоят измеренные при
 * помощи АЦП значения, а по нечетным - "ноль" величины сигнала.
 * src - источник даных (предполагается, что буффер upp_buffer)
 * src_len - длина массива src
 * dst - выходной массив типа float
 */
/*void cast_UPPDataToSE2(uint8_t *src, Processing_Params *proc_params, float *dst)
{
	int src_len = proc_params->points_count;

	int i;
	int src_pre = 0;
	int src_post = src_len;
	if (src_len > DATA_MAX_LEN/2)
	{
		src_pre = (2*src_len - DATA_MAX_LEN)/2;
		src_post = (2*src_pre + DATA_MAX_LEN)/2;
	}

	int dst_pre = 0;
	if (src_len < DATA_MAX_LEN/2) dst_pre = (DATA_MAX_LEN - 2*src_len)/2;
	int dst_post = DATA_MAX_LEN;
	if (src_len < DATA_MAX_LEN/2) dst_post = (DATA_MAX_LEN + 2*src_len)/2;

	for (i = 0; i < dst_pre; i++)
	{
		dst[i] = 2048.0;
		dst[i+dst_post] = 2048.0;
	}

	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i + src_pre];
		uint16_t x2 = (uint16_t)src[2*i+1 + src_pre];
		float x = (x1 | (x2 << 8));

		dst[index++] = x;
		dst[index++] = 2048.0;
	}

	proc_params->points_count = 2*src_len;
}*/
void cast_UPPDataToSE2(uint8_t *src, Processing_Params *proc_params, float *dst)
{
	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	int i;
	int src_pre = 0;
	int src_post = src_len;
	if (src_len > DATA_MAX_LEN/2)
	{
		src_pre = (2*src_len - DATA_MAX_LEN)/2;
		src_post = (2*src_pre + DATA_MAX_LEN)/2;
	}

	int dst_pre = 0;
	if (src_len < DATA_MAX_LEN/2) dst_pre = (DATA_MAX_LEN - 2*src_len)/2;
	int dst_post = DATA_MAX_LEN;
	if (src_len < DATA_MAX_LEN/2) dst_post = (DATA_MAX_LEN + 2*src_len)/2;

	float M = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i + src_pre];
		uint16_t x2 = (uint16_t)src[2*i+1 + src_pre];
		uint16_t x = (x1 | (x2 << 8));

		M += x;
		dst[index] = (float)x;
		index += 2;
	}
	M /= len;

	memset(dst, 0x0, dst_pre*sizeof(float));
	memset(dst+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = dst[index];
		dst[index++] = (x - M)*ADC_to_mV;
		dst[index++] = 0;
	}

	proc_params->points_count = 2*src_len;
}

/* функция определения "нулевого уровня" (обычно ~2048) в массиве оцифрованных данных
 * src - массив исходных данных
 * len - длина массива
 * proc_params - структура параметров обработки данных
 * Результат - определение параметра "нулевого уровня" для оцифрованных данных и сохранение
 * его в поле zero_level структуры proc_params.
 */
void define_ZeroLevel(float *src, int len, Processing_Params *proc_params)
{
	proc_params->zero_level = 2048.0;
}

/* Функция копирования данных из src в dst.
 * Длина массива src должна быть меньше или равна длине массива dst
 * src - источник даных
 * src_len - длина массива источника данных (массива src)
 * dst - приемник данных
 */
void copy_DataTo(STACKPtrF *stack, int src_len, float *dst)
{
	if (stack->cnt < 1) return; 			// стек пуст !

	float *src = STACKPtrF_first(stack);
	memcpy(dst, src, src_len*sizeof(float));
}

/* Функция извлечения данных из стека данных и копирование их в буффер dst,
 * dst - буффер, в который копируются данные
 * src_len - длина данных в буффере
 * stack - стек с данными
 */
void move_FromStack(STACKPtrF *stack, int src_len, float *dst)
{
	if (stack->cnt < 1) return; 			// стек пуст !

	float *src = STACKPtrF_pop(stack);
	memcpy(dst, src, src_len*sizeof(float));
}

/* Квадратурное детектирование в над данными на вершине стека */
void do_QuadDetect(STACKPtrF *stack)
{
	if (stack->cnt < 2) return;			// в стеке должны быть: буффер-приемник данных и источник данных (на вершине стека)

	float *src = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);
	doQuadDetect(src, dst, 0, DATA_MAX_LEN);
}

/* Применение оконной функции к данным на вершине стека
 * step_id - тип данных: TIME_DMAIN_DATA/FREQ_DATA (данные во временной области / в частотной)
 * proc_params - структура с параметрами обработки данных (параметры оконных функций в том числе)
 */
void apply_WinFunc(int step_id, Processing_Params *proc_params, STACKPtrF *stack)
{
	if (stack->cnt < 2) return;			// в стеке должны быть: буффер-приемник данных и источник данных (на вершине стека)

	float *src = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);

	applyWinFunc(src, dst, DATA_MAX_LEN, proc_params, step_id);
}

/* Функция вычисления амплитудного спектра */
void calc_AmplSpec(STACKPtrF *stack, int src_len)
{
	if (stack->cnt < 2) return;			// в стеке должны быть: буффер-источник данных (в S0, т.е. на вершине стека) и буффер-приемник данных (в ST1)

	float *src = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);

	int i;
	int index = 0;
	for (i = 0; i < src_len; i += 2)
	{
		float x = src[i]*src[i] + src[i+1]*src[i+1];
		dst[index++] = Q_rsqrt(x);	// without sqrt !
		/*if (dst[index-1] < 0)
		{
			printf("x = %f, sqrt_x = %f, src_re = %f, src_im = %f\n", x, dst[index-1], src[i], src[i+1]);
		}*/
	}
}

/* Функция вычисления мощностного спектра */
void calc_PowerSpec(STACKPtrF *stack, int src_len)
{
	if (stack->cnt < 2) return;			// в стеке должны быть: буффер-источник данных (в S0, т.е. на вершине стека) и буффер-приемник данных (в ST1)

	float *src = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);

	int i;
	int index = 0;
	for (i = 0; i < src_len; i += 2)
	{
		dst[index++] = src[i]*src[i] + src[i+1]*src[i+1];	// without sqrt !
	}
}

/* Функция оценки амплитуды сигнала по мощностному спектру
 * src_len - длина буфферов данных (должна совпадать)
 *  * Размещение в стеке:
 * |--------------------|
 * |ST2:      dst       |
 * |--------------------|
 * |ST1:   src_nspec    |
 * |--------------------|
 * |ST0:   src_sspec    |
 * |--------------------|
 * формат операции: dst = src2 {oper} src1
 */
void estimate_SignalAmp1(STACKPtrF *stack, int src_len)
{
	float res = NaN;
	if (stack->cnt < 3) return;			// в стеке должны быть: буффер-источник данных спектра сигнала (ST0)
										// буффер-источник данных по спектру мощости шума (в S1), и буффер-приемник данных (в ST2)

	float *src_sspec = STACKPtrF_pop(stack);
	float *src_nspec = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);

	int i;
	for (i = 0; i < src_len; i++)
	{
		if (src_sspec[i] - src_nspec[i] >= 0)
		{
			dst[i] = Q_rsqrt(src_sspec[i] - src_nspec[i]);
		}
		else
		{
			dst[i] = -Q_rsqrt(src_nspec[i] - src_sspec[i]);
		}
	}
}

/* Функция оценки положения максимума спектра сигнала
 * Размещение в стеке:
 * |----------|
 * |ST0: src  |
 * |----------|
 * src - спектр сигнала
 * src_len - длина массива данных спектра
 * proc_params -
 */
void estimate_MaxSpectrum1(STACKPtrF *stack, int src_len, Processing_Params *proc_params)
{
	if (stack->cnt < 1) return;			// в стеке должны быть: буффер-источник данных спектра сигнала (ST0)
	if (src_len <= 0) return;

	float *src = STACKPtrF_pop(stack);

	int i;
	float S = 0;
	float max = src[0];
	int max_i = 0;
	for (i = 1; i < src_len; i++)
	{
		S += src[i];
		if (src[i] > max)
		{
			max = src[i];
			max_i = i;
		}
	}
	S /= src_len;

	if (max < 2*S) max_i = 0;
	proc_params->spectr_x0 = max_i;
}

/* Функция выполнения бинарной операции над буфером в стеке данных и числом
 * Размещение в стеке:
 * |----------|
 * |ST1: dst  |
 * |----------|
 * |ST0: src1 |
 * |----------|
 * Размещение данных в Data_Cmd *instr:
 * instr->params[0] - код операции
 * instr->params[1] - код ячейки XX (XX = X0, X1, X2 или X3), в которой находится число
 * формат операции: dst = src1 {oper} XX
 */
void do_MathOperationVal(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Data_Cmd *instr)
{
	if (stack->cnt < 2) return;			// в стеке должны быть: буффер-источник данных (в ST0) и буффер-приемник данных (в ST1)

	float *src1 = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);

	int oper_code = (int)instr->params[0];
	int cell_code = (int)instr->params[1];

	float XX = 0;
	switch (cell_code)
	{
	case X0: XX = sum_buff->xx[0]; break;
	case X1: XX = sum_buff->xx[1]; break;
	case X2: XX = sum_buff->xx[2]; break;
	case X3: XX = sum_buff->xx[3]; break;
	default: return;
	}

	float value = XX;
	int i;
	switch (oper_code)
	{
	case OPER_ADD: for (i = 0; i < src_len; i++) dst[i] = src1[i] + value; break;
	case OPER_SUB: for (i = 0; i < src_len; i++) dst[i] = src1[i] - value; break;
	case OPER_MUL: for (i = 0; i < src_len; i++) dst[i] = src1[i] * value; break;
	default: break;
	}
}

/* Функция выполнения бинарной операции над двумя буферами в стеке данных
 * Размещение в стеке:
 * |----------|
 * |ST2: dst  |
 * |----------|
 * |ST1: src2 |
 * |----------|
 * |ST0: src1 |
 * |----------|
 * формат операции: dst = src2 {oper} src1
 */
void do_MathOperationBin(STACKPtrF *stack, int src_len, Data_Cmd *instr)
{
	if (stack->cnt < 3) return;			// в стеке должны быть: два буффера-источника данных (в ST0 и ST1) и буффер-приемник данных (в ST2)

	float *src1 = STACKPtrF_pop(stack);
	float *src2 = STACKPtrF_pop(stack);
	float *dst = STACKPtrF_first(stack);

	int oper_code = (int)instr->params[0];
	float val = 0;
	int sgn = 0;
	int i;
	switch (oper_code)
	{
	case OPER_ADD: for (i = 0; i < src_len; i++) dst[i] = src2[i] + src1[i]; break;
	case OPER_SUB: for (i = 0; i < src_len; i++) dst[i] = src2[i] - src1[i]; break;
	case OPER_MUL: for (i = 0; i < src_len; i++) dst[i] = src2[i] * src1[i]; break;
	case OPER_VD1:
	{
		for (i = 0; i < src_len; i++)
		{
			val = src2[i]; // - src1[i]/15.0;
			if (val >= 0) sgn = 1; else sgn = -1;
			val = fabs(val);
			dst[i] = sgn*Q_rsqrt(val);
		}
		break;
	}
	default: break;
	}
}

/* Функция выполнения бинарной операции над двумя ячейками XX
 * Коды ячеек и код операции в структуре instr:
 * oper_code = (int)instr->params[0];
 * XX_1 = (float)instr->xx[1];
 * XX_2 = (float)instr->xx[2];
 * XX_3 = (float)instr->xx[3];
 * формат операции: XX_3 = XX_2 {oper} XX_1
 */
void do_MathOperationXX(SummationBuffer *sum_buff, Data_Cmd *instr)
{
	int oper_code = (int)instr->params[0];
	int cell_1 = (int)instr->params[1];
	int cell_2 = (int)instr->params[2];
	int cell_3 = (int)instr->params[3];

	float XX_1 = 0;
	switch (cell_1)
	{
	case X0: XX_1 = sum_buff->xx[0]; break;
	case X1: XX_1 = sum_buff->xx[1]; break;
	case X2: XX_1 = sum_buff->xx[2]; break;
	case X3: XX_1 = sum_buff->xx[3]; break;
	default: return;
	}

	float XX_2 = 0;
	switch (cell_2)
	{
	case X0: XX_2 = sum_buff->xx[0]; break;
	case X1: XX_2 = sum_buff->xx[1]; break;
	case X2: XX_2 = sum_buff->xx[2]; break;
	case X3: XX_2 = sum_buff->xx[3]; break;
	default: return;
	}

	float *XX_3;
	switch (cell_3)
	{
	case X0: XX_3 = sum_buff->xx; break;
	case X1: XX_3 = sum_buff->xx+1; break;
	case X2: XX_3 = sum_buff->xx+2; break;
	case X3: XX_3 = sum_buff->xx+3; break;
	default: return;
	}

	switch (oper_code)
	{
	case OPER_ADD: *XX_3 = XX_2 + XX_1; break;
	case OPER_SUB: *XX_3 = XX_2 - XX_1; break;
	case OPER_MUL: *XX_3 = XX_2 * XX_1; break;
	case OPER_DIV: if (XX_1 == 0) *XX_3 = XX_2; else *XX_3 = XX_2 / XX_1; break;
	default: break;
	}
}

/* Функция прибавления числа в ячейку XX
 * Код ячейки и прибавляемого числа в структуре instr:
 * value = (float)instr->xx[0];
 * XX = (float)instr->xx[1];
 * формат операции: XX = XX + value
 */
void add_ValueToXX(SummationBuffer *sum_buff, Data_Cmd *instr)
{
	float value = (int)instr->params[0];
	int cell = (int)instr->params[1];

	if (cell == GR_SUM_BUFF) sum_buff->group_index += value;
	else
	{
		float *XX;
		switch (cell)
		{
		case X0: XX = sum_buff->xx; break;
		case X1: XX = sum_buff->xx+1; break;
		case X2: XX = sum_buff->xx+2; break;
		case X3: XX = sum_buff->xx+3; break;
		default: return;
		}

		float xx = *XX;
		*XX = xx + value;
	}
}

/* Функция накопления данных
 * src_len - длина данных
 */
void accumulate_Data(STACKPtrF *stack, int src_len, Processing_Params *proc_params)
{
	if (stack->cnt < 2) return;			// в стеке должны быть: буффер-источник данных (в ST0) и буффер-накопитель данных (в ST1)

	float *src = STACKPtrF_pop(stack);  // ps
	float *dst = STACKPtrF_first(stack); //ps-1

	int i;
	int N = proc_params->current_echo;

	if (N < 2) memcpy(dst, src, src_len*sizeof(float));
	else
	{
		for (i = 0; i < src_len; i++)
		{
			dst[i] = (N-1)*dst[i]/N + src[i]/N;
		}
	}
}

/* Функция накопления данных с экспоненциальным сглаживаинем
 * src_len - длина данных
 * alpha - параметр сглаживания (0 <= alpha <= 1000)
 *
 */
void accsmooth_Data(STACKPtrF *stack, int src_len, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (stack->cnt < 3) return;			// в стеке должны быть: буфферы-источники данных (в ST0, ST1) и буффер-накопитель данных (в ST2)

	float *src = STACKPtrF_pop(stack);
	float *dst1 = STACKPtrF_pop(stack);
	float *dst2 = STACKPtrF_pop(stack);

	float alpha = instr->params[0];
	alpha /= 1000;

	int i;
	int N = proc_params->current_echo;

	if (N < 2) memcpy(dst1, src, src_len*sizeof(float));
	else
	{
		for (i = 0; i < src_len; i++)
		{
			dst1[i] = (1-alpha)*dst1[i] + src[i]*alpha;
		}
	}

	float invN = 1.0/N;
	if (N < 2) memcpy(dst2, src, src_len*sizeof(float));
	else
	{
		for (i = 0; i < src_len; i++)
		{
			dst2[i] = ((N-1)*dst2[i] + dst1[i])*invN;
		}
	}

	STACKPtrF_push(dst2,stack);
}

/* Функция прореживания данных в стеке
 * Результат помещается на вершину стека
 */
Bool decimateDataInOutputbuffer(STACKPtrF *stack, OutBuffer *out_buff, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (stack->cnt < 1) return False;			// в стеке должны быть: буффер-источник данных (в ST0) и буффер-накопитель данных (в ST1)

	int data_type = instr->type;

	float *src = STACKPtrF_pop(stack);
	float *dst = out_buff->out_data;
	int dst_pos = out_buff->full_size;
	int data_cnt = out_buff->outdata_counter;

	switch (data_type)
	{
	case DT_RFP:
	{
		if (instr->count != 1) return False;		// у инструкции имеется один параметр
		int decim_step = instr->params[0];
		if (decim_step < 1) decim_step = 1;

		//int src_len = proc_params->points_count/2;
		//int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
		int src_len = proc_params->points_count;
		int pre_pos = (DATA_MAX_LEN - src_len)/2;
		if (pre_pos < 0) pre_pos = 0;

		//src_len = proc_params->points_count/2/decim_step;
		//if (src_len % decim_step > 0) dst_len++;
		//if (src_len < decim_step) return False;

		int i;
		int index = 0;
		float val_re, val_im, val_abs = 0;
		for (i = 0; i < src_len; i += 2*decim_step)
		{
			val_re = src[pre_pos+i];
			val_im = src[pre_pos+i+1];
			val_abs = Q_rsqrt(val_re*val_re + val_im*val_im);
			dst[dst_pos+index++] = val_abs;
		}
		out_buff->outdata_len[data_cnt] = index; //src_len;
		out_buff->full_size += index; //src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	default: return False;
	}

	return True;

}

/* Функция поточечного накопления данных в результате сложения всех точек массива src
 *
 */
Bool summarize_Data(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Data_Cmd *instr)
{
	if (stack->cnt < 1) return False;			// в стеке должны быть: буффер-источник данных (в ST0)

	uint8_t cmd = instr->type;
	sum_buff->data_id = cmd;

	float *src = STACKPtrF_pop(stack);
	int i;
	float S = 0;
	for (i = 0; i < src_len; i++)
	{
		S += src[i];
	}

	int index = sum_buff->pos;
	if (index >= sum_buff->max_size) return False;
	sum_buff->sum_data[index] = S;

	sum_buff->pos++;

	return True;
}

Bool summarize_DataForRelax(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (stack->cnt < 1) return False;			// в стеке должны быть: буффер-источник данных (в ST0)

	uint8_t cmd = instr->type;
	sum_buff->data_id = cmd;
	sum_buff->channel_id = proc_params->channel_id; //proger_rd_ch_number();

	float *src = STACKPtrF_pop(stack);
	int i;
	float S = 0;
	for (i = 0; i < src_len/2; i++)
	{
		S += src[i];
	}

	int index = /*sum_buff->pos +*/ proc_params->current_echo - 1;
	if (index >= sum_buff->max_size) return False;
	if (index < 0) index = 0;

	sum_buff->sum_data[index] = S;
	sum_buff->pos = index+1;

	return True;
}

// Функция усреднения массива данных, находящегося в стеке
Bool average_Data(STACKPtrF *stack, int src_len, SummationBuffer *sum_buff, Data_Cmd *instr)
{
	if (stack->cnt < 1) return False;			// в стеке должны быть: буффер-источник данных (в ST0)

	float *src = STACKPtrF_pop(stack);

	if (instr->count != 1) return False;		// у инструкции имеется один параметр

	float *XX;
	int num = instr->params[0];
	switch (num)
	{
	case X0: XX = &sum_buff->xx[0]; break;
	case X1: XX = &sum_buff->xx[1]; break;
	case X2: XX = &sum_buff->xx[2]; break;
	case X3: XX = &sum_buff->xx[3]; break;
	default: return False;
	}

	int i;
	float S = 0;
	for (i = 0; i < src_len; i++)
	{
		S += src[i];
	}
	if (src_len != 0) S /= src_len;
	*XX = S;

	return True;
}

/* Перенос данных в буфер хранения данных ЯМР */
/*Bool move_ToNMRBuffer(STACKPtrF *stack, int src_len, float *dst, int *dst_len, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (stack->cnt < 1) return False;			// в стеке должны быть: буффер-источник данных (в ST0) и буффер-накопитель данных (в ST1)

	float *src = STACKPtrF_pop(stack);

	uint8_t data_type = instr->type;
	switch (data_type)
	{
	case DT_SGN_SE:
	case DT_RFP:
		{
			int pre_pos = proc_params->points_count/2;
			memcpy(dst, src+pre_pos, proc_params->points_count*sizeof(float));
			*dst_len = proc_params->points_count;
			break;
		}
	case RELAX_DATA:
		{
			memcpy(dst, src, src_len*sizeof(float));
			*dst_len = proc_params->echo_count; break;
		}
	default: return False;

	return True;
	}
}*/

Bool move_AccToOutputBuffer(STACKPtrF *stack, SummationBuffer *sum_buff, OutBuffer *out_buff, Processing_Params *proc_params)
{
	static int ii = 0;
	ii++;

	if (sum_buff->max_size <= 0) return False;
	if (sum_buff->pos <= 0) return False;

	uint8_t cmd = sum_buff->data_id;
	int echo_count_sum = sum_buff->pos;

	float *dst = out_buff->out_data;
	int dst_pos = out_buff->full_size;
	int data_cnt = out_buff->outdata_counter;

	/*int echo_count = proc_params->echo_count;
	float *tmp = (float*)calloc(echo_count, sizeof(float));
	memset(tmp, 0xFF, echo_count*sizeof(float));
	memcpy(tmp, sum_buff->sum_data, echo_count_sum*sizeof(float));

	STACKPtrF_push(tmp, stack);
	int last_index = out_buff->outdata_counter;
	out_buff->group_index[last_index-1] = sum_buff->group_index;
	move_ToOutputBuffer(stack, out_buff, proc_params, cmd);

	SummationBuffer_ClearAll(sum_buff);*/

	float *tmp = (float*)calloc(echo_count_sum, sizeof(float));
	memcpy(dst+dst_pos, sum_buff->sum_data, echo_count_sum*sizeof(float));
	out_buff->outdata_len[data_cnt] = echo_count_sum;
	out_buff->full_size += echo_count_sum;
	out_buff->data_id[data_cnt] = cmd;
	out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
	out_buff->outdata_counter++;
	out_buff->group_index[data_cnt] = sum_buff->group_index;

	SummationBuffer_ClearAll(sum_buff);

	free(tmp);

	return True;
}


Bool move_ToOutputBuffer(STACKPtrF *stack, OutBuffer *out_buff, Processing_Params *proc_params, uint8_t data_type)
{
	if (stack->cnt < 1) return False;			// в стеке должны быть: буффер-источник данных (в ST0) и буффер-накопитель данных (в ST1)

	float *src = STACKPtrF_pop(stack);
	float *dst = out_buff->out_data;
	int dst_pos = out_buff->full_size;
	int data_cnt = out_buff->outdata_counter;

	/*if (dst_pos + proc_params->points_count > NMR_DATA_LEN)
	{
		volatile int aaa = 0;
		return False;
	}*/

	switch (data_type)
	{
	case DT_SGN_FID_ORG:
	case DT_NS_FID_ORG:
	{
		int src_len = proc_params->points_count;
		memcpy(dst+dst_pos, src, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	case DT_SGN_SE_ORG:
	case DT_NS_SE_ORG:
	//case DT_SGN_SE:
	//case DT_NS_SE:
	{
		int src_len = proc_params->points_count;
		int pre_pos = (DATA_MAX_LEN - src_len)/2;
		if (pre_pos < 0) pre_pos = 0;
		memcpy(dst+dst_pos, src+pre_pos, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id;//proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	case DT_SGN_SE:
	case DT_NS_SE:
	case DT_RFP:
	{
		int src_len = proc_params->points_count/2;
		int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
		if (pre_pos < 0) pre_pos = 0;
		int i;
		for (i = 0; i < src_len; i++) dst[dst_pos+i] = src[pre_pos+2*i];
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	case DT_SGN_FID:
	case DT_NS_FID:
	{
		int src_len = proc_params->points_count/2;
		int i;
		for (i = 0; i < src_len; i++) dst[dst_pos+i] = src[2*i];
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	case DT_SGN_QUAD_SE:
	case DT_NS_QUAD_SE:
	case DT_SGN_QUAD_FID:
	case DT_NS_QUAD_FID:
	{
		int src_len = proc_params->points_count;
		int pre_pos = (DATA_MAX_LEN - src_len)/2;
		if (pre_pos < 0) pre_pos = 0;
		int i;
		int data_len = proc_params->points_count/2;
		for (i = 0; i < data_len; i++)
		{
			*(dst+dst_pos+i) = src[pre_pos+2*i];
		}
		out_buff->outdata_len[data_cnt] = proc_params->points_count/2;
		out_buff->full_size += proc_params->points_count/2;
		out_buff->outdata_counter++;
		if (data_type == DT_SGN_QUAD_SE) out_buff->data_id[data_cnt] = DT_SGN_QUAD_SE_RE;
		else if (data_type == DT_NS_QUAD_SE) out_buff->data_id[data_cnt] = DT_NS_QUAD_SE_RE;
		else if (data_type == DT_SGN_QUAD_FID) out_buff->data_id[data_cnt] = DT_SGN_QUAD_FID_RE;
		else out_buff->data_id[data_cnt] = DT_NS_QUAD_FID_RE;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		dst_pos += data_len;
		data_cnt++;
		for (i = 0; i < data_len; i++)
		{
			*(dst+dst_pos+i) = src[pre_pos+2*i+1];
		}
		out_buff->outdata_len[data_cnt] = proc_params->points_count/2;
		out_buff->full_size += proc_params->points_count/2;
		out_buff->outdata_counter++;
		if (data_type == DT_SGN_QUAD_SE) out_buff->data_id[data_cnt] = DT_SGN_QUAD_SE_IM;
		else if (data_type == DT_NS_QUAD_SE) out_buff->data_id[data_cnt] = DT_NS_QUAD_SE_IM;
		else if (data_type == DT_SGN_QUAD_FID) out_buff->data_id[data_cnt] = DT_SGN_QUAD_FID_IM;
		else out_buff->data_id[data_cnt] = DT_NS_QUAD_FID_IM;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		break;
	}
	case DT_NS_FFT_FID:
	case DT_NS_FFT_SE:
	case DT_SGN_FFT_FID:
	case DT_SGN_FFT_SE:
	{
		int src_len = DATA_MAX_LEN/2;
		//int src_len = DATA_MAX_LEN;
		int pre_pos = 0;
		int i;
		int data_len = src_len/2;
		for (i = 0; i < data_len; i++)
		{
			*(dst+dst_pos+i) = src[pre_pos+2*i];
		}
		out_buff->outdata_len[data_cnt] = data_len;
		out_buff->full_size += data_len;
		out_buff->outdata_counter++;
		if (data_type == DT_SGN_FFT_SE) out_buff->data_id[data_cnt] = DT_SGN_FFT_SE_RE;
		else if (data_type == DT_NS_FFT_SE) out_buff->data_id[data_cnt] = DT_NS_FFT_SE_RE;
		else if (data_type == DT_SGN_FFT_FID) out_buff->data_id[data_cnt] = DT_SGN_FFT_FID_RE;
		else out_buff->data_id[data_cnt] = DT_NS_FFT_FID_RE;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();

		dst_pos += data_len;
		data_cnt++;

		for (i = 0; i < data_len; i++)
		{
			*(dst+dst_pos+i) = src[pre_pos+2*i+1];
		}
		out_buff->outdata_len[data_cnt] = data_len;
		out_buff->full_size += data_len;
		out_buff->outdata_counter++;
		if (data_type == DT_SGN_FFT_SE) out_buff->data_id[data_cnt] = DT_SGN_FFT_SE_IM;
		else if (data_type == DT_NS_FFT_SE) out_buff->data_id[data_cnt] = DT_NS_FFT_SE_IM;
		else if (data_type == DT_SGN_FFT_FID) out_buff->data_id[data_cnt] = DT_SGN_FFT_FID_IM;
		else out_buff->data_id[data_cnt] = DT_NS_FFT_FID_IM;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		break;
	}
	case DT_SGN_FFT_FID_AM:
	case DT_NS_FFT_FID_AM:
	case DT_SGN_FFT_SE_AM:
	case DT_NS_FFT_SE_AM:
	{
		int src_len = DATA_MAX_LEN/4;
		memcpy(dst+dst_pos, src, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}

	case DT_SGN_POWER_SE:
	case DT_SGN_POWER_FID:
	case DT_NS_POWER_SE:
	case DT_NS_POWER_FID:
	{
		//int src_len = DATA_MAX_LEN/2;
		int src_len = DATA_MAX_LEN/4;
		memcpy(dst+dst_pos, src, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	/*case RELAX_DATA:
	{
		int src_len = proc_params->echo_count;
		memcpy(dst+dst_pos, src, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->outdata_counter++;
		break;
	}*/
	default: return False;
	}

	return True;
}

Bool move_XXToOutBuffer(SummationBuffer *sum_buff, OutBuffer *out_buff, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (instr->count != 1) return False;		// у инструкции имеется один параметр

	uint8_t data_type = instr->type;

	float *XX;
	int num = instr->params[0];
	switch (num)
	{
	case X0: XX = &sum_buff->xx[0]; break;
	case X1: XX = &sum_buff->xx[1]; break;
	case X2: XX = &sum_buff->xx[2]; break;
	case X3: XX = &sum_buff->xx[3]; break;
	default: return False;
	}

	//float value = (float)(*XX);

	float *dst = out_buff->out_data;
	int dst_pos = out_buff->full_size;
	int data_cnt = out_buff->outdata_counter;

	switch (data_type)
	{
	case DT_GAMMA:
	{
		memcpy(dst+dst_pos, XX, sizeof(float));
		out_buff->outdata_len[data_cnt] = 1;
		out_buff->full_size += 1;
		out_buff->data_id[data_cnt] = data_type;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
		break;
	}
	default: break;
	}

	return True;
}

Bool write_ValueToSummationBuffer(SummationBuffer *sum_buff, Data_Cmd *instr)
{
	if (instr->count != 1) return False;		// у инструкции имеется один параметр

	uint8_t cmd = instr->type;
	sum_buff->data_id = cmd;
	//sum_buff->channel_id = proger_rd_ch_number();

	float value;
	int num = instr->params[0];
	switch (num)
	{
	/*case 0: value = sum_buff->xx[0]; break;
	case 1: value = sum_buff->xx[1]; break;
	case 2: value = sum_buff->xx[2]; break;
	case 3: value = sum_buff->xx[3]; break;*/
	case X0: value = sum_buff->xx[0]; break;
	case X1: value = sum_buff->xx[1]; break;
	case X2: value = sum_buff->xx[2]; break;
	case X3: value = sum_buff->xx[3]; break;
	default: return False;
	}

	int index = sum_buff->pos + 1;
	sum_buff->sum_data[index] = value;
	sum_buff->pos = index;

	return True;
}


void TimingData_ClearAll(TimingData *tim_data)
{
	memset(&(tim_data->device_id)[0], 0x0, MAX_TIMINGS*sizeof(unsigned int));
	memset(&(tim_data->hard_echo_counter)[0], 0x0, MAX_TIMINGS*sizeof(int));
	memset(&(tim_data->pack_id)[0], 0x0, MAX_TIMINGS*sizeof(int));
	memset(&(tim_data->device_id)[0], 0x0, MAX_TIMINGS*sizeof(int));
	tim_data->count = 0;
	tim_data->target_loop = -1;
	tim_data->loop = 0;
}

void TimingData_Clear(TimingData *tim_data)
{
	memset(&(tim_data->device_id)[0], 0x0, MAX_TIMINGS*sizeof(unsigned int));
	memset(&(tim_data->hard_echo_counter)[0], 0x0, MAX_TIMINGS*sizeof(int));
	memset(&(tim_data->pack_id)[0], 0x0, MAX_TIMINGS*sizeof(int));
	memset(&(tim_data->device_id)[0], 0x0, MAX_TIMINGS*sizeof(int));
	tim_data->count = 0;
}

void TimingData_Print(TimingData *tim_data)
{
	if (tim_data->count == 0) return;

	printf("CPU clocks\tEcho\tDevice\tPackage\n");

	int i;
	for (i = 0; i < tim_data->count; i++)
	{
		printf("%d\t%d\t%d\t%d\n", tim_data->time[i], tim_data->hard_echo_counter[i], tim_data->device_id[i], tim_data->pack_id[i]);
	}
	printf("\n\n");
}


void TimingProc_Buffer_Init(TimingProc_Buffer *timproc_buff, uint32_t tsch, uint32_t tscl)
{
	timproc_buff->len = 0;
	uint64_t tsc_state = 0;
	timproc_buff->tsc_init = ((tsc_state | tsch) << 32) | tscl;
}

Bool TimingProc_Buffer_Add(TimingProc_Buffer *timproc_buff, uint16_t prid, uint32_t tsch, uint32_t tscl)
{
	uint16_t index = timproc_buff->len;
	if (index >= MAX_TIMING_BUFF_LEN-1) return False;

	uint64_t cur_tsc = 0;
	cur_tsc = (((cur_tsc | tsch) << 32) | tscl) - timproc_buff->tsc_init;
	timproc_buff->time[index] = (uint32_t)(cur_tsc / 300);
	timproc_buff->proc_id[index] = prid;
	timproc_buff->len++;

	return True;
}

uint32_t TimingProc_Buffer_ProcId_At(TimingProc_Buffer *timproc_buff, uint16_t index)
{
	if (index >= timproc_buff->len) return 0;

	return timproc_buff->proc_id[index];
}

uint32_t TimingProc_Buffer_Time_At(TimingProc_Buffer *timproc_buff, uint16_t index)
{
	if (index >= timproc_buff->len) return 0;

	return timproc_buff->time[index];
}

uint16_t TimingProc_Buffer_Count(TimingProc_Buffer *timproc_buff)
{
	return timproc_buff->len;
}

// proc_id:
// |-- H.byte 4 --|--- byte 3 ---|--- byte 2 ---|-- 1 byte 1 --|
// H.byte 4 (highest byte) - id устройства, с которым в данный момент идет работа (см. NMR_TOOL, GAMMA_TOOL и т.д.)
// byte 3 - hard_echo_counter (highest byte)
// byte 2 - hard_echo_counter (lowest byte)
// L.byte 1 (lowest byte) - GPIO 1-3 state: 4 h.bits - GPIO status, 4 l.bits - GPIO number (1,2 or 3)
void TimingProc_Buffer_Print1(TimingProc_Buffer *timproc_buff, uint16_t echo_index)
{
	if (timproc_buff->len == 0) return;

	uint16_t last_index = timproc_buff->len;
	uint32_t proc_id = timproc_buff->proc_id[last_index];

	uint16_t echo_number = (uint16_t)((proc_id >> 8) & 0xFFFF);
	if (echo_index != echo_number) return;

	printf("time (mks)\tGPIO_N\tstatus\tEcho\tDevice\n");

	int i;
	for (i = 0; i < timproc_buff->len; i++)
	{
		uint8_t GPIO_N = 0;
		uint8_t GPIO_status = 0;
		uint16_t h_echo_counter = 0;
		uint8_t dev_id = 0;

		uint32_t proc_id = timproc_buff->proc_id[i];
		GPIO_N = (uint8_t)(proc_id & 0x0F);
		GPIO_status = (uint8_t)((proc_id & 0xF0) >> 4);
		h_echo_counter = (uint16_t)((proc_id >> 8) & 0xFFFF);
		dev_id = (uint8_t)((proc_id >> 24) & 0xFF);

		printf("%d\t%d\t%d\t%d\t%d\n", timproc_buff->time[i], GPIO_N, GPIO_status, h_echo_counter, dev_id);
	}
	printf("\n\n");
}

void TimingProc_Buffer_Print2(TimingProc_Buffer *timproc_buff)
{
	if (timproc_buff->len == 0) return;

	uint16_t last_index = timproc_buff->len;
	uint32_t proc_id = timproc_buff->proc_id[last_index];

	printf("time (mks)\tGPIO_N\tstatus\tEcho\tDevice\n");

	int i;
	for (i = 0; i < timproc_buff->len; i++)
	{
		uint8_t GPIO_N = 0;
		uint8_t GPIO_status = 0;
		uint16_t h_echo_counter = 0;
		uint8_t dev_id = 0;

		uint32_t proc_id = timproc_buff->proc_id[i];
		GPIO_N = (uint8_t)(proc_id & 0xF);
		GPIO_status = (uint8_t)((proc_id & 0xF0) >> 4);
		h_echo_counter = (uint16_t)((proc_id >> 8) & 0xFFFF);
		dev_id = (uint8_t)((proc_id >> 24) & 0xFF);

		printf("%d\t%d\t%d\t%d\t%d\n", timproc_buff->time[i], GPIO_N, GPIO_status, h_echo_counter, dev_id);
	}
	printf("\n\n");
}
