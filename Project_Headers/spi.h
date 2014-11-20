#ifndef __SPI__
#define __SPI__

/* Includes */

#include "derivative.h"
#include "types.h" 
//#include "configuracion.h"

/* Definitions */

#define  SPI_SS  PTBD_PTBD5      /* Slave Select */
#define _SPI_SS  PTBDD_PTBDD5    /* Se cambio el PTED 7 por PTBD 5*/

/* Prototypes */

void  SPI_Init(void);
void  SPI_SendByte(UINT8 u8Data);
UINT8 SPI_ReceiveByte(void);
void  SPI_HighRate(void);


#endif /* __SPI__ */

