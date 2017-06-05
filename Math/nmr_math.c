/*
 * nmr_math.c
 *
 *  Created on: 14.01.2015
 *      Author: Admin
 */

#include <math.h>
#include "nmr_math.h"
#include "../common_data.h"


/*void initExpTab(void)
{
	int i;
	for (i = 0; i < EXP_TAB_LEN; i++) exp_tab[i] = exp(-(float)i/1000);
}*/

void initGaussTab(void)
{
	int i;
	for (i = 0; i < REF_DATA_LEN; i++)
	{
	    gauss_tab[i] = exp(-(i-REF_X0)*(i-REF_X0)/(float)(2*REF_SIGMA*REF_SIGMA));
	}
	/*for (i = 0; i < REF_DATA_LEN; i += 2)
	{
	    gauss_tab[i] = exp(-(i-REF_X0)*(i-REF_X0)/(float)(2*REF_SIGMA*REF_SIGMA));
	    gauss_tab[i+1] = gauss_tab[i];
	}*/
}

void initBiGaussTab(void)
{
	int i;
	for (i = 0; i < REF_DATA_LEN; i++)
	{
	    bigauss_tab[i] = exp(-(i-REF_X0)*(i-REF_X0)/(2.0f*REF_SIGMA*REF_SIGMA)*(i-REF_X0)*(i-REF_X0)/(2.0f*REF_SIGMA*REF_SIGMA));
	}
	/*for (i = 0; i < REF_DATA_LEN/2; i++)
	{
	    bigauss_tab[2*i] = exp(-(i-REF_X0)*(i-REF_X0)/(2.0f*REF_SIGMA*REF_SIGMA)*(i-REF_X0)*(i-REF_X0)/(2.0f*REF_SIGMA*REF_SIGMA));
	    bigauss_tab[2*i+1] = bigauss_tab[2*i];
	}*/
}

// fast exp(...)
inline float Q_exp(float x)
{
	if (x >= 10000) return 0;

	//int i = round(x*1000);
	//int i = (int)(x*1000);

	//if (f < 1) return exp_tab[0];

	int *ptr_u32 = (int*)&x;
	int val = *ptr_u32;

	int E = ((val>>23) & 0x01FF) - 127;
	int i = ((val & 0x7FFE00)>>(23-E)) | (1<<E);

	return exp_tab[i];
}

// Euclid's algorithm search of the greatest common divisor (поиск НОД)
inline int Q_NOD(int A, int B)
{
	while (A != B)
	{
	    if (A > B) A -= B;
	    else B -= A;
	}
	return A;
}

//fast sqrt(...)
/*inline float Q_rsqrt( float number )
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
//  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y*number*1.00168579158972f;			// 1,0016857915897235564467551425010197 is number to correct systematic error
}*/

// AUC (area under curve) for square of module of complex numbers
float Q_power(float *src, int from, int to)
{
	int i;
	float S = 0;
	if (from >= to) return 0;

	for (i = from; i < to; i += 2)
	{
		S += src[i]*src[i] + src[i+1]*src[i+1];	// without sqrt !
	}

	return S;
}


int rad_gen (int n)
{
	volatile int x;

	for(x = 0; x <= 31; x++)
		if((n & (1 << x)) != 0)
			break;

	if(x % 2 == 0)
		return 4;
	else
		return 2;
}/* rad_gen */


void tw_gen (float *w, int n)
{
	int i, j, k;
	double x_t, y_t, theta1, theta2, theta3;
	const double PI = 3.141592654;

	for(j = 1, k = 0; j <= n >> 2; j = j << 2)
	{
		for(i = 0; i < n >> 2; i += j)
		{
			theta1 = 2 * PI * i / n;
			x_t = cos(theta1);
			y_t = sin(theta1);
			w[k] = (float)(x_t);
			w[k + 1] = (float)(y_t);

			theta2 = 4 * PI * i / n;
			x_t = cos(theta2);
			y_t = sin(theta2);
			w[k + 2] = (float)(x_t);
			w[k + 3] = (float)(y_t);

			theta3 = 6 * PI * i / n;
			x_t = cos(theta3);
			y_t = sin(theta3);
			w[k + 4] = (float)(x_t);
			w[k + 5] = (float)(y_t);

			k += 6;
		}
	}
}/* tw_gen */

void init_fft_data (float *x, int n, int pad)
{
	volatile int i;

	for(i = 0; i < n; i++)
	{
		x[pad + 2 * i] = sin(2 * 3.1415 * 50 * i / (double)(n));
		x[pad + 2 * i + 1] = sin(2 * 3.1415 * 100 * i / (double)(n));
	}
}/* init_fft_vec */

// Предварительная обработка исходных данных АЦП:
// 1) 2*uint8_t -> float
// 2) дозаписывание нулями "слева" и "справа", если число принятых отсчетов АЦП < 2048
// 3) "центрирование данных", т.е. вычитание 2048
void DoubleBytesToFloat(unsigned char *src, float *dst, int len, float zero)
{
	int zero_cnt_pre = (DATA_MAX_LEN - len)/2;
	int zero_cnt_post = zero_cnt_pre + len;
	memset(dst, 0x0, zero_cnt_pre*sizeof(float));
	memset(dst+zero_cnt_post, 0x0, zero_cnt_pre*sizeof(float));

	int i;
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)src[2*i];
		uint16_t x2 = (uint16_t)src[2*i+1];
		float x = (x1 | (x2 << 8)) - zero;
		dst[i+zero_cnt_pre] = x;

		/*union { float f; unsigned int a; } u;
		uint16_t *x = (uint16_t*)(src + i);
		u.f = 16777216.0f;
		u.a |= *x;
		dst[i] = u.f - 16777216.0f - zero; // - 2048.0f; */
	}
}


/* Запоминание новых параметров оконной функции
 * x0 - положение центра оконной функции
 * sigma - параметр ширины оконной функции
 * func_id - тип оконной функции: NoFunc - нет функции, Func_Rect - прямоугольная функция,
 * 			 Func_Gauss - гауссова функция, Func_BiGauss - "двойная" (степени 4) гауссова функция
 * params - структура с параметрами обработки данных
 */
void setWinFuncParams(int x0, int sigma, int func_id, uint8_t step_id, Processing_Params *params)
{
	if (step_id == FREQ_DATA)
	{
		params->spectr_func = func_id;
		params->spectr_sigma = sigma;
		params->spectr_x0 = x0;
	}
	else if (step_id == TIME_DOMAIN_DATA)
	{
		params->echo_func = func_id;
		params->echo_sigma = sigma;
		params->echo_x0 = x0;
	}
}

/* Чтение из инструкции instr и запоминание новых параметров оконной функции
 * instr - инструкция типа Data_Cmd
 * step_id - параметр, определяющий тип данных, к которым будет применяться оконная функция (TIME_DOMAIN_DATA/FREQ_DATA)
 * params - структура с параметрами обработки данных
 */
void setWinFuncParamsPro(Data_Cmd *instr, uint8_t step_id, Processing_Params *params)
{
	if (instr->count != 3)	return;		// число параметров команды INS_WIN_TIME должно быть равно 3

	uint8_t func_type = (uint8_t)instr->params[0];
	int x0 = (int)instr->params[1];
	int sigma = (int)instr->params[2];

	if (step_id == FREQ_DATA)
	{
		params->spectr_func = func_type;
		params->spectr_x0 = x0;
		params->spectr_sigma = sigma;
	}
	else if (step_id == TIME_DOMAIN_DATA)
	{
		params->echo_func = func_type;
		params->echo_x0 = x0;
		params->echo_sigma = sigma;
	}
}

// Применение оконной функции к массиву данных src
// src - указатель на исходные данные
// dst - указатель на массив выходных данных
// N - длина массива данных, к которому применяется оконная функция
// params - указател на структуру, содержащую параметры оконной функции
// step_id - идентификатор стадии обработки данных: применение оконной функции во временной (step_id = Time_Domain = 0) или в частотной (step_id = Freq_Spectrum = 1) области
/*void applyWinFunc(float *src, float *dst, int N, Processing_Params *params, int step_id)
{
	int i;

	uint8_t func_id = 0;
	int x0 = 0;
	int sigma = 256;

	if (step_id == TIME_DOMAIN_DATA)
	{
		func_id = params->echo_func;
		x0 = params->echo_x0;
		sigma = params->echo_sigma;
	}
	else if (step_id == FREQ_DATA)
	{
		func_id = params->spectr_func;
		x0 = params->spectr_x0;
		sigma = params->spectr_sigma;
	}
	else return;


	switch (func_id)
	{
	case NoFunc: 				// No win-function
		memcpy(&dst[0], &src[0], N*sizeof(float));
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < N; i++)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			dst[i] = a*src[i];
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);

		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0) dst[0] = tab[index]*src[0];
		else dst[0] = 0;
		int counter = 1;
		int i;
		for (i = 1; i < N; i++)
		{
		    index += step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN) dst[i] = tab[index]*src[i];
		    else dst[i] = 0;
		}
		break;
	}
	default: break;
	}
}*/

// Функция скорректирована с учетом того, что во временной области точки чередуются: Re, Im, Re, Im,...
void applyWinFunc(float *src, float *dst, int N, Processing_Params *params, int step_id)
{
	int i;

	uint8_t func_id = 0;
	int x0 = 0;
	int sigma = 256;

	if (step_id == TIME_DOMAIN_DATA)
	{
		func_id = params->echo_func;
		x0 = params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
		sigma = params->echo_sigma;

		int points_count = params->points_count;
		float mul = N/(float)points_count;
		//x0 = (int)(x0*mul);
		//sigma = (int)(sigma*mul);

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			memcpy(&dst[0], &src[0], N*sizeof(float));
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < N; i+= 2)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				dst[i] = a*src[i];
				dst[i+1] = a*src[i+1];
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);
			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				dst[0] = tab[index]*src[0];
				dst[1] = tab[index]*src[1];
			}
			else
			{
				dst[0] = 0;
				dst[1] = 0;
			}
			int counter = 1;
			int i;
			for (i = 2; i < N; i+=2)
			{
			    index += 2*step;
			    if (counter++ >= d0 && dindex != 0)
			    {
			        index += 2*dindex;
			        counter = 1;
			    }
			    if (index >= 0 && index < REF_DATA_LEN)
			    {
			    	dst[i] = tab[index]*src[i];
			    	dst[i+1] = tab[index]*src[i+1];
			    }
			    else
			    {
			    	dst[i] = 0;
			    	dst[i+1] = 0;
			    }
			}
			break;
		}
		default: break;
		}
	}
	else if (step_id == FREQ_DATA)
	{
		func_id = params->spectr_func;
		//x0 = params->spectr_x0 + CMPLX_DATA_MAX_LEN/2;
		x0 = params->spectr_x0;
		sigma = params->spectr_sigma;
		//N /= 2;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			memcpy(&dst[0], &src[0], N*sizeof(float));
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < N; i++)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				dst[i] = a*src[i];
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);

			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				dst[0] = tab[index]*src[0];
				dst[1] = tab[index]*src[1];
			}
			else
			{
				dst[0] = 0;
				dst[1] = 0;
			}
			int counter = 1;
			int i;
			for (i = 2; i < N; i+=2)
			{
			    index += 2*step;
			    if (counter++ >= d0 && dindex != 0)
			    {
			        index += 2*dindex;
			        counter = 1;
			    }
			    if (index >= 0 && index < REF_DATA_LEN)
			    {
			    	dst[i] = tab[index]*src[i];
			    	dst[i+1] = tab[index]*src[i+1];
			    }
			    else
			    {
			    	dst[i] = 0;
			    	dst[i+1] = 0;
			    }
			}
			break;
		}
		default: break;
		}
	}
	else return;

}

void doQuadDetect(float *src, float *dst, int from, int to)
{
	float a0 = src[0];
	float a1 = src[1];

	int i;
	int mul = 1;
	for (i = from; i < to-2; i += 2)
	{
		dst[i]   = mul*(src[i] - src[i+2]);
		dst[i+1] = mul*(src[i+1] - src[i+3]);
		mul *= -1;
	}

	dst[to-2] = src[to-2] - a0;
	dst[to-1] = src[to-1] - a1;
}


/* Эмуляция сигнала спинового эхо и запись его в буфер dst
 * В структуре proc_params:
 *  - длина эмулированного массива proc_params->points_count (числа в массиве - целые двухбайтные)
 *  - номер спинового эхо proc_params->current_echo (чем больше номер, тем меньше амплитуда эхо в соответствии с модельным экспоненциальным затуханием сигнала спинового эхо)
 */
void emulate_EchoData(uint8_t *dst, Processing_Params *proc_params)
{
	int len = proc_params->points_count;
	if (len > NMR_DATA_LEN) len = NMR_DATA_LEN;
	int index = proc_params->current_echo;
	if (index < 0) index = 0;
	if (index > 1000) index = 1000;

	float mul = relax_ampls[index]/8.0;

	int shift = (NMR_DATA_LEN - len)/2;
	int i;
	for (i = 0; i < len; i++)
	{
		int16_t tab_echo = echo_ui16_60mks[i+shift] - 2048;
		uint16_t y = (uint16_t)(mul*tab_echo + 2048);

		//printf("%d ", y);

		dst[2 * i] = (uint8_t) (y & 0x00FF);
		dst[2 * i + 1] = (uint8_t) (y >> 8);
	}
}

/* Эмуляция сигнала ССИ и запись его в буфер dst
 * В структуре proc_params:
 *  - длина эмулированного массива proc_params->points_count (числа в массиве - целые двухбайтные)
 */
void emulate_FIDData(uint8_t *dst, Processing_Params *proc_params)
{
	int len = proc_params->points_count;

	int shift = NMR_DATA_LEN/2;
	int i;
	for (i = 0; i < len; i++)
	{
		int16_t tab_fid = 0;
		if (i+shift < NMR_DATA_LEN) tab_fid = echo_ui16_60mks[i+shift] - 2048;
		uint16_t y = (uint16_t)(tab_fid + 2048);

		dst[2 * i] = (uint8_t) (y & 0x00FF);
		dst[2 * i + 1] = (uint8_t) (y >> 8);
	}
	//printf("%d, %d\n", shift, len);
}

/* Эмуляция шума и запись его в буфер dst
 * В структуре proc_params:
 *  - длина эмулированного массива proc_params->points_count (числа в массиве - целые двухбайтные)
 */
void emulate_NoiseData(uint8_t *dst, Processing_Params *proc_params)
{
	int len = proc_params->points_count;

	int shift = rand()/(float)RAND_MAX*2048;

	int i;
	for (i = 0; i < len; i++)
	{
		int16_t tab_noise = 0;
		if (i+shift < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift] - 2048;
		else tab_noise = noise_ui16[i] - 2048;
		uint16_t y = (uint16_t)(tab_noise + 2048);

		dst[2 * i] = (uint8_t) (y & 0x00FF);
		dst[2 * i + 1] = (uint8_t) (y >> 8);
	}
}

/* Эмуляция шума и запись его в буфер dst
 * В структуре proc_params:
 *  - длина эмулированного массива proc_params->points_count (числа в массиве - целые двухбайтные)
 * В структуре instr:
 *  - требуемое отношение сигнал/шум
 */
void emulate_FIDNoiseData(Data_Cmd *instr, uint8_t *dst, Processing_Params *proc_params)
{
	if (instr->count != 1) return;

	int len = proc_params->points_count;
	float sn_ratio = instr->params[0];

	int shift_ns = rand()/(float)RAND_MAX*2048;
	int shift = NMR_DATA_LEN/2;

	int i;
	for (i = 0; i < len; i++)
	{
		int16_t tab_noise = 0;
		if (i+shift_ns < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift_ns] - 2048;
		else tab_noise = noise_ui16[i] - 2048;

		int16_t tab_fid = 0;
		if (i+shift < NMR_DATA_LEN) tab_fid = echo_ui16_60mks[i+shift] - 2048;

		uint16_t y = (uint16_t)((tab_fid*sn_ratio) + tab_noise + 2048);

		dst[2 * i] = (uint8_t) (y & 0x00FF);
		dst[2 * i + 1] = (uint8_t) (y >> 8);
	}
}

void emulate_EchoNoiseData(Data_Cmd *instr, uint8_t *dst, Processing_Params *proc_params)
{
	if (instr->count != 1) return;

	int len = proc_params->points_count;
	if (len > NMR_DATA_LEN) len = NMR_DATA_LEN;

	int index = proc_params->current_echo;
	if (index < 0) index = 0;
	if (index > 1000) index = 1000;
	float mul = relax_ampls[index]/32.0/3.0;		// "3" = 3 sigma

	float sn_ratio = instr->params[0];

	int shift_ns = rand()/(float)RAND_MAX*2048;
	int shift = (NMR_DATA_LEN - len)/2;

	int i;
	for (i = 0; i < len; i++)
	{
		int16_t tab_noise = 0;
		if (i+shift_ns < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift_ns] - 2048;
		else tab_noise = noise_ui16[i] - 2048;

		int16_t tab_echo = echo_ui16_60mks[i+shift] - 2048;
		uint16_t y = (uint16_t)((tab_echo*sn_ratio*mul) + tab_noise + 2048);

		//printf("%d ", y);

		dst[2 * i] = (uint8_t) (y & 0x00FF);
		dst[2 * i + 1] = (uint8_t) (y >> 8);
	}
}


void clearDataHeap(float **hx_bank, int *hx_len, Data_Cmd *instr)
{
	if (instr->count != 2) return;

	int hx_from = (int)instr->params[0];		// откуда очищать буфер
	int hx_to = (int)instr->params[1];			// докуда очищать буфер

	int i;
	for (i = hx_from; i < hx_to; i++)
	{
		float *data = hx_bank[i];
		memset(data, 0x00, DATA_MAX_LEN*sizeof(float));
	}
	memset(hx_len+hx_from, 0x00, (hx_to-hx_from)*sizeof(int));
}

// Источник данных: upp_data
// в качестве промежуточного буфера используется ptr_data_org = data_bank[0] (соответствует D0)
// результат - приведенные данные в буфере, код которого - параметр в instr
void noiseUppDataPreprocessing1(uint8_t *upp_data, float **data_bank, float **hx_bank, int *hx_len, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (instr->count != 3) return;

	int data_src = (int)instr->params[0];		// источник данных (эмулятор = 0, АЦП = 1)
	int data_dst = (int)instr->params[1];		// приемник данных (D1 = 0, hx_bank = 1)
	int N = (int)instr->params[2];				// количество накоплений

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	//clock_t t_start, t_stop, t_overhead;
	//t_start = clock();
	//t_stop = clock();
	//t_overhead = t_stop - t_start;
	//t_start = clock();

	float *data1 = data_bank[1];
	float *temp_data = data_bank[8];
	memset(temp_data, 0x00, DATA_MAX_LEN*sizeof(float));

	int i;

	// ------------ эмуляция данных шума -------------
	if (data_src == 0)
	{
		int shift = rand()/(float)RAND_MAX*2048;
		for (i = 0; i < src_len; i++)
		{
			int16_t tab_noise = 0;
			if (i+shift < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift] - 2048;
			else tab_noise = noise_ui16[i] - 2048;
			uint16_t y = (uint16_t)(tab_noise + 2048);

			upp_data[2 * i] = (uint8_t) (y & 0x00FF);
			upp_data[2 * i + 1] = (uint8_t) (y >> 8);
		}
	}
	// ------------------------------------------------

	// ------------- приведение данных ----------------
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

	float S = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)upp_data[2*i + src_pre];
		uint16_t x2 = (uint16_t)upp_data[2*i+1 + src_pre];
		uint16_t x = (x1 | (x2 << 8));

		S += x;
		temp_data[index] = (float)x;
		index += 2;
	}
	S /= len;

	memset(temp_data, 0x0, dst_pre*sizeof(float));
	memset(temp_data+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = temp_data[index];
		temp_data[index++] = (x - S)*ADC_to_mV;
		temp_data[index++] = 0;
	}

	// ----- Запись результата в приемник данных ------
	//float invN = Q_rsqrt(1.0/N);
	float invN = 1.0/N;
	float *dst = data1;
	if (data_dst == 1)
	{
		int index_hx = proc_params->current_echo;
		hx_len[index_hx-1] = index_hx;
		dst = hx_bank[index_hx-1];
		for (i = 0; i < DATA_MAX_LEN; i++)
		{
			dst[i] += temp_data[i]*invN;
		}
	}
	/*
	else if (data_dst == 0)
	{
		int NN = proc_params->current_echo;
		if (NN > 0)
		{
			for (i = 0; i < DATA_MAX_LEN; i++)
			{
				dst[i] = (NN-1)*dst[i]/NN + temp_data[i]/NN;
				dst[i] *= invN;
			}
		}
	}
	*/
	// ------------------------------------------------

	//t_stop = clock();
	//printf("\t NMR data processing time: %d clock cycles\n", (t_stop - t_start) - t_overhead);

	proc_params->points_count = 2*src_len;
}

void signalUppDataPreprocessing1(uint8_t *upp_data, float **data_bank, float **hx_bank, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (instr->count != 3) return;

	int data_src = (int)instr->params[0];		// источник данных (эмулятор = 0, АЦП = 1)
	int N = (int)instr->params[1];				// число накоплений
	int sn_ratio = (int)instr->params[2];		// отношение сигнал/шум

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	//clock_t t_start, t_stop, t_overhead;
	//t_start = clock();
	//t_stop = clock();
	//t_overhead = t_stop - t_start;
	//t_start = clock();

	float *data1 = data_bank[1];
	float *temp_data = data_bank[8];
	memset(temp_data, 0x00, NMR_DATA_LEN*sizeof(float));

	int i;

	// ----------- эмулляция данных сигнала -----------
	if (data_src == 0)
	{
		if (src_len > NMR_DATA_LEN) src_len = NMR_DATA_LEN;

		int index = proc_params->current_echo;
		if (index < 0) index = 0;
		if (index > 1000) index = 1000;
		float mul = relax_ampls[index]/32.0/3.0;		// "3" = 3 sigma

		int shift_ns = rand()/(float)RAND_MAX*2048;
		int shift = (NMR_DATA_LEN - src_len)/2;

		for (i = 0; i < src_len; i++)
		{
			int16_t tab_noise = 0;
			if (i+shift_ns < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift_ns] - 2048;
			else tab_noise = noise_ui16[i] - 2048;

			int16_t tab_echo = echo_ui16_60mks[i+shift] - 2048;
			uint16_t y = (uint16_t)((tab_echo*sn_ratio*mul) + tab_noise + 2048);

			upp_data[2 * i] = (uint8_t) (y & 0x00FF);
			upp_data[2 * i + 1] = (uint8_t) (y >> 8);
		}
	}
	// ------------------------------------------------

	// ------------- приведение данных ----------------
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

	float S = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)upp_data[2*i + src_pre];
		uint16_t x2 = (uint16_t)upp_data[2*i+1 + src_pre];
		uint16_t x = (x1 | (x2 << 8));

		S += x;
		temp_data[index] = (float)x;
		index += 2;
	}
	S /= len;

	memset(temp_data, 0x0, dst_pre*sizeof(float));
	memset(temp_data+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = temp_data[index];
		temp_data[index++] = (x - S)*ADC_to_mV;
		temp_data[index++] = 0;
	}
	// ------------------------------------------------

	// ----- Запись результата в приемник данных ------
	//float invN = Q_rsqrt(1.0/N);
	float invN = 1.0/N;
	//float *dst = data1;
	int index_hx = proc_params->current_echo;
	float *dst = hx_bank[index_hx-1];
	for (i = 0; i < DATA_MAX_LEN; i++)
	{
		dst[i] += temp_data[i]*invN;
	}
	// ------------------------------------------------

	proc_params->points_count = 2*src_len;

	//t_stop = clock();
	//printf("\t NMR data processing time: %d clock cycles\n", (t_stop - t_start) - t_overhead);
}

void signal_noise_UppDataPreprocessing3(uint8_t *upp_data, float **data_bank, float **hx_bank, int *hx_len, Processing_Params *proc_params, Data_Cmd *instr)
{
	if (instr->count != 6) return;

	int data_src = (int)instr->params[0];		// источник данных (эмулятор = 0, АЦП = 1)
	int NN = (int)instr->params[1];				// число окон шума
	int N = (int)instr->params[2];				// число накоплений
	int sn_ratio = (int)instr->params[3];		// отношение сигнал/шум
	int win_aver = (int)instr->params[4];		// параметр, определяющий метод накопления сигнала: 0 - копить во временной области столько же раз, сколько и шум;
												// 													1 - копить соседние эхо сигнала в пределах окна шириной N
	float a = (float)instr->params[5]/1000.0;	// параметр, на который необходимо домножать первое эхо сигнала
	if (a == 0) a = 1;
	if (proc_params->current_echo != NN + 1) a = 1; // домножается на a только первое эхо сигнала

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	//clock_t t_start, t_stop, t_overhead;
	//t_start = clock();
	//t_stop = clock();
	//t_overhead = t_stop - t_start;
	//t_start = clock();

	float *temp_data = data_bank[8];
	memset(temp_data, 0x00, NMR_DATA_LEN*sizeof(float));

	int i;
	int current_echo = proc_params->current_echo;

	// ------------ эмуляция данных шума -------------
	if (data_src == 0 && current_echo <= NN)
	{
		int shift = rand()/(float)RAND_MAX*2048;
		for (i = 0; i < src_len; i++)
		{
			int16_t tab_noise = 0;
			if (i+shift < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift] - 2048;
			else tab_noise = noise_ui16[i] - 2048;
			uint16_t y = (uint16_t)(tab_noise + 2048);

			upp_data[2 * i] = (uint8_t) (y & 0x00FF);
			upp_data[2 * i + 1] = (uint8_t) (y >> 8);
		}
	}
	// ----------- эмулляция данных сигнала -----------
	else if (data_src == 0 && current_echo > NN)
	{
		if (src_len > NMR_DATA_LEN) src_len = NMR_DATA_LEN;

		int index = current_echo - NN;
		if (index < 1) index = 1;
		if (index > 1000) index = 1000;
		float mul = relax_ampls[index-1]/32.0/3.0;		// "3" = 3 sigma

		int shift_ns = rand()/(float)RAND_MAX*2048;
		int shift = (NMR_DATA_LEN - src_len)/2;

		for (i = 0; i < src_len; i++)
		{
			int16_t tab_noise = 0;
			if (i+shift_ns < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift_ns] - 2048;
			else tab_noise = noise_ui16[i] - 2048;

			int16_t tab_echo = echo_ui16_60mks[i+shift] - 2048;
			uint16_t y = (uint16_t)((tab_echo*sn_ratio*mul) + tab_noise + 2048);

			upp_data[2 * i] = (uint8_t) (y & 0x00FF);
			upp_data[2 * i + 1] = (uint8_t) (y >> 8);
		}
	}
	// ------------------------------------------------

	// ------------- приведение данных ----------------
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

	float S = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)upp_data[2*i + src_pre];
		uint16_t x2 = (uint16_t)upp_data[2*i+1 + src_pre];
		uint16_t x = (x1 | (x2 << 8));

		S += x;
		temp_data[index] = (float)x;
		index += 2;
	}
	S /= len;

	memset(temp_data, 0x0, dst_pre*sizeof(float));
	memset(temp_data+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = temp_data[index];
		temp_data[index++] = a*(x - S)*ADC_to_mV;
		temp_data[index++] = 0;
	}
	// ------------------------------------------------

	// ----- Запись результата в приемник данных ------
	//float invN = Q_rsqrt(1.0/N);
	//float invN = 1.0/N;
	if (win_aver == 0)
	{
		int index_hx = proc_params->current_echo;
		float winN = hx_len[index_hx-1];
		float inv_winN = 1.0/(hx_len[index_hx-1] + 1);

		float *dst = hx_bank[index_hx-1];
		hx_len[index_hx-1]++;
		for (i = 0; i < DATA_MAX_LEN; i++)
		{
			//dst[i] += temp_data[i]*invN;
			dst[i] = winN*dst[i]*inv_winN + temp_data[i]*inv_winN;
		}
	}
	else
	{
		if (current_echo <= NN)
		{
			int winN = (current_echo-1) % N;
			int index_hx = (current_echo-1)/N;

			float inv_winN = 1.0/(winN+1);

			hx_len[index_hx]++;
			float *dst = hx_bank[index_hx];
			for (i = 0; i < DATA_MAX_LEN; i++)
			{
				dst[i] = winN*dst[i]*inv_winN + temp_data[i]*inv_winN;
			}
		}
		else
		{
			//int winN = (current_echo - NN - 1) % N;
			int sh = 0;
			if (NN % N > 0) sh = 1;
			int index_hx = NN/N + (current_echo - NN - 1)/N + sh;	// ???

			int winN = hx_len[index_hx];
			float inv_winN = 1.0/(winN+1);

			hx_len[index_hx]++;
			float *dst = hx_bank[index_hx];
			for (i = 0; i < DATA_MAX_LEN; i++)
			{
				dst[i] = winN*dst[i]*inv_winN + temp_data[i]*inv_winN;
			}
		}
	}
	// ------------------------------------------------

	proc_params->points_count = 2*src_len;

	//t_stop = clock();
	//printf("\t NMR data processing time: %d clock cycles\n", (t_stop - t_start) - t_overhead);
}


// Источник данных: upp_data или data_heap (hx)
// в качестве промежуточного буфера используется ptr_data_org = data_bank[0] (соответствует D0)
// результат - накопленных шум в ptr_data2 = data_bank[2] (соответствует D2) либо data_heap (hx)
void noiseProcessing2(float **data_bank, float **hx_bank, int rad, Data_Cmd *instr, Processing_Params *proc_params, OutBuffer *out_buff)
{
	if (instr->count != 5) return;

	int NN = (int)instr->params[0];				// количество окон шума
	int data_src = (int)instr->params[1];		// источник данных (D1 = 0, hx_data = 1)
	int data_dst = (int)instr->params[2];		// приемник данных (D2 = 0, hx_data = 1)
	int out_time_data = (int)instr->params[3];	// выводить/не выводить накопленные во временной области данные шума
	int out_spec_data = (int)instr->params[4];	// выводить/не выводить спектр мощности накопленного шума после: мощностной спектр фурье, наложение окна в спектральной области

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	//clock_t t_start, t_stop, t_overhead;
	//t_start = clock();
	//t_stop = clock();
	//t_overhead = t_stop - t_start;
	//t_start = clock();

	float *data0 = data_bank[0];
	float *data1 = data_bank[1];
	float *data2 = data_bank[2];
	float *temp_data = data_bank[8];
	float *ptr_w = data_bank[9];

	//memset(data2, 0x00, DATA_MAX_LEN*sizeof(float));

	int i = 0;
	int j = 0;
	for (j = 0; j < NN; j++)
	{
		if (data_src == 0) memcpy(data0, data1, DATA_MAX_LEN*sizeof(float));
		else if (data_src == 1) memcpy(data0, hx_bank[j], DATA_MAX_LEN*sizeof(float));
		else return;

		// ----- оконная функция во временной области -----
		uint8_t func_id = 0;
		int x0 = 0;
		int sigma = 256;

		func_id = proc_params->echo_func;
		x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
		sigma = proc_params->echo_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i+= 2)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data0[i] *= a;
				data0[i+1] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);
			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data0[0] = tab[index]*data0[0];
				data0[1] = tab[index]*data0[1];
			}
			else
			{
				data0[0] = 0;
				data0[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
			    index += 2*step;
			    if (counter++ >= d0 && dindex != 0)
			    {
			        index += 2*dindex;
			        counter = 1;
			    }
			    if (index >= 0 && index < REF_DATA_LEN)
			    {
			    	data0[i] = tab[index]*data0[i];
			    	data0[i+1] = tab[index]*data0[i+1];
			    }
			    else
			    {
			    	data0[i] = 0;
			    	data0[i+1] = 0;
			    }
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// - копирование в выходной буфер временнЫх данных
		if (out_time_data > 0 && j == 0)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = proc_params->points_count/2;
			int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
			if (pre_pos < 0) pre_pos = 0;
			for (i = 0; i < src_len; i++) dst[dst_pos+i] = data0[pre_pos+2*i];
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_NS_SE;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		// --------- быстрое преобразование Фурье ---------
		memcpy(temp_data - PAD, data0 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
		DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data0, brev, rad, 0, CMPLX_DATA_MAX_LEN);
		// ------------------------------------------------

		// --------- вычисление спектра мощности ----------
		int index = 0;
		for (i = 0; i < DATA_MAX_LEN; i += 2)
		{
			data0[index++] = data0[i]*data0[i] + data0[i+1]*data0[i+1];	// without sqrt !
		}
		// ------------------------------------------------

		// ----- оконная функция в частотной области ------
		func_id = proc_params->spectr_func;
		x0 = proc_params->spectr_x0;
		sigma = proc_params->spectr_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i++)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data0[i] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);

			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data0[0] *= tab[index];
				data0[1] *= tab[index];
			}
			else
			{
				data0[0] = 0;
				data0[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
			    index += 2*step;
			    if (counter++ >= d0 && dindex != 0)
			    {
			        index += 2*dindex;
			        counter = 1;
			    }
			    if (index >= 0 && index < REF_DATA_LEN)
			    {
			    	data0[i] *= tab[index];
			    	data0[i+1] *= tab[index];
			    }
			    else
			    {
			    	data0[i] = 0;
			    	data0[i+1] = 0;
			    }
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		if (data_dst == 0)
		{
		// ---------- Накопление шума (в data2) -----------
			if (j < 1) memcpy(data2, data0, DATA_MAX_LEN*sizeof(float));
			else
			{
				float invJ2 = 1.0/(j+1);
				float invJ1 = j*invJ2;
				for (i = 0; i < DATA_MAX_LEN; i++)
				{
					data2[i] = invJ1*data2[i] + invJ2*data0[i];
				}
			}
		// ------------------------------------------------
		}
		else if (data_dst == 1)
		{
			// помещение спектров в hx_bank
			memcpy(hx_bank[j], data0, DATA_MAX_LEN*sizeof(float));
			// ------------------------------------------------
		}
	}

	// помещение в выходной буфер накопленного частотного спектра мощности
	if (out_spec_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		float *data = 0;
		if (data_dst == 0) data = data2;
		else if (data_dst == 1) data = hx_bank[0];
		else return;

		int src_len = DATA_MAX_LEN/4;
		if (data_dst == 0) memcpy(dst+dst_pos, data, src_len*sizeof(float));
		else if (data_dst == 1) memcpy(dst+dst_pos, hx_bank[0], src_len*sizeof(float));
		else return;
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_NS_FFT_SE_AM;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	//t_stop = clock();
	//printf("\t NMR data processing time: %d clock cycles\n", (t_stop - t_start) - t_overhead);
}


// Источник данных: upp_data
// в качестве промежуточного буфера используется ptr_data_org = data_bank[0] (соответствует D0)
// результат - накопленных шум в ptr_data2 = data_bank[2] (соответствует D2)
void signalProcessing2(float **data_bank, float **hx_bank, int rad, Data_Cmd *instr, Processing_Params *proc_params, SummationBuffer *sum_buff, OutBuffer *out_buff)
{
	if (instr->count != 5) return;

	int NN = (int)instr->params[0];				// количество окон сигнала
	int data_src = (int)instr->params[1];		// источник данных (D1 = 0, hx_data = 1)
	int proc_method = (int)instr->params[2];	// метод вычистания спектра шума из спектра сигнала (0 - не вычитать спектр шума; 1 - метод В.Д.; вычитание корня квадратного из спектра шума из корня квадратного спектра сигнала)
	int out_time_data = (int)instr->params[3];	// выводить/не выводить накопленные во временной области данные АЦП
	int out_spec_data = (int)instr->params[4];	// выводить/не выводить спектр мощности сигнала после: мощностной спектр фурье, наложение окна в спектральной области

	//int current_echo = proc_params->current_echo;
	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	float *data0 = data_bank[0];
	float *data1 = data_bank[1];
	float *data2 = data_bank[2];
	float *temp_data = data_bank[8];
	float *ptr_w = data_bank[9];

	if (data_src == 0) memcpy(data0, data1, DATA_MAX_LEN*sizeof(float));
	int i = 0;
	int j = 0;
	for (j = 0; j < NN; j++)
	{
		int current_echo = j+1;

		if (data_src == 1) memcpy(data0, hx_bank[j], DATA_MAX_LEN*sizeof(float));
		else return;

		// ----- оконная функция во временной области -----
		uint8_t func_id = 0;
		int x0 = 0;
		int sigma = 256;

		func_id = proc_params->echo_func;
		x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
		sigma = proc_params->echo_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i+= 2)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data0[i] *= a;
				data0[i+1] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);
			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data0[0] = tab[index]*data0[0];
				data0[1] = tab[index]*data0[1];
			}
			else
			{
				data0[0] = 0;
				data0[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
				index += 2*step;
				if (counter++ >= d0 && dindex != 0)
				{
					index += 2*dindex;
					counter = 1;
				}
				if (index >= 0 && index < REF_DATA_LEN)
				{
					data0[i] = tab[index]*data0[i];
					data0[i+1] = tab[index]*data0[i+1];
				}
				else
				{
					data0[i] = 0;
					data0[i+1] = 0;
				}
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// - копирование в выходной буфер временнЫх данных
		if (out_time_data > 0 && current_echo == 1)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = proc_params->points_count/2;
			int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
			if (pre_pos < 0) pre_pos = 0;
			for (i = 0; i < src_len; i++) dst[dst_pos+i] = data0[pre_pos+2*i];
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_SGN_SE;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		// --------- быстрое преобразование Фурье ---------
		memcpy(temp_data - PAD, data0 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
		DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data0, brev, rad, 0, CMPLX_DATA_MAX_LEN);
		// ------------------------------------------------

		// --------- вычисление спектра мощности ----------
		int index = 0;
		for (i = 0; i < DATA_MAX_LEN; i += 2)
		{
			data0[index++] = data0[i]*data0[i] + data0[i+1]*data0[i+1];	// power spectrum
			//data0[index++] = Q_rsqrt(data0[i]*data0[i] + data0[i+1]*data0[i+1]);
		}
		// ------------------------------------------------

		// ----- оконная функция в частотной области ------
		func_id = proc_params->spectr_func;
		x0 = proc_params->spectr_x0;
		sigma = proc_params->spectr_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i++)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data0[i] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);

			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data0[0] *= tab[index];
				data0[1] *= tab[index];
			}
			else
			{
				data0[0] = 0;
				data0[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
				index += 2*step;
				if (counter++ >= d0 && dindex != 0)
				{
					index += 2*dindex;
					counter = 1;
				}
				if (index >= 0 && index < REF_DATA_LEN)
				{
					data0[i] *= tab[index];
					data0[i+1] *= tab[index];
				}
				else
				{
					data0[i] = 0;
					data0[i+1] = 0;
				}
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// вычисление спектра сигнала по алгоритму Скирды В.Д.
		float val = 0;
		int sgn = 0;
		switch (proc_method)
		{
		case 0:		// don't remove noise spectrum
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(data0[i]);
			}
			break;
		case 1:		// method V.D.
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				val = data0[i] - data2[i];
				if (val >= 0) sgn = 1; else sgn = -1;
				val = fabs(val);
				data0[i] = sgn*Q_rsqrt(val);
			}
			break;
		case 2:		// square root of signal power spectrum minus square root of noise power spectrum
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(fabs(data0[i])) - Q_rsqrt(fabs(data2[i]));
			}
			break;
		case 3:	// square root of noise power spectrum
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(data2[i]);
			}
			break;
		default:
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(fabs(data0[i])) - Q_rsqrt(fabs(data2[i]));
			}
			break;
		}
		// ------------------------------------------------

		// помещение в выходной буфер частотного спектра мощности сигнала
		// минус накопленный спктр мощности шума (алгоритм В.Д.Скирды)
		if (out_spec_data > 0  && current_echo == 1)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = DATA_MAX_LEN/4;
			memcpy(dst+dst_pos, data0, src_len*sizeof(float));
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_SGN_FFT_SE_AM;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		// --- Накопление интеграла эхо в буфере сумирования --
		sum_buff->data_id = DT_SGN_RELAX;
		sum_buff->channel_id = proc_params->channel_id; // proger_rd_ch_number();
		float S = 0;
		for (i = 0; i < DATA_MAX_LEN/4; i++) S += data0[i];

		//index = proc_params->current_echo - 1;
		index = j;
		if (index >= sum_buff->max_size) return;
		if (index < 0) index = 0;

		sum_buff->sum_data[index] = S;
		sum_buff->pos = index+1;
		// ------------------------------------------------
	}
}

void signalProcessing3(float **data_bank, float **hx_bank, int rad, Data_Cmd *instr, Processing_Params *proc_params, SummationBuffer *sum_buff, OutBuffer *out_buff)
{
	/*clock_t t_start, t_stop, t_overhead;
	t_start = clock();
	t_stop = clock();
	t_overhead = t_stop - t_start;
	t_start = clock();*/

	if (instr->count != 9) return;

	int NN = (int)instr->params[0];				// количество окон шума
	int NS = (int)instr->params[1];				// количество окон сигнала
	int N = (int)instr->params[2];				// число накоплений
	int proc_method = (int)instr->params[3];	// метод вычитания спектра шума из спектра сигнала (0 - не вычитать спектр шума; 1 - метод В.Д.; 2 - вычитание корня квадратного из спектра шума из корня квадратного спектра сигнала)
	int out_time_data = (int)instr->params[4];	// выводить/не выводить накопленные во временной области данные АЦП
	int out_spec_data = (int)instr->params[5];	// выводить/не выводить спектр мощности сигнала после: мощностной спектр фурье, наложение окна в спектральной области
	int win_aver = (int)instr->params[6];		// параметр, определяющий метод накопления сигнала: 0 - копить во временной области столько же раз, сколько и шум;
												// 													1 - копить соседние эхо сигнала и шума в пределах окна шириной N
	int group_index = (int)instr->params[7];	// групповой индекс
	int probe_number = (int)instr->params[8];	// номер датчика ЯМР

	uint8_t data_code = DT_SGN_RELAX;
	switch (probe_number)
	{
	case 2:	data_code = DT_SGN_RELAX2; break;
	case 3: data_code = DT_SGN_RELAX3; break;
	default: data_code = DT_SGN_RELAX; break;
	}

	//int current_echo = proc_params->current_echo;
	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	float *data0 = data_bank[0];
	float *data1 = data_bank[1];
	float *data2 = data_bank[2];
	float *temp_data = data_bank[8];
	float *ptr_w = data_bank[9];

	int i = 0;
	int j = 0;

	int NNN = NN;
	if (win_aver > 0)
	{
		if (NN % N > 0) NNN = NN/N + 1;
		else NNN = NN/N;
	}

	float SNs = 0;
	int noise_echo = 0;
	for (j = 0; j < NNN; j++)
	{
		memcpy(data2, hx_bank[noise_echo], DATA_MAX_LEN*sizeof(float));

		// ----- оконная функция во временной области -----
		uint8_t func_id = 0;
		int x0 = 0;
		int sigma = 256;

		func_id = proc_params->echo_func;
		x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
		sigma = proc_params->echo_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i+= 2)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data2[i] *= a;
				data2[i+1] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);
			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data2[0] = tab[index]*data2[0];
				data2[1] = tab[index]*data2[1];
			}
			else
			{
				data2[0] = 0;
				data2[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
				index += 2*step;
				if (counter++ >= d0 && dindex != 0)
				{
					index += 2*dindex;
					counter = 1;
				}
				if (index >= 0 && index < REF_DATA_LEN)
				{
					data2[i] = tab[index]*data2[i];
					data2[i+1] = tab[index]*data2[i+1];
				}
				else
				{
					data2[i] = 0;
					data2[i+1] = 0;
				}
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// - копирование в выходной буфер временнЫх данных накопленного шума
		if (out_time_data > 0 && j == 0)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = proc_params->points_count/2;
			int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
			if (pre_pos < 0) pre_pos = 0;
			for (i = 0; i < src_len; i++) dst[dst_pos+i] = data2[pre_pos+2*i];
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_NS_SE;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		// --------- быстрое преобразование Фурье ---------
		memcpy(temp_data - PAD, data2 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
		DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data2, brev, rad, 0, CMPLX_DATA_MAX_LEN);
		// ------------------------------------------------

		// ------- вычисление спектра мощности шума -------
		int index = 0;
		//float temp_data2[2048];
		//memcpy(&temp_data2[0], data2, 2048*sizeof(float));
		for (i = 0; i < DATA_MAX_LEN; i += 2)
		{
			data2[index++] = data2[i]*data2[i] + data2[i+1]*data2[i+1];	// power spectrum
		}
		//memcpy(&temp_data2[0], data2, 2048*sizeof(float));
		// ------------------------------------------------

		// ----- оконная функция в частотной области ------
		func_id = proc_params->spectr_func;
		x0 = proc_params->spectr_x0;
		sigma = proc_params->spectr_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i++)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data2[i] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);

			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data2[0] *= tab[index];
				data2[1] *= tab[index];
			}
			else
			{
				data2[0] = 0;
				data2[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
				index += 2*step;
				if (counter++ >= d0 && dindex != 0)
				{
					index += 2*dindex;
					counter = 1;
				}
				if (index >= 0 && index < REF_DATA_LEN)
				{
					data2[i] *= tab[index];
					data2[i+1] *= tab[index];
				}
				else
				{
					data2[i] = 0;
					data2[i+1] = 0;
				}
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// помещение в выходной буфер частотного спектра мощности шума
		if (out_spec_data > 0  && j == 0)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = DATA_MAX_LEN/4;
			//memcpy(dst+dst_pos, data0, src_len*sizeof(float));
			for (i = 0; i < src_len; i++)
			{
				*(dst+dst_pos+i) = Q_rsqrt(data2[i]);
			}
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_NS_FFT_SE_AM;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		for (i = 0; i < DATA_MAX_LEN/4; i++)
		{
			SNs += Q_rsqrt(data2[i]);
		}
		//SNs /= 512.0;

		memcpy(hx_bank[noise_echo], data2, DATA_MAX_LEN*sizeof(float));

		noise_echo++;
	}
	SNs = SNs/NNN;

	noise_echo = 0;
	int signal_echo = NNN;
	int NNS = NS;
	if (win_aver > 0)
	{
		if (NS % N > 0) NNS = NS/N + 1;
		else NNS = NS/N;
	}

	for (j = 0; j < NNS; j++)
	{
		if (noise_echo >= NNN) noise_echo = 0;

		memcpy(data0, hx_bank[signal_echo], DATA_MAX_LEN*sizeof(float));
		memcpy(data2, hx_bank[noise_echo], DATA_MAX_LEN*sizeof(float));

		// ----- оконная функция во временной области -----
		uint8_t func_id = 0;
		int x0 = 0;
		int sigma = 256;

		func_id = proc_params->echo_func;
		x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
		sigma = proc_params->echo_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN; i+= 2)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data0[i] *= a;
				data0[i+1] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);
			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data0[0] = tab[index]*data0[0];
				data0[1] = tab[index]*data0[1];
			}
			else
			{
				data0[0] = 0;
				data0[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
				index += 2*step;
				if (counter++ >= d0 && dindex != 0)
				{
					index += 2*dindex;
					counter = 1;
				}
				if (index >= 0 && index < REF_DATA_LEN)
				{
					data0[i] = tab[index]*data0[i];
					data0[i+1] = tab[index]*data0[i+1];
				}
				else
				{
					data0[i] = 0;
					data0[i+1] = 0;
				}
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// - копирование в выходной буфер временнЫх данных
		if (out_time_data > 0 && j == 0)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = proc_params->points_count/2;
			int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
			if (pre_pos < 0) pre_pos = 0;
			for (i = 0; i < src_len; i++) dst[dst_pos+i] = data0[pre_pos+2*i];
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_SGN_SE;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		// --------- быстрое преобразование Фурье ---------
		memcpy(temp_data - PAD, data0 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
		DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data0, brev, rad, 0, CMPLX_DATA_MAX_LEN);
		// ------------------------------------------------

		// --------- вычисление спектра мощности ----------
		int index = 0;
		for (i = 0; i < DATA_MAX_LEN; i += 2)
		{
			data0[index++] = (data0[i]*data0[i] + data0[i+1]*data0[i+1]);	// power spectrum
		}
		memset(data0+index, 0x0, DATA_MAX_LEN/2*sizeof(float));				// second part of the power spectrum is zero
		// ------------------------------------------------

		// ----- оконная функция в частотной области ------
		func_id = proc_params->spectr_func;
		x0 = proc_params->spectr_x0;
		sigma = proc_params->spectr_sigma;

		switch (func_id)
		{
		case NoFunc: 				// No win-function
			break;
		case Func_Rect: 			// Rectangular function
		{
			int width = sigma>>1;
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				int a = 0;
				if (i < x0 && i > x0-width ) a = 1;
				else if (i > x0 && i < x0+width ) a = 1;
				data0[i] *= a;
			}
			break;
		}
		case Func_Gauss:
		case Func_BiGauss:
		{
			float *tab;
			if (func_id == 2) tab = &gauss_tab[0];
			else tab = &bigauss_tab[0];

			int nod = Q_NOD(REF_SIGMA, sigma);
			int D0 = REF_SIGMA/nod;
			int d0 = sigma/nod;

			int step = round(D0/d0);

			int dindex = D0 % d0;
			if (step != D0/d0) dindex -= d0;

			int ref_from = REF_X0 - (x0*D0)/d0;

			int index = ref_from;
			if (index >= 0)
			{
				data0[0] *= tab[index];
				data0[1] *= tab[index];
			}
			else
			{
				data0[0] = 0;
				data0[1] = 0;
			}
			int counter = 1;
			for (i = 2; i < DATA_MAX_LEN; i+=2)
			{
				index += 2*step;
				if (counter++ >= d0 && dindex != 0)
				{
					index += 2*dindex;
					counter = 1;
				}
				if (index >= 0 && index < REF_DATA_LEN)
				{
					data0[i] *= tab[index];
					data0[i+1] *= tab[index];
				}
				else
				{
					data0[i] = 0;
					data0[i+1] = 0;
				}
			}
			break;
		}
		default: break;
		}
		// ------------------------------------------------

		// вычисление спектра сигнала по алгоритму Скирды В.Д.
		float val = 0;
		int sgn = 0;
		switch (proc_method)
		{
		case 0:		// don't remove noise spectrum
		case 4:
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(data0[i]);
			}
			break;
		case 1:		// method V.D.
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				val = data0[i] - data2[i];
				if (val >= 0) sgn = 1; else sgn = -1;
				val = fabs(val);
				data0[i] = sgn*Q_rsqrt(val);
			}
			break;
		case 2:		// square root of signal power spectrum minus square root of noise power spectrum
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(fabs(data0[i])) - Q_rsqrt(fabs(data2[i]));
			}
			break;
		case 3:	// square root of noise power spectrum
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(data2[i]);
			}
			break;
		default:
			for (i = 0; i < DATA_MAX_LEN/2; i++)
			{
				data0[i] = Q_rsqrt(fabs(data0[i])) - Q_rsqrt(fabs(data2[i]));
			}
			break;
		}
		// ------------------------------------------------

		// помещение в выходной буфер частотного спектра мощности сигнала
		// минус накопленный спктр мощности шума (алгоритм В.Д.Скирды)
		if (out_spec_data > 0  && j == 0)
		{
			float *dst = out_buff->out_data;
			int dst_pos = out_buff->full_size;
			int data_cnt = out_buff->outdata_counter;

			int src_len = DATA_MAX_LEN/4;
			memcpy(dst+dst_pos, data0, src_len*sizeof(float));
			out_buff->outdata_len[data_cnt] = src_len;
			out_buff->full_size += src_len;
			out_buff->data_id[data_cnt] = DT_SGN_FFT_SE_AM;
			out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
			out_buff->outdata_counter++;
		}
		// ------------------------------------------------

		// --- Накопление интеграла эхо в буфере сумирования --
		sum_buff->data_id = data_code;
		sum_buff->channel_id = proc_params->channel_id;  //proger_rd_ch_number();
		float S = 0;
		switch (proc_method)
		{
		case 4:
		{
			for (i = 0; i < DATA_MAX_LEN/4; i++) S += data0[i];
			data1[j] = (S-SNs)/512.0;
			break;
		}
		default:
		{
			for (i = 0; i < DATA_MAX_LEN/4; i++) S += data0[i];
			data1[j] = S/512.0;
			break;
		}
		}


		//index = proc_params->current_echo - 1;
		/*index = j;
		if (index >= sum_buff->max_size) return;
		if (index < 0) index = 0;

		sum_buff->sum_data[index] = S;
		sum_buff->pos = index+1;*/
		// ------------------------------------------------

		noise_echo++;
		signal_echo++;
	}

	// Запись накопленного спада с выходной буффер
	float *dst = out_buff->out_data;
	int dst_pos = out_buff->full_size;
	int data_cnt = out_buff->outdata_counter;

	src_len = signal_echo-NNN;
	memcpy(dst+dst_pos, data1, src_len*sizeof(float));
	out_buff->outdata_len[data_cnt] = src_len;
	out_buff->full_size += src_len;
	out_buff->data_id[data_cnt] = data_code;
	out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
	out_buff->group_index[data_cnt] = group_index;
	out_buff->outdata_counter++;

	//t_stop = clock();
	//printf("\t NMR data processing time: %d clock cycles\n", (t_stop - t_start) - t_overhead);
}

// Источник данных: upp_data
// в качестве промежуточного буфера используется ptr_data_org = data_bank[0] (соответствует D0)
// результат - накопленных шум в ptr_data2 = data_bank[2] (соответствует D2)
void noiseProcessing1(uint8_t *upp_data, float **data_bank, int rad, Data_Cmd *instr, Processing_Params *proc_params, OutBuffer *out_buff)
{
	if (instr->count != 3) return;

	int data_src = (int)instr->params[0];		// источник данных (эмулятор = 0, АЦП = 1)
	int out_time_data = (int)instr->params[1];	// выводить/не выводить исходные данные шума после: приведения, наложения окна
	int out_spec_data = (int)instr->params[2];	// выводить/не выводить спектр мощности шума после: мощностной спектр фурье, наложение окна в спектральной области

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	float *data0 = data_bank[0];
	float *data2 = data_bank[2];
	float *temp_data = data_bank[8];
	float *ptr_w = data_bank[9];

	int i;

	// ------------ эмулляция данных шума -------------
	if (data_src == 0)
	{
		int shift = rand()/(float)RAND_MAX*2048;
		for (i = 0; i < src_len; i++)
		{
			int16_t tab_noise = 0;
			if (i+shift < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift] - 2048;
			else tab_noise = noise_ui16[i] - 2048;
			uint16_t y = (uint16_t)(tab_noise + 2048);

			upp_data[2 * i] = (uint8_t) (y & 0x00FF);
			upp_data[2 * i + 1] = (uint8_t) (y >> 8);
		}
	}
	// ------------------------------------------------

	// ------------- приведение данных ----------------
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

	float S = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)upp_data[2*i + src_pre];
		uint16_t x2 = (uint16_t)upp_data[2*i+1 + src_pre];
		uint16_t x = (x1 | (x2 << 8));

		S += x;
		data0[index] = (float)x;
		index += 2;
	}
	S /= len;

	memset(data0, 0x0, dst_pre*sizeof(float));
	memset(data0+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = data0[index];
		data0[index++] = (x - S)*ADC_to_mV;
		data0[index++] = 0;
	}

	proc_params->points_count = 2*src_len;
	// ------------------------------------------------

	// ----- оконная функция во временной области -----
	uint8_t func_id = 0;
	int x0 = 0;
	int sigma = 256;

	func_id = proc_params->echo_func;
	x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
	sigma = proc_params->echo_sigma;

	int points_count = proc_params->points_count;
	float mul = DATA_MAX_LEN/(float)points_count;

	switch (func_id)
	{
	case NoFunc: 				// No win-function
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < DATA_MAX_LEN; i+= 2)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			data0[i] *= a;
			data0[i+1] *= a;
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);
		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0)
		{
			data0[0] = tab[index]*data0[0];
			data0[1] = tab[index]*data0[1];
		}
		else
		{
			data0[0] = 0;
			data0[1] = 0;
		}
		int counter = 1;
		for (i = 2; i < DATA_MAX_LEN; i+=2)
		{
		    index += 2*step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += 2*dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN)
		    {
		    	data0[i] = tab[index]*data0[i];
		    	data0[i+1] = tab[index]*data0[i+1];
		    }
		    else
		    {
		    	data0[i] = 0;
		    	data0[i+1] = 0;
		    }
		}
		break;
	}
	default: break;
	}
	// ------------------------------------------------

	// - копирование в выходной буфер временнЫх данных
	if (out_time_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		int src_len = proc_params->points_count/2;
		int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
		if (pre_pos < 0) pre_pos = 0;
		for (i = 0; i < src_len; i++) dst[dst_pos+i] = data0[pre_pos+2*i];
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_NS_SE;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	// --------- быстрое преобразование Фурье ---------
	memcpy(temp_data - PAD, data0 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
	DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data0, brev, rad, 0, CMPLX_DATA_MAX_LEN);
	// ------------------------------------------------

	// --------- вычисление спектра мощности ----------
	index = 0;
	for (i = 0; i < DATA_MAX_LEN; i += 2)
	{
		data0[index++] = data0[i]*data0[i] + data0[i+1]*data0[i+1];	// without sqrt !
	}
	// ------------------------------------------------

	// ----- оконная функция в частотной области ------
	func_id = proc_params->spectr_func;
	x0 = proc_params->spectr_x0;
	sigma = proc_params->spectr_sigma;

	switch (func_id)
	{
	case NoFunc: 				// No win-function
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < DATA_MAX_LEN; i++)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			data0[i] *= a;
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);

		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0)
		{
			data0[0] *= tab[index];
			data0[1] *= tab[index];
		}
		else
		{
			data0[0] = 0;
			data0[1] = 0;
		}
		int counter = 1;
		for (i = 2; i < DATA_MAX_LEN; i+=2)
		{
		    index += 2*step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += 2*dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN)
		    {
		    	data0[i] *= tab[index];
		    	data0[i+1] *= tab[index];
		    }
		    else
		    {
		    	data0[i] = 0;
		    	data0[i+1] = 0;
		    }
		}
		break;
	}
	default: break;
	}
	// ------------------------------------------------

	// помещение в выходной буфер частотного спектра мощности
	if (out_spec_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		int src_len = DATA_MAX_LEN/4;
		memcpy(dst+dst_pos, data0, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_NS_POWER_SE;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	// ---------- Накопление шума (в data2) -----------
	int N = proc_params->current_echo;
	if (N < 2) memcpy(data2, data0, DATA_MAX_LEN*sizeof(float));
	else
	{
		for (i = 0; i < DATA_MAX_LEN; i++)
		{
			data2[i] = (N-1)*data2[i]/N + data0[i]/N;
		}
	}
	// ------------------------------------------------
}


// Источник данных: upp_data
// в качестве промежуточного буфера используется ptr_data_org = data_bank[0] (соответствует D0)
// результат - накопленных шум в ptr_data2 = data_bank[2] (соответствует D2)
void signalProcessing1(uint8_t *upp_data, float **data_bank, int rad, Data_Cmd *instr, Processing_Params *proc_params, SummationBuffer *sum_buff, OutBuffer *out_buff)
{
	if (instr->count != 4) return;

	int data_src = (int)instr->params[0];		// источник данных (эмулятор = 0, АЦП = 1)
	int sn_ratio = (int)instr->params[1];		// отношение сигнал/шум
	int out_time_data = (int)instr->params[2];	// выводить/не выводить исходные данные шума после: приведения, наложения окна
	int out_spec_data = (int)instr->params[3];	// выводить/не выводить спектр мощности шума после: мощностной спектр фурье, наложение окна в спектральной области

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	float *data0 = data_bank[0];
	float *data2 = data_bank[2];
	float *temp_data = data_bank[8];
	float *ptr_w = data_bank[9];

	int i;

	// ----------- эмулляция данных сигнала -----------
	if (data_src == 0)
	{
		if (src_len > NMR_DATA_LEN) src_len = NMR_DATA_LEN;

		int index = proc_params->current_echo;
		if (index < 0) index = 0;
		if (index > 1000) index = 1000;
		float mul = relax_ampls[index]/32.0/3.0;		// "3" = 3 sigma

		int shift_ns = rand()/(float)RAND_MAX*2048;
		int shift = (NMR_DATA_LEN - src_len)/2;

		for (i = 0; i < src_len; i++)
		{
			int16_t tab_noise = 0;
			if (i+shift_ns < NMR_DATA_LEN) tab_noise = noise_ui16[i+shift_ns] - 2048;
			else tab_noise = noise_ui16[i] - 2048;

			int16_t tab_echo = echo_ui16_60mks[i+shift] - 2048;
			uint16_t y = (uint16_t)((tab_echo*sn_ratio*mul) + tab_noise + 2048);

			upp_data[2 * i] = (uint8_t) (y & 0x00FF);
			upp_data[2 * i + 1] = (uint8_t) (y >> 8);
		}
	}
	// ------------------------------------------------

	// ------------- приведение данных ----------------
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

	float S = 0;
	int index = dst_pre;
	int len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		uint16_t x1 = (uint16_t)upp_data[2*i + src_pre];
		uint16_t x2 = (uint16_t)upp_data[2*i+1 + src_pre];
		uint16_t x = (x1 | (x2 << 8));

		S += x;
		data0[index] = (float)x;
		index += 2;
	}
	S /= len;

	memset(data0, 0x0, dst_pre*sizeof(float));
	memset(data0+dst_post, 0x0, dst_pre*sizeof(float));

	index = dst_pre;
	len = (src_post - src_pre);
	for (i = 0; i < len; i++)
	{
		float x = data0[index];
		data0[index++] = (x - S)*ADC_to_mV;
		data0[index++] = 0;
	}

	proc_params->points_count = 2*src_len;
	// ------------------------------------------------

	// ----- оконная функция во временной области -----
	uint8_t func_id = 0;
	int x0 = 0;
	int sigma = 256;

	func_id = proc_params->echo_func;
	x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
	sigma = proc_params->echo_sigma;

	int points_count = proc_params->points_count;
	float mul = DATA_MAX_LEN/(float)points_count;

	switch (func_id)
	{
	case NoFunc: 				// No win-function
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < DATA_MAX_LEN; i+= 2)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			data0[i] *= a;
			data0[i+1] *= a;
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);
		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0)
		{
			data0[0] = tab[index]*data0[0];
			data0[1] = tab[index]*data0[1];
		}
		else
		{
			data0[0] = 0;
			data0[1] = 0;
		}
		int counter = 1;
		for (i = 2; i < DATA_MAX_LEN; i+=2)
		{
		    index += 2*step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += 2*dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN)
		    {
		    	data0[i] = tab[index]*data0[i];
		    	data0[i+1] = tab[index]*data0[i+1];
		    }
		    else
		    {
		    	data0[i] = 0;
		    	data0[i+1] = 0;
		    }
		}
		break;
	}
	default: break;
	}
	// ------------------------------------------------

	// - копирование в выходной буфер временнЫх данных
	if (out_time_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		int src_len = proc_params->points_count/2;
		int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
		if (pre_pos < 0) pre_pos = 0;
		for (i = 0; i < src_len; i++) dst[dst_pos+i] = data0[pre_pos+2*i];
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_SGN_SE;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	// --------- быстрое преобразование Фурье ---------
	memcpy(temp_data - PAD, data0 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
	DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data0, brev, rad, 0, CMPLX_DATA_MAX_LEN);
	// ------------------------------------------------

	// --------- вычисление спектра мощности ----------
	index = 0;
	for (i = 0; i < DATA_MAX_LEN; i += 2)
	{
		data0[index++] = data0[i]*data0[i] + data0[i+1]*data0[i+1];	// without sqrt !
	}
	// ------------------------------------------------

	// ----- оконная функция в частотной области ------
	func_id = proc_params->spectr_func;
	x0 = proc_params->spectr_x0;
	sigma = proc_params->spectr_sigma;

	switch (func_id)
	{
	case NoFunc: 				// No win-function
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < DATA_MAX_LEN; i++)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			data0[i] *= a;
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);

		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0)
		{
			data0[0] *= tab[index];
			data0[1] *= tab[index];
		}
		else
		{
			data0[0] = 0;
			data0[1] = 0;
		}
		int counter = 1;
		for (i = 2; i < DATA_MAX_LEN; i+=2)
		{
		    index += 2*step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += 2*dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN)
		    {
		    	data0[i] *= tab[index];
		    	data0[i+1] *= tab[index];
		    }
		    else
		    {
		    	data0[i] = 0;
		    	data0[i+1] = 0;
		    }
		}
		break;
	}
	default: break;
	}
	// ------------------------------------------------

	// вычисление спектра сигнала по алгоритму Скирды В.Д.
	float val = 0;
	int sgn = 0;
	for (i = 0; i < DATA_MAX_LEN/2; i++)
	{
		val = data0[i] - data2[i];
		if (val >= 0) sgn = 1; else sgn = -1;
		val = fabs(val);
		data0[i] = sgn*Q_rsqrt(val);
	}
	// ------------------------------------------------

	// помещение в выходной буфер частотного спектра мощности
	if (out_spec_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		int src_len = DATA_MAX_LEN/4;
		memcpy(dst+dst_pos, data0, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_SGN_POWER_SE;
		out_buff->channel_id[data_cnt] = proc_params->channel_id; //proger_rd_ch_number();
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	// ---------- Накопление шума (в data2) -----------
	sum_buff->data_id = DT_SGN_RELAX;
	sum_buff->channel_id = proc_params->channel_id; //proger_rd_ch_number();
	S = 0;
	for (i = 0; i < DATA_MAX_LEN/2; i++) S += data0[i];

	index = proc_params->current_echo - 1;
	if (index >= sum_buff->max_size) return;
	if (index < 0) index = 0;

	sum_buff->sum_data[index] = S/512.0;
	sum_buff->pos = index+1;
	// ------------------------------------------------
}

/*
// Источник данных: upp_data или data_heap (hx)
// в качестве промежуточного буфера используется ptr_data_org = data_bank[0] (соответствует D0)
// результат - накопленных шум в ptr_data2 = data_bank[2] (соответствует D2)
void signalProcessing1(uint8_t *upp_data, float **data_bank, float **hx_bank, int rad, Data_Cmd *instr, Processing_Params *proc_params, SummationBuffer *sum_buff, OutBuffer *out_buff)
{
	if (instr->count != 3) return;

	int data_src = (int)instr->params[0];		// источник данных (D0 = 0, hx = 1)
	int out_time_data = (int)instr->params[1];	// выводить/не выводить исходные данные шума после: приведения, наложения окна
	int out_spec_data = (int)instr->params[2];	// выводить/не выводить спектр мощности шума после: мощностной спектр фурье, наложение окна в спектральной области

	int src_len = proc_params->points_count;
	if (src_len == 0) return;

	float *data2 = data_bank[2];
	float *temp_data = data_bank[8];
	float *ptr_w = data_bank[9];

	int src_index = proc_params->current_echo;
	float *data0 = 0;
	if (data_src == 0) data0 = data_bank[0];
	else if (data_src == 1)
	{
		data0 = data_bank[0];
		memcpy(data0, hx_bank[src_index], DATA_MAX_LEN*sizeof(float));
	}
	else return;

	int i;

	// ----- оконная функция во временной области -----
	uint8_t func_id = 0;
	int x0 = 0;
	int sigma = 256;

	func_id = proc_params->echo_func;
	x0 = proc_params->echo_x0 + DATA_MAX_LEN/2; //params->points_count/2;
	sigma = proc_params->echo_sigma;

	int points_count = proc_params->points_count;
	float mul = DATA_MAX_LEN/(float)points_count;

	switch (func_id)
	{
	case NoFunc: 				// No win-function
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < DATA_MAX_LEN; i+= 2)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			data0[i] *= a;
			data0[i+1] *= a;
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);
		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0)
		{
			data0[0] = tab[index]*data0[0];
			data0[1] = tab[index]*data0[1];
		}
		else
		{
			data0[0] = 0;
			data0[1] = 0;
		}
		int counter = 1;
		for (i = 2; i < DATA_MAX_LEN; i+=2)
		{
		    index += 2*step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += 2*dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN)
		    {
		    	data0[i] = tab[index]*data0[i];
		    	data0[i+1] = tab[index]*data0[i+1];
		    }
		    else
		    {
		    	data0[i] = 0;
		    	data0[i+1] = 0;
		    }
		}
		break;
	}
	default: break;
	}
	// ------------------------------------------------

	// - копирование в выходной буфер временнЫх данных
	if (out_time_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		int src_len = proc_params->points_count/2;
		int pre_pos = (DATA_MAX_LEN - 2*src_len)/2;
		if (pre_pos < 0) pre_pos = 0;
		for (i = 0; i < src_len; i++) dst[dst_pos+i] = data0[pre_pos+2*i];
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_SGN_SE;
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	// --------- быстрое преобразование Фурье ---------
	memcpy(temp_data - PAD, data0 - PAD, UPP_DATA_SIZE); 	// дублирование данных src, т.к. они разрушаются при выполнении функции DSPF_sp_fftSPxSP(...)
	DSPF_sp_fftSPxSP(CMPLX_DATA_MAX_LEN, temp_data, ptr_w, data0, brev, rad, 0, CMPLX_DATA_MAX_LEN);
	// ------------------------------------------------

	// --------- вычисление спектра мощности ----------
	int index = 0;
	for (i = 0; i < DATA_MAX_LEN; i += 2)
	{
		data0[index++] = data0[i]*data0[i] + data0[i+1]*data0[i+1];	// without sqrt !
	}
	// ------------------------------------------------

	// ----- оконная функция в частотной области ------
	func_id = proc_params->spectr_func;
	x0 = proc_params->spectr_x0;
	sigma = proc_params->spectr_sigma;

	switch (func_id)
	{
	case NoFunc: 				// No win-function
		break;
	case Func_Rect: 			// Rectangular function
	{
		int width = sigma>>1;
		for (i = 0; i < DATA_MAX_LEN; i++)
		{
			int a = 0;
			if (i < x0 && i > x0-width ) a = 1;
			else if (i > x0 && i < x0+width ) a = 1;
			data0[i] *= a;
		}
		break;
	}
	case Func_Gauss:
	case Func_BiGauss:
	{
		float *tab;
		if (func_id == 2) tab = &gauss_tab[0];
		else tab = &bigauss_tab[0];

		int nod = Q_NOD(REF_SIGMA, sigma);
		int D0 = REF_SIGMA/nod;
		int d0 = sigma/nod;

		int step = round(D0/d0);

		int dindex = D0 % d0;
		if (step != D0/d0) dindex -= d0;

		int ref_from = REF_X0 - (x0*D0)/d0;

		int index = ref_from;
		if (index >= 0)
		{
			data0[0] *= tab[index];
			data0[1] *= tab[index];
		}
		else
		{
			data0[0] = 0;
			data0[1] = 0;
		}
		int counter = 1;
		for (i = 2; i < DATA_MAX_LEN; i+=2)
		{
		    index += 2*step;
		    if (counter++ >= d0 && dindex != 0)
		    {
		        index += 2*dindex;
		        counter = 1;
		    }
		    if (index >= 0 && index < REF_DATA_LEN)
		    {
		    	data0[i] *= tab[index];
		    	data0[i+1] *= tab[index];
		    }
		    else
		    {
		    	data0[i] = 0;
		    	data0[i+1] = 0;
		    }
		}
		break;
	}
	default: break;
	}
	// ------------------------------------------------

	// вычисление спектра сигнала по алгоритму Скирды В.Д.
	float val = 0;
	int sgn = 0;
	for (i = 0; i < DATA_MAX_LEN/2; i++)
	{
		val = data0[i] - data2[i];
		if (val >= 0) sgn = 1; else sgn = -1;
		val = fabs(val);
		data0[i] = sgn*Q_rsqrt(val);
	}
	// ------------------------------------------------

	// помещение в выходной буфер частотного спектра мощности
	if (out_spec_data > 0)
	{
		float *dst = out_buff->out_data;
		int dst_pos = out_buff->full_size;
		int data_cnt = out_buff->outdata_counter;

		int src_len = DATA_MAX_LEN/4;
		memcpy(dst+dst_pos, data0, src_len*sizeof(float));
		out_buff->outdata_len[data_cnt] = src_len;
		out_buff->full_size += src_len;
		out_buff->data_id[data_cnt] = DT_SGN_POWER_SE;
		out_buff->outdata_counter++;
	}
	// ------------------------------------------------

	// ---------- Накопление шума (в data2) -----------
	sum_buff->data_id = DT_SGN_RELAX;
	float S = 0;
	for (i = 0; i < DATA_MAX_LEN/2; i++) S += data0[i];

	index = proc_params->current_echo - 1;
	if (index >= sum_buff->max_size) return;
	if (index < 0) index = 0;

	sum_buff->sum_data[index] = S;
	sum_buff->pos = index+1;
	// ------------------------------------------------
}
*/
