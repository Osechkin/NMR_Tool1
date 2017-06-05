/*
 * timer_drv.h
 *
 *  Created on: 5.07.2014
 *      Author: Andrey
 */

// OMAP-L138 Timer driver
// TIMER API & Timer's struct

#ifndef TIMER_DRV_H_
#define TIMER_DRV_H_

#include <c6x.h>
#include "../Common/OMAPL138_global.h"



typedef struct
{
	Uint32 	freq;
	Bool 	enabled;
} Timer_Settings;


void setup_Timer(CSL_TmrRegsOvly tmrRegs, Timer_Settings timerSettings);	// Setup Timer settings
void setup_Timer_INTC (CSL_TmrRegsOvly tmrRegs, int INTC);					// Map Timer system interrupts
void enable_Timer(CSL_TmrRegsOvly tmrRegs);									// Enable Timer Continuously
void disable_Timer(CSL_TmrRegsOvly tmrRegs);								// Disable Timer


#endif /* UART_DRV_H_ */



