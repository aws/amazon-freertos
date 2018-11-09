/*
 * Amazon FreeRTOS PKCS #11 PAL for Infineon XMC4800 IoT Connectivity Kit V1.0.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright (c) 2018, Infineon Technologies AG
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 */


/**
 * @file aws_pkcs11_pal.c
 * @brief Amazon FreeRTOS device specific helper functions for
 * PKCS#11 implementation based on mbedTLS.  This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "semphr.h"

#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* flash driver includes. */
#include "e_eeprom_xmc4.h"

#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE    2048

#define pkcs11OBJECT_FLASH_OBJECT_PRESENT    ( 0xABCDEFuL )

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

/**
 * @brief Structure for certificates/key storage.
 */
typedef struct
{
    CK_CHAR cDeviceCertificate[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_CHAR cDeviceKey[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_CHAR cCodeVerificationKey[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_ULONG ulDeviceCertificateMark;
    CK_ULONG ulDeviceKeyMark;
    CK_ULONG ulCodeVerificationMark;
    CK_ULONG ulDeviceCertificateLength;
    CK_ULONG ulDeviceKeyLength;
    CK_ULONG ulCodeVerificationLength;
} P11KeyConfig_t;

/**
 * @brief Certificates/key storage in flash.
 */
static P11KeyConfig_t P11KeyConfig;

static E_EEPROM_XMC4_t e_eeprom;

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
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    CK_ULONG ulFlashMark = pkcs11OBJECT_FLASH_OBJECT_PRESENT;


    uint16_t usFlashOffset = 0;
    uint16_t usMarkOffset = 0;
    uint16_t usLengthOffset = 0;

    if( ulDataSize <= pkcs11OBJECT_CERTIFICATE_MAX_SIZE )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pxLabel->pValue,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            usFlashOffset = offsetof( P11KeyConfig_t, cDeviceCertificate );
            usMarkOffset = offsetof( P11KeyConfig_t, ulDeviceCertificateMark );
            usLengthOffset = offsetof( P11KeyConfig_t, ulDeviceCertificateLength );
            xHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            usFlashOffset = offsetof( P11KeyConfig_t, cDeviceKey );
            usMarkOffset = offsetof( P11KeyConfig_t, ulDeviceKeyMark );
            usLengthOffset = offsetof( P11KeyConfig_t, ulDeviceKeyLength );
            xHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            usFlashOffset = offsetof( P11KeyConfig_t, cDeviceKey );
            usMarkOffset = offsetof( P11KeyConfig_t, ulDeviceKeyMark );
            usLengthOffset = offsetof( P11KeyConfig_t, ulDeviceKeyLength );
            xHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            usFlashOffset = offsetof( P11KeyConfig_t, cCodeVerificationKey );
            usMarkOffset = offsetof( P11KeyConfig_t, ulCodeVerificationMark );
            usLengthOffset = offsetof( P11KeyConfig_t, ulCodeVerificationLength );
            xHandle = eAwsCodeSigningKey;
        }

        if( xHandle != eInvalidHandle )
        {
            E_EEPROM_XMC4_WriteArray( usFlashOffset, pucData, ( ulDataSize ) );
            E_EEPROM_XMC4_WriteArray( usMarkOffset, ( uint8_t * const ) &ulFlashMark, sizeof( CK_ULONG ) );
            E_EEPROM_XMC4_WriteArray( usLengthOffset, ( uint8_t * const ) &ulDataSize, sizeof( CK_ULONG ) );
            taskENTER_CRITICAL();
            E_EEPROM_XMC4_STATUS_t status = E_EEPROM_XMC4_UpdateFlashContents();
            taskEXIT_CRITICAL();

            if( status == E_EEPROM_XMC4_STATUS_OK )
            {
                E_EEPROM_XMC4_ReadArray( 0, ( uint8_t * const ) &P11KeyConfig, sizeof( P11KeyConfig_t ) );
            }
            else
            {
                xHandle = eInvalidHandle;
            }
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
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                        uint8_t usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;

    /* Translate from the PKCS#11 label to local storage file name. */
    if( 0 == memcmp( pLabel,
                     &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                     sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
    {
        if( P11KeyConfig.ulDeviceCertificateMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsDeviceCertificate;
        }
    }
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
    {
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsDevicePrivateKey;
        }
    }
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
    {
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsDevicePublicKey;
        }
    }
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                          sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
    {
        if( P11KeyConfig.ulCodeVerificationMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsCodeSigningKey;
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
    CK_RV ulReturn = CKR_OK;

    if( xHandle == eAwsDeviceCertificate )
    {
        *pIsPrivate = CK_FALSE;

        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceCertificateMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceCertificate;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulDeviceCertificateLength;
        }
    }
    else if( xHandle == eAwsDevicePrivateKey )
    {
        *pIsPrivate = CK_TRUE;

        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceKey;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulDeviceKeyLength;
        }
    }
    else if( xHandle == eAwsDevicePublicKey )
    {
        /* Public and private key are stored together in same file. */
        *pIsPrivate = CK_FALSE;

        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceKey;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulDeviceKeyLength;
        }
    }
    else if( xHandle == eAwsCodeSigningKey )
    {
        *pIsPrivate = CK_FALSE;

        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulCodeVerificationMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            *ppucData = P11KeyConfig.cCodeVerificationKey;
            *pulDataSize = ( uint32_t ) P11KeyConfig.ulCodeVerificationLength;
        }
    }
    else
    {
        ulReturn = CKR_KEY_HANDLE_INVALID;
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
    ( void ) pucData;
    ( void ) ulDataSize;

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}
/*-----------------------------------------------------------*/

extern CK_RV prvMbedTLS_Initialize( void );

/**
 * @brief Initialize the Cryptoki module for use.
 *
 * Overrides the implementation of C_Initialize in
 * aws_pkcs11_mbedtls.c when pkcs11configC_INITIALIZE_ALT
 * is defined.
 */
#ifndef pkcs11configC_INITIALIZE_ALT
    #error XMC4800 requires alternate C_Initialization
#endif

CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );

    CK_RV xResult = CKR_OK;

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    E_EEPROM_XMC4_Init( &e_eeprom, sizeof( P11KeyConfig_t ) );

    if( E_EEPROM_XMC4_IsFlashEmpty() == false )
    {
        E_EEPROM_XMC4_ReadArray( 0, ( uint8_t * const ) &P11KeyConfig, sizeof( P11KeyConfig_t ) );
    }

    if( xResult == CKR_OK )
    {
        xResult = prvMbedTLS_Initialize();
    }

    return xResult;
}
