/*!
@file gps.h
@brief Declara las variables y funciones públicas para el manejo del gps.
*/
#include "derivative.h"
#include "error.h"

//*************************   DEFINICIONES   ****************************
#define tam_dato 32
#define cantidad_datos 14
#define NO 4
#define SI 5
#define dato unsigned char
#define trama_crudo unsigned char
#define trama_reducida unsigned char
#define tam_trama_crudo 256
#define tam_trama_reducida 72
#define _PESOS 0x24
#define _G 0x47
#define _P 0x50
#define _A 0x41
#define _R 0x52
#define _M 0x4d
#define _C 0x43
#define _COMA 0x2c
#define _FIXED  0x41
#define _NO_FIXED 0x30  //valido para trama gpgga verificar para gprmc 
#define _TRAMA_GPRMC_OK 12
#define _POSICION_GPRMC_ACEPTABLE 183
#define _POS_FIXED 18
#define _RAD_25M 20 
#define _RAD_800M 1280
#define _HORA_MUERTE 1500

//! Inicializacion del GPS
error Init_GPS(void);

//! Enciende el módulo GPS
/*!
@return Indica si hubo un error
    @li @c 0 Se produjo un error
    @li @c 1 No se produjo un error
*/

error GPS_Prender(void);

//! Apaga el módulo GPS
/*!
@return Indica si hubo un error
    @li @c 0 Se produjo un error
    @li @c 1 No se produjo un error
*/

error GPS_Apagar(void);

//! Recibe datos del GPS
/*!
    Se recojen datos crudos del puerto serial y se van almacenando en el vector tc hasta llenarlo
    @param[in] tc es el vector donde se almacenara la trama cruda
    @return Indica si hubo un error
        @li @c 0 Se produjo un error
        @li @c 1 No se produjo un error
*/

error GPS_Recibir(trama_crudo *tc);

//! Se analiza si la trama recibida es correcta.
/*!
Se analiza si la trama recibida contiene la subtrama GPRMC en forma completa, si es asi, se la almacena en un nuevo vector tr
    @param[in] tr es el vector donde se almacenará la trama GPRMC
    @param[in] tc contiene la trama completa recibida
@return Indica si hubo un error
        @li @c 1 No se produjo un error
        @li @c 2 Se produjo un error en la cantidad de comas que deberia tener la trama GPRMC
        @li @c 3 Se produjo un error en la trama cruda
*/

error GPS_Analizar(trama_reducida *tr,trama_crudo* tc);

//! Se obtiene la parte no variable de la trama GPRMC
/*!
Se eliminan 3 datos de la trama GPRMC (no se cuales son)
@param[in] d es el vector donde se almacenarán los datos de la trama GPRMC que nos interesa
@param[in] tr tr es el vector donde se almacena la trama GPRMC
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/

error GPS_Dato(dato d[],trama_reducida tr[]);

//! Escribe datos en el buffer
/*!
Escribe datos en el buffer hasta que este se llena, en este caso se devuelve un error de overflow.
Se agrega al final de los datos del gps datos de la temperatura y tension del CPU
Se calcula checksum y se agrega al final de la trama
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@param[in] buf es el buffer donde se van almacenando todos los datos recogidos del gps para luego ser volcados en la SD
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 8 Se produjo un error de overflow, buffer lleno
*/

error GPS_EscribirBuffer(dato dat[], dato buf[][tam_dato]);

//! Verifica si el movil está cerca de la base
/*!
Verifica si el movil esta en un radio de 800m
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 15 El móvil no está cerca de la base
*/

error GPS_CompararBase(dato *);

//! Compara con la medición anterior para saber si el móvil de desplazó
/*!
Compara la posición actual con la anterior para saber si el móvil se encuentra en una posición considerada igual
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@param[in] ult_lat Latitud del ultimo dato
@param[in] ult_lon Longitud del ultimo dato
@return Indica si hubo un error
    @li @c 1 El móvil está en la misma posición
    @li @c 15 El móvil no está en la misma posición
*/

error GPS_CompararDato(dato dat[],long *ult_lat,long *ult_lon);

//!* Copia el dato en un nuevo vector
/*!
Guardo el dato en un nuevo vector en el caso de que el móvil se encuentre en la misma posición, por si cambia de lugar en la próxima
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@param[in] ult_dat vector donde se copiará dat
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/

error GPS_CopiarDato(dato dat[],dato ult_dat[]);

//! Compara hora actual con hora de muerte
/*!
Si la comparacion está entre 0 y 6 horas retorna OK, caso contrario retorna error
@return Indica si hubo un error
    @li @c 1 No se produjo un error
    @li @c 16 Se produjo un error de hora
*/

error GPS_VerificarHora();

//! Correge el RTC para despertar en horario de señal de muerte
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/

error GPS_SincronizarHM(void);
