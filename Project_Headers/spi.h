/*!
 @file spi.h
 @brief Declara constantes y funciones para el manejo del spi
 */

#ifndef __SPI__
#define __SPI__

#include "derivative.h"
#include "types.h"

//! Define el pin del micro utilizado como SPI
#define  SPI_SS  PTBD_PTBD5      /* Slave Select */
//! Define la direccion del pin del micro utilizado como SPI
#define _SPI_SS  PTBDD_PTBDD5    /* Se cambio el PTED 7 por PTBD 5*/


//! Inicializa el SPI
/*!
Configura el pin como salida y lo habilita. Configura los registros SPIBR, SPIC1 y SPIC2
 */
void  SPI_Init(void);

//! Envia un byte a traves del SPI
/*!
Espera a que el buffer de transmision este vacio y envia
@param[in] u8Data Dato a enviar
 */
void  SPI_SendByte(UINT8 u8Data);

//! Recibe un byte del SPI
/*!
Espera hasta que el buffer tiene un dato y retorna
@return Valor leido de SPID
 */
UINT8 SPI_ReceiveByte(void);

//!Configura el SPI a una frecuencia alta
/*!
 Configura el registro SPIBR
 */
void  SPI_HighRate(void);


#endif /* __SPI__ */

