/*
Amazon FreeRTOS OTA PAL for SAM E70
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* OTA PAL implementation for Microchip PIC32MZ platform. */


/*lint -e9045 Ignore advisories about non-hidden definitions in header files. */

#include "aws_ota_pal.h"
#include "iot_pkcs11.h"
#include "pkcs11_nvm.h"
#include "iot_crypto.h"
#include "iot_pkcs11_config.h"
#include "aws_ota_codesigner_certificate.h"

#include "device.h" 
#include "string.h"
#include "aws_flash_config.h"
#include "aws_iot_ota_agent.h"
#include "peripheral/efc/plib_efc.h"

#define SIZEOF_AWS_BOOT_IMAGE_DCPT      0x20    // preprocessor doesn't know sizeof()

// size of the AWS bootloader
// NOTE: always 32 pages (16 KB) to be multiple of the lock region size!
#define AWS_BOOTLOADER_16KB_UNITS       4       // 64 KB
#define AWS_BOOTLOADER_SIZE     (AWS_BOOTLOADER_16KB_UNITS * 16 * 1024)

// start of the current running image descriptor
#define AWS_FLASH_RUN_DCPT_START        (IFLASH_ADDR + AWS_BOOTLOADER_SIZE)

// the bootloader size also enforces the start of the application descriptor to be 16 pages (8KB) aligned
// (16 pages is the minimum size of flash that can be independently erased on this part)
#if ((AWS_FLASH_RUN_DCPT_START & ((1U << 14) - 1)) != 0)
#error "AWS bootloader size should be multiple of 16 KB!"
#endif

// space for the AWS_BOOT_IMAGE_DCPT at the beginning of the running image
// keep it in its own 512 bytes page so it's processed independently
// from the app image itself
#define AWS_IMAGE_RUN_DCPT_SIZE         IFLASH_PAGE_SIZE         

// the current running image is always in the lower part of the flash
// there is only one flash bank on E70!
#define AWS_FLASH_RUN_APP_START  (AWS_FLASH_RUN_DCPT_START + AWS_IMAGE_RUN_DCPT_SIZE)


/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

#define OTA_HALF_SECOND_DELAY            pdMS_TO_TICKS( 500UL )

/* definitions shared with the resident bootloader. */

#define AWS_BOOT_IMAGE_SIGNATURE_SIZE    ( 7U )

/* Microchip PAL error codes. */

#define MCHP_ERR_NONE                       0     /* No error. */
#define MCHP_ERR_INVALID_CONTEXT            -1    /* The context valiation failed. */
#define MCHP_ERR_ADDR_OUT_OF_RANGE          -2    /* The block write address was out of range. */
#define MCHP_ERR_FLASH_WRITE_FAIL           -3    /* We failed to write data to flash. */
#define MCHP_ERR_FLASH_ERASE_FAIL           -4    /* The flash erase operation failed. */
#define MCHP_ERR_NOT_PENDING_COMMIT         -5    /* Image isn't in the Pending Commit state. */

#define AWS_BOOT_CRC_SEED           0xffffffff


/**
 * Regarding MISRA 2012 requirements and the following values, first, the values
 * are based on definitions in an external SDK. For maintenance and portability,
 * it is undesirable to modify the external header files or duplicate their
 * contents. Second, the values in question are, by definition of the framework,
 * valid virtual addresses that are allocated for firmware image storage.
 */


/* The new image is always programmed in the upper flash bank. */
static const uint32_t ulFlashImageMaxSize = AWS_FLASH_RUN_APP_SIZE;

typedef struct
{
    const OTA_FileContext_t * pxCurOTAFile; /* Current OTA file to be processed. */
    uint32_t ulLowImageOffset;              /* Lowest offset/address in the application image. */
    uint32_t ulHighImageOffset;             /* Highest offset/address in the application image. */
} OTA_OperationDescriptor_t;

/* NOTE that this implementation supports only one OTA at a time since it uses a single static instance. */
static OTA_OperationDescriptor_t xCurOTAOpDesc;         /* current OTA operation in progress. */
static OTA_OperationDescriptor_t * pxCurOTADesc = NULL; /* pointer to current OTA operation. */


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * C );
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );
static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );



// data

// CRC calculator
static uint32_t     crc_lfsr;               // the LFSR register

static const uint32_t crc_tbl[256] =       // CRC calculating table
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F, 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};



// CRC calculation


void AWS_CrcInit(void)
{

    AWS_CrcInitEx(AWS_BOOT_CRC_SEED);
}

void AWS_CrcInitEx(uint32_t seed)
{
    crc_lfsr = seed;
}

void AWS_CrcAddBuffer(const uint8_t* buff, int buffLen)
{
    while(buffLen--)
    {
        crc_lfsr = (crc_lfsr >> 8) ^ crc_tbl[(crc_lfsr & 0xff) ^ *buff++];
    }
}

uint32_t AWS_CrcResult(void)
{
    return crc_lfsr;
}



static __inline__ bool_t prvContextValidate( OTA_FileContext_t * C )
{
    return( ( pxCurOTADesc != NULL ) && ( C != NULL ) &&
            ( pxCurOTADesc->pxCurOTAFile == C ) &&
            ( C->pucFile == ( uint8_t * ) pxCurOTADesc ) ); /*lint !e9034 This preserves the abstraction layer. */
}

static __inline__ void prvContextClose( OTA_FileContext_t * C )
{
    if( NULL != C )
    {
        C->pucFile = NULL;
    }

    xCurOTAOpDesc.pxCurOTAFile = NULL;
    pxCurOTADesc = NULL;
}

static bool_t prvContextUpdateImageHeaderAndTrailer( OTA_FileContext_t * C )
{
    AWS_BOOT_IMAGE_DCPT imgDcpt;

    const AWS_BOOT_IMAGE_DCPT*  app_run_dcpt = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_RUN_DCPT_START);

    memcpy(imgDcpt.header.img_sign, AWS_BOOT_IMAGE_SIGNATURE, sizeof(imgDcpt.header.img_sign));
    imgDcpt.header.img_flags = AWS_BOOT_FLAG_IMG_NEW;
    imgDcpt.version = app_run_dcpt->version + 1;
    if(imgDcpt.version == 0)
    {
        imgDcpt.version++;
    }
    imgDcpt.start_address = (void*)(AWS_FLASH_RUN_APP_START + pxCurOTADesc->ulLowImageOffset);
    imgDcpt.end_address = (void*)(AWS_FLASH_RUN_APP_START + pxCurOTADesc->ulHighImageOffset);

    // pointer to the new app descriptor in the flash
    const AWS_BOOT_IMAGE_DCPT*  app_new_dcpt = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_NEW_DCPT_START);

    const uint8_t* flash_store_address = (const uint8_t*)(app_new_dcpt) + pxCurOTADesc->ulLowImageOffset + AWS_NVM_PAGE_SIZE;
    imgDcpt.entry_address = (void*)*((uint32_t*)flash_store_address + 1);   // ARM image: IVT is located at the beginning of the image 
                                                                            // and the image execution address is always saved at (IVT + 4)
    // store the crc data:
    // calculate the CRC of the just programmed flash
    AWS_CrcInit();
    AWS_CrcAddBuffer(flash_store_address, (uint8_t*)imgDcpt.end_address - (uint8_t*)imgDcpt.start_address);
    imgDcpt.crc = ~AWS_CrcResult();

    bool flashRes = AWS_FlashBlockWrite((const uint8_t*)app_new_dcpt, (const uint8_t*)&imgDcpt, sizeof(imgDcpt));
    OTA_LOG_L1("[OTA-MCHP] Image - version: %d\r\n", imgDcpt.version);
    OTA_LOG_L1("[OTA-MCHP] Image - start: 0x%08x, end: 0x%08x, entry: 0x%08x\r\n", imgDcpt.start_address, imgDcpt.end_address, imgDcpt.entry_address);
    OTA_LOG_L1("[OTA-MCHP] Image - crc: 0x%08x, start: 0x%08x, end: 0x%08x, len(end-start): 0x%08x\r\n", imgDcpt.crc, flash_store_address, flash_store_address + ((uint8_t*)imgDcpt.end_address - (uint8_t*)imgDcpt.start_address), (uint8_t*)imgDcpt.end_address - (uint8_t*)imgDcpt.start_address);

    //return flashRes;
    return true;
}

/*
 * Turns the watchdog timer off.
 */
static void prvPAL_WatchdogDisable( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_WatchdogDisable" );

    OTA_LOG_L1( "[%s] Disable watchdog timer. \n", OTA_METHOD_NAME );

    /* Turn off the WDT. */
    //WDTCONbits.ON = 0;
   
}


/**
 * @brief Attempts to create a new receive file to write the file chunks to as
 * they come in.
 */
OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );

    int32_t lErr = MCHP_ERR_NONE;
    OTA_Err_t xReturnCode = kOTA_Err_Uninitialized;

    /* Check parameters. The filepath is unused on this platform so ignore it. */
    if( NULL == C )
    {
        OTA_LOG_L1( "[%s] Error: context pointer is null.\r\n", OTA_METHOD_NAME );
        lErr = MCHP_ERR_INVALID_CONTEXT;
    }
	else
	{
	    // program this new file in the new dcpt + app area
	    // few sanity checks first
	    configASSERT(SIZEOF_AWS_BOOT_IMAGE_DCPT == sizeof(AWS_BOOT_IMAGE_DCPT));

	    uint32_t flashSize = AWS_FLASH_NEW_APP_END - AWS_FLASH_NEW_DCPT_START;
	    int flashPages = flashSize / AWS_NVM_PAGE_SIZE;
	    // should be multiple of minimum erase chunk!
	    configASSERT( (flashPages % AWS_NVM_ERASE_PAGES_MIN) == 0 );

	    uint32_t* pFlashDest = (uint32_t*)AWS_FLASH_NEW_DCPT_START;
	    configASSERT( ((uint32_t)pFlashDest & ((1U << 13) - 1)) == 0 );

	    // start erasing
	    taskENTER_CRITICAL();
	    bool eraseSuccess = AWS_FlashPagesErase(pFlashDest, flashPages);
	    taskEXIT_CRITICAL();

	    if(!eraseSuccess)
	    {
            OTA_LOG_L1( "[%s] Error: Failed to erase the flash!\r\n", OTA_METHOD_NAME );
            lErr = MCHP_ERR_FLASH_ERASE_FAIL;
        }
        else
        {
            pxCurOTADesc = &xCurOTAOpDesc;
            pxCurOTADesc->pxCurOTAFile = C;
            pxCurOTADesc->ulLowImageOffset = ulFlashImageMaxSize;
            pxCurOTADesc->ulHighImageOffset = 0;

            OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
            C->pucFile = ( uint8_t * ) pxCurOTADesc;
        }
    }

    if( MCHP_ERR_NONE == lErr )
    {
        xReturnCode = kOTA_Err_None;
    }
    else
    {
        xReturnCode = ( uint32_t ) kOTA_Err_RxFileCreateFailed | ( ( ( uint32_t ) lErr ) & ( uint32_t ) kOTA_PAL_ErrMask ); /*lint !e571 intentionally cast lErr to larger composite error code. */
    }

    return xReturnCode;
}

/**
 * @brief Aborts access to an existing open file. This is only valid after a job
 * starts successfully.
 */
OTA_Err_t prvPAL_Abort( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );

    /* Check for null file handle since we may call this before a file is actually opened. */
    prvContextClose( C );
    OTA_LOG_L1( "[%s] Abort - OK\r\n", OTA_METHOD_NAME );

    return kOTA_Err_None;
}

/* Write a block of data to the specified file.
 * Returns the number of bytes written on success or negative error code.
 */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pcData,
                           uint32_t ulBlockSize )
{
    int16_t sReturnVal = 0;
    uint8_t ucPadBuff[ AWS_NVM_QUAD_SIZE ];
    uint8_t * pucWriteData = pcData;
    uint32_t ulWriteBlockSzie = ulBlockSize;

    if( prvContextValidate( C ) == ( bool_t ) pdFALSE )
    {
        sReturnVal = MCHP_ERR_INVALID_CONTEXT;
    }
    else if( ( ulOffset + ulBlockSize ) > ulFlashImageMaxSize )
    {   /* invalid address. */
        sReturnVal = MCHP_ERR_ADDR_OUT_OF_RANGE;
    }
    else /* Update the image offsets. */
    {
        if( ulOffset < pxCurOTADesc->ulLowImageOffset )
        {
            pxCurOTADesc->ulLowImageOffset = ulOffset;
        }

        if( ( ulOffset + ulBlockSize ) > pxCurOTADesc->ulHighImageOffset )
        {
            pxCurOTADesc->ulHighImageOffset = ulOffset + ulBlockSize;
        }

        // pointer to the new app descriptor in the flash
        const AWS_BOOT_IMAGE_DCPT*  app_new_dcpt = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_NEW_DCPT_START);
        const uint8_t* flash_address = (const uint8_t*)(app_new_dcpt) + ulOffset + AWS_NVM_PAGE_SIZE;

        if(!AWS_FlashBlockWrite(flash_address, pcData, ulBlockSize))
        {
            sReturnVal=MCHP_ERR_NOT_PENDING_COMMIT;
        }
        else
            sReturnVal=ulBlockSize;
    }
    
    // OTA_LOG_L1("[OTA-MCHP] Write Block - iOffset: 0x%08x, iBlockSize: 0x%08x\r\n", iOffset, iBlockSize);

    // all good
    return sReturnVal;
}

/**
 * @brief Closes the specified file. This will also authenticate the file if it
 * is marked as secure.
 */
OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    OTA_Err_t eResult = kOTA_Err_None;

    if( prvContextValidate( C ) == ( bool_t ) pdFALSE )
    {
        eResult = kOTA_Err_FileClose;
    }

    if( kOTA_Err_None == eResult )
    {
        /* Verify that a block has actually been written by checking that the high image offset
         *  is greater than the low image offset. If that is not the case, then an invalid memory location
         *  may get passed to CRYPTO_SignatureVerificationUpdate, resulting in a data bus error. */
        if( ( C->pxSignature != NULL ) &&
            ( pxCurOTADesc->ulHighImageOffset > pxCurOTADesc->ulLowImageOffset ) )
        {


            /* Verify the file signature, close the file and return the signature verification result. */
            eResult = prvPAL_CheckFileSignature( C );
        }
        else
        {
            eResult = kOTA_Err_SignatureCheckFailed;
        }
    }

    if( kOTA_Err_None == eResult )
    {
        /* Update the image header. */
        OTA_LOG_L1( "[%s] %s signature verification passed.\r\n", OTA_METHOD_NAME, cOTA_JSON_FileSignatureKey );

        if( prvContextUpdateImageHeaderAndTrailer( C ) == ( bool_t ) pdTRUE )
        {
            OTA_LOG_L1( "[%s] Image header updated.\r\n", OTA_METHOD_NAME );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR: Failed to update the image header.\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_FileClose;
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] ERROR: Failed to pass %s signature verification: %d.\r\n", OTA_METHOD_NAME,
                    cOTA_JSON_FileSignatureKey, eResult );
    }

    prvContextClose( C );
    return eResult;
}


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t *  C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

    OTA_Err_t eResult;
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    uint8_t * pucSignerCert = NULL;

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        eResult = kOTA_Err_SignatureCheckFailed;
    }
    else
    {
        OTA_LOG_L1( "[%s] Started %s signature verification, file: %s\r\n", OTA_METHOD_NAME,
                    cOTA_JSON_FileSignatureKey, ( const char * ) C->pucCertFilepath );
        pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const uint8_t * const ) C->pucCertFilepath, &ulSignerCertSize );

        if( pucSignerCert == NULL )
        {
            eResult = kOTA_Err_BadSignerCert;
        }
        else
        {
            const AWS_BOOT_IMAGE_DCPT*  app_new_dcpt = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_NEW_DCPT_START);
            uint8_t* pucFlashAddr = (uint8_t*)(app_new_dcpt) + pxCurOTADesc->ulLowImageOffset + AWS_NVM_PAGE_SIZE;

            CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, pucFlashAddr,
                                                pxCurOTADesc->ulHighImageOffset - pxCurOTADesc->ulLowImageOffset );

            if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                                   C->pxSignature->ucData, C->pxSignature->usSize ) == pdFALSE )
            {
                eResult = kOTA_Err_SignatureCheckFailed;

                /* Erase the image as signature verification failed.*/
                
            }
            else
            {
                eResult = kOTA_Err_None;
            }
        }
    }

    /* Free the signer certificate that we now own after prvPAL_ReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        vPortFree( pucSignerCert );
    }

    return eResult;
}


static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle )
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = strlen( pcLabelName ) + 1;
        xTemplate.pValue = ( char * ) pcLabelName;
        xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xFindInit = CK_TRUE;
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 ( CK_OBJECT_HANDLE_PTR ) pxCertHandle,
                                                 1,
                                                 &ulCount );
    }

    if( CK_TRUE == xFindInit )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/* Note that this function mallocs a buffer for the certificate to reside in,
 * and it is the responsibility of the caller to free the buffer. */
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize )
{
    /* Find the certificate */
    CK_OBJECT_HANDLE xHandle;
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR xFunctionList;
    CK_SLOT_ID xSlotId;
    CK_ULONG xCount = 1;
    CK_SESSION_HANDLE xSession;
    CK_ATTRIBUTE xTemplate = { 0 };
    uint8_t * pucCert = NULL;
    CK_BBOOL xSessionOpen = CK_FALSE;

    xResult = C_GetFunctionList( &xFunctionList );

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_Initialize( NULL );
    }

    if( ( CKR_OK == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
    {
        xResult = xFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &xCount );
    }

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession );
    }

    if( CKR_OK == xResult )
    {
        xSessionOpen = CK_TRUE;
        xResult = prvGetCertificateHandle( xFunctionList, xSession, pcLabelName, &xHandle );
    }

    if( ( xHandle != 0 ) && ( xResult == CKR_OK ) ) /* 0 is an invalid handle */
    {
        /* Get the length of the certificate */
        xTemplate.type = CKA_VALUE;
        xTemplate.pValue = NULL;
        xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

        if( xResult == CKR_OK )
        {
            pucCert = pvPortMalloc( xTemplate.ulValueLen );
        }

        if( ( xResult == CKR_OK ) && ( pucCert == NULL ) )
        {
            xResult = CKR_HOST_MEMORY;
        }

        if( xResult == CKR_OK )
        {
            xTemplate.pValue = pucCert;
            xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

            if( xResult == CKR_OK )
            {
                *ppucData = pucCert;
                *pulDataSize = xTemplate.ulValueLen;
            }
            else
            {
                vPortFree( pucCert );
            }
        }
    }
    else /* Certificate was not found. */
    {
        *ppucData = NULL;
        *pulDataSize = 0;
    }

    if( xSessionOpen == CK_TRUE )
    {
        ( void ) xFunctionList->C_CloseSession( xSession );
    }

    return xResult;
}

/* Read the specified signer certificate from the filesystem into a local buffer. The
 * allocated memory becomes the property of the caller who is responsible for freeing it.
 */
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
   DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        OTA_LOG_L1( "[%s] Using cert with label: %s OK\r\n", OTA_METHOD_NAME, ( const char * ) pucCertName );
    }
    else
    {
        OTA_LOG_L1( "[%s] No such certificate file: %s. Using aws_ota_codesigner_certificate.h.\r\n", OTA_METHOD_NAME,
                    ( const char * ) pucCertName );

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
        pucSignerCert = pvPortMalloc( ulCertSize + 1 );                       /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
            pucSignerCert[ ulCertSize ] = 0U;
            *ulSignerCertSize = ulCertSize + 1U;
        }
        else
        {
            OTA_LOG_L1( "[%s] Error: No memory for certificate of size %d!\r\n", OTA_METHOD_NAME, ulCertSize );
        }
    }

    return pucSignerCert;
}


/* Reset the device. */

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ResetDevice" );

    OTA_LOG_L1( "[%s] Resetting the device.\r\n", OTA_METHOD_NAME );

    /* Short delay for debug log output before reset. */
    vTaskDelay( OTA_HALF_SECOND_DELAY );
    NVIC_SystemReset();

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    return kOTA_Err_None;
}


/* Activate the new MCU image by resetting the device. */

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ActivateNewImage" );

    OTA_LOG_L1( "[%s] Activating the new MCU image.\r\n", OTA_METHOD_NAME );
    return prvPAL_ResetDevice();
}


/* Platform specific handling of the last transferred OTA file.
 * Commit the image if the state == eOTA_ImageState_Accepted.
 */
OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    AWS_BOOT_IMAGE_DCPT copy_dcpt;
	OTA_Err_t result = kOTA_Err_Uninitialized;
    char bTemp[AWS_NVM_PAGE_SIZE];
    const AWS_BOOT_IMAGE_DCPT*  app_run_dcptx = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_RUN_DCPT_START);  // current running app descriptor
    copy_dcpt = *app_run_dcptx;
    

    OTA_LOG_L1("[OTA-MCHP] Flash state = %d, copy_dcpt.header.img_flags = %d Error = %d\r\n",eState,copy_dcpt.header.img_flags,EFC_ErrorGet());
    
    // this should be an image launched in the test run!
    if(copy_dcpt.header.img_flags == AWS_BOOT_FLAG_IMG_TEST)
    {
        if (eState == eOTA_ImageState_Accepted)
        {   // mark the image as valid
            copy_dcpt.header.img_flags = AWS_BOOT_FLAG_IMG_VALID;
            memcpy(bTemp, (const uint8_t*)&copy_dcpt, sizeof(copy_dcpt));
            taskENTER_CRITICAL();
            AWS_FlashPagesErase((const uint32_t*)app_run_dcptx,1);
            if(AWS_FlashPagesWrite((const uint8_t*)app_run_dcptx, (const uint8_t*)bTemp, 1))
            {
                OTA_LOG_L1("[OTA-MCHP] Accepted final image. Committed.\r\n");
                result = kOTA_Err_None;
            }
            else
            {
                OTA_LOG_L1("[OTA-MCHP] Commit failed (%d).\r\n", -3);
                OTA_LOG_L1("[OTA-MCHP] Flash Erase #2 Error = %d\r\n",EFC_ErrorGet());
                result = kOTA_Err_CommitFailed | (-3 & kOTA_PAL_ErrMask);
            }
            taskEXIT_CRITICAL();
        }
        else if ( eState == eOTA_ImageState_Rejected || eState == eOTA_ImageState_Aborted)
        {
            const char* stateMsg = eState == eOTA_ImageState_Rejected ? "Rejected" : "Aborted";
            result = eState == eOTA_ImageState_Rejected ? kOTA_Err_RejectFailed : kOTA_Err_AbortFailed; 
            /* Mark the image as invalid */
            copy_dcpt.header.img_flags = AWS_BOOT_FLAG_IMG_INVALID;
            memcpy(bTemp, (const uint8_t*)&copy_dcpt, sizeof(copy_dcpt));
            if(AWS_FlashBlockWrite((const uint8_t*)app_run_dcptx, (const uint8_t*)bTemp, AWS_NVM_PAGE_SIZE))
            {
                OTA_LOG_L1( "[OTA-MCHP] %s image.\r\n", stateMsg );
                result = kOTA_Err_None;
            }
            else
            {
                OTA_LOG_L1("[OTA-MCHP] %s failed (%d).\r\n", stateMsg, -4);
                result |= (-4 & kOTA_PAL_ErrMask);
            }
        }
        else if ( eState == eOTA_ImageState_Testing )
        {
            result = kOTA_Err_None;
        }
        else
        {
            OTA_LOG_L1("[OTA-MCHP] Unknown state received %d.\r\n", ( int32_t ) eState);
            result = kOTA_Err_BadImageState;
        }
    }
    else
    {
        /* Not in self-test */
        if ( eState == eOTA_ImageState_Testing )
        {
            result = kOTA_Err_None;
        }
        else if( eState == eOTA_ImageState_Accepted)
        {
            /* We are not in self-test mode so can not set the image in upper bank as valid.  */
            OTA_LOG_L1( "[OTA-MCHP] Not in commit pending mode.\r\n");
            result = kOTA_Err_CommitFailed | (-5 & kOTA_PAL_ErrMask);
        } 
        else if( eState == eOTA_ImageState_Rejected || eState == eOTA_ImageState_Aborted)
        {
            result = kOTA_Err_None;
        } 
        else
        {
            result = kOTA_Err_BadImageState;
        }
    }

	return result;
}

/* Get the state of the currently running image.
 *
 * For the Microchip SAME70, this reads the flag bits of the MCU image
 * header and determines the appropriate state based on that.
 *
 * We read this at OTA_Init time so we can tell if the MCU image is in self
 * test mode. If it is, we expect a successful connection to the OTA services
 * within a reasonable amount of time. If we don't satisfy that requirement,
 * we assume there is something wrong with the firmware and reset the device,
 * causing it to rollback to the previous code.
 */
OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    AWS_BOOT_IMAGE_DCPT xDescCopy;
    OTA_PAL_ImageState_t eImageState = eOTA_PAL_ImageState_Unknown;
    const AWS_BOOT_IMAGE_DCPT* pxAppImgDesc;
    pxAppImgDesc = ( const AWS_BOOT_IMAGE_DCPT* )( AWS_FLASH_RUN_DCPT_START );
    xDescCopy = *pxAppImgDesc;


    switch ( xDescCopy.header.img_flags )
    {
        case AWS_BOOT_FLAG_IMG_TEST:
        {
            /* Pending Commit means we're in the Self Test phase. */
            eImageState = eOTA_PAL_ImageState_PendingCommit;
            break;
        }
        case AWS_BOOT_FLAG_IMG_VALID:
        case AWS_BOOT_FLAG_IMG_NEW:
        {
            eImageState = eOTA_PAL_ImageState_Valid;
            break;
        }
        default:
        {
            eImageState = eOTA_PAL_ImageState_Invalid;
            break;
        }
    }
    return eImageState;
}

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
