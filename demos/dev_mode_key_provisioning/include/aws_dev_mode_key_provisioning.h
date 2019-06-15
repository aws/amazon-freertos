/*
 * Amazon FreeRTOS Provisioning V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


#ifndef _AWS_DEV_MODE_KEY_PROVISIONING_H_
#define _AWS_DEV_MODE_KEY_PROVISIONING_H_
#include "aws_pkcs11.h"

typedef struct ProvisioningParams_t
{
    uint8_t * pucClientPrivateKey;
    uint32_t ulClientPrivateKeyLength;
    uint8_t * pucClientCertificate;
    uint32_t ulClientCertificateLength;
    uint8_t * pucJITPCertificate;
    uint32_t ulJITPCertifiateLength;
} ProvisioningParams_t;

void vDevModeKeyProvisioning( void );

void vAlternateKeyProvisioning( ProvisioningParams_t * xParams );

CK_RV xProvisionPrivateKey( CK_SESSION_HANDLE xSession,
                            uint8_t * pucPrivateKey,
                            size_t xPrivateKeyLength,
                            uint8_t * pucLabel,
                            CK_OBJECT_HANDLE_PTR pxObjectHandle );

CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                             uint8_t * pucCertificate,
                             size_t xCertificateLength,
                             uint8_t * pucLabel,
                             CK_OBJECT_HANDLE_PTR pxObjectHandle );

CK_RV xProvisionGenerateKeyPairRSA( CK_SESSION_HANDLE xSession,
                                    uint8_t * pucPrivateKeyLabel,
                                    uint8_t * pucPublicKeyLabel,
                                    CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                    CK_OBJECT_HANDLE_PTR pxPublicKeyHandle );

CK_RV xProvisionGenerateKeyPairEC( CK_SESSION_HANDLE xSession,
                                   uint8_t * pucPrivateKeyLabel,
                                   uint8_t * pucPublicKeyLabel,
                                   CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                   CK_OBJECT_HANDLE_PTR pxPublicKeyHandle );

CK_RV xDestroyCredentials( CK_SESSION_HANDLE xSession );

#endif /* _AWS_DEV_MODE_KEY_PROVISIONING_H_ */
