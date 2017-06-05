/*
 * proger_defines.h
 *
 *  Created on: 18.04.2015
 *      Author: aivanov
 *
 */

#ifndef PROGER_DEFINES_H_
#define PROGER_DEFINES_H_

//---------------------------------------------------------------------------
//Programmer type and firmware version
//---------------------------------------------------------------------------
//Interval type programmer has type number 1,
//straight time programmer has type 0.
#define  PROGR_TYPE 1 UL;
#define  PROGR_VERS 53 UL;
#define  PROGR_SUB_VERS 'f';
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Programmer parameters definitions.
//---------------------------------------------------------------------------
// Number of bits in command and output bus data
// (data lenth of programmer external bus).
#define	PROGR_OUT_BUS_WIDE	8

// Number of bits in output address bus (address of programmer external bus).
#define	PROGR_OUT_ADR_WIDE	16

// Number of bytes in singe command.
#define	PROG_BYTES_IN_COM	4

// Maximum count of commands.
#define	PROG_MAX_COMMANDS	4096

#define PROGER_SIZE_IN_BYTES   (PROG_BYTES_IN_COM*PROG_MAX_COMMANDS)

// Should be the same in upp.c
#define UPP_DMA_BUF_SIZE 64

//DDS basic frequency
#define DDS_BASIC_FRQ 250000000
#define DDS_STEPS 4294967296
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Internal programmer addresses.
//---------------------------------------------------------------------------
#define PROGER_BASE_ADDR   (0x66000000 + 0)
#define PROGER_CONFIG_ADDR (PROGER_BASE_ADDR + 28)

#define MEM_ADDR_TMP_WORD (PROGER_BASE_ADDR + 4)
#define MEM_ADDR_CNTR_CLR (PROGER_BASE_ADDR + 8)
#define MEM_ADDR_CNTR_INC (PROGER_BASE_ADDR + 32)
#define MEM_ADDR_FIFO_DTA (PROGER_BASE_ADDR + 12)

#define MEM_ADDR_CNTR_PROC_CLR (PROGER_BASE_ADDR + 40)
#define MEM_ADDR_CNTR_PROC_INC (PROGER_BASE_ADDR + 44)
#define MEM_ADDR_CNTR_PROC_DTA (PROGER_BASE_ADDR + 48)

#define MEM_ADDR_ECHO_CNTR (PROGER_BASE_ADDR + 56)

#define MEM_ADDR_ADC_PNTS_CNTR (PROGER_BASE_ADDR + 96)

#define MEM_ADDR_GAMMA_CNTR01_REG (PROGER_BASE_ADDR + 280)

//#define MEM_ADDR_DDS_FRQ01_REG (PROGER_BASE_ADDR + 284)

//#define MEM_ADDR_IO_STATUS_REG (PROGER_BASE_ADDR + 288)

#define MEM_ADDR_PROGR_PROGR_TYPE_REG (PROGER_BASE_ADDR + 100)
#define MEM_ADDR_PROGR_PROGR_VERS_REG (PROGER_BASE_ADDR + 104)
#define MEM_ADDR_PROGR_PROGR_SUB_VERS_REG (PROGER_BASE_ADDR + 108)

#define MEM_ADDR_DDS_FRQ01_REG (PROGER_BASE_ADDR + 64)
#define MEM_ADDR_DDS_FRQ02_REG (PROGER_BASE_ADDR + 284)
#define MEM_ADDR_DDS_FRQ03_REG (PROGER_BASE_ADDR + 288)
#define MEM_ADDR_DDS_FRQ04_REG (PROGER_BASE_ADDR + 292)
#define MEM_ADDR_DDS_FRQ05_REG (PROGER_BASE_ADDR + 296)
#define MEM_ADDR_DDS_FRQ06_REG (PROGER_BASE_ADDR + 300)
//reserved #define MEM_ADDR_DDS_FRQ01_REG (PROGER_BASE_ADDR + 304)

#define MEM_ADDR_GAMMA_CNTR01_RST_REG (PROGER_BASE_ADDR + 312)

#define MEM_ADDR_DEV_STATE_REG (PROGER_BASE_ADDR + 316)

#define MEM_ADDR_CTRL_REG		(PROGER_BASE_ADDR + 320)
#define MEM_ADDR_PIT_OUT_REG	(PROGER_BASE_ADDR + 324)
#define MEM_ADDR_PIT_LV_REG		(PROGER_BASE_ADDR + 328)

#define MEM_ADDR_CNTR_SET (PROGER_BASE_ADDR + 332)

#define MEM_ADDR_PROC_GPIO (PROGER_BASE_ADDR + 336)

#define MEM_ADDR_CH_NUM (PROGER_BASE_ADDR + 340)

#define MEM_ADDR_DEVICE_SERIAL (PROGER_BASE_ADDR + 344)

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Confiuratoin memory in processor address space defined here
//---------------------------------------------------------------------------

#define MEM_ADDR_CONF_MEM ( 0xC0000000 )

#define CONF_MEM_LEN ( 0x20000 )

//---------------------------------------------------------------------------
//Programmer commands defined here
//---------------------------------------------------------------------------
#define	COM_RET						0

#define	COM_EMPTY					128

#define	 COM_PROC_CONTROL_STB_0		193
#define	 COM_PROC_CONTROL_STB_1		194
#define	 COM_PROC_CONTROL_STB_2		195
#define	 COM_PROC_CONTROL_STB_3		196
#define	 COM_PROC_CONTROL_STB_4		197
#define	 COM_PROC_CONTROL_STB_5		198
#define	 COM_PROC_CONTROL_STB_6		199
#define	 COM_PROC_CONTROL_STB_7		200

#define	COM_WR_RST_PROC_MEM			201
#define	COM_WR_INC_PROC_MEM			202

#define	COM_WR_INS_STACK			203
#define	COM_EXEC_INS_STACK			204

#define	COM_ENB_TUNE				205

#define	COM_SET_VC_DAC_1			206
#define	COM_SET_VC_DAC_2			207

#define	COM_BLANK_PWR				208

#define	COM_ENB_DAMP				215

#define	COM_SET_RCV_ADDR			216
#define	COM_SET_PA_ADDR				217

#define	COM_SWITCH_FRQ				218

#define	COM_PH_RST					219

#define	COM_OFS01					220
#define	COM_PH01					221
#define	COM_RF01					222
#define	COM_RF01_ON_OFF				223

#define	COM_OFS02					225
#define	COM_PH02					226
#define	COM_RF02					227
#define	COM_RF02_ON_OFF				228

#define	COM_OFS03					230
#define	COM_PH03					231
#define	COM_RF03					232
#define	COM_RF03_ON_OFF				233

#define	COM_OFS04					235
#define	COM_PH04					236
#define	COM_RF04					237
#define	COM_RF04_ON_OFF				238

#define	COM_TIME					241

//#define	COM_ADS_IN					242// ADC now controlled by bit, which is the same as GPIO[1]

#define	COM_CYCLE_OPEN				243
#define	COM_CYCLE_CLOSE				244

//#define	COM_MACRO_JUMP				245// not implemented yet
//#define	COM_MACRO_RET				246// not implemented yet

#define	COM_TEST_CONF_WR			250

#define	COM_SYNC					251

#define COM_SIMPLE_CYCLE_OPEN		253
#define COM_SIMPLE_CYCLE_CLOSE		254

#define	COM_STOP					255

//#define	COM_TIME2				16// not imlemented
//#define	COM_CYCLE_OPEN2			17
//#define	COM_CYCLE_CLOSE2		18
//#define	COM_RF01W				32
//#define	COM_ADC					33
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Commands from Programmer to Processor defined here
//---------------------------------------------------------------------------

// Wait from MM

#define GPIO_5_12_DIELEC	0xE0 //224
#define GPIO_5_12_GAMMA		0xFD //253
#define GPIO_5_12_ADC		0xFE //254
#define GPIO_5_12_NO_DEV	0xC8 //200
#define GPIO_5_12_PACK_03	0x03 //200

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Processor control from FPGA register definitions
//---------------------------------------------------------------------------

#define	ARG_AWAKE_LED_ON	(1 << 7)
#define	ARG_AWAKE_LED_OFF	(0 << 7)

#define	ARG_STB_ACT			(1 << 0)
#define	ARG_STB_DEACT		(0 << 0)

#define	ARG_GPIO_0_ACT		(1 << 0)
#define	ARG_GPIO_0_DEACT	(0 << 0)

#define	ARG_GPIO_1_ACT		(1 << 1)
#define	ARG_GPIO_1_DEACT	(0 << 1)

#define	ARG_GPIO_2_ACT		(1 << 2)
#define	ARG_GPIO_2_DEACT	(0 << 2)

#define	ARG_GPIO_3_ACT		(1 << 3)
#define	ARG_GPIO_3_DEACT	(0 << 3)

#define	ARG_GPIO_4_ACT		(1 << 4)
#define	ARG_GPIO_4_DEACT	(0 << 4)

#define	ARG_PWR_ON			(0 << 0)
#define	ARG_PWR_OFF			(1 << 0)

#define	ARG_DAMP_ON			(1 << 0)
#define	ARG_DAMP_OFF		(0 << 0)

#define	ARG_RF_ON			(1 << 0)
#define	ARG_RF_OFF			(0 << 0)

#endif /* PROGER_DEFINES_H_ */
