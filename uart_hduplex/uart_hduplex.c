/*
 * uart_hduplex.c
 *
 *  Created on: 12.05.2015
 *      Author: aivanov
 */

#include <stdio.h>
#include "OMAPL138_common.h"
#include <ti/pspiom/cslr/cslr.h>
//#include <ti/pspiom/cslr/cslr_upp.h>
#include "uart_hduplex.h"
//#include "upp.h"
//#include <c6x.h>

char uart_hduplex_input_arr[64];

static void init_GPIO ()
{

	//PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);	// initialization of GPIO support in PSC module
	modulesEnabled = FALSE;

	// Ensure previous initiated transitions have finished
	if(check_psc_transition(CSL_PSC_1) == pscTimeout) return;

	// Enable peripherals; Initiate transition
	CSL_FINST(psc1Regs->MDCTL[CSL_PSC_GPIO], PSC_MDCTL_NEXT, ENABLE);
	CSL_FINST(psc1Regs->PTCMD, PSC_PTCMD_GO0, SET);

	// Ensure previous initiated transitions have finished
	if(check_psc_transition(CSL_PSC_1) == pscTimeout) return;

	// Ensure modules enabled
	if(check_psc_MDSTAT(CSL_PSC_1, CSL_PSC_GPIO, CSL_PSC_MDSTAT_STATE_ENABLE) == pscTimeout) return;

	modulesEnabled = TRUE;

	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_31_28, GPIO0_0);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_27_24, GPIO0_1);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_23_20, GPIO0_2);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_19_16, GPIO0_3);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_15_12, GPIO0_4);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_11_8, GPIO0_5);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_7_4, GPIO0_6);
	CSL_FINST(sysRegs->PINMUX1, SYSCFG_PINMUX1_PINMUX1_3_0, GPIO0_7);

	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_31_28, GPIO0_8);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_27_24, GPIO0_9);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_23_20, GPIO0_10);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_19_16, GPIO0_11);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_15_12, GPIO0_12);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_11_8, GPIO0_13);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_7_4, GPIO0_14);
	CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_3_0, GPIO0_15);

	CSL_FINS(gpioRegs->BANK[0].DIR,
			GPIO_DIR_DIR13,
			CSL_GPIO_DIR_DIR_OUT);
	CSL_FINS(gpioRegs->BANK[0].DIR,
			GPIO_DIR_DIR14,
			CSL_GPIO_DIR_DIR_OUT);
	CSL_FINS(gpioRegs->BANK[0].DIR,
			GPIO_DIR_DIR15,
			CSL_GPIO_DIR_DIR_OUT);

	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR0, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR1, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR2, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR3, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR4, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR5, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR6, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR7, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR8, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR9, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR10, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR11, CSL_GPIO_DIR_DIR_IN);
	CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR12, CSL_GPIO_DIR_DIR_IN);
	//CSL_FINS(gpioRegs->BANK[0].DIR,	GPIO_DIR_DIR13, CSL_GPIO_DIR_DIR_IN);


	//CSL_FINS(gpioRegs->BANK->CLR_DATA, GPIO_CLR_DATA_CLR13, CSL_GPIO_CLR_DATA_CLR_CLR);//RX ON
	//CSL_FINS(gpioRegs->BANK->CLR_DATA, GPIO_CLR_DATA_CLR14, CSL_GPIO_CLR_DATA_CLR_CLR);//TX OFF
	//CSL_FINS(gpioRegs->BANK->SET_DATA, GPIO_SET_DATA_SET14, CSL_GPIO_SET_DATA_SET_SET);//TX ON
}

/*
void enable_UART(CSL_UartRegsOvly uartRegs)
{
	// Enable UART Transmitter/Receiver
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_URRST, ENABLE);
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_UTRST, ENABLE);
}


void reset_UART(CSL_UartRegsOvly uartRegs)
{
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_URRST, RESET);
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_UTRST, RESET);
}
*/

void UART_RX_EN(CSL_UartRegsOvly uartRegs)
{
	// Enable UART Receiver
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_URRST, ENABLE);
}

void UART_TX_EN(CSL_UartRegsOvly uartRegs)
{
	// Enable UART Transmitter
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_UTRST, ENABLE);
}

void UART_RX_DIS(CSL_UartRegsOvly uartRegs)
{
	// Disable UART Receiver
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_URRST, RESET);
}

void UART_TX_DIS(CSL_UartRegsOvly uartRegs)
{
	// Disable UART Transmitter
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_UTRST, RESET);
}

void UART_RX_RST(CSL_UartRegsOvly uartRegs)
{
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_URRST, RESET);
}

void UART_TX_RST(CSL_UartRegsOvly uartRegs)
{
	CSL_FINST(uartRegs->PWREMU_MGMT, UART_PWREMU_MGMT_UTRST, RESET);
}

void RS485_TX_ON()
{
	P14_SET();//TX ON
}

void RS485_TX_OFF()
{
	P14_CLR();//TX OFF
}

void RS485_RX_ON()
{
	P13_CLR();//RX ON
}

void RS485_RX_OFF()
{
	P13_SET();//RX OFF
}

void P13_CLR()
{
	CSL_FINS(gpioRegs->BANK[0].OUT_DATA, GPIO_OUT_DATA_OUT13, 0);
}

void P13_SET()
{
	CSL_FINS(gpioRegs->BANK[0].OUT_DATA, GPIO_OUT_DATA_OUT13, 1);
}

void P14_CLR()
{
	CSL_FINS(gpioRegs->BANK[0].OUT_DATA, GPIO_OUT_DATA_OUT14, 0);//RX ON
}

void P14_SET()
{
	CSL_FINS(gpioRegs->BANK[0].OUT_DATA, GPIO_OUT_DATA_OUT14, 1);//RX OFF
}

void P15_CLR()
{
	CSL_FINS(gpioRegs->BANK[0].OUT_DATA, GPIO_OUT_DATA_OUT15, 0);//HS ON
}

void P15_SET()
{
	CSL_FINS(gpioRegs->BANK[0].OUT_DATA, GPIO_OUT_DATA_OUT15, 1);//HS OFF
}

unsigned int P13_RD()
{
	unsigned int pin;
	pin = CSL_FEXT(gpioRegs->BANK[0].IN_DATA, GPIO_IN_DATA_IN13);
	return ( pin );
}

unsigned int GPIO_B0_RD()
{
	volatile unsigned int pin;
	//pin = gpioRegs->BANK[0].IN_DATA;
	pin = proger_read_gpio();
	return ( pin );
}

/*
unsigned int GPIO_B0_RD----====000()
{
	unsigned int pin;
	pin = gpioRegs->BANK[0].IN_DATA;
	return ( pin );
}
*/


void test_GPIO ()
{
	volatile unsigned int pin, pins_reg;
	while(1)
	{

		P13_SET();
		pin = P13_RD();
		pins_reg = GPIO_B0_RD();

		P13_CLR();
		pin = P13_RD();
		pins_reg = GPIO_B0_RD();

		//P13_CLR();
		//P14_CLR();

		//P13_SET();
		//P14_SET();

		//P13_CLR();
		//P14_CLR();


		//P14_SET();
		//P13_SET();

		//P14_CLR();
		//P13_CLR();
	}
}

void test_GPIO_P15 ()
{
	volatile unsigned int pins_reg;
	while(1)
	{

		P15_SET();
		pins_reg = GPIO_B0_RD();
		//dummyDelay(10000);

		P15_CLR();
		pins_reg = GPIO_B0_RD();
		//dummyDelay(10000);

	}
}


void init_uart_hduplex_pins()
{
	init_GPIO();
	RS485_TX_OFF();
	RS485_RX_ON();
	P15_CLR();
}

void uart_hduplex_sendchar(CSL_UartRegsOvly uartRegs, unsigned char ch)
{
	while (CSL_FEXT(uartRegs->LSR, UART_LSR_THRE) != CSL_UART_LSR_THRE_EMPTY);
	while (CSL_FEXT(uartRegs->LSR, UART_LSR_THRE) != CSL_UART_LSR_TEMT_EMPTY);

	UART_RX_DIS(uartRegs);
//	RS485_RX_OFF();

	RS485_TX_ON();
	UART_TX_EN(uartRegs);

	while (CSL_FEXT(uartRegs->LSR, UART_LSR_THRE) != CSL_UART_LSR_THRE_EMPTY);
	while (CSL_FEXT(uartRegs->LSR, UART_LSR_TEMT) != CSL_UART_LSR_TEMT_EMPTY);

	CSL_FINS(uartRegs->THR, UART_THR_DATA, ch);

	while (CSL_FEXT(uartRegs->LSR, UART_LSR_THRE) != CSL_UART_LSR_THRE_EMPTY);
	while (CSL_FEXT(uartRegs->LSR, UART_LSR_TEMT) != CSL_UART_LSR_TEMT_EMPTY);
	//dummyDelay(200);

	RS485_TX_OFF();
	UART_TX_DIS(uartRegs);


//	RS485_RX_ON();
	UART_RX_EN(uartRegs);

}

//for example only
/*
void RS485_TX_TST_INFINITE(CSL_UartRegsOvly uartRegs)
{
	in_arr_clear();

	while (1)
	{
		dummyDelay(4000);
		uart_hduplex_sendchar(uartRegs, 0x31);
	}

}*/

void in_arr_clear()
{
	int k;
	for (k = 0; k < 1000; k++)
	{
		uart_hduplex_input_arr[k] = 0x00;
	}
}


//-------------------------------------------------------------------------------
//
// Example
//
//-------------------------------------------------------------------------------

/*void main (void)
{
	_disable_interrupts();
	//shutdown_ARM();
	//wakeup_ARM();

	uartRegs = uart1Regs;
	proger_stop();

	// Initialize OMAPL138 EVM (Configure Pins with UART; UART with DSP)
	init_OMAPL138();

	// Enable Peripherals (UART)
	enable_module_clocks();

	memset_array (in_upp_arr, 0x00, UPP_ARRAYS_SIZE);

	// Setup Peripherals; Run Example
	if(modulesEnabled)
	{
		init_GPIO ();
		init_RS485();
//test_GPIO ();
		reset_UART(uartRegs);
		// Configure UART and Enable UART Interrupts
		setup_UART();

		// Map system interrupts to the DSP interrupt controller
		setup_DSP_INTC();

		_enable_interrupts();

		uart_enable();

		RS485_TX_TST_INFINITE();

}// End of main */


/*
interrupt void UART_isr (void)
{
	char ch=0xFF, k;

	// Determine Prioritized Pending UART Interrupt
	prioritizedINT = CSL_FEXT(uartRegs->IIR, UART_IIR_INTID);

	// Set Appropriate Bool
	if(prioritizedINT == UART_REC_BUF_FULL_INT)
	{
		dataUnavailable = FALSE;
		ch = CSL_FEXT(uartRegs->RBR, UART_RBR_DATA);

		if (ch == UART_COMM_START)
			UART_command_start = 1;

		//in_arr[in_cnt++] = ch;
		static int counter = 0;
		//counter++;

//		RS485_sendchar(ch);
//		RS485_sendchar(++ch);
//		RS485_sendchar(++ch);

		if (in_cnt < 1000)//10 in_cnt >= 1
		{
			in_arr[in_cnt++] = ch;
			counter++;

			for (k = 0; k <= 0; k++)
			{
				//while (CSL_FEXT(uartRegs->LSR, UART_LSR_THRE) != CSL_UART_LSR_THRE_EMPTY);
				//CSL_FINS(uartRegs->THR, UART_THR_DATA, in_arr[k]);

				//RS485_sendchar(ch);

			}
			//in_cnt = 0;
		}
		else
		{
			in_cnt = in_cnt;
		}


	}
	else if(prioritizedINT == UART_TRAN_BUF_EMPTY_INT)
		transmitterFull = FALSE;

	//char ch = UART_receiveData();
	//UART_sendData(ch);



	//CSL_FINS(uartRegs->THR, UART_THR_DATA, ch);
}
*/

