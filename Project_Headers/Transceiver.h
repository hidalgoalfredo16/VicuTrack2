//! @file Transceiver.h
//! @brief Contiene todas las funciones que requieren la utilizacion del dispositivo
#include "derivative.h"
#include"error.h"
#include "gps.h"

#define TIMERACK 50
#define ACK 65
#define NACK 33
#define POS_LAT 12
#define POS_LONG 22
#define INTENTOSENVIO 50


//! Inicializacion del Transceiver
error Init_Trans(void);

//! Seteo de parametros, ver hoja de datos, no entiendo
error Transceiver_SetAlto(void);

//! Seteo de parametros, ver hoja de datos, no entiendo
error Transceiver_SetBajo(void);

//! Prende el transceiver
/*! Coloca un "1" en PTAD_PTAD1 que está conectado al Enable del Transceiver*/
error Transceiver_Prender(void);

//! Apaga el transceiver
/*! Coloca un "0" en PTAD_PTAD1 que está conectado al Enable del Transceiver*/
error Transceiver_Apagar(void);

//! Permite transmitir un caracter
/*!
    @param[in]
    @li @c Recibe el caracter que se desea transmitir

    SCI2S1: This register has eight read-only status flags.
        _TDRE:  Transmit Data Register Empty Flag — TDRE is set out of reset and when a transmit data value transfers from
        (bit7)  the transmit data buffer to the transmit shifter, leaving room for a new character in the buffer. To clear TDRE,
                read SCIxS1 with TDRE = 1 and then write to the SCI data register (SCIxD).
                    0 Transmit data register (buffer) full.
                    1 Transmit data register (buffer) empty.
        _TC:    Transmission Complete Flag — TC is set out of reset and when TDRE = 1 and no data, preamble, or break
        (bit6)      character is being transmitted.
                    0 Transmitter active (sending data, a preamble, or a break).
                    1 Transmitter idle (transmission activity complete).
                TC is cleared automatically by reading SCIxS1 with TC = 1 and then doing one of the following three things:
                    • Write to the SCI data register (SCIxD) to transmit new data
                    • Queue a preamble by changing TE from 0 to 1
                    • Queue a break character by writing 1 to SBK in SCIxC2

    SCI2D:  SCI Data Register (SCIxD) This register is actually two separate registers. Reads return the contents of the
            read-only receive data buffer and writes go to the write-only transmit data buffer. Reads and writes of this register
            are also involved in the automatic flag clearing mechanisms for the SCI status flags

    La función consiste en:
        Cargar el byte en SCI2D
        Esperar que el buffer se vacie
        Esperar que se complete la transmision
*/
error Transceiver_EnviarByte(byte);

//! Permite recibir un caracter
/*!
    SCI2S1_RDRF (bit5): Receive Data Register Full Flag — RDRF becomes set when a character transfers from the receive shifter
                        into the receive data register (SCIxD). To clear RDRF, read SCIxS1 with RDRF = 1 and then read the SCI
                        data register (SCIxD).
                            0 Receive data register empty.
                            1 Receive data register full.

    La función consiste en:
        Controlar si se recibio un dato
        Copiar el caracter desde SCI2D

*/
error Transceiver_RecibirByte(byte *);

//! Realiza el envio de datos desde el dispositivo a la base
/*!
    @param[in]
    @li @c Matriz con los datos a ser transmitidos
    @li @c Contador de los datos transmitidos
    @li @c Nº de secuencia de los datos
*/
error Transceiver_Enviar(dato buf[][tam_dato],byte *j,byte *nrosec);

//! Permite recibir un ACK
/*!
    La función consiste en:
        Controlar si se recibio un dato en SCI2S1_RDRF
        Controlar que el byte recibido en SCI2D corresponde con un ACK

*/
error Transceiver_RecibirACK(void);

//! Recibe un paquete y se fija si era para el
/*!
    La función consiste en:
        Controlar si el id es el correspondiente
        Controlar que sea el turno correcto ???????
*/
error Transceiver_Analizar(byte buf[]);

//! Permite saber si el ACK recibido es correcto
/*!
    La función consiste en:
        Controlar si el Id es el que corresponde
        Controlar que sea un ACK
        Verificar que el nº de secuencia sea correcto
        En caso de que algo no este bien, se retorna un error
*/
error Transceiver_AnalizarACK(byte buf[],byte*);

//! Realiza el control para saber si el dato recibido es correcto o no
error Transceiver_ControlarDato(void);

//! Recibe la señal de un animal muerto y guarda la informacion
error Transceiver_RecibirSM(void);
