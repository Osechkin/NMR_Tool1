/*
 * OMAPL138_global.h
 *
 *  Created on: 23.10.2013
 *      Author: Andrey
 */
#ifndef OMAPL138_GLOBAL_H_
#define OMAPL138_GLOBAL_H_

#include <ti/pspiom/cslr/csl_types.h>
#include <ti/pspiom/cslr/soc_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_syscfg0_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_psc_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_pllc_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_gpio.h>
#include <ti/pspiom/cslr/cslr_i2c.h>
#include <ti/pspiom/cslr/cslr_tmr.h>
#include <ti/pspiom/cslr/cslr_uart.h>
#include <ti/pspiom/cslr/cslr_edma3cc.h>
#include <ti/pspiom/cslr/cslr_cache.h>
#include <ti/pspiom/cslr/cslr_aintc.h>
#include <ti/pspiom/cslr/cslr_dspintc.h>
//#include <sharedCpuVars.h>
#include <ti/pspiom/cslr/tistdtypes.h>



#define SHARED_CPU_VARS_MEM 0x8001FC00


#define True			1
#define False			0


/*typedef struct{
	volatile Bool HEARTBEAT;
	volatile Bool DSP_FUNC1;
	volatile Bool DSP_FUNC2;
	volatile Bool DSP_FUNC3;
	volatile Bool DSP_FUNC4;
} sharedCpuVars;*/

//typedef volatile sharedCpuVars *sharedCpuVarsPtr;

typedef volatile void (*ISRpointer);


extern void intcVectorTable (void);

void enable_all_INTC(int count, int *INTCs);
void enableSYSCFG_Regs();
void disableSYSCFG_Regs();
void disableARM();
void enableCacheL1();
void enableCacheL2();
void disableCache();
void setupDDR2Cache();
//void soft_delay(unsigned int count);

/*-----------------------------------------------------------------------------
 *
 * 									DEFINES
 *
 *---------------------------------------------------------------------------*/
// Generic
#define BYTE_SIZE 		8
#define BYTE_MASK 		0xFF


// OMAPL1x SYSCFG Specific
#define KICK0_KEY 0x83E70B13
#define KICK1_KEY 0x95A4F1E0
#define KICK_LOCK 0x00000000

// OMAPL1x PSC Specific
#define pscTimeout 10240

// OMAPL1x DSP Specific
#define DSPINTC_IST_NMI	(1 << 1)
#define DSPINTC_IST_INT4 (1 << 4)
#define DSPINTC_IST_INT5 (1 << 5)
#define DSPINTC_IST_INT6 (1 << 6)
#define DSPINTC_IST_INT7 (1 << 7)
#define DSPINTC_IST_INT8 (1 << 8)
#define DSPINTC_IST_INT9 (1 << 9)
#define DSPINTC_IST_INT10 (1 << 10)
#define DSPINTC_IST_INT11 (1 << 11)
#define DSPINTC_IST_INT12 (1 << 12)
#define DSPINTC_IST_INT13 (1 << 13)
#define DSPINTC_IST_INT14 (1 << 14)
#define DSPINTC_IST_INT15 (1 << 15)
#define DSPINTC_IST_ALL_MASK_INT 0xFFF0

// SWI Key to Unlock ARM (Not Specific to OMAPL1x/ARM9)
#define ARM_PRIV_MODE_KEY 327680

/* OMAPL138 AINTC System Interrupt Assignments */
#define NUM_AINTC_EVENTID 101
#define AINTC_SYSINT_0_31_ALL 0xFFFFFFFF
#define AINTC_SYSINT_32_63_ALL 0xFFFFFFFF
#define AINTC_SYSINT_64_95_ALL 0xFFFFFFFF
#define AINTC_SYSINT_96_101_ALL 0x0000003F

#define AINTC_EVENTID_COMMTX					0
#define AINTC_EVENTID_COMMRX					1
#define AINTC_EVENTID_NINT						2
#define AINTC_EVENTID_PRU_EVTOUT0				3
#define AINTC_EVENTID_PRU_EVTOUT1				4
#define AINTC_EVENTID_PRU_EVTOUT2				5
#define AINTC_EVENTID_PRU_EVTOUT3				6
#define AINTC_EVENTID_PRU_EVTOUT4				7
#define AINTC_EVENTID_PRU_EVTOUT5				8
#define AINTC_EVENTID_PRU_EVTOUT6				9
#define AINTC_EVENTID_PRU_EVTOUT7				10
#define AINTC_EVENTID_EDMA3_0_CC0_INT0			11
#define AINTC_EVENTID_EDMA3_0_CC0_ERRINT		12
#define AINTC_EVENTID_EDMA3_0_TC0_ERRINT		13
#define AINTC_EVENTID_EMIFA_INT					14
#define AINTC_EVENTID_IIC0_INT					15
#define AINTC_EVENTID_MMCSD0_INT0				16
#define AINTC_EVENTID_MMCSD0_INT1				17
#define AINTC_EVENTID_PSC0_ALLINT				18
#define AINTC_EVENTID_RTC_IRQS					19
#define AINTC_EVENTID_SPI0_INT					20
#define AINTC_EVENTID_T64P0_TINT12				21
#define AINTC_EVENTID_T64P0_TINT34				22
#define AINTC_EVENTID_T64P1_TINT12				23
#define AINTC_EVENTID_T64P1_TINT34				24
#define AINTC_EVENTID_UART0_INT					25
#define AINTC_EVENTID_PROTERR					27
#define AINTC_EVENTID_SYSCFG_CHIPINT0			28
#define AINTC_EVENTID_SYSCFG_CHIPINT1			29
#define AINTC_EVENTID_SYSCFG_CHIPINT2			30
#define AINTC_EVENTID_SYSCFG_CHIPINT3			31
#define AINTC_EVENTID_EDMA3_0_TC1_ERRINT		32
#define AINTC_EVENTID_EMAC_C0RXTHRESH			33
#define AINTC_EVENTID_EMAC_C0RX					34
#define AINTC_EVENTID_EMAC_C0TX					35
#define AINTC_EVENTID_EMAC_C0MISC				36
#define AINTC_EVENTID_EMAC_C1RXTHRESH			37
#define AINTC_EVENTID_EMAC_C1RX					38
#define AINTC_EVENTID_EMAC_C1TX					39
#define AINTC_EVENTID_EMAC_C1MISC				40
#define AINTC_EVENTID_DDR2_MEMERR				41
#define AINTC_EVENTID_GPIO_B0INT				42
#define AINTC_EVENTID_GPIO_B1INT				43
#define AINTC_EVENTID_GPIO_B2INT				44
#define AINTC_EVENTID_GPIO_B3INT				45
#define AINTC_EVENTID_GPIO_B4INT				46
#define AINTC_EVENTID_GPIO_B5INT				47
#define AINTC_EVENTID_GPIO_B6INT				48
#define AINTC_EVENTID_GPIO_B7INT				49
#define AINTC_EVENTID_GPIO_B8INT				50
#define AINTC_EVENTID_IIC1_INT					51
#define AINTC_EVENTID_LCDC_INT					52
#define AINTC_EVENTID_UART_INT1					53
#define AINTC_EVENTID_MCASP_INT					54
#define AINTC_EVENTID_PSC1_ALLINT				55
#define AINTC_EVENTID_SPI1_INT					56
#define AINTC_EVENTID_UHPI_ARMINT				57
#define AINTC_EVENTID_USB0_INT					58
#define AINTC_EVENTID_USB1_HCINT				59
#define AINTC_EVENTID_USB1_RWAKEUP				60
#define AINTC_EVENTID_UART2_INT					61
#define AINTC_EVENTID_EHRPWM0					63
#define AINTC_EVENTID_EHRPWM0TZ					64
#define AINTC_EVENTID_EHRPWM1					65
#define AINTC_EVENTID_EHRPWM1TZ					66
#define AINTC_EVENTID_SATA_INT					67
#define AINTC_EVENTID_T64P2_ALL					68
#define AINTC_EVENTID_ECAP0						69
#define AINTC_EVENTID_ECAP1						70
#define AINTC_EVENTID_ECAP2						71
#define AINTC_EVENTID_MMCSD1_INT0				72
#define AINTC_EVENTID_MMCSD1_INT1				73
#define AINTC_EVENTID_T64P0_CMPINT0				74
#define AINTC_EVENTID_T64P0_CMPINT1				75
#define AINTC_EVENTID_T64P0_CMPINT2				76
#define AINTC_EVENTID_T64P0_CMPINT3				77
#define AINTC_EVENTID_T64P0_CMPINT4				78
#define AINTC_EVENTID_T64P0_CMPINT5				79
#define AINTC_EVENTID_T64P0_CMPINT6				80
#define AINTC_EVENTID_T64P0_CMPINT7				81
#define AINTC_EVENTID_T64P1_CMPINT0				82
#define AINTC_EVENTID_T64P1_CMPINT1				83
#define AINTC_EVENTID_T64P1_CMPINT2				84
#define AINTC_EVENTID_T64P1_CMPINT3				85
#define AINTC_EVENTID_T64P1_CMPINT4				86
#define AINTC_EVENTID_T64P1_CMPINT5				87
#define AINTC_EVENTID_T64P1_CMPINT6				88
#define AINTC_EVENTID_T64P1_CMPINT7				89
#define AINTC_EVENTID_ARMCLKSTOPREQ				90
#define AINTC_EVENTID_UPP_ALLINT				91
#define AINTC_EVENTID_VPIF_ALLINT				92
#define AINTC_EVENTID_EDMA3_1_CC0_INT0			93
#define AINTC_EVENTID_EDMA3_1_CC0_ERRINT		94
#define AINTC_EVENTID_EDMA3_1_TC0_ERRINT		95
#define AINTC_EVENTID_T64P3_ALL					96
#define AINTC_EVENTID_MCBSP0_RINT				97
#define AINTC_EVENTID_MCBSP0_XINT				98
#define AINTC_EVENTID_MCBSP1_RINT				99
#define AINTC_EVENTID_MCBSP1_XINT				100

// OMAPL1x EDMA Specific
#define EDMA_EVENT10 10
#define EDMA_EVENT11 11
#define PaRAM_NULL_LINK 0xFFFF


#define CSL_EDMA3CC_REGS CSL_EDMA30CC_0_REGS

// Pointers to OMAPL138 Module Memory Mapped Registers
/*CSL_SyscfgRegsOvly sysRegs;
CSL_PscRegsOvly psc0Regs;
CSL_PscRegsOvly psc1Regs;
CSL_PllcRegsOvly pll0Regs;
CSL_GpioRegsOvly gpioRegs;
CSL_I2cRegsOvly i2c0Regs;
CSL_TmrRegsOvly tmr0Regs;
CSL_TmrRegsOvly tmr1Regs;
CSL_UartRegsOvly uart0Regs;
CSL_UartRegsOvly uart1Regs;
CSL_UartRegsOvly uart2Regs;
CSL_Edma3ccRegsOvly edma3ccRegs;
CSL_CacheRegsOvly cacheRegs;
CSL_AintcRegsOvly aintcRegs;
CSL_DspintcRegsOvly dspintcRegs;*/

// Pointers to OMAPL138 Module Memory Mapped Registers
extern CSL_SyscfgRegsOvly sysRegs;
extern CSL_PscRegsOvly psc0Regs;
extern CSL_PscRegsOvly psc1Regs;
extern CSL_PllcRegsOvly pll0Regs;
extern CSL_GpioRegsOvly gpioRegs;
extern CSL_I2cRegsOvly i2c0Regs;
extern CSL_TmrRegsOvly tmr0Regs;
extern CSL_TmrRegsOvly tmr1Regs;
extern CSL_UartRegsOvly uart0Regs;
extern CSL_UartRegsOvly uart1Regs;
extern CSL_UartRegsOvly uart2Regs;
extern CSL_Edma3ccRegsOvly edma3ccRegs;
extern CSL_CacheRegsOvly cacheRegs;
extern CSL_AintcRegsOvly aintcRegs;
extern CSL_DspintcRegsOvly dspintcRegs;

// Pointer to Shared (DSP/ARM) Variables
/*sharedCpuVarsPtr sharedVars = (sharedCpuVarsPtr)(SHARED_CPU_VARS_MEM);

// ARM System Interrupt Table (Map System Interrupts to Corresponding ISRs)
ISRpointer sysISRtbl[NUM_AINTC_EVENTID] = {NULL};

// Global PSC Variables
volatile Bool modulesEnabled;

// Global UART/I2C Variables (Use with Interrupts)
volatile Bool dataUnavailable;
volatile Bool transmitterFull;

// Global ARM/DSP Variables
volatile Bool dspIsBusy;
volatile Bool remainIDLE;

// Global EDMA3 PaRAM OPT Variables
volatile Uint8 optTCC;*/



#endif /* OMAPL138_GLOBAL_H_ */
