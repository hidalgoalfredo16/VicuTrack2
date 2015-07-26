/*!
 @file sd.c
 @brief Define variables e implementa funciones necesarias para el manejo de la sd
 */

#include "sd.h"
#include "storage.h"

//! Estructura que almacena informacion del tipo de tarjeta sd que se esta utilizando
struct {
  UINT8 NUM_BLOCKS;
  UINT8 SDHC;
  UINT8 VERSION2;
} gSDCard;

extern byte dir_lectura[4];
extern byte ban_bufferTx;
extern byte ban_SDvacia;
extern byte dir_escritura[4];
extern byte dir_lectura[4];
extern dato Buffer_Envio[cantidad_datos][tam_dato];
extern UINT16 u16FAT_Data_BASE;
extern UINT32 direccion;

//! Variable que almacena la latitud de la posicion de la base leida desde la tarjeta SD
byte dir_base_lat[4];
//! Variable que almacena la longitud de la posicion de la base leida desde la tarjeta SD
byte dir_base_lon[4];
//! Variable que define el id del movil leido de la tarjeta SD
byte id;

UINT8 SD_SendCommand(UINT8 u8SDCommand, UINT32 u32Param, UINT8 pu32Response[], UINT8 u8ResponseLength) {
  UINT8 u8R1;
  UINT8 u8Counter;
  UINT8 buffer[4];
  
  buffer[0] = (UINT8)((u32Param & 0xFF000000) >> 24);
  buffer[1] = (UINT8)((u32Param & 0x00FF0000) >> 16);
  buffer[2] = (UINT8)((u32Param & 0x0000FF00) >>  8);
  buffer[3] = (UINT8)((u32Param & 0x000000FF) >>  0);
  
  /* Enable CS */  
  //SPI_SS = ENABLE;
 
  /* Send Start byte */
  SPI_SendByte((UINT8)(u8SDCommand | 0x40));

  /* Send Argument */
  for(u8Counter = 0; u8Counter < 4; u8Counter++) {
     SPI_SendByte(buffer[u8Counter]);
  }

  /* Send CRC */
  if((u8SDCommand & 0x02F) == SD_CMD8_SEND_IF_COND) {
    SPI_SendByte(0x87);
  } else {
    SPI_SendByte(0x95);
  }
  
  /*
  } else if ((u8SDCommand & 0x02F) == SD_CMD1) {
    SPI_SendByte(0x95);
  } else {
    SPI_SendByte(0x01);
  }
  */
  
  /* Response RHandler (R1) */
  u8Counter = SD_WAIT_CYCLES;
  do {
      u8R1 = SPI_ReceiveByte();							//Recibe Respuesta de SD (R1=1byte)
      u8Counter--;
  } while(((u8R1 & 0x80) != 0) && (u8Counter > 0));
 
  if(u8Counter == 0) {
    //SPI_SS = DISABLE;
    return (SD_FAIL_TIMEOUT);
  }
  
   
  for (u8Counter = 0; u8Counter < u8ResponseLength; u8Counter++)  {
    pu32Response[u8Counter] = SPI_ReceiveByte();
  }
  
  /* Disable CS */  
  //SPI_SS = DISABLE;  
  
  return (u8R1);
}


UINT8 SD_ReadData(UINT8 pu8DataPointer[], UINT32 u32DataLength) {
  UINT8  CRC[2];
  UINT32 u32Counter;

  /* Enable CS */  
  //SPI_SS = ENABLE;

  for(u32Counter = 0; u32Counter < u32DataLength; u32Counter++) {
    pu8DataPointer[u32Counter] = SPI_ReceiveByte();
  }

  for(u32Counter = 0; u32Counter < 2; u32Counter++) {
    CRC[u32Counter] = SPI_ReceiveByte();
  }

  /* Disable CS */  
  //SPI_SS = DISABLE;

  (void) SPI_ReceiveByte(); // Unknown issue. SPI dummy cicle required by some cards

	return (SD_OK);  
}


UINT8 SD_WriteData(UINT8 pu8DataPointer[], UINT32 u32DataLength) {
  UINT8 u8R1;
  UINT8 u8Counter;
  UINT32 u32Counter;

	//SPI_SS = ENABLE;
	
	SPI_SendByte(0xFE);
    
	for(u32Counter = 0; u32Counter < u32DataLength; u32Counter++)
		SPI_SendByte(pu8DataPointer[u32Counter]);

	SPI_SendByte(0xFF);    // checksum Bytes not needed
	SPI_SendByte(0xFF);
	
  /* Response RHandler (R1) */
  u8Counter = SD_WAIT_CYCLES;
  do {
		u8R1 = SPI_ReceiveByte();
    u8Counter--;
  } while(((u8R1 & 0x80) != 0) && (u8Counter > 0)); // #001 wait for SD response <> 0xFF
   	 
	//SPI_SS = DISABLE;
 
  if(u8Counter == 0) return (SD_FAIL_TIMEOUT);
  
	if((u8R1 & 0x1F) != 0x05) return (SD_FAIL_WRITE); // #001 checks if response = 0101 = data accepted
	
	while(SPI_ReceiveByte()==0x00) ;  // Dummy SPI cycle
	
  return(SD_OK);	
}


UINT8 SD_Init(void) {
  //volatile UINT8 DelayFlag; 
  UINT16 u16Counter;
  volatile UINT8  u8R1;
  UINT8  CSD[16];
  UINT32 u32Ifc;
  UINT32 u32Ocr;
  
  gSDCard.NUM_BLOCKS = 0;
  gSDCard.SDHC = False;
  gSDCard.VERSION2 = False;
  
  /* Initialize SPI Module */
  SPI_Init();
  
	//for (u16Counter = 0; u16Counter < 3; u16Counter++) {
	  
    /* Send 80 dummy clocks without CS */
    SPI_SS = DISABLE;      

    /* Wait until voltage stabilize before start initialization */
    //DelayFlag = 0;
    //tmrStart(250, &DelayFlag);
    //while(DelayFlag == 0) __RESET_WATCHDOG(); 		

    for(u16Counter = 0; u16Counter < 10; u16Counter++) SPI_SendByte(0xFF); 
    
    /* Send CMD0 = go idle */ 
    for(u16Counter = 0; u16Counter < SD_INIT_RETRIES; u16Counter++) {
      SPI_SS = ENABLE;
      u8R1 = SD_SendCommand(SD_CMD0_GO_IDLE_STATE, 0, NULL, 0);
      SPI_SS = DISABLE;
      if(u8R1 ==  SD_R1_IDLE_STATE) break;
    }
    if(u16Counter >= SD_INIT_RETRIES) return (SD_FAIL_INIT);

	//}

  /* Send CMD8 = test version */
  SPI_SS = ENABLE;
  u8R1 = SD_SendCommand(SD_CMD8_SEND_IF_COND, SD_IF_COND_VHS_27_36 | 0xAA,(UINT8*)&u32Ifc, 4);
  SPI_SS = DISABLE;
  if(u8R1 == SD_FAIL_TIMEOUT) return (SD_FAIL_INIT);
  if(0 == (u8R1 & SD_R1_ILLEGAL_CMD)) {
    /* command supported, v2 card */
    gSDCard.VERSION2 = True;
    if(((u32Ifc & 0xFF) != 0xAA) || 
      ((u32Ifc & SD_IF_COND_VHS_MASK) != SD_IF_COND_VHS_27_36))
      return (SD_FAIL_INIT);
  }

  /* Send CMD58 = read OCR ... 3.3V */
  SPI_SS = ENABLE;
  u8R1 = SD_SendCommand(SD_CMD58_READ_OCR, 0, (UINT8*)&u32Ocr, 4); 
  SPI_SS = DISABLE;
  if(u8R1 == SD_FAIL_TIMEOUT) return (SD_FAIL_INIT);  
  if(0 == (u8R1 & SD_R1_ILLEGAL_CMD)) {
    if(u8R1 & SD_R1_ERROR_MASK) return (SD_FAIL_INIT);
    if(SD_OCR_VOLTAGE_3_3V != (u32Ocr & SD_OCR_VOLTAGE_3_3V)) return (SD_FAIL_INIT);
  }
  
  /* Send CMD55 + ACMD41 = initialize */
  for(u16Counter = 0; u16Counter < SD_INIT_RETRIES; u16Counter++) {
      SPI_SS = ENABLE;      
      u8R1 = SD_SendCommand(SD_CMD55_APP_CMD, 0, NULL, 0);
      SPI_SS = DISABLE;
      
      if(u8R1 == SD_FAIL_TIMEOUT) return (SD_FAIL_INIT);  
      
      if(u8R1 & SD_R1_ERROR_MASK) return (SD_FAIL_INIT);

      SPI_SS = ENABLE;
      u8R1 = SD_SendCommand(SD_ACMD41_SEND_OP_COND, gSDCard.VERSION2 ? SD_ACMD41_HCS : 0, NULL, 0);
      SPI_SS = DISABLE;
      
      if(u8R1 == SD_FAIL_TIMEOUT) return (SD_FAIL_INIT);  
      
      if(u8R1 & SD_R1_ERROR_MASK) return (SD_FAIL_INIT);

      if(0 == (u8R1 & SD_R1_IDLE_STATE)) break;
  }
  
  if(u16Counter >= SD_INIT_RETRIES) return (SD_FAIL_INIT);
  
  /* Version 2 or later card */ 
  if (gSDCard.VERSION2) { 
        /* Send CMD58 = read OCR to check CCS */
        SPI_SS = ENABLE;
        u8R1 = SD_SendCommand(SD_CMD58_READ_OCR, 0, (UINT8*)&u32Ocr, 4);
        SPI_SS = DISABLE;
        
        if(u8R1 == SD_FAIL_TIMEOUT) return (SD_FAIL_INIT);          
        
        if(u8R1 & SD_R1_ERROR_MASK) return (SD_FAIL_INIT);

        if(0 == (u32Ocr & SD_OCR_POWER_UP_STATUS)) return (SD_FAIL_INIT);

        if(u32Ocr & SD_OCR_CCS)  gSDCard.SDHC = True;
  }

  /* Send CMD9 = get CSD */
  
  SPI_SS = ENABLE;
  u8R1 = SD_SendCommand(SD_CMD9_SEND_CSD, 0, NULL, 0);
  if((u8R1 == SD_FAIL_TIMEOUT) || (u8R1 & SD_R1_ERROR_MASK)) {
    SPI_SS = DISABLE;
    return (SD_FAIL_INIT);
  } else {
    if(SD_ReadData(CSD, 16) != SD_OK) return (SD_FAIL_INIT);
    SPI_SS = DISABLE;
  }
  
  //gSDCard.NUM_BLOCKS = _io_sdcard_csd_capacity(csd); 
  
  /* Send CMD16 = set block length to 512 */
  SPI_SS = ENABLE;
  u8R1 = SD_SendCommand(SD_CMD16_SET_BLOCKLEN, 512, NULL, 0);
  SPI_SS = DISABLE;
  
  if(u8R1 == SD_FAIL_TIMEOUT) return (SD_FAIL_INIT);
  
  if(u8R1 & SD_R1_ERROR_MASK) return (SD_FAIL_INIT);
  
  //SPI_HighRate();	//Cambio el baudrate
  FAT_Read_Master_Block((UINT8 *)Buffer_Envio);
  (void)SD_LeerDireccion();
  return (SD_OK);
}

UINT8 SD_ReadSector(UINT32 u32SD_Block,UINT8 pu8DataPointer[]) {
  volatile UINT8 u8Temp = 0;

  __RESET_WATCHDOG(); /* feeds the dog */
	
  if (!gSDCard.SDHC) u32SD_Block <<= SD_BLOCK_SHIFT;
     
  SPI_SS = ENABLE;
  //Utilizo u8Temp para capturar respuesta de la tarjeta SD (fines depurativos)
  u8Temp = SD_SendCommand(SD_CMD17_READ_BLOCK, u32SD_Block, NULL, 0); 
  if((u8Temp & SD_R1_ERROR_MASK) != SD_OK) {
    SPI_SS = DISABLE;    
    return (SD_FAIL_READ);
  }
  
	while(u8Temp != 0xFE) u8Temp = SPI_ReceiveByte(); 
	//La SD responde 0xFF...0xFF mientras accede al sector y 0xFE cuando esta lista. 
	//Los datos que envía a continuación corresponden al sector solicitado. 
	
  if (SD_ReadData(pu8DataPointer,SD_BLOCK_SIZE) != SD_OK) return (SD_FAIL_READ);
  
  SPI_SS = DISABLE;
  
  return (SD_OK);
}

UINT8 SD_WriteSector(UINT32 u32SD_Block, UINT8 * pu8DataPointer) {
    UINT16 u16Counter;
    UINT8  SD_response;
    
    __RESET_WATCHDOG(); /* feeds the dog */
  
    SPI_SS = ENABLE;
    
    if(!gSDCard.SDHC) u32SD_Block <<= SD_BLOCK_SHIFT;
    
    if(SD_SendCommand(SD_CMD24_WRITE_BLOCK, u32SD_Block, NULL, 0) != SD_OK) {
      SPI_SS = DISABLE;
      return (SD_FAIL_WRITE);      
    }
    
    SPI_SendByte(0xFE);    
		
     for(u16Counter = 0; u16Counter < SD_BLOCK_SIZE; u16Counter++)
			SPI_SendByte(*pu8DataPointer++);

    SPI_SendByte(0xFF);    // checksum Bytes not needed
    SPI_SendByte(0xFF);
    
	SD_response=0xFF; 
    while (SD_response == 0xFF) SD_response = SPI_ReceiveByte(); 
    if((SD_response & 0x0F) != 0x05) {
        SPI_SS=DISABLE;
        return (SD_FAIL_WRITE);    
    }
		
	while(SPI_ReceiveByte()==0x00) ;  // Dummy SPI cycle
    (void) SPI_ReceiveByte();
    SPI_SS = DISABLE;
	
    return (SD_OK);
}

error SD_Leer(byte *direccion, dato lectura[][tam_dato]){
    UINT32 u32SD_Block; // Convertimos la direccion en una variable de 32 bits
	
	SPI_Init();
	u32SD_Block = direccion[0];
	u32SD_Block <<= 8;
	u32SD_Block |= direccion[1];
	u32SD_Block <<= 8;
	u32SD_Block |= direccion[2];
	u32SD_Block <<= 8;
	u32SD_Block |= direccion[3];
    
    (void) SD_CalculaDireccion(dir_lectura, lectura);
    (void) SD_ReadSector(u32SD_Block,(UINT8 *) lectura);
    (void)Cpu_Delay100US(100); 
    //ban_bufferTx=0;
        
    return _ERR_OK;

}

error SD_Escribir(byte *direccion, dato buf[][tam_dato]){
	UINT32 u32SD_Block; // Convertimos la direccion en una variable de 32 bits
    byte tem;
    
    SPI_Init();
    u32SD_Block = direccion[0];
    u32SD_Block <<= 8;
    u32SD_Block |= direccion[1];
    u32SD_Block <<= 8;
    u32SD_Block |= direccion[2];
    u32SD_Block <<= 8;
    u32SD_Block |= direccion[3];
    
    tem = SD_WriteSector(u32SD_Block, (UINT8 *) buf);
    
    (void)Cpu_Delay100US(100);
    ban_SDvacia=0;
    return _ERR_OK;
}

error SD_CalculaDireccion(byte * dir, dato buf[][tam_dato]){
	long Auxiliar;
	byte i;
	
    Auxiliar = dir[0];
    Auxiliar <<= 8;
    Auxiliar |= dir[1];
    Auxiliar <<= 8;
    Auxiliar |= dir[2];
    Auxiliar <<= 8;
    Auxiliar |= dir[3];
    
	Auxiliar += 1;

    dir[0]=(byte)(Auxiliar>>24);
    dir[1]=(byte)(Auxiliar>>16);
    dir[2]=(byte)(Auxiliar>>8);
    dir[3]=(byte)(Auxiliar);
    
    SD_Assert();
    
    for(i=0;i<4;i++)
    	buf[0][i] = dir_lectura[i];
    for(;i<8;i++)
    	buf[0][i] = dir_escritura[i-4];
    for(;i<12;i++)
    	buf[0][i] = dir_base_lat[i-8];
    for(;i<16;i++)
        buf[0][i] = dir_base_lon[i-12];
    buf[0][i] = id;
    
    (void) SD_WriteSector((UINT32) u16FAT_Data_BASE, (UINT8 *) buf); //2600 sector del archivo binario, hacer vble global
    return _ERR_OK;            
}

error SD_Condatos(){

if(dir_escritura[0]==dir_lectura[0]
   && dir_escritura[1]==dir_lectura[1]
   && dir_escritura[2]==dir_lectura[2]
   && dir_escritura[3]==dir_lectura[3]){
                  ban_SDvacia=1;
                  return _ERR_DIR;
   }

return _ERR_OK;
}

error SD_LeerDireccion(){
	UINT32 u32SD_Arch;
    int h=0, i=0;
    (void)SD_Assert();
    
    //u32SD_Block=DIRECCION_BIN; //Cargar la direccion del sector fisico donde se encuentran las direcciones de lectura y escritura 
    //(UINT32) u16FAT_Data_BASE
    (void)SD_ReadSector((UINT32) u16FAT_Data_BASE, (UINT8 *) Buffer_Envio); //Lee el sector que contiene las direcciones de lectura y escritura
    
    for(i=0;i<4;i++)
    	dir_lectura[i] = Buffer_Envio[0][i]; 
    												//Se guardan las direcciones en las variables
    for( ;i<8;i++)
    	dir_escritura[i-4] = Buffer_Envio[0][i];
    
    for( ;i<12;i++)
    	dir_base_lat[i-8] = Buffer_Envio[0][i];
    
    for( ;i<16;i++)
        dir_base_lon[i-12] = Buffer_Envio[0][i];
    
    id = Buffer_Envio[0][i];
    
    if(dir_lectura[0]==0&&dir_lectura[1]==0&&dir_lectura[2]==0&&dir_lectura[3]==0&&
    		dir_escritura[0]==0&&dir_escritura[1]==0&&dir_escritura[2]==0&&dir_escritura[3]==0){
    	u32SD_Arch=(UINT32) u16FAT_Data_BASE + 1; //Establece el segundo sector del archivo como el inicio de lectura y escritura
    	dir_lectura[0]=(byte)(u32SD_Arch>>24);
		dir_lectura[1]=(byte)(u32SD_Arch>>16);
		dir_lectura[2]=(byte)(u32SD_Arch>>8);
		dir_lectura[3]=(byte)(u32SD_Arch);
		dir_escritura[0]=(byte)(u32SD_Arch>>24);
		dir_escritura[1]=(byte)(u32SD_Arch>>16);
		dir_escritura[2]=(byte)(u32SD_Arch>>8);
		dir_escritura[3]=(byte)(u32SD_Arch);
		SD_Assert();
		for(i=0;i<4;i++)
			Buffer_Envio[0][i] = dir_lectura[i];
		for(;i<8;i++)
			Buffer_Envio[0][i] = dir_escritura[i-4];
		(void) SD_WriteSector((UINT32) u16FAT_Data_BASE, (UINT8 *) Buffer_Envio); //2600 sector del archivo binario, hacer vble global
    }
        
    (void)SD_DesAssert();
    (void)Cpu_Delay100US(100);
    //ban_bufferTx=0;
    
    return _ERR_OK;
}
