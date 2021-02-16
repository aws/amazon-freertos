/*
 * FreeRTOS PKCS #11 PAL V1.0.0
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
 * @file core_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* FreeRTOS Includes. */
#include "core_pkcs11.h"
#include "FreeRTOS.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/**
 * @brief Initializes the PKCS #11 PAL.
 *
 * @return CKR_OK on success.
 * CKR_FUNCTION_FAILED on failure.
 */
CK_RV PKCS11_PAL_Initialize( void )
{
    CK_RV xReturn = CKR_OK;

    return xReturn;
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
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = 0;

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
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    CK_OBJECT_HANDLE xHandle = 0;

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
                                 CK_BYTE_PTR * ppucData,
                                 CK_ULONG_PTR pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    CK_RV xReturn = CKR_OK;

    return xReturn;
}


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
}

/*-----------------------------------------------------------*/

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
    /* FIX ME. */
    return 0;
}

/**
 * @brief Given a label delete the corresponding private or public key.
 */
static CK_RV prvOverwritePalObject( CK_OBJECT_HANDLE xPalHandle,
                                    CK_ATTRIBUTE_PTR pxLabel )
{
    /* See explanation in prvCheckValidSessionAndModule for this exception. */
    /* coverity[misra_c_2012_rule_10_5_violation] */
    CK_BBOOL xIsPrivate = ( CK_BBOOL ) CK_TRUE;
    CK_RV xResult = CKR_OK;
    CK_BYTE_PTR pxZeroedData = NULL;
    CK_BYTE_PTR pxObject = NULL;
    CK_ULONG ulObjectLength = sizeof( CK_BYTE ); /* MISRA: Cannot initialize to 0, as the integer passed to memset must be positive. */
    CK_OBJECT_HANDLE xPalHandle2 = CK_INVALID_HANDLE;

    xResult = PKCS11_PAL_GetObjectValue( xPalHandle, &pxObject, &ulObjectLength, &xIsPrivate );

    if( xResult == CKR_OK )
    {
        /* Some ports return a pointer to memory for which using memset directly won't work. */
        pxZeroedData = mbedtls_calloc( 1, ulObjectLength );

        if( NULL != pxZeroedData )
        {
            /* Zero out the object. */
            ( void ) memset( pxZeroedData, 0x0, ulObjectLength );
            /* Create an object label attribute. */
            /* Overwrite the object in NVM with zeros. */
            xPalHandle2 = PKCS11_PAL_SaveObject( pxLabel, pxZeroedData, ( size_t ) ulObjectLength );

            if( xPalHandle2 != xPalHandle )
            {
                LogError( ( "Failed destroying object. Received a "
                            "different handle from the PAL when writing "
                            "to the same label." ) );
                xResult = CKR_GENERAL_ERROR;
            }
        }
        else
        {
            LogError( ( "Failed destroying object. Failed to allocated "
                        "a buffer of length %lu bytes.", ulObjectLength ) );
            xResult = CKR_HOST_MEMORY;
        }

        PKCS11_PAL_GetObjectValueCleanup( pxObject, ulObjectLength );
        mbedtls_free( pxZeroedData );
    }

    return xResult;
}

CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
    CK_BYTE_PTR pcLabel = NULL;
    CK_ULONG xLabelLength = 0;
    CK_RV xResult = CKR_OK;
    CK_ATTRIBUTE xLabel = { 0 };
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE xAppHandle2 = CK_INVALID_HANDLE;
    CK_BYTE pxPubKeyLabel[] = { pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS };
    CK_BYTE pxPrivKeyLabel[] = { pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS };

    prvFindObjectInListByHandle( xHandle, &xPalHandle, &pcLabel, &xLabelLength );

    if( pcLabel != NULL )
    {
        xLabel.type = CKA_LABEL;
        xLabel.pValue = pcLabel;
        xLabel.ulValueLen = xLabelLength;
        xResult = prvOverwritePalObject( xPalHandle, &xLabel );
    }
    else
    {
        LogError( ( "Failed destroying object. Could not found the object label." ) );
        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
    }

    if( xResult == CKR_OK )
    {
        if( 0 == strncmp( xLabel.pValue, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, xLabel.ulValueLen ) )
        {
            /* Remove NULL terminator in comparison. */
            prvFindObjectInListByLabel( pxPubKeyLabel, sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ), &xPalHandle, &xAppHandle2 );
        }
        else if( 0 == strncmp( xLabel.pValue, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, xLabel.ulValueLen ) )
        {
            /* Remove NULL terminator in comparison. */
            prvFindObjectInListByLabel( pxPrivKeyLabel, sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ), &xPalHandle, &xAppHandle2 );
        }
        else
        {
            LogWarn( ( "Trying to destroy an object with an unknown label." ) );
        }

        if( ( xPalHandle != CK_INVALID_HANDLE ) && ( xAppHandle2 != CK_INVALID_HANDLE ) )
        {
            xResult = prvDeleteObjectFromList( xAppHandle2 );
        }

        if( xResult != CKR_OK )
        {
            LogWarn( ( "Failed to remove xAppHandle2 from object list when destroying object memory." ) );
        }

        xResult = prvDeleteObjectFromList( xHandle );
    }

    return xResult;
}
