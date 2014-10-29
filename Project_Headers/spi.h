#ifndef __SPI__
#define __SPI__

/* Includes */

#include "derivative.h"
#include "types.h" 
//#include "configuracion.h"

/* Definitions */

#define  SPI_SS  PTED_PTED7      /* Slave Select */
#define _SPI_SS  PTEDD_PTEDD7    

/* Prototypes */

void  SPI_Init(void);
void  SPI_SendByte(UINT8 u8Data);
UINT8 SPI_ReceiveByte(void);
void  SPI_HighRate(void);


#endif /* __SPI__ */

