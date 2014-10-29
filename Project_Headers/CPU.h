//**************************   INCLUDES   *******************************

#include "derivative.h"

//! @file CPU.h
//! @brief Inicializa el cpu y declara las variables y funciones propias del mismo.

//*************************   DEFINICIONES   ****************************

//////RTC////////

/*! Definicion de ctes de delay??
RTC_1SEG
RTC_1SEG
RTC_MED
RTC_01S
RTC_1MIN
*/

#define RTC_1SEG  0x9F  // Delay de 1 segundo
#define RTC_1MS   0x98  // Delay de 1 msegundo
#define RTC_MED   0x9E  // No Sabemos q es
#define RTC_01S   0x9D  // No Sabemos q es
#define RTC_1MIN  0x00  // Delay de 1 minuto
#define APAGADO 	0
#define CORRIENDO 	1
#define FIN 		2
#define HAYPAQUETE 	3
#define tam_paquete 35
#define MINUTO 60
#define VUELTAS 1 //CADA VUELTA ES 1 MINUTO DORMIDO DEFAULT=60;
#define MUERTO 500 //500
#define UNSEG 10000

///////Transceiver/////// MOVER A TRANSCEIVER

/*! Definicion de los flags del protocolo de comunicacion
flag_dato:
flag_ultimo:
flag_ack: se envía para confirmar la recepción de un dato
flag_muerte:
flag_inicio:
flag_turno:
*/

#define flag_dato   0x00  //  00000000
#define flag_ultimo 0x01  //  00000001
#define flag_ack    0x02  //  00000010
#define flag_muerte 0x04  //  00000100
#define flag_inicio 0x08  //  00001000
#define flag_turno  0x0C  //  00010000

//////Vbles globales//////
/*!
byte vueltasRTC:
int diferencia:
*/

extern byte vueltasRTC;
extern int diferencia;

//**************************   FUNCIONES   ******************************
//! Inicializacion del CPU
error Init_CPU(void);

//! Produce un delay de 100uS multiplicado por el valor recibido
void Cpu_Delay100US(word);

//! Permite iniciar el RTC
/*!
    @param[in]
    @li @c modifica RTCSC
    @li @c modifica RTCMOD

    Configuracion del RTC
        RTIF=1: This status bit indicates the RTC counter register reached the value in the RTC modulo register.
                Writing a logic 1 clears the bit and the real-time interrupt request.
        RTCLKS=00: Real-Time Clock Source Select. 00 Real-time clock source is the 1-kHz low power oscillator (LPO)
        RTIE=1: Real-Time Interrupt Enable. 1 Real-time interrupt requests are enabled.
        RTCPS=pasado en variable: Real-Time Clock Prescaler Select. These four read/write bits select binary-based or decimal-based divide-by
values for the clock source.

    RTCSC=0x80; NO DEBERIA SER 0x90?
*/
void CPU_PrenderRTC(byte,int);

//! Permite deshabilitar el RTC
/*!
    Se deshabilitan las interrupciones del RTC (RTIE=0)
    RTCSC=0x80
*/
void CPU_ApagarRTC(void);

//! Permite conocer el estado de carga de la bateria
/*!
    Se lee el valor desde el ADCRL usando el PTDA7
*/
byte CPU_DameTension(void);

//! Permite conocer la temperatura del encapsulado
/*!
    Se lee el valor desde el ADCRL
*/
byte CPU_DameTemperatura(void);
