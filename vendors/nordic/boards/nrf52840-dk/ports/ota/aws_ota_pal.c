/*
 * Amazon FreeRTOS OTA PAL V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_ota_agent.h"
#include "aws_ota_pal.h"
#include "aws_ota_agent_internal.h"
#include "aws_ota_pal_settings.h"
#include "aws_ota_codesigner_certificate.h"

#include "asn1utility.h"
#include "mbedtls/base64.h"
#include "nrf_mbr.h"
#include "nrf_sdm.h"
#include "nrf_nvmc.h"
#include "nrf_sdh_soc.h"
#include "nrf_crypto.h"

#include "event_groups.h"


/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";
const char pcOTA_PAL_Magick[ otapalMAGICK_SIZE ] = "@AFRTOS";
const size_t otapalFLASH_END = ( size_t ) &__FLASH_segment_end__;
/* Tag by which the beginning of the ECDSA in the public key can be found */
const char pcOTA_PAL_ASN_1_ECDSA_TAG[] = "\x06\x07\x2A\x86\x48\xCE\x3D\x02\x01\x06\x08\x2A\x86\x48\xCE\x3D\x03\x01\x07";

const char pcOTA_PAL_CERT_BEGIN[] = "-----BEGIN CERTIFICATE-----";
const char pcOTA_PAL_CERT_END[] = "-----END CERTIFICATE-----";
static uint32_t pulSerializingArray[otapalSERIALIZING_ARRAY_SIZE];
size_t ulPublicKeySize = 0;

/* The static functions below (prvPAL_CheckFileSignature and prvPAL_ReadAndAssumeCertificate)
 * are optionally implemented. If these functions are implemented then please set the following macros in
 * aws_test_ota_config.h to 1:
 * otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 * otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */

/**
 * @brief Verify the signature of the specified file.
 *
 * This function should be implemented if signature verification is not offloaded
 * to non-volatile memory io functions.
 *
 * This function is called from prvPAL_Close().
 *
 * @param[in] C OTA file context information.
 *
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 *
 */
static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C );

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 *
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 *
 * This function is called from prvPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 *
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 *
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/**
 * @brief Populates a flash memory location with an image descriptor.
 *
 * @param[in] C OTA file context information.
 *
 * @return A number of bytes written or -1 if an error happened.
 */
static ret_code_t prvPAL_WriteImageDescriptor( OTA_FileContext_t * const C );

/**
 * @brief Sincronously write data to flash
 *
 * @param[in] ulOffset Position to write
 * @param[in] ulBlockSize Data size
 * @param[in] pacData Data to be written
 *
 * @return Amount of written data or -1 in the case of error
 */
static ret_code_t prvWriteFlash( uint32_t ulOffset,
                                 uint32_t ulBlockSize,
                                 uint8_t * const pacData );

static int prvNextFreeFileHandle = 1;
static EventGroupHandle_t xFlashEventGrp = NULL;

/*-----------------------------------------------------------*/

ret_code_t prvComputeSHA256Hash( void * pvMessage,
                                 size_t ulMessageSize,
                                 void * pvHashDigest,
                                 size_t * pulDigestSize );

/*-----------------------------------------------------------*/

ret_code_t prvVerifySignature( uint8_t * pusHash,
                               size_t ulHashSize,
                               uint8_t * pusSignature,
                               size_t ulSignatureSize,
                               uint8_t * pucPublicKey,
                               uint32_t ulPublicKeySize);

/*-----------------------------------------------------------*/

OTA_Err_t prvErasePages(size_t xFrom, size_t xTo)
{
    size_t ulErasedAddress = (size_t) xFrom;
    OTA_Err_t xStatus = kOTA_Err_None;
    ret_code_t xErrCode = NRF_SUCCESS;
    while(xStatus == kOTA_Err_None && ulErasedAddress < (size_t) xTo )
    {
        size_t ulErasedPage = ( size_t ) ( ulErasedAddress / NRF_FICR->CODEPAGESIZE );
        ulErasedAddress += NRF_FICR->CODEPAGESIZE;
        xEventGroupClearBits( xFlashEventGrp, otapalFLASH_SUCCESS | otapalFLASH_FAILURE );
        xErrCode = sd_flash_page_erase( ulErasedPage );

        if( xErrCode != NRF_SUCCESS )
        {
            xStatus = kOTA_Err_RxFileCreateFailed;
        }
        else
        {
            EventBits_t xFlashResult = xEventGroupWaitBits( xFlashEventGrp, otapalFLASH_SUCCESS | otapalFLASH_FAILURE, pdTRUE, pdFALSE, portMAX_DELAY );
            if( xFlashResult & otapalFLASH_FAILURE )
            {
                xStatus = kOTA_Err_RxFileCreateFailed;
            }
        }
    }
    return xStatus;
}

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );
    ret_code_t xErrCode;
    C->lFileHandle = prvNextFreeFileHandle;
    OTA_Err_t xStatus = kOTA_Err_None;
    /* Check that the second bank size is big enough to contain the new firmware */
    if( C->ulFileSize > otapalSECOND_BANK_SIZE )
    {
        xStatus = kOTA_Err_OutOfMemory;
    }
    if (xStatus == kOTA_Err_None){
        /* Create the event group for flash events */
        xFlashEventGrp = xEventGroupCreate();

        if( xFlashEventGrp == NULL )
        {
            OTA_LOG_L1( "[%s] No memory for creation of event group \n", OTA_METHOD_NAME );
            xStatus = kOTA_Err_OutOfMemory;
        }
    }
    if (xStatus == kOTA_Err_None){
        /* Erase the required memory */
        OTA_LOG_L1( "[%s] Erasing the flash memory \n", OTA_METHOD_NAME );
        xStatus = prvErasePages(otapalSECOND_BANK_START, otapalSECOND_BANK_END);
        if (xStatus != kOTA_Err_None) 
        {
             OTA_LOG_L1( "[%s] Erasing the flash memory failed with error_code %d\n", OTA_METHOD_NAME, xStatus );
        }
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );

    C->lFileHandle = ( int32_t ) NULL;
    /* Delete the event group */
    if( xFlashEventGrp != NULL )
    {
        vEventGroupDelete( xFlashEventGrp );
        xFlashEventGrp = NULL;
    }

    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );

    /* We assume that the flash is already erased by this address (it should be, as we erase it in the
     * prvPAL_CreateFileForRx, but the second write to the same position can break this invariant.
     * Anyway, the OTA procedure must not try to write to the same addresses */
    ret_code_t xErrCode = prvWriteFlash( otapalSECOND_BANK_START + otapalDESCRIPTOR_SIZE + ulOffset, ulBlockSize, pacData );

    if( xErrCode == NRF_SUCCESS )
    {
        OTA_LOG_L1( "[%s] Write %ul bytes from the %ul address \n", OTA_METHOD_NAME, ulBlockSize, ulOffset );
        return ulBlockSize;
    }
    else
    {
        OTA_LOG_L1( "[%s] Write %ul bytes from the %ul address failed with error code %d\n", OTA_METHOD_NAME, ulBlockSize,
                    ulOffset, xErrCode );
        return -1;
    }
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    OTA_Err_t xError;
    ret_code_t xStatus;

    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    OTA_LOG_L1( "[%s] Erasing the flash memory was successful\n", OTA_METHOD_NAME );

    /* Write the image descriptor */
    xStatus = prvPAL_WriteImageDescriptor( C );
    if(xStatus == NRF_SUCCESS)
    {
      /* Increment the file handler */
        prvNextFreeFileHandle += 1;

        xError = prvPAL_CheckFileSignature( C );
    }else
    {
        xError = kOTA_Err_SignatureCheckFailed;
    }

    return xError;
}
/*-----------------------------------------------------------*/


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

    nrf_crypto_hash_sha256_digest_t xHash;
    size_t ulHashLength = sizeof( xHash );
    ret_code_t xErrCode = NRF_SUCCESS;
    uint32_t ulCertificateSize = 0;
    uint8_t *pusCertificate = prvPAL_ReadAndAssumeCertificate (NULL, &ulCertificateSize);
    if (pusCertificate == NULL) {
        xErrCode = NRF_ERROR_INTERNAL;
    }
    if (xErrCode == NRF_SUCCESS) {
        prvComputeSHA256Hash( (uint8_t *)otapalSECOND_BANK_START + otapalDESCRIPTOR_SIZE, C->ulFileSize, &xHash, &ulHashLength );
        xErrCode = prvVerifySignature( (uint8_t*) &xHash, ulHashLength, C->pxSignature->ucData, C->pxSignature->usSize, pusCertificate, ulCertificateSize );
        vPortFree( pusCertificate );        
    }

    if( xErrCode != NRF_SUCCESS )
    {
        OTA_LOG_L1( "[%s] Signature check failed \n", OTA_METHOD_NAME );
        return kOTA_Err_SignatureCheckFailed;
    }

    OTA_LOG_L1( "[%s] Signature check passed \n", OTA_METHOD_NAME );
    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    uint8_t * pucPublicKey;

    DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );
    * ulSignerCertSize = sizeof(signingcredentialSIGNING_CERTIFICATE_PEM);
    /* Skip the "BEGIN CERTIFICATE" */
    uint8_t* pucCertBegin = strstr (signingcredentialSIGNING_CERTIFICATE_PEM, pcOTA_PAL_CERT_BEGIN) ;
    if (pucCertBegin == NULL)
    {
        return NULL;
    }
    pucCertBegin += sizeof(pcOTA_PAL_CERT_BEGIN);
    /* Skip the "END CERTIFICATE" */
    uint8_t* pucCertEnd = strstr(pucCertBegin, pcOTA_PAL_CERT_END);
    if (pucCertEnd == NULL)
    {
        return NULL;
    }
    uint8_t *pucDecodedCertificate;
    size_t ulDecodedCertificateSize;
    mbedtls_base64_decode(pucDecodedCertificate, 0, &ulDecodedCertificateSize, pucCertBegin, pucCertEnd - pucCertBegin);
    pucDecodedCertificate = (char*) pvPortMalloc(ulDecodedCertificateSize);
    if (pucDecodedCertificate == NULL) 
    {
        return NULL;
    }    
    mbedtls_base64_decode(pucDecodedCertificate, ulDecodedCertificateSize, &ulDecodedCertificateSize, pucCertBegin, pucCertEnd - pucCertBegin);
    /* Find the tag of the ECDSA public signature*/
    uint8_t * pucPublicKeyStart = pucDecodedCertificate;
    while (pucPublicKeyStart < pucDecodedCertificate + ulDecodedCertificateSize ) 
    {
        if (memcmp(pucPublicKeyStart, pcOTA_PAL_ASN_1_ECDSA_TAG, sizeof(pcOTA_PAL_ASN_1_ECDSA_TAG) - 1) == 0)
        {
            break;
        }
        pucPublicKeyStart ++;
    }

    if (pucPublicKeyStart == pucPublicKeyStart + ulDecodedCertificateSize) {
        vPortFree(pucDecodedCertificate);
        return NULL;
    }
    pucPublicKeyStart -= 4; 
    * ulSignerCertSize = pucPublicKeyStart[1] + 2;
    vPortFree(pucDecodedCertificate);
    pucPublicKey = pvPortMalloc(* ulSignerCertSize);
    if( pucPublicKey != NULL)
    {
        memcpy(pucPublicKey, pucPublicKeyStart, * ulSignerCertSize);
    }
    return pucPublicKey;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ResetDevice" );
    NVIC_SystemReset();
    return kOTA_Err_ResetNotSupported;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ActivateNewImage" );

    /* FIX ME. */
    prvPAL_ResetDevice();
    return kOTA_Err_Uninitialized;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );
    OTA_Err_t xStatus = kOTA_Err_None;
    if (xFlashEventGrp == NULL){
        /* Create the event group for flash events */
        xFlashEventGrp = xEventGroupCreate();

        if( xFlashEventGrp == NULL )
        {
            OTA_LOG_L1( "[%s] No memory for creation of event group \n", OTA_METHOD_NAME );
            xStatus = kOTA_Err_OutOfMemory;
        }
    }
    /* Read the old image */
    /* Right now we always boot from the first bank */
    /* TODO: Support boot from the second bank */
    ImageDescriptor_t * old_descriptor = ( ImageDescriptor_t * ) ( otapalFIRST_BANK_START );
    ImageDescriptor_t new_descriptor;
    /* Check if the correct image is located at the beginning of the bank */
    if ((memcmp(old_descriptor->pMagick, pcOTA_PAL_Magick, otapalMAGICK_SIZE) != 0)&&(eState == eOTA_ImageState_Accepted)) {
        xStatus = kOTA_Err_CommitFailed;
    }
    if (xStatus == kOTA_Err_None){
        memcpy(&new_descriptor, old_descriptor, sizeof(new_descriptor));
        switch (eState) {
        case eOTA_ImageState_Accepted:
            if (old_descriptor->usImageFlags == otapalIMAGE_FLAG_COMMIT_PENDING)
            {
                new_descriptor.usImageFlags = otapalIMAGE_FLAG_VALID;
            }
            else
            {
                new_descriptor.usImageFlags = otapalIMAGE_FLAG_INVALID;
            }

            break;
        case eOTA_ImageState_Rejected:
        case eOTA_ImageState_Aborted:
            if (old_descriptor->usImageFlags == otapalIMAGE_FLAG_COMMIT_PENDING)
            {
                new_descriptor.usImageFlags = otapalIMAGE_FLAG_INVALID;
            }
            break;
        case eOTA_ImageState_Testing:
            break;
        default:
            xStatus = kOTA_Err_BadImageState;
            break;
        }
    }

    if ( xStatus == kOTA_Err_None )
    {
        /* We don't wont to do anything with flash if it would leave it in the same state as it were */
        if ((new_descriptor.usImageFlags != old_descriptor->usImageFlags)&&(eState != eOTA_ImageState_Testing))
        {
            xStatus = prvErasePages(otapalFIRST_BANK_START, otapalFIRST_BANK_START + otapalDESCRIPTOR_SIZE);
            if(xStatus == kOTA_Err_None)
            {
                xStatus = prvWriteFlash(otapalFIRST_BANK_START, sizeof(new_descriptor), (uint8_t *)&new_descriptor);
            }
        }
    }
    return xStatus;
}
/*-----------------------------------------------------------*/

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );

    OTA_PAL_ImageState_t xImageState = eOTA_PAL_ImageState_Invalid;

    ImageDescriptor_t * descriptor = ( ImageDescriptor_t * ) ( otapalFIRST_BANK_START );

    switch( descriptor->usImageFlags )
    {
        case otapalIMAGE_FLAG_COMMIT_PENDING:
            xImageState = eOTA_PAL_ImageState_PendingCommit;
            break;

        case otapalIMAGE_FLAG_VALID:
            xImageState = eOTA_PAL_ImageState_Valid;
            break;

        case otapalIMAGE_FLAG_INVALID:
            xImageState = eOTA_PAL_ImageState_Invalid;
            break;
    }

    return xImageState;
}
/*-----------------------------------------------------------*/

static ret_code_t prvPAL_WriteImageDescriptor( OTA_FileContext_t * const C )
{
    ImageDescriptor_t xDescriptor;
    ImageDescriptor_t xOldDescriptor;
    OTA_Err_t xStatus = kOTA_Err_None;

    memset( &xDescriptor, 0, sizeof( xDescriptor ) );

    /* Read the first bank descriptor */
    memcpy( &xOldDescriptor, ( void * ) otapalFIRST_BANK_START, sizeof( xOldDescriptor ) );

    /* Fill the new descriptor */
    if( memcmp( &xOldDescriptor.pMagick, pcOTA_PAL_Magick, otapalMAGICK_SIZE ) == 0 ) /* Check if an AFR image contained in the first bank */
    {
        xDescriptor.ulSequenceNumber = xOldDescriptor.ulSequenceNumber + 1;
    }
    else /* It seems that the first bank contains a classic Nordic image */
    {
        xDescriptor.ulSequenceNumber = 1;
    }

    memcpy( &xDescriptor.pMagick, pcOTA_PAL_Magick, otapalMAGICK_SIZE );
    xDescriptor.ulStartAddress = otapalFIRST_BANK_START + otapalDESCRIPTOR_SIZE;
    xDescriptor.ulEndAddress = xDescriptor.ulStartAddress + C->ulFileSize;
    xDescriptor.ulExecutionAddress = xDescriptor.ulStartAddress; /* TODO: Check if this assumption is true */
    xDescriptor.ulHardwareID = 0;                                /* TODO: Fill the Hardware ID */
    xDescriptor.usImageFlags = otapalIMAGE_FLAG_NEW;
    xDescriptor.ulSignatureSize = C->pxSignature->usSize;

    if( C->pxSignature != NULL)
    {
      if( C->pxSignature->usSize <= kOTA_MaxSignatureSize)
       {
          memcpy( &xDescriptor.pSignature, C->pxSignature->ucData, C->pxSignature->usSize );
       }
     }
    prvErasePages(otapalSECOND_BANK_START, otapalSECOND_BANK_START + otapalDESCRIPTOR_SIZE);
    return prvWriteFlash( otapalSECOND_BANK_START, sizeof( xDescriptor ), (uint8_t *)&xDescriptor );
}

/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
ret_code_t prvWriteFlash( uint32_t ulOffset,
                          uint32_t ulBlockSize,
                          uint8_t * const pacData )
{
    uint32_t ulByteSent, ulByteToSend;
    ret_code_t xErrCode = NRF_SUCCESS;

    ulByteSent = 0;
    while(ulByteSent < ulBlockSize)
    {
      ulByteToSend = ulBlockSize - ulByteSent;

      if(ulByteToSend > otapalSERIALIZING_ARRAY_SIZE*4)
      {
        ulByteToSend = otapalSERIALIZING_ARRAY_SIZE*4;
      }

      if(ulByteToSend > NRF_FICR->CODEPAGESIZE)
      {
        ulByteToSend = NRF_FICR->CODEPAGESIZE*4;
      }
       /* clear buffer to avoid garbage. */
      pulSerializingArray[(ulByteToSend+3)/4] = 0;
      memcpy(pulSerializingArray, pacData + ulByteSent, ulByteToSend); 

      xEventGroupClearBits( xFlashEventGrp, otapalFLASH_SUCCESS | otapalFLASH_FAILURE );
      /* Softdevice can write only by 32-bit words */
      ret_code_t xErrCode = sd_flash_write( ( uint32_t * ) (ulOffset + ulByteSent), pulSerializingArray, (ulByteToSend+3)/4);

      if( xErrCode == NRF_SUCCESS )
      {
          EventBits_t xFlashResult = xEventGroupWaitBits( xFlashEventGrp, otapalFLASH_SUCCESS | otapalFLASH_FAILURE, pdTRUE, pdFALSE, portMAX_DELAY );

          if( xFlashResult & otapalFLASH_FAILURE )
          {
              xErrCode = NRF_ERROR_INTERNAL;
              break;
          }
      }

      ulByteSent += ulByteToSend;

    }

    return xErrCode;
}
/*-----------------------------------------------------------*/

ret_code_t prvComputeSHA256Hash( void * pvMessage,
                                 size_t ulMessageSize,
                                 void * pvHashDigest,
                                 size_t * pulDigestSize )
{
    /* We are computing the hash of memory stored on the flash so we need to first move it to ram to use CC310
     * That's why we are computing the hash by chunks */
    ret_code_t err_code;
    static nrf_crypto_hash_context_t hash_context;

    err_code = nrf_crypto_hash_init( &hash_context, &g_nrf_crypto_hash_sha256_info );
    uint8_t pulMemBuffer[ 256 ];
    size_t ulConsumed = 0;

    while( err_code == NRF_SUCCESS && ulConsumed < ulMessageSize )
    {
        size_t ulToConsume = MIN( ulMessageSize - ulConsumed, 256 );
        memcpy( pulMemBuffer, pvMessage + ulConsumed, ulToConsume );
        err_code = nrf_crypto_hash_update( &hash_context, pulMemBuffer, ulToConsume );
        ulConsumed += ulToConsume;
    }

    if( err_code == NRF_SUCCESS )
    {
        err_code = nrf_crypto_hash_finalize( &hash_context, pvHashDigest, pulDigestSize );
    }

    return err_code;
}

/*-----------------------------------------------------------*/

ret_code_t prvVerifySignature( uint8_t * pusHash,
                               size_t ulHashSize,
                               uint8_t * pusSignature,
                               size_t ulSignatureSize,
                               uint8_t * pucPublicKey,
                               uint32_t ulPublicKeySize)
{
    ret_code_t xErrCode;
    nrf_crypto_ecdsa_verify_context_t xVerifyContext;
    nrf_crypto_ecc_public_key_t xPublicKey;
    nrf_crypto_ecdsa_signature_t xSignature;
    uint8_t * pucTmpPublicKey;

    memset( &xPublicKey, 0, sizeof( xPublicKey ) );
    memset( &xSignature, 0, sizeof( xSignature ) );
    
    /* The public key comes in the ASN.1 DER format,  and so we need everything
     * except the DER metadata which fortunately in this case is containded in the front part of buffer */
    pucTmpPublicKey  = pucPublicKey + ( ulPublicKeySize - NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE );
    /* Convert the extracted public key to the NRF5 representation */
    xErrCode = nrf_crypto_ecc_public_key_from_raw( &g_nrf_crypto_ecc_secp256r1_curve_info, &xPublicKey, pucTmpPublicKey, NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE );
    if (xErrCode == NRF_SUCCESS) {
        /* The signature is also ASN.1 DER encoded, but here we need to decode it properly */
        xErrCode = asn1_decodeSignature( xSignature, pusSignature, pusSignature + ulSignatureSize );
    }
    if (xErrCode == 0) {
        /* Aand... Verify! */
        xErrCode = nrf_crypto_ecdsa_verify( &xVerifyContext,                             /* Context */
                                            &xPublicKey,                                 /* Public key */
                                            pusHash,                                     /* Message hash */
                                            ulHashSize,                                  /* Hash size */
                                            xSignature,                                  /* Signature */
                                            NRF_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE ); /* Signature size */

    }
    return xErrCode;
}

/*-----------------------------------------------------------*/

void flash_event_handler( uint32_t evt_id,
                          void * p_context )
{
    switch( evt_id )
    {
        case NRF_EVT_FLASH_OPERATION_SUCCESS:

            if( xFlashEventGrp != NULL )
            {
                xEventGroupSetBits( xFlashEventGrp, otapalFLASH_SUCCESS );
            }

            break;

        case NRF_EVT_FLASH_OPERATION_ERROR:

            if( xFlashEventGrp != NULL )
            {
                xEventGroupSetBits( xFlashEventGrp, otapalFLASH_FAILURE );
            }

            break;
    }
}

NRF_SDH_SOC_OBSERVER( flash_observer, BLE_DFU_SOC_OBSERVER_PRIO, flash_event_handler, NULL );

OTA_Err_t testCheckSignature(){
    OTA_FileContext_t C;
    uint32_t ulCertSize = sizeof(signingcredentialSIGNING_CERTIFICATE_PEM);
    uint8_t * pusCertificate = prvPAL_ReadAndAssumeCertificate(signingcredentialSIGNING_CERTIFICATE_PEM, &ulCertSize);
    vPortFree( pusCertificate ); 
    ImageDescriptor_t * descriptor = ( ImageDescriptor_t * ) ( otapalSECOND_BANK_START );
    C.ulFileSize = descriptor->ulEndAddress - descriptor->ulStartAddress;
    return prvPAL_CheckFileSignature( &C );
}


/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
