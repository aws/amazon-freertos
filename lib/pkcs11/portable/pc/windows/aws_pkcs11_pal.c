/*
 * Amazon FreeRTOS PKCS#11 PAL for Windows Simulator V1.0.2
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
 * @brief Windows Simulator file save and read implementation
 * for PKCS#11 based on mbedTLS with for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/*-----------------------------------------------------------*/


#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "aws_pkcs11.h"


/* C runtime includes. */
#include <stdio.h>
#include <string.h>


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
    uint32_t ulStatus = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD lpNumberOfBytesWritten;

    /* Open the file. */
    hFile = CreateFileA( pcFileName,
                         GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        ulStatus = GetLastError();
    }

    /* Write the data. */
    if( ERROR_SUCCESS == ulStatus )
    {
        if( FALSE == WriteFile( hFile, pucData, ulDataSize, &lpNumberOfBytesWritten, NULL ) )
        {
            ulStatus = GetLastError();
        }
    }

    /* Clean up. */
    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
    }

    return 0 == ulStatus;
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
    uint32_t ulStatus = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    uint32_t ulSize = 0;

    /* Open the file. */
    hFile = CreateFileA( pcFileName,
                         GENERIC_READ,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        ulStatus = GetLastError();
    }

    if( 0 == ulStatus )
    {
        /* Get the file size. */
        *pulDataSize = GetFileSize( hFile, ( LPDWORD ) ( &ulSize ) );

        /* Create a buffer. */
        *ppucData = pvPortMalloc( *pulDataSize );

        if( NULL == *ppucData )
        {
            ulStatus = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    /* Read the file. */
    if( 0 == ulStatus )
    {
        if( FALSE == ReadFile( hFile,
                               *ppucData,
                               *pulDataSize,
                               ( LPDWORD ) ( &ulSize ),
                               NULL ) )
        {
            ulStatus = GetLastError();
        }
    }

    /* Confirm the amount of data read. */
    if( 0 == ulStatus )
    {
        if( ulSize != *pulDataSize )
        {
            ulStatus = ERROR_INVALID_DATA;
        }
    }

    /* Clean up. */
    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
    }

    return 0 == ulStatus;
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
    ( void ) ulBufferSize;

    vPortFree( pucBuffer );
}
