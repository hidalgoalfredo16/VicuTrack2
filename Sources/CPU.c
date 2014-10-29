#include "error.h"
#include "CPU.h"
#include "led.h"

//**********************Variables Globales*****************************
long dir_base_lat;
long dir_base_lon;
//byte dir_escritura[4];
//byte dir_lectura[4];
byte id;
byte ban_turno;     //=0apagada =1corriendo =2finalizado
byte ban_bufferTx;
byte ban_SDvacia;
byte ban_ACK;
//byte ban_fix;
byte index_Rx;
byte ban_vueltacomp;
byte ban_muerte;
byte ban_finao;
byte ban_esperafix;

//**********************Variables Externas******************************
extern byte Buffer_Rx[tam_paquete];


//****************************Funciones*****************************************
error Init_CPU(void){
		SOPT1 = 0x23;      // SOPT1: COPE=0,COPT=1,STOPE=1,??=0,??=0,??=0,BKGDPE=1,RSTPE=1                
	    SOPT2 = 0x10;      // SOPT2: COPCLKS=0,??=0,??=0,SPIFE=1,SPIPS=0,ACIC2=0,IICPS=0,ACIC1=0                
	    SPMSC1 = 0x1C;     // SPMSC1: LVDF=0,LVDACK=0,LVDIE=0,LVDRE=1,LVDSE=1,LVDE=1,??=0,BGBE=0               
	    SPMSC2 = 0x02;     // SPMSC2: LPR=0,LPRS=0,LPWUI=0,??=0,PPDF=0,PPDACK=0,PPDE=1,PPDC=0
	    SPMSC3 = 0x00;     // SPMSC3: LVDV=0,LVWV=0,LVWIE=0 
	    
	    //  System clock initialization
	    if (*(unsigned char*)0xFFAF != 0xFF) { // Test if the device trim value is stored on the specified address
	        ICSTRM = *(unsigned char*)0xFFAF;  // Initialize ICSTRM register from a non volatile memory
	        ICSSC = (unsigned char)((*(unsigned char*)0xFFAE) & (unsigned char)0x01); // Initialize ICSSC register from a non volatile memory
	    }
	    
	    // ICSC1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=1
	    ICSC1 = 0x07;                        // Initialization of the ICS control register 1 
	    
	    // ICSC2: BDIV=0,RANGE=0,HGO=0,LP=0,EREFS=0,ERCLKEN=0,EREFSTEN=0
	    ICSC2 = 0x00;                        // Initialization of the ICS control register 2
	    while(!ICSSC_IREFST) {               // Wait until the source of reference clock is internal clock
	    }
	    
	    // ICSSC: DRST_DRS=1,DMX32=0
	    ICSSC = 0x40;   // Initialization of the ICS status and control 
	    while((ICSSC & 0xC0) != 0x40) {      // Wait until the FLL switches to High range DCO mode 
	    }
	    
	    //Configuracion del RTC
	    //RTIF=1, RTCLKS=11, RTIE=1, RTCPS=0110
	    //RTCSC =0xF6;
	    //RTCMOD=0x01;
	    id=0x34;
	    ban_vueltacomp = APAGADO;     //=0apagada =1corriendo =2finalizado
	    ban_turno=APAGADO;          //=0apagada =1corriendo =2finalizado
	    ban_bufferTx=1;             //1=no tiene datos
	    ban_SDvacia=1;              //1=no tiene datos
	    ban_ACK=APAGADO;
	    ban_muerte=APAGADO;
	    ban_finao=APAGADO;
	    dir_base_lat=89600000;//89600000;//26481282;//26481282 gabriel//26505903 facultad;
	    dir_base_lon=0;//65122019;//65122019 gabriel//65138195 facultad;
	    index_Rx=0;
	    return _ERR_OK;
}

void Cpu_Delay100US(word us100){
    asm{
        loop:
          // 100 us delay block begin
          //
          // Delay
          //   - requested                  : 100 us @ 16MHz,
          //   - possible                   : 1600 c, 100000 ns
          //   - without removable overhead : 1592 c, 99500 ns
          pshh                               // (2 c: 125 ns) backup H
          pshx                               // (2 c: 125 ns) backup X
          ldhx #$00C5                        // (3 c: 187.5 ns) number of iterations
          label0:
          aix #-1                            // (2 c: 125 ns) decrement H:X
          cphx #0                            // (3 c: 187.5 ns) compare it to zero
          bne label0                         // (3 c: 187.5 ns) repeat 197x
          pulx                               // (3 c: 187.5 ns) restore X
          pulh                               // (3 c: 187.5 ns) restore H
          nop                                // (1 c: 62.5 ns) wait for 1 c
          nop                                // (1 c: 62.5 ns) wait for 1 c
          nop                                // (1 c: 62.5 ns) wait for 1 c
          // 100 us delay block end
          aix #-1                            // us100 parameter is passed via H:X registers
          cphx #0
          bne loop                           // next loop
          rts
    }
}

void CPU_PrenderRTC(byte cps,int modulo){
	// Configuracion del RTC
	// RTIF=1, RTCLKS=00, RTIE=1, RTCPS=pasado en variable
	//RTCSC=0x80;
	RTCSC = 0x90 | (cps & 0x0F);  //xq pasa esto????
	RTCMOD=(byte)modulo;
}

void CPU_ApagarRTC(){
	RTCSC=0x80;
}

byte CPU_DameTension(){
	int c=0;
	byte temp,i;
	PTAD_PTAD7 = 1;  
	for(i=0;i<3;i++){
		ADCSC1=0x08;	// Trata de medir tension en PTA6 AD8
		while((ADCSC1&0x80)==0 && c++<5000){ //Esperan que COCO=1
		}
		temp=ADCRL;
	}
	PTAD_PTAD7 = 0;
	return temp;
}

byte CPU_DameTemperatura(){
	int c=0;
	byte temp,i;

	for(i=0;i<3;i++){
		ADCSC1=0x1A;	//Configura para leer el sensor de temperatura AD26
		while((ADCSC1&0x80)==0 && (c++<5000)){ //Esperan que COCO=1
		}
		temp=ADCRL;		//Se almacena el dato de la temperatura
	}
	return temp;
}

//! Interrrupcion por RTC
interrupt VectorNumber_Vrtc void ISR_RTC(void){
	static int cont_vc,cont_dia;
	RTCSC = RTCSC | 0x80;   //limpiamos la bandera de interrupcion.
	ban_esperafix=1;
	if(ban_muerte==CORRIENDO)//esto se hace cuando estamos escuchando una SM.
		ban_muerte=FIN;
	if(ban_finao==CORRIENDO){//esto se hace cuando ya esta muerto
		cont_dia++;
		if(cont_dia>diferencia){
			ban_finao=FIN;
			cont_dia=0;
		}
	}
	if(ban_vueltacomp==CORRIENDO){ // esto se hace cuando dormimos
		cont_vc++;
		if(cont_vc >= vueltasRTC){
			 ban_vueltacomp=FIN;
			 cont_vc=0;
		}
		LED_BrillarR(1,500);
	}
	index_Rx=0;	//Preguntar
	if(ban_turno==CORRIENDO) // esto se hace cuando esperamos turno.
		 ban_turno=FIN;
	if(ban_ACK==APAGADO) // esto hacemos cuando esperamos ack.
		ban_ACK=FIN;
}

//! Interrupcion por Recepcion de Datos del Transceiver
interrupt VectorNumber_Vsci2rx void ISR_Transceiver(void){
	(void)SCI2S1;   //limpiamos la bandera de interrupcion.
	if(index_Rx<tam_paquete-1){
		Buffer_Rx[index_Rx]=SCI2D;
		index_Rx++;
	}else{
		Buffer_Rx[index_Rx]=SCI2D;
		index_Rx=0;
		ban_turno=HAYPAQUETE;
		ban_ACK=HAYPAQUETE;
		ban_muerte=HAYPAQUETE;
	}
}

//! Interrupcion por Overrun
interrupt VectorNumber_Vsci2err void ISR_OVR(){
	(void)SCI2S1;
	(void)SCI2D;
}

