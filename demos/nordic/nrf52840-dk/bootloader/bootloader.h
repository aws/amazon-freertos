#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H



#define AFR_MAGICK                     "@AFRTOS"                                       /* Magick number for the AFR image */
#define MAGICK_SIZE                    7                                               /* sizeof Magick */
#define SIGNATURE_MAX_SIZE             256                                             /* Size allocated for signature in the header */
#define CODE_REGION_1_START            SD_SIZE_GET( MBR_SIZE )                         /* Start of the first region*/
#define CODE_REGION_2_START            0x8C000                                         /* Start of the second region */
#define DESCRIPTOR_SIZE                CODE_PAGE_SIZE
#define NRF_UICR_BOOT_START_ADDRESS    ( NRF_UICR_BASE + 0x14 )                        /* UICR page containing bootloader start address */
#define BOOTLOADER_REGION_START        ( *( uint32_t * ) NRF_UICR_BOOT_START_ADDRESS ) /* Bootloader start address */
#define HARDWARE_ID                    0
#define STACK_BEGIN                    0x20040000                                      /* Starting address for the stack */
#define MAX_PUBLIC_KEY_SIZE            96                                              /* Maximum size of the public key */
#define APP_DATA_RESERVED              3 * CODE_PAGE_SIZE                              /* Reserve for application data that must be retained */
#define BOOTLOADER_START               0xF8000                                         /* Starting address of the bootloader */

#define SWAP_AREA_SIZE      4                                       /* Number of pages dedicated for swapping */
#define SWAP_STATUS_PAGE    (( uint8_t * ) ( BOOTLOADER_START - APP_DATA_RESERVED - (SWAP_AREA_SIZE + 1) * CODE_PAGE_SIZE )) /* Page containing status of swapping */
#define SWAP_AREA_BEGIN     (( uint8_t * ) ( BOOTLOADER_START - APP_DATA_RESERVED - SWAP_AREA_SIZE * CODE_PAGE_SIZE )) /* Pages for swapping data */
#define CHUNK_SIZE_BYTES    ( SWAP_AREA_SIZE * CODE_PAGE_SIZE)      /* Size of a swapping chunk */

#define MAX_FIRMWARE_SIZE              ( BOOTLOADER_START - (SWAP_AREA_SIZE + 1) * CODE_PAGE_SIZE - APP_DATA_RESERVED - CODE_REGION_2_START )   /* Maximum firmware size */
#define MAX_FIRMWARE_PAGES             ( MAX_FIRMWARE_SIZE / CODE_PAGE_SIZE )          /* Maximum firmware size in pages*/



typedef struct
{
    uint8_t pMagick[ MAGICK_SIZE ];           /* 7 byte pattern used to identify if application image is present on the image slot in flash. */
    uint8_t usImageFlags;                     /* Image flags */
    uint32_t ulSequenceNumber;                /* A 32 bit field in image descriptor and incremented after image is received and validated by OTA module */
    uint32_t ulStartAddress;                  /* Starting address of the application image */
    uint32_t ulEndAddress;                    /* End address of the application image */
    uint32_t ulExecutionAddress;              /* The load/execution address of the application image */
    uint32_t ulHardwareID;                    /* 32 bit ID that can be generated unique for a particular platform */
    uint32_t ulSignatureSize;                 /* Size of the crypto signature being used */
    uint8_t pSignature[ SIGNATURE_MAX_SIZE ]; /* Crypto signature of the application image excluding the descriptor */
} ImageDescriptor_t;

typedef enum
{
    IMAGE_FLAG_NEW = 0xFF,            /* If the application image is running for the first time and never executed before. */
    IMAGE_FLAG_COMMIT_PENDING = 0xFE, /* The application image is marked to execute for test boot. */
    IMAGE_FLAG_VALID = 0xFC,          /*The application image is marked valid and committed. */
    IMAGE_FLAG_INVALID = 0xF8         /*The application image is marked invalid. */
} ImageFlags_t;

#define BANK1_DESCRIPTOR    ( ( ImageDescriptor_t * ) CODE_REGION_1_START )
#define BANK2_DESCRIPTOR    ( ( ImageDescriptor_t * ) CODE_REGION_2_START )

typedef enum
{
    LED_BOOT, LED_NO_CORRECT_FIRMWARE, LED_SWAP_IN_PROCESS
} LedStatus_t; /* Enum for the status indication */

typedef enum
{
    FIRST_BANK, SECOND_BANK
} Bank_t;                                                           /* Enum for the possible banks */



typedef enum                                                        /* Swapping state */
{
    SWAP_NONE = 0xFF,
    SWAP_FIRST_TO_TEMP = 0xFE,
    SWAP_SECOND_TO_FIRST = 0xFC,
    SWAP_TEMP_TO_SECOND = 0xF8,
    SWAP_COMPLETED = SWAP_TEMP_TO_SECOND,
    SWAP_IN_PROGRESS = 0x00
} SwapState_t;

typedef enum {
    BANK_VALID,
    BANK_NEW,
    BANK_COMMIT_PENDING,
    BANK_NORDIC,
    BANK_NORDIC_DESCRIPTOR,
    BANK_INVALID,
    BANK_NONE
} BankState_t;

#endif /* __BOOTLOADER_H */
