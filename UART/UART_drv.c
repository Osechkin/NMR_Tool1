#include <c6x.h>
#include "../Common/PSC_common.h"
#include "UART_drv.h"


int init_PSC_UART(CSL_UartRegsOvly uartRegs)
{
	int num = -1;
	if (uartRegs == uart0Regs) num = 0;
	else if (uartRegs == uart1Regs) num = 1;
	else if (uartRegs == uart2Regs) num = 2;

	if (num == 0)	// UART0
	{
		// Ensure previous initiated transitions have finished
		if(check_psc_transition(CSL_PSC_0) == pscTimeout) return E_UNKNOWN;

		// Enable peripherals; Initiate transition
		CSL_FINST(psc0Regs->MDCTL[CSL_PSC_UART0], PSC_MDCTL_NEXT, ENABLE);	// UART0
		CSL_FINST(psc0Regs->PTCMD, PSC_PTCMD_GO0, SET);

		// Ensure previous initiated transitions have finished
		if(check_psc_transition(CSL_PSC_0) == pscTimeout) return E_UNKNOWN;

		// Ensure modules enabled
		if(check_psc_MDSTAT(CSL_PSC_0, CSL_PSC_UART0, CSL_PSC_MDSTAT_STATE_ENABLE) == pscTimeout) return E_UNKNOWN;	// UART0
	}
	else if (num == 1)	// UART1
	{
		// Ensure previous initiated transitions have finished
		if(check_psc_transition(CSL_PSC_1) == pscTimeout) return E_UNKNOWN;

		// Enable peripherals; Initiate transition
		CSL_FINST(psc1Regs->MDCTL[CSL_PSC_UART1], PSC_MDCTL_NEXT, ENABLE);	// UART1
		CSL_FINST(psc1Regs->PTCMD, PSC_PTCMD_GO0, SET);

		// Ensure previous initiated transitions have finished
		if(check_psc_transition(CSL_PSC_1) == pscTimeout) return E_UNKNOWN;

		// Ensure modules enabled
		if(check_psc_MDSTAT(CSL_PSC_1, CSL_PSC_UART1, CSL_PSC_MDSTAT_STATE_ENABLE) == pscTimeout) return E_UNKNOWN;	// UART1
	}
	else if (num == 2)	// UART2
	{
		// Ensure previous initiated transitions have finished
		if(check_psc_transition(CSL_PSC_1) == pscTimeout) return E_UNKNOWN;

		// Enable peripherals; Initiate transition
		CSL_FINST(psc1Regs->MDCTL[CSL_PSC_UART2], PSC_MDCTL_NEXT, ENABLE);	// UART2
		CSL_FINST(psc1Regs->PTCMD, PSC_PTCMD_GO0, SET);

		// Ensure previous initiated transitions have finished
		if(check_psc_transition(CSL_PSC_1) == pscTimeout) return E_UNKNOWN;

		// Ensure modules enabled
		if(check_psc_MDSTAT(CSL_PSC_1, CSL_PSC_UART2, CSL_PSC_MDSTAT_STATE_ENABLE) == pscTimeout) return E_UNKNOWN;	// UART2
	}

	return E_OK;
}

/*static void disableARM(CSL_UartRegsOvly uartRegs)
{
	CSL_FINS(sysRegs->HOST0CFG, SYSCFG_HOST0CFG_BOOTRDY, 0x00);//added aivanov to shutdown ARM core and UARTs
}*/

void set_PINMUX_UART(CSL_UartRegsOvly uartRegs)
{
	if (uartRegs == uart0Regs)
	{
		// Configure UART0 Transmit & Receive Signals
		CSL_FINST(sysRegs->PINMUX3, SYSCFG_PINMUX3_PINMUX3_27_24, UART0_CTS);
		CSL_FINST(sysRegs->PINMUX3, SYSCFG_PINMUX3_PINMUX3_31_28, UART0_RTS);
		CSL_FINST(sysRegs->PINMUX3, SYSCFG_PINMUX3_PINMUX3_23_20, UART0_TXD);
		CSL_FINST(sysRegs->PINMUX3, SYSCFG_PINMUX3_PINMUX3_19_16, UART0_RXD);

		// Configure UART0 with the DSP CPU
		CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_UART0SRC, DSP);
	}
	else if (uartRegs == uart1Regs)
	{
		// Configure UART1 Transmit & Receive Signals
		CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_23_20, UART1_CTS);
		CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_19_16, UART1_RTS);
		CSL_FINST(sysRegs->PINMUX4, SYSCFG_PINMUX4_PINMUX4_31_28, UART1_TXD);
		CSL_FINST(sysRegs->PINMUX4, SYSCFG_PINMUX4_PINMUX4_27_24, UART1_RXD);

		// Configure UART1 with the DSP CPU
		CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_UART1SRC, DSP);
	}
	else if (uartRegs == uart2Regs)
	{
		// Configure UART2 Transmit & Receive Signals
		CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_31_28, UART2_CTS);
		CSL_FINST(sysRegs->PINMUX0, SYSCFG_PINMUX0_PINMUX0_27_24, UART2_RTS);
		CSL_FINST(sysRegs->PINMUX4, SYSCFG_PINMUX4_PINMUX4_23_20, UART2_TXD);
		CSL_FINST(sysRegs->PINMUX4, SYSCFG_PINMUX4_PINMUX4_19_16, UART2_RXD);

		// Configure UART2 with the DSP CPU
		CSL_FINST(sysRegs->SUSPSRC, SYSCFG_SUSPSRC_UART2SRC, DSP);
	}

	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}

void setup_UART_INTC(CSL_UartRegsOvly uartRegs, int INTC)
{
	if (INTC < 4 || INTC > 15) return;

	switch (INTC)
	{
	case 4:
		// Map UART system interrupts to DSP INT4
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 5:
		// Map UART system interrupts to DSP INT5
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL5, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL5, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL5, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 6:
		// Map UART system interrupts to DSP INT6
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 7:
		// Map UART system interrupts to DSP INT7
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL7, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL7, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL7, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 8:
		// Map UART system interrupts to DSP INT8
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL8, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL8, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL8, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 9:
		// Map UART system interrupts to DSP INT9
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 10:
		// Map UART system interrupts to DSP INT10
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL10, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL10, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL10, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 11:
		// Map UART system interrupts to DSP INT11
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL11, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL11, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL11, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 12:
		// Map UART system interrupts to DSP INT12
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL12, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL12, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL12, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 13:
		// Map UART system interrupts to DSP INT13
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL13, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL13, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL13, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 14:
		// Map UART system interrupts to DSP INT14
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL14, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL14, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL14, CSL_INTC_EVENTID_UARTINT2);
		break;
	case 15:
		// Map UART system interrupts to DSP INT15
		if (uartRegs == uart0Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL15, CSL_INTC_EVENTID_UARTINT0);
		else if (uartRegs == uart1Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL15, CSL_INTC_EVENTID_UARTINT1);
		else if (uartRegs == uart2Regs) CSL_FINS(dspintcRegs->INTMUX3, DSPINTC_INTMUX3_INTSEL15, CSL_INTC_EVENTID_UARTINT2);
		break;
	default:
		return;
	}

	// Assign the address of the IST to the IST pointer
/*	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = DSPINTC_IST_ALL_MASK_INT;

	// Enable INTC interrupts
	unsigned long DSPINTC_IST_INT = 1 << INTC;
	IER = DSPINTC_IST_NMI | DSPINTC_IST_INT; */
}/* setup_DSP_INTC */


void setup_UART_Settings(CSL_UartRegsOvly uartRegs, UART_Settings uartSettings)
{
	// Configure Baud Rate; divisor 0x64 = 115200 Baud Rate for 13x Sampling
	uint32_t divisor = UART_BASE / uartSettings.BaudRate;
	uint8_t dll = (uint8_t)(divisor & 0x000000FF);
	uint8_t dlh = (uint8_t)((divisor >> 8) & 0x000000FF);
	CSL_FINS(uartRegs->DLL, UART_DLL_DLL, dll);
	CSL_FINS(uartRegs->DLH, UART_DLH_DLH, dlh);

	// Select Word Bit Length
	if (uartSettings.DataBits == 5) CSL_FINST(uartRegs->LCR, UART_LCR_WLS, 5BITS);
	else if (uartSettings.DataBits == 6) CSL_FINST(uartRegs->LCR, UART_LCR_WLS, 6BITS);
	else if (uartSettings.DataBits == 7) CSL_FINST(uartRegs->LCR, UART_LCR_WLS, 7BITS);
	else if (uartSettings.DataBits == 8) CSL_FINST(uartRegs->LCR, UART_LCR_WLS, 8BITS);

	if (uartSettings.Parity == NO_PARITY) CSL_FINST(uartRegs->LCR, UART_LCR_PEN, DISABLE);
	else if (uartSettings.Parity == EVEN_PARITY)
	{
		CSL_FINST(uartRegs->LCR, UART_LCR_PEN, ENABLE);
		CSL_FINST(uartRegs->LCR, UART_LCR_EPS, EVEN);
	}
	else if (uartSettings.Parity == ODD_PARITY)
	{
		CSL_FINST(uartRegs->LCR, UART_LCR_PEN, ENABLE);
		CSL_FINST(uartRegs->LCR, UART_LCR_EPS, ODD);
	}

	// Select 1 Stop Bit
	CSL_FINST(uartRegs->LCR, UART_LCR_STB, 1BIT);

	// Select 13x Oversampling
	CSL_FINST(uartRegs->MDR, UART_MDR_OSM_SEL, 13XOVERSAMPLING);

	// Select Loopback Mode
	if (uartSettings.LoopBackMode == True) CSL_FINST(uartRegs->MCR, UART_MCR_LOOP, ENABLE);
	else CSL_FINST(uartRegs->MCR, UART_MCR_LOOP, DISABLE);

	if (uartSettings.FIFOMode == True)
	{
		CSL_FINST(uartRegs->FCR, UART_FCR_FIFOEN, ENABLE);
		switch (uartSettings.FIFOLen)
		{
		case 1:
			CSL_FINST(uartRegs->FCR, UART_FCR_RXFIFTL, CHAR1);
			break;
		case 4:
			CSL_FINST(uartRegs->FCR, UART_FCR_RXFIFTL, CHAR4);
			break;
		case 8:
			CSL_FINST(uartRegs->FCR, UART_FCR_RXFIFTL, CHAR8);
			break;
		case 14:
			CSL_FINST(uartRegs->FCR, UART_FCR_RXFIFTL, CHAR14);
			break;
		default:
			CSL_FINST(uartRegs->FCR, UART_FCR_RXFIFTL, CHAR1);
		}
	}
	else CSL_FINST(uartRegs->FCR, UART_FCR_FIFOEN, DISABLE);

	// Enable UART Interrupts (Receive & Transmit)
	CSL_FINST(uartRegs->IER, UART_IER_ERBI, ENABLE);
	CSL_FINST(uartRegs->IER, UART_IER_ETBEI, ENABLE);
}/* setup_UART */

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

/*void enableSYSCFG_Regs()
{
	// Open Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK0_KEY);
	CSL_FINS(sysRegs->KICK1R, SYSCFG_KICK1R_KICK1, KICK1_KEY);
}

void disableSYSCFG_Regs()
{
	// Close Permissions to SYSCFG Registers
	CSL_FINS(sysRegs->KICK0R, SYSCFG_KICK0R_KICK0, KICK_LOCK);
}*/

int setup_UART(CSL_UartRegsOvly uartRegs, UART_Settings uartSettings)
{
	set_PINMUX_UART(uartRegs);
	uint32_t uart_status = init_PSC_UART(uartRegs);
	setup_UART_Settings(uartRegs, uartSettings);

	return uart_status;
}

int read_UART(CSL_UartRegsOvly uartRegs, uint8_t *byte)
{
	uint32_t uart_status = E_UNKNOWN;

	uart_status = CSL_FEXT(uartRegs->IIR, UART_IIR_INTID);
	if (uart_status == E_DATA_READY)
	{
		*byte = CSL_FEXT(uartRegs->RBR, UART_RBR_DATA);
		uart_status = E_OK;
	}
	else uart_status = E_UNKNOWN;

	return uart_status;
}

void write_UART(CSL_UartRegsOvly uartRegs, uint8_t byte)
{
	while (CSL_FEXT(uartRegs->LSR, UART_LSR_THRE) != CSL_UART_LSR_THRE_EMPTY);

	CSL_FINS(uartRegs->THR, UART_THR_DATA, byte);
}

int write_data_UART(CSL_UartRegsOvly uartRegs, uint8_t *byte, int len)
{
	uint32_t uart_status = E_UNKNOWN;
	if (len < 0) return uart_status;

	int cnt = 0;
	while (cnt < len)
	{
		int tout = 100000000;
		while (--tout > 0 && CSL_FEXT(uartRegs->IIR, UART_IIR_INTID) != E_TRAN_BUF_EMPTY);

		if (tout > 0)
		{
			// Send Data to Transmitter Buffer
			CSL_FINS(uartRegs->THR, UART_THR_DATA, byte[cnt++]);
			tout = 100000000;
		}
		else return E_TIMEOUT;
	}

	return E_OK;
}
