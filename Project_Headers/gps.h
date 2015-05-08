/*!
@file gps.h
@brief Declara las variables y funciones públicas para el manejo del gps.
*/
#include "derivative.h"
#include "error.h"

//! Define el tipo de dato 'dato' como unsigned char
#define dato unsigned char
//! Define el tipo de dato 'trama_crudo' como unsigned char
#define trama_crudo unsigned char
//! Define el tipo de dato 'trama_reducida' como unsigned char
#define trama_reducida unsigned char

//! Constante que indica el tamaño del dato. id: 1 byte, payload: 31 bytes
#define tam_dato 32
//! Constante que indica la cantidad de datos en un bloque de 512 bytes
#define cantidad_datos 16 //era 14
//! Valor que toma la variable ban_horasm en cualquier otro momento que no sea la hora de recibir la señal de muerte
#define NO 4
//! Valor que toma la variable ban_horasm cuando es la hora de recibir la señal de muerte
#define SI 5
//! Constante que define el tamaño de la trama en crudo tomada por el gps
#define tam_trama_crudo 256
//! Constante que define el tamaño de la trama reducida a partir de la trama cruda
#define tam_trama_reducida 72
//! Constante que define el valor del caracter '$'
#define _PESOS 0x24
//! Constante que define el valor del caracter 'G'
#define _G 0x47
//! Constante que define el valor del caracter 'P'
#define _P 0x50
//! Constante que define el valor del caracter 'A'
#define _A 0x41
//! Constante que define el valor del caracter 'R'
#define _R 0x52
//! Constante que define el valor del caracter 'M' 
#define _M 0x4d
//! Constante que define el valor del caracter 'C'
#define _C 0x43
//! Constante que define el valor del caracter ','
#define _COMA 0x2c
//! Constante que define el valor del caracter 'A' que indica que el dato tomado por el gps es valido
#define _FIXED  0x41
//#define _NO_FIXED 0x30  //valido para trama gpgga verificar para gprmc 
//! Constante que define el valor de la cantidad de comas que se encuentran en una trama $GPMRC correcta
#define _TRAMA_GPRMC_OK 12
//! Constante que define la posicion aceptable de la trama $GPMRC dentro de una trama cruda de gps
#define _POSICION_GPRMC_ACEPTABLE 183
//! Constante que define la posicion donde se encuentra el caracter que indica si el gps fixeo dentro de la trama $GPMRC
#define _POS_FIXED 18
//#define _RAD_25M 20 
//! Constante que define el radio de 800m en los cuales deberia encontrarse la base en relacion al movil para poder transmitir
#define _RAD_800M 1280
//! Constante que define el horario de recepcionde señal de muerte
#define _HORA_MUERTE 1500

//! Define la direccion del pin conectado a VCC del gps
#define GPS_VCC_DIR PTCDD_PTCDD1
//! Define el pin conectado a VCC del gps
#define GPS_VCC PTCD_PTCD1 

//! Inicializacion del GPS
/*!
 Inicializa el puerto SCI, configura el pin conectado a vcc como salida, inicializa variable ban_fix
 @return 1 _ERR_OK Se inicializo correctamente
 */
error Init_GPS(void);

//! Enciende el módulo GPS
/*!
 Envia un '0' por el pin conectado a vcc
@return 1
*/
error GPS_Prender(void);

//! Apaga el módulo GPS
/*!
Envia un '1' por el pin conectado a vcc
@return 1
*/
error GPS_Apagar(void);

//! Recibe la trama cruda del GPS
/*!
    Se recojen datos crudos del puerto serial y se van almacenando en el vector tc hasta llenarlo
    @param[out] tc vector donde se almacenara la trama cruda
    @return Indica si hubo un error
        @li @c 17 _ERR_GPS Se produjo un error en la recepcion
        @li @c 1 _ERR_OK No se produjo un error
*/
error GPS_Recibir(trama_crudo *tc);

//! Se analiza si la trama recibida es correcta y se almacena la trama $GPRMC
/*!
Se analiza si la trama recibida contiene la subtrama GPRMC en forma completa, si es asi, se la almacena en un nuevo vector tr
    @param[out] tr es el vector donde se almacenará la trama $GPRMC
    @param[in] tc contiene la trama cruda recibida
@return Indica si hubo un error
        @li @c 1 _ERR_OK Se encontro la trama $GPRMC completa y correcta
        @li @c 2_ERR_COMAS  La cantidad de comas que deberia tener la trama $GPRMC no es correcta
        @li @c 3 _ERR_TRAMA_CRUDO La trama cruda no es correcta
        @li @c 4 _ERR_TRAMA_NO_FIXED El dato de la trama cruda no es correcto porque el gps no fixeo
*/
error GPS_Analizar(trama_reducida *tr,trama_crudo* tc);

//! Se obtiene la parte no variable de la trama GPRMC
/*!
Se toman solo los datos importantes de la trama (hora,minutos,latitud, longitud y Fecha) sin puntos ni coma

$GPRMC,205550.000,A,0443.9191,N,07402.8330,W,0.00,,240113,,,A*6A
-Hora y minutos:2055
-Latitud: 04439191N
-Longitud: 074028330W
-Fecha: 240113

@param[out] d Vector donde se almacenarán los datos de la trama GPRMC que nos interesa
@param[in] tr Vector donde se almacena la trama GPRMC
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
@param[out] buf es el buffer donde se van almacenando todos los datos recogidos del gps para luego ser volcados en la SD
@return Indica si el buffer esta lleno o no
    @li @c 1 _ERR_OK Indica que el buffer todavia no esta lleno
    @li @c 8 _ERR_OVF Indica que el buffer esta lleno
*/
error GPS_EscribirBuffer(dato dat[], dato buf[][tam_dato]);

//! Verifica si el movil está cerca de la base
/*!
Verifica si el movil esta en un radio de 800m
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@return Indica si la base esta cerca o no
    @li @c 1 _ERR_OK La base esta dentro del radio de 800m
    @li @c 15 _ERR_DISTINTO La base no está dentro del radio de 800m
*/
error GPS_CompararBase(dato *);

//! Compara con la medición anterior para saber si el móvil de desplazó
/*!
Compara la posición actual con la anterior para saber si el móvil se encuentra en una posición considerada igual
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@param[in] ult_lat Latitud del ultimo dato
@param[in] ult_lon Longitud del ultimo dato
@return Indica si el dato es igual al anterior o no
    @li @c 1 _ERR_OK El móvil está en la misma posición
    @li @c 15 _ERR_DISTINTO El móvil no está en la misma posición
*/
error GPS_CompararDato(dato dat[],long *ult_lat,long *ult_lon);

//!* Copia el dato en un nuevo vector
/*!
Guardo el dato en un nuevo vector en el caso de que el móvil se encuentre en la misma posición, por si cambia de lugar en la próxima
@param[in] dat es el vector donde se almacenan los datos de la trama GPRMC que nos interesa
@param[out] ult_dat vector donde se copiará dat
@return Indica si hubo un error
    @li @c 1 _ERR_OK No se produjo un error
*/
error GPS_CopiarDato(dato dat[],dato ult_dat[]);

//! Compara hora actual con hora de muerte
/*!
Si la comparacion está entre 0 y 6 horas retorna OK, caso contrario retorna error
@return Indica si hubo un error
    @li @c 1 _ERR_OK No se produjo un error
    @li @c 16 _ERR_HORA Se produjo un error de hora
*/
//error GPS_VerificarHora();

//! Corrige el RTC para despertar en horario de señal de muerte
/*!
@return Indica si hubo un error
    @li @c 1 No se produjo un error
*/
error GPS_SincronizarHM(void);
