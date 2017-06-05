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
#include "spi.h"


#define SPI_NUM_OF_TXBITS   8
#define SPI_NEW_DATA_NO   0x01


CSL_SpiRegsOvly  spiRegs = (CSL_SpiRegsOvly) (CSL_SPI_0_REGS);


/*
================================================================================
                         LOCAL FUNCTION PROTOTYPES
================================================================================
*/


        	//spiRegs->SPIBUF |=  CSL_FMK(SPI_SPIBUF_RXEMPTY,0x01);


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
	spi_send8( 0x03 );

	spi_send8( (char) (adr >> 16) );
	spi_send8( (char) (adr >>  8) );
	spi_send8( (char) (adr >>  0) );

	spi_rcv( buf, n);

}

void spi_flash_set_adr (const int adr)
{
	spi_send8( 0x03 );

	spi_send8( (char) (adr >> 16) );
	spi_send8( (char) (adr >>  8) );
	spi_send8( (char) (adr >>  0) );

}


/*
================================================================================
                            END OF FILE
================================================================================
*/

