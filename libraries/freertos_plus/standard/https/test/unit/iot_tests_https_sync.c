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
#include "platform/iot_clock.h"

/*-----------------------------------------------------------*/

/**
 * @brief Timeout for IotHttpsClient_SendSync() for all tests.
 */
#define HTTPS_TEST_SYNC_TIMEOUT_MS                      ( (uint32_t) 30000 )

/**
 * @brief Wait time before the network receive callback is invoked.
 * 
 * This wait time is to mimic not only response being received on the network, but also needs to incorporate the time
 * it takes to complete sending the request.
 */
#define HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS     ( ( uint32_t) 300 )

/*-----------------------------------------------------------*/

/**
 * @brief The connection handle that is needed by tests that are testing a workflow that involves receiving data on the 
 * network. 
 * 
 * HTTP tests run sequentially, so there is no race condition here.
 */
static IotHttpsConnectionHandle_t _receiveCallbackConnHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

/**
 * @brief Flag indicating that the _invokeNetworkReceiveCallback task was already created.
 * 
 * This is reset to false before each test. This is set during tests that are testing a workflow that involes 
 * receiving data on the network. 
 * 
 * We do not want to create two tasks that invoke the network receive callback. That is incorrect behavior of the
 * network receive callback.
 * 
 * HTTP tests run sequentially, so there is no race condition here.
 * 
 * This is used for tests sending and receiving one response.
 */
static bool _alreadyCreatedReceiveCallbackThread = false;

/**
 * @brief The request handle that is currently being serviced in a test.
 * 
 * This is needed to replace the parseFunc, in the corresponding response, with a unit test mock. 
 */
static IotHttpsRequestHandle_t _currentlySendingRequestHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

/*-----------------------------------------------------------*/

/**
 * @brief thread that invokes the _networkReceiveCallback internal to the library.
 */
static void _invokeNetworkReceiveCallback( void *pArgument )
{
    void *pNetworkConnection = pArgument;

    /* Sleep for a bit to wait for the rest of test request to finished sending and simulate a network response. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS );

    /* Envoke the network receive callback. */
    IotTestHttps_networkReceiveCallback( pNetworkConnection, _receiveCallbackConnHandle );
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that fails. 
 */
static size_t _networkSendFailHeaders( void * pConnection,
                                       const uint8_t * pMessage,
                                       size_t messageLength )
{
    (void)pConnection;
    (void)pMessage;
    (void)messageLength;
    
    size_t retValue = 0;

    /* Check if we are sending the headers to return failure. */
    if( pMessage == _currentlySendingRequestHandle->pHeaders)
    {
        retValue = 0;
    }
    else
    {
        retValue = 1;
    }
    return retValue;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that succeeds.
 * 
 * Because the network send succeeded we mimic the network by starting a thread to envoke the network receive callback.
 */
static size_t _networkSendSuccess( void * pConnection,
                                   const uint8_t * pMessage,
                                   size_t messageLength )
{
    (void)pConnection;
    (void)pMessage;
   
    /* This thread must be created only once to mimic the behavior of the network abstraction.  */
    if( !_alreadyCreatedReceiveCallbackThread )
    {
        Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                  pConnection,
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE);
        _alreadyCreatedReceiveCallbackThread = true;
    }

    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction receive function that timesout. 
 */
static size_t _networkReceiveFail( void * pConnection,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested )
{
    (void)pConnection;
    (void)pBuffer;
    (void)bytesRequested;
    return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network send success that replaces the _httpsResponse_t.httpParserInfo.parseFunc with one from this test.
 * 
 * This function relies on _httpsResponse_t.pHttpsResponse getting set, in the currently processing _httpsResponse_t,
 * before the network send function is called.
 */
static size_t _networkSendSuccessWithSettingParseFail( void * pConnection,
                                                       const uint8_t * pMessage,
                                                       size_t messageLength )
{
    (void)pConnection;
    (void)pMessage;

    /* Set the response parser function to mock a failure. */
    _currentlySendingRequestHandle->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFail;

    /* This thread must be created only once to mimic the behavior of the network abstraction.  */
    if( !_alreadyCreatedReceiveCallbackThread )
    {
        Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                  pConnection,
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE);
        _alreadyCreatedReceiveCallbackThread = true;
    }
    
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction receive function that succeeds. 
 */
static size_t _networkReceiveSuccess( void * pConnection,
                                      uint8_t * pBuffer,
                                      size_t bytesRequested )
{
    size_t testHeadersLen = sizeof( HTTPS_TEST_HEADER_LINES ) - 1;
    size_t copyLen = 0;

    (void)pConnection;
    (void)pBuffer;
    (void)bytesRequested;

    /* Fill with random response data so that this function succeeds if used in a test where the parser must succeed. */
    if( testHeadersLen < bytesRequested )
    {
        copyLen = testHeadersLen;
    }
    else
    {
        copyLen = bytesRequested;
    }
    memcpy(pBuffer, HTTPS_TEST_HEADER_LINES, copyLen);

    return bytesRequested;
}

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
    /* Reset the shared network interface. */
    ( void ) memset( &_networkInterface, 0x00, sizeof( IotNetworkInterface_t ) );

    /* Reset some global static variables. */
    _receiveCallbackConnHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    _alreadyCreatedReceiveCallbackThread = false;
    _currentlySendingRequestHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

    /* This will initialize the library before every test case, which is OK. */
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, IotHttpsClient_Init());
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for HTTPS Client API tests.
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_Sync )
{
    IotHttpsClient_Deinit();
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Client API tests.
 */
TEST_GROUP_RUNNER( HTTPS_Client_Unit_Sync )
{
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncInvalidParameters);
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureSendingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureParsingHeaders );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invalid parameters input into IotHttpsClient_SendSync().
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
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
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failures to send over the network the headers and the body.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureSendingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Register a failing send function in the network interface. */
    _networkInterface.send = _networkSendFailHeaders;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    /* Get a valid request handle. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL(reqHandle);

    _currentlySendingRequestHandle = reqHandle;

    /* Test the network send failing. */
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failures to receive over the network the headers and body.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receive = _networkReceiveFail;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    
    /* Get a valid request handle. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL(reqHandle);

    /* Set the global test connection handle to be passed to the library network receive callback. */
    memcpy(&_receiveCallbackConnHandle, &connHandle, sizeof(IotHttpsConnectionHandle_t));

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failure to parse over the network the headers and body.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureParsingHeaders)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    _networkInterface.send = _networkSendSuccessWithSettingParseFail;
    _networkInterface.receive = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    
    /* Get a valid request handle. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL(reqHandle);
    _currentlySendingRequestHandle = reqHandle;

    /* Set the global test connection handle to be passed to the library network receive callback. */
    memcpy(&_receiveCallbackConnHandle, &connHandle, sizeof(IotHttpsConnectionHandle_t));

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_PARSING_ERROR, returnCode);
}
