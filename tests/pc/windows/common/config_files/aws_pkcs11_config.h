/*
 * Amazon FreeRTOS V1.1.4
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
 * @file aws_pkcs11_config.h
 * @brief PCKS#11 config options.
 */


#ifndef _AWS_PKCS11_CONFIG_H_
#define _AWS_PKCS11_CONFIG_H_

/* A non-standard version of C_INITIALIZE should be used by this port. */
/* #define pkcs11configC_INITIALIZE_ALT */

/**
 * PKCS #11 default user PIN.
 *
 * The PKCS #11 standard specifies the presence of a user PIN. That feature is
 * sensible for applications that have an interactive user interface and memory
 * protections. However, since typical microcontroller applications lack one or
 * both of those, the user PIN is assumed to be used herein for interoperability
 * purposes only, and not as a security feature.
 */
#define configPKCS11_DEFAULT_USER_PIN    "0000"

/**
 * @brief The PKCS #11 label for device private key.
 *
 * Private key for connection to AWS IoT endpoint.  The corresponding
 * public key should be registered with the AWS IoT endpoint.
 */
#define pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS    "Device Priv TLS Key"

/**
 * @brief The PKCS #11 label for device public key.
 *
 * The public key corresponding to pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS.
 */
#define pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS     "Device Pub TLS Key"

/**
 * @brief The PKCS #11 label for the device certificate.
 *
 * Device certificate corresponding to pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS.
 */
#define pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS    "Device Cert"

/**
 * @brief The PKCS #11 label for the object to be used for code verification.
 *
 * Used by over-the-air update code to verify an incoming signed image.
 */
#define pkcs11configLABEL_CODE_VERIFICATION_KEY         "Code Verify Key"

/**
 * @brief The PKCS #11 label for Just-In-Time-Provisioning.
 *
 * The certificate corresponding to the issuer of the device certificate
 * (pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS) when using the JITR or
 * JITP flow.
 */
#define pkcs11configLABEL_JITP_CERTIFICATE              "JITP Cert"

/**
 * @brief The PKCS #11 label for the AWS Trusted Root Certificate.
 *
 * @see aws_default_root_certificates.h
 */
#define pkcs11configLABEL_ROOT_CERTIFICATE              "Root Cert"

#endif /* _AWS_PKCS11_CONFIG_H_ include guard. */
