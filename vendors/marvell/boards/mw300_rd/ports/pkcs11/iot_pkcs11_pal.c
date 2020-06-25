/*
 * FreeRTOS PKCS #11 PAL
 *
 * (C) Copyright 2018-2019 Marvell International Ltd. All Rights Reserved.
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell
 * International Ltd or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Marvell or its
 * suppliers and licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Marvell's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 */

/* For best security practice, it is recommended to utilize a random number
 * generation solution that is truly randomized and conforms to the guidelines
 * provided in the Device Qualification Program for FreeRTOS Guide
 * (https://docs.aws.amazon.com/freertos/latest/qualificationguide/afq-checklist.html).
 * The random number generator method presented in this file by the silicon vendor
 * is not truly random in nature.
 * Please contact the silicon vendor for details regarding the method implemented.
 */

/**
 * @file iot_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* FreeRTOS Includes. */
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"
#include "FreeRTOS.h"

/* Marvell specific Includes. */
#include <wm_os.h>
#include <psm-v2.h>
#include <psm-utils.h>
#include <wm_utils.h>

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "P11_Cert"
#define pkcs11palFILE_NAME_KEY                   "P11_Key"
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "P11_CSK"

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
    if (pcLabel != NULL) {
        /* Translate from the PKCS#11 label to local storage file name. */
        if (0 == memcmp(pcLabel,
                        &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                        sizeof(pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS))) {
            *pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pHandle = eAwsDeviceCertificate;
        } else if (0 == memcmp(pcLabel,
                               &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                               sizeof(pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS))) {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePrivateKey;
        } else if (0 == memcmp(pcLabel,
                               &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                               sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS))) {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePublicKey;
        } else if (0 == memcmp(pcLabel,
                               &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                               sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY))) {
            *pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
            *pHandle = eAwsCodeSigningKey;
        } else {
            *pcFileName = NULL;
            *pHandle = eInvalidHandle;
        }
    }
}

CK_RV PKCS11_PAL_Initialize( void )
{
    return CKR_OK;
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
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle(pxLabel->pValue, &pcFileName, &xHandle);

    if (pcFileName == NULL) {
        /* TODO: Check what print function to use */
        wmprintf("Error: failed to translate label to filename\r\n");
        return eInvalidHandle;
    }

    /* We always call sys_psm_init(). Internally it keeps
     * track if it is already initialized or not.
     */
    if (sys_psm_init() != WM_SUCCESS) {
        /* TODO: Check what print function to use */
        wmprintf("Error: sys psm init failed\r\n");
        return eInvalidHandle;
    }

    if (psm_set_variable(sys_psm_get_handle(), pcFileName, pucData, ulDataSize)
             != WM_SUCCESS) {
        /* TODO: Check what print function to use */
        wmprintf("Error: write to sys psm failed\r\n");
        return eInvalidHandle;
    }

    return xHandle;
}

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pcLabel        Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;
    int ret;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle(pxLabel, &pcFileName, &xHandle);

    if (pcFileName == NULL) {
        /* TODO: Check what print function to use */
        wmprintf("Error: failed to translate label to filename\r\n");
        return eInvalidHandle;
    }

    /* We always call sys_psm_init(). Internally it keeps
     * track if it is already initialized or not.
     */
    if (sys_psm_init() != WM_SUCCESS) {
        /* TODO: Check what print function to use */
        wmprintf("Error: sys psm init failed\r\n");
        return eInvalidHandle;
    }

    ret = psm_get_variable_size(sys_psm_get_handle(), pcFileName);
    if (ret < 0) {
        /* TODO: Check what print function to use */
        wmprintf("Error: couldn't find the object\r\n");
        return eInvalidHandle;
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
* @return CKR_OK if operation was successful.  CKR_OBJECT_HANDLE_INVALID if
* no such object handle was found, CKR_DEVICE_MEMORY if memory for
* buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
* error, CKR_ARGUMENTS_BAD for bad arguments.
*/
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
                                      CK_BBOOL * pIsPrivate )
{
    char * pcFileName = NULL;
    int ret;

    if (ppucData == NULL || pulDataSize == NULL) {
        /* TODO: Check what print function to use */
        wmprintf("Error: Invalid arguments\r\n");
        return CKR_ARGUMENTS_BAD;
    }

    switch (xHandle) {
        case eAwsDeviceCertificate:
            pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pIsPrivate = CK_FALSE;
            break;
        case eAwsDevicePrivateKey:
            pcFileName = pkcs11palFILE_NAME_KEY;
            *pIsPrivate = CK_TRUE;
            break;
        case eAwsDevicePublicKey:
            /* Public and private key are stored together in same file. */
            pcFileName = pkcs11palFILE_NAME_KEY;
            *pIsPrivate = CK_FALSE;
            break;
        case eAwsCodeSigningKey:
            pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
            *pIsPrivate = CK_FALSE;
            break;
        default:
            /* TODO: Check what print function to use */
            wmprintf("Error: Invalid handle\r\n");
            return CKR_OBJECT_HANDLE_INVALID;
    }

    /* We always call sys_psm_init(). Internally it keeps
     * track if it is already initialized or not.
     */
    if (sys_psm_init() != WM_SUCCESS) {
        /* TODO: Check what print function to use */
        wmprintf("Error: sys psm init failed\r\n");
        return CKR_FUNCTION_FAILED;
    }

    ret = psm_get_variable_size(sys_psm_get_handle(), pcFileName);
    if (ret < 0) {
        *pulDataSize = 0;
        /* TODO: Check what print function to use */
        wmprintf("Error: get variable size from sys psm failed\r\n");
        return CKR_OBJECT_HANDLE_INVALID;
    }

    *pulDataSize = ret;
    *ppucData = os_mem_alloc(*pulDataSize);
    if (*ppucData == NULL) {
        *pulDataSize = 0;
        /* TODO: Check what print function to use */
        wmprintf("Error: mem alloc for file failed\r\n");
        return CKR_DEVICE_MEMORY;
    }

    ret = psm_get_variable(sys_psm_get_handle(), pcFileName, *ppucData, *pulDataSize);
    if (ret < 0)
    {
        os_mem_free(*ppucData);
        *ppucData = NULL;
        *pulDataSize = 0;
        /* TODO: Check what print function to use */
        wmprintf("Error: get variable from sys psm failed\r\n");
        return CKR_FUNCTION_FAILED;
    }

    if (ret != *pulDataSize) {
        os_mem_free(*ppucData);
        *ppucData = NULL;
        *pulDataSize = 0;
        /* TODO: Check what print function to use */
        wmprintf("Error: read from sys psm failed\r\n");
        return CKR_FUNCTION_FAILED;
    }

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
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    if (pucData == NULL)
        return;

    os_mem_free(pucData);
}

/*-----------------------------------------------------------*/


int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
    static bool seed_handler_registered;
    ((void) data);

    if (seed_handler_registered != 1) {
	if (random_register_seed_handler(sample_initialise_random_seed) != WM_SUCCESS) {
		configPRINT("Failed to register seed_handler\r\n");
	} else {
		configPRINT("Registered seed_handler\r\n");
		seed_handler_registered = 1;
	}
    }

    get_random_sequence(output, len);

    *olen = len;
    return 0;
}
