/*
 * clocker_drv.h
 *
 *  Created on: 28.05.2014
 *      Author: Andrey
 */

// Clocker API & Clocker's struct
#ifndef CLOCKER_DRV_H_
#define CLOCKER_DRV_H_


#include "stdint.h"


#define CLR_CREATED			0		// initial state (not started yet);
#define CLR_STARTED			1		// clocker started
#define CLR_STOPPED			2		// clocker stopped
#define CLR_FINISHED		3		// clocker finished (when counts == max_val)

#define CLR_MAX_VALUE		0xFFFFFFFFFFFFFFFF	// max value of the clocks

#define CLR_ASYNC_FREQ		24000u	// set timer interrupt period = 24000 ticks = 1 ms (timer frequency = 24 MHz)


typedef struct
{
	void (*ptr_isr)(void);
	volatile uint64_t counts;
	volatile uint64_t max_val;
	volatile uint8_t state;			// Values: CLR_CREATED, CLR_STARTED, CLR_STOPPED, CLR_FINISHED
	volatile uint32_t tag;			// for extra-info
} Clocker;

extern Clocker *app_clocker;
extern Clocker *clocker1;
extern Clocker *clocker2;
extern Clocker *clocker3;
extern Clocker *clocker4;
extern Clocker *clocker5;

extern void app_clocker_ISR(void);
extern void clocker1_ISR(void);
extern void clocker2_ISR(void);
extern void clocker3_ISR(void);
extern void clocker4_ISR(void);
extern void clocker5_ISR(void);


void initClocker(uint64_t _max_val, void (*_ptr_isr)(void), Clocker *_clocker);
void startClocker(Clocker *_clocker);
void stopClocker(Clocker *_clocker);
void setClockerMaxValue(uint64_t _max_val, Clocker *_clocker);
uint8_t getClockerState(Clocker *_clocker);
uint64_t getClocks(Clocker *_clocker);
void restartClocker(Clocker *_clocker);


#endif /* CLOCKER_DRV_H_ */
