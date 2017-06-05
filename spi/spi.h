
/*
================================================================================
                            INLCUDE FILES
================================================================================
*/

#ifndef SPI_H_
#define SPI_H_

#include <stdio.h>
//#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_spi.h>
#include <ti/pspiom/cslr/cslr.h>
#include "OMAPL138_common.h"



#define SPI_NUM_OF_TXBITS   8
#define SPI_NEW_DATA_NO   0x01

/*
================================================================================
                         LOCAL FUNCTION PROTOTYPES
================================================================================
*/

void spi_send8(const char byte);

void spi_rcv(char *buf, int n);

void spi_flash_rd (const int adr, char *buf, int n);

void spi_flash_set_adr (const int adr);



#endif
