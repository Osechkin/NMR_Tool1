/*
 * timer_drv.c
 *
 *  Created on: 04.07.2014
 *      Author: Admin
 */

#include "timer_drv.h"


void setup_Timer(CSL_TmrRegsOvly tmrRegs, Timer_Settings timerSettings)
{
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	// Configure Timer0 with the DSP CPU
	if (tmrRegs == tmr0Regs) CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_TIMER64P_0SRC, DSP);
	else if (tmrRegs == tmr1Regs) CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_TIMER64P_1SRC, DSP);
	else return;

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);

	// Set Timer as 32 Bit Unchain
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_TIMMODE, 32BIT_UNCHAIN);

	// Remove Timer0:12 from Reset
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_TIM12RS, NO_RESET);

	// Reset the Counter for Timer0:12
	CSL_FINST(tmrRegs->TIM12, TMR_TIM12_TIM12, RESETVAL);

	// Select Internal Clock for Timer0:12 (24 MHz)
	CSL_FINST(tmrRegs->TCR, TMR_TCR_CLKSRC12, INTERNAL);

	// Set Timer0:12 Period (1 ms)
	CSL_FINS(tmrRegs->PRD12, TMR_PRD12_PRD12, (timerSettings.freq));

	// Disable the New Timer Features
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_PLUSEN, DISABLE);
}/* setup_Timer */

/*void setup_Timer_Intervals(CSL_TmrRegsOvly	tmrRegs, Timer_Settings timerSettings)
{
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);

	// Configure Timer0 with the DSP CPU
	if (tmrRegs == tmr0Regs) CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_TIMER64P_0SRC, DSP);
	else if (tmrRegs == tmr1Regs) CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_TIMER64P_1SRC, DSP);
	else return;

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);

	// Set Timer as 32 Bit Unchain
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_TIMMODE, 32_CHAIN);

	// Remove Timer0:12 from Reset
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_TIM12RS, NO_RESET);
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_TIM34RS, NO_RESET);

	// Reset the Counter for Timer0:12
	//CSL_FINST(tmrRegs->TIM12, TMR_TIM12_TIM12, RESETVAL);

	// Select Internal Clock for Timer0:12 (24 MHz)
	CSL_FINST(tmrRegs->TCR, TMR_TCR_CLKSRC12, INTERNAL);
	CSL_FINST(tmrRegs->TCR, TMR_TCR_CLKSRC34, INTERNAL);

	// Set Timer0:12 Period (1 ms)
	CSL_FINS(tmrRegs->PRD12, TMR_PRD12_PRD12, (timerSettings.freq));

	// Disable the New Timer Features
	CSL_FINST(tmrRegs->TGCR, TMR_TGCR_PLUSEN, DISABLE);
}/* setup_Timer1 */


void setup_Timer_INTC (CSL_TmrRegsOvly tmrRegs, int INTC)
{
	// Map Timer0:12 system interrupts to DSP INT4
	if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4,	CSL_INTC_EVENTID_T64P0_TINT12);
	else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4,	CSL_INTC_EVENTID_T64P1_TINT12);
	else return;

	if (INTC < 4 || INTC > 15) return;

	switch (INTC)
	{
	case 4:
		// Map Timer system interrupts to DSP INT4
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 5:
		// Map Timer system interrupts to DSP INT5
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL5, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL5, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 6:
		// Map Timer system interrupts to DSP INT6
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 7:
		// Map Timer system interrupts to DSP INT7
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL7, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL7, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 8:
		// Map Timer system interrupts to DSP INT8
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL8, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL8, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 9:
		// Map Timer system interrupts to DSP INT9
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 10:
		// Map Timer system interrupts to DSP INT10
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL10, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL10, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 11:
		// Map Timer system interrupts to DSP INT11
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL11, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL11, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 12:
		// Map Timer system interrupts to DSP INT12
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL12, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL12, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 13:
		// Map Timer system interrupts to DSP INT13
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL13, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL13, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 14:
		// Map Timer system interrupts to DSP INT14
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL14, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL14, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	case 15:
		// Map Timer system interrupts to DSP INT15
		if (tmrRegs == tmr0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL15, CSL_INTC_EVENTID_T64P0_TINT12);
		else if (tmrRegs == tmr1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL15, CSL_INTC_EVENTID_T64P1_TINT12);
		break;
	default:
		return;
	}
}// setup_DSP_INTC

void enable_Timer(CSL_TmrRegsOvly tmrRegs)
{
	if (tmrRegs == tmr0Regs) CSL_FINST(tmr0Regs->TCR, TMR_TCR_ENAMODE12, EN_CONT);
	else if (tmrRegs == tmr1Regs) CSL_FINST(tmr1Regs->TCR, TMR_TCR_ENAMODE12, EN_CONT);
	else return;
}

void disable_Timer(CSL_TmrRegsOvly tmrRegs)
{
	if (tmrRegs == tmr0Regs) CSL_FINST(tmr0Regs->TCR, TMR_TCR_ENAMODE12, DISABLE);
	else if (tmrRegs == tmr1Regs) CSL_FINST(tmr1Regs->TCR, TMR_TCR_ENAMODE12, DISABLE);
	else return;
}
