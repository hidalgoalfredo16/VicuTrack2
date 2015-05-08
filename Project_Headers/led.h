/*!
 @file led.h
 !@brief Declara constantes y funciones para el manejo de los leds
 */

#include "derivative.h"
#include"error.h"

//! Define la direccion de la pata del micro conectada al led rojo
#define LedV_Direccion PTCDD_PTCDD5
//! Define la pata del micro conectada al led rojo
#define LedV PTCD_PTCD5
//! Define la direccion de la pata del micro conectada al led verde
#define LedR_Direccion PTCDD_PTCDD4
//! Define la pata del micro conectada al led verde
#define LedR PTCD_PTCD4

//! Inicializacion del LED
/*!
 	Setea como salida las patas del microprocesador conectadas al led
 	@return 1 _ERR_OK Se inicializo correctamente 
 */
error Init_LED(void);

//! Enciende led rojo
void LED_PrenderR(void);

//! Apaga led rojo
void LED_ApagarR(void);

//! Titila led rojo
/*!
@param[in] int indica la cantidad de veces que se repetirá la secuencia de apagado/encendido
@param[in] word indica por cuantos cientos de milisegundos el led permanece en estado apagado o encendido
*/
void LED_BrillarR(int , word);

//! Enciende led verde
void LED_PrenderV(void);

//! Apaga led verde
void LED_ApagarV(void);

//! Titila led verde
/*!
@param[in] int indica la cantidad de veces que se repetirá la secuencia de apagado/encendido
@param[in] word indica por cuantos cientos de milisegundos el led permanece en estado apagado o encendido
*/
void LED_BrillarV(int , word);
