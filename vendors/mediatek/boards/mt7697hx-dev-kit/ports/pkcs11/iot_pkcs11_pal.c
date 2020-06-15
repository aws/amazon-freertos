/* Copyright Statement:
 *
 * FreeRTOS PKCS glue layer.
 * (c) 2018 MediaTek Inc. or its affiliates. All Rights Reserved.
 *
 * FreeRTOS PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.1
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

/**
 * @file iot_pkcs11_pal.c
 * @brief MT7697H specific helpers for PKCS11 Interface.
 */

/*-----------------------------------------------------------*/

/* FreeRTOS Includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "iot_crypto.h"
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* flash driver includes. */
#include "hal_file.h"

#include "iot_pkcs11_config.h"

/*-----------------------------------------------------------*/

#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "FreeRTOS_P11_Certificate.dat"
#define pkcs11palFILE_NAME_KEY                   "FreeRTOS_P11_Key.dat"
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "FreeRTOS_P11_CodeSignKey.dat"

/*-----------------------------------------------------------*/

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

/*-----------------------------------------------------------*/

struct dict_entry
{
    char                    *label;
    char                    *filename;
    CK_OBJECT_HANDLE        xHandle;
    CK_BBOOL                isPrivate;
} dict[] = {
    {
        pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
        pkcs11palFILE_NAME_CLIENT_CERTIFICATE,
        eAwsDeviceCertificate,
        CK_FALSE
    },
    {
        pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
        pkcs11palFILE_NAME_KEY, // why same as pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS?
        eAwsDevicePrivateKey,
        CK_TRUE
    },
    {
        pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
        pkcs11palFILE_NAME_KEY, // why same as pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS?
        eAwsDevicePublicKey,
        CK_FALSE
    },
    {
        pkcs11configLABEL_CODE_VERIFICATION_KEY,
        pkcs11palFILE_CODE_SIGN_PUBLIC_KEY,
        eAwsCodeSigningKey,
        CK_FALSE
    }
};

#define DICT_SIZE   ( sizeof( dict ) / sizeof ( struct dict_entry ) )

/*-----------------------------------------------------------*/

/* Converts a label to its respective filename and handle. */
void prvLabelToFilenameHandle( uint8_t              *pcLabel,
                               char                 **pcFileName,
                               CK_OBJECT_HANDLE_PTR pHandle )
{
    int i;

    if( pcLabel != NULL )
    {
        /* Translate from the PKCS#11 label to local storage file name. */

        for( i = 0; i < DICT_SIZE; i++ )
        {
            if( 0 == strcmp( dict[ i ].label, (const char *)pcLabel ) )
            {
                *pcFileName = &dict[ i ].filename[0];
                *pHandle    = dict[ i ].xHandle;
                return;
            }
        }
    }

    *pcFileName = NULL;
    *pHandle    = eInvalidHandle;
}

/*-----------------------------------------------------------*/

/* Converts a label to its respective filename and handle. */
void prvHandleToFilenamePrivate( CK_OBJECT_HANDLE   xHandle,
                                 char               **pcFileName,
                                 CK_BBOOL           *pIsPrivate )
{
    int i;

    /*
     * Translate from the PKCS#11 handle to local storage file name and
     * private requirement.
     */

    for( i = 0; i < DICT_SIZE; i++ )
    {
        if( dict[ i ].xHandle == xHandle )
        {
            if( pcFileName )
            {
                *pcFileName = &dict[ i ].filename[ 0 ];
            }

            if( pIsPrivate )
            {
                *pIsPrivate = dict[ i ].isPrivate;
            }

            return;
        }
    }

    *pcFileName = NULL;
    *pIsPrivate = CK_FALSE;
}

/*-----------------------------------------------------------*/

CK_RV PKCS11_PAL_Initialize( void )
{
    return CKR_OK;
}

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
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    char                *pcFileName = NULL;
    CK_OBJECT_HANDLE    xHandle     = eInvalidHandle;

    /* Converts a label to its respective filename and handle. */

    prvLabelToFilenameHandle( pxLabel->pValue, &pcFileName, &xHandle );

    /* If your project requires additional PKCS#11 objects, add them here. */

    if( pcFileName != NULL )
    {
        if( hal_file_write( pcFileName, pucData, ulDataSize ) == pdFALSE )
        {
            xHandle = eInvalidHandle;
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
 * @param[in] pxLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */

CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    /* Avoid compiler warnings about unused variables. */
    ( void ) usLength;

    char                *pcFileName = NULL;
    CK_OBJECT_HANDLE    xHandle     = eInvalidHandle;

    /* Converts a label to its respective filename and handle. */

    prvLabelToFilenameHandle( pxLabel, &pcFileName, &xHandle );

    if( pcFileName != NULL )
    {
        /* Check if object exists/has been created before returning. */

        if( pdTRUE != hal_file_find( pcFileName, NULL ) )
        {
            xHandle = eInvalidHandle;
        }
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
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
                                      CK_BBOOL * pIsPrivate )
{
    CK_RV       ulReturn        = CKR_OK;
    char        *pcFileName     = NULL;
    uint8_t     *pucData;

    prvHandleToFilenamePrivate( xHandle, &pcFileName, pIsPrivate );
    if( NULL == pcFileName )
    {
        ulReturn = CKR_KEY_HANDLE_INVALID;
    }
    else
    {
        pucData = pvPortMalloc( MAX_FILE_SIZE );
        if( NULL == pucData )
        {
            ulReturn = CKR_DEVICE_MEMORY;
        }
        else
        {
            uint32_t    status;

            status = hal_file_read( pcFileName, pucData, pulDataSize );
            if ( status != pdTRUE )
            {
                vPortFree( pucData );
                ulReturn = CKR_FUNCTION_FAILED;
            }
            else
            {
                *ppucData = pucData;
            }
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
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    /* Unused parameters. */
    ( void ) ulDataSize;

    if( NULL != pucData )
    {
        vPortFree( pucData );
    }
    else
    {

    }
}

/*-----------------------------------------------------------*/

#include <string.h>
#include <hal_trng.h>

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
    hal_trng_status_t   st;
    uint32_t            random_number = 0;

    st = hal_trng_get_generated_random_number(&random_number);
    if (st == HAL_TRNG_STATUS_OK) {
        memcpy(output, &random_number, sizeof(uint32_t));
        *olen = sizeof(uint32_t);
    }

    return 0;
}

