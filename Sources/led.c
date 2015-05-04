/*!
 @file led.c
 @brief Define variables e implementa funciones necesarias para el manejo de los leds
 */

#include "led.h"
#include "CPU.h"
#include "error.h"

error Init_LED(void){
	LedR_Direccion = 1;
	LedV_Direccion = 1;
	return _ERR_OK;
}

void LED_PrenderR(){
	LedR = 1;
}

void LED_ApagarR(){
	LedR = 0;
}

void LED_PrenderV(){
	LedV = 1;
}

void LED_ApagarV(){
	LedV = 0;
}

void LED_BrillarR(int a, word b){
    int i;
    for(i=0 ; i < a ; i++){
        LED_PrenderR();
        Cpu_Delay100US(b);
        LED_ApagarR();
        Cpu_Delay100US(b);
    }
}

void LED_BrillarV(int a, word b){
    int i;
    for(i=0 ; i < a ; i++){
        LED_PrenderV();
        Cpu_Delay100US(b);
        LED_ApagarV();
        Cpu_Delay100US(b);
    }
}

