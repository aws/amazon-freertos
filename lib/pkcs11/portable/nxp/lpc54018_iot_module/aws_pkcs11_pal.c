/*
 * Amazon FreeRTOS PKCS#11 for LPC54018 IoT Module V1.0.1
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
 * @brief NXP LPC54018 IoT module file save and read implementation  
 * for PKCS#11  based on mbedTLS with for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "aws_crypto.h"
#include "task.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* Flash write */
#include "mflash_file.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* Flash structure */
mflash_file_t g_cert_files[] =
{
    { .path = pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
      .flash_addr = MFLASH_FILE_BASEADDR,
      .max_size = MFLASH_FILE_SIZE },
    { .path = pkcs11configFILE_NAME_KEY,
      .flash_addr = MFLASH_FILE_BASEADDR + MFLASH_FILE_SIZE,
      .max_size = MFLASH_FILE_SIZE },
    { 0 }
};


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
    return mflash_save_file( pcFileName, pucData, ulDataSize );
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
    return mflash_read_file( pcFileName, ppucData, pulDataSize );
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


/**
 *      PKCS#11 Override
 *
 */

/**
 * @brief Initialize the Cryptoki module for use.
 *
 * Overrides the implementation of C_Initialize in 
 * aws_pkcs11_mbedtls.c when pkcs11configC_INITIALIZE_ALT
 * is defined.
 */
#ifndef pkcs11configC_INITIALIZE_ALT
#error LPC54018 requires alternate C_Initialization
#endif 

CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );
    uint32_t status = CKR_GENERAL_ERROR;

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    /* Initialize flash storage. */
    if( pdTRUE == mflash_init( g_cert_files, 1 ) )
    {
        status = CKR_OK;
    }

    return status;
}
