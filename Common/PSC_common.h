/*
 * PSC_common.h
 *
 *  Created on: 24.10.2013
 *      Author: Andrey
 */

// PSC API
#ifndef PSC_COMMON_H_
#define PSC_COMMON_H_

#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/cslr/tistdtypes.h>

#include "OMAPL138_global.h"


volatile int pscTimeoutCount;
static CSL_PscRegsOvly pscRegs;

extern Int32 Psc_ModuleClkCtrl(Psc_DevId pscDevId, Uint32 moduleId, Bool clkCtrl);
extern Int32 Psc_getDependencyCount(Psc_DevId pscDevId, Uint32 moduleId);
extern Void Psc_init(Void);

// OMAPL1x PSC0,1 Specific
/*-----------------------------------------------------------------------------
 *
 * 							OMAPL1x PSC Functions
 *
 *---------------------------------------------------------------------------*/
// Arguments: pscController - An integer which represents the PSC controller
// Return: Integer - Value of variable pscTimeoutCount
int check_psc_transition (int pscController)
{
	if(pscController == CSL_PSC_0)
		pscRegs = psc0Regs;
	else if(pscController == CSL_PSC_1)
		pscRegs = psc1Regs;
	else
		return pscTimeout;

	// Reset the PSC Timeout Counter
	pscTimeoutCount = 0;

	// Increment the PSC Timeout Counter While the PSC is in Transition
	while((CSL_FEXT(pscRegs->PTSTAT, PSC_PTSTAT_GOSTAT0)
			== CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION)
			&& (pscTimeoutCount != pscTimeout))
		pscTimeoutCount++;

	// Return the PSC Timeout Counter
	return pscTimeoutCount;
}/* check_psc_transition */

/*---------------------------------------------------------------------------*/

// Arguments:	pscController - An integer which represents the PSC controller
//				module - An integer which represents the PSC Module number
//				state - An integer which represents the state of the Module to
//						be checked
// Return: Integer - Value of variable pscTimeoutCount
int check_psc_MDSTAT (int pscController, int module, int state)
{
	if(pscController == CSL_PSC_0)
		pscRegs = psc0Regs;
	else if(pscController == CSL_PSC_1)
		pscRegs = psc1Regs;
	else
		return pscTimeout;

	// Reset the PSC Timeout Counter
	pscTimeoutCount = 0;

	// Increment the PSC Timeout Counter While the
	// Specified Module State is Disabled
	while((CSL_FEXT(pscRegs->MDSTAT[module], PSC_MDSTAT_STATE)
			!= state) && (pscTimeoutCount != pscTimeout))
		pscTimeoutCount++;

	// Return the PSC Timeout Counter
	return pscTimeoutCount;
} /* check_psc_MDSTAT */

#endif /* PSC_COMMON_H_ */
