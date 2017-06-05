#include "gpioMux.h"


/*****************************************************************************
**                       FUNCTION DEFINITION
*****************************************************************************/
void GPIOBank0Pin0PinMuxSetup(void)
{
     unsigned int savePinmux = 0;

     /*
     ** Clearing the bit in context and retaining the other bit values
     ** in PINMUX1 register.
     */
     savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) &
                  ~(SYSCFG_PINMUX1_PINMUX1_31_28));

     /* Setting the pins corresponding to GP0[0] in PINMUX1 register.*/
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) =
          (PINMUX1_GPIO0_0_ENABLE | savePinmux);
}

void GPIOBank0Pin1PinMuxSetup(void)
{
     unsigned int savePinmux = 0;

     /*
     ** Clearing the bit in context and retaining the other bit values
     ** in PINMUX1 register.
     */
     savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) &
                  ~(SYSCFG_PINMUX1_PINMUX1_27_24));

     /* Setting the pins corresponding to GP0[1] in PINMUX1 register.*/
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) =
          (PINMUX1_GPIO0_1_ENABLE | savePinmux);
}

void GPIOBank0Pin2PinMuxSetup(void)
{
     unsigned int savePinmux = 0;

     /*
     ** Clearing the bit in context and retaining the other bit values
     ** in PINMUX1 register.
     */
     savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) &
                  ~(SYSCFG_PINMUX1_PINMUX1_23_20));

     /* Setting the pins corresponding to GP0[2] in PINMUX1 register.*/
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) =
          (PINMUX1_GPIO0_2_ENABLE | savePinmux);
}

void GPIOBank0Pin3PinMuxSetup(void)
{
     unsigned int savePinmux = 0;

     /*
     ** Clearing the bit in context and retaining the other bit values
     ** in PINMUX1 register.
     */
     savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) &
                  ~(SYSCFG_PINMUX1_PINMUX1_19_16));

     /* Setting the pins corresponding to GP0[3] in PINMUX1 register.*/
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) =
          (PINMUX1_GPIO0_3_ENABLE | savePinmux);
}

void GPIOBank0Pin4PinMuxSetup(void)
{
     unsigned int savePinmux = 0;
     
     /*
     ** Clearing the bit in context and retaining the other bit values 
     ** in PINMUX1 register.
     */
     savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) &
                  ~(SYSCFG_PINMUX1_PINMUX1_15_12));

     /* Setting the pins corresponding to GP0[4] in PINMUX1 register.*/
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) =
          (PINMUX1_GPIO0_4_ENABLE | savePinmux);
}

void GPIOBank0Pin9PinMuxSetup(void)
{
     unsigned int savePinmux = 0;

     /*
     ** Clearing the bit in context and retaining the other bit values
     ** in PINMUX0 register.
     */
     savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) &
                  ~(SYSCFG_PINMUX0_PINMUX0_27_24));

     /* Setting the pins corresponding to GP0[9] in PINMUX0 register.*/
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) =
          (PINMUX0_GPIO0_9_ENABLE | savePinmux);

}

void GPIOBank0Pin0_initState(int state)
{
	if (state == GPIO_FALL_STATE) { new_GP0p0 = 0; old_GP0p0 = 1; }
	else if (state == GPIO_RISE_STATE) { new_GP0p0 = 1; old_GP0p0 = 0; }
	else if (state == GPIO_LOW_STATE) { new_GP0p0 = 0; old_GP0p0 = 0; }
	else if (state == GPIO_HIGH_STATE) { new_GP0p0 = 1; old_GP0p0 = 1; }
}

void GPIOBank0Pin1_initState(int state)
{
	if (state == GPIO_FALL_STATE) { new_GP0p1 = 0; old_GP0p1 = 1; }
	else if (state == GPIO_RISE_STATE) { new_GP0p1 = 1; old_GP0p1 = 0; }
	else if (state == GPIO_LOW_STATE) { new_GP0p1 = 0; old_GP0p1 = 0; }
	else if (state == GPIO_HIGH_STATE) { new_GP0p1 = 1; old_GP0p1 = 1; }
}

void GPIOBank0Pin2_initState(int state)
{
	if (state == GPIO_FALL_STATE) { new_GP0p2 = 0; old_GP0p2 = 1; }
	else if (state == GPIO_RISE_STATE) { new_GP0p2 = 1; old_GP0p2 = 0; }
	else if (state == GPIO_LOW_STATE) { new_GP0p2 = 0; old_GP0p2 = 0; }
	else if (state == GPIO_HIGH_STATE) { new_GP0p2 = 1; old_GP0p2 = 1; }
}

void GPIOBank0Pin3_initState(int state)
{
	if (state == GPIO_FALL_STATE) { new_GP0p3 = 0; old_GP0p3 = 1; }
	else if (state == GPIO_RISE_STATE) { new_GP0p3 = 1; old_GP0p3 = 0; }
	else if (state == GPIO_LOW_STATE) { new_GP0p3 = 0; old_GP0p3 = 0; }
	else if (state == GPIO_HIGH_STATE) { new_GP0p3 = 1; old_GP0p3 = 1; }
}

void GPIOBank0Pin4_initState(int state)
{
	if (state == GPIO_FALL_STATE) { new_GP0p4 = 0; old_GP0p4 = 1; }
	else if (state == GPIO_RISE_STATE) { new_GP0p4 = 1; old_GP0p4 = 0; }
	else if (state == GPIO_LOW_STATE) { new_GP0p4 = 0; old_GP0p4 = 0; }
	else if (state == GPIO_HIGH_STATE) { new_GP0p4 = 1; old_GP0p4 = 1; }
}

Bool GPIOBank0Pin0_isRisen(void)
{
	new_GP0p0  = GPIOPinRead(SOC_GPIO_0_REGS, 1);

	Bool res = FALSE;
	if (new_GP0p0 > 0 && old_GP0p0 == 0) res = TRUE;
	old_GP0p0 = new_GP0p0;

	return res;
}

Bool GPIOBank0Pin1_isRisen(void)
{
	new_GP0p1  = GPIOPinRead(SOC_GPIO_0_REGS, 2);

	Bool res = FALSE;
	if (new_GP0p1 > 0 && old_GP0p1 == 0) res = TRUE;
	old_GP0p1 = new_GP0p1;

	return res;
}

Bool GPIOBank0Pin2_isRisen(void)
{
	new_GP0p2  = GPIOPinRead(SOC_GPIO_0_REGS, 3);

	Bool res = false;
	if (new_GP0p2 > 0 && old_GP0p2 == 0) res = true;
	old_GP0p2 = new_GP0p2;

	return res;
}

Bool GPIOBank0Pin3_isRisen(void)
{
	new_GP0p3  = GPIOPinRead(SOC_GPIO_0_REGS, 4);

	Bool res = false;
	if (new_GP0p3 > 0 && old_GP0p3 == 0) res = true;
	old_GP0p3 = new_GP0p3;

	return res;
}

Bool GPIOBank0Pin4_isRisen(void)
{
	new_GP0p4  = GPIOPinRead(SOC_GPIO_0_REGS, 5);

	Bool res = false;
	if (new_GP0p4 > 0 && old_GP0p4 == 0) res = true;
	old_GP0p4 = new_GP0p4;

	return res;
}

Bool GPIOBank0Pin0_isFallen(void)
{
	new_GP0p0  = GPIOPinRead(SOC_GPIO_0_REGS, 1);

	Bool res = false;
	if (new_GP0p0 == 0 && old_GP0p0 > 0) res = true;
	old_GP0p0 = new_GP0p0;

	return res;
}

Bool GPIOBank0Pin1_isFallen(void)
{
	new_GP0p1  = GPIOPinRead(SOC_GPIO_0_REGS, 2);

	Bool res = false;
	if (new_GP0p1 == 0 && old_GP0p1 > 0) res = true;
	old_GP0p1 = new_GP0p1;

	return res;
}

Bool GPIOBank0Pin2_isFallen(void)
{
	new_GP0p2  = GPIOPinRead(SOC_GPIO_0_REGS, 3);

	Bool res = false;
	if (new_GP0p2 == 0 && old_GP0p2 > 0) res = true;
	old_GP0p2 = new_GP0p2;

	return res;
}

Bool GPIOBank0Pin3_isFallen(void)
{
	new_GP0p3  = GPIOPinRead(SOC_GPIO_0_REGS, 4);

	Bool res = false;
	if (new_GP0p3 == 0 && old_GP0p3 > 0) res = true;
	old_GP0p3 = new_GP0p3;

	return res;
}

Bool GPIOBank0Pin4_isFallen(void)
{
	new_GP0p4  = GPIOPinRead(SOC_GPIO_0_REGS, 5);

	Bool res = false;
	if (new_GP0p4 == 0 && old_GP0p4 > 0) res = true;
	old_GP0p4 = new_GP0p4;

	return res;
}

int GPIOBank0Pin0_State(void)
{
	new_GP0p0  = (unsigned char)GPIOPinRead(SOC_GPIO_0_REGS, 1);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p0 == 0 && old_GP0p0 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p0 > 0 && old_GP0p0 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p0 == 0 && old_GP0p0 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p0 > 0 && old_GP0p0 > 0) res = GPIO_HIGH_STATE;

	old_GP0p0 = new_GP0p0;

	return res;
}

int GPIOBank0Pin1_State(void)
{
	new_GP0p1  = (unsigned char)GPIOPinRead(SOC_GPIO_0_REGS, 2);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p1 == 0 && old_GP0p1 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p1 > 0 && old_GP0p1 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p1 == 0 && old_GP0p1 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p1 > 0 && old_GP0p1 > 0) res = GPIO_HIGH_STATE;

	old_GP0p1 = new_GP0p1;

	return res;
}

int GPIOBank0Pin2_State(void)
{
	new_GP0p2  = GPIOPinRead(SOC_GPIO_0_REGS, 3);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p2 == 0 && old_GP0p2 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p2 > 0 && old_GP0p2 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p2 == 0 && old_GP0p2 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p2 > 0 && old_GP0p2 > 0) res = GPIO_HIGH_STATE;

	old_GP0p2 = new_GP0p2;

	return res;
}

int GPIOBank0Pin3_State(void)
{
	new_GP0p3 = GPIOPinRead(SOC_GPIO_0_REGS, 4);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p3 == 0 && old_GP0p3 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p3 > 0 && old_GP0p3 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p3 == 0 && old_GP0p3 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p3 > 0 && old_GP0p3 > 0) res = GPIO_HIGH_STATE;

	old_GP0p3 = new_GP0p3;

	return res;
}

int GPIOBank0Pin4_State(void)
{
	new_GP0p4 = GPIOPinRead(SOC_GPIO_0_REGS, 5);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p4 == 0 && old_GP0p4 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p4 > 0 && old_GP0p4 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p4 == 0 && old_GP0p4 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p4 > 0 && old_GP0p4 > 0) res = GPIO_HIGH_STATE;

	old_GP0p4 = new_GP0p4;

	return res;
}


int GPIOBank0Pin0_CmdAddr_State(unsigned char *cmd_addr)
{
	unsigned int value = GPIOBankRead(SOC_GPIO_0_REGS, 0);

	*cmd_addr = (value & 0x1FE0) >> 5;

	unsigned int pinOffset = 0;
	unsigned int val = value & (1 << pinOffset);
	new_GP0p0 = (val >> pinOffset);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p0 == 0 && old_GP0p0 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p0 > 0 && old_GP0p0 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p0 == 0 && old_GP0p0 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p0 > 0 && old_GP0p0 > 0) res = GPIO_HIGH_STATE;

	old_GP0p0 = new_GP0p0;

	return res;
}

int GPIOBank0Pin1_CmdAddr_State(unsigned char *cmd_addr)
{
	unsigned int value = GPIOBankRead(SOC_GPIO_0_REGS, 0);

	unsigned char v_cmd_addr = (unsigned char)((value & 0x1FE0) >> 5);
	*cmd_addr = v_cmd_addr;

	unsigned int pinOffset = 1;
	unsigned int val = value & (1 << pinOffset);
	new_GP0p1 = (val >> pinOffset);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p1 == 0 && old_GP0p1 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p1 > 0 && old_GP0p1 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p1 == 0 && old_GP0p1 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p1 > 0 && old_GP0p1 > 0) res = GPIO_HIGH_STATE;

	old_GP0p1 = new_GP0p1;

	return res;
}

int GPIOBank0Pin2_CmdAddr_State(unsigned char *cmd_addr)
{
	unsigned int value = GPIOBankRead(SOC_GPIO_0_REGS, 0);

	*cmd_addr = (value & 0x1FE0) >> 5;
	//new_GP0p2 = (value & 0x04) >> 2;
	unsigned int pinOffset = 2;
	unsigned int val = value & (1 << pinOffset);
	new_GP0p2 = (val >> pinOffset);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p2 == 0 && old_GP0p2 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p2 > 0 && old_GP0p2 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p2 == 0 && old_GP0p2 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p2 > 0 && old_GP0p2 > 0) res = GPIO_HIGH_STATE;

	old_GP0p2 = new_GP0p2;

	return res;
}

int GPIOBank0Pin3_CmdAddr_State(unsigned char *cmd_addr)
{
	unsigned int value = GPIOBankRead(SOC_GPIO_0_REGS, 0);

	*cmd_addr = (value & 0x1FE0) >> 5;
	//new_GP0p3 = (value & 0x08) >> 3;
	unsigned int pinOffset = 3;
	unsigned int val = value & (1 << pinOffset);
	new_GP0p3 = (val >> pinOffset);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p3 == 0 && old_GP0p3 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p3 > 0 && old_GP0p3 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p3 == 0 && old_GP0p3 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p3 > 0 && old_GP0p3 > 0) res = GPIO_HIGH_STATE;

	old_GP0p3 = new_GP0p3;

	return res;
}

int GPIOBank0Pin4_CmdAddr_State(unsigned char *cmd_addr)
{
	unsigned int value = GPIOBankRead(SOC_GPIO_0_REGS, 0);

	*cmd_addr = (value & 0x1FE0) >> 5;
	//new_GP0p4 = (value & 0x10) >> 4;
	unsigned int pinOffset = 4;
	unsigned int val = value & (1 << pinOffset);
	new_GP0p4 = (val >> pinOffset);

	int res = GPIO_UNKNOWN_STATE;
	if (new_GP0p4 == 0 && old_GP0p4 > 0) res = GPIO_FALL_STATE;
	else if (new_GP0p4 > 0 && old_GP0p4 == 0) res = GPIO_RISE_STATE;
	else if (new_GP0p4 == 0 && old_GP0p4 == 0) res = GPIO_LOW_STATE;
	else if (new_GP0p4 > 0 && old_GP0p4 > 0) res = GPIO_HIGH_STATE;

	old_GP0p4 = new_GP0p4;

	return res;
}

/******************************* End of file ********************************/
