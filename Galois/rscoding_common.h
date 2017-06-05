/*
 * rscoding_common.h
 *
 *  Created on: Oct 30, 2013
 *      Author: Osechkin
 */

#ifndef RSCODING_COMMON_H_
#define RSCODING_COMMON_H_


#define NoD             255             /* No data */


#define E_GF_OK         0               /* No errors */
#define E_GF_RANGE      1               /* Galois base is out of range. Base = 2^N, 2 <= N <= 8 */

#define E_GFP_OK        0               /* No error */
#define E_GFP_FATAL     1               /* Fatal error. Ex.: Pointer is not allocated and etc. */
#define E_GFP_OVERFLOW  2               /* Power of Galois polynome is out of range N (GF(N)) */
#define E_GFP_NODATA    3               /* No data in GFPoly struct */
#define E_GFP_NOTFOUND  4               /* target parameter was not found (roots of polynome etc.) */
#define E_GFP_BADDATA   5               /* Bad data. Ex.: division by zero and etc. */

#define E_RS_OK         0               /* No errors */
#define E_RS_FATAL      1               /* Fatal error. Ex.: Pointer is not allocated and etc. */
#define E_RS_NOTFOUND   2               /* target parameter was not found (locators, etc.) */
#define E_RS_LEN		3				/* bad data length */

#define E_MSG_OK		0				// result = OK
#define E_MSG_FAIL		1				// ошибка в сообщении
#define E_MSG_TYPE		2				// неверный тип сообщения
#define E_MSG_READER	3				// неверный адрес устройства-адресата
#define E_MSG_WRITER	4				// неверный адрес устройства-отправителя
#define E_MSG_LEN		5				// неверная длина тела сообщения
#define E_MSG_REC_LEN	6				// неверное число корректируемых ошибок
#define E_MSG_CRC		7				// ошибка контрольной суммы


typedef char            gint8;          /* 8-bit signed data    */
typedef unsigned char   guint8;         /* 8-bit unsigned data  */
typedef short           gint16;         /* 16-bit signed data 	*/
typedef unsigned short  guint16;    	/* 16-bit unsigned data */
typedef long            gint32;         /* 32-bit signed data   */
typedef unsigned long   guint32;        /* 32-bit unsigned data */




#endif /* RSCODING_COMMON_H_ */
