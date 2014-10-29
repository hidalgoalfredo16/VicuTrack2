/*
 * gps.c
 *
 *  Created on: Sep 24, 2014
 *      Author: usuario
 */
#include"gps.h"
#include"error.h"
#include "led.h"
#include "CPU.h"

//***********************Variables Globales**********************
byte vueltasRTC;
byte ban_fix;

//***********************Variables Externas**********************
extern dato dat[tam_dato];
extern long dir_base_lat;
extern long dir_base_lon;
extern byte ban_horasm;

error Init_GPS(void){
	return 1;
}

error GPS_Prender(void){
    PTCD_PTCD1 = 0;
    return 1;
}

error GPS_Apagar(void){
 PTCD_PTCD1 = 1;   
    return 1;
}

error GPS_Recibir(trama_crudo *tc){
    unsigned int i=0;
    unsigned long c=0;
    while(i<tam_trama_crudo){
        while(SCI1S1_RDRF !=0){  //recojo datos del puerto hasta llenar el vector 
            tc[i]=SCI1D;
            i++;
            c=0;        
        }
        c++;
        if(c==200000)
          return _ERR_GPS;
    }
    return _ERR_OK;
}

error GPS_Analizar(trama_reducida* tr,trama_crudo* tc){
    unsigned int i=0,j=0,cont_comas=0,b=0;
    while(i < _POSICION_GPRMC_ACEPTABLE){ // Para NO encontrar la trama $GPRMC cortada
        if( tc[i]== _PESOS && 
          tc[i+1]== _G && 
          tc[i+2]== _P &&    
          tc[i+3]== _R && // Buscamos el comienzo de la trama que nos interesa $GPRMC
          tc[i+4]== _M && 
          tc[i+5]== _C){
           /* if(tc[i+_POS_FIXED] != _FIXED){
            	ban_fix=0;
                return  _ERR_TRAMA_NO_FIXED;
            }*/
            ban_fix=1;
            while(j < tam_trama_reducida){
                tr[j]=tc[i];
                if(tc[i] == _COMA)
                    cont_comas++;
                j++;
                i++;
            }
            if(cont_comas == _TRAMA_GPRMC_OK)
                return _ERR_OK;
            else
                return _ERR_COMAS;      
        }//termina el IF
    i++;
    }//termina el WHILE
    return _ERR_TRAMA_CRUDO;
}

error GPS_Dato(dato d[],trama_reducida tr[]){
    int i=0,j;
    d[0]=tr[7];
    d[1]=tr[8];
    d[2]=tr[9];
    d[3]=tr[10];
    d[4]=tr[20];
    d[5]=tr[21];
    d[6]=tr[22];
    d[7]=tr[23];
    d[8]=tr[25];
    d[9]=tr[26];
    d[10]=tr[27];
    d[11]=tr[28];
    d[12]=tr[30];
    d[13]=tr[32];
    d[14]=tr[33];
    d[15]=tr[34];
    d[16]=tr[35];
    d[17]=tr[36];
    d[18]=tr[38];
    d[19]=tr[39];
    d[20]=tr[40];
    d[21]=tr[41];
    d[22]=tr[43];
    j=44;           //Porcion variable de la trama
    while (i < 3){
        if(tr[j] == _COMA)
            i++;
        j++;
    }    
    d[23]=tr[j];
    d[24]=tr[j+1];
    d[25]=tr[j+2];
    d[26]=tr[j+3];
    d[27]=tr[j+4];
    d[28]=tr[j+5];
    return _ERR_OK;
}

error GPS_EscribirBuffer(dato dat[], dato buf[][tam_dato]){
    static int i=0;
    int j,e=0;
    byte suma=0;
    if(i >= cantidad_datos){
        i=0;
        return _ERR_OVF;
    }
    for(j=0 ; j < tam_dato-3 ; j++) //termino 4 antes para escribir temp,V y checksum
        buf[i][j]=dat[j];
    
    buf[i][tam_dato-3]=CPU_DameTemperatura();
    buf[i][tam_dato-2]=CPU_DameTension();    
    //Escribir el checksum
    while(e<tam_dato-1){
	    if(e!=12 && e!=22)
	        suma=suma+(buf[i][e]-0x30);
	    e++;
    }
    buf[i][tam_dato-1]=suma;
    //Posicion siguiente del buffer
    i++;
    return _ERR_OK;
}

error GPS_CompararBase(dato dat[]){
	int i;
	long res_lat,res_lon,lat=0,p=1,lon=0;
	for(i=11;i>3;i--) {
		lat=lat+((long)dat[i]-48)*p;
		p=p*10;
	}
	p=1;
	for(i=21;i>12;i--) {
		lon=lon+((long)dat[i]-48)*p;
		p=p*10;
	}
	res_lat=dir_base_lat - lat;
	res_lon=dir_base_lon - lon;
	if(res_lat<0)
		res_lat=res_lat*(-1); 
	if(res_lon<0)
		res_lon=res_lon*(-1); 
	if(res_lat+res_lon<=_RAD_800M)
		return _ERR_OK;
	else
		return _ERR_DISTINTO;
}

error GPS_CompararDato(dato dat[],long *ult_lat,long *ult_lon){
	int i;
	long res_lat,res_lon,lat=0,lon=0,p=1;
	//Guardamos latitud en un long
	//Tomamos desde la posicion anterior al N o S hasta antes de la hora 
	for(i=11;i>3;i--) {
		lat=lat+(((long)dat[i]-48)*p);
		p=p*10;
	}
	//Guardamos longitud en un long
	//Tomamos desde la posicion anterior al E o W hasta antes de la hora
	p=1;
	for(i=21;i>12;i--) {
		lon=lon+(((long)dat[i]-48)*p);
		p=p*10;
	}
	//Diferencia con la ultima posicion
	res_lat=*ult_lat - lat;
	res_lon=*ult_lon - lon;
	//Sacamos los valores absolutos
	if(res_lat<0)
		res_lat=res_lat*(-1); 
	if(res_lon<0)
		res_lon=res_lon*(-1); 
	//Si esta dentro del radio que consideramos igual posicion retornamos OK 
	//en caso negativo retornamos error y almacenamos nuevo posicion
	if(res_lat+res_lon <= 400)  //MODIFICAR A <= y a 400
		return _ERR_OK;
	else{
		*ult_lat=lat;
	    *ult_lon=lon;
	    return _ERR_DISTINTO;
	  }
}

error GPS_CopiarDato(dato d[],dato ud[]){
	int i;
    for(i=0;i<tam_dato;i++)
    	ud[i]=d[i];
    return _ERR_OK;
}

error GPS_VerificarHora(){
	int hora_actual;
	hora_actual= (dat[3]-0x30)+(dat[2]-0x30)*10+(dat[1]-0x30)*100+(dat[0]-0x30)*1000;
	if ((hora_actual - _HORA_MUERTE) >= 0 && (hora_actual - _HORA_MUERTE) < 6)
		return _ERR_OK;
	else
	    return _ERR_HORA;
}

#pragma MESSAGE DISABLE C2705
error GPS_SincronizarHM(){
	int hora_actual;
	hora_actual= (dat[3]-0x30)+(dat[2]-0x30)*10+(dat[1]-0x30)*100+(dat[0]-0x30)*1000;
	if ((_HORA_MUERTE - hora_actual) > 0 && (_HORA_MUERTE - hora_actual) < 100){
		vueltasRTC=(_HORA_MUERTE - hora_actual)-40;
		ban_horasm=SI;
	}
	return _ERR_OK;
}
