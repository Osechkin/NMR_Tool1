/*
 * timer_drv.c
 *
 *  Created on: 30.05.2014
 *      Author: Admin
 */
#include "stdint.h"

#include "clocker_drv.h"


void initClocker(uint64_t _max_val, void (*_ptr_isr)(void), Clocker *_clocker)
{
	_clocker->ptr_isr = _ptr_isr;
	_clocker->counts = 0;
	_clocker->max_val = _max_val;
	_clocker->state = CLR_CREATED;
	_clocker->tag = 0;
}

void startClocker(Clocker *_clocker)
{
	_clocker->counts = 0;
	_clocker->state = CLR_STARTED;
}

void stopClocker(Clocker *_clocker)
{
	if (_clocker->state == CLR_STARTED) _clocker->state = CLR_STOPPED;
}

void setClockerMaxValue(uint64_t _max_val, Clocker *_clocker)
{
	if (_clocker->state != CLR_STARTED) _clocker->max_val = _max_val;
}

uint8_t getClockerState(Clocker *_clocker)
{
	return _clocker->state;
}

uint64_t getClocks(Clocker *_clocker)
{
	return _clocker->counts;
}

void restartClocker(Clocker *_clocker)
{
	_clocker->counts = 0;
	_clocker->state = CLR_STARTED;
}


