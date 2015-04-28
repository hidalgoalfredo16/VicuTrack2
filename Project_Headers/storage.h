/*
 * storage.h
 *
 *  Created on: Apr 16, 2015
 *      Author: usuario
 */

#include<derivative.h>

#include "types.h"
//#include "configuracion.h"

/************************* HIL ****************************/
/**********************************************************/
/* Includes */


/* Storage HIL */
#define GetPhysicalBlock(A,B)       (void)SD_ReadSector(A,B);     
#define StorePhysicalBLock(A,B)     (void)SD_WriteSector(A,B);
/**********************************************************/
/**********************************************************/

/* Macros */
#define ByteSwap(A)     (A=(A<<8)+(A>>8))

/* definitions */
#define MASTER_BLOCK            0x00
#define RootEntrySize           32
#define EntriesPerBlock         16          //Block size / RootEntrySize 
#define RHandler_FAT_ENTRIES    8

#define BLOCK_SIZE            512


/*-- Directory Defines --*/
#define FILE_AVAILABLE      0x00
#define FILE_USER           0xFF

#define FILE_Erased         0xE5
#define FILE_Clear          0x00

#define AT_VOLUME           0x01
#define AT_DIRECTORY        0x02
#define AT_HIDDEN           0x04
#define AT_SYSTEM           0x08
#define AT_READONLY         0x10
#define AT_ARCHIVE          0x20
#define AT_LFN              0x0F
#define FILE_Index_Clear    0x0000



enum {
    READ,
    CREATE,
    MODIFY,
    DELETE,
    NEXT_ENTRY,
    WRITE_ENTRY,
    OVERWRITE
};

enum {
    FILE_FOUND,
    FILE_NOT_FOUND,
    FILE_CREATE_OK,
    NO_FILE_ENTRY_AVAILABLE,
    NO_FAT_ENTRY_AVAIlABLE,
    ERROR_IDLE
};      


/* typedef */

#pragma options align = packed //Solve structure alignment problem when porting from 8bits to 32bits MCU


typedef struct _ReadHandler {
    UINT16  FAT_Entry;
    UINT16  SectorOffset;
    UINT16  Dir_Entry;
    UINT32  File_Size;
}ReadRHandler;


typedef struct _WriteRHandler {
    UINT8   FileName[8];
    UINT8   Extension[3];
    UINT16  Dir_Entry;
    UINT32  File_Size;
    UINT16  BaseFatEntry;
    UINT16  CurrentFatEntry;
    UINT16  SectorIndex;
    UINT16  ClusterIndex;
}WriteRHandler;



/* Root Directory Structure */
typedef struct _root_Entries {
    UINT8   FileName[8];
    UINT8   Extension[3];
    UINT8   Attributes;
    UINT8   _Case;
    UINT8   MiliSeconds;
    UINT16  CreationTime;
    UINT16  CreationDate;
    UINT16  AccessDate;                            
    UINT16  Reserved;
    UINT16  ModificationTime;    
    UINT16  ModificationDate;
    UINT16  ClusterNumber;
    UINT32  SizeofFile;
}root_Entries;

/* Master Boot Record */
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

/* Partition Table */
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

/*
void FAT_CreateFATLinks(UINT16);
void FAT_LS(void);
*/

/* Prototypes */
UINT32 LWordSwap(UINT32);
void   FAT_Read_Master_Block(UINT8 *);
UINT16 FAT_SearchAvailableFAT(UINT16);
UINT16 FAT_Entry(UINT16,UINT16,UINT8);
UINT8  FAT_FileOpen(UINT8*,UINT8);
void   FAT_FileClose(void);        
void   FAT_FileWrite(UINT8*,UINT32);
UINT16 FAT_FileRead(UINT8*,UINT32*);  
UINT8  FAT_FileRename(UINT8*,UINT8*);
UINT8  FAT_GetFileInfo(ReadRHandler*);
UINT8  FAT_ListFiles(UINT8*, UINT16, UINT16*);
