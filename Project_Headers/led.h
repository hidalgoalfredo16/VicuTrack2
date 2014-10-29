/*!
@file led.h
@brief Declara las variables y funciones públicas para el manejo de leds indicadores
*/
#include "derivative.h"
#include"error.h"

//! Inicializacion del LED
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
