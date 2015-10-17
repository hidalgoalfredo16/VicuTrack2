/*!
 @file sd.h
 @brief Declara constantes y funciones para el manejo de la sd
 */

#include "derivative.h"
#include "types.h"
#include "error.h"
#include "gps.h"
#include "sd_command.h"
#include "spi.h"
#include "CPU.h"

//! Define la pata del micro conectado al CS de la sd
#define SD_CS PTBD_PTBD5

//! Define la funcion que realiza un Assert
#define SD_Assert() ( \
    (void) (SD_CS = 0)  \
  )

//! Define la funcion que realiza un DesAssert
#define SD_DesAssert() ( \
    (void) (SD_CS = 1) \
  )

//! Constante que determina el tamaño de bloque
#define SD_BLOCK_SIZE   (0x00000200)
//! Constante que determina un shift de la direccion de la tarjeta SD (?)
#define SD_BLOCK_SHIFT  (9)
//! Constante que determina la cantidad de ciclos que se esperara respuesta a un comando por parte de la sd
#define SD_WAIT_CYCLES  20
//! Constante que determina la cantidad de ciclos en los que se espera respuesta a comandos durante la etapa de inicializacion
#define SD_INIT_RETRIES 5000

//! Define constantes para identificar los distintos tipos de errores
#define SD_FAIL_TIMEOUT 	0xFF
#define SD_FAIL_INIT 	  	0xFE
#define SD_FAIL_READ 		0xFD
#define SD_FAIL_WRITE 		0xFC
#define SD_OK				0x00

//! Define la direccion de memoria del archivo punteros.bin en la tarjeta sd
//#define DIRECCION_BIN 2600 // Direccion del archivo donde se guardan los punteros de lectura y escritura, 8GB=2600 4GB=680 512MB=2576
		
//! Función para envio de un comando a la tarjeta SD
/*! Esta función recibe solo la respuesta  del comando que puede ser
    un byte para la mayoria de los comandos y hasta 5 para algunos en particular
    Para la lectura de un bloque de datos como un sector o el CSD se utiliza esta
    función para enviar el comando y si el comando es aceptado se utiliza
    la función de lectura de datos (SD_ReadData) para recuperar el bloque
    que la tarjeta envia despues de la respuesta.
    @param[in] u8SDCommand Comando a enviar
    @param[in] u32Param Parametro del comando
    @param[out] pu32Response[] Vector donde se almacena la respuesta al comando
    @param[out] u8ResponseLength Tamaño de la respuesta esperada al comando
    @return Indica si el comando fue enviado y respondido con exito
    	@li @c 0xFF SD_FAIL_TIMEOUT No se recibio respuesta por parte de la sd
    	@li @c 0 Se recibio respuesta por parte de la sd
*/
UINT8 SD_SendCommand(UINT8 u8SDCommand, UINT32 u32Param, UINT8 pu32Response[], UINT8 u8ResponseLength);

//! Función para la recepción de un bloque de datos de la tarjeta
/*! Esta función se llama a continuación de la funcion SD_SendCommand para 
    recuperar el bloque de datos que se envia a continuación de la respuesta
    al comando.
    @param[out] pu8DataPointer[] Vector donde se almacena el dato leido
    @param[out] u32DataLength Tamaño del dato a leer
    @return 0 SD_OK
*/
UINT8 SD_ReadData(UINT8 pu8DataPointer[], UINT32 u32DataLength);

//! Función para el envio de un bloque de datos a la tarjeta
/*! Esta función se llama a continuación de la funcion SD_SendCommand para 
    enviar el bloque de datos a continuación de la respuesta
    al comando.
    @param[in] pu8DataPointer[] Vector que contiene el dato a enviar
    @param[in] u32DataLength Tamaño del dato a enviar
    @return Indica si el dato pudo enviarse correctamente
    	@li @c 0xFF SD_FAIL_TIMEOUT Fallo en la escritura. Se envio el dato pero no hubo respuesta de la sd
    	@li @c 0xFC SD_FAIL_WRITE Fallo en la escritura
    	@li @c 0 SD_OK Se escribio correctamente
*/
UINT8 SD_WriteData(UINT8 pu8DataPointer[], UINT32 u32DataLength);

//! Función para la incialización de la tarjeta SD
/*! 
    Esta función para inicializar a la tarjeta en modo de comunicación SPI
    y para detectar el tipo de tarjeta y tomar los parametros de la misma.
    @return Indica si la trajeta se incializo correctamente
      @li @c 0xFE SD_FAIL_INIT Fallo en la inicializacion
      @li @c 0 SD_OK Inicializacion correcta
*/
UINT8 SD_Init(void);

UINT8 SD_Apagar(void);

UINT8 SD_Prender(void);

//! Función para la lectura de un sector de disco de la tarjeta
/*! 
    Esta función realiza la lectura de un sector de 512 bytes de datos
    @param[in] u32SD_Block Direccion del sector fisico de la tarejta donde se va a leer 
    @param[in] pu8DataPointer[] Vector donde se almacenan los datos leidos
    @return Indica si la lectura del sector se produjo correctamente
    	@li @c 0xFD SD_FAIL_READ Fallo en la lectura del sector
    	@li @c 0 SD_OK Lectura de sector correcta
*/
UINT8 SD_ReadSector(UINT32 u32SD_Block,UINT8 pu8DataPointer[]);

//! Función para la escritura de un sector de disco de la tarjeta
/*! 
    Esta función realiza la escritura de un sector de 512 bytes de datos
    @param[in] u32SD_Block Direccion del sector fisico de la tarejta donde se va a escribir
    @param[in] pu8DataPointer[] Vector que contiene los datos a escribir en la sd
    @return Indica si la escritura del sector se produjo correctamente
    	@li @c 0xFC SD_FAIL_WRITE Fallo en la escritura del sector
    	@li @c 0 SD_OK Escritura de sector correcta
*/
UINT8 SD_WriteSector(UINT32 u32SD_Block, UINT8 * pu8DataPointer);

//! Lee un bloque de datos de la tarjeta sd
/*!
Inicializa el SPI, calcula la nueva direccion de lectura y lee un sector de 512 bytes de la tarjeta
@param[in] lectura buffer utilizado, primero para escribir la nueva direccion de lectura y luego para almacenar los datos leidos de la tarjeta
@param[in] direccion Direccion a partir de la cual de leeran los datos
@return Indica si hubo un error
    @li @c 1 _ERR_OK No se produjo un error
*/
error SD_Leer(byte *direccion,dato lectura[][tam_dato]);

//! Escribe datos en la tarjeta
/*! LLama a la funcion SD_WriteSector para que realice la escritura
@param[in] direccion a partir de la cual se escribe
@param[in] datos Datos a escribir
@return Indica si hubo un error
    @li @c 1 _ERR_OK No se produjo un error
*/
error SD_Escribir(byte *direccion,dato datos[][tam_dato]);

//!Calcula y escribe direccion
/*! Calcula y actualiza el valor de la direccion pasada como parametro. Escribe en la tarjeta la nueva direccion
@param[in] dir Direccion actual
@param[in] buf Buffer utilizado para escribir la nueva direccion en la tarjeta
@return Indica si hubo un error
    @li @c _ERR_OK 1 No se produjo un error
*/
error SD_CalculaDireccion(byte *dir, dato buf[][tam_dato]);

//! Controla si la sd está vacía
/*!
Controla si dir_escritura es igual a dir_lectura, si es asi, no hay datos nuevos para transmitir
@return Indica si hubo un error
    @li @c 1 _ERR_OK Hay datos nuevos para transmitir
    @li @c 7 _ERR_DIR No hay datos nuevos para transmitir
*/
error SD_Condatos(void);

//! Lee dirección de escritura guardada en la tarjeta SD
/*! Lee el sector de la tarjeta donde esta almacenada la direccion de escritura y la almacena en una variable global
@return Indica si hubo un error
    @li @c 1 _ERR_OK No se produjo un error
*/
error SD_LeerDireccion(void);

//! Envia el comando 48 para saber si la SD puede entrar en estado de bajo consumo
void SD_Dormir();
