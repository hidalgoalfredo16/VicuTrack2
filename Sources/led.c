/*
 * led.c
 *
 *  Created on: Sep 24, 2014
 *      Author: usuario
 */
#include "led.h"
#include "CPU.h"
#include "error.h"

error Init_LED(void){
	PTCDD_PTCDD5 =1;
	PTCDD_PTCDD4 =1;
	return _ERR_OK;
}

void LED_PrenderR(){
    PTCD_PTCD5 = 1;
}

void LED_ApagarR(){
    PTCD_PTCD5 = 0;
}

void LED_PrenderV(){
    PTCD_PTCD4 = 1;
}

void LED_ApagarV(){
    PTCD_PTCD4 = 0;
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

