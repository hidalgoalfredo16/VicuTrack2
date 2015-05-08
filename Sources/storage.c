/*!
 @file storage.c
 @brief Define variables e implementa funciones necesarias para el manejo del storage
 */

#include "storage.h"
#include "sd.h"
#include "types.h"

/* File Handlers */
//WriteRHandler WHandler;
//ReadRHandler  RHandler;

/* File Buffers */
//UINT8 ag8FATReadBuffer[512];
//UINT8 ag8FATWriteBuffer[512];

/* Global Variables */
UINT16 u16FAT_Sector_Size;
UINT16 u16FAT_Cluster_Size;
UINT16 u16FAT_FAT_BASE;
UINT16 u16FAT_Root_BASE;
UINT16 u16FAT_Data_BASE;
UINT16 u16Main_Offset=0;

#pragma warn_implicitconv off

UINT32 LWordSwap(UINT32 u32DataSwap) {
    UINT32 u32Temp;
    u32Temp = (u32DataSwap & 0xFF000000) >> 24;
    u32Temp += (u32DataSwap & 0xFF0000)  >> 8;
    u32Temp +=(u32DataSwap & 0xFF00)     << 8;
    u32Temp +=(u32DataSwap & 0xFF)       << 24;
    return(u32Temp);    
}

void FAT_Read_Master_Block(UINT8* ag8FATReadBuffer) {
    static MasterBoot_Entries     *pMasterBoot; //Make static for debug
    static PartitionTable_Entries *PartitionTable; //Make static for debug
    
    /*
    Opcion 1: Partiton Loop
    Este bucle permite posisionarse en la primera particion sin tener que consultar el MBR para
    determinar su ubicacion. Para esto recorre secuancialmente el volumen, byte por byte, hasta
    encontrar el patron EB 3C 90, que identifica a la misma. Luego obtiene el sector correspond
    a la particion para contar con la informacion necesaria para poder montar el sistema de 
    archivos.
    + Simple.
    - Solo pueden accederse archivos ubicados en la primera particion.
    - El tiempo requerido para montar el FS es variable y esta sujeto a que tan proxima se 
      encuentre la primera particion al inicio del volumen (sector 0). Ademas, en caso de ubicarse
      otra particion en las direcciones mas bajas del volumen, esta podria contener archivos con
      el patron EB 3C 90 y provocar que este metodo no funcione.*/
    /*
    while(ag8FATReadBuffer[0]!= 0xEB || ag8FATReadBuffer[1]!=0x3C || ag8FATReadBuffer[2]!=0x90) //Recorre linealmente el volumen en busca del patron que identifica a la primera particion
    {
        SD_ReadSector(u16Main_Offset++,&ag8FATReadBuffer[0]);  //Si encuentra el patron, obtiene el bloque y lo carga en ag8FATReadBuffer
        __RESET_WATCHDOG();
    }
    u16Main_Offset--; //Corrige la direccion donde se encuentra la primera particion
    */
    /* Opcion 2: MBR Parser
    En lugar de recorrer linealmente el volumen hasta dar con la primera particion, se obtiene
    el primer bloque (sector 0, donde reside el MBR) y se analiza el campo del MBR donde se 
    especifica la dirección, en numero de bloques, de la primer particion.
    + No muy complejo.
    + El tiempo requerido para montar el FS es siempre el mismo, ya que no es necesario recorrer
      el volumen para obtener la direccion de las particiones, sino que se obtienen luego de interpretar 
      el MBR, ubicado siempre en el sector 0 del volumen.
    + Permite montar cualquier particion, no solo la primera, con leves cambios en el codigo. */    
    
    //Supongo que la primera particion se encuentra en el sector0
    u16Main_Offset = 0;
     //Cargo sector0 en ag8FATReadBuffer (arreglo de bytes sin estructura)
    SD_ReadSector(0, &ag8FATReadBuffer[0]);

    //Supongo que sector0 es el sector correspondiente a la primera particion
    pMasterBoot = (MasterBoot_Entries*)ag8FATReadBuffer; 

    //Si el sector no corresponde a la primera particion, no verifica el patron inicial de la misma (0xEB3C90). Por lo tanto debo calcular su ubicacion 
    if(pMasterBoot->JMP_NOP[0]!=0xEB || pMasterBoot->JMP_NOP[1] != 0x3C || pMasterBoot->JMP_NOP[2] != 0x90) {                                       
      //Apunta el contenido del buffer ag8FATReadBuffer de acuerdo a la estructura PartitionTable_Entries
      PartitionTable = (PartitionTable_Entries*)ag8FATReadBuffer;
      //Si no hay ExecutableMarker, no es el sector correspondiente a la primera particion, salgo
      if(PartitionTable->ExcecutableMarker[0] != 0x55 || PartitionTable->ExcecutableMarker[1] != 0xAA) return;                         
      //Calculo la ubicacion de la primera particion y la almaceno en u16Main_Offset
      u16Main_Offset = (UINT16)LWordSwap(PartitionTable->Partition[0].LBAStart); 
      //Obtiene sector que contiene la primera particion, de acuerdo al offset calculado      
      SD_ReadSector(u16Main_Offset, &ag8FATReadBuffer[0]);                   
    }
    //Si la primera particion esta en el sector0 (primer sector obtenido) ni en el sector calculado (segundo sector obtenido), salgo
    if( (pMasterBoot->JMP_NOP[0] != 0xEB || pMasterBoot->JMP_NOP[1] != 0x3C || pMasterBoot->JMP_NOP[2] != 0x90 ) || (pMasterBoot->ExcecutableMarker[0] != 0x55 || pMasterBoot->ExcecutableMarker[1] != 0xAA) ) return; 

    /* Cargo informacion de la particion en la estructura correspondiente al MBR y en otras variables */
    pMasterBoot = (MasterBoot_Entries*)ag8FATReadBuffer;
    u16FAT_Cluster_Size = pMasterBoot->SectorsPerCluster; //Tamano del cluster en sectores, usualmente 16Sectores 
    u16FAT_Sector_Size  = ByteSwap(pMasterBoot->BytesPerSector); //Tamano del sector, usualmente 512Bytes 
    u16FAT_FAT_BASE  = u16Main_Offset + ByteSwap(pMasterBoot->ReservedSectors);  //Sectores reservados para almacenar copias de respaldo de la FAT
    u16FAT_Root_BASE = (ByteSwap(pMasterBoot->SectorsPerFat)<<1) + u16FAT_FAT_BASE;  //Direccion del Root Directory
    u16FAT_Data_BASE = (ByteSwap(pMasterBoot->RootDirectoryEntries) >>4) + u16FAT_Root_BASE; //Direccion del primer archivo de la particion
    
    /*Es necesario aplicar un Byte Swap en ciertos campos del MBR para interpretarlos correctamente, ya que algunos por definicion estan escritos en Little Endian y otros en Big Endian*/

}

