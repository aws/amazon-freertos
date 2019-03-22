/******************************************************************************
*
* Copyright (C) 2018 Xilinx, Inc.  All rights reserved.
*
* Amazon FreeRTOS PKCS #11 PAL for Xilinx Avnet MicroZed V1.0.0
* Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
******************************************************************************/

/**
 * @file aws_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"
#include "FreeRTOS.h"
#include "ff.h"
#include "xil_printf.h"
#include "task.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "FreeRTOS_P11_Certificate.dat"
#define pkcs11palFILE_NAME_KEY                   "FreeRTOS_P11_Key.dat"
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "FreeRTOS_P11_CodeSignKey.dat"

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

/* Converts a label to its respective filename and handle. */
void prvLabelToFilenameHandle( uint8_t * pcLabel,
                               uint8_t ** pcFileName,
                               CK_OBJECT_HANDLE_PTR pHandle )
{
    if( pcLabel != NULL )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pcLabel,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            *pcFileName = ( uint8_t * ) pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pcFileName = ( uint8_t * ) pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            *pcFileName = ( uint8_t * ) pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *pcFileName = ( uint8_t * ) pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
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
    static FIL fil;
    FRESULT Res;
    uint32_t n;
    uint8_t * pcFileName;
    CK_OBJECT_HANDLE xHandle;

    prvLabelToFilenameHandle( pxLabel->pValue,
                              &pcFileName,
                              &xHandle );

    if( xHandle != eInvalidHandle )
    {
        taskENTER_CRITICAL();
        Res = f_open( &fil, pcFileName, FA_CREATE_ALWAYS | FA_WRITE );

        if( Res )
        {
            taskEXIT_CRITICAL();
            xil_printf( "PKCS11_PAL_SaveObject ERROR: Unable to open file %s  Res %d\r\n", pcFileName, Res );
            xHandle = eInvalidHandle;
        }
        else
        {
            Res = f_write( &fil, pucData, ulDataSize, ( UINT * ) ( &n ) );
            f_close( &fil );
            taskEXIT_CRITICAL();

            if( ( n < ulDataSize ) || ( Res != 0 ) )
            {
                xil_printf( "PKCS11_PAL_SaveFile ERROR: Write to file %s failed  Res %d\r\n", pcFileName, Res );
                xHandle = eInvalidHandle;
            }
        }
    }

    return xHandle;
}

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
    uint8_t * pcFileName = NULL;
    static FIL fil;
    FRESULT Res;

    /* Converts a label to its respective filename and handle. */
    prvLabelToFilenameHandle( pLabel,
                              &pcFileName,
                              &xHandle );

    /* Check if object exists/has been created before returning. */
    taskENTER_CRITICAL();

    Res = f_open( &fil, ( TCHAR * ) pcFileName, FA_READ );

    if( Res )
    {
        taskEXIT_CRITICAL();
        xil_printf( "PKCS11_PAL_FindObject ERROR: File %s does not exist\r\n", pcFileName );
        return eInvalidHandle;
    }

    f_close( &fil );
    taskEXIT_CRITICAL();

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
    CK_RV ulReturn = CKR_OK;
    FRESULT Res;
    static FIL fil;
    uint32_t n;
    uint8_t * buf;
    char * pcFileName = NULL;

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
        return CKR_KEY_HANDLE_INVALID;
    }

    taskENTER_CRITICAL();

    Res = f_open( &fil, pcFileName, FA_READ );

    if( Res )
    {
        taskEXIT_CRITICAL();
        xil_printf( "PKCS11_PAL_GetObjectValue ERROR: Unable to open file %s  Res %d\r\n", pcFileName, Res );
        return CKR_FUNCTION_FAILED;
    }

    n = fil.fsize;
    buf = pvPortMalloc( n );

    if( buf == NULL )
    {
        taskEXIT_CRITICAL();
        xil_printf( "PKCS11_PAL_GetObjectValue ERROR: buf alloc failed \r\n" );
        return CKR_DEVICE_MEMORY;
    }

    Res = f_read( &fil, buf, n, ( UINT * ) pulDataSize );

    if( ( *pulDataSize == 0 ) || ( Res != 0 ) )
    {
        f_close( &fil );
        vPortFree( buf );
        taskEXIT_CRITICAL();
        xil_printf( "PKCS11_PAL_GetObjectValue ERROR: Read from file %s failed  Res %d\r\n", pcFileName, Res );
        return CKR_FUNCTION_FAILED;
    }

    if( Res != 0 )
    {
        f_close( &fil );
        vPortFree( buf );
        taskEXIT_CRITICAL();
        xil_printf( "PKCS11_PAL_GetObjectValue ERROR: Decryption of file %s failed  Res %d\r\n", pcFileName, Res );
        return CKR_FUNCTION_FAILED;
    }

    *ppucData = buf;
    f_close( &fil );
    taskEXIT_CRITICAL();

    return CKR_OK;
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

    if( NULL != pucData )
    {
        vPortFree( pucData );
    }
}


int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
    ( void ) data;

    int copylen = len;
    int size;
    int entropy;

    size = sizeof( entropy );

    while( copylen > 0 )
    {
        entropy = rand();

        if( entropy == 0 )
        {
            xil_printf( "Error RAND is returning 0 at copylen 0x%x \n\r", copylen );
        }

        memcpy( output, &entropy, size );
        output += size;
        copylen -= size;
    }

    *olen = len;

    return 0;
}


int platform_init_fs()
{
    static FATFS fatfs;
    FRESULT Res;
    TCHAR * Path = "0:/";

    /*
     * Register volume work area, initialize device
     */
    Res = f_mount( &fatfs, Path, 1 );

    if( Res != FR_OK )
    {
        xil_printf( "Failed to mount FAT FS. Formatting... 0x%x \r\n", Res );
        Res = f_mkfs( Path, 0, 0 );

        if( Res != FR_OK )
        {
            xil_printf( "Failed to format FAT FS 0x%x \r\n", Res );
            return -1;
        }

        Res = f_mount( &fatfs, Path, 0 );

        if( Res != FR_OK )
        {
            xil_printf( "Failed to mount FAT FS after format\r\n" );
            return -1;
        }
    }

    xil_printf( "File system initialization successful\r\n" );
    return 0;
}
