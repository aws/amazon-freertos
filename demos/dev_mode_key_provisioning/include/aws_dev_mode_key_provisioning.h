/*
 * Amazon FreeRTOS V201906.00 Major
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "aws_pkcs11.h"

typedef struct ProvisioningParams_t
{
    uint32_t ulClientPrivateKeyType;       /**< The type of key to be provisioned, CKK_RSA or CKK_EC */
    uint8_t * pcClientPrivateKey;          /**< Pointer to the device private key in PEM format.  
                                                See tools/certificate_configuration/PEMfileToCString.html 
                                                for help with formatting.*/
    uint32_t ulClientPrivateKeyLength;     /**< Length of the private key data, in bytes. */
    uint8_t * pcClientCertificate;         /**< Pointer to the device certificate in PEM format.
                                                See tools/certificate_configuration/PEMfileToCString.html 
                                                for help with formatting.*/ 
    uint32_t ulClientCertificateLength;    /**< Length of the certificate in bytes. */
} ProvisioningParams_t;

/** \brief Provisions device with default credentials.
 * 
 * Imports the certificate and private key located in
 * aws_clientcredential_keys.h to device NVM.
 */
void vDevModeKeyProvisioning( void );

/** \brief Provisions device with provided credentials. 
 * 
 * \param[in] xParams       Provisioning parameters for credentials
 *                          to be provisioned.
 */
void vAlternateKeyProvisioning( ProvisioningParams_t * xParams );

/** \brief Initializes a PKCS #11 session.
 * 
 * This wrapper initializes the PKCS #11 module, and opens
 * a session on the default slot.
 * 
 * \param[out] ppxFunctionList  Updated to provide a pointer to the PKCS #11
 *                              module function list.
 * \param[out] pxSlotId         Updated to indicate which PKCS #11 slot will be
 *                              used by the session.
 * \param[out] pxSession        Updated to provide the opened PKCS #11 session
 *                              handle.
 * 
 * \return CKR_OK if opening the PKCS #11 session succeeded. 
 * Otherwise, a positive PKCS #11 error code.
 */     
CK_RV xInitializePkcsSession( CK_FUNCTION_LIST_PTR * ppxFunctionList,
                              CK_SLOT_ID * pxSlotId,
                              CK_SESSION_HANDLE * pxSession );

/** \brief Imports a certificate into the PKCS #11 module.
 * 
 * \param[in] xSession              A valid PKCS #11 session handle.
 * \param[in] pucCertificate        Pointer to a PEM certificate.  
 *                                  See tools/certificate_configuration/PEMfileToCString.html 
                                    for help with formatting.
 * \param[in] xCertificateLength    Length of pucCertificate, in bytes.
 * \param[in] pucLabel              PKCS #11 label attribute value for certificate to be imported.
 *                                  This should be a string value. See aws_pkcs11.h.
 * \param[out] xObjectHandle        Updated to provide the PKCS #11 handle for the certificate
 *                                  that was imported.
 *  
 * \return CKR_OK if certificate import succeeded. 
 * Otherwise, a positive PKCS #11 error code.
 */   
CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                             uint8_t * pucCertificate,
                             size_t xCertificateLength,
                             uint8_t * pucLabel,
                             CK_OBJECT_HANDLE_PTR xObjectHandle );

#endif /* _AWS_DEV_MODE_KEY_PROVISIONING_H_ */
