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
#include "task.h"
#include "core_pkcs11.h"
#include "iot_crypto.h"
#include "ota.h"
#include "ota_interface_private.h"
#include "ota_platform_interface.h"
#include "ota_pal.h"
#include "core_pkcs11_config.h"
#include "ota_config.h"

#include "NuMicro.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char OTA_JsonFileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";
static const char codeSigningCertificatePEM[] = otapalconfigCODE_SIGNING_CERTIFICATE;

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
    const OtaFileContext_t * pxCurOTAFile; /* Current OTA file to be processed. */
    uint32_t ulImageOffset;              /* offset/address of the application image. */
} OTA_OperationDescriptor_t;

/* NOTE that this implementation supports only one OTA at a time since it uses a single static instance. */
static OTA_OperationDescriptor_t xCurOTAOpDesc;         /* current OTA operation in progress. */
static OTA_OperationDescriptor_t * pxCurOTADesc = NULL; /* pointer to current OTA operation. */

static OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const C );
static uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/*-----------------------------------------------------------*/
static __inline bool prvContextValidate( OtaFileContext_t * C )
{
    return( ( pxCurOTADesc != NULL ) && ( C != NULL ) &&
            ( pxCurOTADesc->pxCurOTAFile == C ) &&
            ( C->pFile == ( uint8_t * ) pxCurOTADesc ) ); /*lint !e9034 This preserves the abstraction layer. */
}


static __inline void prvContextClose( OtaFileContext_t * C )
{
    if( NULL != C )
    {
        C->pFile = NULL;
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
            LogError(( "[%s] FMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", __FUNCTION__, u32Addr, u32data, u32Pattern));   
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


#define OTA_SPI_BANK_START  0x00
#define OTA_SPI_BANK_SIZE   0x80000

static bool_t prvSPI_FLASH_EraseBank()
{

  bool_t result = pdFALSE;
  // FIX ME
  /* Erase SPI flash 512KB bank before program. */
  result = NVT_SPI_Flash_Bank_Erase(OTA_SPI_BANK_START, OTA_SPI_BANK_SIZE);
  return result;
}

#define ROUND_SIZE(a, b)            (((a + b - 1) / b) * b)
/* To use 2nd SPI bank to backup current active image */
static bool prvSPI_FLASH_BackupBank(uint32_t dstAddress, uint32_t srcAddress, uint32_t srcSize)
{
  int spiBlockSzie =  (4*1024);     // W25Q32 block size is 4KB, chip size is 4MB
  uint32_t  startAddress = dstAddress + spiBlockSzie;
  uint32_t  bankSize = ROUND_SIZE(srcSize, spiBlockSzie);
  bool      result = false;
  int       ret;

  BootImageDescriptor_t xDescImage;
  memcpy(xDescImage.xImgHeader.cImgSignature, AWS_BOOT_IMAGE_SIGNATURE, AWS_BOOT_IMAGE_SIGNATURE_SIZE);
  xDescImage.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_VALID;
  xDescImage.pvStartAddr = 0x00;
  xDescImage.pvEndAddr = xDescImage.pvStartAddr + (srcSize - 1);
  xDescImage.pvExecAddr = 0x00;
  xDescImage.ulReserved = startAddress; /* Backup image's SPI Flash start address */

    /*
     *  Erase flash page
     */
    if( (startAddress + bankSize) > (OTA_SPI_BANK_SIZE*2) )  
    {
        LogError(("[%s] FAILED!\n", __FUNCTION__));
        LogError(("[%s] Exceed reserved Flash Bank boundary 0x%x > 0x%x \n", __FUNCTION__, (startAddress + bankSize), (OTA_SPI_BANK_SIZE*2)));
        goto lexit;
    }
    LogInfo(("[%s] Erase SPI flash bank 0x%x ~ 0x%x ...", __FUNCTION__, dstAddress, (startAddress + bankSize)));
    if( NVT_SPI_Flash_Bank_Erase(dstAddress, bankSize + spiBlockSzie) == pdTRUE )
    {
        result = true;
        LogInfo(("[%s] done.\n", __FUNCTION__));
    } else {
       result = false;
       LogError(("[%s] fail.\n", __FUNCTION__));
       goto lexit;
    }
    /*
     *  Copy src image to SPI flash
     */
    LogInfo(("[%s] Backup src image to SPI flash backup-bank 0x%x from 0x%x...", __FUNCTION__, startAddress, srcAddress));
    
    uint32_t ulOffset;
    uint8_t *pacData;   

    for (ulOffset = 0; ulOffset < srcSize; ulOffset += spiBlockSzie )
    {
        pacData = (uint8_t *)srcAddress + ulOffset;
        if( NVT_SPI_Flash_Block_Write( startAddress + ulOffset, pacData, spiBlockSzie) < 0 )
        {
            LogError(( "[%s] ERROR - SPI Flash write image failed\r\n", __FUNCTION__ ));
            result = false;
            goto lexit;
        }
    }
    
    /* Write Image Meta data in 1st block of backup bank */
    if( NVT_SPI_Flash_Block_Write( dstAddress, (const uint8_t*)&xDescImage, sizeof(BootImageDescriptor_t)) < 0 )
    {
        LogError(( "[%s] ERROR - SPI Flash write Image header failed\r\n", __FUNCTION__ ));
        result = false;
        goto lexit;
    }

    /* Mark FMC image-header ulReserved field as SPI address of backup image */
    const BootImageDescriptor_t * pxAppImgDesc;
    BootImageDescriptor_t xDescCopy;
    pxAppImgDesc = ( BootImageDescriptor_t * ) NVT_BOOT_IMG_HEAD_BASE;
    xDescCopy = *pxAppImgDesc;                    /* Copy image descriptor from flash into RAM structure. */
    xDescCopy.ulReserved = dstAddress; /* Backup bank's SPI Flash start address */
    if( prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy,
                        sizeof( BootImageDescriptor_t) ) == true )
    {
        LogInfo(( "[%s] Mark backup image address.\r\n", __FUNCTION__ ));
        result = true;
    }

lexit:
  return result;
}

static bool_t prvContextUpdateImageHeaderAndTrailer( OtaFileContext_t * C )
{    

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
    
    LogInfo(( "[%s] OTA Sequence Number: %d\r\n", __FUNCTION__, pxImgDesc->ulSequenceNum ));
    LogInfo(( "[%s] Image - Start: 0x%08x, End: 0x%08x\r\n", __FUNCTION__,
                pxImgDesc->pvStartAddr, pxImgDesc->pvEndAddr ));

    /* If header write is successful write trailer. */
    if( bProgResult )
    {
        /* Create image trailer. */
        memcpy( xImgTrailer.aucSignatureType, OTA_JsonFileSignatureKey, sizeof( OTA_JsonFileSignatureKey ) );
        xImgTrailer.ulSignatureSize =  C->pSignature->size;
        memcpy( xImgTrailer.aucSignature, C->pSignature->data, C->pSignature->size );

        /* Write trailer to flash. */
        bProgResult = prvFLASH_update(NVT_BOOT_IMG_TRAIL_BASE, (uint8_t *)&xImgTrailer, sizeof( xImgTrailer));        
        LogInfo(( "[%s] Writing Trailer at: 0x%08x\n", __FUNCTION__, NVT_BOOT_IMG_TRAIL_BASE ));
    }

    return bProgResult;
}



/* Attempt to create a new receive file for the file chunks as they come in. */

OtaPalStatus_t otaPal_CreateFileForRx( OtaFileContext_t * const C )
{
    OtaPalStatus_t result = OTA_PAL_COMBINE_ERR( OtaPalUninitialized, 0 );

    if( C != NULL )
    {
        if( NVT_SPI_Flash_Init() == ( bool_t ) pdFALSE )
        {
            result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, -1 );
            LogError( ( "[%s] ERROR - Failed to init SPI Flash.\r\n", __FUNCTION__) );
            return result;
        }
         
        /* Erase SPI flash 512KB bank before program. */
        if( prvSPI_FLASH_EraseBank() == false )
        {
            LogError(( "[%s] Error: Failed to erase the flash!\r\n", __FUNCTION__ ));
            result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, -1 );
        }
        else
        {
            pxCurOTADesc = &xCurOTAOpDesc;
            pxCurOTADesc->pxCurOTAFile = C;
            pxCurOTADesc->ulImageOffset = 0;
            LogInfo( ("[%s] Receive file created.\r\n", __FUNCTION__ ));
            C->pFile = ( uint8_t * ) pxCurOTADesc;
            
            /* Update Boot Descriptor */
            BootImageDescriptor_t xDescCopy;
            memcpy(xDescCopy.xImgHeader.cImgSignature, AWS_BOOT_IMAGE_SIGNATURE, AWS_BOOT_IMAGE_SIGNATURE_SIZE);
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_INVALID;
            xDescCopy.pvStartAddr = 0x00;
            xDescCopy.pvEndAddr = xDescCopy.pvStartAddr + C->fileSize -1;
            xDescCopy.pvExecAddr = 0x00;
            xDescCopy.ulReserved = 0x00;

            if( false == prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                            sizeof( BootImageDescriptor_t) ) )
            {
                LogError(( "[%s] ERROR - FMC write failed\r\n", __FUNCTION__ ));
            } 
            else 
            {            
                result = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
            }
        }
    }
    else
    {
        result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, -1 );
        LogError(( "[%s] ERROR - Invalid context provided.\r\n", __FUNCTION__ ));
    }

    return result; /*lint !e480 !e481 Exiting function without calling fclose.
                     * Context file handle state is managed by this API. */
}


/* Abort receiving the specified OTA update by closing the file. */

OtaPalStatus_t otaPal_Abort( OtaFileContext_t * const C )
{
    OtaPalMainStatus_t mainErr = OtaPalUninitialized;
    int32_t subErr = 0;

    if( NULL != C )
    {
        /* Check for null file handle since we may call this before a file is actually opened. */
        prvContextClose( C );
        mainErr = OtaPalSuccess;
        LogInfo(( "[%s] Abort - OK\r\n", __FUNCTION__ ));
    }
    else /* Context was not valid. */
    {
        LogError(( "[%s] Abort Input Context is NULL - Fail\r\n", __FUNCTION__ ));
        mainErr = OtaPalFileAbort;
    }
    
    return OTA_PAL_COMBINE_ERR( mainErr, subErr );
}


/* Write a block of data to the specified file. 
   Returns the number of bytes written on success or negative error code.
*/
int16_t otaPal_WriteBlock( OtaFileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{

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
                LogError(( "[%s] ERROR - FMC write failed\r\n", __FUNCTION__ ));
            }
        }
#endif        
        lResult = NVT_SPI_Flash_Block_Write(ulOffset, pacData, ulBlockSize);

        if( lResult < 0 )
        {
                LogError(( "[%s] ERROR - SPI Flash write failed\r\n", __FUNCTION__ ));
                /* Mask to return a negative value. */
                lResult = -1; /*lint !e40 !e9027
                                                                * Errno is being used in accordance with host API documentation.
                                                                * Bitmasking is being used to preserve host API error with library status code. */
        }
    }
    else /* Invalid context or file pointer provided. */
    {
        LogError(( "[%s] ERROR - Invalid context.\r\n", __FUNCTION__ ));
        lResult = -1; /*TODO: Need a negative error code from the PAL here. */
    }

    return ( int16_t ) lResult;
}

/* Close the specified file. This shall authenticate the file if it is marked as secure. */

OtaPalStatus_t otaPal_CloseFile( OtaFileContext_t * const C )
{
    OtaPalMainStatus_t mainErr = OtaPalSuccess;
    OtaPalSubStatus_t subErr = 0;
    OtaPalStatus_t result;
    
    if( prvContextValidate( C ) == pdTRUE )
    {
        if( C->pSignature != NULL )
        {
            /* Verify the file signature, close the file and return the signature verification result. */
            result = otaPal_CheckFileSignature( C );
            mainErr = OTA_PAL_MAIN_ERR( result );
            subErr = OTA_PAL_SUB_ERR( result );
        }
        else
        {
            LogError(( "[%s] ERROR - NULL OTA Signature structure.\r\n", __FUNCTION__ ));
            mainErr = OtaPalSignatureCheckFailed;
        }

        if( mainErr == OtaPalSuccess )
        {
            LogInfo(( "[%s] %s signature verification passed.\r\n", __FUNCTION__, OTA_JsonFileSignatureKey ));
            /* AFR implementation not set any image state here, just state keep on AWS_BOOT_FLAG_IMG_NEW */
            //( void ) otaPal_SetPlatformImageState( C, OtaImageStateTesting );
            /* Update Image state as  AWS_BOOT_FLAG_IMG_NEW and store Signature data */
            if( prvContextUpdateImageHeaderAndTrailer( C ) == true ) 
            {
                LogInfo(( "[%s] Image header updated.\r\n", __FUNCTION__ ));
            }
            else
            {
                LogInfo(( "[%s] ERROR: Failed to update the image header.\r\n", __FUNCTION__ ));
                mainErr = OtaPalFileClose;
            }
        }
        else
        {
            LogError(( "[%s] ERROR - Failed to pass %s signature verification: %d.\r\n", __FUNCTION__,
                        OTA_JsonFileSignatureKey, OTA_PAL_COMBINE_ERR( mainErr, subErr ) ));

			/* If we fail to verify the file signature that means the image is not valid. We need to set the image state to aborted. */
            ( void ) otaPal_SetPlatformImageState( C, OtaImageStateAborted );
        }
    }
    else /* Invalid OTA Context. */
    {
        /* FIXME: Invalid error code for a null file context and file handle. */
        LogInfo(( "[%s] ERROR - Invalid context.\r\n", __FUNCTION__ ));
        mainErr = OtaPalFileClose;
    }

    prvContextClose( C );
    return OTA_PAL_COMBINE_ERR( mainErr, subErr );
}


/* Verify the signature of the specified file. */

static OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const C )
{
#ifdef __ICCARM__
#pragma data_alignment=4
    uint8_t  ucBuff[512];
#else
    uint8_t  ucBuff[512] __attribute__((aligned(4)));
#endif

    OtaPalMainStatus_t result = OtaPalSuccess; //OtaPalSignatureCheckFailed;
    uint32_t ulBytesRead, ulOffset;
    uint32_t ulSignerCertSize;
    uint8_t * pucBuf, * pucSignerCert;
    void * pvSigVerifyContext;
    int ret;
    
    if( prvContextValidate( C ) == pdTRUE )
    {
        /* Verify an ECDSA-SHA256 signature. */
        if( false == CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256 ) )
        {
            result = OtaPalSignatureCheckFailed;
        }
        else
        {
            LogInfo(( "[%s] Started %s signature verification, file: %s\r\n", __FUNCTION__,
                        OTA_JsonFileSignatureKey, ( const char * ) C->pCertFilepath ));
            pucSignerCert = otaPal_ReadAndAssumeCertificate( ( const uint8_t * const ) C->pCertFilepath, &ulSignerCertSize );

            if( pucSignerCert != NULL )
            {
                /* if ulFileSize not correct, alternative by  xCurBootImgDesc.pvEndAddr - prvStAddr + (sizeof( BootImageDescriptor_t) - sizeof( BootImageHeader_t)) */
                for( ulOffset=0; ulOffset < C->fileSize; ulOffset+= sizeof(ucBuff) )
                {
                    if( (C->fileSize - ulOffset) > sizeof(ucBuff) ) {
                        ulBytesRead = sizeof(ucBuff);
                    } else {
                        ulBytesRead = (C->fileSize - ulOffset);
                    }
                    ulBytesRead = NVT_SPI_Flash_Block_Read(ulOffset, ucBuff, ulBytesRead );

                    /* Include the file chunk in the signature validation. Zero size is OK. */
                    CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, ucBuff, ulBytesRead );
                }

                if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert,
                                                       ulSignerCertSize, C->pSignature->data, 
                                                       C->pSignature->size ) == false )
                {
                    result = OtaPalSignatureCheckFailed;
                    LogError(( "[%s] ERROR - Signature Verification Failed.\r\n", __FUNCTION__ ));
                    /* Erase the image as signature verification failed.*/
//                    prvSPI_FLASH_EraseBank();
                }
                else
                {
                    result = OtaPalSuccess;
                }
 
            }
            else
            {
                result = OtaPalBadSignerCert;
            }
        }
    }
    else
    {
        /* FIXME: Invalid error code for a NULL file context. */
        LogError(( "[%s] ERROR - Invalid OTA file context.\r\n", __FUNCTION__ ));
        /* Invalid OTA context or file pointer. */
        result = OtaPalNullFileContext;
    }

    /* Free the signer certificate that we now own after otaPal_ReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        free( pucSignerCert );
    }
 
    return OTA_PAL_COMBINE_ERR( result, 0 );;
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

static uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{

    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        LogInfo(( "[%s] Using cert with label: %s OK\r\n", __FUNCTION__, ( const char * ) pucCertName ));
    }
    else
    {

        LogInfo( ( "[%s] No such certificate file: %s. Using certificate in ota_demo_config.h." , __FUNCTION__,
                 ( const char * ) pucCertName ));
                    
        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( codeSigningCertificatePEM );
        pucSignerCert = pvPortMalloc( ulCertSize + 1 );                       /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) codeSigningCertificatePEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
            pucSignerCert[ ulCertSize ] = 0U;
            *ulSignerCertSize = ulCertSize + 1U;
        }
        else
        {
            LogError(( "[%s] Error: No memory for certificate of size %d!\r\n", __FUNCTION__, ulCertSize ));
        }
    }

    return pucSignerCert;
}


/*-----------------------------------------------------------*/
#define OTA_DELAY       ( ( portTickType ) 1000 / portTICK_RATE_MS )

OtaPalStatus_t otaPal_ResetDevice( OtaFileContext_t * const C )
{
    ( void ) C;
    
    portTickType xLastExecutionTime = xTaskGetTickCount();
    LogInfo(( "[%s] Resetting the device.\r\n", __FUNCTION__ ));

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
          LogError(( " Read FMC config failed.\r\n"));
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
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}

/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_ActivateNewImage( OtaFileContext_t * const C )
{
#define APPLICATION_ADDR 0x00

    LogInfo(( "[%s] Activating the new MCU image.\r\n", __FUNCTION__ ));

    /* Backup current active image to SPI flash in the 2nd bank at (OTA_SPI_BANK_START + OTA_SPI_BANK_SIZE) */
    /* Also mark FMC's image header reserved field as backup src-image SPI address  */
#ifndef NVT_OTA_WITHOUT_BACKUP_BANK
    prvSPI_FLASH_BackupBank((OTA_SPI_BANK_START + OTA_SPI_BANK_SIZE), APPLICATION_ADDR, (NVT_OTA_META_BASE - APPLICATION_ADDR) );
#if 1 // Monitor header of Bank1 & Bank2
    char buffer[64];
    NVT_SPI_Flash_Block_Read(0x00, buffer, sizeof(buffer) );
    LogInfo(("### Bank-1: 0x%x,%x, %x, %x\n", (uint32_t *)&buffer[0], (uint32_t *)&buffer[4],(uint32_t *)&buffer[8], (uint32_t *)&buffer[12]));
    NVT_SPI_Flash_Block_Read(0x80000, buffer, sizeof(buffer) );
    LogInfo(("### Bank-2: %s\n", buffer));
#endif
#endif
    return otaPal_ResetDevice(C);
}



/*
 * Set the final state of the last transferred (final) OTA file (or bundle).
 * The state of the OTA image is stored in FMC header.
 */

OtaPalStatus_t otaPal_SetPlatformImageState( OtaFileContext_t * const C,
                                             OtaImageState_t eState )
{

    BootImageDescriptor_t xDescCopy;
    OtaPalMainStatus_t result = OtaPalUninitialized;
    ( void ) C;
    /* Descriptor handle for the image. */
    const BootImageDescriptor_t * pxAppImgDesc;
    pxAppImgDesc = ( BootImageDescriptor_t * ) NVT_BOOT_IMG_HEAD_BASE;
    xDescCopy = *pxAppImgDesc;                    /* Copy image descriptor from flash into RAM structure. */

    
    if( (eState == OtaImageStateUnknown) || (eState > OtaLastImageState) )
    {
        LogError(( "[%s] ERROR - Invalid image state provided.\r\n", __FUNCTION__ ));
        result = OtaPalBadImageState;
    } /*lint !e481 Allow fopen and fclose calls in this context. */
    else /* Image state valid. */    
    /* This should be an image launched in self test mode! */
//    if( xDescCopy.xImgHeader.ucImgFlags == AWS_BOOT_FLAG_IMG_PENDING_COMMIT )
    {
        if( eState == OtaImageStateAccepted )
        {
            /* Mark the image as valid */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_VALID;

            if( (xCurOTAOpDesc.pxCurOTAFile == NULL) &&
                (prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == true) )
            {
                LogInfo(( "[%s] Accepted and committed final image.\r\n", __FUNCTION__ ));
                result = OtaPalSuccess;
            }
            else
            {
                LogError(( "[%s] Accepted final image but commit failed (%d).\r\n", __FUNCTION__));
                result = OtaPalCommitFailed;
            }
        }
        else if( eState == OtaImageStateRejected )
        {
            /* Mark the image as invalid */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_INVALID;

            if( prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == true )
            {
                LogInfo(( "[%s] Rejected image.\r\n", __FUNCTION__ ));

                result = OtaPalSuccess;
            }
            else
            {
                LogError(( "[%s] Failed updating the flags.\r\n", __FUNCTION__ ));
                result = OtaPalRejectFailed;
            }
        }
        else if( eState == OtaImageStateAborted )
        {
            /* Mark the image as invalid */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_INVALID;

            if( prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == true )
            {
                LogInfo(( "[%s] Aborted image.\r\n", __FUNCTION__ ));

                result = OtaPalSuccess;
            }
            else
            {
                LogError(( "[%s] Failed updating the flags.\r\n", __FUNCTION__));
                result = OtaPalAbortFailed ;
            }
        }
        else if( eState == OtaImageStateTesting )
        {
            /* Mark the image as pending commit */
            xDescCopy.xImgHeader.ucImgFlags = AWS_BOOT_FLAG_IMG_PENDING_COMMIT;

            if( prvFLASH_update(NVT_BOOT_IMG_HEAD_BASE, (uint8_t *)&xDescCopy, 
                                sizeof( BootImageDescriptor_t) ) == true )
            {
                LogInfo(( "[%s] Testing image.\r\n", __FUNCTION__ ));

                result = OtaPalSuccess;
            }
            else
            {
                LogError(( "[%s] Failed updating the flags.\r\n", __FUNCTION__));
                result = OtaPalCommitFailed ;
            }
            result = OtaPalSuccess;
        }
        else
        {
            LogError(( "[%s] Unknown state received %d.\r\n", __FUNCTION__, ( int32_t ) eState ));
            result = OtaPalBadImageState;
        }
    }

    LogInfo(( "[%s] image state [%d] ---Flag[0x%x].\r\n", __FUNCTION__, eState, xDescCopy.xImgHeader.ucImgFlags));
 
    return OTA_PAL_COMBINE_ERR( result, 0 );
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
OtaPalImageState_t otaPal_GetPlatformImageState( OtaFileContext_t * const C )
{
    BootImageDescriptor_t xDescCopy;
    OtaPalImageState_t eImageState = OtaPalImageStateUnknown; 
    const BootImageDescriptor_t * pxAppImgDesc;
    ( void ) C;
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
                eImageState = OtaPalImageStatePendingCommit;
                break;
            case AWS_BOOT_FLAG_IMG_VALID:
            case AWS_BOOT_FLAG_IMG_NEW:
                eImageState = OtaPalImageStateValid;
                break;

            default:
                eImageState = OtaPalImageStateInvalid;
                break;
        }
            
        
    }
    LogInfo(( "[%s] image state [%d] -- Flag[0x%x].\r\n", __FUNCTION__, eImageState, xDescCopy.xImgHeader.ucImgFlags));
    return eImageState;
}

/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef FREERTOS_ENABLE_UNIT_TESTS
#include "aws_ota_pal_test_access_define.h"
#endif
