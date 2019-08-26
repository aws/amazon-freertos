#ifndef __AWS_OTA_PAL_SETTINGS
#define __AWS_OTA_PAL_SETTINGS

#include "stdint.h"
#include "aws_iot_ota_agent.h"
/* Number of bytes to represent an component (R,S) of the the curve. */
#define ECC_NUM_BYTES_PER_SIG_COMPONENT    ( 32U )

/* Number of components of signature. */
#define ECC_NUM_SIG_COMPONENTS             ( 2U )

/*#define BOOTLOADER_SETTING_SIZE */
/*#define BOOTLOADER_SIZE 16*1024 // Size of bootloader */

/*FLASH_PH_START=0x0 */
/*FLASH_PH_SIZE=0x100000 */
/*RAM_PH_START=0x20000000 */
/*RAM_PH_SIZE=0x40000 */
/*FLASH_START=0x26000 */
/*FLASH_SIZE=0xda000 */
/*RAM_START=0x200046F8 */
/*RAM_SIZE=0x3B908 */

/* TODO: Aling the addresses to the begining of code pages */



extern size_t __reserved_flash_end__, __FLASH_segment_end__;


#define otapalBOOTLOADER_START       (0xF8000)
#define otapalFLASH_START            ( SD_SIZE_GET( MBR_SIZE ) )
#define otapalTOPRESERVED            6 * 4096                                                  /* Reserved for bootloader MBR data and user data on top of the flash */
#define otapalFIRST_BANK_START       ( size_t ) ( otapalFLASH_START )                          /* First bank start address */
#define otapalSECOND_BANK_END        ( ( size_t ) otapalBOOTLOADER_START - otapalTOPRESERVED ) /* Second bank end address */
#define otapalFIRST_BANK_END         0x8C000                                                   /* First bank end address */
#define otapalSECOND_BANK_START      otapalFIRST_BANK_END                                      /* Second bank start address */
#define otapalFIRST_BANK_SIZE        ( otapalFIRST_BANK_END - otapalFIRST_BANK_START )         /* First bank size */
#define otapalSECOND_BANK_SIZE       ( otapalSECOND_BANK_END - otapalSECOND_BANK_START )       /*Second bank size */

#define otapalDESCRIPTOR_SIZE        NRF_FICR->CODEPAGESIZE                                    /* The size of the firmware descriptor */
#define otapalMAGICK_SIZE            7
/* This array need to be at most equal to a page. */
#define otapalSERIALIZING_ARRAY_SIZE 1024 

#define otapalMAX_PUBLIC_KEY_SIZE    96 /* Maximum size of the public key */
typedef enum
{
    otapalIMAGE_FLAG_NEW = 0xFF,            /* If the application image is running for the first time and never executed before. */
    otapalIMAGE_FLAG_COMMIT_PENDING = 0xFE, /* The application image is marked to execute for test boot. */
    otapalIMAGE_FLAG_VALID = 0xFC,          /*The application image is marked valid and committed. */
    otapalIMAGE_FLAG_INVALID = 0xF8         /*The application image is marked invalid. */
} ImageFlags_t;

typedef struct
{
    uint8_t pMagick[ otapalMAGICK_SIZE ];           /* 7 byte pattern used to identify if application image is present on the image slot in flash. */
    uint8_t usImageFlags;                           /* Image flags */
    uint32_t ulSequenceNumber;                      /* A 32 bit field in image descriptor and incremented after image is received and validated by OTA module */
    uint32_t ulStartAddress;                        /* Starting address of the application image */
    uint32_t ulEndAddress;                          /* End address of the application image */
    uint32_t ulExecutionAddress;                    /* The load/execution address of the application image */
    uint32_t ulHardwareID;                          /* 32 bit ID that can be generated unique for a particular platform */
    uint32_t ulSignatureSize;                       /* Size of the crypto signature being used */
    uint8_t pSignature[ kOTA_MaxSignatureSize ]; /* Crypto signature of the application image excluding the descriptor */
} ImageDescriptor_t;

enum FlashEvents_t
{
    otapalFLASH_SUCCESS = 1, otapalFLASH_FAILURE = 2
};
#endif /*__AWS_OTA_PAL_SETTINGS */
