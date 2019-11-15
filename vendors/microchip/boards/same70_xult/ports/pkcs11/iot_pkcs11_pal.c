/*
  * Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
  * 
  * Amazon FreeRTOS PKCS#11 for SAM E70 platform
  * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy of 
  * this software and associated documentation files (the "Software"), to deal in 
  * the Software without restriction, including without limitation the rights to 
  * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
  * of the Software, and to permit persons to whom the Software is furnished to do 
  * so, subject to the following conditions:
  * The above copyright notice and this permission notice shall be included in all 
  * copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
  * SOFTWARE
  ******************************************************************************
 */


/**
 * @file pkcs11.c
 * @brief Amazon FreeRTOS PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "aws_clientcredential.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* SAM includes */
///#include "system/random/sys_random.h" /* system header. */
#include "peripheral/trng/plib_trng.h"
#include "device.h"
#include "core_cm7.h"
#include "pkcs11_nvm.h"
#include "NetworkConfig.h"



/**
 * @brief File storage location definitions.
 */

#define pkcs11FILE_NAME_PUBLISHER_CERTIFICATE    "FreeRTOS_Publisher_Certificate.dat"
#define pkcs11FILE_NAME_PUBLISHER_KEY            "FreeRTOS_Publisher_Key.dat"
#define pkcs11configFILE_CODE_SIGN_PUBLIC_KEY    "FreeRTOS_Code_Verification_Key.dat"

// the number of the supported certificates:
//      - client certificate + key
//      - signing certificate + key
#define PKCS11_CERTIFICATES_NO          4

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID                         1
#define pkcs11OBJECT_HANDLE_PUBLIC_KEY        1
#define pkcs11OBJECT_HANDLE_PRIVATE_KEY       2
#define pkcs11OBJECT_HANDLE_CERTIFICATE       3

#define pkcs11SUPPORTED_KEY_BITS              2048

// flash section where the certificates are stored
// reserve the last lock region (AWS_NVM_LOCK_REGION_PAGES/16 KB) at the end of the flash
// this is the last AWS_NVM_LOCK_REGION_PAGES/32 flash pages (512 Bytes in size each) at the end of the flash
// keep it always page aligned and multiple of AWS_NVM_ERASE_PAGES_MIN!

#define PKCS11_CERTIFICATE_FLASH_PAGES              (32)    
#define PKCS11_CERTIFICATE_SECTION_SIZE             (AWS_NVM_PAGE_SIZE * PKCS11_CERTIFICATE_FLASH_PAGES)    
#define PKCS11_CERTIFICATE_SECTION_START_ADDRESS    (IFLASH_ADDR + IFLASH_SIZE - PKCS11_CERTIFICATE_SECTION_SIZE )    

// MPU region to use for the PKCS11 certificates
// Note: regions 0 - 11 are used by default in the system_core_mpu.c as part of
// system_startup.c ARCH_CORE_MPU_Initialize() call.
#define PKCS11_CERTIFICATE_FLASH_MPU_REGION         12


enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

// the number of the supported certificates:
//      - client certificate + key
//      - signing certificate + key
#define PKCS11_CERTIFICATES_NO          4

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID                         1
#define pkcs11OBJECT_HANDLE_PUBLIC_KEY        1
#define pkcs11OBJECT_HANDLE_PRIVATE_KEY       2
#define pkcs11OBJECT_HANDLE_CERTIFICATE       3

#define pkcs11SUPPORTED_KEY_BITS              2048

// selected such that each certificate takes length = PKCS11_CERTIFICATE_SECTION_SIZE / PKCS11_CERTIFICATES_NO
// i.e. length = (AWS_NVM_PAGE_SIZE * PKCS11_CERTIFICATE_FLASH_PAGES) / PKCS11_CERTIFICATES_NO
// which should be multiple of page size and aligned!
#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE     (PKCS11_CERTIFICATE_SECTION_SIZE / PKCS11_CERTIFICATES_NO - sizeof(CK_ULONG) * 2)

#if (((PKCS11_CERTIFICATE_SECTION_SIZE / PKCS11_CERTIFICATES_NO) % AWS_NVM_PAGE_SIZE) != 0)
#error "Certificate size should be a multiple of page size and aligned!"
#endif

#define pkcs11OBJECT_FLASH_CERT_PRESENT       ( 0xABCDEFuL )

typedef int( *pfnMbedTlsSign )(
    void *ctx,
    mbedtls_md_type_t md_alg,
    const unsigned char *hash,
    size_t hash_len,
    unsigned char *sig,
    size_t *sig_len,
    int( *f_rng )( void *, unsigned char *, size_t ),
    void *p_rng );

/**
 * @brief Key structure.
 */
typedef struct P11Key
{
    mbedtls_pk_context xMbedPkCtx;
    mbedtls_x509_crt xMbedX509Cli;
    mbedtls_pk_info_t xMbedPkInfo;
    pfnMbedTlsSign pfnSavedMbedSign;
    void *pvSavedMbedPkCtx;
} P11Key_t, * P11KeyPtr_t;

/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    P11KeyPtr_t pxCurrentKey;
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    CK_OBJECT_CLASS xFindObjectClass;
    mbedtls_ctr_drbg_context xMbedDrbgCtx;
    mbedtls_entropy_context xMbedEntropyContext;
} P11Session_t, * P11SessionPtr_t;


/**
 * @brief Structure for certificates/key storage.
 */
typedef struct
{
    CK_ULONG ulCertificatePresent;
    CK_ULONG ulCertificateSize;
    CK_CHAR cCertificateData[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
} P11CertData_t;

typedef struct
{
    P11CertData_t xDeviceCertificate;
    P11CertData_t xDeviceKey;
    P11CertData_t xCodeVerificationKey;
} P11KeyConfig_t;

/**
 * Certificates/key save area for flash operations
 */
static P11KeyConfig_t P11ConfigSave;
int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );


/**
 * @brief Certificates/key storage in flash.
   Reserve this area!

   For now prevent filling this area with 0's
 */

#define PKCS11_CERTIFICATE_FLASH_RESERVE_SECTION        0

#if (PKCS11_CERTIFICATE_FLASH_RESERVE_SECTION != 0)
const P11KeyConfig_t __attribute__((space(prog), section("PKCS11_CERTIFICATE_SECTION"), address(PKCS11_CERTIFICATE_SECTION_START_ADDRESS))) __attribute__((keep)) P11ConfigFlash;
#endif  // (PKCS11_CERTIFICATE_FLASH_RESERVE_SECTION != 0)



/**
 * Certificates/key save area for flash operations
*/
static P11KeyConfig_t P11ConfigSave;


/*-----------------------------------------------------------*/

/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
static P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}

/*-----------------------------------------------------------*/

/**
 * @brief Writes a file to local storage.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    uint32_t * pFlashDest, * pDataSrc;
    int rowIx, nRows,nPages;
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    const P11CertData_t * pCertFlash;
    P11CertData_t * pCertSave = 0;
    const P11KeyConfig_t * P11ConfigFlashPtr = (const P11KeyConfig_t*)PKCS11_CERTIFICATE_SECTION_START_ADDRESS;
    P11KeyConfig_t * P11ConfigSavePtr = &P11ConfigSave;
    

    if (ulDataSize <= pkcs11OBJECT_CERTIFICATE_MAX_SIZE)
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pxLabel->pValue,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            pCertSave = &P11ConfigSavePtr->xDeviceCertificate;
            pCertFlash = &P11ConfigFlashPtr->xDeviceCertificate;
            xHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            pCertSave = &P11ConfigSavePtr->xDeviceKey;
            pCertFlash = &P11ConfigFlashPtr->xDeviceKey;
            xHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            pCertSave = &P11ConfigSavePtr->xDeviceKey;
            pCertFlash = &P11ConfigFlashPtr->xDeviceKey;
            xHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            pCertSave = &P11ConfigSavePtr->xCodeVerificationKey;
            pCertFlash = &P11ConfigFlashPtr->xCodeVerificationKey;
            xHandle = eAwsCodeSigningKey;
        }
        
        
        
        if( pCertSave != 0 )
        {                                           /* can proceed with the write */
            *P11ConfigSavePtr = *P11ConfigFlashPtr; /* copy the (whole) existent data before erasing flash */
            memcpy( pCertSave->cCertificateData, pucData, ulDataSize );
            pCertSave->ulCertificatePresent = pkcs11OBJECT_FLASH_CERT_PRESENT;
            pCertSave->ulCertificateSize = ulDataSize;
            /* now update the flash */
            nPages = sizeof(*P11ConfigSavePtr) / AWS_NVM_PAGE_SIZE;
            pFlashDest = ( uint32_t * ) P11ConfigFlashPtr;
            pDataSrc = ( uint32_t * ) P11ConfigSavePtr;

            /* start critical */
            // start critical
            taskENTER_CRITICAL();
			//Commented, since flash utility had issues running AFQP with protect enabled.
            //AWS_FlashRegionProtect(PKCS11_CERTIFICATE_FLASH_MPU_REGION, PKCS11_CERTIFICATE_SECTION_SIZE, P11ConfigFlashPtr, false);
            //AWS_FlashRegionUnlock((uint32_t*)P11ConfigFlashPtr);
            AWS_FlashPagesErase(pFlashDest, PKCS11_CERTIFICATE_FLASH_PAGES);  

            // write the pages
            AWS_FlashPagesWrite(pFlashDest, pDataSrc, nPages);

            //AWS_FlashRegionLock((uint32_t*)P11ConfigFlashPtr);
            //AWS_FlashRegionProtect(PKCS11_CERTIFICATE_FLASH_MPU_REGION, PKCS11_CERTIFICATE_SECTION_SIZE, P11ConfigFlashPtr, true);
            // done; verify
            
            if( memcmp( pCertFlash, pCertSave, ulDataSize ) != 0 )
            {
                xHandle = eInvalidHandle;
            }
            taskEXIT_CRITICAL();
        }
    }
    
    return xHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */

CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                        uint8_t usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    const P11CertData_t * pCertFlash = 0;

    const P11KeyConfig_t * P11ConfigFlashPtr = (const P11KeyConfig_t*)PKCS11_CERTIFICATE_SECTION_START_ADDRESS;

    /* TODO: Check if object actually exists/has been created before returning. */
    if( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, usLength )  )
    {
        xHandle = eAwsDeviceCertificate;
                pCertFlash = &P11ConfigFlashPtr->xDeviceCertificate;

    }
    else if( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, usLength ) )
    {
        xHandle = eAwsDevicePrivateKey;
                pCertFlash = &P11ConfigFlashPtr->xDeviceKey;

    }
    else if( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, usLength ) )
    {
        /* Public and private key are stored together in same file. */
        xHandle = eAwsDevicePublicKey;
                pCertFlash = &P11ConfigFlashPtr->xDeviceKey;

    }
    else if( 0 == memcmp( pLabel, pkcs11configFILE_CODE_SIGN_PUBLIC_KEY, usLength ) )
    {
        xHandle = eAwsCodeSigningKey;
                pCertFlash = &P11ConfigFlashPtr->xCodeVerificationKey;

    }
    
    
    if(!( ( pCertFlash != 0 ) && ( pCertFlash->ulCertificatePresent == pkcs11OBJECT_FLASH_CERT_PRESENT ) ))
    {
        xHandle = eInvalidHandle;
    }

    return xHandle;
}




/**
 * @brief Gets the value of an object in storage, by handle.
 *
 * Port-specific file access for cryptographic information.
 *
 * This call dynamically allocates the buffer which object value
 * data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
 * should be called after each use to free the dynamically allocated
 * buffer.
 *
 * @sa PKCS11_PAL_GetObjectValueCleanup
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 * @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
 *                          or exportable (CK_FALSE)
 *
 * @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
 * no such object handle was found, CKR_DEVICE_MEMORY if memory for
 * buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
 * error.
 */
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                 uint8_t ** ppucData,
                                 uint32_t * pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    CK_RV xResult = CKR_OK;
    uint32_t certSize = 0;
    uint8_t * pCertData = 0;
    const P11CertData_t * pCertFlash = 0;

    const P11KeyConfig_t * P11ConfigFlashPtr = (const P11KeyConfig_t*)PKCS11_CERTIFICATE_SECTION_START_ADDRESS;

    if( xHandle == eAwsDeviceCertificate )
    {
        pCertFlash = &P11ConfigFlashPtr->xDeviceCertificate;
                *pIsPrivate = CK_FALSE;

    }
    else if( xHandle == eAwsDevicePrivateKey )
    {
        pCertFlash = &P11ConfigFlashPtr->xDeviceKey;
                *pIsPrivate = CK_TRUE;

    }
  else if( xHandle == eAwsDevicePublicKey )
  {
      pCertFlash = &P11ConfigFlashPtr->xDeviceKey;
              *pIsPrivate = CK_FALSE;

  }
    else if( xHandle == eAwsCodeSigningKey )
    {
        pCertFlash = &P11ConfigFlashPtr->xCodeVerificationKey;
                *pIsPrivate = CK_FALSE;

    }
    else
    {
        xResult = CKR_OBJECT_HANDLE_INVALID;
    }

    if( ( pCertFlash != 0 ) && ( pCertFlash->ulCertificatePresent == pkcs11OBJECT_FLASH_CERT_PRESENT ) )
    {
        pCertData = ( uint8_t * ) pCertFlash->cCertificateData;
        certSize = pCertFlash->ulCertificateSize;
        xResult = CKR_OK;
    }

    *pulDataSize = certSize;
    *ppucData = pCertData;

    return xResult;
}


/**
 * @brief Cleanup after PKCS11_GetObjectValue().
 *
 * @param[in] pucData       The buffer to free.
 *                          (*ppucData from PKCS11_PAL_GetObjectValue())
 * @param[in] ulDataSize    The length of the buffer to free.
 *                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
 */
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
                                       uint32_t ulDataSize )
{
    /* Unused parameters. */
    ( void ) pucData;
    ( void ) ulDataSize;

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}



/*-----------------------------------------------------------*/

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{

    ((void) data);

    union
    {
        uint64_t    v64;
        uint8_t     v8[8];
    }suint_64;

    int n8Chunks = len / 8;
    int nLeft = len % 8;

    while(n8Chunks--)
    {
        suint_64.v64 = TRNG_ReadData();
        suint_64.v64 = suint_64.v64 << 32;
        suint_64.v64 |= TRNG_ReadData();
        memcpy(output, suint_64.v8, sizeof(suint_64.v8));
        output += sizeof(suint_64.v8);
    }

    if(nLeft)
    {
        suint_64.v64 = TRNG_ReadData();
        suint_64.v64 = suint_64.v64 << 32;
        suint_64.v64 |= TRNG_ReadData();
        memcpy(output, suint_64.v8, nLeft);
    }


  *olen = len;
  
  return 0;
}


