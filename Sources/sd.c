/*
 * sd.c
 *
 *  Created on: Sep 24, 2014
 *      Author: usuario
 */

#include "sd.h"

extern byte dir_lectura[4];
extern byte ban_bufferTx;
extern byte ban_SDvacia;
extern byte dir_escritura[4];
extern byte dir_lectura[4];

struct {
  UINT8 NUM_BLOCKS;
  UINT8 SDHC;
  UINT8 VERSION2;
} gSDCard;

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
  WriteSPIByte((UINT8)(u8SDCommand | 0x40));

  /* Send Argument */
  for(u8Counter = 0; u8Counter < 4; u8Counter++) {
     WriteSPIByte(buffer[u8Counter]);
  }

  /* Send CRC */
  if((u8SDCommand & 0x02F) == SD_CMD8_SEND_IF_COND) {
    WriteSPIByte(0x87);
  } else {
    WriteSPIByte(0x95);
  }
  
  /*
  } else if ((u8SDCommand & 0x02F) == SD_CMD1) {
    WriteSPIByte(0x95);
  } else {
    WriteSPIByte(0x01);
  }
  */
  
  /* Response RHandler (R1) */
  u8Counter = SD_WAIT_CYCLES;
  do {
      u8R1 = ReadSPIByte();
      u8Counter--;
  } while(((u8R1 & 0x80) != 0) && (u8Counter > 0));
 
  if(u8Counter == 0) {
    //SPI_SS = DISABLE;
    return (SD_FAIL_TIMEOUT);
  }
  
   
  for (u8Counter = 0; u8Counter < u8ResponseLength; u8Counter++)  {
    pu32Response[u8Counter] = ReadSPIByte();
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
    pu8DataPointer[u32Counter] = ReadSPIByte();
  }

  for(u32Counter = 0; u32Counter < 2; u32Counter++) {
    CRC[u32Counter] = ReadSPIByte();
  }

  /* Disable CS */  
  //SPI_SS = DISABLE;

  (void) ReadSPIByte(); // Unknown issue. SPI dummy cicle required by some cards

	return (SD_OK);  
}

UINT8 SD_WriteData(UINT8 pu8DataPointer[], UINT32 u32DataLength) {
  UINT8 u8R1;
  UINT8 u8Counter;
  UINT32 u32Counter;

	//SPI_SS = ENABLE;
	
	WriteSPIByte(0xFE);
    
	for(u32Counter = 0; u32Counter < u32DataLength; u32Counter++)
		WriteSPIByte(pu8DataPointer[u32Counter]);

	WriteSPIByte(0xFF);    // checksum Bytes not needed
	WriteSPIByte(0xFF);
	
  /* Response RHandler (R1) */
  u8Counter = SD_WAIT_CYCLES;
  do {
		u8R1 = ReadSPIByte();
    u8Counter--;
  } while(((u8R1 & 0x80) != 0) && (u8Counter > 0)); // #001 wait for SD response <> 0xFF
   	 
	//SPI_SS = DISABLE;
 
  if(u8Counter == 0) return (SD_FAIL_TIMEOUT);
  
	if((u8R1 & 0x1F) != 0x05) return (SD_FAIL_WRITE); // #001 checks if response = 0101 = data accepted
	
	while(ReadSPIByte()==0x00) ;  // Dummy SPI cycle
	
  return(SD_OK);	
}

UINT8 SD_Init(void) {
  //volatile UINT8 DelayFlag; 
  UINT16 u16Counter;
  UINT8  u8R1;
  UINT8  CSD[16];
  UINT32 u32Ifc;
  UINT32 u32Ocr;
  
  gSDCard.NUM_BLOCKS = 0;
  gSDCard.SDHC = False;
  gSDCard.VERSION2 = False;
  
  /* Initialize SPI Module */
  InitSPI();
  
	//for (u16Counter = 0; u16Counter < 3; u16Counter++) {
	  
    /* Send 80 dummy clocks without CS */
    SPI_SS = DISABLE;      

    /* Wait until voltage stabilize before start initialization */
    //DelayFlag = 0;
    //tmrStart(250, &DelayFlag);
    //while(DelayFlag == 0) __RESET_WATCHDOG(); 		

    for(u16Counter = 0; u16Counter < 10; u16Counter++) WriteSPIByte(0xFF); 
    
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

  return (SD_OK);
}

error SD_Prender(){
    word Index;
    byte Rx,arg[4];
    int timer=0;
    SPIC1_SPE = 1;     // Enable device
    (void)SPIS;
    (void)SD_SetBaudRateMode(SD_BM_285_714KHZ);
  	SD_DesAssert();    //Hacemos un Desassert (-SS=1 Activo en baja)
  	for ( Index=0 ; Index < 10 ; Index ++ )	// Send dummy char
  	    (void) SD_EnviarByte(DUMMY_BYTE);
    Cpu_Delay100US(100);
  	SD_Assert();       //Hacemos un Assert (-SS=0 Activo en baja)
  	arg[0]=0x00;
  	arg[1]=0x00;
    arg[2]=0x00;
    arg[3]=0x00;
    //Protocolo para Inicializar el modo SPI en la SD 
    (void)SD_EnviarCMD(CMD0,arg);      //Enviamos el CMD0
  	(void)SD_EnviarCMD(CMD1,arg);      //Enviamos el CMD1
    do{
        (void)SD_EnviarCMD(CMD1,arg);	  
    	  (void)SD_EnviarRecibirByte(DUMMY_BYTE,&Rx);
    	  (void)SD_EnviarRecibirByte(DUMMY_BYTE,&Rx);
    	  (void)SD_EnviarRecibirByte(DUMMY_BYTE,&Rx);
    }while(Rx != 0 && timer++ < TIMER_RESP);
  	if(timer >= TIMER_RESP)
  	    return _ERR_TIMER;
  	(void)SD_SetBaudRateMode(SD_BM_2MHZ);
  	return _ERR_OK;	
}


//***********************************************************************   SD_ENVIARCMD


error SD_EnviarCMD(byte CMD,byte *ARG){
    (void)SD_EnviarByte(CMD);
    (void)SD_EnviarByte(ARG[0]);
    (void)SD_EnviarByte(ARG[1]);
    (void)SD_EnviarByte(ARG[2]);
    (void)SD_EnviarByte(ARG[3]);
    if(CMD == CMD0)
        (void)SD_EnviarByte(CRC_CMD0);
    else
        (void)SD_EnviarByte(DUMMY_BYTE);
    return _ERR_OK;
}


//***********************************************************************   SD_ENVIARBYTE


error SD_EnviarByte(byte  Data){
    do{          // Wait for transmit buffer become empty
        //WatchDog_Clear();
    }while(!SPIS_SPTEF);
    SPID=Data;   // Put into transmit buffer
    do{          // Wait for transmit buffer become empty
        //WatchDog_Clear();
    }while(!SPIS_SPTEF);
    return _ERR_OK;
}


//***********************************************************************   SD_RECIBIRBYTE


error SD_RecibirByte( byte *Rx ){
    // Init SPI transfer
    (void)SD_EnviarByte( DUMMY_BYTE );
    do{        // Wait for recive buffer become full
       //WatchDog_Clear();
    }while(!SPIS_SPRF);
    *Rx=SPID;  // Read receive buffer
    return _ERR_OK; 
}


//***********************************************************************   SD_ENVIARRECIBIRBYTE


error SD_EnviarRecibirByte(byte Tx,byte *Rx){
    do{
    }while(!SPIS_SPTEF);
    SPID=Tx;
    do{
    }while(!SPIS_SPTEF);
    do{
    }while(!SPIS_SPRF);
    *Rx=SPID;
    return _ERR_OK; 
}


//-----------------------------   SD_LEER   -----------------------------//


error SD_Leer(dato lectura[][tam_dato]) {
    byte arg[4],test;
    int h=0, i=0;
    (void)SD_Assert();
    // Leemos 512 bytes
        arg[0]=0x00;
    	  arg[1]=0x00;
        arg[2]=0x02;
        arg[3]=0x00;
        
    
    // Enviar cmd16 y leeremos 480 bytes
  	(void)SD_EnviarCMD(CMD16,arg);
  	// esperamos la respuesta del cmd16
  	h=0;
  	do{
  	    (void)SD_RecibirByte(&test);
    }while(test != 0 && h++ < TIMER_RESP);
  	if(h > TIMER_RESP)
  	    return _ERR_TIMER;	
    // Trying to read a BLOCK
  	// Enviamos el cmd17 donde indicamos el inicio de la lectura
  	(void)SD_EnviarCMD(CMD17,dir_lectura);
  	// esperamos la respuesta R1 del cmd17 es decir un 0
  	h=0;
  	do{
  	    (void)SD_RecibirByte(&test);
  	}while(test != 0 && h++ < TIMER_RESP);
    if(h > TIMER_RESP)
  	    return _ERR_TIMER;	
  	// Recibimos el FE?--->Data token for reading 11111110 =)
  	h=0;
  	do{
  	    (void)SD_RecibirByte(&test);
  	}while(test != 254 && h++ < TIMER_RESP);
  	if(h > TIMER_RESP)
  	    return _ERR_TIMER;	
    //Guardamos lo q leeemos segun TipoLectura     
 
  	while(i<cantidad_datos){
  	    h=0;
  	    while(h<tam_dato){
	      
  	    (void)SD_RecibirByte(&test);
  	    lectura[i][h]=test;
  	    h++;
  	    }
  	    (void)SD_RecibirByte(&test);
  	    (void)SD_RecibirByte(&test);
  	    i++;
  	}
    
    
    do{
        (void)SD_RecibirByte(&test);
    }while(test != 0xff);
    (void)SD_DesAssert();
    (void)Cpu_Delay100US(100);
    
    (void)SD_CalculaDireccion(dir_lectura);
     ban_bufferTx=0;
        
    return _ERR_OK;

} 


//-----------------------------   SD_ESCRIBIR   -----------------------------//


error SD_Escribir(byte *direccion,dato buf[][tam_dato]){
    byte Rx;
    int i=0 , h=0 , cantidad_espacios=0 , j=0;
    (void)SD_EnviarCMD(CMD24,direccion);  // CMD24 para escribir
    // Debemos recibir respuesta R1
		do{
		    (void)SD_RecibirByte(&Rx);
		}while(Rx != 0 && h++ < TIMER_RESP);
    if(h > TIMER_RESP)
        return _ERR_TIMER;
    (void)SD_EnviarByte(TOKEN);   // Enviar el FE--->Data token for write 11111110 =)
    //Intentamos escribir
    for(i=0 ; i < cantidad_datos ; i++){
        h=0;    
        do{
	          (void)SD_EnviarByte(buf[i][h]);
	          h++;  
	      }while(h < (tam_dato));
	      if(i < (cantidad_datos-1)){ 
	          (void)SD_EnviarByte(0x0D);  
	          (void)SD_EnviarByte(0x0A);
	      } // cierra el IF
    } // cierra el FOR
    //Para completar el bloque de 512
    //enviamos 1 espacio y luego 1 salto de linea
    cantidad_espacios = TAM_BLOQUE-(((tam_dato)*cantidad_datos)+(cantidad_datos*2));
    for(j=0 ; j < cantidad_espacios ; j++) 
        (void)SD_EnviarByte(0x20);
    (void)SD_EnviarByte(0x0D);
    (void)SD_EnviarByte(0x0A);
    (void)SD_EnviarByte(ID_H); // Se envian dos CRC que no estan calculados.
    (void)SD_EnviarByte(ID_L);
    //Esperamos que la tarjeta termine de escribir
    h=0;
    do{
		    (void)SD_RecibirByte(&Rx);
		}while(Rx == 0xFF && h++ < TIMER_RESP);
    do{ 
        (void)SD_RecibirByte(&Rx);
    }while(Rx == 0);
    SD_DesAssert();  // Hacemos un Desassert (-SS=1 Activo en baja)
	  (void)Cpu_Delay100US(100);
	  SD_Assert();     // Hacemos un Assert (-SS=0 Activo en baja)
    ban_SDvacia=0;
    return _ERR_OK;
}


//-----------------------------   SD_SETBAUDRATEMODE   -----------------------------//


error SD_SetBaudRateMode(byte Mod){
    static const  byte SD_BaudRate[2] = {98,2};
    if (Mod >= 2) {
        return _ERR_BAUDRATE;
    }
    SPIBR = SD_BaudRate[Mod];
    return _ERR_OK;
}


//-----------------------------   SD_CALCULARDIRECCION   -----------------------------//


error SD_CalculaDireccion(byte * dir){
    //long *aux , aux2;
    //aux=dir;
	long Auxiliar;
	
    Auxiliar = dir[0];
    Auxiliar <<= 8;
    Auxiliar |= dir[1];
    Auxiliar <<= 8;
    Auxiliar |= dir[2];
    Auxiliar <<= 8;
    Auxiliar |= dir[3];
    
	Auxiliar += 512;
	
    if(Auxiliar < 512)
        return _ERR_DIR;
    dir[0]=(byte)(Auxiliar>>24);
    dir[1]=(byte)(Auxiliar>>16);
    dir[2]=(byte)(Auxiliar>>8);
    dir[3]=(byte)(Auxiliar);
    (void)SD_Prender();
    (void)SD_EscribirDireccion();
    return _ERR_OK;            
}


//-----------------------------   SD_ESCRIBIRPRUEBA   -----------------------------//


error SD_escribirPrueba(){
    byte Rx , leer[4];
    int i=0 , h=0;
    leer[0]=0x00;
    leer[1]=0x00;
    leer[2]=0x02;
    leer[3]=0x00;
    (void)SD_EnviarCMD(CMD24,leer);  // CMD24 para escribir
    //Debemos recibir respuesta R1
    h=0;
    do{
        (void)SD_RecibirByte(&Rx);
    }while(Rx != 0 && h++ < TIMER_RESP);
    if(h>TIMER_RESP)
        return _ERR_TIMER;
    (void)SD_EnviarByte(TOKEN);   // Enviar el FE--->Data token for write 11111110 =)
     h=0;      //Intentamos escribir
    //do {
        (void)SD_EnviarByte(0x00);
    	  (void)SD_EnviarByte(0x00);
    	  (void)SD_EnviarByte(0x08);
    	  (void)SD_EnviarByte(0x00);
    //}while(h++ < 4);
    (void)SD_EnviarByte(DUMMY_BYTE); // Se envian dos CRC que no estan calculados.
    (void)SD_EnviarByte(DUMMY_BYTE);
    h=0;
    do{
        (void)SD_RecibirByte(&Rx);
    }while(Rx == 0xFF && h++ < TIMER_RESP);
    do{ 
        (void)SD_RecibirByte(&Rx);
    }while(Rx == 0);
    SD_DesAssert();    // Hacemos un Desassert (-SS=1 Activo en baja)
    (void)Cpu_Delay100US(100);
    SD_Assert();     // Hacemos un Assert (-SS=0 Activo en baja)
    return _ERR_OK;
}



//////////////////////////////////SD CON DATOS/////////////////////////////////
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
	byte arg[4],test;
	byte dirbase[4];
    int h=0, i=0;
    (void)SD_Assert();
    // Leemos 512 bytes
    arg[0]=0x00;
    arg[1]=0x00;
    arg[2]=0x00;
    arg[3]=0x08;
    dirbase[0]=0x00;
    dirbase[1]=0x05;
    dirbase[2]=0x04;
    dirbase[3]=0x00;
        
    
    // Enviar cmd16 y leeremos 8 bytes
  	(void)SD_EnviarCMD(CMD16,arg);
  	// esperamos la respuesta del cmd16
  	h=0;
  	do{
  	    (void)SD_RecibirByte(&test);
    }while(test != 0 && h++ < TIMER_RESP);
  	if(h > TIMER_RESP)
  	    return _ERR_TIMER;	
    // Trying to read a BLOCK
  	// Enviamos el cmd17 donde indicamos el inicio de la lectura
  	(void)SD_EnviarCMD(CMD17,dirbase);
  	// esperamos la respuesta R1 del cmd17 es decir un 0
  	h=0;
  	do{
  	    (void)SD_RecibirByte(&test);
  	}while(test != 0 && h++ < TIMER_RESP);
    if(h > TIMER_RESP)
  	    return _ERR_TIMER;	
  	// Recibimos el FE?--->Data token for reading 11111110 =)
  	h=0;
  	do{
  	    (void)SD_RecibirByte(&test);
  	}while(test != 254 && h++ < TIMER_RESP);
  	if(h > TIMER_RESP)
  	    return _ERR_TIMER;	
    //Guardamos lo q leeemos segun TipoLectura     
    i=0;
  	while(i<4){
  	    (void)SD_RecibirByte(&test);
  	    dir_lectura[i]=test;
  	    i++;
  	}
  	while(i<8){
  	    (void)SD_RecibirByte(&test);
  	    dir_escritura[i-4]=test;
  	    i++;
  	}
  	//(void)SD_RecibirByte(&test);
  	//(void)SD_RecibirByte(&test);
    do{
        (void)SD_RecibirByte(&test);
    }while(test != 0xff);
    (void)SD_DesAssert();
    (void)Cpu_Delay100US(100);
    ban_bufferTx=0;
    return _ERR_OK;
}



error SD_EscribirDireccion(){
	byte Rx;
	byte dirbase[4];
    int i=0 , h=0 , cantidad_espacios=0 , j=0;
    dirbase[0]=0x00;
    dirbase[1]=0x05;
    dirbase[2]=0x04;
    dirbase[3]=0x00;
    (void)SD_EnviarCMD(CMD24,dirbase);  // CMD24 para escribir
    // Debemos recibir respuesta R1
		do{
		    (void)SD_RecibirByte(&Rx);
		}while(Rx != 0 && h++ < TIMER_RESP);
    if(h > TIMER_RESP)
        return _ERR_TIMER;
    (void)SD_EnviarByte(TOKEN);   // Enviar el FE--->Data token for write 11111110 =)
    //Intentamos escribir
        
	for(i=0 ; i < cantidad_datos ; i++){
        h=8;    
        
        (void)SD_EnviarByte(dir_lectura[0]);
        (void)SD_EnviarByte(dir_lectura[1]);
        (void)SD_EnviarByte(dir_lectura[2]);
        (void)SD_EnviarByte(dir_lectura[3]);
        
        (void)SD_EnviarByte(dir_escritura[0]);
        (void)SD_EnviarByte(dir_escritura[1]);
        (void)SD_EnviarByte(dir_escritura[2]);
        (void)SD_EnviarByte(dir_escritura[3]);
        do{
	          (void)SD_EnviarByte(0x00);
	          h++;  
	      }while(h < (tam_dato));
	      if(i < (cantidad_datos-1)){ 
	          (void)SD_EnviarByte(0x0D);  
	          (void)SD_EnviarByte(0x0A);
	      } // cierra el IF
    } // cierra el FOR
    //Para completar el bloque de 512
    //enviamos 1 espacio y luego 1 salto de linea
    cantidad_espacios = TAM_BLOQUE-(((tam_dato)*cantidad_datos)+(cantidad_datos*2));
    for(j=0 ; j < cantidad_espacios ; j++) 
        (void)SD_EnviarByte(0x20);
    (void)SD_EnviarByte(0x0D);
    (void)SD_EnviarByte(0x0A);    
	(void)SD_EnviarByte(ID_H); // Se envian dos CRC que no estan calculados.
    (void)SD_EnviarByte(ID_L);
    //Esperamos que la tarjeta termine de escribir
    h=0;
    do{
		    (void)SD_RecibirByte(&Rx);
		}while(Rx == 0xFF && h++ < TIMER_RESP);
    do{ 
        (void)SD_RecibirByte(&Rx);
    }while(Rx == 0);
    SD_DesAssert();  // Hacemos un Desassert (-SS=1 Activo en baja)
	(void)Cpu_Delay100US(100);
	SD_Assert();     // Hacemos un Assert (-SS=0 Activo en baja)
    return _ERR_OK;
}

