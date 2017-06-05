/*
 * proger.c
 *
 *  Created on: 08.06.2014
 *      Author: aivanov
 *
 */


#include "proger.h"
//#include "../upp/upp.c"


void proger_stop ()
{
	unsigned char *proger_adr;
	 proger_adr  = (unsigned char *) PROGER_CONFIG_ADDR;
	*proger_adr &= 0xFE;//stop proger
}/* proger_stop */
/*---------------------------------------------------------------------------*/

void proger_start ()
{
	unsigned char *proger_adr;
	 proger_adr  = (unsigned char *) PROGER_CONFIG_ADDR;
	*proger_adr |= 0x01;//start proger
}/* proger_start */
/*---------------------------------------------------------------------------*/

unsigned int proger_rd_reg_32 (unsigned int adr)
{
	volatile unsigned int *proger_adr;
	volatile unsigned int data;

	proger_adr = (unsigned int *) adr;

	data = *proger_adr;

	return (data);
}// proger_rd_reg_32
/*---------------------------------------------------------------------------*/

unsigned int proger_wr_reg_32 (unsigned int adr, unsigned int data)
{
	volatile unsigned int *proger_adr;
	volatile unsigned int data_wr;

	proger_adr = (unsigned int *) adr;
	data_wr    = data;

	*proger_adr = data_wr;

	return (data);
}// proger_wr_reg_32
/*---------------------------------------------------------------------------*/

unsigned int proger_read_time_counter ()
{
	volatile unsigned int clks;

	proger_wr_reg_32 (MEM_ADDR_CTRL_REG, 0x00);
	clks  = proger_rd_reg_32(MEM_ADDR_PIT_OUT_REG);
	proger_wr_reg_32 (MEM_ADDR_CTRL_REG, 0x02);
	clks /= 100;

	return (clks);
}// proger_read_time_counter
/*---------------------------------------------------------------------------*/

unsigned int proger_restart_time_counter ()
{
	proger_wr_reg_32 (MEM_ADDR_CTRL_REG, 0x01);

	proger_wr_reg_32 (MEM_ADDR_CTRL_REG, 0x02);

	return (0);
}// proger_reset_time_counter
/*---------------------------------------------------------------------------*/

unsigned int proger_read_gpio ()
{
	volatile unsigned int gpio_pins = 0xFFFFFFFF;

	gpio_pins  = proger_rd_reg_32(MEM_ADDR_PROC_GPIO);

	return (gpio_pins);
}// proger_read_gpio
/*---------------------------------------------------------------------------*/

unsigned int proger_rd_ch_number(void)
{
	volatile unsigned int ch_number = 0xFFFFFFFF;

	ch_number  = proger_rd_reg_32(MEM_ADDR_CH_NUM) & 0xFF;

	return (ch_number);
}// proger_rd_ch_number
/*---------------------------------------------------------------------------*/

unsigned int proger_rd_device_serial(void)
{
	volatile unsigned int device_serial = 0xFFFFFFFF;

	device_serial  = proger_rd_reg_32(MEM_ADDR_DEVICE_SERIAL);

	return (device_serial);
}// proger_rd_device_serial
/*---------------------------------------------------------------------------*/

int proger_rd_acquisition_info (unsigned char *array, unsigned int byte_count)
{
	volatile unsigned char *proger_proc_adr_clr, *proger_proc_adr_inc, *proger_proc_adr_dta;
	volatile unsigned int k;

	if (byte_count > 1024)
		return (0);

	proger_proc_adr_clr = (unsigned char *) MEM_ADDR_CNTR_PROC_CLR;
	proger_proc_adr_inc = (unsigned char *) MEM_ADDR_CNTR_PROC_INC;
	proger_proc_adr_dta = (unsigned char *) MEM_ADDR_CNTR_PROC_DTA;

	*proger_proc_adr_clr = 0x00;
	for (k = 0; k < byte_count; k++)
	{
		array[k] = *proger_proc_adr_dta;
		*proger_proc_adr_inc = 0x00;
	}

	return (1);
}/* proger_rd_acquisition_info */

/*---------------------------------------------------------------------------*/

int proger_wr_pulseprog (unsigned char *array, unsigned int byte_count)
{
	volatile unsigned char *proger_adr_tmp, *proger_adr_clr, *proger_adr_inc, *proger_adr_dta;
	volatile unsigned int k;

	proger_stop ();

	if (byte_count > PROGER_SIZE_IN_BYTES)
		return (0);

	proger_adr_tmp = (unsigned char *) MEM_ADDR_TMP_WORD;
	proger_adr_clr = (unsigned char *) MEM_ADDR_CNTR_CLR;
	proger_adr_inc = (unsigned char *) MEM_ADDR_CNTR_INC;
	proger_adr_dta = (unsigned char *) MEM_ADDR_FIFO_DTA;

	*proger_adr_clr = 0x00;
	for (k = 0; k < byte_count; k++)
	{
		*proger_adr_tmp = array[k];
		*proger_adr_dta = 0x00;
		*proger_adr_inc = 0x00;
	}

	return (1);
}/* proger_wr_pulseprog */

/*---------------------------------------------------------------------------*/

int proger_rd_pulseprog (unsigned char *array, unsigned int byte_count)
{
	volatile unsigned char *proger_adr_tmp, *proger_adr_clr, *proger_adr_inc, *proger_adr_dta;
	volatile unsigned int k, tmp;

	proger_stop ();

	if (byte_count > PROGER_SIZE_IN_BYTES)
		return (0);

	proger_adr_tmp = (unsigned char *) MEM_ADDR_TMP_WORD;
	proger_adr_clr = (unsigned char *) MEM_ADDR_CNTR_CLR;
	proger_adr_inc = (unsigned char *) MEM_ADDR_CNTR_INC;
	proger_adr_dta = (unsigned char *) MEM_ADDR_FIFO_DTA;

	*proger_adr_clr = 0x00;
	for (k = 0; k < byte_count; k++)
	{
		array[k] = *proger_adr_dta;
		tmp = array[k];
		*proger_adr_inc = 0x00;
	}

	return (1);
}/* proger_rd_pulseprog */

/*---------------------------------------------------------------------------*/

int proger_compare_pulseprog (unsigned char *array, unsigned int byte_count)
{
	volatile unsigned char *proger_adr_tmp, *proger_adr_clr, *proger_adr_inc, *proger_adr_dta;
	volatile unsigned int k, result;
	volatile unsigned char array_prog [PROGER_SIZE_IN_BYTES];

	proger_stop ();

	if (byte_count > PROGER_SIZE_IN_BYTES)
		return (0);

	proger_adr_tmp = (unsigned char *) MEM_ADDR_TMP_WORD;
	proger_adr_clr = (unsigned char *) MEM_ADDR_CNTR_CLR;
	proger_adr_inc = (unsigned char *) MEM_ADDR_CNTR_INC;
	proger_adr_dta = (unsigned char *) MEM_ADDR_FIFO_DTA;

	*proger_adr_clr = 0x00;
	for (k = 0; k < byte_count; k++)
	{
		array[k] = *proger_adr_dta;
		*proger_adr_inc = 0x00;
	}

	result = 1;

	for (k = 0; k < byte_count; k++)
	{
		if (array_prog[k] != array[k])
		{
			result = 0;
			break;
		}
	}

	return (result);
}/* proger_compare_pulseprog */

/*---------------------------------------------------------------------------*/

int proger_mem_init ()
{
	volatile unsigned char comm [4];
	volatile unsigned int k;

	proger_stop ();
	proger_reset_comm_fifo_counter();

	memset(comm, 0x00, 4);

	for (k = 0; k < PROG_MAX_COMMANDS; k++)
	{
		proger_wr_comm_arr_to_fifo (comm);
	}

	return (1);
}/* proger_mem_init */

/*---------------------------------------------------------------------------*/

/*int proger_test_mem_pulseprog ()
{
	volatile unsigned char array [PROGER_SIZE_IN_BYTES];
	volatile unsigned int k, result;

	for (k = 0; k < PROGER_SIZE_IN_BYTES; k++)
	{
		array[k] = k;
	}

	result = proger_compare_pulseprog ( (unsigned char *) array, PROGER_SIZE_IN_BYTES);

	proger_mem_init ();

	return (result);
}// proger_test_mem_pulseprog */

/*---------------------------------------------------------------------------*/

int proger_rd_echo_count (void)
{
	volatile unsigned int *proger_proc_adr_echo_count;
	volatile unsigned int echo_count;

	proger_proc_adr_echo_count = (unsigned int *) MEM_ADDR_ECHO_CNTR;

	echo_count = *proger_proc_adr_echo_count;

	return (echo_count);
}/* proger_rd_echo_count */

/*---------------------------------------------------------------------------*/

int proger_rd_gamma_count (void)
{
	volatile unsigned int *proger_proc_adr_gamma_count, *proger_proc_adr_gamma_count_rst;
	volatile unsigned int gamma_count;

	proger_proc_adr_gamma_count     = (unsigned int *) MEM_ADDR_GAMMA_CNTR01_REG;
	proger_proc_adr_gamma_count_rst = (unsigned int *) MEM_ADDR_GAMMA_CNTR01_RST_REG;

	gamma_count = *proger_proc_adr_gamma_count;

	*proger_proc_adr_gamma_count_rst = 0xFF;
	*proger_proc_adr_gamma_count_rst = 0x00;

	return (gamma_count);
}/* proger_rd_gamma_count */

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
int proger_rd_proger_type (void)
{
	volatile unsigned int *adr_proger_type_reg;
	volatile unsigned int proger_type;

	adr_proger_type_reg = (unsigned int *) MEM_ADDR_PROGR_PROGR_TYPE_REG;

	proger_type = *adr_proger_type_reg;

	return (proger_type);
}/* proger_rd_proger_type */

/*---------------------------------------------------------------------------*/
int proger_rd_proger_version (void)
{
	volatile unsigned int *adr_proger_version;
	volatile unsigned int proger_version;

	adr_proger_version = (unsigned int *) MEM_ADDR_PROGR_PROGR_VERS_REG;

	proger_version = *adr_proger_version;

	return (proger_version);
}/* proger_rd_proger_version */

/*---------------------------------------------------------------------------*/
int proger_rd_proger_sub_version (void)
{
	volatile unsigned int *adr_proger_sub_version;
	volatile unsigned int proger_sub_version;

	adr_proger_sub_version = (unsigned int *) MEM_ADDR_PROGR_PROGR_SUB_VERS_REG;

	proger_sub_version = *adr_proger_sub_version;

	return (proger_sub_version);
}/* proger_rd_proger_sub_version */

/*---------------------------------------------------------------------------*/
int proger_rd_adc_points_count (void)
{
	volatile unsigned int *proger_proc_adr_adc_points_count;
	volatile unsigned int adc_points_count;
	unsigned int max_num_points_in_UPP_DMA_buf, tmp_mask;
	//dummyDelay(2);

#if ( (UPP_DMA_BUF_SIZE == 64) || (UPP_DMA_BUF_SIZE == 128) || (UPP_DMA_BUF_SIZE == 256) )

	proger_proc_adr_adc_points_count = (unsigned int *) MEM_ADDR_ADC_PNTS_CNTR;

	adc_points_count = *proger_proc_adr_adc_points_count;

	//Calculating number of received points if upp DMA chunk is UPP_DMA_BUF_SIZE
	max_num_points_in_UPP_DMA_buf = UPP_DMA_BUF_SIZE >> 1;
	tmp_mask = ~(max_num_points_in_UPP_DMA_buf - 1);
	adc_points_count = adc_points_count & tmp_mask;

	return (adc_points_count);

#else
#error UPP_DMA_BUF_SIZE could be only 64 or 128 or 256 !
	check upp.h and proger.h
#endif

}/* proger_rd_adc_points_count */

/*---------------------------------------------------------------------------*/

double proger_rd_adc_frq_hz (void)
{
	volatile unsigned int *proger_proc_adr_adc_frq;
	volatile unsigned int adc_frq_num;
	volatile double adc_frq;

	proger_proc_adr_adc_frq = (unsigned int *) MEM_ADDR_DDS_FRQ05_REG;

	adc_frq_num = *proger_proc_adr_adc_frq;

	adc_frq  = adc_frq_num;
	adc_frq *= 250000000;
	adc_frq /= 4294967296;

	return (adc_frq);
}/* proger_rd_adc_frq */

/*---------------------------------------------------------------------------*/

int proger_rd_pwr_pg (void)
{
	volatile unsigned int data;

	data = proger_rd_reg_32 (MEM_ADDR_DEV_STATE_REG);
	data = data & 0x00000001;

	return (data);
}// proger_rd_pwr_pg

/*---------------------------------------------------------------------------*/

int proger_rd_conf_mem ( unsigned char *data )
{
	volatile unsigned char *addr;

	//addr = (unsigned char *) MEM_ADDR_CONF_MEM;

	memcpy ( data, (unsigned char *) MEM_ADDR_CONF_MEM, CONF_MEM_LEN );
	//memcpy ( (void *) data, (void *) addr, CONF_MEM_LEN );

	return ( 1 );
}// proger_rd_pwr_pg

/*---------------------------------------------------------------------------*/

int proger_reset_comm_fifo_counter (void)
{
	volatile unsigned char *proger_adr_clr;

	proger_stop ();

	proger_adr_clr = (unsigned char *) MEM_ADDR_CNTR_CLR;

	*proger_adr_clr = 0x00;

	return (1);
}/* proger_reset_comm_fifo_counter */

/*---------------------------------------------------------------------------*/

int proger_wr_comm_arr_to_fifo (volatile unsigned char *comm)
{
	volatile unsigned char *proger_adr_tmp, *proger_adr_inc, *proger_adr_dta;
	volatile unsigned int k;

	proger_stop ();

	proger_adr_tmp = (unsigned char *) MEM_ADDR_TMP_WORD;
	proger_adr_inc = (unsigned char *) MEM_ADDR_CNTR_INC;
	proger_adr_dta = (unsigned char *) MEM_ADDR_FIFO_DTA;

	for (k = 0; k < 4; k++)
	{
		*proger_adr_tmp = comm[k];
		*proger_adr_dta = 0x00;
		*proger_adr_inc = 0x00;
	}

	return (1);
}/* proger_wr_comm_arr_to_fifo */

/*---------------------------------------------------------------------------*/

int proger_wr_comm_to_fifo (unsigned char comm, unsigned char arg1, unsigned char arg2, unsigned char arg3)
{
	volatile unsigned char *proger_adr_tmp, *proger_adr_inc, *proger_adr_dta;
	volatile unsigned int k;

	proger_stop ();

	proger_adr_tmp = (unsigned char *) MEM_ADDR_TMP_WORD;
	proger_adr_inc = (unsigned char *) MEM_ADDR_CNTR_INC;
	proger_adr_dta = (unsigned char *) MEM_ADDR_FIFO_DTA;

	*proger_adr_tmp = comm;
	*proger_adr_dta = 0x00;
	*proger_adr_inc = 0x00;

	*proger_adr_tmp = arg1;
	*proger_adr_dta = 0x00;
	*proger_adr_inc = 0x00;

	*proger_adr_tmp = arg2;
	*proger_adr_dta = 0x00;
	*proger_adr_inc = 0x00;

	*proger_adr_tmp = arg3;
	*proger_adr_dta = 0x00;
	*proger_adr_inc = 0x00;

	return (1);
}/* proger_wr_comm_arr_to_fifo */

/*---------------------------------------------------------------------------*/

int proger_rd_group_index(void)
{
	return 0;
}/* int proger_rd_group_index */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*
int main_proger_wr_pulseprog_test_GPIO3 ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	unsigned int k, result;
	unsigned char array[PROGER_SIZE_IN_BYTES] = {

			//241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses
			241	,	0	,	0x00	,	0x63	, // 10 ms delay before RF pulses

				210	,	0x00,	0x00,	0x10 | 0x00,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses

				210	,	0x00,	0x00,	0x10 | 0x08,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0	,	9	, // duration of GPIO pulse

			//	231	,	0	,	0	,	200	, //RF pulse 20 mks

				210	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO2 goes low - adc window started

				243	,	0	,	0x01,	0x2B, // ecos number = 300

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

			241	,	0	,	0	,	99	,	 // adc time 100 mks

				242	,	0	,	0	,	1	,

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

				242	,	0	,	0	,	0	, // adc off

				244	,	0	,	0	,	0	, // end of cycles

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay after RF pulses

				210	,	0x00	,	0x00	,	0x10  | 0x08,	//GPIO2 goes high - adc window ended

			241	,	0	,	0	,	9	,

				210	,	0x00,	0x00,	0x00, //GPIO3 goes high -  RF pulses ended, awake led off

			241	,	0	,	0	,	19	,	// last delay

				243	,
				0	,
				0	,
				0	,

			241,	0x0F,	0x42,	0x3F,

				244	,
				0	,
				0	,
				0	,

				0	,
				0	,
				0	,
				0	,

	};


	proger_stop ();
	//proger_mem_init ();

	proger_wr_pulseprog (array, 25*4);

}// main_proger_wr_pulseprog_test_GPIO3

int main_proger_wr_pulseprog_test_GPIO_48f ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	unsigned int k, result;
	unsigned char array[PROGER_SIZE_IN_BYTES] = {

			//241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses
			241	,	0	,	0x00	,	0x63	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x00,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x08,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0	,	9	, // duration of GPIO pulse

			//	231	,	0	,	0	,	200	, //RF pulse 20 mks

				200	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO2 goes low - adc window started

				243	,	0	,	0x01,	0x2C, // ecos number = 300

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

			241	,	0	,	0	,	99	,	 // adc time 100 mks

				242	,	0	,	0	,	1	,

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

				242	,	0	,	0	,	0	, // adc off

				244	,	0	,	0	,	0	, // end of cycles

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay after RF pulses

				200	,	0x00	,	0x00	,	0x10  | 0x08,	//GPIO2 goes high - adc window ended

			241	,	0	,	0	,	9	,

				200	,	0x00,	0x00,	0x00, //GPIO3 goes high -  RF pulses ended, awake led off

			241	,	0	,	0	,	19	,	// last delay

			241,	0x0F,	0x42,	0x3F,

				0	,
				0	,
				0	,
				0	,

	};


	proger_stop ();
	//proger_mem_init ();

	proger_wr_pulseprog (array, 25*4);

}// main_proger_wr_pulseprog_test_GPIO_48f

int main_proger_wr_pulseprog_test_GPIO_48f_COUNTER ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	unsigned int k, result;
	unsigned char array[PROGER_SIZE_IN_BYTES] = {

			//241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses
			241	,	0	,	0x00	,	0x63	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x00,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

				250	,	0xFF,	0xFF,	0xFF, // activate test configuration

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x08,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0	,	9	, // duration of GPIO pulse

			//	231	,	0	,	0	,	200	, //RF pulse 20 mks

				200	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO2 goes low - adc window started

				243	,	0	,	0x01,	0x2C, // begin cycle, ecos number = 300

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

			241	,	0	,	0	,	99	,	 // adc time 100 mks

				242	,	0	,	0	,	1	,

				201	,	0	,	12	,	0	, //write to adc processing memory

				202	,	1	,	2	,	3	,

				202	,	4	,	5	,	6	,

				202	,	7	,	8	,	9	,

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

				242	,	0	,	0	,	0	, // adc off

				244	,	0	,	0	,	0	, // end of cycle

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay after RF pulses

				200	,	0x00	,	0x00	,	0x10  | 0x08,	//GPIO2 goes high - adc window ended

			241	,	0	,	0	,	9	,

				200	,	0x00,	0x00,	0x00, //GPIO3 goes high -  RF pulses ended, awake led off

			241	,	0	,	0	,	19	,	// last delay

				243	,
				0	,
				0	,
				0	,

			241,	0x0F,	0x42,	0x3F,

				244	,
				0	,
				0	,
				0	,

				0	, //restart
				0	,
				0	,
				0	,

	};


	proger_stop ();
	//proger_mem_init ();

	proger_wr_pulseprog (array, 27*4);

}// main_proger_wr_pulseprog_test_GPIO_48f_COUNTER
*/

int main_proger_wr_pulseprog_test_GPIO_48f_ADC ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	unsigned int k, result;
	unsigned char array[PROGER_SIZE_IN_BYTES] = {

			//241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses
			241	,	0	,	0x00	,	0x63	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x00,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

				//250	,	0xFF,	0xFF,	0xFF, // activate test configuration

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x08,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0	,	9	, // duration of GPIO pulse

			//	231	,	0	,	0	,	200	, //RF pulse 20 mks

				200	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO2 goes low - adc window started

				243	,	0	,	0x01,	0x2C, // begin cycle, ecos number = 300

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

			241	,	0	,	0	,	99	,	 // adc time 100 mks

				242	,	0	,	0	,	1	,

				201	,	0	,	12	,	0	, //write to adc processing memory

				202	,	1	,	2	,	3	,

				202	,	4	,	5	,	6	,

				202	,	7	,	8	,	9	,

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

				242	,	0	,	0	,	0	, // adc off

				244	,	0	,	0	,	0	, // end of cycle

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay after RF pulses

				200	,	0x00	,	0x00	,	0x10  | 0x08,	//GPIO2 goes high - adc window ended

			241	,	0	,	0	,	9	,

				200	,	0x00,	0x00,	0x00, //GPIO3 goes high -  RF pulses ended, awake led off

			241	,	0	,	0	,	19	,	// last delay

				243	,
				0	,
				0	,
				0	,

			241,	0x0F,	0x42,	0x3F,

				244	,
				0	,
				0	,
				0	,

				0	, //restart
				0	,
				0	,
				0	,

	};


	proger_stop ();
	//proger_mem_init ();

	proger_wr_pulseprog (array, 27*4);
	return (0);

}// main_proger_wr_pulseprog_test_GPIO_48f_ADC

/*
int main_proger_wr_pulseprog_test_GPIO_48f_ADC_Diel ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	unsigned int k, result;
	unsigned char array[PROGER_SIZE_IN_BYTES] = {

			//241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses
			241	,	0	,	0x00	,	0x63	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x00,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

				//250	,	0xFF,	0xFF,	0xFF, // activate test configuration

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before RF pulses

				200	,	0x00,	0x00,	0x10 | 0x08,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

			241	,	0	,	0	,	9	, // duration of GPIO pulse

			//	231	,	0	,	0	,	200	, //RF pulse 20 mks

				200	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO2 goes low - adc window started

				243	,	0	,	0x01,	0x2C, // begin cycle, ecos number = 300

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

			241	,	0	,	0	,	99	,	 // adc time 100 mks

				242	,	0	,	0	,	1	,

				201	,	0	,	12	,	0	, //write to adc processing memory

				202	,	1	,	2	,	3	,

				202	,	4	,	5	,	6	,

				202	,	7	,	8	,	9	,

			241	,	0	,	0x01	,	0xC1, // delay 450 mks

				242	,	0	,	0	,	0	, // adc off

				244	,	0	,	0	,	0	, // end of cycle

			241	,	0	,	0x00	,	0x02	, // 2 mks interval for writing to processing memory

				201	,	0	,	12	,	10	, //write to processing memory

				202	,	11	,	12	,	13	,

				202	,	14	,	15	,	16	,

				202	,	17	,	18	,	19	,

			241	,	0	,	0x27	,	0x0F	, // 10 ms interval for read ext. device

				200	,	0x00,	0x00,	0x10 | 0x08 | 0x04 | 0x01, //GPIO[0] goes low - ext dev window started

			241	,	0	,	0x00	,	9	, // 10 mks interval end of read ext. device

					200	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO[0] goes low - ext dev window ended

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay after RF pulses

				200	,	0x00	,	0x00	,	0x10  | 0x08,	//GPIO2 goes high - adc window ended

			241	,	0	,	0	,	9	,

				200	,	0x00,	0x00,	0x00, //GPIO3 goes high -  RF pulses ended, awake led off

			241	,	0	,	0	,	19	,	// last delay

				243	,
				0	,
				0	,
				0	,

			241,	0x0F,	0x42,	0x3F,

				244	,
				0	,
				0	,
				0	,

				0	, //restart
				0	,
				0	,
				0	,

	};


	proger_stop ();
	//proger_mem_init ();

	proger_wr_pulseprog (array, 35*4);

	return(1);

}// main_proger_wr_pulseprog_test_GPIO_48f_ADC_Diel

int main_proger_wr_pulseprog_test_GPIO_48f_ADC_FID ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	//unsigned int k, result;
	unsigned char array[PROGER_SIZE_IN_BYTES] = {

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before all operations

				200	,	0x00,	0x00,	0x00,	//out reg 6 goes high - awake led lights up //GPIO3 goes low - rf pulses started
				//250	,	0xFF,	0xFF,	0xFF, // activate test configuration


			241	,	0	,	0x27	,	0x0F	, // 10 ms delay before ADC FULL WINDOW

				200	,	0x00,	0x00,	0x08,	//out reg 4 goes high - awake led lights up //GPIO3 goes low - rf pulses started

				243	,	0	,	0x00,	0x01, // begin cycle, ecos number = 1

			241	,	0	,	0	,	99	,	 // adc time 100 mks, NOISE ADC ON

					242	,	0	,	0	,	1	,

					201	,	1	,	0	,	1	, //write to adc processing memory COM/Number params/Type of returned data

					//202	,	2	,	0	,	2	,

			241	,	0	,	0	,	0	,	 // adc time 1 mks, NOISE ADC OFF

					242	,	0	,	0	,	0	,


			241	,	0	,	0	,	99	, // RF pulse interval

				222	,	0	,	0x03	,	0xE8	, //RF pulse 100 mks

				200	,	0x00,	0x00,	0x08 | 0x04, //GPIO2 goes low - adc window started


			241	,	0	,	0	,	99	,	 // adc time 100 mks

				242	,	0	,	0	,	1	,

				201	,	2	,	0	,	2	, //write to adc processing memory COM/Number params/Type of returned data


			241	,	0	,	0	,	99	,	 // time 100 mks

				242	,	0	,	0	,	0	, // adc off

				244	,	0	,	0	,	0	, // end of cycle


			241	,	0	,	0x27	,	0x0F	, // 10 ms interval for read ext. device

				200	,	0x00,	0x00,	0x10 | 0x08 | 0x04 | 0x01, //GPIO[0] goes low - ext dev window started

			241	,	0	,	0x00	,	9	, // 10 mks interval end of read ext. device

					200	,	0x00,	0x00,	0x10 | 0x08 | 0x04, //GPIO[0] goes low - ext dev window ended

			241	,	0	,	0x27	,	0x0F	, // 10 ms delay after RF pulses

				200	,	0x00	,	0x00	,	0x10  | 0x08,	//GPIO2 goes high - adc window ended

			241	,	0	,	0	,	9	,

				200	,	0x00,	0x00,	0x00, //GPIO3 goes high -  RF pulses ended, awake led off

			241	,	0	,	0	,	19	,	// last delay

				243	,
				0	,
				0	,
				0	,

			241,	0x0F,	0x42,	0x3F, // 1s delay

				244	,
				0	,
				0	,
				0	,

				0	, //restart
				0	,
				0	,
				0	,

	};


	proger_stop ();
	//proger_mem_init ();

	proger_wr_pulseprog (array, 32*4);

	return(1);

}// main_proger_wr_pulseprog_test_GPIO_48f_ADC_FID
*/

int main_proger_wr_pulseprog_test_FRQ ()
{
	//unsigned char array [PROGER_SIZE_IN_BYTES], array_prog [PROGER_SIZE_IN_BYTES];
	unsigned int k, result;

	unsigned char array[PROGER_SIZE_IN_BYTES];
	//memset(array, 0x00, PROGER_SIZE_IN_BYTES);
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 2);
//		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_REG, 0, 0, 0x80); // Busy led on
		proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0);
		proger_wr_comm_to_fifo ( COM_OFS01, 0x1A, 0x36, 0xE2);
		//proger_wr_comm_to_fifo ( COM_OFS01, 0xE5, 0xC9, 0x1E);
		proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0, 2);
		proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0, 3);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1);
		proger_wr_comm_to_fifo ( COM_RF01, 0, 0, 5);
		proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0, 0);
		proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1);
		proger_wr_comm_to_fifo ( COM_RF01, 0, 0, 10);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
//		proger_wr_comm_to_fifo ( COM_ADS_IN, 0, 0, 1);
		proger_wr_comm_to_fifo ( COM_WR_RST_PROC_MEM, 2, 1, 0);
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 5, 4, 3);
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 8, 7, 6);
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 11, 10, 9);
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 14, 13, 12);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x01, 0xC1);// time interval 450 mks
//		proger_wr_comm_to_fifo ( COM_ADS_IN, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x01, 0xC1);// time interval 10 ms for delay after RF pulses
//		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_REG, 0, 0, 0x00); // Busy led off
	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // last delay
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x01);
		proger_wr_comm_to_fifo ( COM_RET, 0x0F, 0x42, 0x3F); // last delay

		return (0);
}// main_proger_wr_pulseprog_test_FRQ

int main_proger_wr_pulseprog_test_FID (
										double       resonance_frq_ofs_hz,
										unsigned int rf_pulse_duration,
										unsigned int nmr_aq_time,
										unsigned int noise_aq_time,
										unsigned int dead_time,
										double       adc_frq_ofs_hz,
										unsigned int damping_pulse_duration_after_rf_pulse,
										unsigned int delay_before_rf_pulses
										)
{
	unsigned int result = 0;
	volatile double tmp;
	unsigned int resonance_frq_ofs_num, adc_frq_ofs_num;
	unsigned char adc_clk_period = 1;//mks - Should be 1/Fclk, where Fclk is RF channel 4 frequency (ADC sampling frequency)

	resonance_frq_ofs_hz					= 0;
	rf_pulse_duration						= 100;//mks
	nmr_aq_time								= 1000;//mks
	noise_aq_time							= 1000;//mks
	dead_time								= 10;//mks
	adc_frq_ofs_hz							= 0;//Hz
	damping_pulse_duration_after_rf_pulse	= 5;//mks
	delay_before_rf_pulses					= 10000;//mks

	noise_aq_time -= 1;
	nmr_aq_time   -= 1;
	rf_pulse_duration -= 1;
	dead_time -= damping_pulse_duration_after_rf_pulse;
	dead_time -= 1;
	damping_pulse_duration_after_rf_pulse -= 1;
	delay_before_rf_pulses -= 1;

	tmp = DDS_BASIC_FRQ / 4294967296;
	tmp = resonance_frq_ofs_hz/tmp;
	resonance_frq_ofs_num = tmp;

	tmp = DDS_BASIC_FRQ / 4294967296;
	tmp = adc_frq_ofs_hz/tmp;
	adc_frq_ofs_num = tmp;
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27, 0x0F); // 10 ms interval for aq info writes (for MM)
//*********************************************************************************************
//commands of how to NMR and other data will be processed should be placed here
//*********************************************************************************************
	// Example of internal buffer usage for MM stack transfer to processor,
	// it transfers 12 bytes - from 0 to 11.
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1); // 2 mks interval
		proger_wr_comm_to_fifo ( COM_WR_RST_PROC_MEM, 0, 1, 2);		//first write
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 3, 4, 5);		//others writes
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 6, 7, 8);		//others writes
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 9, 10, 11);	//others writes

	//Здесь должна быть функция, сообщающая процессору о том, что пора забирать данные из буфера с инструкциями в ПЛИС

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer
		//simple delay, nothing happens during this interval


//*********************************************************************************************
// Initialization of frequency channels and so on
//*********************************************************************************************
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 5); // 6 mks interval
		proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0);	// test configuration sets to 0 - normal acquisition, ADC sampled signal instead of counter

		proger_wr_comm_to_fifo ( COM_SET_VC_DAC_1, 0, 0, 0);	// sets voltage on DAC1 for resonant circuit varicaps
		proger_wr_comm_to_fifo ( COM_SET_VC_DAC_2, 0, 0, 0);	// sets voltage on DAC2 for resonant circuit varicaps

		proger_wr_comm_to_fifo ( COM_SWITCH_FRQ, 0, 0, 0);	// set RF frequencies to Larmor frequency #0
		proger_wr_comm_to_fifo ( COM_OFS01, resonance_frq_ofs_num >> 2, resonance_frq_ofs_num >> 1, resonance_frq_ofs_num );	// set offset of RF channel 1 to 0
		proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
		proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0
		proger_wr_comm_to_fifo ( COM_OFS04, adc_frq_ofs_num >> 2, adc_frq_ofs_num >> 1, adc_frq_ofs_num );	// set offset of RF channel 4 to 0 (ADC sampling frequency)

		proger_wr_comm_to_fifo ( COM_PH01, 0, 0, 0);	// set phase of RF channel 1 to 0
		proger_wr_comm_to_fifo ( COM_PH02, 0, 0, 0);	// set phase of RF channel 2 to 0
		proger_wr_comm_to_fifo ( COM_PH03, 0, 0, 0);	// set phase of RF channel 3 to 0
		proger_wr_comm_to_fifo ( COM_PH04, 0, 0, 0);	// set phase of RF channel 4 to 0 (ADC sampling frequency)

		proger_wr_comm_to_fifo ( COM_SET_PA_ADDR, 0, 0, 0);	// set address of currently activated power amplifier
		proger_wr_comm_to_fifo ( COM_SET_RCV_ADDR, 0, 0, 0);	// set address of currently activated receiver

		proger_wr_comm_to_fifo ( COM_PH_RST, 0, 0, 0);	// synchronization of all RF channels (should be used only once)


//*********************************************************************************************
// RF pulses window and acquisition window begin
//*********************************************************************************************

	proger_wr_comm_to_fifo ( COM_TIME, delay_before_rf_pulses >> 2, delay_before_rf_pulses >> 1, delay_before_rf_pulses); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
		//simple delay, nothing happens during this interval

//*********************************************************************************************
// Noise acquisition
//*********************************************************************************************
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0); // 1 mks interval for starting receiver dumping pulse
		proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_ON); // Receiver damping pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, rf_pulse_duration >> 2, rf_pulse_duration >> 1, rf_pulse_duration); // time interval for dummy RF pulse
		proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // dummy RF pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, damping_pulse_duration_after_rf_pulse >> 2, damping_pulse_duration_after_rf_pulse >> 1, damping_pulse_duration_after_rf_pulse); // 5 mks interval for receiver dumping pulse
		proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // dummy RF pulse end

	proger_wr_comm_to_fifo ( COM_TIME, dead_time >> 2, dead_time >> 1, dead_time); // 5 mks interval for receiver to comes out of damping and to shutdown the power supplies
		proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_OFF); // Damping pulse end
		proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, noise_aq_time >> 2, noise_aq_time >> 1, noise_aq_time); // Acquisition time
		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_ACT); // ADC pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 8 * adc_clk_period - 1); // 8 ADC CLKs interval for ADC to complete the acquisition
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_DEACT); // ADC pulse end
		proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end

	//Здесь закончилось окно АЦП, процессор должен обрабатывать данные с шумом


//*********************************************************************************************
// Signal acquisition
//*********************************************************************************************
		proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0); // 1 mks interval for starting receiver dumping pulse
			proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_ON); // Receiver damping pulse begin

		proger_wr_comm_to_fifo ( COM_TIME, rf_pulse_duration >> 2, rf_pulse_duration >> 1, rf_pulse_duration); // time interval for dummy RF pulse
			proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin

		proger_wr_comm_to_fifo ( COM_TIME, damping_pulse_duration_after_rf_pulse >> 2, damping_pulse_duration_after_rf_pulse >> 1, damping_pulse_duration_after_rf_pulse); // 5 mks interval for receiver dumping pulse
			proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end

		proger_wr_comm_to_fifo ( COM_TIME, dead_time >> 2, dead_time >> 1, dead_time); // 5 mks interval for receiver to comes out of damping and to shutdown the power supplies
			proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_OFF); // Damping pulse end
			proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

		proger_wr_comm_to_fifo ( COM_TIME, noise_aq_time >> 2, noise_aq_time >> 1, noise_aq_time); // Acquisition time
			proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_ACT); // ADC pulse begin

		proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 8 * adc_clk_period - 1); // 8 ADC CLKs interval for ADC to complete the acquisition
		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_DEACT); // ADC pulse end
			proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end

	//Здесь закончилось окно АЦП, процессор должен обрабатывать данные с сигналом


//*********************************************************************************************
// RF pulses window and acquisition window end
//*********************************************************************************************

	//Здесь должны стоять функции, сообщающие, процессору об окончании полного окна АЦП и об окончании окна РЧ


//*********************************************************************************************
// Relaxation delay
//*********************************************************************************************
	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
		//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
		proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program
//*********************************************************************************************

	return(result);
}// main_proger_wr_pulseprog_test_FID

int main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style_windows_reserved (
															double       resonance_frq_ofs_hz,
															unsigned int rf_pulse_duration,
															unsigned int nmr_aq_time,
															unsigned int noise_aq_time,
															unsigned int dead_time,
															double       adc_frq_ofs_hz,
															unsigned int damping_pulse_duration_after_rf_pulse,
															unsigned int delay_before_rf_pulses
)
{
unsigned int result = 0;
volatile double tmp;
unsigned int resonance_frq_ofs_num, adc_frq_ofs_num;
unsigned char adc_clk_period = 1;//mks - Should be 1/Fclk, where Fclk is RF channel 4 frequency (ADC sampling frequency)

/*
resonance_frq_ofs_hz					= 0;
rf_pulse_duration						= 100;//mks
nmr_aq_time								= 1000;//mks
noise_aq_time							= 1000;//mks
dead_time								= 10;//mks
adc_frq_ofs_hz							= 0;//Hz
damping_pulse_duration_after_rf_pulse	= 5;//mks
delay_before_rf_pulses					= 10000;//mks
*/

noise_aq_time -= 1;
nmr_aq_time   -= 1;
rf_pulse_duration -= 1;
dead_time -= damping_pulse_duration_after_rf_pulse;
dead_time -= 1;
damping_pulse_duration_after_rf_pulse -= 1;
delay_before_rf_pulses -= 1;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = resonance_frq_ofs_hz/tmp;
resonance_frq_ofs_num = tmp;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = adc_frq_ofs_hz/tmp;
adc_frq_ofs_num = tmp;
//*********************************************************************************************
proger_stop ();
proger_mem_init();
proger_reset_comm_fifo_counter();

//proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27, 0x0F); // 10 ms interval for aq info writes (for MM)
//*********************************************************************************************
//commands of how to NMR and other data will be processed should be placed here
//*********************************************************************************************
// Example of internal buffer usage for MM stack transfer to processor,
// it transfers 12 bytes - from 0 to 11.
proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 10); // 2 mks interval
proger_wr_comm_to_fifo ( COM_WR_RST_PROC_MEM, 0, 1, 2);		//first write
proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 3, 4, 5);		//others writes
proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 6, 7, 8);		//others writes
proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 9, 10, 11);	//others writes
/**/
//Здесь должна быть функция, сообщающая процессору о том, что пора забирать данные из буфера с инструкциями в ПЛИС

proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer
//simple delay, nothing happens during this interval

proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0


proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_3_ACT);
proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_2_ACT);

proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms delay
proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_0, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_0_ACT);
proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 1 second last delay
proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_0, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_0_DEACT);

//*********************************************************************************************
// Initialization of frequency channels and so on
//*********************************************************************************************
/*
proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 5); // 6 mks interval
proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0);	// test configuration sets to 0 - normal acquisition, ADC sampled signal instead of counter

proger_wr_comm_to_fifo ( COM_SET_VC_DAC_1, 0, 0, 0);	// sets voltage on DAC1 for resonant circuit varicaps
proger_wr_comm_to_fifo ( COM_SET_VC_DAC_2, 0, 0, 0);	// sets voltage on DAC2 for resonant circuit varicaps

proger_wr_comm_to_fifo ( COM_SWITCH_FRQ, 0, 0, 0);	// set RF frequencies to Larmor frequency #0
proger_wr_comm_to_fifo ( COM_OFS01, resonance_frq_ofs_num >> 2, resonance_frq_ofs_num >> 1, resonance_frq_ofs_num );	// set offset of RF channel 1 to 0
proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0
proger_wr_comm_to_fifo ( COM_OFS04, adc_frq_ofs_num >> 2, adc_frq_ofs_num >> 1, adc_frq_ofs_num );	// set offset of RF channel 4 to 0 (ADC sampling frequency)

proger_wr_comm_to_fifo ( COM_PH01, 0, 0, 0);	// set phase of RF channel 1 to 0
proger_wr_comm_to_fifo ( COM_PH02, 0, 0, 0);	// set phase of RF channel 2 to 0
proger_wr_comm_to_fifo ( COM_PH03, 0, 0, 0);	// set phase of RF channel 3 to 0
proger_wr_comm_to_fifo ( COM_PH04, 0, 0, 0);	// set phase of RF channel 4 to 0 (ADC sampling frequency)

proger_wr_comm_to_fifo ( COM_SET_PA_ADDR, 0, 0, 0);	// set address of currently activated power amplifier
proger_wr_comm_to_fifo ( COM_SET_RCV_ADDR, 0, 0, 0);	// set address of currently activated receiver

proger_wr_comm_to_fifo ( COM_PH_RST, 0, 0, 0);	// synchronization of all RF channels (should be used only once)
*/

	//*********************************************************************************************
	// RF pulses window and acquisition window begin
	//*********************************************************************************************

/*	proger_wr_comm_to_fifo ( COM_TIME, delay_before_rf_pulses >> 2, delay_before_rf_pulses >> 1, delay_before_rf_pulses); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_3_ACT);
	proger_wr_comm_to_fifo ( COM_TIME, delay_before_rf_pulses >> 2, delay_before_rf_pulses >> 1, delay_before_rf_pulses); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_2_ACT);
	//simple delay, nothing happens during this interval
*/
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
//Счетчик ЗДЕЗ!
		proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 1);


	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
		proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
		proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
		proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0x01, 0x2C);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
		proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0x1F);
		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_ACT);
		proger_wr_comm_to_fifo ( COM_WR_RST_PROC_MEM, 0, 1, 2);		//first write
		proger_wr_comm_to_fifo ( COM_WR_INC_PROC_MEM, 3, 4, 5);		//others writes
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0xE7);//1ms
		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_DEACT);
		proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end
		proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0x01, 0xF3);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_2_DEACT);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
		proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_GPIO_3_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program



	//*********************************************************************************************
	// Noise acquisition
	//*********************************************************************************************
/*	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0); // 1 mks interval for starting receiver dumping pulse
	proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_ON); // Receiver damping pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, rf_pulse_duration >> 2, rf_pulse_duration >> 1, rf_pulse_duration); // time interval for dummy RF pulse
	proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // dummy RF pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, damping_pulse_duration_after_rf_pulse >> 2, damping_pulse_duration_after_rf_pulse >> 1, damping_pulse_duration_after_rf_pulse); // 5 mks interval for receiver dumping pulse
	proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // dummy RF pulse end

	proger_wr_comm_to_fifo ( COM_TIME, dead_time >> 2, dead_time >> 1, dead_time); // 5 mks interval for receiver to comes out of damping and to shutdown the power supplies
	proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_OFF); // Damping pulse end
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, noise_aq_time >> 2, noise_aq_time >> 1, noise_aq_time); // Acquisition time

	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_ACT); // ADC pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 8 * adc_clk_period - 1); // 8 ADC CLKs interval for ADC to complete the acquisition
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_DEACT); // ADC pulse end
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end

	//Здесь закончилось окно АЦП, процессор должен обрабатывать данные с шумом
*/

	//*********************************************************************************************
	// Signal acquisition
	//*********************************************************************************************
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0); // 1 mks interval for starting receiver dumping pulse
proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0x01, 0x2C);
	proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_ON); // Receiver damping pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, rf_pulse_duration >> 2, rf_pulse_duration >> 1, rf_pulse_duration); // time interval for dummy RF pulse
	proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, damping_pulse_duration_after_rf_pulse >> 2, damping_pulse_duration_after_rf_pulse >> 1, damping_pulse_duration_after_rf_pulse); // 5 mks interval for receiver dumping pulse
	proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end

	proger_wr_comm_to_fifo ( COM_TIME, dead_time >> 2, dead_time >> 1, dead_time); // 5 mks interval for receiver to comes out of damping and to shutdown the power supplies
	proger_wr_comm_to_fifo ( COM_ENB_DAMP, 0, 0, ARG_DAMP_OFF); // Damping pulse end
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, noise_aq_time >> 2, noise_aq_time >> 1, noise_aq_time); // Acquisition time
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_ACT); // ADC pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 8 * adc_clk_period - 1); // 8 ADC CLKs interval for ADC to complete the acquisition
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 0, ARG_GPIO_1_DEACT); // ADC pulse end
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end
proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0x01, 0x2C);
	//Здесь закончилось окно АЦП, процессор должен обрабатывать данные с сигналом


	//*********************************************************************************************
	// RF pulses window and acquisition window end
	//*********************************************************************************************

	//Здесь должны стоять функции, сообщающие, процессору об окончании полного окна АЦП и об окончании окна РЧ
	proger_wr_comm_to_fifo ( COM_TIME, delay_before_rf_pulses >> 2, delay_before_rf_pulses >> 1, delay_before_rf_pulses); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_2_DEACT);
	proger_wr_comm_to_fifo ( COM_TIME, delay_before_rf_pulses >> 2, delay_before_rf_pulses >> 1, delay_before_rf_pulses); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_GPIO_3_DEACT);

	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_0, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_0_DEACT);

	//*********************************************************************************************
	// Relaxation delay
	//*********************************************************************************************
	//proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program
//*********************************************************************************************

return(result);
}// main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style

int main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style_windows_reserv2 (
															double       resonance_frq_ofs_hz,
															unsigned int rf_pulse_duration,
															unsigned int nmr_aq_time,
															unsigned int noise_aq_time,
															unsigned int dead_time,
															double       adc_frq_ofs_hz,
															unsigned int damping_pulse_duration_after_rf_pulse,
															unsigned int delay_before_rf_pulses
)
{
unsigned int result = 0;
volatile double tmp;
unsigned int resonance_frq_ofs_num, adc_frq_ofs_num;
unsigned char adc_clk_period = 1;//mks - Should be 1/Fclk, where Fclk is RF channel 4 frequency (ADC sampling frequency)

/*
resonance_frq_ofs_hz					= 0;
rf_pulse_duration						= 100;//mks
nmr_aq_time								= 1000;//mks
noise_aq_time							= 1000;//mks
dead_time								= 10;//mks
adc_frq_ofs_hz							= 0;//Hz
damping_pulse_duration_after_rf_pulse	= 5;//mks
delay_before_rf_pulses					= 10000;//mks
*/

noise_aq_time -= 1;
nmr_aq_time   -= 1;
rf_pulse_duration -= 1;
dead_time -= damping_pulse_duration_after_rf_pulse;
dead_time -= 1;
damping_pulse_duration_after_rf_pulse -= 1;
delay_before_rf_pulses -= 1;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = resonance_frq_ofs_hz/tmp;
resonance_frq_ofs_num = tmp;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = adc_frq_ofs_hz/tmp;
adc_frq_ofs_num = tmp;
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//Счетчик ЗДЕЗ!
	proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0x01);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer
	//simple delay, nothing happens during this interval

	proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
	proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_3_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms
//	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_GPIO_1_ACT);
	proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
//	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_GPIO_1_DEACT);

	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin
	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_2_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0x01, 0x2C); // 300

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0x1F);// 800 mks
	//proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300 ms
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0,	99);// 100 us
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_1_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0xE7);// 1ms
	//proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_1_DEACT);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end
	proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0x00, 0x00);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_2_DEACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_GPIO_3_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program

return(result);
}// main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style


int main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style_windows_reserv3_before_adding_Dielec (
															double       resonance_frq_ofs_hz,
															unsigned int rf_pulse_duration,
															unsigned int nmr_aq_time,
															unsigned int noise_aq_time,
															unsigned int dead_time,
															double       adc_frq_ofs_hz,
															unsigned int damping_pulse_duration_after_rf_pulse,
															unsigned int delay_before_rf_pulses
)
{
unsigned int result = 0;
volatile double tmp;
unsigned int resonance_frq_ofs_num, adc_frq_ofs_num;
unsigned char adc_clk_period = 1;//mks - Should be 1/Fclk, where Fclk is RF channel 4 frequency (ADC sampling frequency)

/*
resonance_frq_ofs_hz					= 0;
rf_pulse_duration						= 100;//mks
nmr_aq_time								= 1000;//mks
noise_aq_time							= 1000;//mks
dead_time								= 10;//mks
adc_frq_ofs_hz							= 0;//Hz
damping_pulse_duration_after_rf_pulse	= 5;//mks
delay_before_rf_pulses					= 10000;//mks
*/

noise_aq_time -= 1;
nmr_aq_time   -= 1;
rf_pulse_duration -= 1;
dead_time -= damping_pulse_duration_after_rf_pulse;
dead_time -= 1;
damping_pulse_duration_after_rf_pulse -= 1;
delay_before_rf_pulses -= 1;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = resonance_frq_ofs_hz/tmp;
resonance_frq_ofs_num = tmp;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = adc_frq_ofs_hz/tmp;
adc_frq_ofs_num = tmp;
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//Счетчик ЗДЕЗ!
	//proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0x01);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer
	//simple delay, nothing happens during this interval

	proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
	proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_3_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms
//	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_GPIO_1_ACT);
	proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
//	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_GPIO_1_DEACT);

	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin
	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_2_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0x01, 0x2C); // 300

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0x1F);// 800 mks
	//proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300 ms
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0,	99);// 100 us
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_1_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0xE7);// 1ms
	//proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_1_DEACT);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end
	proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0x00, 0x00);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_2_DEACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_GPIO_3_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program

return(result);
}// main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style_windows_reserv3_before_adding_Dielec


int main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style_windows (
															double       resonance_frq_ofs_hz,
															unsigned int rf_pulse_duration,
															unsigned int nmr_aq_time,
															unsigned int noise_aq_time,
															unsigned int dead_time,
															double       adc_frq_ofs_hz,
															unsigned int damping_pulse_duration_after_rf_pulse,
															unsigned int delay_before_rf_pulses
)
{
unsigned int result = 0;
volatile double tmp;
unsigned int resonance_frq_ofs_num, adc_frq_ofs_num;
unsigned char adc_clk_period = 1;//mks - Should be 1/Fclk, where Fclk is RF channel 4 frequency (ADC sampling frequency)

/*
resonance_frq_ofs_hz					= 0;
rf_pulse_duration						= 100;//mks
nmr_aq_time								= 1000;//mks
noise_aq_time							= 1000;//mks
dead_time								= 10;//mks
adc_frq_ofs_hz							= 0;//Hz
damping_pulse_duration_after_rf_pulse	= 5;//mks
delay_before_rf_pulses					= 10000;//mks
*/

noise_aq_time -= 1;
nmr_aq_time   -= 1;
rf_pulse_duration -= 1;
dead_time -= damping_pulse_duration_after_rf_pulse;
dead_time -= 1;
damping_pulse_duration_after_rf_pulse -= 1;
delay_before_rf_pulses -= 1;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = resonance_frq_ofs_hz/tmp;
resonance_frq_ofs_num = tmp;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = adc_frq_ofs_hz/tmp;
adc_frq_ofs_num = tmp;
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//Счетчик ЗДЕЗ!
	//proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0x01);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer
	//simple delay, nothing happens during this interval

	proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
	proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_GPIO_3_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_GPIO_1_ACT);
	proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_GPIO_1_DEACT);


	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin
	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_2_ACT);
	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0x01, 0x2C); // 300

//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);// 100 us
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_1_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0xE7);// 1ms
	//proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);

	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_1_DEACT);
//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 10);

	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end
	proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0x00, 0x00);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_GPIO_2_DEACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_GPIO_3_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program

return(result);
}// main_proger_wr_pulseprog_test_GPIO_49q_ADC_old_style

int main_proger_wr_pulseprog_test_GPIO_51_ADC_old_style_windows (
															double       resonance_frq_ofs_hz,
															unsigned int rf_pulse_duration,
															unsigned int nmr_aq_time,
															unsigned int noise_aq_time,
															unsigned int dead_time,
															double       adc_frq_ofs_hz,
															unsigned int damping_pulse_duration_after_rf_pulse,
															unsigned int delay_before_rf_pulses
)
{
unsigned int result = 0;
volatile double tmp;
unsigned int resonance_frq_ofs_num, adc_frq_ofs_num;
unsigned char adc_clk_period = 1;//mks - Should be 1/Fclk, where Fclk is RF channel 4 frequency (ADC sampling frequency)

/*
resonance_frq_ofs_hz					= 0;
rf_pulse_duration						= 100;//mks
nmr_aq_time								= 1000;//mks
noise_aq_time							= 1000;//mks
dead_time								= 10;//mks
adc_frq_ofs_hz							= 0;//Hz
damping_pulse_duration_after_rf_pulse	= 5;//mks
delay_before_rf_pulses					= 10000;//mks
*/

noise_aq_time -= 1;
nmr_aq_time   -= 1;
rf_pulse_duration -= 1;
dead_time -= damping_pulse_duration_after_rf_pulse;
dead_time -= 1;
damping_pulse_duration_after_rf_pulse -= 1;
delay_before_rf_pulses -= 1;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = resonance_frq_ofs_hz/tmp;
resonance_frq_ofs_num = tmp;

tmp = DDS_BASIC_FRQ / 4294967296;
tmp = adc_frq_ofs_hz/tmp;
adc_frq_ofs_num = tmp;
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//Счетчик ЗДЕЗ!
	//proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0x01);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer
	//simple delay, nothing happens during this interval

	proger_wr_comm_to_fifo ( COM_OFS02, 0, 0, 0);	// set offset of RF channel 2 to 0
	proger_wr_comm_to_fifo ( COM_OFS03, 0, 0, 0);	// set offset of RF channel 3 to 0

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_STB_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_STB_DEACT);
	proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_DIELEC, ARG_STB_DEACT);


	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_ON); // RF pulse begin
	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 0);
	//proger_wr_comm_to_fifo ( COM_RF01_ON_OFF, 0, 0, ARG_RF_OFF); // RF pulse end
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_STB_ACT);
	//proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_CYCLE_OPEN, 0, 0x01, 0x2C); // 300

//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);
	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_OFF); // Blanking power supplies pulse begin

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);// 100 us
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_STB_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0xE7);// 1ms
	//proger_wr_comm_to_fifo ( COM_TIME, 0x01, 0x86, 0x9F); // 100 ms
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);

	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_STB_DEACT);
//proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 10);

	proger_wr_comm_to_fifo ( COM_BLANK_PWR, 0, 0, ARG_PWR_ON); // Blanking power supplies pulse end
	proger_wr_comm_to_fifo ( COM_CYCLE_CLOSE, 0, 0x00, 0x00);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_2, ARG_AWAKE_LED_ON, GPIO_5_12_ADC, ARG_STB_DEACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_STB_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program

return(result);
}// main_proger_wr_pulseprog_test_GPIO_51_ADC_old_style

int main_proger_wr_pulseprog_test_spi_51_m ()
{
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0, 1, 0xF4);//500
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_SET_VC_DAC_1, 0x00, 0x7F, 0xFF);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 1, 0xF4);//500
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_SET_VC_DAC_1, 0x00, 0xFF, 0xFF);


	proger_wr_comm_to_fifo ( COM_STOP, 0, 0, 0);	// set offset of RF channel 3 to 0

	return(0);

}// main_proger_wr_pulseprog_test_spi_51_m

int main_proger_wr_pulseprog_test_gamma_counter_51_r ()
{
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_GAMMA, ARG_STB_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_GAMMA, ARG_STB_DEACT);

	proger_wr_comm_to_fifo ( COM_STOP, 0, 0, 0);

	return(0);

}// main_proger_wr_pulseprog_test_gamma_counter_51_r

int main_proger_wr_pulseprog_default ()
{
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	//Счетчик ЗДЕЗ!
	//proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0x01);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_STB_ACT);

	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_STB_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program

return(0);
}// main_proger_wr_pulseprog_default

int main_proger_wr_pulseprog_test_default ()
{
//*********************************************************************************************
	proger_stop ();
	proger_mem_init();
	proger_reset_comm_fifo_counter();

	//Счетчик ЗДЕЗ!
	//proger_wr_comm_to_fifo ( COM_TEST_CONF_WR, 0, 0, 0x01);

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99); // 100 mks interval for processor to read buffer

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_ON, 0x00, ARG_STB_ACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0, 0, 99);// 100 us
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, 200, ARG_STB_ACT);
	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x03,	0xE7);// 1ms
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_1, ARG_AWAKE_LED_ON, GPIO_5_12_PACK_03, ARG_STB_DEACT);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 1);
	proger_wr_comm_to_fifo ( COM_SYNC, 0, 0, 0);


	proger_wr_comm_to_fifo ( COM_TIME, 0x04, 0x93, 0xDF); // 300ms

	proger_wr_comm_to_fifo ( COM_TIME, 0, 0x27,	0x0F); // 10 ms interval for delay before real RF pulses will be started (needed to Andrey program to finish current data exchange with computer)
	proger_wr_comm_to_fifo ( COM_PROC_CONTROL_STB_3, ARG_AWAKE_LED_OFF, 0x00, ARG_STB_DEACT);


	proger_wr_comm_to_fifo ( COM_TIME, 0x0F, 0x42, 0x3F); // 1 second last delay
	//simple delay, nothing happens during this interval
	proger_wr_comm_to_fifo ( COM_TIME, 0x00, 0x00, 0x00);// 1 mks interval
	proger_wr_comm_to_fifo ( COM_RET, 0x00, 0x00, 0x00); // restart pulse program

return(0);
}// main_proger_wr_pulseprog_test_default

