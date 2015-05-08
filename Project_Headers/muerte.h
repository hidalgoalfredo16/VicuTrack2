/*!
@file muerte.h
@brief Declara las variables y funciones p�blicas para el manejo de la funci�n de muerte.
*/

#include "derivative.h"

//! Encargada del funcionamiento de la se�al de muerte
/*!
Es llamada cuando el contador que indica la cantidad de veces que se puede repetir el mismo dato tomado por el gps llega a su limite
Cada un intervalo de tiempo se enciende el gps y se reciben datos y se intenta transmitir un paquete con las modificaciones correspondientes.
*/
void RUTINA_MUERTE(void);

//! Env�a la se�al de muerte
/*!
Env�a id del collar, flag de muerte y la trama dat
*/
void MUERTE_TransceiverEnviar(void);

//! Genera la trama de la se�al de muerte que luego ser� enviada
/*
Agrega un inicio de trama al comienzo del dato tomado por el gps para distinguir que es una se�al de muerte y agrega el id del dispositivo
Calcula el cheksum del dato y lo agrega a la trama
*/
void MUERTE_ArmarPaquete(void);
