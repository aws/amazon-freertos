/*
 * FreeRTOS PKCS #11 PAL for Infineon XMC4800 IoT Connectivity Kit V1.0.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file core_pkcs11_pal.c
 * @brief FreeRTOS device specific helper functions for
 * PKCS#11 implementation based on mbedTLS.  This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "semphr.h"

#include "iot_crypto.h"
#include "core_pkcs11.h"
#include "core_pkcs11_config.h"

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
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
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
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;

    /* Translate from the PKCS#11 label to local storage file name. */
    if( 0 == memcmp( pxLabel,
                     &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                     sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
    {
        if( P11KeyConfig.ulDeviceCertificateMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsDeviceCertificate;
        }
    }
    else if( 0 == memcmp( pxLabel,
                          &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
    {
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsDevicePrivateKey;
        }
    }
    else if( 0 == memcmp( pxLabel,
                          &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
    {
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_OBJECT_PRESENT )
        {
            xHandle = eAwsDevicePublicKey;
        }
    }
    else if( 0 == memcmp( pxLabel,
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
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
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
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    /* Unused parameters. */
    ( void ) pucData;
    ( void ) ulDataSize;

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}
/*-----------------------------------------------------------*/

CK_RV PKCS11_PAL_Initialize( void )
{   
    CK_RV xResult = CKR_OK;
    CRYPTO_Init();

    E_EEPROM_XMC4_Init( &e_eeprom, sizeof( P11KeyConfig_t ) );

    if( E_EEPROM_XMC4_IsFlashEmpty() == false )
    {
        E_EEPROM_XMC4_ReadArray( 0, ( uint8_t * const ) &P11KeyConfig, sizeof( P11KeyConfig_t ) );
    }

    return xResult;
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
    CK_ULONG ulObjectLength = sizeof( CK_BYTE );     /* MISRA: Cannot initialize to 0, as the integer passed to memset must be positive. */
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
