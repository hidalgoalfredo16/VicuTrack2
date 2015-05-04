/*!
 @file spi.c
 @brief Define variables e implementa funciones necesarias para el manejo del spi
 */

#include "spi.h"

void SPI_Init(void) {
  
  //SOPT2 = SOPT2_SPI1PS_MASK;  // Drive PTE as SPI port
  
  SPI_SS = 1; //deshabilita PTBD5
  _SPI_SS= 1; 
  
  SPIBR = 0x06; // 375 Khz(esto es con 0x06 que venia en el de Esteban)  Setea el prescaler y el baud rate		
  SPIC2 = 0x00; // Registro de control 
  SPIC1 = SPIC1_SPE_MASK | SPIC1_MSTR_MASK; //pone en 1 spe para habilitar y el mstr para q sea master y no esclavo
}

void SPI_SendByte(UINT8 u8Data) {
	while(!SPIS_SPTEF); // Registro de estado indica si el buffer esta vacio o no
    (void)SPIS; //Limpia el registro SPIS
	SPID = u8Data; 
}

UINT8 SPI_ReceiveByte(void) {
  (void)SPID;
  /*
	while(!SPI1S_SPTEF) ;
  (void)SPI1S;
  */
  SPI_SendByte(0xFF);
  //SPI1DL = 0xFF;
  while (!SPIS_SPRF) ;	//Espera hasta que hay un dato disponible
  return(SPID);
} 

/*UINT8 SPI_Receive_byte(void) {
  byte Count;
  byte Result;
  volatile byte Timeout;
  
  (void)SPI1DL;
	for(Count=0; Count < 3; Count++) {
	  SPI1DL = 0xFF;
    //tmrStart(15, &Timeout);
	  do {
	    Result = SPI1S_SPRF;
	  } while ((!Result) && (!Timeout));	
	  if (Result) return(SPI1DL);
	}
  ApplyChanges = True;
	
}*/

void SPI_HighRate(void) {
  SPIC1 = 0x00;
  SPIBR = 0x12; // 4 Mhz, lo mas proximo posible a 4.5 Mhz     		
  SPIC1 = SPIC1_SPE_MASK | SPIC1_MSTR_MASK;
}
