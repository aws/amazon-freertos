/*
 * FreeRTOS V202107.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_ota_pal_test_access_declare.h
 * @brief Function wrappers that access private methods in ota_pal.c.
 *
 * Needed for testing private functions.
 */

#ifndef AWS_OTA_PAL_TEST_ACCESS_DECLARE_H_
#define AWS_OTA_PAL_TEST_ACCESS_DECLARE_H_

#include "ota.h"
#include "aws_test_ota_config.h"

#if otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
    OtaPalStatus_t test_otaPal_CheckFileSignature( OtaFileContext_t * const C );
#endif

#if otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
    uint8_t * test_otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                    uint32_t * const ulSignerCertSize );
#endif

#endif /* ifndef AWS_OTA_PAL_TEST_ACCESS_DECLARE_H_ */
