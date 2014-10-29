#include "spi.h"

/************************************************/
void SPI_Init(void) {
  
  //SOPT2 = SOPT2_SPI1PS_MASK;  // Drive PTE as SPI port
  
  SPI_SS = 1;
  _SPI_SS= 1;
  
  SPIBR = 0x06; // 375 Khz    		
  SPIC2 = 0x00;  
  SPIC1 = SPIC1_SPE_MASK | SPIC1_MSTR_MASK;
}
/************************************************/
void SPI_SendByte(UINT8 u8Data) {
	while(!SPIS_SPTEF) ;
    (void)SPIS;
	SPID = u8Data;
}
/************************************************/
UINT8 SPI_ReceiveByte(void) {
  (void)SPID;
  /*
	while(!SPI1S_SPTEF) ;
  (void)SPI1S;
  */
  SPI_SendByte(0xFF);
  //SPI1DL = 0xFF;
  while (!SPIS_SPRF) ;	
  return(SPID);
} 
/************************************************/
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

/************************************************/
void SPI_HighRate(void) {
  SPIC1 = 0x00;
  SPIBR = 0x12; // 4 Mhz, lo mas proximo posible a 4.5 Mhz     		
  SPIC1 = SPIC1_SPE_MASK | SPIC1_MSTR_MASK;
}
