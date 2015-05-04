/*!
 @file storage.h
 @brief Declara constantes y funciones para el manejo del cpu
 */

#include<derivative.h>
#include "types.h"

//! Define la operacion de intercambio del byte mas significativo por el menos significativo
#define ByteSwap(A)     (A=(A<<8)+(A>>8))

#pragma options align = packed //Solve structure alignment problem when porting from 8bits to 32bits MCU

typedef struct _MasterBoot_Entries {
    UINT8   JMP_NOP[3];
    UINT8   OEMName[8];
    UINT16  BytesPerSector;
    UINT8   SectorsPerCluster;
    UINT16  ReservedSectors;
    UINT8   FatCopies;
    UINT16  RootDirectoryEntries;
    UINT16  SectorsLess32MB;
    UINT8   MediaDescriptor;
    UINT16  SectorsPerFat;
    UINT16  SectorsPerTrack;
    UINT16  NumberOfHeads;
    UINT32  HiddenSectors;                     
    UINT32  SectorsInPartition;
    UINT16  LogicalNumberOfPartitions;
    UINT8   ExtendedSignature;
    UINT32  SerialNumber;
    UINT8   VolumeNumber[11];
    UINT8   FatName[8];
    UINT8   ExcecutableCode[448];
    UINT8   ExcecutableMarker[2];
}MasterBoot_Entries;

typedef struct _PartitionTable_Entries {
    UINT8   Code[440];
    UINT32  WindowsSignature;
    UINT16  EOF;
    struct {
        UINT8  ActivePartition;
        UINT8  CHSStart[3]; //Unorder!
        UINT8  PartitionType;
        UINT8  CHSEnd[3];   //Unorder!
        UINT32 LBAStart;
        UINT32 Size;
    } Partition[4];
    UINT8  ExcecutableMarker[2];  
}PartitionTable_Entries;

#pragma options align = reset

//!Realiza un swap en un dato de 32 bytes
/*!
Intercambio los dos bytes mas significativos con los dos menos significativos
param[in] u32DataSwap Dato sobre el cual se realizara el swap
return u32Temp Resultado del swap
 */
UINT32 LWordSwap(UINT32);

//! Obtiene el Master Block
/*!
 Se obtiene la siguiente informacion de la particion: 
 @li @c Tamano del cluster en sectores, usualmente 16Sectores
 @li @c Tamano del sector, usualmente 512Bytes
 @li @c Sectores reservados para almacenar copias de respaldo de la FAT
 @li @c Direccion del Root Directory
 @li @c Ubicacion del primer archivo de la particion
 */
void   FAT_Read_Master_Block(UINT8 *);
