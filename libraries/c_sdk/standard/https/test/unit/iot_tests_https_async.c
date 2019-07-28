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
 * @file iot_tests_https_sync.c
 * @brief Tests for IotHttpsClient_SendAsync(), IotHttpsClient_WriteRequestBody(), and IotHttpsClient_ReadResponseBody()
 * in iot_https_client.h.
 */

#include "iot_tests_https_common.h"
#include "platform/iot_clock.h"

/*-----------------------------------------------------------*/

/**
 * @brief Test group for HTTPS Client Async Unit tests.
 */
TEST_GROUP( HTTPS_Client_Unit_Async );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for the HTTP Client Async unit tests.
 */
TEST_SETUP( HTTPS_Client_Unit_Async )
{
    /* This will initialize the library before every test case, which is OK. */
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, IotHttpsClient_Init());
}

/*-----------------------------------------------------------*/

/**
 * @brief Test teardown for the HTTP Client Async unit tests.
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_Async )
{
    IotHttpsClient_Deinit();
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( HTTPS_Client_Unit_Async )
{
}

