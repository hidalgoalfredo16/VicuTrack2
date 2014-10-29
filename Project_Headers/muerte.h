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

/*
Arma la trama de se�al de muerte para ser enviado
???
*/

void MUERTE_ArmarPaquete(void);
