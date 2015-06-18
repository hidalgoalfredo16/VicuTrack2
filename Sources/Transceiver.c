/*!
 @file transceiver.c
 @brief Define variables e implementa funciones necesarias para el manejo del transceiver
 */

#include"Transceiver.h"
#include"error.h"
#include "CPU.h"
#include "led.h"
#include "sd.h"
#include "gps.h"

//int BORRAME=0;
//int BORRAME_ERRACK=0;

extern byte Buffer_Rx[tam_paquete];
extern byte ban_ACK;
extern byte ban_bufferTx; 
extern byte ban_turno;
extern byte ban_muerte;
extern dato Buffer_GPS[cantidad_datos][tam_dato];
extern byte dir_escritura[4];
extern byte dir_lectura[4];
extern byte id;

error Init_Trans(void){
    byte i, CadenaInit[19]="WR 455000 3 9 3 0\r\n"; //configuracion del transceiver f=455MHz, Data Rate 9600bps, output power 100mw
    //byte CadenaInit[4]="RD\r\n";					 //UART data rate 9600bps, no checkout
    SCI2BDH = 0x00;
    SCI2BDL = 0x6D;							//El baudrate era 0x70
    // SCIC1: LOOPS=0,SCISWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0
    SCI2C1 = 0x00;                // Configure the SCI
    // SCIC3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=1,NEIE=0,FEIE=0,PEIE=0
    SCI2C3 = 0x08;                // Disable error interrupts
    // SCIC2: TIE=0,TCIE=0,RIE=1,ILIE=0,TE=1,RE=1,RWU=0,SBK=0
    SCI2C2 = 0x2C;
    // PIN set como salida
    TransceiverSet_Direccion = 1;
    // PIN enable comosalida
    TransceiverEnable_Direccion = 1;
    
    (void) Transceiver_Prender();
    (void) Transceiver_SetBajo();
    
    for(i=0; i<19; i++){
    	(void) Transceiver_EnviarByte(CadenaInit[i]);
    }
    
    i=0;
    while(i<19){
		if(SCI2S1_RDRF != 0x00){           //si hay algo en el buffer de recepcion lo guardo
			(void)SCI2S1;
			CadenaInit[i]=SCI2D;
			i++;
		}
    }
    
    if(CadenaInit[0]=='P' && CadenaInit[1]=='A' && CadenaInit[2]=='R' && CadenaInit[3]=='A' 
    && CadenaInit[5]=='4' && CadenaInit[6]=='5' && CadenaInit[7]=='5' && CadenaInit[8]=='0' && CadenaInit[9]=='0' && CadenaInit[10]=='0' 
    && CadenaInit[12]=='3' && CadenaInit[14]=='9' && CadenaInit[16]=='3' && CadenaInit[18]=='0')
    {
    	return _ERR_OK;
    }
    else
    	return _ERR_DISTINTO;
}

error Transceiver_SetAlto(){
    TransceiverSet = 1;
    return _ERR_OK;
}

error Transceiver_SetBajo(){
    TransceiverSet = 0;
    return _ERR_OK;
}

error Transceiver_Prender(){
    TransceiverEnable = 1; //Enable 1
    return Transceiver_SetAlto();
}

error Transceiver_Apagar(){
    TransceiverEnable = 0; //Enable 0
    return Transceiver_SetBajo();
}

error Transceiver_EnviarByte(byte d){
    (void)SCI2S1;             //NO ENTIENDO
    SCI2D=d;
    while(SCI2S1_TDRE == 0){
    }
    while(SCI2S1_TC == 0) {            //Esperamos que se transmita el caracter
    }
    return _ERR_OK;
}

error Transceiver_RecibirByte(byte *Rxdat){
    if(SCI2S1_RDRF != 0x00){           //si hay algo en el buffer de recepcion lo guardo
        (void)SCI2S1;
        *Rxdat=SCI2D;
        return _ERR_OK;
    }else
        return _ERR_RXEMPTY;
}

error Transceiver_Enviar(dato buf[][tam_dato], byte *j,byte *nrosec){
    int  i=0, intentos_ack=0, bandera_ACK=0, bandera_incompleto=0;
    while(*j<cantidad_datos && bandera_incompleto==0){
        intentos_ack=0;
        while(intentos_ack<40){
            (void)Transceiver_EnviarByte(id);
            (void)Transceiver_EnviarByte(flag_dato);
            (void)Transceiver_EnviarByte(*nrosec);
            i=0;
            while(i<tam_dato){
                (void)Transceiver_EnviarByte(buf[*j][i]);
                i++;
            }
            //AHORA TENGO QUE ESPERAR EL ACK 10 INTENTOS
            CPU_PrenderRTC(RTC_01S,25);
            ban_ACK=APAGADO;
            while(ban_ACK==APAGADO){
            }
            CPU_ApagarRTC();
            // si es V sale por RTC
            if(ban_ACK==HAYPAQUETE &&
            	Transceiver_AnalizarACK(Buffer_Rx,nrosec)==_ERR_OK){
                	(*j)++;
                	intentos_ack=40;
                    if(*j==cantidad_datos){
                    	ban_bufferTx=1; //buffer de transmision vacio
                    	LED_PrenderR();
		                //LED_PrenderV();
		                Cpu_Delay100US(200);
		                LED_ApagarR();
		                //LED_ApagarV();
						if(SD_Condatos()==_ERR_OK){
							(void)SD_Leer(dir_lectura, buf);
							*j=0;
							ban_bufferTx=0; //buffer de transmision con datos
						} else {
							//En caso de que no haya mas datos para transmitir se envia una trama con flag ultimo
							if((*nrosec)==255)
								(*nrosec)=1;    //lo pongo a 1 porque si fuera 0 puedo tener problemas
							(*nrosec)++;        //ya que en la base comparo con el anterior
							Cpu_Delay100US(50);
							(void)Transceiver_EnviarByte(id);
							(void)Transceiver_EnviarByte(flag_ultimo);
							(void)Transceiver_EnviarByte(*nrosec);
							for(i=0;i<tam_dato;i++)
								(void)Transceiver_EnviarByte(0x55); //payload
							CPU_PrenderRTC(RTC_01S,25);
							            ban_ACK=APAGADO;
							while(ban_ACK==APAGADO){
							}
							CPU_ApagarRTC();
						}

                    }
	                if((*nrosec)==255)
	                	(*nrosec)=1;    //lo pongo a 1 porque si fuera 0 puedo tener problemas
	                (*nrosec)++;        //ya que en la base comparo con el anterior

            }else{
                intentos_ack++;
                //BORRAME++;
                if(intentos_ack==40)
                    bandera_incompleto=1;
            }
        ban_ACK=APAGADO;
        ban_turno=APAGADO;
        }// cierra while intentos ack
    //ban_ACK=APAGADO;//no va es de prueba
    }//cierra while j < cant datos
    return _ERR_ACK;
}

error Transceiver_Analizar(byte buf[]){
	if(buf[0]==id)
    	if((buf[1] & flag_turno) != 0){
        	LED_BrillarV(1,200);
        	LED_BrillarR(1,200);
            return _ERR_OK;
        }
    return _ERR_TURNO;
}

error Transceiver_AnalizarACK(byte buf[],byte *nrosec){
	if(buf[0]==id)
    	if((buf[1] & flag_ack) != 0)
        	if(buf[2]==*nrosec)
            	return _ERR_OK;

    //BORRAME_ERRACK++;
    return _ERR_ACK;
}

error Transceiver_ControlarDato(){
    byte e=3;
    byte suma=0;
    if(Buffer_Rx[POS_LAT] == 'S' || Buffer_Rx[POS_LAT] == 'N'){
        while(e<tam_paquete-1){
            if(e!=POS_LAT && e!=POS_LONG)
                suma=suma+(Buffer_Rx[e]-0x30);
            e++;
        }
        if(Buffer_Rx[tam_paquete-1]==suma)
            return _ERR_OK;
    }
    return _ERR_DATO;
}

error Transceiver_RecibirSM(){
	int c=0;
	(void)Transceiver_Prender();
	while(c<600){
		(void)CPU_PrenderRTC(RTC_1SEG,1);
	 	ban_muerte=CORRIENDO;
	 	while(ban_muerte==CORRIENDO){
	 	}
	 	if(ban_muerte==HAYPAQUETE){
	    	(void)CPU_ApagarRTC(); //yo lo pondria dentro del siguiente if
	    	if(Transceiver_ControlarDato()==_ERR_OK){
	    		if(GPS_EscribirBuffer(Buffer_Rx,Buffer_GPS)== _ERR_OVF){
					(void)SD_Escribir(dir_escritura,Buffer_GPS);// escribimos la SD con el buffer lleno
					//(void)SD_CalculaDireccion(dir_escritura);// actualizamos la dir escritura de la SD
					(void)SD_CalculaDireccion(dir_escritura,Buffer_GPS);
					(void)GPS_EscribirBuffer(Buffer_Rx,Buffer_GPS);
	    	  	}
	    		(void) Transceiver_Apagar();
	    		return _ERR_OK;
	    	}
	 	}
	 	c++;
	}
	(void)Transceiver_Apagar();
	(void)CPU_ApagarRTC();
	return _ERR_OK;
}
