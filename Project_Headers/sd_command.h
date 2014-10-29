/******************************* SD Card Commands **********************************/

#define SD_IF_COND_VHS_MASK        (0x00000F00L)
#define SD_IF_COND_VHS_27_36       (0x00000100L)

#define SD_OCR_VOLTAGE_3_3V        (0x00300000L)
#define SD_OCR_CCS                 (0x40000000L)
#define SD_OCR_POWER_UP_STATUS     (0x80000000L)

#define SD_ACMD41_HCS              (0x40000000L)

#define SD_CMD0_GO_IDLE_STATE      (0) /* Resets the SD Memory Card */  
#define SD_CMD8_SEND_IF_COND       (8)
#define SD_CMD9_SEND_CSD           (9)
#define SD_CMD12_STOP              (12)
#define SD_CMD13_SEND_STATUS       (13)
#define SD_CMD16_SET_BLOCKLEN      (16)
#define SD_CMD17_READ_BLOCK        (17)
#define SD_CMD18_READ_MULTI_BLOCK  (18)
#define SD_CMD24_WRITE_BLOCK       (24)
#define SD_CMD25_WRITE_MULTI_BLOCK (25)
#define SD_ACMD41_SEND_OP_COND     (41)
#define SD_CMD55_APP_CMD           (55)
#define SD_CMD58_READ_OCR          (58)
#define SD_CMD59_CRC_ON            (59)
#define SD_ACMD13_SEND_STATUS      SD_CMD13_SEND_STATUS
#define SD_ACMD22_SEND_NUM_WR_BLOCKS (22)


#define SD_R1_IDLE_STATE           (1 << 0)
#define SD_R1_ERASE_RESET          (1 << 1)
#define SD_R1_ILLEGAL_CMD          (1 << 2)
#define SD_R1_CRC_ERROR            (1 << 3)
#define SD_R1_ERASE_SEQ            (1 << 4)
#define SD_R1_ADDRESS_ERROR        (1 << 5)
#define SD_R1_PARAM_ERROR          (1 << 6)
#define SD_R1_START_MASK           (1 << 7)
#define SD_R1_ERROR_MASK           (0x7E)
