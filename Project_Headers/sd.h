/*!
@file sd.h
@brief Declara las variables y funciones públicas para el manejo de la tarjeta sd.
*/
#include "derivative.h"
#include "types.h"
#include "error.h"
#include "gps.h"
#include "sd_command.h"
//#include "null.h"
#include "spi.h"
#include "CPU.h"


#define OVERRUN_ERR      0x01          // Overrun error flag bit
#define CHAR_IN_RX       0x08          // Char is in RX buffer  
#define FULL_TX          0x10          // Full transmit buffer  
#define RUNINT_FROM_TX   0x20          // Interrupt is in progress 
#define FULL_RX          0x40          // Full receive buffer   
#define DUMMY_BYTE       0xFF          
#define CRC_CMD0         0x95
#define CMD0             0x40
#define CMD1             0x41
#define CMD16            0x50
#define CMD17            0x51
#define CMD24            0x58
#define TOKEN            0xFE
#define TAM_BLOQUE        512
#define TIMER_RESP       2000
#define SD_BM_285_714KHZ          0    // Constant for switch to mode 0 */
#define SD_BM_2MHZ                1    // Constant for switch to mode 1 */
#define LECTURA_16DATOS      0
#define LECTURA_BLOQUE    1
#define ID_H              0xCA
#define ID_L              0xFE
#define SD_Assert() ( \
    (void)(PTBD_PTBD5 = 0)  	/* PTBD5=0x00*/  \
  )
#define SD_DesAssert() ( \
    (void)(PTBD_PTBD5 = 1)      /* PTBD5=0x01 */ \
  )

#define SD_BLOCK_SIZE   (0x00000200)
#define SD_BLOCK_SHIFT  (9)

#define SD_WAIT_CYCLES  20
#define SD_INIT_RETRIES 3000

#define SD_FAIL_TIMEOUT 0xFF
#define SD_FAIL_INIT 	  0xFE
#define SD_FAIL_READ 		0xFD
#define SD_FAIL_WRITE 	0xFC
#define SD_OK						0x00

#define InitSPI()       (void)SPI_Init()
#define ReadSPIByte()   SPI_ReceiveByte()
#define WriteSPIByte(x) (void)SPI_SendByte(x)
#define HighSpeedSPI()  SPI_HighRate()

		
//! Función para envio de un comando a la tarjeta SD
/*! Esta función recibe solo la respuesta  del comando que puede ser
    un byte para la mayoria de los comandos y hasta 5 para algunos en particular
    Para la lectura de un bloque de datos como un sector o el CSD se utiliza esta
    función para enviar el comando y si el comando es aceptado se utiliza
    la función de lectura de datos (SD_ReadData) para recuperar el bloque
    que la tarjeta envia despues de la respuesta.
*/
UINT8 SD_SendCommand(UINT8 u8SDCommand, UINT32 u32Param, UINT8 pu32Response[], UINT8 u8ResponseLength);

//! Función para la recepción de un bloque de datos de la tarjeta
/*! Esta función se llama a continuación de la funcion SD_SendCommand para 
    recuperar el bloque de datos que se envia a continuación de la respuesta
    al comando.
*/
UINT8 SD_ReadData(UINT8 pu8DataPointer[], UINT32 u32DataLength);

//! Función para el envio de un bloque de datos a la tarjeta
/*! Esta función se llama a continuación de la funcion SD_SendCommand para 
    enviar el bloque de datos a continuación de la respuesta
    al comando.
*/
UINT8 SD_WriteData(UINT8 pu8DataPointer[], UINT32 u32DataLength);

//! Función para la incialización de la tarjeta SD
/*! 
    Esta función para inicializar a la tarjeta en modo de comunicación SPI
    y para detectar el tipo de tarjeta y tomar los parametros de la misma.  
*/
UINT8 SD_Init(void);


//! Enciende tarjeta SD
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/
error SD_Prender(void);

//! Envia byte CMD
/*!
@param[in] CMD
@param[in] arg
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/
error SD_EnviarCMD(byte,byte*);

//! Envia byte a la tarjeta SD
/*!
@param[in] data dato a enviar
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/
error SD_EnviarByte(byte);

//! Recibe byte de la tarjeta SD
/*!
@param[in] Rx byte donde se almacena el dato
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/
error SD_RecibirByte(byte*);

//! Envia y recibe byte de la tarjeta SD
/*!
@param[in] Tx dato a enviar
@param[in] Rx byte donde se almacena el dato
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/
error SD_EnviarRecibirByte(byte,byte*);

//! Lee datos de la tarjeta
/*!
@param[in] lectura variable donde se almacenarán los datos leidos
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 5 Se produjo un error de timer
*/
error SD_Leer(dato lectura[][tam_dato]);

//! Escribe datos en la tarjeta
/*!
@param[in] direccion a partir de la cual se escribe
@param[in] datos a escribir
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 5 Se produjo un error de timer
*/
error SD_Escribir(byte *direccion,dato datos[][tam_dato]);

//! Establece el Baud Rate
/*!
@param[in] mod indica cual de los dos baud rate se establecerá
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 6 Se produjo un error, mod incorrecto
*/
error SD_SetBaudRateMode(byte);

//!Calcula y escribe direccion
/*!
@param[in] dir, direccion
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 7 Se produjo un error de direccion
*/
error SD_CalculaDireccion(byte *dir);

//! Escritura de prueba
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 5 Se produjo un error de tiemer
*/
error SD_escribirPrueba(void);

//! Controla si la sd está vacía
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 7 Se produjo un error de direccion
*/
error SD_Condatos(void);

//!Escribe dirección en tarjeta SD
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 5 Se produjo un error de tiemer
*/
error SD_EscribirDireccion(void);

//! Lee dirección de tarjeta SD
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 5 Se produjo un error de tiemer
*/
error SD_LeerDireccion(void);
