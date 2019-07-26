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
 * @brief A IotHttpsClientCallbacks_t to share among the tests.
 */
static IotHttpsClientCallbacks_t _callbacks = { 0 };

/**
 * @brief A IotHttpsAsyncInfo_t to share among the tests.
 * 
 * The tests will replace callbacks in .callbacks as needed and add pPrivData as need for the test.
 */
static IotHttpsAsyncInfo_t _asyncInfo = {
    .callbacks = { 0 },
    .pPrivData = NULL
};

/**
 * @brief A IotHttpsRequestInfo_t to share among the tests. 
 */
static IotHttpsRequestInfo_t _reqInfo = {
    .pPath = HTTPS_TEST_PATH,
    .pathLen = sizeof( HTTPS_TEST_PATH ) - 1,
    .method = IOT_HTTPS_METHOD_GET,
    .pHost = HTTPS_TEST_ADDRESS,
    .hostLen = sizeof( HTTPS_TEST_ADDRESS ) - 1,
    .isNonPersistent = false,
    .userBuffer.pBuffer = _pReqUserBuffer,
    .userBuffer.bufferLen = sizeof( _pReqUserBuffer ),
    .isAsync = true,
    .pAsyncInfo = &_asyncInfo
};

/**
 * @brief A IotHttpsResponseInfo_t to share among the tests.
 */
static IotHttpsResponseInfo_t _respInfo = {
    .userBuffer.pBuffer = _pRespUserBuffer,
    .userBuffer.bufferLen = sizeof( _pRespUserBuffer ),
    .pSyncInfo = NULL
};

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
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, WriteRequestBodyInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, WriteRequestBodySuccess );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_WriteRequestBody() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_Async, WriteRequestBodyInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    int isCompleteSuccess = 1;
    int isCompleteUnsupported = 0;

    /* Get a valid request handle to test other items being with proper coverage. */
    reqHandle = _getReqHandle(&_reqInfo);

    /* Test a NULL request handle parameter. */
    returnCode = IotHttpsClient_WriteRequestBody(NULL, (uint8_t*)HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test a NULL buf parameter. */
    returnCode = IotHttpsClient_WriteRequestBody(reqHandle, NULL, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test an unsupported isComplete parameter. */
    returnCode = IotHttpsClient_WriteRequestBody(reqHandle, (uint8_t*)HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteUnsupported);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NOT_SUPPORTED, returnCode);

    /* Test that for a synchronous request the function fails. */
    _reqInfo.isAsync = false;
    reqHandle = _getReqHandle(&_reqInfo);
    returnCode = IotHttpsClient_WriteRequestBody(reqHandle, (uint8_t*)HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    /* Restore the global _reqInfo so other tests can use it. */
    _reqInfo.isAsync = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a successful call to IotHttpsClient_WriteRequestBody().
 */
TEST( HTTPS_Client_Unit_Async, WriteRequestBodySuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    int isCompleteSuccess = 1;

    /* Get a valid request handle to use for testing. */
    reqHandle = _getReqHandle(&_reqInfo);

    /* Test that for the same reqHandle we cannot write twice. */
    returnCode = IotHttpsClient_WriteRequestBody(reqHandle, (uint8_t*)HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);

    /* Test that we cannot write twice on the same request handle. */
    returnCode = IotHttpsClient_WriteRequestBody(reqHandle, (uint8_t*)HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess);
    TEST_ASSERT_EQUAL(IOT_HTTPS_MESSAGE_FINISHED, returnCode);
}