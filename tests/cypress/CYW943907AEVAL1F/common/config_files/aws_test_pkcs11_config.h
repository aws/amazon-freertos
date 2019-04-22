/*
 * Amazon FreeRTOS
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

/**
 * @file aws_test_pkcs11_config.h
 * @brief Port-specific variables for PKCS11 tests.
 */
#ifndef _AWS_TEST_PKCS11_CONFIG_H_
#define _AWS_TEST_PKCS11_CONFIG_H_

/**
 * @brief Number of simultaneous tasks for multithreaded tests.
 *
 * Each task consumes both stack and heap space, which may cause memory allocation
 * failures if too many tasks are created.
 */
#define pkcs11testMULTI_THREAD_TASK_COUNT     ( 2 )

/**
 * @brief The number of iterations of the test that will run in multithread tests.
 *
 * A single iteration of Signing and Verifying may take up to a minute on some
 * boards. Ensure that pkcs11testEVENT_GROUP_TIMEOUT is long enough to accommodate
 * all iterations of the loop.
 */
#define pkcs11testMULTI_THREAD_LOOP_COUNT     ( 10 )

/**
 * @brief
 *
 * All tasks of the SignVerifyRoundTrip_MultitaskLoop test must finish within
 * this timeout, or the test will fail.
 */
#define pkcs11testEVENT_GROUP_TIMEOUT_MS      ( pdMS_TO_TICKS( 1000000UL ) )

/**
 * @brief The index of the slot that should be used to open sessions for PKCS #11 tests.
 */
#define pkcs11testSLOT_NUMBER                 ( 0 )

/*
 * @brief Set to 1 if RSA private keys are supported by the platform.  0 if not.
 */
#define pkcs11testRSA_KEY_SUPPORT             ( 1 )

/*
 * @brief Set to 1 if elliptic curve private keys are supported by the platform.  0 if not.
 */
#define pkcs11testEC_KEY_SUPPORT              ( 1 )

/*
 * @brief Set to 1 if importing device private key via C_CreateObject is supported.  0 if not.
 */
#define pkcs11testCREATE_OBJECT_SUPPORT       ( 1 )

/*
 * @brief Set to 1 if generating a device private-public key pair via C_GenerateKeyPair. 0 if not.
 */
#define pkcs11testGENERATE_KEYPAIR_SUPPORT    ( 1 )

#endif /* _AWS_TEST_PKCS11_CONFIG_H_ */
