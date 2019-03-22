/*
 * Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
 *
 * Amazon FreeRTOS PKCS #11 PAL for Curiosity PIC32MZEF V1.0.4
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
 *
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
 * @file aws_pkcs11_pal.c
 * @brief Amazon FreeRTOS device-specific code for mbedTLS based PKCS#11 
 * implementation with software keys. This file deviates from the FreeRTOS 
 * style standard for some function names and data types in order to
 * maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#include "pkcs11_nvm.h"
#include <sys/kmem.h>

#define pkcs11FILE_NAME_PUBLISHER_CERTIFICATE    "FreeRTOS_Publisher_Certificate.dat"
#define pkcs11FILE_NAME_PUBLISHER_KEY            "FreeRTOS_Publisher_Key.dat"
#define pkcs11configFILE_CODE_SIGN_PUBLIC_KEY    "FreeRTOS_Code_Verification_Key.dat"


/* flash section where the certificates are stored */
/* reserve the last flash page in the upper boot alias */
/* keep it page aligned and size of the page size(16 KB)! */
/* use k1 pointers to avoid issues with reading from cache */
#define PKCS11_CERTIFICATE_SECTION_START_ADDRESS    ( __UPPERBOOTALIASLASTPAGE_BASE )
#define PKCS11_CERTIFICATE_SECTION_SIZE             ( __UPPERBOOTALIASLASTPAGE_LENGTH )

/* the number of the supported certificates: */
/*      - client certificate + key */
/*      - signing certificate + key */
#define PKCS11_CERTIFICATES_NO               4

#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE    ( PKCS11_CERTIFICATE_SECTION_SIZE / PKCS11_CERTIFICATES_NO - sizeof( CK_ULONG ) * 2 )

#define pkcs11OBJECT_FLASH_CERT_PRESENT      ( 0xABCDEFuL )

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

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

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen );

static void PIC32MZ_HW_TRNG_Initialize( void );
static uint64_t PIC32MZ_HW_TRNG_Get( void );


/*-----------------------------------------------------------*/

/**
 * @brief Saves an object in non-volatile storage.
 *
 * Port-specific file write for cryptographic information.
 *
 * @param[in] pxLabel       The label of the object to be stored.
 * @param[in] pucData       The object data to be saved
 * @param[in] pulDataSize   Size (in bytes) of object data.
 *
 * @return The object handle if successful.
 * eInvalidHandle = 0 if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    uint32_t ulStatus = 0;
    uint32_t * pFlashDest, * pDataSrc;
    int rowIx, nRows;
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    const P11CertData_t * pCertFlash;
    P11CertData_t * pCertSave = 0;
    const P11KeyConfig_t * P11ConfigFlashPtr = ( const P11KeyConfig_t * ) KVA0_TO_KVA1( PKCS11_CERTIFICATE_SECTION_START_ADDRESS );
    P11KeyConfig_t * P11ConfigSavePtr = ( P11KeyConfig_t * ) KVA0_TO_KVA1( ( uint32_t ) &P11ConfigSave );

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
            nRows = PKCS11_CERTIFICATE_SECTION_SIZE / AWS_NVM_ROW_SIZE;
            pFlashDest = ( uint32_t * ) P11ConfigFlashPtr;
            pDataSrc = ( uint32_t * ) P11ConfigSavePtr;

            /* start critical */
            taskENTER_CRITICAL();
            AWS_UpperBootPage4ProtectionDisable();
            AWS_UpperBootPage4Erase();

            /* start writing */
            for( rowIx = 0; rowIx < nRows; rowIx++ )
            {
                AWS_UpperBootWriteRow( pFlashDest, pDataSrc );
                pFlashDest += AWS_NVM_ROW_SIZE / sizeof( uint32_t );
                pDataSrc += AWS_NVM_ROW_SIZE / sizeof( uint32_t );
            }

            AWS_UpperBootPage4ProtectionEnable();
            taskEXIT_CRITICAL();

            /* done; verify */
            if( memcmp( pCertFlash, pCertSave, ulDataSize ) != 0 )
            {
                xHandle = eInvalidHandle;
            }
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
        uint32_t certSize = 0;
    uint8_t * pCertData = 0;
    const P11CertData_t * pCertFlash = 0;

    const P11KeyConfig_t * P11ConfigFlashPtr = ( const P11KeyConfig_t * ) KVA0_TO_KVA1( PKCS11_CERTIFICATE_SECTION_START_ADDRESS );

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

    const P11KeyConfig_t * P11ConfigFlashPtr = ( const P11KeyConfig_t * ) KVA0_TO_KVA1( PKCS11_CERTIFICATE_SECTION_START_ADDRESS );

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
    static bool pic32mz_hw_init = false;

    if( !pic32mz_hw_init )
    {
        PIC32MZ_HW_TRNG_Initialize();
        pic32mz_hw_init = true;
    }

    ( ( void ) data );

    union
    {
        uint64_t v64;
        uint8_t v8[ 8 ];
    }
    suint_64;

    int n8Chunks = len / 8;
    int nLeft = len - n8Chunks * 8;

    while( n8Chunks-- )
    {
        suint_64.v64 = PIC32MZ_HW_TRNG_Get();
        memcpy( output, suint_64.v8, sizeof( suint_64.v8 ) );
        output += sizeof( suint_64.v8 );
    }

    if( nLeft )
    {
        suint_64.v64 = PIC32MZ_HW_TRNG_Get();
        memcpy( output, suint_64.v8, nLeft );
    }

    *olen = len;

    return 0;
}


/*-----------------------------------------------------------*/

static void PIC32MZ_HW_TRNG_Initialize( void )
{
    RNGCONbits.TRNGMODE = 1;
    RNGCONbits.TRNGEN = 1;
}


/*-----------------------------------------------------------*/

static uint64_t PIC32MZ_HW_TRNG_Get( void )
{
    /* make sure that the TRNG has finished */
    while( RNGCNT < 64 )
    {
    }

    return RNGSEED1 + ( ( uint64_t ) RNGSEED2 << 32 );
}
