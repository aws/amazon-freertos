/*
 * Amazon FreeRTOS PKCS #11 PAL for CC3220SF-LAUNCHXL V1.0.3
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


/**
 * @file iot_pkcs11_pal.c
 * @brief PKCS#11 implementation for CC3220SF-LAUNCHXL. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "iot_pkcs11.h"
#include "aws_clientcredential.h"
#include "aws_secure_sockets_config.h"
#include "iot_pkcs11_config.h"
#include "iot_pkcs11_pal.h"

/* flash driver includes. */
#include <ti/drivers/net/wifi/simplelink.h>

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/base64.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    eAwsTrustedServerCertificate,
    eAwsJITPCertificate
};

/**
 * @brief Save file to flash
 */
static BaseType_t prvSaveFile( char * pcFileName,
                               char * pucData,
                               uint32_t ulDataSize );

/*-----------------------------------------------------------*/

static BaseType_t prvSaveFile( char * pcFileName,
                               char * pucData,
                               uint32_t ulDataSize )
{
    CK_RV xResult = pdPASS;
    int32_t lFileHdl;
    unsigned long ulMasterToken = 0;
    const uint16_t usMaxSize = 2000;
    BaseType_t xCheck;

    lFileHdl = sl_FsOpen( ( const unsigned char * ) pcFileName,
                          SL_FS_CREATE | SL_FS_CREATE_NOSIGNATURE | SL_FS_OVERWRITE | SL_FS_CREATE_MAX_SIZE( usMaxSize ), /*lint !e9029 !e9027  offending macros in a TI file. */
                          &ulMasterToken );

    if( lFileHdl < 0 )
    {
        xResult = pdFAIL;
        configPRINTF( ( "Failed creating file %s...\r\n", pcFileName ) );
    }

    if( xResult == pdPASS )
    {
        xCheck = sl_FsWrite( lFileHdl,
                             0U,
                             ( unsigned char * ) pucData, /*lint !e605 cert bytes must be cast as unsigned for compliance with TI file. */
                             ulDataSize );

        if( xCheck < 0 )
        {
            xResult = pdFAIL;
        }
    }

    if( lFileHdl >= 0 )
    {
        xCheck = sl_FsClose( lFileHdl, NULL, NULL, 0 );
    }

    if( xCheck < 0 )
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

#define BEGIN_EC_PRIVATE_KEY     "-----BEGIN EC PRIVATE KEY-----\n"
#define END_EC_PRIVATE_KEY       "-----END EC PRIVATE KEY-----\n"
#define BEGIN_RSA_PRIVATE_KEY    "-----BEGIN RSA PRIVATE KEY-----\n"
#define END_RSA_PRIVATE_KEY      "-----END RSA PRIVATE KEY-----\n"
#define BEGIN_CERTIFICATE        "-----BEGIN CERTIFICATE-----\n"
#define END_CERTIFICATE          "-----END CERTIFICATE-----\n"
#define NUM_CHAR_PER_PEM_LINE    64

/*
 * @brief Converts DER objects to PEM objects.
 *
 * \note Only elliptic curve and RSA private keys are supported.
 *
 * \param[in]   pDerBuffer      A pointer the DER object.
 * \param[in]   xDerLength      The length of the DER object (bytes)
 * \param[out]  ppcPemBuffer    A pointer to the buffer that will be allocated
 *                              for the PEM object.  This function performs
 *                              a memory allocation for this buffer, and the
 *                              caller MUST free the buffer.
 * \param[out]  pPemLength      Length of the PEM object
 * \param[in]   xObjectType     Type of object being converted to PEM.
 *                              Valid values are CKO_PRIVATE_KEY and
 *                              CKO_CERTIFICATE.
 */
static CK_RV prvDerToPem( uint8_t * pDerBuffer,
                          size_t xDerLength,
                          char ** ppcPemBuffer,
                          size_t * pPemLength,
                          CK_OBJECT_CLASS xObjectType )
{
    CK_RV xReturn = CKR_OK;
    size_t xTotalPemLength = 0;
    mbedtls_pk_context pk;
    mbedtls_pk_type_t xKeyType;
    char * pcHeader;
    char * pcFooter;
    unsigned char * pemBodyBuffer;
    uint8_t * pFinalBufferPlaceholder;
    int ulLengthOfContentsCopiedSoFar = 0;
    int ulBytesInLine = 0;
    int ulBytesRemaining;

    if( xObjectType == CKO_PRIVATE_KEY )
    {
        mbedtls_pk_init( &pk );
        /* Parse key. */
        xReturn = mbedtls_pk_parse_key( &pk, pDerBuffer, xDerLength, NULL, 0 );

        if( xReturn != 0 )
        {
            xReturn = CKR_ATTRIBUTE_VALUE_INVALID;
        }

        /* Get key algorithm. */
        xKeyType = mbedtls_pk_get_type( &pk );

        switch( xKeyType )
        {
            case ( MBEDTLS_PK_RSA ):
            case ( MBEDTLS_PK_RSA_ALT ):
                pcHeader = BEGIN_RSA_PRIVATE_KEY;
                pcFooter = END_RSA_PRIVATE_KEY;
                xTotalPemLength = strlen( BEGIN_RSA_PRIVATE_KEY ) + strlen( END_RSA_PRIVATE_KEY );
                break;

            case ( MBEDTLS_PK_ECKEY ):
            case ( MBEDTLS_PK_ECKEY_DH ):
            case ( MBEDTLS_PK_ECDSA ):
                pcHeader = BEGIN_EC_PRIVATE_KEY;
                pcFooter = END_EC_PRIVATE_KEY;
                xTotalPemLength = strlen( BEGIN_EC_PRIVATE_KEY ) + strlen( END_EC_PRIVATE_KEY );
                break;

            default:
                xReturn = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
        }

        mbedtls_pk_free( &pk );
    }
    else /* Certificate object. */
    {
        pcHeader = BEGIN_CERTIFICATE;
        pcFooter = END_CERTIFICATE;
        xTotalPemLength = strlen( BEGIN_CERTIFICATE ) + strlen( END_CERTIFICATE );
    }

    if( xReturn == CKR_OK )
    {
        /* A PEM object has a header, body, and footer. */

        /*
         * ------- BEGIN SOMETHING --------\n
         * BODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODY\n
         * BODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODY\n
         * BODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODY\n
         * ....
         * ------- END SOMETHING ---------\n
         */

        /* Determine the length of the Base 64 encoded body. */
        xReturn = mbedtls_base64_encode( NULL, 0, pPemLength, pDerBuffer, xDerLength );

        if( ( int32_t ) xReturn != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL )
        {
            xReturn = CKR_ATTRIBUTE_VALUE_INVALID;
        }
        else
        {
            xReturn = 0;
        }

        if( xReturn == 0 )
        {
            /* Allocate memory for the PEM contents (excluding header, footer, newlines). */
            pemBodyBuffer = pvPortMalloc( *pPemLength );

            if( pemBodyBuffer == NULL )
            {
                xReturn = CKR_HOST_MEMORY;
            }
        }

        if( xReturn == 0 )
        {
            /* Convert the body contents from DER to PEM. */
            xReturn = mbedtls_base64_encode( pemBodyBuffer,
                                             *pPemLength,
                                             pPemLength,
                                             pDerBuffer,
                                             xDerLength );
        }

        if( xReturn == 0 )
        {
            /* Calculate the length required for the entire PEM object. */
            xTotalPemLength += *pPemLength;
            /* Add in space for the newlines. */
            xTotalPemLength += ( *pPemLength ) / NUM_CHAR_PER_PEM_LINE;

            if( ( *pPemLength ) % NUM_CHAR_PER_PEM_LINE != 0 )
            {
                xTotalPemLength += 1;
            }

            /* Allocate space for the full PEM certificate, including header, footer, and newlines.
             * This space must be freed by the application. */
            *ppcPemBuffer = pvPortMalloc( xTotalPemLength );

            if( *ppcPemBuffer == NULL )
            {
                xReturn = CKR_HOST_MEMORY;
            }
        }

        if( xReturn == 0 )
        {
            /* Copy the header. */
            pFinalBufferPlaceholder = ( uint8_t * ) *ppcPemBuffer;
            memcpy( pFinalBufferPlaceholder, pcHeader, strlen( pcHeader ) );
            pFinalBufferPlaceholder += strlen( pcHeader );

            /* Copy the Base64 encoded contents into the final buffer 64 bytes at a time, adding newlines */
            while( ulLengthOfContentsCopiedSoFar < *pPemLength )
            {
                ulBytesRemaining = *pPemLength - ulLengthOfContentsCopiedSoFar;
                ulBytesInLine = ( ulBytesRemaining > NUM_CHAR_PER_PEM_LINE ) ? NUM_CHAR_PER_PEM_LINE : ulBytesRemaining;
                memcpy( pFinalBufferPlaceholder,
                        pemBodyBuffer + ulLengthOfContentsCopiedSoFar,
                        ulBytesInLine );
                pFinalBufferPlaceholder += ulBytesInLine;
                *pFinalBufferPlaceholder = '\n';
                pFinalBufferPlaceholder++;

                ulLengthOfContentsCopiedSoFar += ulBytesInLine;
            }
        }

        if( pemBodyBuffer != NULL )
        {
            vPortFree( pemBodyBuffer );
        }

        /* Copy the footer. */
        memcpy( pFinalBufferPlaceholder, pcFooter, strlen( pcFooter ) );

        /* Update the total length of the PEM object returned by the function. */
        *pPemLength = xTotalPemLength;
    }

    return xReturn;
}

/* Converts a label to its respective filename and handle. */
void prvLabelToFilenameHandle( uint8_t * pcLabel,
                               char ** pcFileName,
                               CK_OBJECT_HANDLE_PTR pHandle )
{
    if( pcLabel != NULL )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pcLabel,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            *pcFileName = socketsconfigSECURE_FILE_NAME_CLIENTCERT;
            *pHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pcFileName = socketsconfigSECURE_FILE_NAME_PRIVATEKEY;
            *pHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_ROOT_CERTIFICATE,
                              sizeof( pkcs11configLABEL_ROOT_CERTIFICATE ) ) )
        {
            *pcFileName = socketsconfigSECURE_FILE_NAME_ROOTCA;
            *pHandle = eAwsTrustedServerCertificate;
        }
        else
        {
            *pcFileName = NULL;
            *pHandle = eInvalidHandle;
        }
    }
}

/*-----------------------------------------------------------*/

void prvHandleToFileName( CK_OBJECT_HANDLE pxHandle,
                          char ** pcFileName )
{
    switch( pxHandle )
    {
        case ( eAwsDeviceCertificate ):
            *pcFileName = socketsconfigSECURE_FILE_NAME_CLIENTCERT;
            break;

        case ( eAwsDevicePrivateKey ):
            *pcFileName = socketsconfigSECURE_FILE_NAME_PRIVATEKEY;
            break;

        case ( eAwsTrustedServerCertificate ):
            *pcFileName = socketsconfigSECURE_FILE_NAME_ROOTCA;
            break;

        default:
            *pcFileName = NULL;
            break;
    }
}

/*-----------------------------------------------------------*/

/* PKCS #11 PAL Implementation. */

/**
 * @brief Delete an object from non-volatile storage.
 *
 * @param[in] xHandle    Handle of the object to be destroyed.
 *
 * @return CKR_OK if object was successfully destroyed.
 * Otherwise, PKCS #11-style error code.
 *
 */
#if ( pkcs11configPAL_DESTROY_SUPPORTED == 1 )
    CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
    {
        ( void ) xHandle;
        return CKR_FUNCTION_NOT_SUPPORTED;
    }
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pxTemplate     Pointer to the template of the object
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
    char * pcFileName = NULL;
    int16_t iStatus = 0;
    SlFsFileInfo_t FsFileInfo = { 0 };

    /* Converts a label to its respective filename and handle. */
    prvLabelToFilenameHandle( pLabel,
                              &pcFileName,
                              &xHandle );

    if( pcFileName != NULL )
    {
        /* Check if object exists/has been created before returning. */
        iStatus = sl_FsGetInfo( ( const unsigned char * ) pcFileName, 0, &FsFileInfo );
    }

    if( SL_ERROR_FS_FILE_NOT_EXISTS == iStatus )
    {
        xHandle = eInvalidHandle;
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

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
                                      CK_BBOOL * xIsPrivate )
{
    CK_RV ulReturn = CKR_OK;
    int32_t iReadBytes = 0;
    int32_t iFile = 0;
    char * pcFileName = NULL;
    SlFsFileInfo_t FsFileInfo = { 0 };

    prvHandleToFileName( xHandle, &pcFileName );

    if( xHandle == eAwsDevicePrivateKey )
    {
        *xIsPrivate = CK_TRUE;
    }
    else
    {
        *xIsPrivate = CK_FALSE;
    }

    if( pcFileName == NULL )
    {
        ulReturn = CKR_FUNCTION_FAILED;
    }

    if( 0 == ulReturn )
    {
        /* Check if the file exists and, if so, how big it is. */
        if( 0 != sl_FsGetInfo( ( const unsigned char * ) pcFileName, 0, &FsFileInfo ) )
        {
            ulReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( 0 == ulReturn )
    {
        /* Create a buffer. */
        *pulDataSize = FsFileInfo.Len;
        *ppucData = pvPortMalloc( *pulDataSize );

        if( NULL == *ppucData )
        {
            ulReturn = CKR_HOST_MEMORY;
        }
    }

    if( 0 == ulReturn )
    {
        /* Open the file. */
        iFile = sl_FsOpen( ( const unsigned char * ) pcFileName, SL_FS_READ, 0 );

        if( iFile < 0 )
        {
            ulReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( 0 == ulReturn )
    {
        /* Read the file. */
        iReadBytes = sl_FsRead( iFile, 0, *ppucData, *pulDataSize );

        if( iReadBytes != *pulDataSize )
        {
            ulReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( 0 != iFile )
    {
        sl_FsClose( iFile, NULL, NULL, 0 );
    }

    return ( BaseType_t ) ulReturn;
}

/*-----------------------------------------------------------*/

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
    ( void ) ulDataSize;

    if( NULL != pucData )
    {
        vPortFree( pucData );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Saves an object in non-volatile storage.
 *
 * Port-specific file write for cryptographic information.
 *
 * @param[in] pxTemplate    Structure containing searchable attributes.
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
    char * pcFileName = NULL;
    char * pcPemBuffer = NULL;
    size_t xPemLength = 0;
    int i;
    CK_BBOOL isDestroy = CK_FALSE;
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;

    /* Converts a label to its respective filename and handle. */
    prvLabelToFilenameHandle( ( uint8_t * ) pxLabel->pValue,
                              &pcFileName,
                              &xHandle );

    if( pcFileName != NULL )
    {
        if( 0 == prvDerToPem( pucData,
                              ulDataSize,
                              &pcPemBuffer,
                              &xPemLength,
                              ( eAwsDevicePrivateKey == xHandle ) ? CKO_PRIVATE_KEY : CKO_CERTIFICATE ) )
        {
            /* If the object type is valid, and the DER-to-PEM conversion
             * succeeded, try to write the data to flash. */
            if( pdFALSE == prvSaveFile( pcFileName,
                                        ( char * ) pucData,
                                        ulDataSize ) )
            {
                xHandle = eInvalidHandle;
            }
        }
        else
        {
            /* DestroyObject workaround is to overwrite the memory with zeros.
             * Check if this looks like a deliberate destroy call, and if so,
             * overwrite the file with all 0s. */
            isDestroy = CK_TRUE;

            for( i = 0; i < ulDataSize; i++ )
            {
                if( pucData[ i ] != 0 )
                {
                    isDestroy = CK_FALSE;
                    break;
                }
            }

            if( isDestroy == CK_TRUE )
            {
                if( pdFALSE == prvSaveFile( pcFileName,
                                            ( char * ) pucData,
                                            ulDataSize ) )
                {
                    xHandle = eInvalidHandle;
                }
            }
            else /* This is not a valid key or cert, and is also not a DestroyObject call. */
            {
                xHandle = eInvalidHandle;
            }

        }

        if( NULL != pcPemBuffer )
        {
            /* Free the temporary buffer used for conversion. */
            vPortFree( pcPemBuffer );
        }
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
    int lStatus = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;

    /* Use the SimpleLink driver to get a hardware-derived seed for additional
     * PRNG entropy. */
    *olen = len;

    if( 0 == sl_NetUtilGet( SL_NETUTIL_TRUE_RANDOM,
                            0,
                            output,
                            ( unsigned short int * ) olen ) )
    {
        lStatus = 0;
    }

    return lStatus;
}

/*-----------------------------------------------------------*/
