/*
 * Amazon FreeRTOS PKCS #11 PAL for STM32L4 Discovery kit IoT node V1.0.3
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
 * @file aws_pkcs11_pal.c
 * @brief Amazon FreeRTOS device specific helper functions for
 * PKCS#11 implementation based on mbedTLS.  This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
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

/* flash driver includes. */
#include "flash.h"

/* WiFi includes. */
#ifdef USE_OFFLOAD_SSL
    #include "aws_wifi.h"
/* mbedTLS includes. */
    #include "mbedtls/pk.h"
    #include "mbedtls/base64.h"
    #include "mbedtls/platform.h"
#endif

#define pkcs11OBJECT_MAX_SIZE         2048
#define pkcs11OBJECT_PRESENT_MAGIC    ( 0xABCD0000uL )
#define pkcs11OBJECT_LENGTH_MASK      ( 0x0000FFFFuL )
#define pkcs11OBJECT_PRESENT_MASK     ( 0xFFFF0000uL )

enum eObjectHandles
{
    eInvalidHandle = 0, /* From PKCS #11 spec: 0 is never a valid object handle.*/
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
    CK_CHAR cDeviceCertificate[ pkcs11OBJECT_MAX_SIZE ];
    CK_CHAR cDeviceKey[ pkcs11OBJECT_MAX_SIZE ];
    CK_CHAR cCodeSignKey[ pkcs11OBJECT_MAX_SIZE ];
    uint32_t ulDeviceCertificateMark;
    uint32_t ulDeviceKeyMark;
    uint32_t ulCodeSignKeyMark;
} P11KeyConfig_t;


/**
 * @brief Certificates/key storage in flash.
 */
P11KeyConfig_t P11KeyConfig __attribute__( ( section( "UNINIT_FIXED_LOC" ) ) );
/*-----------------------------------------------------------*/

#ifdef USE_OFFLOAD_SSL

/* Exported from aws_wifi.c */
    extern WIFIReturnCode_t WIFI_StoreCertificate( uint8_t * pucCertificate,
                                                   uint16_t usCertificateLength );
    extern WIFIReturnCode_t WIFI_StoreKey( uint8_t * pucKey,
                                           uint16_t usKeyLength );

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

            if( xReturn != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL )
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
                    xReturn = CKR_DEVICE_MEMORY;
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
                    xReturn = CKR_DEVICE_MEMORY;
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

#endif /* ifdef USE_OFFLOAD_SSL */
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
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    CK_RV xBytesWritten = 0;
    CK_RV xReturn;
    uint32_t ulFlashMark = ( pkcs11OBJECT_PRESENT_MAGIC | ( ulDataSize ) );
    uint8_t * pemBuffer;
    size_t pemLength;


    if( ulDataSize <= pkcs11OBJECT_MAX_SIZE )
    {
        /*
         * write client certificate.
         */
        if( strcmp( pxLabel->pValue,
                    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) == 0 )
        {
            xBytesWritten = FLASH_update( ( uint32_t ) P11KeyConfig.cDeviceCertificate,
                                          pucData,
                                          ( ulDataSize ) );

            if( xBytesWritten == ( ulDataSize ) )
            {
                xHandle = eAwsDeviceCertificate;

                /*change flash written mark'*/
                FLASH_update( ( uint32_t ) &P11KeyConfig.ulDeviceCertificateMark,
                              &ulFlashMark,
                              sizeof( uint32_t ) );
            }

            #ifdef USE_OFFLOAD_SSL

                /* If we are using offload SSL, write the certificate to the
                 * WiFi module as well. */

                xReturn = prvDerToPem( pucData,
                                       ulDataSize,
                                       ( char ** ) &pemBuffer,
                                       &pemLength,
                                       CKO_CERTIFICATE );

                if( xReturn != CKR_OK )
                {
                    xHandle = eInvalidHandle;
                }

                if( xHandle == eAwsDeviceCertificate )
                {
                    if( WIFI_StoreCertificate( pemBuffer, ( uint16_t ) pemLength ) != eWiFiSuccess )
                    {
                        xHandle = eInvalidHandle;
                    }
                }
                vPortFree( pemBuffer );
            #endif /* USE_OFFLOAD_SSL */
        }

        /*
         * write client key.
         */

        else if( strcmp( pxLabel->pValue,
                         pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) == 0 )
        {
            xBytesWritten = FLASH_update( ( uint32_t ) P11KeyConfig.cDeviceKey,
                                          pucData,
                                          ulDataSize );

            if( xBytesWritten == ( ulDataSize ) )
            {
                xHandle = eAwsDevicePrivateKey;
                /*change flash written mark'*/
                FLASH_update( ( uint32_t ) &P11KeyConfig.ulDeviceKeyMark,
                              &ulFlashMark,
                              sizeof( uint32_t ) );
            }

            #ifdef USE_OFFLOAD_SSL
                xReturn = prvDerToPem( pucData,
                                       ulDataSize,
                                       ( char ** ) &pemBuffer,
                                       &pemLength,
                                       CKO_PRIVATE_KEY );

                if( xReturn != CKR_OK )
                {
                    xHandle = eInvalidHandle;
                }

                /* If we are using offload SSL, write the key to the WiFi
                 * module as well. */
                if( xHandle == eAwsDevicePrivateKey )
                {
                    if( WIFI_StoreKey( pemBuffer, ( uint16_t ) pemLength ) != eWiFiSuccess )
                    {
                        xHandle = eInvalidHandle;
                    }
                }
                vPortFree( pemBuffer );
            #endif /* USE_OFFLOAD_SSL */
        }

        else if( strcmp( pxLabel->pValue,
                         pkcs11configLABEL_CODE_VERIFICATION_KEY ) == 0 )
        {
            xBytesWritten = FLASH_update( ( uint32_t ) P11KeyConfig.cCodeSignKey,
                                          pucData,
                                          ulDataSize );

            if( xBytesWritten == ( ulDataSize ) )
            {
                xHandle = eAwsCodeSigningKey;

                /*change flash written mark'*/
                FLASH_update( ( uint32_t ) &P11KeyConfig.ulCodeSignKeyMark,
                              &ulFlashMark,
                              sizeof( uint32_t ) );
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

    if( ( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, usLength ) ) &&
        ( ( P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
    {
        xHandle = eAwsDeviceCertificate;
    }
    else if( ( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, usLength ) ) &&
             ( ( P11KeyConfig.ulDeviceKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
    {
        xHandle = eAwsDevicePrivateKey;
    }
    else if( ( 0 == memcmp( pLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, usLength ) ) &&
             ( ( P11KeyConfig.ulCodeSignKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC ) )
    {
        xHandle = eAwsCodeSigningKey;
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
    CK_RV ulReturn = CKR_OBJECT_HANDLE_INVALID;

    /*
     * Read client certificate.
     */

    if( xHandle == eAwsDeviceCertificate )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( ( P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
        {
            *ppucData = P11KeyConfig.cDeviceCertificate;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_LENGTH_MASK;
            *pIsPrivate = CK_FALSE;
            ulReturn = CKR_OK;
        }
    }

    /*
     * Read client key.
     */

    else if( xHandle == eAwsDevicePrivateKey )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( ( P11KeyConfig.ulDeviceKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
        {
            *ppucData = P11KeyConfig.cDeviceKey;
            *pulDataSize = ( uint32_t ) ( P11KeyConfig.ulDeviceKeyMark & pkcs11OBJECT_LENGTH_MASK );
            *pIsPrivate = CK_TRUE;
            ulReturn = CKR_OK;
        }
    }

    else if( xHandle == eAwsDevicePublicKey )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( ( P11KeyConfig.ulDeviceKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
        {
            *ppucData = P11KeyConfig.cDeviceKey;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulDeviceKeyMark & pkcs11OBJECT_LENGTH_MASK;
            *pIsPrivate = CK_FALSE;
            ulReturn = CKR_OK;
        }
    }

    else if( xHandle == eAwsCodeSigningKey )
    {
        if( ( P11KeyConfig.ulCodeSignKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
        {
            *ppucData = P11KeyConfig.cCodeSignKey;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulCodeSignKeyMark & pkcs11OBJECT_LENGTH_MASK;
            *pIsPrivate = CK_FALSE;
            ulReturn = CKR_OK;
        }
    }

    return ulReturn;
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
    ( void ) pucData;
    ( void ) ulDataSize;

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}
/*-----------------------------------------------------------*/
