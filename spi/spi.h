
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

int spi_wait_for_tx_unsafe(void);

int spi_wait_for_tx(void);

char spi_send_byte_with_cs_start(const char byte);

char spi_send_byte_with_cs_nochange(const char byte);

char spi_send_byte_with_cs_finish(const char byte);

void spi_flash_rd (const int adr, char *buf, int n);

int spi_flash_wr (const int adr, char *buf, int n);

void spi_cfg_flash ();

char spi_dummy_read ();

int spi_flash_rd_status ( void );

int spi_flash_wait_wr_complete ( void );

void spi_flash_wr_en ( void );

void spi_flash_wr_dis ( void );

void spi_flash_test_buf_inc_fill (char *buf, int n);


//Unused
void spi_send8(const char byte);

void spi_rcv(char *buf, int n);

void spi_flash_set_adr (const int adr);

void spi_cfg_mdm ();

void spi_send_mdm(const char byte);

char spi_send_byte_with_cs_start(const char byte);

#endif
