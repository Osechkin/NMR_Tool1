/*
================================================================================
                            INLCUDE FILES
================================================================================
*/

#include <stdio.h>
//#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_spi.h>
#include <ti/pspiom/cslr/cslr.h>
#include "OMAPL138_common.h"
#include "../Common/OMAPL138_global.h"
#include "spi.h"


#define SPI_NUM_OF_TXBITS   8
#define SPI_NEW_DATA_NO   0x01


CSL_SpiRegsOvly  spiRegs  = (CSL_SpiRegsOvly) (CSL_SPI_0_REGS);
CSL_SpiRegsOvly  spiRegs1 = (CSL_SpiRegsOvly) (CSL_SPI_1_REGS);

/*
================================================================================
                         LOCAL FUNCTION PROTOTYPES
================================================================================
*/


        	//spiRegs->SPIBUF |=  CSL_FMK(SPI_SPIBUF_RXEMPTY,0x01);
extern volatile int spi_rx_started = 0, spi_tx_started = 0;
volatile char spi_rx_buf[1024];


int spi_wait_for_tx_unsafe(void)
{
	volatile int status = 0;

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	return(0); //no error
}

int spi_wait_for_tx(void)
{
	volatile int status = 0, i = 0, err = 1;

	for(i = 0; i < 1E+6; i++) //1E+6 is est. at 3 ms time
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG));
		if ( status  == 0x01 )
		{
			err = 0;
			break;
		}
	};

	return(err); //0 = no error
}


void spi_send8(const char byte)
{
	volatile int status = 0;
	//status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};
	CSL_FINS(spiRegs->SPIDAT1,SPI_SPIDAT1_TXDATA, byte);
}

void spi_rcv(char *buf, int n)
{
    int i;

    spi_send8(0x03);

    for(i=0; i < n; i++)
    {
        /* check if data is recieved                                          */
        while ((CSL_FEXT(spiRegs->SPIBUF,SPI_SPIBUF_RXEMPTY))
                                                    == SPI_NEW_DATA_NO);

        buf[i] = (CSL_FEXT(spiRegs->SPIBUF,SPI_SPIBUF_RXDATA));
        //i++;
    }
}

void spi_flash_rd (const int adr, char *buf, int n)
{
	int i, err;

	err = spi_flash_wait_wr_complete();

	spi_send_byte_with_cs_start(0x03);

	spi_send_byte_with_cs_nochange( (char) (adr >> 16) );
	spi_send_byte_with_cs_nochange( (char) (adr >>  8) );
	spi_send_byte_with_cs_nochange( (char) (adr >>  0) );

	spi_send_byte_with_cs_nochange(0);// dummy read

    for(i=0; i < (n-1); i++)
    {
    	buf[i] = spi_send_byte_with_cs_nochange(i);
    }

    buf[n-1] = spi_send_byte_with_cs_finish(i);
}

int spi_flash_wr (const int adr, char *buf, int n)
{
	int i, err;

	err = spi_flash_wait_wr_complete();

	spi_flash_wr_en();

	spi_send_byte_with_cs_start(0x02);

	spi_send_byte_with_cs_nochange( (char) (adr >> 16) );
	spi_send_byte_with_cs_nochange( (char) (adr >>  8) );
	spi_send_byte_with_cs_nochange( (char) (adr >>  0) );

    for(i=0; i < (n-1); i++)
    {
    	spi_send_byte_with_cs_nochange(buf[i]);
    }
    spi_send_byte_with_cs_finish(buf[n-1]);

	spi_flash_wait_wr_complete();
	spi_flash_wr_dis();

    return (err);
}

int spi_flash_rd_status ( void )
{
	int status_reg = 0xFF;

	spi_send_byte_with_cs_start(0x05);

	spi_send_byte_with_cs_nochange(0);// dummy read

	status_reg = spi_send_byte_with_cs_finish(0);

	return (status_reg);
}

int spi_flash_wait_wr_complete ( void )
{
	volatile int status = 0, i = 0, err = 1;

	for(i = 0; i < 5E+3; i++) //5E+3 is est. at 4 s time
	{
		status = spi_flash_rd_status() & 0x01;
		if ( status  == 0x00 )
		{
			err = 0;
			break;
		}
	};

	return(err); //0 = no error
}

void spi_flash_wr_en ( void )
{
	spi_send_byte_with_cs_finish(0x06);
}

void spi_flash_wr_dis ( void )
{
	spi_send_byte_with_cs_finish(0x04);
}



void spi_flash_test_buf_inc_fill (char *buf, int n)
{
	int i;

    for(i=0; i < n; i++)
    {
    	buf[i] = i;
    }
}

void spi_flash_test_buf_ff_fill (char *buf, int n)
{
	int i;

    for(i=0; i < n; i++)
    {
    	buf[i] = 0xFF;
    }
}



void spi_flash_set_adr (const int adr)
{
	spi_send8( 0x03 );

	spi_send8( (char) (adr >> 16) );
	spi_send8( (char) (adr >>  8) );
	spi_send8( (char) (adr >>  0) );

}



void spi_cfg_mdm ()
{
	CSL_FINST(spiRegs1->SPIGCR0, SPI_SPIGCR0_RESET, IN_RESET);
	CSL_FINST(spiRegs1->SPIGCR0, SPI_SPIGCR0_RESET, OUT_OF_RESET);

	//CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_ENABLE,    ENABLE);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_LOOPBACK,  DISABLE);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_POWERDOWN, DISABLE);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_CLKMOD,    INTERNAL);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_MASTER,    MASTER);

	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_SOMIFUN,  SPI);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_SIMOFUN,  SPI);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_CLKFUN,   SPI);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_ENAFUN,   GPIO);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_SCS0FUN1, SPI);//GPIO

	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_SOMIDIR,  INPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_SIMODIR,  OUTPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_CLKDIR,   OUTPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_ENADIR,   INPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_SCS0DIR1, OUTPUT);

	CSL_FINST(sysRegs->PINMUX5, SYSCFG_PINMUX5_PINMUX5_3_0, NSPI1_SCS1);//GPIO2_15

	CSL_FINST(spiRegs1->SPIDAT1, SPI_SPIDAT1_DFSEL, FORMAT0);
	CSL_FINS (spiRegs1->SPIDAT1, SPI_SPIDAT1_CSNR,  0xFD);//FFFD

	CSL_FINST(spiRegs1->SPIFMT[0], SPI_SPIFMT_SHIFTDIR, MSB);
	CSL_FINST(spiRegs1->SPIFMT[0], SPI_SPIFMT_POLARITY, LOW);
	CSL_FINST(spiRegs1->SPIFMT[0], SPI_SPIFMT_PHASE,    NO_DELAY);
	CSL_FINS (spiRegs1->SPIFMT[0], SPI_SPIFMT_PRESCALE, 100); // 2-255
	CSL_FINS (spiRegs1->SPIFMT[0], SPI_SPIFMT_CHARLEN,  8); // 2-16

	CSL_FINS (spiRegs1->SPIDEF,    SPI_SPIDEF_CSDEF0,   0xFF);

	//CSL_FINS (dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_SPIINT1);


	CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_SPIINT1);

	CSL_FINST(spiRegs1->SPIINT0, SPI_SPIINT0_RXINTENA, ENABLE);
	CSL_FINST(spiRegs1->SPILVL,  SPI_SPILVL_RXINTLVL, INT1);

	//CSL_FINST(spiRegs1->SPIINT0, SPI_SPIINT0_TXINTENA, ENABLE);
	//CSL_FINST(spiRegs1->SPILVL,  SPI_SPILVL_TXINTLVL, INT1);

	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_ENABLE,    ENABLE);
}

void spi_cfg_flash ()
{
	CSL_FINST(spiRegs1->SPIGCR0, SPI_SPIGCR0_RESET, IN_RESET);
	CSL_FINST(spiRegs1->SPIGCR0, SPI_SPIGCR0_RESET, OUT_OF_RESET);

	//CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_ENABLE,    ENABLE);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_LOOPBACK,  DISABLE);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_POWERDOWN, DISABLE);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_CLKMOD,    INTERNAL);
	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_MASTER,    MASTER);

	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_SOMIFUN,  SPI);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_SIMOFUN,  SPI);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_CLKFUN,   SPI);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_ENAFUN,   GPIO);
	CSL_FINST(spiRegs1->SPIPC0, SPI_SPIPC0_SCS0FUN0, SPI);

	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_SOMIDIR,  INPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_SIMODIR,  OUTPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_CLKDIR,   OUTPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_ENADIR,   INPUT);
	CSL_FINST(spiRegs1->SPIPC1, SPI_SPIPC1_SCS0DIR0, OUTPUT);

	CSL_FINST(sysRegs->PINMUX5, SYSCFG_PINMUX5_PINMUX5_7_4, NSPI1_SCS0);

	CSL_FINST(spiRegs1->SPIDAT1, SPI_SPIDAT1_DFSEL, FORMAT0);
	CSL_FINS (spiRegs1->SPIDAT1, SPI_SPIDAT1_CSNR,  0xFE);

	CSL_FINST(spiRegs1->SPIFMT[0], SPI_SPIFMT_SHIFTDIR, MSB);
	CSL_FINST(spiRegs1->SPIFMT[0], SPI_SPIFMT_POLARITY, LOW);//LOW//HIGH
	CSL_FINST(spiRegs1->SPIFMT[0], SPI_SPIFMT_PHASE,    DELAY);//DELAY//NO_DELAY
	CSL_FINS (spiRegs1->SPIFMT[0], SPI_SPIFMT_PRESCALE, 100); // 2-255
	CSL_FINS (spiRegs1->SPIFMT[0], SPI_SPIFMT_CHARLEN,  8); // 2-16


	CSL_FINS (spiRegs1->SPIDELAY, SPI_SPIDELAY_C2TDELAY,  8); // 0-FF
	CSL_FINS (spiRegs1->SPIDELAY, SPI_SPIDELAY_T2CDELAY,  8); // 0-FF

	CSL_FINS (spiRegs1->SPIDEF,    SPI_SPIDEF_CSDEF0,   0xFF);

	//CSL_FINS (dspintcRegs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_SPIINT1);


	CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_SPIINT1);

	CSL_FINST(spiRegs1->SPIINT0, SPI_SPIINT0_RXINTENA, ENABLE);
	CSL_FINST(spiRegs1->SPILVL,  SPI_SPILVL_RXINTLVL, INT1);

	//CSL_FINST(spiRegs1->SPIINT0, SPI_SPIINT0_TXINTENA, ENABLE);
	//CSL_FINST(spiRegs1->SPILVL,  SPI_SPILVL_TXINTLVL, INT1);

	CSL_FINST(spiRegs1->SPIGCR1, SPI_SPIGCR1_ENABLE,    ENABLE);
}

//static void setup_spi_int_6 ()
//{

	// Map spi system interrupts to DSP INT6
	//CSL_FINS(dspintcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6, CSL_INTC_EVENTID_SPIINT1);

	// Assign the address of the IST to the IST pointer
	//ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	//ICR = DSPINTC_IST_ALL_MASK_INT;

	 //Enable INT6 interrupts
	//IER |= DSPINTC_IST_NMI | DSPINTC_IST_INT6;

//}// setup_upp_int_6 */
/*---------------------------------------------------------------------------*/

void spi_send_mdm(const char byte)
{
	volatile int status = 0, d_out = 0;
	volatile unsigned char d_in = 0;

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

/*	while (1)
	{
		CSL_FINST(spiRegs1->SPIPC4, SPI_SPIPC4_SCS0SET1, SET);
		d_out = CSL_FEXT (spiRegs1->SPIPC3, SPI_SPIPC3_SCS0DOUT1);
		d_in  = CSL_FEXT (spiRegs1->SPIPC2, SPI_SPIPC2_SCS0DIN1);
		dummyDelay(1000);
		CSL_FINST(spiRegs1->SPIPC5, SPI_SPIPC5_SCS0CLR1, CLEAR);
		d_out = CSL_FEXT (spiRegs1->SPIPC3, SPI_SPIPC3_SCS0DOUT1);
		d_in  = CSL_FEXT (spiRegs1->SPIPC2, SPI_SPIPC2_SCS0DIN1);
		dummyDelay(1000);
	}*/

/*WTF 1 piece of code*//*
	unsigned int data = 0, d_tmp = 0;

	data  = spiRegs1->SPIDAT1;
	data &= 0xFFFF0000;
	data |= 1 << 28;
	d_tmp = byte;
	data |= d_tmp;
*/
	volatile unsigned int data = 0, d_tmp = 0;

	data |= byte;
	data &= 0x000000FF;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp |= 1 << 28;

	data |= d_tmp;

	spiRegs1->SPIDAT1 = data;

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};
	d_in = spiRegs1->SPIBUF;
	//d_in = spiRegs1->SPIFLG;

	CSL_FINS(spiRegs1->SPIDAT0,SPI_SPIDAT0_TXDATA, byte);

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};
	d_in = spiRegs1->SPIBUF;

	data = byte;
	data &= 0x000000FF;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp &= ~(1 << 28);

	data |= d_tmp;

	spiRegs1->SPIDAT1 = data;
	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};
	d_in = spiRegs1->SPIBUF;
	/**/
}

void spi_txbuf_prepare(int *data, int count)
{
	volatile int status = 0, d_out = 0, d_in = 0, d_tmp = 0, i;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp |= 1 << 28;



	//if ( (count != 0) && (count <= SPI_TXBUFLEN) )
		for ( i = 0; i < count-1; i++ )
		{
			data[i] &= 0x000000FF;
			data[i] |= d_tmp;
		};
	data[count-1] &= 0x000000FF;
}

interrupt void spi_isr(void)
{
	static volatile int count = 0;
	volatile int d_in = 0;

	if ( CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG) == 0x01 )
	{
		if ( spi_tx_started == 1 )
		{
			//for (int i = 0; i < spi_tx_len; i++ )
			{
				//spiRegs1->SPIDAT1 = spi_tx_buf[i];
			}
		}
	};

	if ( CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_RXINTFLG) == 0x01 )
	{
		if ( spi_rx_started == 1 )
		{
			;
		}
	};

	d_in = spiRegs1->SPIFLG;


	spiRegs1->SPIFLG = 0xFFFFFFFF;
	count++;
}


void spi_send_mdm2 (const char byte)
{
	volatile int status = 0, d_out = 0, d_in = 0;

	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	CSL_FINS (spiRegs1->SPIDEF, SPI_SPIDEF_CSDEF0, 0xFD);

	CSL_FINS(spiRegs1->SPIDAT0,SPI_SPIDAT0_TXDATA, byte);

	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	CSL_FINS(spiRegs1->SPIDAT0,SPI_SPIDAT0_TXDATA, byte);

	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	CSL_FINS(spiRegs1->SPIDAT0,SPI_SPIDAT0_TXDATA, byte);

	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	CSL_FINS (spiRegs1->SPIDEF, SPI_SPIDEF_CSDEF0, 0xFF);
}

void spi_set_reg (const int word, int *mdm_data_in, int *mdm_data_out)
{
	volatile int status = 0, d_out = 0;
	volatile unsigned char d_in = 0;
	volatile unsigned int data = 0, d_tmp = 0;

	memset(spi_rx_buf, 0x00, 1024 * sizeof(char));

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	spiRegs1->SPIFLG = 0xFFFFFFFF;


	data |= *mdm_data_out >> 8;
	data &= 0x000000FF;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp |= 1 << 28;

	data |= d_tmp;

	spiRegs1->SPIDAT1 = data;

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_RXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_RXINTFLG)) == 0x01;
	};
	spi_rx_buf[0] = spiRegs1->SPIBUF;


	data = *mdm_data_out;
	data &= 0x000000FF;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp &= ~(1 << 28);

	data |= d_tmp;

	spiRegs1->SPIDAT1 = data;
	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_TXINTFLG)) == 0x01;
	};

	status = (CSL_FEXT(spiRegs->SPIFLG, SPI_SPIFLG_RXINTFLG)) == 0x01;
	while (!status)
	{
		status = (CSL_FEXT(spiRegs1->SPIFLG, SPI_SPIFLG_RXINTFLG)) == 0x01;
	};
	spi_rx_buf[1] = spiRegs1->SPIBUF;
	;

	*mdm_data_in = *spi_rx_buf;
}

char spi_send_byte_with_cs_start(const char byte)
{
	volatile int status = 0;
	volatile unsigned char d_in = 0;
	volatile unsigned int data = 0, d_tmp = 0;

	spi_wait_for_tx();

	data |= byte;
	data &= 0x000000FF;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp |= 1 << 28;

	data |= d_tmp;

	spiRegs1->SPIDAT1 = data;

	spi_wait_for_tx();
	d_in = spiRegs1->SPIBUF;//d_in = spiRegs1->SPIBUF;

	return d_in;
}

char spi_dummy_read()
{
	volatile unsigned char d_in = 0;

	spi_wait_for_tx();
	d_in = spiRegs1->SPIBUF;

	return d_in;
}


char spi_send_byte_with_cs_nochange(const char byte)
{
	volatile unsigned char d_in = 0;

	spi_wait_for_tx();

	CSL_FINS(spiRegs1->SPIDAT0,SPI_SPIDAT0_TXDATA, byte);

	spi_wait_for_tx();

	d_in = spiRegs1->SPIBUF;//d_in = spiRegs1->SPIBUF;

	return d_in;
}

char spi_send_byte_with_cs_finish(const char byte)
{
	volatile int status = 0, d_out = 0;
	volatile unsigned char d_in = 0;
	volatile unsigned int data = 0, d_tmp = 0;

	spi_wait_for_tx();

	data = byte;
	data &= 0x000000FF;

	d_tmp  = spiRegs1->SPIDAT1;
	d_tmp &= 0xFFFF0000;
	d_tmp &= ~(1 << 28);

	data |= d_tmp;

	spiRegs1->SPIDAT1 = data;

	spi_wait_for_tx();

	d_in = spiRegs1->SPIBUF;//d_in = spiRegs1->SPIBUF;

	return d_in;
}
/*
================================================================================
                            END OF FILE
================================================================================
*/

