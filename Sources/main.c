/*!
 @file main.c
 @brief 
 */

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "gps.h"
#include "led.h"
#include "sd.h"
#include "Transceiver.h"
#include "muerte.h"
#include "CPU.h"

//! Buffer que almacena datos leidos de la sd para ser enviados
dato Buffer_Envio[cantidad_datos][tam_dato]; // Vector para la transmision
//!Buffer que almacena datos tomados por el gps para luego ser escritos en la sd
dato Buffer_GPS[cantidad_datos][tam_dato];
//! Buffer que almacena un dato recibido por el transceiver
byte Buffer_Rx[tam_paquete];
//! Vector que almacena una trama en crudo tomada por el gps
trama_crudo tc[tam_trama_crudo];
//! Vector que almacena la parte no variable de la trama GPRMC
dato dat[tam_dato];
//! Vector donde se copia dat para luego ser escrito en el buffer
dato ult_dat[tam_dato];
//! Bandera que indica si es la hora de recepcion de la se�al de muerte
byte ban_horasm=NO;
//! Vector que almacena la direccion en donde debe realizarse la proxima escritura de datos
byte dir_escritura[4];
//! Vector que almacena la direccion en donde debe realizarse la proxima lectura de datos
byte dir_lectura[4];
//////// DECLARACION DE VARIABLES EXTERNAS //////////
extern byte ban_fix;
extern byte ban_esperafix;
extern byte ban_bufferTx;
extern byte ban_SDvacia;
extern byte ban_turno;
extern byte index_Rx;
extern byte ban_ACK;
extern byte ban_vueltacomp;
void main(void) {
  UINT32 u32SD_Block;
  error temp;                   
  byte nrosec=1,ban_datogps=0;
  byte i,intentos_gps=0,intentos_turno=0,intentos_ack=0,indice=0, ban_datodif=0;
  int sync=0, cont_muerte=0;
  error resp2, resp3;
  long ult_lat=0,ult_lon=0;
  trama_reducida tr[tam_trama_reducida];

  ///////////////// INICIALIZACIONES /////////////// 
  temp=Init_CPU();	
  temp=Init_LED();
  Cpu_Delay100US(UNSEG);
  temp=Init_Trans();
  temp=Init_GPS();
  (void)GPS_Prender();
  Cpu_Delay100US(UNSEG);
  Cpu_Delay100US(UNSEG);
  temp=(error) SD_Init();
  LED_BrillarV(2,UNSEG);
  (void)SD_Apagar();
  (void)GPS_Apagar();
  ult_lat=0;
  ult_lon=0;
  vueltasRTC=VUELTAS;
  EnableInterrupts;
  /* include your code here */
  
  for(;;) {
  ///////////////// RECEPCION SE�AL DE MUERTE ///////////////       

		 if(ban_horasm == SI){
			EnableInterrupts;
			(void)Transceiver_RecibirSM();	  
			ban_horasm=NO;
			vueltasRTC=MINUTO; //Duerme 1 hora
		 } 

 /////////////// PRENDEMOS EL GPS PARA QUE FIXEE ///////////////			  

		 intentos_gps=0;
		 DisableInterrupts;
		 CPU_ApagarRTC();
		 (void)GPS_Prender();
		 if(ban_fix==1)
			CPU_PrenderRTC(RTC_1SEG,40);// 40 espero para que fixee el GPS. default=40seg; con pila de fix = 5;
		 else
			CPU_PrenderRTC(RTC_1SEG,120); //120
		
		EnableInterrupts;
		ban_esperafix=0;
		while(ban_esperafix==0){
			
		  asm{STOP
		 }
		}
		 CPU_ApagarRTC();

 ////////// PRIMERA PARTE: TOMA DE DATOS DEL GPS //////////

		do{
			 (void)GPS_Recibir(tc);// recibo datos en crudo del GPS durante 5 intentos
				  
		}while(GPS_Analizar(tr,tc) != _ERR_OK && intentos_gps++<5);// analizamos si son datos validos 5 veces
		(void)GPS_Apagar();
		 if(intentos_gps<5){
			resp2 = GPS_Dato(dat,tr);// limpiamos la trama y dejamos solo los datos importantes
			//LED_BrillarV(2,UNSEG); // Avisa que tengo un dato bien tomado del GPS
			ban_datogps=1;	 //indica q logro tomar un dato gps.
			vueltasRTC=VUELTAS;      //para prueba
			(void)GPS_SincronizarHM(); //Corregimos el RTC para despertar en horario de se�al de muerte
			
 ///////// CONTROLAMOS SI SE MOVIO EL MOVIL ////////////
			//LED_ApagarV();
			if(/*GPS_CompararDato(dat,&ult_lat,&ult_lon)!=_ERR_OK*/1==1){// (va !=)comparamos con la medicion anterior para saber si me movil
				cont_muerte=0;									 //ERROR OK->Iguales
				if(ban_datodif==1){// esta bandera dice que ya cambio entonces a cont. escribimos el ultimo dato 
					 ban_datodif=0; //igual y a cont. escribimos el dato distinto.
					 if( GPS_EscribirBuffer(ult_dat,Buffer_GPS)==_ERR_OVF){// escribimos en el buffer, si esta lleno informa
						 (void)GPS_Prender();	//Esto se hace ya que la sd esta conectada al regulador del GPS, lo ideal seria que se use otra pata del micro
						 (void)SD_Prender(); 
						  temp=(error) SD_Init();
						  resp3=SD_Escribir(dir_escritura,Buffer_GPS);// escribimos la SD con el buffer lleno
						  resp2=SD_CalculaDireccion(dir_escritura, Buffer_GPS); // Usamos ese buffer xq necesitamos mandar 512 bytes y ya se guardo lo que tenia
						 (void)GPS_EscribirBuffer(ult_dat,Buffer_GPS);// escribimos el dato q no se pudo guardar anteriormente
						 (void)SD_Apagar(); 
						 (void)GPS_Apagar();
					 }
				}
				if( GPS_EscribirBuffer(dat,Buffer_GPS)==_ERR_OVF){ //va ==
					(void)GPS_Prender();	//Esto se hace ya que la sd esta conectada al regulador del GPS, lo ideal seria que se use otra pata del micro
					(void)SD_Prender(); 
					temp=(error) SD_Init(); 
					resp3=SD_Escribir(dir_escritura,Buffer_GPS);
					resp2=SD_CalculaDireccion(dir_escritura,Buffer_GPS);
					(void)GPS_EscribirBuffer(dat,Buffer_GPS);
					(void)SD_Apagar(); 
					(void)GPS_Apagar();
					/*for(i=0;i<10;i++){
					 	 LED_BrillarR(2,300); //Avisa que se esta escribiendo la SD
					 	 LED_BrillarV(2,300);  
					}*/
				}
			}
			else{
				cont_muerte++;
				(void)GPS_CopiarDato(dat,ult_dat);//guardo el ultimo dato por
				ban_datodif=1;                    // si cambia de posicion en la prox.
			}	
		 } // Cierra el if de intentos_gps   
		intentos_gps=0; //Limpiamos la bandera que marca los intentos de tomar datos del GPS
		
 ////////// SEGUNDA PARTE: TRANSMISION DE DATOS //////////
		
		if(ban_datogps==1){
			ban_datogps=0;
			if((GPS_CompararBase(dat)==_ERR_OK) && (ban_bufferTx==0 || ban_SDvacia==0)){ // si esta cerca de la base y tiene algo para transmitir 
				EnableInterrupts;
				vueltasRTC=VUELTAS;//tiene mas prioridad la Tx a la base q la SM
				ban_horasm=NO;//
				do{
					(void)Transceiver_Prender();
					while(sync<300){  // escuchamos 30 seg esperando un turno para nosotros de la base
						EnableInterrupts; 
						ban_turno=CORRIENDO;          
						CPU_PrenderRTC(RTC_1MS,100); // controlamos que llegue el paquete entero en el tiempo esperado
						sync++;
						while(ban_turno==CORRIENDO){             
						}
						if(ban_turno==HAYPAQUETE){ // si llego un paquete reiniciamos los flags
							index_Rx=0;
							DisableInterrupts;
							ban_turno=APAGADO;
							ban_ACK=APAGADO;
							CPU_ApagarRTC();
							// TransAnaliz recibe un paquete y se fija si era para mi
							if(Transceiver_Analizar(Buffer_Rx)==_ERR_OK){
								intentos_turno=4;// para salir del lazo de intentos de recibir turno                     
								EnableInterrupts;
								// primero cargo el buffer_envio con datos para ser transmitidos y luego transmito
								if(ban_bufferTx==1 && SD_Condatos()== _ERR_OK){// si bufferTx vacio y hay datos en 
									indice=0;
									resp2=SD_Leer(dir_lectura, Buffer_Envio); // la SD lo cargo
									ban_bufferTx=0; //buffer de transmision con datos
								}
								(void)Transceiver_Enviar(Buffer_Envio,&indice,&nrosec);
								sync=300;
								intentos_turno=4;
							} //cierra el if de transceiver analizar
						} //cierra el if de HAY paquete
						index_Rx=0;
					} // Cierra el while de sync
					sync=0;
					////// me durmo 4min e intento recibir turno de nuevo //////
					(void)Transceiver_Apagar();
					EnableInterrupts;
					CPU_PrenderRTC(RTC_1SEG,1);//Originalmente 240
					asm{STOP
					}
					
				}while(intentos_turno++<4);//Intento 4 veces recibir un turno	
			}//cierra if de comparar base
			intentos_turno=0;
		}// cierra el if de ban_datogps

 /////////// ENTRO EN RUTINA DE MUERTE DESP DE 2 SEMANAS EN EL MISMO LUGAR ///////////    
	 
		if(cont_muerte == MUERTO)  //Va igual!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! es solo para probar el envio de datos.
			RUTINA_MUERTE();
	 
 ////////// TERCERA PARTE: DORMIR DURANTE 15 MINUTOS //////////
		
		 CPU_PrenderRTC(RTC_1SEG,MINUTO);//(RTC_1SEG,1)
		 ban_vueltacomp=CORRIENDO;
		 while(ban_vueltacomp!=FIN){  //me duermo durante 1 minuto x veces
			 asm{STOP
			 }
		 } 
    __RESET_WATCHDOG();	/* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
