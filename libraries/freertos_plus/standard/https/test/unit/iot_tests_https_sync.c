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
 * @brief Tests for IotHttpsClient_SendSync() in iot_https_client.h.
 */

#include "iot_tests_https_common.h"

/*-----------------------------------------------------------*/

// TODO 

/*-----------------------------------------------------------*/

/**
 * @brief Test group for HTTPS Client API tests.
 */
TEST_GROUP( HTTPS_Client_Unit_Sync );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for HTTPS Client API tests.
 */
TEST_SETUP( HTTPS_Client_Unit_Sync )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for HTTPS Client API tests.
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_Sync )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Client API tests.
 */
TEST_GROUP_RUNNER( HTTPS_Client_Unit_Sync )
{
    /* Initialize the library once. */
    IotHttpsClient_Init();

    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncInvalidParameters);

    /* Deinitialize the library after the tests. */
    IotHttpsClient_Deinit();
}

/*-----------------------------------------------------------*/

TEST( HTTPS_Client_Unit_Sync, SendSyncInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = 0;
    IotHttpsResponseInfo_t testRespInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL(reqHandle);

    /* Test a NULL connHandle parameter. */
    returnCode = IotHttpsClient_SendSync(NULL, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(respHandle);

    /* Test a NULL reqHandle parameter */
    returnCode = IotHttpsClient_SendSync(connHandle, NULL, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(respHandle);

    /* Test a NULL pRespHandle parameter. */
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, NULL, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(respHandle);

    /* Test a NULL pRespInfo parameter. */
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, NULL, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(respHandle);

    /* Test an async request handle. */
    reqHandle->isAsync = true;
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(respHandle);
    /* Restore the request handle for other tests. */
    reqHandle->isAsync = false;

    /* A NULL response user buffer. */
    memcpy(&testRespInfo, &_respInfo, sizeof(IotHttpsResponseInfo_t));
    testRespInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &testRespInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(respHandle);

    /* Test a response user buffer that is too small. */
    memcpy(&testRespInfo, &_respInfo, sizeof(IotHttpsResponseInfo_t));
    testRespInfo.userBuffer.bufferLen = responseUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &testRespInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    TEST_ASSERT_NULL(respHandle);

    /* TODO: Add tests that mock a failure in the task pool scheduling and semaphore creation. */
}


