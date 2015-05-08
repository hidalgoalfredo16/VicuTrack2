/*!
 @file transceiver.h
 @brief Declara constantes y funciones para el manejo del transceiver
 */

#include "derivative.h"
#include"error.h"
#include "gps.h"

//! Define la posicion en que se encuentra LAT en el vector que guarda el dato
#define POS_LAT 12
//! Define la posicion en que se encuentra LONG en el vector que guarda el dato
#define POS_LONG 22

//! Define la direccion de la pata del micro conectada al set del transceiver
#define TransceiverSet_Direccion PTADD_PTADD0
//! Define la pata del micro conectada al set del transceiver
#define TransceiverSet PTAD_PTAD0
//! Define la direccion de la pata del micro conectada al enable del transceiver
#define TransceiverEnable_Direccion PTADD_PTADD1
//! Define la pata del micro conectada al enable del transceiver
#define TransceiverEnable PTAD_PTAD1

//! Inicializacion del Transceiver
/*!
 Configura el puerto serial mediante registros SCIxBDH, SCIxC1, SCIxC2, SCIxC3. 
 Configura pins de set y enable como salida. Configura set en bajo y enable en alto
 Configura los parametros del transceiver f=455MHz, Data Rate 9600bps, output power 100mw, UART data rate 9600bps, no checkout
 @return Indica si los parametros del transceiver se configuraron correctamente
 	 @li @c 1 _ERR_OK Los parametros del transceiver se configuraron correctamente
 	 @li @c 16 _ERR_DISTINTO Los parametros del transceiver no se configuraron correctamente
 */
error Init_Trans(void);

//! Coloca un 1 en la pata Set del Transceiver
/*!
 @return 1 _ERR_OK
 */
error Transceiver_SetAlto(void);

//! Coloca un 0 en la pata Set del Transceiver
/*!
 @return 1 _ERR_OK
 */
error Transceiver_SetBajo(void);

//! Prende el transceiver
/*! 
Coloca un "1" en el Enable y un '1' en el Set del Transceiver
@return 1 _ERR_OK
*/
error Transceiver_Prender(void);

//! Apaga el transceiver
/*! 
Coloca un "0" en el Enable y un '0' en el Set del Transceiver
@return 1 _ERR_OK
*/
error Transceiver_Apagar(void);

//! Permite transmitir un caracter
/*!
Carga el dato a transmitir en el registro SCI2D. Espera a que el buffer se vacie. Espera a que la transmision se complete
@param[in] byte Recibe el caracter que se desea transmitir
@return 1 _ERR_OK
*/
error Transceiver_EnviarByte(byte);

//! Permite recibir un caracter
/*! Controla el bit SCI2S1_RDRF para saber si hay un dato en el buffer de recepcion. Si hay un dato lo almacena.
@param[out] Rxdat Almacena el byte recibido
@return Indica si hubo un dato en el buffer de recepcion
	@li @c 1 _ERR_OK Habia un dato y se guardo
	@li @c 9 _ERR_RXEMPTY No habia un dato
*/
error Transceiver_RecibirByte(byte *);

//! Realiza el envio de datos desde el dispositivo a la base
/*!
 Envia un bloque de datos de 512 bytes a la base 
 Envia el id del dispositivo, el flag indicando que es un dato, el numero de secuencia y el dato en si
 Se espera el ack por parte de la base
 En caso de que el dato enviado haya sido el ultimo del buffer, se carga el buffer con datos de la sd, si ésta los tuviera
 @param[in] buf Buffer con los datos a ser transmitidos
 @param[inout] j Contador de los datos transmitidos
 @param[inout] nrosec Nº de secuencia de los datos
 @return 13 _ERR_ACK
*/
error Transceiver_Enviar(dato buf[][tam_dato],byte *j,byte *nrosec);

//! Recibe un paquete y se fija si es un turno para el
/*!
Controlar si el id es el correspondiente y si es un paquete del tipo 'turno'
@param[in] buf Contiene el dato recibido y que debe analizarse
@return Indica si el dato es un turno para mi
	@li @c 1 _ERR_OK El dato es un turno para mi
 	@li @c 14 _ERR_TURNO El dato no es un turno y/o no es para mi
*/
error Transceiver_Analizar(byte buf[]);

//! Permite saber si el ACK recibido es correcto
/*!
Controla si el Id es el que corresponde, controla que sea un ACK y verifica que el nº de secuencia sea correcto
@param[in] buf Contiene el dato recibido y que debe analizarse
@param[in] nrosec Indica el numero de secuencia del dato
@return Inidica si el dato es el ACK esperado
	@li @c 1 _ERR_OK El dato recibido es el ACK esperado
 	@li @c 13 _ERR_ACK El dato recibido no es el ACK esperado      
*/
error Transceiver_AnalizarACK(byte buf[],byte*);

//! Realiza el control para saber si el dato recibido es correcto o no
/*
Realiza un control del checksum
@return Indica si el checksum es correcto o no 
	@li @c 1 _ERR_OK El checksum es correcto
	@li @c 12 _ERR_DATO El checksum no es correcto
 */
error Transceiver_ControlarDato(void);

//! Recibe la señal de un animal muerto y guarda la informacion
/*!
Enciende el transceiver y espera a recibir datos, si es asi, los almacena en un buffer y posteriormente en la sd
@return 1 _ERR_OK
 */
error Transceiver_RecibirSM(void);
