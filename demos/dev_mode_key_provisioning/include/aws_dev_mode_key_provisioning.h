/*
 * FreeRTOS V202203.00
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
 * @file  aws_dev_mode_key_provsioning.h
 * @brief Provisioning example code for developers.
 *
 * Helpers for importing device private key and device
 * certificate for use with AWS connectivity libraries.
 *
 * \warn This code is provided for example purposes only, and
 * should not be used in production code.
 */

#ifndef _AWS_DEV_MODE_KEY_PROVISIONING_H_
#define _AWS_DEV_MODE_KEY_PROVISIONING_H_

#include "core_pkcs11_config.h"
#include "core_pkcs11.h"

typedef struct ProvisioningParams_t
{
    uint8_t * pucClientPrivateKey;      /**< Pointer to the device private key in PEM format.
                                         *   See tools/certificate_configuration/PEMfileToCString.html
                                         *   for help with formatting.*/
    uint32_t ulClientPrivateKeyLength;  /**< Length of the private key data, in bytes. */
    uint8_t * pucClientCertificate;     /**< Pointer to the device certificate in PEM format.
                                         *   See tools/certificate_configuration/PEMfileToCString.html
                                         *   for help with formatting.*/
    uint32_t ulClientCertificateLength; /**< Length of the device certificate in bytes. */
    uint8_t * pucJITPCertificate;       /**< Pointer to the Just-In-Time Provisioning (JITP) certificate in
                                         *   PEM format.
                                         *   - This is REQUIRED if JITP is being used.
                                         *   - If you are not using JITP, this certificate
                                         *   is not needed and should be set to NULL.
                                         *   - See tools/certificate_configuration/PEMfileToCString.html
                                         *   for help with formatting.
                                         *   - See https://aws.amazon.com/blogs/iot/setting-up-just-in-time-provisioning-with-aws-iot-core/
                                         *   for more information about getting started with JITP */
    uint32_t ulJITPCertificateLength;   /**< Length of the Just-In-Time Provisioning (JITP) certificate in bytes.
                                         *   If JITP is not being used, this value should be set to 0. */
} ProvisioningParams_t;

/** \brief Provisions device with default credentials.
 *
 * Imports the certificate and private key located in
 * aws_clientcredential_keys.h to device NVM.
 *
 * \return CKR_OK upon successful credential setup.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV vDevModeKeyProvisioning( void );

/** \brief Provisiong a device given a valid PKCS #11 session.
 *
 * \param[in] xSession       A valid PKCS #11 session.
 * \param[in] pxParams       Pointer to an initialized provisioning
 *                           structure.
 *
 * \return CKR_OK upon successful credential setup.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV xProvisionDevice( CK_SESSION_HANDLE xSession,
                        ProvisioningParams_t * pxParams );

/** \brief Provisions device with provided credentials.
 *
 * \param[in] xParams       Provisioning parameters for credentials
 *                          to be provisioned.
 *
 * \return CKR_OK upon successful credential setup.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV vAlternateKeyProvisioning( ProvisioningParams_t * xParams );

/** \brief Provisions a private key using PKCS #11 library.
 *
 * \param[in] xSession             An initialized session handle.
 * \param[in] pucPrivateKey        Pointer to private key.  Key may either be PEM formatted
 *                                 or ASN.1 DER encoded.
 * \param[in] xPrivateKeyLength    Length of the data at pucPrivateKey, in bytes.
 * \param[in] pucLabel             PKCS #11 CKA_LABEL attribute value to be used for key.
 *                                 This should be a string values. See core_pkcs11_config.h
 * \param[out] pxObjectHandle      Points to the location that receives the PKCS #11
 *                                 private key handle created.
 *
 * \return CKR_OK upon successful key creation.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV xProvisionPrivateKey( CK_SESSION_HANDLE xSession,
                            uint8_t * pucPrivateKey,
                            size_t xPrivateKeyLength,
                            uint8_t * pucLabel,
                            CK_OBJECT_HANDLE_PTR pxObjectHandle );

/** \brief Imports a public key into the PKCS #11 module.
 *
 * \param[in] xSession               A valid PKCS #11 session handle.
 * \param[in] pucKey                 Pointer to public key.  Key may either be PEM formatted
 *                                   or ASN.1 DER encoded.
 * \param[in] xKeyLength             Length of the data at pucPrivateKey, in bytes.
 * \param[in] xPublicKeyType         The type of key- either CKK_RSA or CKK_EC.
 * \param[in] pucPublicKeyLabel      PKCS #11 CKA_LABEL attribute value to be used for key.
 *                                   This should be a string values.  See core_pkcs11_config.h.
 * \param[out] pxPublicKeyHandle     Points to the location that receives the PKCS #11 public
 *                                   key handle created.
 *
 * \return CKR_OK upon successful key creation.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV xProvisionPublicKey( CK_SESSION_HANDLE xSession,
                           uint8_t * pucKey,
                           size_t xKeyLength,
                           CK_KEY_TYPE xPublicKeyType,
                           uint8_t * pucPublicKeyLabel,
                           CK_OBJECT_HANDLE_PTR pxPublicKeyHandle );


/** \brief Imports a certificate into the PKCS #11 module.
 *
 * \param[in] xSession              A valid PKCS #11 session handle.
 * \param[in] pucCertificate        Pointer to a PEM certificate.
 *                                  See tools/certificate_configuration/PEMfileToCString.html
 *                                  for help with formatting.
 * \param[in] xCertificateLength    Length of pucCertificate, in bytes.
 * \param[in] pucLabel              PKCS #11 label attribute value for certificate to be imported.
 *                                  This should be a string value. See core_pkcs11.h.
 *                                  This should be a string value. See core_pkcs11_config.h.
 * \param[out] pxObjectHandle       Points to the location that receives the PKCS #11
 *                                  certificate handle created.
 *
 * \return CKR_OK if certificate import succeeded.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                             uint8_t * pucCertificate,
                             size_t xCertificateLength,
                             uint8_t * pucLabel,
                             CK_OBJECT_HANDLE_PTR pxObjectHandle );

/** \brief Generates an RSA key pair.
 *
 * \param[in] xSession              A valid PKCS #11 session handle.
 * \param[in] pucPrivateKeyLabel    PKCS #11 label attribute value for private key to be created.
 *                                  This should be a string value. See core_pkcs11_config.h.
 * \param[in] pucPublicKeyLabel     PKCS #11 label attribute value for public key to be created.
 *                                  This should be a string value. See core_pkcs11_config.h.
 * \param[out] pxPrivateKeyHandle   Points to the location that receives the PKCS #11 private
 *                                  key handle created.
 * \param[out] pxPublicKeyHandle    Points to the location that receives the PKCS #11 public
 *                                  key handle created.
 *
 * \return CKR_OK if RSA key pair generation succeeded.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV xProvisionGenerateKeyPairRSA( CK_SESSION_HANDLE xSession,
                                    uint8_t * pucPrivateKeyLabel,
                                    uint8_t * pucPublicKeyLabel,
                                    CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                    CK_OBJECT_HANDLE_PTR pxPublicKeyHandle );

/** \brief Generates an elliptic curve key pair.
 *
 * \param[in] xSession              A valid PKCS #11 session handle.
 * \param[in] pucPrivateKeyLabel    PKCS #11 label attribute value for private key to be created.
 *                                  This should be a string value. See core_pkcs11_config.h.
 * \param[in] pucPublicKeyLabel     PKCS #11 label attribute value for public key to be created.
 *                                  This should be a string value. See core_pkcs11_config.h.
 * \param[out] pxPrivateKeyHandle   Points to the location that receives the PKCS #11 private
 *                                  key handle created.
 * \param[out] pxPublicKeyHandle    Points to the location that receives the PKCS #11 public
 *                                  key handle created.
 *
 * \return CKR_OK if EC key pair generation succeeded.
 * Otherwise, a positive PKCS #11 error code.
 */
CK_RV xProvisionGenerateKeyPairEC( CK_SESSION_HANDLE xSession,
                                   uint8_t * pucPrivateKeyLabel,
                                   uint8_t * pucPublicKeyLabel,
                                   CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                   CK_OBJECT_HANDLE_PTR pxPublicKeyHandle );

/**
 *\brief Destroys FreeRTOS credentials stored in device PKCS #11 module.
 *
 * \note Not all ports support the deletion of all objects.  Successful
 * function return only indicates that all objects for which destroy is
 * supported on the port were erased from non-volatile memory.
 *
 * Destroys objects with the following labels, if applicable:
 *     pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
 *     pkcs11configLABEL_CODE_VERIFICATION_KEY,
 *     pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
 *     pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS
 *
 *   \param[in] xSession         A valid PKCS #11 session handle.
 *
 *   \return CKR_OK if all credentials were destroyed.
 *   Otherwise, a positive PKCS #11 error code.
 */
CK_RV xDestroyDefaultCryptoObjects( CK_SESSION_HANDLE xSession );

/**
 * \brief Destroys specified credentials in PKCS #11 module.
 *
 * \note Some ports only support lookup of objects by label (and
 * not label + class).  For these ports, only the label field is used
 * for determining what objects to destroy.
 *
 * \note Not all ports support the deletion of all objects.  Successful
 * function return only indicates that all objects for which destroy is
 * supported on the port were erased from non-volatile memory.
 *
 *   \param[in] xSession         A valid PKCS #11 session handle.
 *   \param[in] ppxPkcsLabels    An array of pointers to object labels.
 *                               Labels are assumed to be NULL terminated
 *                               strings.
 *   \param[in] pxClass          An array of object classes, corresponding
 *                               to the array of ppxPkcsLabels.  For example
 *                               the first label pointer and first class in
 *                               ppxPkcsLabels are used in combination for
 *                               lookup of the object to be deleted.
 *   \param[in] ulCount          The number of label-class pairs passed in
 *                               to be destroyed.
 *
 *   \return CKR_OK if all credentials were destroyed.
 *   Otherwise, a positive PKCS #11 error code.
 */
CK_RV xDestroyProvidedObjects( CK_SESSION_HANDLE xSession,
                               CK_BYTE_PTR * ppxPkcsLabels,
                               CK_OBJECT_CLASS * pxClass,
                               CK_ULONG ulCount );

#endif /* _AWS_DEV_MODE_KEY_PROVISIONING_H_ */
