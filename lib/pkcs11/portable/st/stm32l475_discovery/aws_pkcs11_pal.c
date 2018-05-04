/*
 * Amazon FreeRTOS PKCS#11 for STM32L4 Discovery kit IoT node V1.0.1
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


#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE    2048
#define pkcs11OBJECT_FLASH_CERT_PRESENT      ( 0xABCDEFuL )


/**
 * @brief Structure for certificates/key storage.
 */
typedef struct
{
    CK_CHAR cDeviceCertificate[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_CHAR cDeviceKey[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_ULONG ulDeviceCertificateMark;
    CK_ULONG ulDeviceKeyMark;
} P11KeyConfig_t;


/**
 * @brief Certificates/key storage in flash.
 */
P11KeyConfig_t P11KeyConfig __attribute__( ( section( "UNINIT_FIXED_LOC" ) ) );
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
    CK_RV xResult = pdFALSE;
    CK_RV xBytesWritten = 0;
    CK_ULONG ulFlashMark = pkcs11OBJECT_FLASH_CERT_PRESENT;

    /*
     * write client certificate.
     */

    if( strncmp( pcFileName,
                 pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
                 strlen( pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) ) == 0 )
    {
        xBytesWritten = FLASH_update( ( uint32_t ) P11KeyConfig.cDeviceCertificate,
                                      pucData,
                                      ( ulDataSize + 1 ) ); /*Include '\0'*/

        if( xBytesWritten == ( ulDataSize + 1 ) )
        {
            xResult = pdTRUE;

            /*change flash written mark'*/
            FLASH_update( ( uint32_t ) &P11KeyConfig.ulDeviceCertificateMark,
                          &ulFlashMark,
                          sizeof( CK_ULONG ) );
        }
    }

    /*
     * write client key.
     */

    if( strncmp( pcFileName,
                 pkcs11configFILE_NAME_KEY,
                 strlen( pkcs11configFILE_NAME_KEY ) ) == 0 )
    {
        xBytesWritten = FLASH_update( ( uint32_t ) P11KeyConfig.cDeviceKey,
                                      pucData,
                                      ulDataSize + 1 ); /*Include '\0'*/

        if( xBytesWritten == ( ulDataSize + 1 ) )
        {
            xResult = pdTRUE;

            /*change flash written mark'*/
            FLASH_update( ( uint32_t ) &P11KeyConfig.ulDeviceKeyMark,
                          &ulFlashMark,
                          sizeof( CK_ULONG ) );
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

    /*
     * Read client certificate.
     */

    if( strncmp( pcFileName,
                 pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
                 strlen( pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) ) == 0 )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceCertificateMark == pkcs11OBJECT_FLASH_CERT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceCertificate;
            *pulDataSize = ( uint32_t ) strlen( ( const char * ) P11KeyConfig.cDeviceCertificate ) + 1;
            xResult = pdTRUE;
        }
    }

    /*
     * Read client key.
     */

    if( strncmp( pcFileName,
                 pkcs11configFILE_NAME_KEY,
                 strlen( pkcs11configFILE_NAME_KEY ) ) == 0 )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_CERT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceKey;
            *pulDataSize = ( uint32_t ) strlen( ( const char * ) P11KeyConfig.cDeviceKey ) + 1;
            xResult = pdTRUE;
        }
    }

    return xResult;
}
/*-----------------------------------------------------------*/

/**
 * @brief Cleanup after ReadFile.
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
