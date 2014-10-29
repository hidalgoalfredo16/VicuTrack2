#include"Transceiver.h"
#include"error.h"
#include "CPU.h"
#include "led.h"
#include "sd.h"
#include "gps.h"

//**********************Variables Globales*****************************
int BORRAME=0;
int BORRAME_ERRACK=0;

extern byte id;
extern byte Buffer_Rx[tam_paquete];
extern byte ban_ACK;
extern byte ban_bufferTx; 
extern byte ban_turno;
extern byte ban_muerte;
extern dato Buffer_GPS[cantidad_datos][tam_dato];
extern byte dir_escritura[4];

error Init_Trans(void){
    SCI2BDH = 0x00;
    SCI2BDL = 0x70;
    // SCIC1: LOOPS=0,SCISWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0
    SCI2C1 = 0x00;                // Configure the SCI
    // SCIC3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=1,NEIE=0,FEIE=0,PEIE=0
    SCI2C3 = 0x08;                // Disable error interrupts
    // SCIC2: TIE=0,TCIE=0,RIE=1,ILIE=0,TE=1,RE=1,RWU=0,SBK=0
    SCI2C2 = 0x2C;
    // PIN set como salida
    PTADD_PTADD0 = 1;
    // PIN enable comosalida
    PTADD_PTADD1 = 1;
    return _ERR_OK;
}

error Transceiver_SetAlto(){
    PTAD_PTAD0 = 1;
    return _ERR_OK;
}

error Transceiver_SetBajo(){
    PTAD_PTAD0 = 0;
    return _ERR_OK;
}

error Transceiver_Prender(){
    PTAD_PTAD1 = 1; //Enable 1
    return Transceiver_SetAlto();
}

error Transceiver_Apagar(){
    PTAD_PTAD1 = 0; //Enable 0
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
		                LED_PrenderV();
		                Cpu_Delay100US(200);
		                LED_ApagarR();
		                LED_ApagarV();
						if(SD_Condatos()==_ERR_OK){
							(void)SD_Leer(buf);
							*j=0;
							ban_bufferTx=0; //buffer de transmision con datos
						}

                    }
	                if((*nrosec)==255)
	                	(*nrosec)=1;    //lo pongo a 1 porque si fuera 0 puedo tener problemas
	                (*nrosec)++;        //ya que en la base comparo con el anterior

            }else{
                intentos_ack++;
                BORRAME++;
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

error Transceiver_RecibirACK(){
    if(SCI2S1_RDRF != 0x00){       //si hay algo en el buffer de recepcion lo guardo
    	(void)SCI2S1;
        if(SCI2D == ACK ){
            return _ERR_OK;
        }
    }
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

    BORRAME_ERRACK++;
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
	    	(void)CPU_ApagarRTC();
	    	if(Transceiver_ControlarDato()==_ERR_OK){
	    		if(GPS_EscribirBuffer(Buffer_Rx,Buffer_GPS)== _ERR_OVF){
					(void)SD_Init();
					(void)SD_Prender();
					(void)SD_Escribir(dir_escritura,Buffer_GPS);// escribimos la SD con el buffer lleno
					(void)SD_CalculaDireccion(dir_escritura);// actualizamos la dir escritura de la SD
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
