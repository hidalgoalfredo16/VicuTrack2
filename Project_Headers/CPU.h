/*
 @file cpu.h
 !@brief Declara constantes y funciones para el manejo del cpu
 */

#include "derivative.h"

//* Constantes enviadas a la funcion CPU_PrenderRTC y determinan en que tiempo el RTC interrumpe
#define RTC_1SEG  0x9F  // Delay de 1 segundo
#define RTC_1MS   0x98  // Delay de 1 msegundo
#define RTC_MED   0x9E  // Delay medio seg
#define RTC_01S   0x9D  // Delay 0,1 seg
#define RTC_1MIN  0x00  // Delay de 1 minuto

//! Constante utilizada para dar valor a las siguientes variables: ban_vueltacomp, ban_turno, ban_ACK, ban_muerte, ban_finao
#define APAGADO 	0
//! Constante utilizada para dar valor a las siguientes variables: ban_vueltacomp, ban_turno, ban_muerte, ban_finao
#define CORRIENDO 	1
//! Constante utilizada para dar valor a las siguientes variables: ban_vueltacomp, ban_turno, ban_ACK, ban_muerte, ban_finao
#define FIN 		2
//! Constante utilizada para dar valor a las siguientes variables: ban_turno, ban_ACK, ban_muerte
#define HAYPAQUETE 	3
//! Contante que indica el tama�o del paquete. id: 1 byte, flag: 1 byte, nrosec: 1 byte, payload: 31 bytes, checksum: 1 byte
#define tam_paquete 35
//! Constante utilizada como paramtero en la funcion CPU_PrenderRTC. 
//! Tambien utilizada para dar valor a la variable vueltasRTC para hacer que el dispositivo duerma una hora
#define MINUTO 60
//! Constante utilizada para dar valor a la variable vueltasRTC e indica cuanto tiempo el dispositivo estara dormido durante el periodo de bajo consumo
#define VUELTAS 1 //CADA VUELTA ES 1 MINUTO DORMIDO DEFAULT=60;
//! Constante que indica cuantas veces se tomara un dato igual a los anteriores antes de declarar al dispositivo en estado de muerte
#define MUERTO 500 //500
//! Constante utilizada como parametro en la funcion Cpu_Delay100US
#define UNSEG 10000

//* Constantes utilizadas para indicar el tipo de paquete transmitido o recibido por el transceiver
#define flag_dato   0x00  //  00000000
#define flag_ultimo 0x01  //  00000001
#define flag_ack    0x02  //  00000010
#define flag_muerte 0x04  //  00000100
#define flag_inicio 0x08  //  00001000
#define flag_turno  0x0C  //  00010000

#define TENSION_OUT PTAD_PTAD7

extern byte vueltasRTC;
extern int diferencia;

//! Inicializacion del CPU
/*
 Configura regsitros SOPT1, SOPT1, SPMSC1, SPMSC2, SPMSC3. Configura el clock. Inicializa variables globales
 @return 1 _ERR_OK Se inicializo correctamente
 */
error Init_CPU(void);

//! Produce un delay
/*
 Produce un delay de 100uS multiplicado por el valor recibido como parametro
 @param[in] us100 Indica el multiplicador que definira la longitud del delay 
 */
void Cpu_Delay100US(word);

//! Permite iniciar el RTC
/*! Se configura el RTC de la siguiente manera:
        RTIF=1: This status bit indicates the RTC counter register reached the value in the RTC modulo register.
                Writing a logic 1 clears the bit and the real-time interrupt request.
        RTCLKS=00: Real-Time Clock Source Select. 00 Real-time clock source is the 1-kHz low power oscillator (LPO)
        RTIE=1: Real-Time Interrupt Enable. 1 Real-time interrupt requests are enabled.
        RTCPS=pasado en variable: Real-Time Clock Prescaler Select. These four read/write bits select binary-based or decimal-based divide-by values for the clock source.
    @param[in] cps modifica la parte baja del RTCSC que corresponde al Clock Prescaler Select
    @param[in] modulo modifica RTCMOD
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
    Se envia un 1 en un pin del micro se lee mediante un conversor AD en otro pin. Configura el registro del conversor AD
    @return Valor de tension leido en ADCRL
*/
byte CPU_DameTension(void);

//! Permite conocer la temperatura del encapsulado
/*!
    Configura el registro del conversor AD y lee el valor desde el ADCRL
     @return Valor de temperatura leido en ADCRL
*/
byte CPU_DameTemperatura(void);
