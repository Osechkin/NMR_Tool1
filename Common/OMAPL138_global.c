/*
 * OMAPL138_global.c
 *
 *  Created on: 24.10.2013
 *      Author: Andrey
 */

#include <c6x.h>
#include "OMAPL138_global.h"


// Pointers to OMAPL138 Module Memory Mapped Registers
CSL_SyscfgRegsOvly sysRegs = (CSL_SyscfgRegsOvly)(CSL_SYSCFG_0_REGS);
CSL_PscRegsOvly psc0Regs = (CSL_PscRegsOvly)(CSL_PSC_0_REGS);
CSL_PscRegsOvly psc1Regs = (CSL_PscRegsOvly)(CSL_PSC_1_REGS);
CSL_PllcRegsOvly pll0Regs = (CSL_PllcRegsOvly)(CSL_PLLC_0_REGS);
CSL_GpioRegsOvly gpioRegs = (CSL_GpioRegsOvly)(CSL_GPIO_0_REGS);
CSL_I2cRegsOvly i2c0Regs = (CSL_I2cRegsOvly)(CSL_I2C_0_REGS);
CSL_TmrRegsOvly tmr0Regs = (CSL_TmrRegsOvly)(CSL_TMR_0_REGS);
CSL_TmrRegsOvly tmr1Regs = (CSL_TmrRegsOvly)(CSL_TMR_1_REGS);
CSL_UartRegsOvly uart0Regs = (CSL_UartRegsOvly)(CSL_UART_0_REGS);
CSL_UartRegsOvly uart1Regs = (CSL_UartRegsOvly)(CSL_UART_1_REGS);
CSL_UartRegsOvly uart2Regs = (CSL_UartRegsOvly)(CSL_UART_2_REGS);
CSL_Edma3ccRegsOvly edma3ccRegs = (CSL_Edma3ccRegsOvly)(CSL_EDMA3CC_REGS);
CSL_CacheRegsOvly cacheRegs = (CSL_CacheRegsOvly)(CSL_CACHE_0_REGS);
CSL_AintcRegsOvly aintcRegs = (CSL_AintcRegsOvly)(CSL_AINTC_0_REGS);
CSL_DspintcRegsOvly dspintcRegs = (CSL_DspintcRegsOvly)(CSL_INTC_0_REGS);


void enable_all_INTC(int count, int *INTCs)
{
	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = DSPINTC_IST_ALL_MASK_INT;

	unsigned long DSPINTC_IST_INT = 0;

	int i;
	for (i = 0; i < count; i++)
	{
		DSPINTC_IST_INT |= 1 << INTCs[i];
	}

	// Enable INTC interrupts
	IER = DSPINTC_IST_NMI | DSPINTC_IST_INT;
}

void enableSYSCFG_Regs()
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);
}

void disableSYSCFG_Regs()
{
	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}

void disableARM()
{
	CSL_FINS(sysRegs->HOST0CFG, SYSCFG_HOST0CFG_BOOTRDY, 0x00);//added aivanov to shutdown ARM core and UARTs
}

void enableCacheL1()
{
	int stall;

	// Set L1P size to 32K
	CSL_FINST(cacheRegs->L1PCFG, CACHE_L1PCFG_MODE, 32K);
	stall = cacheRegs->L1PCFG;

	// Set L1D size to 32K
	CSL_FINST(cacheRegs->L1DCFG, CACHE_L1DCFG_MODE, 32K);
	stall = cacheRegs->L1DCFG;
/**/
}/* enable_L1 */

/*---------------------------------------------------------------------------*/

void enableCacheL2()
{
	int stall;

	// Set L2 size to 256K
	CSL_FINST(cacheRegs->L2CFG, CACHE_L2CFG_MODE, 256K);
	stall = cacheRegs->L2CFG;
/**/
}/* enable_L2 */

void disableCache()
{
	int stall, counter;

	// Set DDR2 (MAR 192 - 223) as not cacheable
	for(counter = 192; counter < 224; counter++)
		CSL_FINST(cacheRegs->MAR[counter], CACHE_MAR_PC, NOT_CACHEABLE);

	// Disable L1P
	CSL_FINST(cacheRegs->L1PCFG, CACHE_L1PCFG_MODE, DISABLE);
	stall = cacheRegs->L1PCFG;

	// Disable L1D
	CSL_FINST(cacheRegs->L1DCFG, CACHE_L1DCFG_MODE, DISABLE);
	stall = cacheRegs->L1DCFG;

	// Disable L2
	CSL_FINST(cacheRegs->L2CFG, CACHE_L2CFG_MODE, DISABLE);
	stall = cacheRegs->L2CFG;
/**/
}/* disable_cache */

/*---------------------------------------------------------------------------*/

void setupDDR2Cache()
{

 	int counter;
	// Set SDRAM (MAR 192 - 223) as cacheable
	for(counter = 192; counter < 224; counter++)
		CSL_FINST(cacheRegs->MAR[counter], CACHE_MAR_PC, CACHEABLE);
/**/
}/* setup_SDRAM_cache */


// Software delay ~5 mks (for release mode)
/*void soft_delay (unsigned int count)
{
	volatile uint32_t tempCount = 0, dummyCount = 0;

	for(tempCount = 0; tempCount < count; tempCount++)
		for (dummyCount = 0; dummyCount < 107; dummyCount++)
			;
}*/
/*---------------------------------------------------------------------------*/

void CacheL1_enable()
{
	int stall;

	// Set L1P size to 32K
	CSL_FINST(cacheRegs->L1PCFG, CACHE_L1PCFG_MODE, 32K);
	stall = cacheRegs->L1PCFG;

	// Set L1D size to 32K
	CSL_FINST(cacheRegs->L1DCFG, CACHE_L1DCFG_MODE, 32K);
	stall = cacheRegs->L1DCFG;

	stall++;
}/* enable_L1 */
/*---------------------------------------------------------------------------*/

void CacheL2_enable()
{
	int stall, counter;

	// Set SDRAM (MAR 192 - 223) as cacheable
	for(counter = 192; counter < 224; counter++)
		CSL_FINST(cacheRegs->MAR[counter], CACHE_MAR_PC, CACHEABLE);

	// Set L2 size to 256K
	CSL_FINST(cacheRegs->L2CFG, CACHE_L2CFG_MODE, 256K);
	stall = cacheRegs->L2CFG;

	stall++;
}/* enable_L2 */
/*---------------------------------------------------------------------------*/

void CacheALL_disable()
{
	int stall, counter;

	// Set DDR2 (MAR 192 - 223) as not cacheable
	for(counter = 192; counter < 224; counter++)
		CSL_FINST(cacheRegs->MAR[counter], CACHE_MAR_PC, NOT_CACHEABLE);

	// Disable L1P
	CSL_FINST(cacheRegs->L1PCFG, CACHE_L1PCFG_MODE, DISABLE);
	stall = cacheRegs->L1PCFG;

	// Disable L1D
	CSL_FINST(cacheRegs->L1DCFG, CACHE_L1DCFG_MODE, DISABLE);
	stall = cacheRegs->L1DCFG;

	// Disable L2
	CSL_FINST(cacheRegs->L2CFG, CACHE_L2CFG_MODE, DISABLE);
	stall = cacheRegs->L2CFG;

	stall++;
}/* disable_cache */

/*---------------------------------------------------------------------------*/

