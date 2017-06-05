/*
 * gpioMux.h
 *
 *  Created on: 11.12.2014
 *      Author: Admin
 */

#ifndef GPIOMUX_H_
#define GPIOMUX_H_

/* HW Macros */
#include "hw_types.h"

/* System Defines */
#include "soc_C6748.h"
#include "hw_syscfg0_C6748.h"

/*****************************************************************************
**                       MACRO DEFINITION
*****************************************************************************/
/* Pin Multiplexing bit mask to select GP0[0] pin. */
#define PINMUX1_GPIO0_0_ENABLE    (SYSCFG_PINMUX1_PINMUX1_31_28_GPIO0_0  << \
                                    SYSCFG_PINMUX1_PINMUX1_31_28_SHIFT)

/* Pin Multiplexing bit mask to select GP0[1] pin. */
#define PINMUX1_GPIO0_1_ENABLE    (SYSCFG_PINMUX1_PINMUX1_27_24_GPIO0_1  << \
                                    SYSCFG_PINMUX1_PINMUX1_27_24_SHIFT)

/* Pin Multiplexing bit mask to select GP0[2] pin. */
#define PINMUX1_GPIO0_2_ENABLE    (SYSCFG_PINMUX1_PINMUX1_23_20_GPIO0_2  << \
                                    SYSCFG_PINMUX1_PINMUX1_23_20_SHIFT)

/* Pin Multiplexing bit mask to select GP0[3] pin. */
#define PINMUX1_GPIO0_3_ENABLE    (SYSCFG_PINMUX1_PINMUX1_19_16_GPIO0_3   << \
                                    SYSCFG_PINMUX1_PINMUX1_19_16_SHIFT)

/* Pin Multiplexing bit mask to select GP0[4] pin. */
#define PINMUX1_GPIO0_4_ENABLE    (SYSCFG_PINMUX1_PINMUX1_15_12_GPIO0_4  << \
                                    SYSCFG_PINMUX1_PINMUX1_15_12_SHIFT)

/* Pin Multiplexing bit mask to select GP0[5] pin. */
#define PINMUX1_GPIO0_5_ENABLE    (SYSCFG_PINMUX1_PINMUX1_11_8_GPIO0_5  << \
                                    SYSCFG_PINMUX1_PINMUX1_11_8_SHIFT)

/* Pin Multiplexing bit mask to select GP0[9] pin. */
#define PINMUX0_GPIO0_9_ENABLE    (SYSCFG_PINMUX0_PINMUX0_27_24_GPIO0_9   << \
                                    SYSCFG_PINMUX0_PINMUX0_27_24_SHIFT)


#ifndef Bool
typedef unsigned short          Bool;           /* boolean */
#endif


#define GPIO_UNKNOWN_STATE		0
#define GPIO_HIGH_STATE			1
#define GPIO_FALL_STATE			2
#define GPIO_LOW_STATE			3
#define GPIO_RISE_STATE			4


static volatile unsigned char new_GP0p0 = 1;
static volatile unsigned char old_GP0p0 = 1;
static volatile unsigned char new_GP0p1 = 1;
static volatile unsigned char old_GP0p1 = 1;
static volatile unsigned int new_GP0p2 = 1;
static volatile unsigned int old_GP0p2 = 1;
static volatile unsigned int new_GP0p3 = 1;
static volatile unsigned int old_GP0p3 = 1;
static volatile unsigned int new_GP0p4 = 1;
static volatile unsigned int old_GP0p4 = 1;


void GPIOBank0Pin0PinMuxSetup(void);
void GPIOBank0Pin1PinMuxSetup(void);
void GPIOBank0Pin2PinMuxSetup(void);
void GPIOBank0Pin3PinMuxSetup(void);
void GPIOBank0Pin4PinMuxSetup(void);
void GPIOBank0Pin5PinMuxSetup(void);
void GPIOBank0Pin9PinMuxSetup(void);

void GPIOBank0Pin0_initState(int state);
void GPIOBank0Pin1_initState(int state);
void GPIOBank0Pin2_initState(int state);
void GPIOBank0Pin3_initState(int state);
void GPIOBank0Pin4_initState(int state);

Bool GPIOBank0Pin0_isRisen(void);
Bool GPIOBank0Pin1_isRisen(void);
Bool GPIOBank0Pin2_isRisen(void);
Bool GPIOBank0Pin3_isRisen(void);
Bool GPIOBank0Pin4_isRisen(void);

Bool GPIOBank0Pin0_isFallen(void);
Bool GPIOBank0Pin1_isFallen(void);
Bool GPIOBank0Pin2_isFallen(void);
Bool GPIOBank0Pin3_isFallen(void);
Bool GPIOBank0Pin4_isFallen(void);

int GPIOBank0Pin0_State(void);
int GPIOBank0Pin1_State(void);
int GPIOBank0Pin2_State(void);
int GPIOBank0Pin3_State(void);
int GPIOBank0Pin4_State(void);

int GPIOBank0Pin0_CmdAddr_State(unsigned char *cmd_addr);
int GPIOBank0Pin1_CmdAddr_State(unsigned char *cmd_addr);
int GPIOBank0Pin2_CmdAddr_State(unsigned char *cmd_addr);
int GPIOBank0Pin3_CmdAddr_State(unsigned char *cmd_addr);
int GPIOBank0Pin4_CmdAddr_State(unsigned char *cmd_addr);

#endif /* GPIOMUX_H_ */
