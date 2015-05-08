/*!
@file muerte.h
@brief Declara las variables y funciones públicas para el manejo de la función de muerte.
*/

#include "derivative.h"

//! Encargada del funcionamiento de la señal de muerte
/*!
Es llamada cuando el contador que indica la cantidad de veces que se puede repetir el mismo dato tomado por el gps llega a su limite
Cada un intervalo de tiempo se enciende el gps y se reciben datos y se intenta transmitir un paquete con las modificaciones correspondientes.
*/
void RUTINA_MUERTE(void);

//! Envía la señal de muerte
/*!
Envía id del collar, flag de muerte y la trama dat
*/
void MUERTE_TransceiverEnviar(void);

//! Genera la trama de la señal de muerte que luego será enviada
/*
Agrega un inicio de trama al comienzo del dato tomado por el gps para distinguir que es una señal de muerte y agrega el id del dispositivo
Calcula el cheksum del dato y lo agrega a la trama
*/
void MUERTE_ArmarPaquete(void);
