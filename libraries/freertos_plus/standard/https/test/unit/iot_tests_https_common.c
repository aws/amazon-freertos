/*
 * Amazon FreeRTOS HTTPS Client V1.0.0
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
 * @file iot_tests_https_common.c
 * @brief Common definitions for the HTTPS Client unit tests.
 */
#include "iot_tests_https_common.h"

/**
 * @brief HTTPS Client connection user buffer to share among the tests. 
 * 
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a 
 * concern.
 */
uint8_t _pConnUserBuffer[HTTPS_TEST_CONN_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief HTTPS Request user buffer to share among the tests.
 * 
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a 
 * concern.
 */
uint8_t _pReqUserBuffer[HTTPS_TEST_REQ_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief HTTPS Response user buffer to share among the tests.
 * 
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a 
 * concern.
 */
uint8_t _pRespUserBuffer[HTTPS_TEST_RESP_USER_BUFFER_SIZE] = { 0 };