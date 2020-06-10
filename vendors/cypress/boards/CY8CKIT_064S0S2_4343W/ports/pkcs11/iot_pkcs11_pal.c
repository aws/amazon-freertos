/*
 * Amazon FreeRTOS PKCS #11 PAL for Cypress development kit V1.0.1
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
#include "FreeRTOS.h"
#include "cyobjstore.h"
#include "iot_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//
// Data layout, very simple packed storage of information in
// EEPROM emulated flash memory
//
// Offset       Size        Purpose
// 0            4           Magic number
// 4            1           Key Value
// 5            4           Data Length
// 9            N           Data
// 9 + N        1           Key Value
//

enum eObjectHandles 
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};


/* Converts a label to its respective filename and handle. */
static void prvLabelToFilenameHandle( uint8_t * pcLabel, CK_OBJECT_HANDLE_PTR pHandle )
{
    if( pcLabel != NULL )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pcLabel,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            *pHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            *pHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *pHandle = eAwsCodeSigningKey;
        }
        else
        {
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
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    CK_OBJECT_HANDLE xHandle ;

    if (cy_objstore_initialize(false, 1) != CY_RSLT_SUCCESS)
        return eInvalidHandle ;

    prvLabelToFilenameHandle( pxLabel->pValue, &xHandle );

    if (cy_objstore_store_object(xHandle, pucData, ulDataSize) != CY_RSLT_SUCCESS)
        return eInvalidHandle ;

    return  xHandle ;
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
    CK_OBJECT_HANDLE xHandle ;
    
    if (cy_objstore_initialize(false, 1) != CY_RSLT_SUCCESS)
        return eInvalidHandle ;

    prvLabelToFilenameHandle( pxLabel, &xHandle );

    if (cy_objstore_find_object(xHandle, NULL, NULL) != CY_RSLT_SUCCESS)
        return eInvalidHandle ;

    return xHandle ;
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
    uint32_t index, size ;
    cy_rslt_t res ;

    if (cy_objstore_initialize(false, 1) != CY_RSLT_SUCCESS)
        return CKR_FUNCTION_FAILED ;

    if( xHandle == eAwsDeviceCertificate )
    {
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsDevicePrivateKey )
    {
        *pIsPrivate = CK_TRUE;
    }
    else if( xHandle == eAwsDevicePublicKey )
    {
        /* Public and private key are stored together in same file. */
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsCodeSigningKey )
    {
        *pIsPrivate = CK_FALSE;
    }
    else
    {
        return CKR_KEY_HANDLE_INVALID;
    }    

    if (cy_objstore_find_object(xHandle, &index, &size) != CY_RSLT_SUCCESS)
    {
        configPRINT_STRING("cyafr: pkcs11: handle that previously existed is gone, internal error") ;
        return CKR_FUNCTION_FAILED ;
    }       

    *pulDataSize = size ;

    *ppucData = (uint8_t *)malloc(size) ;
    if (*ppucData == NULL)
        return CKR_DEVICE_MEMORY ;     

    res = cy_objstore_read_object(xHandle, *ppucData, size) ;
    if (res != CY_RSLT_SUCCESS)
    {
        free(*ppucData) ;
        configPRINT_STRING("cyafr: pkcs11: object store read operation failed") ;
        return CKR_FUNCTION_FAILED ;
    }

    return CKR_OK ;
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
    free(pucData) ;
}

/*-----------------------------------------------------------*/
