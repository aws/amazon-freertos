/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 * 
 * Amazon FreeRTOS PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/**
 * @file aws_pkcs11_pal.c
 * @brief MT7697H specific helpers for PKCS11 Interface.
 */

/*-----------------------------------------------------------*/

/* Amazon FreeRTOS Includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* flash driver includes. */
#include "hal_file.h"

#include "aws_pkcs11_config.h"

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
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR    pxLabel,
                                        uint8_t             *pucData,
                                        uint32_t            ulDataSize )
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
    /* Avoid compiler warnings about unused variables. */
    ( void ) usLength;

    char                *pcFileName = NULL;
    CK_OBJECT_HANDLE    xHandle     = eInvalidHandle;

    /* Converts a label to its respective filename and handle. */

    prvLabelToFilenameHandle( pLabel, &pcFileName, &xHandle );

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
                                 uint8_t ** ppucData,
                                 uint32_t * pulDataSize,
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
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
                                       uint32_t ulDataSize )
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

