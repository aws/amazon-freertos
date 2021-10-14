/*
 * FreeRTOS V1.1.4
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
#define otatestpalCERTIFICATE_FILE                         "tests\\integration_test\\ota_pal\\test_files\\ecdsa-sha256-signer.crt.pem.test"

/**
 * @brief Some devices have a hard-coded name for the firmware image to boot.
 */
#define otatestpalFIRMWARE_FILE                            "dummy.bin"

/**
 * @brief Some boards OTA PAL layers will use the file names passed into it for the
 * image and the certificates because their non-volatile memory is abstracted by a
 * file system. Set this to 1 if that is the case for your device.
 */
#define otatestpalUSE_FILE_SYSTEM                          0

/**
 * @brief 1 if otaPal_CheckFileSignature() is implemented in aws_ota_pal.c.
 */
#define otatestpalCHECK_FILE_SIGNATURE_SUPPORTED           1

/**
 * @brief 1 if otaPal_ReadAndAssumeCertificate() is implemented in aws_ota_pal.c.
 */
#define otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED    1

/**
 * @brief 1 if using PKCS #11 to access the code sign certificate from NVM.
 */
#define otatestpalREAD_CERTIFICATE_FROM_NVM_WITH_PKCS11    1

/**
 * @brief Include of signature testing data applicable to this device.
 */
#include "aws_test_ota_pal_ecdsa_sha256_signature.h"

/**
 * @brief Major version of the firmware.
 *
 *        This is used in the OTA demo to set the appFirmwareVersion variable
 *        that is declared in the ota_appversion32.h file in the OTA library.
 */
#define APP_VERSION_MAJOR    0

/**
 * @brief Minor version of the firmware.
 *
 *        This is used in the OTA demo to set the appFirmwareVersion variable
 *        that is declared in the ota_appversion32.h file in the OTA library.
 */
#define APP_VERSION_MINOR    9

/**
 * @brief Build version of the firmware.
 *
 *        This is used in the OTA demo to set the appFirmwareVersion variable
 *        that is declared in the ota_appversion32.h file in the OTA library.
 */
#define APP_VERSION_BUILD    2

#endif /* ifndef AWS_TEST_OTA_CONFIG_H_ */
