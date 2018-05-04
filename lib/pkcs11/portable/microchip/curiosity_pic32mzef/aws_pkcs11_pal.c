/*
 * Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
 *
 * Amazon FreeRTOS PKCS#11 for Curiosity PIC32MZEF V1.0.2
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
    P11CertData_t xPublisherCertificate;
    P11CertData_t xPublisherKey;
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
 * @brief Writes a file to local storage.
 *
 * Port-specific file write for crytographic information.
 *
 * @param[in] pcFileName    The name of the file to be written to.
 * @param[in] pucData       Data buffer to be written to file
 * @param[in] pulDataSize   Size (in bytes) of file data.
 *
 * @return pdTRUE if data was saved successfully to file,
 * pdFALSE otherwise.
 */
BaseType_t PKCS11_PAL_SaveFile( char * pcFileName,
                                uint8_t * pucData,
                                uint32_t ulDataSize )
{
    uint32_t * pFlashDest, * pDataSrc;
    int rowIx, nRows;
    const P11CertData_t * pCertFlash;
    P11CertData_t * pCertSave = 0;
    CK_RV xResult = pdFALSE;

    const P11KeyConfig_t * P11ConfigFlashPtr = ( const P11KeyConfig_t * ) KVA0_TO_KVA1( PKCS11_CERTIFICATE_SECTION_START_ADDRESS );
    P11KeyConfig_t * P11ConfigSavePtr = ( P11KeyConfig_t * ) KVA0_TO_KVA1( ( uint32_t ) &P11ConfigSave );


    if( ulDataSize <= sizeof( pCertSave->cCertificateData ) )
    {   /* enough room to store the certificate */
        if( strcmp( pcFileName, pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) == 0 )
        {
            pCertSave = &P11ConfigSavePtr->xDeviceCertificate;
            pCertFlash = &P11ConfigFlashPtr->xDeviceCertificate;
        }
        else if( strcmp( pcFileName, pkcs11configFILE_NAME_KEY ) == 0 )
        {
            pCertSave = &P11ConfigSavePtr->xDeviceKey;
            pCertFlash = &P11ConfigFlashPtr->xDeviceKey;
        }
        else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_CERTIFICATE ) == 0 )
        {
            pCertSave = &P11ConfigSavePtr->xPublisherCertificate;
            pCertFlash = &P11ConfigFlashPtr->xPublisherCertificate;
        }
        else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_KEY ) == 0 )
        {
            pCertSave = &P11ConfigSavePtr->xPublisherKey;
            pCertFlash = &P11ConfigFlashPtr->xPublisherKey;
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
            if( memcmp( pCertFlash, pCertSave, ulDataSize ) == 0 )
            {
                xResult = pdTRUE;
            }
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/


/**
 * @brief Reads a file from local storage.
 *
 * Port-specific file access for crytographic information.
 *
 * @sa PKCS11_ReleaseFileData
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 *
 * @return pdTRUE if data was retrieved successfully from files,
 * pdFALSE otherwise.
 */
BaseType_t PKCS11_PAL_ReadFile( char * pcFileName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize )
{
    CK_RV xResult = pdFALSE;
    uint32_t certSize = 0;
    uint8_t * pCertData = 0;
    const P11CertData_t * pCertFlash = 0;

    const P11KeyConfig_t * P11ConfigFlashPtr = ( const P11KeyConfig_t * ) KVA0_TO_KVA1( PKCS11_CERTIFICATE_SECTION_START_ADDRESS );

    if( strcmp( pcFileName, pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) == 0 )
    {
        pCertFlash = &P11ConfigFlashPtr->xDeviceCertificate;
    }
    else if( strcmp( pcFileName, pkcs11configFILE_NAME_KEY ) == 0 )
    {
        pCertFlash = &P11ConfigFlashPtr->xDeviceKey;
    }
    else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_CERTIFICATE ) == 0 )
    {
        pCertFlash = &P11ConfigFlashPtr->xPublisherCertificate;
    }
    else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_KEY ) == 0 )
    {
        pCertFlash = &P11ConfigFlashPtr->xPublisherKey;
    }

    if( ( pCertFlash != 0 ) && ( pCertFlash->ulCertificatePresent == pkcs11OBJECT_FLASH_CERT_PRESENT ) )
    {
        pCertData = ( uint8_t * ) pCertFlash->cCertificateData;
        certSize = pCertFlash->ulCertificateSize;
        xResult = pdTRUE;
    }

    *pulDataSize = certSize;
    *ppucData = pCertData;

    return xResult;
}


/**
 * @brief Cleanup after PKCS11_ReadFile().
 *
 * @param[in] pucBuffer The buffer to free.
 * @param[in] ulBufferSize The length of the above buffer.
 */
void PKCS11_PAL_ReleaseFileData( uint8_t * pucBuffer,
                                 uint32_t ulBufferSize )
{
    /* Unused parameters. */
    ( void ) pucBuffer;
    ( void ) ulBufferSize;

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
