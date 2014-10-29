/*
 * muerte.c
 *
 *  Created on: Sep 30, 2014
 *      Author: Agustin
 */

#include <hidef.h>
#include "muerte.h"
#include "error.h"
#include "CPU.h"
#include "gps.h"
#include "led.h"
#include "sd.h"
#include "Transceiver.h"

extern trama_crudo tc[tam_trama_crudo];
extern dato dat[tam_dato];
extern byte ban_finao;
extern byte id;
int diferencia;


void RUTINA_MUERTE(void){
	trama_reducida tr[tam_trama_reducida];
	int minutos;
	byte intentos_gps=0,i=0,j=0;
	minutos=((dat[1]-0x30)+(dat[0]-0x30)*10)*60+((dat[3]-0x30)+(dat[2]-0x30)*10);
	if(870<minutos){
		diferencia=870+(1440-minutos);
	}else{
		diferencia=870-minutos;
	}
	ban_finao=CORRIENDO;
	CPU_PrenderRTC(RTC_1SEG,MINUTO);
	while(ban_finao==CORRIENDO){
		asm{STOP
		}
	}
	for(;;){
		CPU_ApagarRTC();
		(void)GPS_Prender();
		CPU_PrenderRTC(RTC_1SEG,40);//espero para que fixee el GPS. default=40seg
		EnableInterrupts;
		asm{STOP
		}
		CPU_ApagarRTC();
		////////// PRIMERA PARTE: TOMA DE DATOS DEL GPS //////////
		do{
			(void)GPS_Recibir(tc);// recibo datos en crudo del GPS durante 5 intentos
		}while(GPS_Analizar(tr,tc) != _ERR_OK && intentos_gps++<5);// analizamos si son datos validos 5 veces
		(void)GPS_Apagar();
		if(intentos_gps<5){
			(void)GPS_Dato(dat,tr);// limpiamos la trama y dejamos solo los datos importantes
			minutos=((dat[1]-0x30)+(dat[0]-0x30)*10)*60+((dat[3]-0x30)+(dat[2]-0x30)*10);// convertimos a minutos la hora del gps
			diferencia=900-minutos;
			ban_finao=CORRIENDO;
			CPU_PrenderRTC(RTC_1SEG,MINUTO);
			while(ban_finao==CORRIENDO){
				asm{STOP
				}
			}
			MUERTE_ArmarPaquete();
			for(i=0;i<60;i++){
				(void)Transceiver_Prender();
				MUERTE_TransceiverEnviar();
				(void)Transceiver_Apagar();
				for(j=0;j<10;j++)
					Cpu_Delay100US(UNSEG);
			}
			diferencia=1400;
			ban_finao=CORRIENDO;
			CPU_PrenderRTC(RTC_1SEG,60);
			while(ban_finao==CORRIENDO){
				asm{STOP;
				}
			}
		}else{   //cierra el if de 5 intentos
			diferencia=1400;
			ban_finao=CORRIENDO;
			CPU_PrenderRTC(RTC_1SEG,60);
			while(ban_finao==CORRIENDO){
				asm{STOP;
				}
			}
		}
		intentos_gps=0;
	}// llave del FOR

}

void MUERTE_TransceiverEnviar(void){
	byte i=0;
	(void)Transceiver_EnviarByte(id);
	(void)Transceiver_EnviarByte(flag_muerte);
	(void)Transceiver_EnviarByte(0x01);
	i=0;
	while(i<tam_dato){
		(void)Transceiver_EnviarByte(dat[i]);
		i++;
	}
}

void MUERTE_ArmarPaquete(void){
	byte e=0,suma=0;
	dat[0]=0x57;
	dat[1]=0x57;
	dat[2]=0x57;
	dat[3]=id;
	while(e<tam_dato-1){
		if(e!=12 && e!=22)
			suma=suma+(dat[e]-0x30);
		e++;
	}
	dat[29]=suma;
}
