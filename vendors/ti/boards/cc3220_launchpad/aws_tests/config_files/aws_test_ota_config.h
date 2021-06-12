/*
 * FreeRTOS V202104.00
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
 * @file aws_test_ota_config.h
 * @brief Port-specific variables for firmware Over-the-Air Update tests. */

#ifndef AWS_TEST_OTA_CONFIG_H_
#define AWS_TEST_OTA_CONFIG_H_

/**
 * @brief Path to cert for OTA test PAL. Used to verify signature.
 * If applicable, the device must be pre-provisioned with this certificate. Please see
 * test/common/ota/test_files for the set of certificates.
 */
#define otatestpalCERTIFICATE_FILE                         "rsa-sha1-signer.crt.der"

/**
 * @brief TI CC3220 has a hard-coded name for the firmware image to boot.
 */
#define otatestpalFIRMWARE_FILE                            "/sys/mcuflashimg.bin"

/**
 * @brief Some boards will use aws_codesigner_certificate.h instead of a pre-provisioned
 * certificate.
 */
#define otatestpalUSE_FILE_SYSTEM                          1

/**
 * @brief 1 if prvPAL_CheckFileSignature is implemented in aws_ota_pal.c.
 */
#define otatestpalCHECK_FILE_SIGNATURE_SUPPORTED           0

/**
 * @brief 1 if prvPAL_ReadAndAssumeCertificate is implemented in the aws_ota_pal.c.
 */
#define otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED    0

/**
 * @brief 1 if using PKCS #11 to access the code sign certificate from NVM.
 */
#define otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11    0

/**
 * @brief Include of signature testing data applicable to this device.
 */
#include "aws_test_ota_pal_rsa_sha1_signature.h"

/**
 * @brief Major version of the firmware.
 *
 *        This is used in the OTA demo to set the appFirmwareVersion variable
 *        that is declared in the ota_appversion32.h file in the OTA library.
 */
#define APP_VERSION_MAJOR            0

/**
 * @brief Minor version of the firmware.
 *
 *        This is used in the OTA demo to set the appFirmwareVersion variable
 *        that is declared in the ota_appversion32.h file in the OTA library.
 */
#define APP_VERSION_MINOR            9

/**
 * @brief Build version of the firmware.
 *
 *        This is used in the OTA demo to set the appFirmwareVersion variable
 *        that is declared in the ota_appversion32.h file in the OTA library.
 */
#define APP_VERSION_BUILD            2

/**
 * @brief Define a valid and invalid signature verification method for this
 * platform (TI). These are used for generating test JSON docs.
 */
#define otatestVALID_SIG_METHOD      "sig-sha1-rsa"
#define otatestINVALID_SIG_METHOD    "sig-sha256-ecdsa"

#endif /* AWS_TEST_OTA_CONFIG_H_ */
