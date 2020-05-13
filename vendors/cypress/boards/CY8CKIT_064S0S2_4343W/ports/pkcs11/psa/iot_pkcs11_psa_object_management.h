/*
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2019-2020 Arm Limited. All Rights Reserved.
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
 */

/*
 * This file is derivative of amazon-freertos\libraries\3rdparty\mbedtls\library\
 * pkparse.c(amazon-freertos commit 74875b1d2)
 */

#ifndef __IOT_PKCS11_PSA_OBJECT_MANAGEMENT_H__
#define __IOT_PKCS11_PSA_OBJECT_MANAGEMENT_H__

/* PKCS#11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"

/* PSA includes. */
#include "psa/client.h"
#include "psa/crypto.h"
#include "psa/protected_storage.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"

#define SST_MAX_ASSET_SIZE 2048

/**
 * @brief Object definitions.
 */
#define pkcs11OBJECT_MAX_SIZE         SST_MAX_ASSET_SIZE
#define pkcs11OBJECT_PRESENT_MAGIC    ( 0xABCD0000uL )
#define pkcs11OBJECT_LENGTH_MASK      ( 0x0000FFFFuL )
#define pkcs11OBJECT_PRESENT_MASK     ( 0xFFFF0000uL )

typedef enum eObjectHandles
{
    eInvalidHandle = 0, /* From PKCS #11 spec: 0 is never a valid object handle.*/
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    eAwsJitpCertificate,
    eAwsRootCertificate
}P11ObjectHandles_t;

/**
 * @brief Structure for certificates/keys storage.
 */
typedef struct
{
    psa_storage_uid_t uxDeviceCertificate;  /* UID that indicates the device certificate data in PSA protected storage service. */
    psa_key_handle_t uxDevicePrivateKey;  /* Device private key handle that returned by PSA crypto service. */
    psa_key_handle_t uxDevicePublicKey;  /* Device public key handle that returned by PSA crypto service. */
    psa_key_handle_t uxCodeVerifyKey;  /* Key handle of the key that used by over-the-air update code to verify an incoming signed image. */
    psa_storage_uid_t uxJitpCertificate; /* UID that indicates the JITP certificate data in PSA protected storage service. */
    psa_storage_uid_t uxRootCertificate; /* UID that indicates the Root certificate data in PSA protected storage service. */
    uint32_t ulDeviceCertificateMark;
    uint32_t ulDevicePrivateKeyMark;
    uint32_t ulDevicePublicKeyMark;
    uint32_t ulCodeVerifyKeyMark;
    uint32_t ulJitpCertificateMark;
    uint32_t ulRootCertificateMark;
} P11KeyConfig_t;

/**
* @brief Writes a file to local storage.
*
* Port-specific file write for crytographic information.
*
* @param[in] pxClass       Class of the object to be saved.
* @param[in] pxLabel       Label of the object to be saved.
* @param[in] pucData       Data buffer to be written to file
* @param[in] ulDataSize    Size (in bytes) of data to be saved.
* @param[in] pvContext     Context of the key.
*
* @return The file handle of the object that was stored.
*/
CK_OBJECT_HANDLE PKCS11_PSA_SaveObject( CK_ATTRIBUTE_PTR pxClass,
    CK_ATTRIBUTE_PTR pxLabel,
    uint8_t * pucData,
    uint32_t ulDataSize,
    mbedtls_pk_context *pvContext );

/**
* @brief Gets the value of an object in storage, by handle.
*
* Port-specific file access for cryptographic information.
*
* This call dynamically allocates the buffer which object value
* data is copied into.  PKCS11_PSA_GetObjectValueCleanup()
* should be called after each use to free the dynamically allocated
* buffer.
*
* @sa PKCS11_PSA_GetObjectValueCleanup
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
CK_RV PKCS11_PSA_GetObjectValue( CK_OBJECT_HANDLE xHandle,
    uint8_t * ppucData,
    size_t * pulDataSize,
    CK_BBOOL * pIsPrivate );

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
CK_OBJECT_HANDLE PKCS11_PSA_FindObject( uint8_t * pLabel, uint8_t usLength );

/**
* @brief Cleanup after PKCS11_PSA_GetObjectValue().
*
* @param[in] pucData       The buffer to free.
*                          (*ppucData from PKCS11_PSA_GetObjectValue())
* @param[in] ulDataSize    The length of the buffer to free.
*                          (*pulDataSize from PKCS11_PSA_GetObjectValue())
*/
void PKCS11_PSA_GetObjectValueCleanup( uint8_t * pucData, uint32_t ulDataSize );

#endif /* __IOT_PKCS11_PSA_OBJECT_MANAGEMENT_H__ */
