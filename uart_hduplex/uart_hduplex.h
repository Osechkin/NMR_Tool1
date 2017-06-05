/*
 * uart_hduplex.h
 *
 *  Created on: 12.05.2015
 *      Author: aivanov
 */

#ifndef UART_HDUPLEX_H_
#define UART_HDUPLEX_H_

#include "../proger/proger.h"

void init_uart_hduplex_pins();
void uart_hduplex_sendchar(CSL_UartRegsOvly uartRegs, unsigned char ch);

static void init_GPIO ();


//void enable_UART(CSL_UartRegsOvly uartRegs);
//void reset_UART(CSL_UartRegsOvly uartRegs);
void UART_RX_EN(CSL_UartRegsOvly uartRegs);
void UART_TX_EN(CSL_UartRegsOvly uartRegs);
void UART_RX_DIS(CSL_UartRegsOvly uartRegs);
void UART_TX_DIS(CSL_UartRegsOvly uartRegs);
void UART_RX_RST(CSL_UartRegsOvly uartRegs);
void UART_TX_RST(CSL_UartRegsOvly uartRegs);

void RS485_TX_ON();
void RS485_TX_OFF();
void RS485_RX_ON();
void RS485_RX_OFF();

//for example only
//void RS485_TX_TST_INFINITE(CSL_UartRegsOvly uartRegs);


void P13_CLR();
void P13_SET();
void P14_CLR();
void P14_SET();
void P15_CLR();
void P15_SET();
void test_GPIO ();
void test_GPIO_P15 ();
unsigned int P13_RD();
unsigned int GPIO_B0_RD();

void in_arr_clear();

#endif /* UART_HDUPLEX_H_ */
