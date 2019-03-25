/*
 * Amazon FreeRTOS PKCS #11 PAL V1.0.0
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

/**
 * @file aws_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "aws_pkcs11.h"
#include "FreeRTOS.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#include <string.h>
#include "wm_config.h"
#include "wm_type_def.h"
#include "wm_flash_map.h"
#include "wm_internal_flash.h"
#include "random.h"

#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "P11_Cert"
#define pkcs11palFILE_NAME_KEY                   "P11_Key"
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "P11_CSK"

#define PKCS11_PAL_FILE_MAX_LEN     (2048 + 4)
#define PKCS11_PAL_FILE_CERT_ADDR   (USER_ADDR_START)
#define PKCS11_PAL_FILE_KEY_ADDR    (PKCS11_PAL_FILE_CERT_ADDR + PKCS11_PAL_FILE_MAX_LEN)
#define PKCS11_PAL_FILE_CSK_ADDR    (PKCS11_PAL_FILE_KEY_ADDR  + PKCS11_PAL_FILE_MAX_LEN)

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};
/*-----------------------------------------------------------*/


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
            *pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
            *pHandle = eAwsCodeSigningKey;
        }
        else
        {
            *pcFileName = NULL;
            *pHandle = eInvalidHandle;
        }
    }
}

/**
 * @brief Writes a file to local storage.
 *
 * Port-specific file write for crytographic information.
 *
 * @param[in] pxLabel       Label of the object to be saved.
 * @param[in] pucData       Data buffer to be written to file
 * @param[in] ulDataSize    Size (in bytes) of data to be saved.
 *
 * @return The file handle of the object that was stored.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    int ret = -1;
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pxLabel->pValue,
                              &pcFileName,
                              &xHandle );

    if (!strcmp(pcFileName, pkcs11palFILE_NAME_CLIENT_CERTIFICATE))
    {
        ret  = tls_fls_write(PKCS11_PAL_FILE_CERT_ADDR, (u8 *)&ulDataSize, 4);
        ret |= tls_fls_write(PKCS11_PAL_FILE_CERT_ADDR + 4, pucData, ulDataSize);
    }
    else if (!strcmp(pcFileName, pkcs11palFILE_NAME_KEY))
    {
        ret  = tls_fls_write(PKCS11_PAL_FILE_KEY_ADDR, (u8 *)&ulDataSize, 4);
        ret |= tls_fls_write(PKCS11_PAL_FILE_KEY_ADDR + 4, pucData, ulDataSize);
    }
    else if (!strcmp(pcFileName, pkcs11palFILE_CODE_SIGN_PUBLIC_KEY))
    {
        ret  = tls_fls_write(PKCS11_PAL_FILE_CSK_ADDR, (u8 *)&ulDataSize, 4);
        ret |= tls_fls_write(PKCS11_PAL_FILE_CSK_ADDR + 4, pucData, ulDataSize);
    }

    //printf("Writing file %s, %d bytes, %s\r\n", pcFileName, ulDataSize, ret ? "failed" : "successfully");
    //sdfsfa(pucData, ulDataSize);
        /* TODO: save to flash */
    return xHandle;
}

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pxLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pcLabel,
                                        uint8_t usLength )
{
    int ret = -1;
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pcLabel,
                              &pcFileName,
                              &xHandle );

    if( pcFileName != NULL )
    {
        u32 ulDataSize = 0;
        if (!strcmp(pcFileName, pkcs11palFILE_NAME_CLIENT_CERTIFICATE))
        {
            ret = tls_fls_read(PKCS11_PAL_FILE_CERT_ADDR, (u8 *)&ulDataSize, 4);
        }
        else if (!strcmp(pcFileName, pkcs11palFILE_NAME_KEY))
        {
            ret = tls_fls_read(PKCS11_PAL_FILE_KEY_ADDR, (u8 *)&ulDataSize, 4);
        }
        else if (!strcmp(pcFileName, pkcs11palFILE_CODE_SIGN_PUBLIC_KEY))
        {
            ret = tls_fls_read(PKCS11_PAL_FILE_CSK_ADDR, (u8 *)&ulDataSize, 4);
        }

        if (ret || (0xFFFFFFFF == ulDataSize))
        {
            xHandle = eInvalidHandle;
        }

        //printf( "Finding file %s, %d bytes, %s\r\n", pcFileName, ulDataSize, xHandle ? "successfully" : "failed");
        /* TODO: read form flash  */
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
                                 CK_BBOOL * pIsPrivate )
{
    int ret = -1;
    uint8_t *data = NULL;
    char * pcFileName = NULL;
    CK_RV ulReturn = CKR_OK;

    if( xHandle == eAwsDeviceCertificate )
    {
        pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsDevicePrivateKey )
    {
        pcFileName = pkcs11palFILE_NAME_KEY;
        *pIsPrivate = CK_TRUE;
    }
    else if( xHandle == eAwsDevicePublicKey )
    {
        /* Public and private key are stored together in same file. */
        pcFileName = pkcs11palFILE_NAME_KEY;
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsCodeSigningKey )
    {
        pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
        *pIsPrivate = CK_FALSE;
    }
    else
    {
        ulReturn = CKR_OBJECT_HANDLE_INVALID;
    }

    if (ulReturn == CKR_OK)
    {
        if (!strcmp(pcFileName, pkcs11palFILE_NAME_CLIENT_CERTIFICATE))
        {
            ret  = tls_fls_read(PKCS11_PAL_FILE_CERT_ADDR, (u8 *)pulDataSize, 4);

            if (!ret)
            {
                data = pvPortMalloc(*pulDataSize);
                if (!data)
                {
                    return CKR_HOST_MEMORY;
                }
            }
            else
            {
                return CKR_FUNCTION_FAILED;
            }

            ret = tls_fls_read(PKCS11_PAL_FILE_CERT_ADDR + 4, data, *pulDataSize);
            if (!ret)
            {
                *ppucData = data;
            }
            else
            {
                vPortFree( data );
                return CKR_FUNCTION_FAILED;
            }
        }
        else if (!strcmp(pcFileName, pkcs11palFILE_NAME_KEY))
        {
            ret  = tls_fls_read(PKCS11_PAL_FILE_KEY_ADDR, (u8 *)pulDataSize, 4);

            if (!ret)
            {
                data = pvPortMalloc(*pulDataSize);
                if (!data)
                {
                    return CKR_HOST_MEMORY;
                }
            }
            else
            {
                return CKR_FUNCTION_FAILED;
            }

            ret = tls_fls_read(PKCS11_PAL_FILE_KEY_ADDR + 4, data, *pulDataSize);
            if (!ret)
            {
                *ppucData = data;
            }
            else
            {
                vPortFree( data );
                return CKR_FUNCTION_FAILED;
            }
        }
        else if (!strcmp(pcFileName, pkcs11palFILE_CODE_SIGN_PUBLIC_KEY))
        {
            ret  = tls_fls_read(PKCS11_PAL_FILE_CSK_ADDR, (u8 *)pulDataSize, 4);

            if (!ret)
            {
                data = pvPortMalloc(*pulDataSize);
                if (!data)
                {
                    return CKR_HOST_MEMORY;
                }
            }
            else
            {
                return CKR_FUNCTION_FAILED;
            }

            ret = tls_fls_read(PKCS11_PAL_FILE_CSK_ADDR + 4, data, *pulDataSize);
            if (!ret)
            {
                *ppucData = data;
            }
            else
            {
                vPortFree( data );
                return CKR_FUNCTION_FAILED;
            }
        }
        else
        {
            ulReturn = CKR_OBJECT_HANDLE_INVALID;
        }

        //printf("Reading file %s, %d bytes, %s\r\n", pcFileName, *pulDataSize, ulReturn ? "failed" : "successfully");
        //sdfsfa(*ppucData, *pulDataSize);
    }

    return ulReturn;
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
    ( void ) ulDataSize;

    if( pucData != NULL )
    {
        vPortFree( pucData );
    }
}



/*-----------------------------------------------------------*/

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
    random_get_bytes(output, len);
    *olen = len;
    return 0;
}
