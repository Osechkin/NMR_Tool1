/*
 * UART_drv.h
 *
 *  Created on: 23.10.2013
 *      Author: Andrey
 */

// OMAP-L138 UART driver (No EDMA support)
#ifndef UART_DRV_H_
#define UART_DRV_H_

#include <c6x.h>
#include "../Common/OMAPL138_global.h"


#define NO_PARITY			0
#define ODD_PARITY			1
#define EVEN_PARITY			2

#define UART_BASE			11539200


#define E_OK 				0
#define E_TRAN_BUF_EMPTY 	1
#define E_DATA_READY		2
#define E_LINE_STATUS		3
#define E_TIMEOUT			6
#define E_UNKNOWN			0xFF


typedef struct
{
	uint32_t 	BaudRate;
	uint32_t 	DataBits;
	uint32_t 	StopBits;
	uint32_t 	Parity;
	Bool 		LoopBackMode;
	Bool 		FIFOMode;
	uint32_t	FIFOLen;			// 1, 4, 8, 14 bytes
} UART_Settings;


int init_PSC_UART(CSL_UartRegsOvly uartRegs);
//void disableARM(CSL_UartRegsOvly uartRegs);
//void enableSYSCFG_Regs();
//void disableSYSCFG_Regs();
void set_PINMUX_UART(CSL_UartRegsOvly uartRegs);
void setup_UART_INTC(CSL_UartRegsOvly uartRegs, int INTC);
void setup_UART_Settings(CSL_UartRegsOvly uartRegs, UART_Settings uartSettings);
void enable_UART(CSL_UartRegsOvly uartRegs);
void reset_UART(CSL_UartRegsOvly uartRegs);
int setup_UART(CSL_UartRegsOvly uartRegs, UART_Settings uartSettings);
int read_UART(CSL_UartRegsOvly uartRegs, uint8_t *byte);
void write_UART(CSL_UartRegsOvly uartRegs, uint8_t byte);
int write_data_UART(CSL_UartRegsOvly uartRegs, uint8_t *byte, int len);

extern void intcVectorTable (void);

#endif /* UART_DRV_H_ */
