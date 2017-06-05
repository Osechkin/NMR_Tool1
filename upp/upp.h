/*
 * upp.h
 *
 *  Created on: 16.05.2014
 *      Author: aivanov
 */

//#include <UART_interrupt_loopback_dspL138.h>
//#include "upp.c"

#ifndef UPP_H_
#define UPP_H_

#include <stdint.h>
#include <c6x.h>

#include <stdio.h>
#include "OMAPL138_common.h"
#include <ti/pspiom/cslr/cslr.h>
#include <ti/pspiom/cslr/cslr_upp.h>

#include "../proger/proger.h"


#define UPP_DMA_BUF_SIZE 64

#define UPP_RESET_TIMEOUT 1000000// 0xFFFFFFFF
#define UPP_EOW_TIMEOUT 0x00FFFFFF

//CSL_UppRegsOvly UPP0Regs = (CSL_UppRegsOvly)(CSL_UPP_0_REGS);
extern CSL_UppRegsOvly UPP0Regs;

extern unsigned char in_upp_arr[1024*1];
extern unsigned char out_upp_arr[1024*1];
extern volatile  unsigned int reg1, reg2, reg3;
//extern unsigned short UPP_BYTE_COUNT, UPP_LINE_COUNT;


void dummyDelay (uint32_t count);
void shutdown_ARM (void);
static void init_upp_tx_clk (void);
//static void set_upp_emul_DSP (void);
//static void set_upp_emul_ARM (void);


static void init_upp_pins (void);
/*---------------------------------------------------------------------------*/

static void enable_upp_clocks ();
static void enable_upp_module_clocks ();
/*---------------------------------------------------------------------------*/

int reset_upp ();
int upp_reset_soft();

int init_upp ();
static void init_upp_channels ();
void init_upp_ints ();
//static void arrays_init_all ();

static void arrays_init (unsigned short byte_count, unsigned short line_count);
//static void array_init_parr_bc_lc (unsigned char *array, unsigned short byte_count, unsigned short line_count);
//static void memset_array (unsigned char *array, unsigned char filling_value, unsigned int size);

//static void array_send ();
static int in_array_check (unsigned short byte_count, unsigned short line_count);
static void upp_test_send (unsigned short byte_count, unsigned short line_count);
static void upp_rx_start (unsigned short byte_count, unsigned short line_count, unsigned char *upp_data);
static void upp_test_send_simple (unsigned short byte_count, unsigned short line_count);
static void upp_start_test_rx (unsigned short byte_count, unsigned short line_count);
static int wait_upp_eow ();
//static void setup_upp_int_6 ();
void upp_start (unsigned short byte_count, unsigned short line_count, unsigned char *upp_data);
//static void	upp_check_simple(unsigned short byte_count, unsigned short line_count);
//static void	upp_check_poll(unsigned short byte_count, unsigned short line_count);
//static void	upp_check_isr(unsigned short byte_count, unsigned short line_count);
//static void	upp_check_rx_isr(unsigned short byte_count, unsigned short line_count);

//static void looopback_upp ();

#endif /* UPP_H_ */
