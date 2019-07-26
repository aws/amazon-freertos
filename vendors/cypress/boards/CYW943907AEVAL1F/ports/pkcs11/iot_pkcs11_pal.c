/*
 * Amazon FreeRTOS PKCS #11 PAL for Cypress CYW943907AEVAL1F development kit V1.0.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/**
 * @file iot_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"
#include "FreeRTOS.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#include "wiced_result.h"
#include "wiced.h"

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
    wiced_result_t result = WICED_SUCCESS;
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pxLabel->pValue, &pcFileName, &xHandle );
    if( xHandle != eInvalidHandle )
    {
        if( strcmp( pcFileName, pkcs11palFILE_NAME_CLIENT_CERTIFICATE ) == 0 )
        {
            result = wiced_dct_write( (const void*) &ulDataSize, DCT_APP_SECTION, 0, 4 );
            if ( result != WICED_SUCCESS)
            {
                printf("wiced_dct_write failed for client certificate length %d \n", result );
            }
            wiced_dct_write( (const void*) pucData, DCT_APP_SECTION, 12, ulDataSize );
            if ( result != WICED_SUCCESS)
            {
                printf("wiced_dct_write failed for client certificate %d \n", result );
            }

        }
        else if( strcmp( pcFileName, pkcs11palFILE_NAME_KEY ) == 0 )
        {
            result = wiced_dct_write( (const void*) &ulDataSize, DCT_APP_SECTION, 4, 4 );
            if ( result != WICED_SUCCESS)
            {
                printf("wiced_dct_write failed for client key length %d \n", result );
            }
            result = wiced_dct_write( (const void*) pucData, DCT_APP_SECTION, 12 + 2048, ulDataSize );
            if ( result != WICED_SUCCESS)
            {
                printf("wiced_dct_write failed for client key %d \n", result );
            }
        }
        else if( strcmp( pcFileName, pkcs11palFILE_CODE_SIGN_PUBLIC_KEY ) == 0 )
        {
            result = wiced_dct_write( (const void*) &ulDataSize, DCT_APP_SECTION, 8, 4 );
            if ( result != WICED_SUCCESS)
            {
                printf("wiced_dct_write failed for client key length %d \n", result );
            }
            result = wiced_dct_write( (const void*) pucData, DCT_APP_SECTION, 12 + 4096, ulDataSize );
            if ( result != WICED_SUCCESS)
            {
                printf("wiced_dct_write failed for client key %d \n", result );
            }
        }
        if( result != WICED_SUCCESS )
        {
            xHandle = eInvalidHandle;
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
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pLabel, &pcFileName, &xHandle );

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
    uint32_t len=0;
    wiced_result_t result = WICED_SUCCESS;
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
        return CKR_KEY_HANDLE_INVALID;
    }

    if( strcmp( pcFileName, pkcs11palFILE_NAME_CLIENT_CERTIFICATE ) == 0 )
    {
        result = wiced_dct_read_with_copy( (void*) &len, DCT_APP_SECTION, 0, 4 );
        if ( result != WICED_SUCCESS)
        {
            printf("wiced_dct_read_with_copy failed for client certificate length %d \n", result );
        }
        result = wiced_dct_read_lock( (void**) ppucData, WICED_TRUE, DCT_APP_SECTION, 12, len );
        if ( result != WICED_SUCCESS)
        {
            printf("wiced_dct_read_lock failed for client certificate %d \n", result );
        }
    }
    else if( strcmp( pcFileName, pkcs11palFILE_NAME_KEY ) == 0 )
    {
        result = wiced_dct_read_with_copy( (void*) &len, DCT_APP_SECTION, 4, 4 );
        if ( result != WICED_SUCCESS)
        {
            printf("wiced_dct_read_with_copy failed for client key length %d \n", result );
        }
        result = wiced_dct_read_lock( (void**) ppucData, WICED_TRUE, DCT_APP_SECTION, 12 + 2048, len );
        if ( result != WICED_SUCCESS)
        {
            printf("wiced_dct_read_lock failed for client certificate %d \n", result );
        }
    }
    else if( strcmp( pcFileName, pkcs11palFILE_CODE_SIGN_PUBLIC_KEY ) == 0 )
    {
        result = wiced_dct_read_with_copy( (void*) &len, DCT_APP_SECTION, 8, 4 );
        if ( result != WICED_SUCCESS)
        {
            printf("wiced_dct_read_with_copy failed for client key length %d \n", result );
        }
        result = wiced_dct_read_lock( (void**) ppucData, WICED_TRUE, DCT_APP_SECTION, 12 + 4096, len );
        if ( result != WICED_SUCCESS)
        {
            printf("wiced_dct_read_lock failed for client certificate %d \n", result );
        }
    }
    *pulDataSize = len;

    if( ( result != WICED_SUCCESS ) || ( len == 0 ) )
    {
        ulReturn = CKR_FUNCTION_FAILED;
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
    if( NULL != pucData )
    {
        wiced_dct_read_unlock( pucData, WICED_TRUE );
    }
}

/*-----------------------------------------------------------*/