/*
 * upp.c
 *
 *  Created on: 16.05.2014
 *      Author: aivanov
 */

//#include <UART_interrupt_loopback_dspL138.h>

#include <stdio.h>
#include <c6x.h>
#include "OMAPL138_common.h"
#include <ti/pspiom/cslr/cslr.h>
#include <ti/pspiom/cslr/cslr_upp.h>
//#define TMS320C6X
//#include <tistdtypes.h>
#include "upp.h"

#include "../proger/proger.h"

//#include "../Common/PSC_common.h"
//#include "../Common/OMAPL138_global.h"

//extern void intcVectorTable (void);

/* ======================================================================================
 * Function: dummyDelay
 * Goal: Create a delay
 * Details: Two "do-nothing" nested for loops; Acutal delay is approx 49.993x usec on
 * 			the DSP running at 300 MHz. To create an accurate delay, the Timer should
 * 			be used (See Timer examples).
 * Arguments: count - An unsigned 32 bit integer which equals the number of instances to
 * 						create an approximate 49.9993 usec delay on the DSP running at
 * 						300 MHz
 * Returns: NA
 * =====================================================================================*/
/**/
void dummyDelay (uint32_t count)
{
	volatile uint32_t tempCount = 0, dummyCount = 0;

	for(tempCount = 0; tempCount < count; tempCount++)
		for (dummyCount = 0; dummyCount < 1066; dummyCount++)
			;
}

void shutdown_ARM (void)
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	CSL_FINS(sysRegs->HOST0CFG, SYSCFG_HOST0CFG_BOOTRDY, 0x00);//added aivanov to shutdown ARM core

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}/* shutdown_ARM */
/*---------------------------------------------------------------------------*/

static void init_upp_tx_clk (void)
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	CSL_FINST(sysRegs->CFGCHIP3, SYSCFG_CFGCHIP3_UPP_TX_CLKSRC, ASYNC3);
reg3 = CSL_FEXT(sysRegs->CFGCHIP3, SYSCFG_CFGCHIP3_UPP_TX_CLKSRC);//0

	CSL_FINST(sysRegs->CFGCHIP3, SYSCFG_CFGCHIP3_ASYNC3_CLKSRC, PLL0);
reg3 = CSL_FEXT(sysRegs->CFGCHIP3, SYSCFG_CFGCHIP3_ASYNC3_CLKSRC);//0

reg3 = CSL_FEXT(sysRegs->CFGCHIP0, SYSCFG_CFGCHIP0_PLL_MASTER_LOCK);//1

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}/* init_upp_tx_clk */
/*---------------------------------------------------------------------------*/

/*
static void set_upp_emul_DSP (void)
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	// Configure UPP with the DSP CPU (emulator will halt UPP then halting DSP core)
	CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_UPPSRC, DSP);

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}// set_upp_emul_DSP */
/*---------------------------------------------------------------------------*/

static void set_upp_emul_ARM (void)
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	// Configure UPP with the DSP CPU (emulator will halt UPP then halting DSP core)
	CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_UPPSRC, ARM);

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}/* set_upp_emul_ARM */
/*---------------------------------------------------------------------------*/


static void init_upp_pins (void)
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	// Configure UPP Pins
	CSL_FINST(sysRegs->PINMUX13, SYSCFG_PINMUX13_PINMUX13_31_28, CH1_WAIT);
	CSL_FINST(sysRegs->PINMUX13, SYSCFG_PINMUX13_PINMUX13_27_24, CH1_ENABLE);
	CSL_FINST(sysRegs->PINMUX13, SYSCFG_PINMUX13_PINMUX13_23_20, CH1_START);
	CSL_FINST(sysRegs->PINMUX13, SYSCFG_PINMUX13_PINMUX13_19_16, CH1_CLK);

	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_31_28, UPP_D10);
	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_27_24, UPP_D11);
	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_23_20, UPP_D12);
	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_19_16, UPP_D13);
	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_15_12, UPP_D14);
	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_11_8,  UPP_D15);
	CSL_FINST(sysRegs->PINMUX14, SYSCFG_PINMUX14_PINMUX14_3_0,   DEFAULT);// same as upp 2x TX clock

	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_31_28, UPP_D2);
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_27_24, UPP_D3 );
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_23_20, UPP_D4);
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_19_16, UPP_D5);
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_15_12, UPP_D6);
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_11_8,  UPP_D7);
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_7_4,   UPP_D8);
	CSL_FINST(sysRegs->PINMUX15, SYSCFG_PINMUX15_PINMUX15_3_0,   UPP_D9);

	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_31_28, UPP_XD10);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_27_24, UPP_XD11);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_23_20, UPP_XD12);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_19_16, UPP_XD13);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_15_12, UPP_XD14);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_11_8,  UPP_XD15);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_7_4,   UPP_D0);
	CSL_FINST(sysRegs->PINMUX16, SYSCFG_PINMUX16_PINMUX16_3_0,   UPP_D1);

	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_31_28, UPP_XD2);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_27_24, UPP_XD3);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_23_20, UPP_XD4);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_19_16, UPP_XD5);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_15_12, UPP_XD6);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_11_8,  UPP_XD7);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_7_4,   UPP_XD8);
	CSL_FINST(sysRegs->PINMUX17, SYSCFG_PINMUX17_PINMUX17_3_0,   UPP_XD9);

	CSL_FINST(sysRegs->PINMUX18, SYSCFG_PINMUX18_PINMUX18_23_20, CH0_WAIT);
	CSL_FINST(sysRegs->PINMUX18, SYSCFG_PINMUX18_PINMUX18_19_16, CH0_ENABLE);
	CSL_FINST(sysRegs->PINMUX18, SYSCFG_PINMUX18_PINMUX18_15_12, CH0_START);
	CSL_FINST(sysRegs->PINMUX18, SYSCFG_PINMUX18_PINMUX18_11_8,  CH0_CLK);
	CSL_FINST(sysRegs->PINMUX18, SYSCFG_PINMUX18_PINMUX18_7_4,   UPP_XD0);
	CSL_FINST(sysRegs->PINMUX18, SYSCFG_PINMUX18_PINMUX18_3_0,   UPP_XD1);

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}/* init_upp_pins */
/*---------------------------------------------------------------------------*/

static void enable_upp_module_clocks ()
{
	modulesEnabled = FALSE;

	// Ensure previous initiated transitions have finished
	if(check_psc_transition(CSL_PSC_1) == pscTimeout) return;

	// Enable peripherals; Initiate transition
	CSL_FINST(psc1Regs->MDCTL[CSL_PSC_UPP], PSC_MDCTL_NEXT, ENABLE);
	CSL_FINST(psc1Regs->PTCMD, PSC_PTCMD_GO0, SET);

	// Ensure previous initiated transitions have finished
	if(check_psc_transition(CSL_PSC_1) == pscTimeout) return;

	// Ensure modules enabled
	if(check_psc_MDSTAT(CSL_PSC_1, CSL_PSC_UPP, CSL_PSC_MDSTAT_STATE_ENABLE) == pscTimeout) return;

	modulesEnabled = TRUE;
}/* enable_upp_module_clocks */
/*---------------------------------------------------------------------------*/

int reset_upp ()
{
	int UPP_resetted = FALSE;
	volatile int i = 0;

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_EN, DISABLE);

	CSL_FINST(UPP0Regs->UPIER, UPP_UPIER_EOWI, TRUE);

	while( (!(CSL_FEXT(UPP0Regs->UPPCR, UPP_UPPCR_DB) == CSL_UPP_UPPCR_DB_IDLE)) && (i != UPP_RESET_TIMEOUT) )
	{
		i++;
	}

	if ( i != UPP_RESET_TIMEOUT)
	{
		UPP_resetted = TRUE;
	}
	else
	{
		UPP_resetted = FALSE;//return(UPP_resetted);
	}

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_SWRST, RESET);
	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_SWRST, RUNNING);

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_EN, ENABLE);


	return(UPP_resetted);
}/* reset_upp */
/*---------------------------------------------------------------------------*/

int upp_reset_soft ()
{
	volatile Bool UPP_resetted = FALSE;
	volatile unsigned long i = 0, tmp;

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_EN, DISABLE);

	tmp = CSL_FEXT(UPP0Regs->UPPCR, UPP_UPPCR_DB);

	while( (!(CSL_FEXT(UPP0Regs->UPPCR, UPP_UPPCR_DB) == CSL_UPP_UPPCR_DB_IDLE)) || (i == UPP_RESET_TIMEOUT) )
	{
		i++;
	}

	if ( i != UPP_RESET_TIMEOUT)
	{
		UPP_resetted = TRUE;
	}
	else
	{
		UPP_resetted = FALSE;
	}

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_SWRST, RESET);
//	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_SWRST, RUNNING);

//	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_EN, ENABLE);


	return(UPP_resetted);
}/* upp_reset_soft */
/*---------------------------------------------------------------------------*/

static void init_upp_channels ()
{

	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_MODE,     DUPLEX0); //ch. A - RX, ch. B - TX.
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_CHN,      TWO); //both ch-s en-d
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_SDRTXIL,  DISABLE); //both DMA ch-s en-d
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DDRDEMUX, DISABLE); //both DMA ch-s en-d
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DRA,      SINGLE); //SDR, not DDR
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_IWA,      16BIT); //
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DPWA,     FULL); //no data packing
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DPFA,     RJZE); //no sense
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DRB,      SINGLE); //SDR, not DDR
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_IWB,      16BIT); //
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DPWB,     FULL); //no data packing
	CSL_FINST(UPP0Regs->UPCTL, UPP_UPCTL_DPFB,     RJZE); //no sense

	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_STARTPOLA, INVERT);//NORMAL INVERT
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_ENAPOLA,   INVERT);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_WAITPOLA,  INVERT);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_STARTA,    ENABLE);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_ENAA,      ENABLE);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_WAITA,     DISABLE);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_CLKDIVA,   RESETVAL);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_CLKINVA,   NORMAL);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_TRISA,     ENABLE);

	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_STARTPOLB, INVERT);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_ENAPOLB,   INVERT);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_WAITPOLB,  INVERT);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_STARTB,    ENABLE);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_ENAB,      ENABLE);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_WAITB,     DISABLE);
	//CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_CLKDIVB,   RESETVAL);
	CSL_FINS(UPP0Regs->UPICR, UPP_UPICR_CLKDIVB,    0);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_CLKINVB,   NORMAL);
	CSL_FINST(UPP0Regs->UPICR, UPP_UPICR_TRISB,     ENABLE);

	CSL_FINST(UPP0Regs->UPIVR, UPP_UPIVR_VALA, RESETVAL);
	CSL_FINST(UPP0Regs->UPIVR, UPP_UPIVR_VALB, RESETVAL);

	CSL_FINST(UPP0Regs->UPTCR, UPP_UPTCR_RDSIZEI, 64B);
	CSL_FINST(UPP0Regs->UPTCR, UPP_UPTCR_RDSIZEQ, 64B);
	CSL_FINST(UPP0Regs->UPTCR, UPP_UPTCR_TXSIZEA, 64B);
	CSL_FINST(UPP0Regs->UPTCR, UPP_UPTCR_TXSIZEB, 64B);

	CSL_FINST(UPP0Regs->UPDLB, UPP_UPDLB_BA, DISABLE);//ENABLE
	CSL_FINST(UPP0Regs->UPDLB, UPP_UPDLB_AB, DISABLE);

}/* init_upp_channels */
/*---------------------------------------------------------------------------*/

int init_upp ()
{
	//int UPP_resetted = FALSE;
	volatile int i = 0;


	_disable_interrupts();// Intrinsic Function to Disable Interrupts
	//shutdown_ARM();
	init_upp_tx_clk();
	init_upp_pins();
	set_upp_emul_ARM();
	enable_upp_module_clocks();
	//arrays_init ();
	reset_upp();
	init_upp_channels();



//TO DO: setup isr
	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_EN, ENABLE);
//To Do: init DMA



	return(1);

}/* init_upp */
/*---------------------------------------------------------------------------*/

/*
static void looopback_upp ()//BE SURE TO LOAD PROPER FPGA FILE! Or Processor may be damaged! Should be called after init_upp();
{
	CSL_FINST(UPP0Regs->UPDLB, UPP_UPDLB_BA, ENABLE);
}// looopback_upp */
/*---------------------------------------------------------------------------*/


void init_upp_ints ()
{
	CSL_FINST(UPP0Regs->UPIES, UPP_UPIES_EOWI, SET);
	CSL_FINST(UPP0Regs->UPIES, UPP_UPIES_EOLI, SET);
}// init_upp_ints */
/*---------------------------------------------------------------------------*/

/*
static void arrays_init_all ()
{
	volatile unsigned int i = 0;
	for( i = 0; i < 1024*1; i++ )
	{
		in_upp_arr [i] = 0;
		out_upp_arr[i] = i;
	}

}*/
/* arrays_init */
/*---------------------------------------------------------------------------*/

/*
static void arrays_init (unsigned short byte_count, unsigned short line_count)
{
	volatile unsigned int i = 0;
	for( i = 0; i < byte_count*line_count; i++ )
	{
		in_upp_arr [i] = 0xFF;
		out_upp_arr[i] = i;
	}

}// arrays_init_all */
/*---------------------------------------------------------------------------*/

/*
static void array_init_parr_bc_lc (unsigned char *array, unsigned short byte_count, unsigned short line_count)
{
	unsigned int i = 0;
	for( i = 0; i < byte_count*line_count; i++ )
	{
		array [i] = 0x00;
	}

}// array_init_parr_bc_lc */
/*---------------------------------------------------------------------------*/

/*
static void memset_array (unsigned char *array, unsigned char filling_value, unsigned int size)
{
	unsigned int i;
	for( i = 0; i < size; i++ )
	{
		array [i] = filling_value;
	}

}// memset_array */
/*---------------------------------------------------------------------------*/


/*
static void array_send ()
{
	volatile unsigned int i = 0;
	for( i = 0; i < 1024; i++ )
	{
		out_upp_arr[i] = i+1;
	}
}// array_send */
/*---------------------------------------------------------------------------*/

/*
static int in_array_check (unsigned short byte_count, unsigned short line_count)
{
	volatile int i = 0, ret_code = 1;
	for( i = 0; i < byte_count*line_count; i++ )
	{
		if ( in_upp_arr[i] != out_upp_arr[i] )
			ret_code = 0;
	}
	return(ret_code);
}// in_array_check */
/*---------------------------------------------------------------------------*/

/*
static void upp_test_send (unsigned short byte_count, unsigned short line_count)
{
	arrays_init (byte_count, line_count);

	reg1 = (unsigned int) in_upp_arr;
	reg2 = (unsigned int) out_upp_arr;
	//reg3 = CSL_FEXT(pll0Regs->LSR, UART_LSR_THRE)

	UPP0Regs->UPID0 = (unsigned int) in_upp_arr;
reg3 = UPP0Regs->UPID0;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//1
	UPP0Regs->UPID1 = (unsigned int) ( byte_count | (line_count << 16) );//4
reg3 = UPP0Regs->UPID1;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//1
	UPP0Regs->UPID2 = (unsigned int) byte_count;
reg3 = UPP0Regs->UPID2;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//3

	UPP0Regs->UPQD0 = (unsigned int) out_upp_arr;
reg3 = UPP0Regs->UPQD0;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;//2ln?
reg3 = UPP0Regs->UPQS2;//80
	UPP0Regs->UPQD1 = (unsigned int) ( byte_count | (line_count << 16) );
reg3 = UPP0Regs->UPQD1;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;
	UPP0Regs->UPQD2 = (unsigned int) byte_count;
reg3 = UPP0Regs->UPQD2;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;

//dummyDelay(1000);

reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;
}// upp_test_send */
/*---------------------------------------------------------------------------*/

static void upp_rx_start (unsigned short byte_count, unsigned short line_count, unsigned char *upp_data)
{
	//arrays_init (byte_count, line_count);

	//UPP0Regs->UPID0 = (unsigned int) in_upp_arr;

	UPP0Regs->UPID0 = (unsigned int) upp_data;
	UPP0Regs->UPID1 = (unsigned int) ( byte_count | (line_count << 16) );//4
	UPP0Regs->UPID2 = (unsigned int) byte_count;

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_SWRST, RUNNING);

	CSL_FINST(UPP0Regs->UPPCR, UPP_UPPCR_EN, ENABLE);

}/* upp_rx_start */
/*---------------------------------------------------------------------------*/

/*
static void upp_test_send_simple (unsigned short byte_count, unsigned short line_count)
{
	arrays_init (byte_count, line_count);

	reg1 = (unsigned int) in_upp_arr;
	reg2 = (unsigned int) out_upp_arr;
	//reg3 = CSL_FEXT(pll0Regs->LSR, UART_LSR_THRE)

	UPP0Regs->UPID0 = (unsigned int) in_upp_arr;
reg3 = UPP0Regs->UPID0;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//1
	UPP0Regs->UPID1 = (unsigned int) ( byte_count | (line_count << 16) );//4
reg3 = UPP0Regs->UPID1;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//1
	UPP0Regs->UPID2 = (unsigned int) byte_count;
reg3 = UPP0Regs->UPID2;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//3

	UPP0Regs->UPQD0 = (unsigned int) out_upp_arr;
reg3 = UPP0Regs->UPQD0;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;//2ln?
reg3 = UPP0Regs->UPQS2;//80
	UPP0Regs->UPQD1 = (unsigned int) ( byte_count | (line_count << 16) );
reg3 = UPP0Regs->UPQD1;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;
	UPP0Regs->UPQD2 = (unsigned int) byte_count;
reg3 = UPP0Regs->UPQD2;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;

dummyDelay(1000);

reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;
}// upp_test_send_simple */
/*---------------------------------------------------------------------------*/

/*
static void upp_start_test_rx (unsigned short byte_count, unsigned short line_count)
{
	arrays_init (byte_count, line_count);

	reg1 = (unsigned int) in_upp_arr;
	reg2 = (unsigned int) out_upp_arr;
	//reg3 = CSL_FEXT(pll0Regs->LSR, UART_LSR_THRE)

	UPP0Regs->UPID0 = (unsigned int) in_upp_arr;
reg3 = UPP0Regs->UPID0;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//1
	UPP0Regs->UPID1 = (unsigned int) ( byte_count | (line_count << 16) );//4
reg3 = UPP0Regs->UPID1;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//1
	UPP0Regs->UPID2 = (unsigned int) byte_count;
reg3 = UPP0Regs->UPID2;
reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;//3

dummyDelay(1000);

reg3 = UPP0Regs->UPIS0;
reg3 = UPP0Regs->UPIS1;
reg3 = UPP0Regs->UPIS2;
reg3 = UPP0Regs->UPQS0;
reg3 = UPP0Regs->UPQS1;
reg3 = UPP0Regs->UPQS2;
}// upp_test_send_simple */
/*---------------------------------------------------------------------------*/

/*
static int wait_upp_eow ()
{
	int upp_eow = FALSE;
	volatile int i = 0;

	while( (!(CSL_FEXT(UPP0Regs->UPISR, UPP_UPIER_EOWI) == CSL_UPP_UPISR_EOWI_TRUE)) && (i != UPP_EOW_TIMEOUT) )
	{
		i++;
	}

	if ( i != UPP_EOW_TIMEOUT)
	{
		upp_eow = TRUE;
		CSL_FINST(UPP0Regs->UPIER, UPP_UPIER_EOWI, TRUE);
	}
	else
	{
		upp_eow = FALSE;
	}

	return(upp_eow);
}// wait_upp_eow */
/*---------------------------------------------------------------------------*/

/*
static void setup_upp_int_6 ()
{

	// Map upp system interrupts to DSP INT6
	//CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_UPPINT);

	// Assign the address of the IST to the IST pointer
	//ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	//ICR = DSPINTC_IST_ALL_MASK_INT;

	// Enable INT6 interrupts
	//IER |= DSPINTC_IST_NMI | DSPINTC_IST_INT6;

}// setup_upp_int_6 */
/*---------------------------------------------------------------------------*/

void upp_start (unsigned short byte_count, unsigned short line_count, unsigned char *upp_data)
{
	//static unsigned int upp_tests_count = 0;

	upp_rx_start(byte_count, line_count, upp_data);

}/* upp_start */
/*---------------------------------------------------------------------------*/

/*
static void	upp_check_simple(unsigned short byte_count, unsigned short line_count)
{
	static unsigned int upp_tests_count = 0;
	proger_stop();
	upp_test_send_simple(byte_count, line_count);
	proger_start();
	if ( in_array_check(byte_count, line_count) )
		printf("\n\tOK    in UPP check simple # \t%d.\n", ++upp_tests_count);
	else
		printf("\n\tError in UPP check simple # \t%d.\n", ++upp_tests_count);
}
*/

/*
static void	upp_check_poll(unsigned short byte_count, unsigned short line_count)
{
	static unsigned int upp_tests_count = 0;

	proger_stop();
	upp_test_send(byte_count, line_count);
	proger_start();

	if ( wait_upp_eow() )
		printf("");
	else
		printf("\n\tTimeout in UPP check poll # \t\n");

	if ( in_array_check(byte_count, line_count) )
		printf("OK    in UPP check poll # \t%d.\n", ++upp_tests_count);
	else
		printf("Error in UPP check poll # \t%d.\n", ++upp_tests_count);
}*/

/*
static void	upp_check_isr(unsigned short byte_count, unsigned short line_count)
{
	static unsigned int upp_tests_count = 0;

	printf("\n\tUPP check ISR started, waiting the ISR response... # \t%d.\n", ++upp_tests_count);
	printf("");
	upp_test_send(byte_count, line_count);
	dummyDelay(1000);
}*/

/*
static void	upp_check_rx_isr(unsigned short byte_count, unsigned short line_count)
{
	static unsigned int upp_tests_count = 0;

	printf("\n\tUPP check ISR started, waiting the ISR response... # \t%d.\n", ++upp_tests_count);
	printf("");
	upp_test_send(byte_count, line_count);
	unsigned char *proger_adr;
	proger_adr = (unsigned char *) (0x66000000 + 28);
	*proger_adr = 0x00;//start proger
	dummyDelay(4000);
	*proger_adr = 0xFF;//stop proger
}*/
