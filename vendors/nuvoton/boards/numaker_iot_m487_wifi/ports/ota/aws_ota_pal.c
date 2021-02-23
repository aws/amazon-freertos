/*
 * FreeRTOS OTA PAL for Nuvoton Numaker-IoT-M487
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* OTA PAL implementation for M487 platform. */

#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "core_pkcs11.h"
#include "iot_crypto.h"
#include "aws_iot_ota_pal.h"
#include "core_pkcs11_config.h"
#include "aws_ota_codesigner_certificate.h"
#include "NuMicro.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

/* definitions shared with the resident bootloader. */
#define AWS_BOOT_IMAGE_SIGNATURE         "@AFRTOS"
#define AWS_BOOT_IMAGE_SIGNATURE_SIZE    ( 7U )

/* PAL error codes. */

#define AWS_BOOT_FLAG_IMG_NEW               0xffU /* 11111111b A new image that hasn't yet been run. */
#define AWS_BOOT_FLAG_IMG_PENDING_COMMIT    0xfeU /* 11111110b Image is pending commit and is ready for self test. */
#define AWS_BOOT_FLAG_IMG_VALID             0xfcU /* 11111100b The image was accepted as valid by the self test code. */
#define AWS_BOOT_FLAG_IMG_INVALID           0xf8U /* 11111000b The image was NOT accepted by the self test code. */

#define NVT_OTA_META_BASE                   ( 0x70000UL )      /* OTA meta data storage start address in APROM  */
#define NVT_BOOT_IMG_HEAD_BASE              (NVT_OTA_META_BASE)
#define NVT_BOOT_IMG_TRAIL_BASE             (NVT_OTA_META_BASE + FMC_FLASH_PAGE_SIZE)

/*
 * Image Header.
 */

typedef union
{
    uint32_t ulAlign[ 2 ]; /* Force image header to be 8 bytes. */
#if defined(__CC_ARM)
#pragma anon_unions
#endif
    struct
    {
        char cImgSignature[ AWS_BOOT_IMAGE_SIGNATURE_SIZE ]; /* Signature identifying a valid application: AWS_BOOT_IMAGE_SIGNATURE. */
        uint8_t ucImgFlags;                                  /* Flags from the AWS_BOOT_IMAGE_FLAG_IMG*, above. */
    };                                                       
} BootImageHeader_t;


/* Boot application image descriptor.
 * Total size is 32 bytes (NVM programming does 4 bytes at a time)
 * This is the descriptor used by the bootloader
 * to maintain the application images.
 */
typedef struct
{
    BootImageHeader_t xImgHeader; /* Application image header (8 bytes). */
    uint32_t ulSequenceNum;       /* OTA sequence number. Higher is newer. */
    /* Use byte pointers for image addresses so pointer math doesn't use incorrect scalars. */
    const uint8_t * pvStartAddr;  /* Image start address. */
    const uint8_t * pvEndAddr;    /* Image end address. */
    const uint8_t * pvExecAddr;   /* Execution start address. */
    uint32_t ulHardwareID;        /* Unique Hardware ID. */
    uint32_t ulReserved;          /* Reserved. *//*lint -e754 -e830 intentionally unreferenced alignment word. */
} BootImageDescriptor_t;

//static BootImageDescriptor_t xCurBootImgDesc;         /* current Boot Image in progress. */

/*
 * Image Trailer.
 */
typedef struct
{
    uint8_t aucSignatureType[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ]; /* Signature Type. */
    uint32_t ulSignatureSize;                                    /* Signature size. */
    uint8_t aucSignature[ kOTA_MaxSignatureSize ];               /* Signature */
} BootImageTrailer_t;

typedef struct
{
    const OTA_FileContext_t * pxCurOTAFile; /* Current OTA file to be processed. */
    uint32_t ulImageOffset;              /* offset/address of the application image. */
} OTA_OperationDescriptor_t;

/* NOTE that this implementation supports only one OTA at a time since it uses a single static instance. */
static OTA_OperationDescriptor_t xCurOTAOpDesc;         /* current OTA operation in progress. */
static OTA_OperationDescriptor_t * pxCurOTADesc = NULL; /* pointer to current OTA operation. */

static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C );
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/*-----------------------------------------------------------*/

static __inline bool_t prvContextValidate( OTA_FileContext_t * C )
{
    return( ( pxCurOTADesc != NULL ) && ( C != NULL ) &&
            ( pxCurOTADesc->pxCurOTAFile == C ) &&
            ( C->pucFile == ( uint8_t * ) pxCurOTADesc ) ); /*lint !e9034 This preserves the abstraction layer. */
}

static __inline void prvContextClose( OTA_FileContext_t * C )
{
    if( NULL != C )
    {
        C->pucFile = NULL;
    }

    xCurOTAOpDesc.pxCurOTAFile = NULL;
    pxCurOTADesc = NULL;
}

static bool_t prvFLASH_update(uint32_t u32StartAddr, uint8_t * pucData, uint32_t ulDataSize)
{
    uint32_t    u32Addr;               /* flash address */
    uint32_t    u32data;               /* flash data    */
    uint32_t    *pDataSrc;             /* flash data    */    
    uint32_t    u32EndAddr = (u32StartAddr + ulDataSize);
    uint32_t    u32Pattern = 0xFFFFFFFF;
    bool_t result = pdFALSE;
    
    DEFINE_OTA_METHOD_NAME( "prvFLASH_update" );

    /* Unlock protected registers */
    SYS_UnlockReg();
    FMC_Open();                        /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */
    FMC_Erase(u32StartAddr);    
     /* Verify if each data word from flash u32StartAddr to u32EndAddr be 0xFFFFFFFF.  */
    for (u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        u32data = FMC_Read(u32Addr);   /* Read a flash word from address u32Addr. */

        if (u32data != u32Pattern )     /* Verify if data matched. */
        {
            OTA_LOG_L1( "[%s] FMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", OTA_METHOD_NAME, u32Addr, u32data, u32Pattern);   
            result = pdFALSE;                 /* data verify failed */
            goto lexit;
        }
    }
    
    pDataSrc = (uint32_t *) pucData;
    /* Fill flash range from u32StartAddr to u32EndAddr */
    for (u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        FMC_Write(u32Addr, *pDataSrc);          /* Program flash */
        //printf("#### FMC write: 0x%x, val:0x%x \n", u32Addr, *pDataSrc);
        pDataSrc++;
    }    
    result = pdTRUE; 
    
lexit:   
    FMC_DISABLE_AP_UPDATE();           /* Disable APROM update. */
    FMC_Close();                       /* Disable FMC ISP function */
    /* Lock protected registers */
    SYS_LockReg();   
    
    return result;
    
}

static bool_t prvSPI_FLASH_EraseBank()
{
#define SPI_BANK_START  0x00
#define SPI_BANK_SIZE   0x80000
  bool_t result = pdFALSE;
  // FIX ME
  /* Erase SPI flash 512KB bank before program. */
  result = NVT_SPI_Flash_Bank_Erase(SPI_BANK_START, SPI_BANK_SIZE);
  return result;
}

static bool_t prvContextUpdateImageHeaderAndTrailer( OTA_FileContext_t * C )
{    
    DEFINE_OTA_METHOD_NAME( "prvContextUpdateImageHeaderAndTrailer" );

    bool_t bProgResult;
    BootImageHeader_t xImgHeader;
    BootImageDescriptor_t * pxImgDesc;
    BootImageDescriptor_t xImgDesc;    
    BootImageTrailer_t xImgTrailer;

    /* Pointer to the boot image header in the flash. */
    pxImgDesc = ( BootImageDescriptor_t * ) NVT_BOOT_IMG_HEAD_BASE;
    xImgDesc = *pxImgDesc;
    xImgDesc.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_NEW;
    
    /* Write Boot header to flash. */
    bProgResult = prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, 
                                    (uint8_t *)&xImgDesc, 
                                    sizeof( BootImageDescriptor_t) );
    
    OTA_LOG_L1( "[%s] OTA Sequence Number: %d\r\n", OTA_METHOD_NAME, pxImgDesc->ulSequenceNum );
    OTA_LOG_L1( "[%s] Image - Start: 0x%08x, End: 0x%08x\r\n", OTA_METHOD_NAME,
                pxImgDesc->pvStartAddr, pxImgDesc->pvEndAddr );

    /* If header write is successful write trailer. */
    if( bProgResult )
    {
        /* Create image trailer. */
        memcpy( xImgTrailer.aucSignatureType, cOTA_JSON_FileSignatureKey, sizeof( cOTA_JSON_FileSignatureKey ) );
        xImgTrailer.ulSignatureSize = C->pxSignature->usSize;
        memcpy( xImgTrailer.aucSignature, C->pxSignature->ucData, C->pxSignature->usSize );

        /* Write trailer to flash. */
        bProgResult = prvFLASH_update(NVT_BOOT_IMG_TRAIL_BASE, (uint8_t *)&xImgTrailer, sizeof( xImgTrailer));        
        OTA_LOG_L1( "[%s] Writing Trailer at: 0x%08x\n", OTA_METHOD_NAME, NVT_BOOT_IMG_TRAIL_BASE );
    }

    return bProgResult;
}



/* Attempt to create a new receive file for the file chunks as they come in. */

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );

    OTA_Err_t eResult = kOTA_Err_Uninitialized; /* For MISRA mandatory. */

    if( C != NULL )
    {
        if( NVT_SPI_Flash_Init() == ( bool_t ) pdFALSE )
        {
            eResult = kOTA_Err_RxFileCreateFailed;
            OTA_LOG_L1( "[%s] ERROR - Failed to init SPI Flash.\r\n", OTA_METHOD_NAME );
            return eResult;
        }
         
        /* Erase SPI flash 512KB bank before program. */
        if( prvSPI_FLASH_EraseBank() == ( bool_t ) pdFALSE )
        {
            OTA_LOG_L1( "[%s] Error: Failed to erase the flash!\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_RxFileCreateFailed;
        }
        else
        {
            pxCurOTADesc = &xCurOTAOpDesc;
            pxCurOTADesc->pxCurOTAFile = C;
            pxCurOTADesc->ulImageOffset = 0;
            OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
            C->pucFile = ( uint8_t * ) pxCurOTADesc;
            
            /* Update Boot Descriptor */
            BootImageDescriptor_t xDescCopy;
            memcpy(xDescCopy.xImgHeader.cImgSignature, AWS_BOOT_IMAGE_SIGNATURE, AWS_BOOT_IMAGE_SIGNATURE_SIZE);
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_INVALID;
            xDescCopy.pvStartAddr = 0x00;
            xDescCopy.pvEndAddr = xDescCopy.pvStartAddr + C->ulFileSize -1;
            xDescCopy.pvExecAddr = 0x00;

            if( pdFALSE == prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                            sizeof( BootImageDescriptor_t) ) )
            {
                OTA_LOG_L1( "[%s] ERROR - FMC write failed\r\n", OTA_METHOD_NAME );
            } 
            else 
            {
            
                eResult = kOTA_Err_None;
            }
        }
    }
    else
    {
        eResult = kOTA_Err_RxFileCreateFailed;
        OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
    }

    return eResult; /*lint !e480 !e481 Exiting function without calling fclose.
                     * Context file handle state is managed by this API. */
}


/* Abort receiving the specified OTA update by closing the file. */

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );

    /* Check for null file handle since we may call this before a file is actually opened. */
    prvContextClose( C );
    OTA_LOG_L1( "[%s] Abort - OK\r\n", OTA_METHOD_NAME );

    return kOTA_Err_None;
}

/* Write a block of data to the specified file. 
   Returns the number of bytes written on success or negative error code.
*/
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );

    int32_t lResult = 0;

    if( prvContextValidate( C ) == pdTRUE )
    {
#if 0
        /* If execute image not generate boot-descriptor by utility, 1st block will have no boot-desc info */
        /* OTA image 1st block contain Boot Image Descriptor info */
        if( ulOffset == 0 ) 
        {
            BootImageDescriptor_t xDescCopy;
            const BootImageDescriptor_t * pxAppImgDesc;
            pxAppImgDesc = ( BootImageDescriptor_t * ) NVT_BOOT_IMG_HEAD_BASE;
            xDescCopy = *pxAppImgDesc;           /* Copy image descriptor from flash into RAM struct. */
            memcpy((uint8_t *)&(xDescCopy.ulSequenceNum), pacData, 
            sizeof(xDescCopy) - sizeof(BootImageHeader_t) );
            if( pdFALSE == prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                            sizeof( BootImageDescriptor_t) ) )
            {
                OTA_LOG_L1( "[%s] ERROR - FMC write failed\r\n", OTA_METHOD_NAME );
            }
        }
#endif        
        lResult = NVT_SPI_Flash_Block_Write(ulOffset, pacData, ulBlockSize);

        if( lResult < 0 )
        {
                OTA_LOG_L1( "[%s] ERROR - SPI Flash write failed\r\n", OTA_METHOD_NAME );
                /* Mask to return a negative value. */
                lResult = -1; /*lint !e40 !e9027
                                                                * Errno is being used in accordance with host API documentation.
                                                                * Bitmasking is being used to preserve host API error with library status code. */
        }
    }
    else /* Invalid context or file pointer provided. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        lResult = -1; /*TODO: Need a negative error code from the PAL here. */
    }

    return ( int16_t ) lResult;
}

/* Close the specified file. This shall authenticate the file if it is marked as secure. */

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    OTA_Err_t eResult = kOTA_Err_None;
    int32_t lWindowsError = 0;

    if( prvContextValidate( C ) == pdTRUE )
    {
        if( C->pxSignature != NULL )
        {
            /* Verify the file signature, close the file and return the signature verification result. */
            eResult = prvPAL_CheckFileSignature( C );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - NULL OTA Signature structure.\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_SignatureCheckFailed;
        }

        if( eResult == kOTA_Err_None )
        {
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
            OTA_LOG_L1( "[%s] ERROR - Failed to pass %s signature verification: %d.\r\n", OTA_METHOD_NAME,
                        cOTA_JSON_FileSignatureKey, eResult );

			/* If we fail to verify the file signature that means the image is not valid. We need to set the image state to aborted. */
			prvPAL_SetPlatformImageState( eOTA_ImageState_Aborted );

        }
    }
    else /* Invalid OTA Context. */
    {
        /* FIXME: Invalid error code for a null file context and file handle. */
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileClose;
    }
    
    prvContextClose( C );
    return eResult;
}


/* Verify the signature of the specified file. */

static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
#ifdef __ICCARM__
#pragma data_alignment=4
    uint8_t  ucBuff[512];
#else
    uint8_t  ucBuff[512] __attribute__((aligned(4)));
#endif

    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

    OTA_Err_t eResult = kOTA_Err_None;
    uint32_t ulBytesRead, ulOffset;
    uint32_t ulSignerCertSize;
    uint8_t * pucBuf, * pucSignerCert;
    void * pvSigVerifyContext;

    if( prvContextValidate( C ) == pdTRUE )
    {
        /* Verify an ECDSA-SHA256 signature. */
        if( pdFALSE == CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256 ) )
        {
            eResult = kOTA_Err_SignatureCheckFailed;
        }
        else
        {
            OTA_LOG_L1( "[%s] Started %s signature verification, file: %s\r\n", OTA_METHOD_NAME,
                        cOTA_JSON_FileSignatureKey, ( const char * ) C->pucCertFilepath );
            pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const uint8_t * const ) C->pucCertFilepath, &ulSignerCertSize );

            if( pucSignerCert != NULL )
            {
                /* if ulFileSize not correct, alternative by  xCurBootImgDesc.pvEndAddr - prvStAddr + (sizeof( BootImageDescriptor_t) - sizeof( BootImageHeader_t)) */
                for( ulOffset=0; ulOffset < C->ulFileSize; ulOffset+= sizeof(ucBuff) )
                {
                    if( (C->ulFileSize - ulOffset) > sizeof(ucBuff) )
                        ulBytesRead = NVT_SPI_Flash_Block_Read(ulOffset, ucBuff, sizeof(ucBuff));
                    else
                        ulBytesRead = NVT_SPI_Flash_Block_Read(ulOffset, ucBuff, (C->ulFileSize - ulOffset));
                    /* Include the file chunk in the signature validation. Zero size is OK. */
                    CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, ucBuff, ulBytesRead );
                }

                if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert,
                                                       ulSignerCertSize, C->pxSignature->ucData, 
                                                       C->pxSignature->usSize ) == pdFALSE )
                {
                    eResult = kOTA_Err_SignatureCheckFailed;
                    OTA_LOG_L1( "[%s] ERROR - Signature Verification Failed.\r\n", OTA_METHOD_NAME );
                    /* Erase the image as signature verification failed.*/
//                    prvSPI_FLASH_EraseBank();
                }
                else
                {
                    eResult = kOTA_Err_None;
                }
 
            }
            else
            {
                eResult = kOTA_Err_BadSignerCert;
            }
        }
    }
    else
    {
        /* FIXME: Invalid error code for a NULL file context. */
        OTA_LOG_L1( "[%s] ERROR - Invalid OTA file context.\r\n", OTA_METHOD_NAME );
        /* Invalid OTA context or file pointer. */
        eResult = kOTA_Err_NullFilePtr;
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


/* Read the specified signer certificate from the FMC into a local buffer. The allocated
 * memory becomes the property of the caller who is responsible for freeing it.
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


/*-----------------------------------------------------------*/
#define OTA_DELAY       ( ( portTickType ) 1000 / portTICK_RATE_MS )

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ResetDevice" );
    
    portTickType xLastExecutionTime = xTaskGetTickCount();
    OTA_LOG_L1( "[%s] Resetting the device.\r\n", OTA_METHOD_NAME );

    /* Short delay for debug log output before reset. */
    vTaskDelayUntil( &xLastExecutionTime, OTA_DELAY );

    /* Unlock protected registers before setting Brown-out detector function and Power-down mode */    
    SYS_UnlockReg();
    FMC_Open();
    // Boot From LD-ROM
    /*
        CONFIG0[7:6]
        00 = Boot from LDROM with IAP mode.
        01 = Boot from LDROM without IAP mode.
        10 = Boot from APROM with IAP mode.
        11 = Boot from APROM without IAP mode.
    */
    uint32_t  au32Config[2];
    if( FMC_ReadConfig(au32Config, 2) < 0)
    {
          OTA_LOG_L1( " Read FMC config failed.\r\n");
    }
    if( (au32Config[0] & 0x40) )        /* Check if it's boot from APROM/LDROM with IAP. */
    {
        FMC_ENABLE_CFG_UPDATE();       /* Enable User Configuration update. */
        au32Config[0] &= ~0x40;        /* Select IAP boot mode. */
        FMC_WriteConfig(au32Config, 2);/* Update User Configuration CONFIG0 and CONFIG1. */
        SYS_ResetChip();    /* Perform chip reset to make new User Config take effect. */
    }
    /* Remap to LD-ROM*/
    FMC_SetVectorPageAddr(FMC_LDROM_BASE);    // Remap to LD-ROM address       
    SYS_ResetCPU();
    while(1);
    
    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    return kOTA_Err_None;
}

/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ActivateNewImage" );

    OTA_LOG_L1( "[%s] Activating the new MCU image.\r\n", OTA_METHOD_NAME );
    return prvPAL_ResetDevice();
}


/*
 * Set the final state of the last transferred (final) OTA file (or bundle).
 * The state of the OTA image is stored in FMC header.
 */

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );
    BootImageDescriptor_t xDescCopy;
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    /* Descriptor handle for the image. */
    const BootImageDescriptor_t * pxAppImgDesc;
    pxAppImgDesc = ( BootImageDescriptor_t * ) NVT_BOOT_IMG_HEAD_BASE;
    xDescCopy = *pxAppImgDesc;                    /* Copy image descriptor from flash into RAM struct. */

    
    if( (eState == eOTA_ImageState_Unknown) || (eState > eOTA_LastImageState) )
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid image state provided.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_BadImageState;
    } /*lint !e481 Allow fopen and fclose calls in this context. */
    else /* Image state valid. */    
    /* This should be an image launched in self test mode! */
//    if( xDescCopy.xImgHeader.ucImgFlags == AWS_BOOT_FLAG_IMG_PENDING_COMMIT )
    {
        if( eState == eOTA_ImageState_Accepted )
        {
            /* Mark the image as valid */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_VALID;

            if( (xCurOTAOpDesc.pxCurOTAFile == NULL) &&
                (prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == ( bool_t ) pdTRUE) )
            {
                OTA_LOG_L1( "[%s] Accepted and committed final image.\r\n", OTA_METHOD_NAME );
                eResult = kOTA_Err_None;
            }
            else
            {
                OTA_LOG_L1( "[%s] Accepted final image but commit failed (%d).\r\n", OTA_METHOD_NAME);
                eResult = ( uint32_t ) kOTA_Err_CommitFailed;
            }
        }
        else if( eState == eOTA_ImageState_Rejected )
        {
            /* Mark the image as invalid */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_INVALID;

            if( prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == ( bool_t ) pdTRUE )
            {
                OTA_LOG_L1( "[%s] Rejected image.\r\n", OTA_METHOD_NAME );

                eResult = kOTA_Err_None;
            }
            else
            {
                OTA_LOG_L1( "[%s] Failed updating the flags.\r\n", OTA_METHOD_NAME );
                eResult = ( uint32_t ) kOTA_Err_RejectFailed;
            }
        }
        else if( eState == eOTA_ImageState_Aborted )
        {
            /* Mark the image as invalid */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_INVALID;

            if( prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == ( bool_t ) pdTRUE )
            {
                OTA_LOG_L1( "[%s] Aborted image.\r\n", OTA_METHOD_NAME );

                eResult = kOTA_Err_None;
            }
            else
            {
                OTA_LOG_L1( "[%s] Failed updating the flags.\r\n", OTA_METHOD_NAME);
                eResult = ( uint32_t ) kOTA_Err_AbortFailed ;
            }
        }
        else if( eState == eOTA_ImageState_Testing )
        {
            eResult = kOTA_Err_None;
        }
        else
        {
            OTA_LOG_L1( "[%s] Unknown state received %d.\r\n", OTA_METHOD_NAME, ( int32_t ) eState );
            eResult = kOTA_Err_BadImageState;
        }
    }

    OTA_LOG_L1( "[%s] image state [%d] ---Flag[0x%x].\r\n", OTA_METHOD_NAME, eState, xDescCopy.xImgHeader.ucImgFlags);
    return eResult;
}

/* Get the state of the currently running image.
 *
 * This is simulated by looking for and reading the state from
 * FMC Boot Image Header.
 *
 * We read this at OTA_Init time so we can tell if the MCU image is in self
 * test mode. If it is, we expect a successful connection to the OTA services
 * within a reasonable amount of time. If we don't satisfy that requirement,
 * we assume there is something wrong with the firmware and reset the device,
 * causing it to rollback to the previous code. On Windows, this is not
 * fully simulated as there is no easy way to reset the simulated device.
 */
OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    BootImageDescriptor_t xDescCopy;
    OTA_PAL_ImageState_t eImageState = eOTA_PAL_ImageState_Unknown; //eOTA_PAL_ImageState_Invalid;
    const BootImageDescriptor_t * pxAppImgDesc;
    DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );

    pxAppImgDesc = ( const BootImageDescriptor_t * ) NVT_BOOT_IMG_HEAD_BASE; /*lint !e923 !e9027 !e9029 !e9033 !e9079 !e9078 !e9087 Please see earlier lint comment header. */
    xDescCopy = *pxAppImgDesc;

    /**
     *  Check if valid magic code is present for the application image.
     */
    if( memcmp( pxAppImgDesc->xImgHeader.cImgSignature,
                AWS_BOOT_IMAGE_SIGNATURE,
                AWS_BOOT_IMAGE_SIGNATURE_SIZE ) == 0 )
    {

        switch( xDescCopy.xImgHeader.ucImgFlags )
        {
            case AWS_BOOT_FLAG_IMG_PENDING_COMMIT:
                eImageState = eOTA_PAL_ImageState_PendingCommit;
                break;
            case AWS_BOOT_FLAG_IMG_VALID:
            case AWS_BOOT_FLAG_IMG_NEW:
                eImageState = eOTA_PAL_ImageState_Valid;
                break;

            default:
                eImageState = eOTA_PAL_ImageState_Invalid;
                break;
        }
            
        
    }
    OTA_LOG_L1( "[%s] image state [%d] -- Flag[0x%x].\r\n", OTA_METHOD_NAME, eImageState, xDescCopy.xImgHeader.ucImgFlags);
    return eImageState;
}

/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef FREERTOS_ENABLE_UNIT_TESTS
#include "aws_ota_pal_test_access_define.h"
#endif
