/*
 * FreeRTOS HTTPS Client V1.2.0
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
 * @file iot_tests_https_sync.c
 * @brief Tests for IotHttpsClient_SendSync() in iot_https_client.h.
 */

#include "iot_tests_https_common.h"
#include "platform/iot_clock.h"

/*-----------------------------------------------------------*/

/**
 * @brief Timeout for IotHttpsClient_SendSync() for all tests.
 */
#define HTTPS_TEST_SYNC_TIMEOUT_MS                         ( ( uint32_t ) 30000 )

/**
 * @brief Wait time before the network receive callback is invoked.
 *
 * This wait time is to mimic not only response being received on the network, but also needs to incorporate the time
 * it takes to complete sending the request.
 */
#define HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS        ( ( uint32_t ) 300 )

/**
 * Definitions for part of the small HTTP test response.
 *
 * Theser are used to verify behavior when the header buffer ends with just a part of the HTTP response headers.
 */
#define HTTPS_TEST_SMALL_RESPONSE_UP_TO_CARRIAGE_RETURN    "HTTP/1.1 200 OK\r\nheader0: value0\r\nheader1: value1\r"   /**< @brief A test response from the network that goes up to a carriage return in the HTTP response headers. */
#define HTTPS_TEST_SMALL_RESPONSE_UP_TO_NEWLINE            "HTTP/1.1 200 OK\r\nheader0: value0\r\nheader1: value1\r\n" /**< @brief A test response from the network that goes up to a newline in the HTTP response headers.  */
#define HTTPS_TEST_SMALL_RESPONSE_UP_TO_COLON              "HTTP/1.1 200 OK\r\nheader0: value0\r\nheader1:"            /**< @brief A test response from the network that goes up to a colon in the HTTP response headers. */
#define HTTPS_TEST_SMALL_RESPONSE_UP_TO_SPACE              "HTTP/1.1 200 OK\r\nheader0: value0\r\nheader1: "           /**< @brief A test response from the network that goes up to the space after a colon in the HTTP response headers. */
#define HTTPS_TEST_SMALL_RESPONSE_UP_TO_SPACE_IN_BETWEEN_VALUE \
    "HTTP/1.1 200 OK\r\nheader0: value0\r\nheader1: value1\r\nheader2: value2 "                                        /**< @brief A test response from the network that goes up to the space that is after a header value in the HTTP response headers. */
#define HTTPS_TEST_HEADER1                                 "header1"                                                   /**< @brief The string literal for a header field named "header1". This is for test result checking. */
#define HTTPS_TEST_HEADER1_PLUS_COLON                      "header1:"                                                  /**< @brief The string literal for a header field named "header1" including the colon after. */
#define HTTPS_TEST_HEADER1_PLUS_SPACE                      "header1: "                                                 /**< @brief The string literal for a header field named "header1" including the colon and space after. */
#define HTTPS_TEST_HEADER2                                 "header2"                                                   /**< @brief The string literal for a header field named "header2". This is for test result checking. */
#define HTTPS_TEST_HEADER_VALUE1                           "value1"                                                    /**< @brief The string literal for a header value named "value1". */
#define HTTPS_TEST_HEADER_VALUE2_PLUS_SPACE                "value2 "                                                   /**< @brief The string literal for a header value named "value2." */
#define HTTPS_TEST_HEADER_VALUE2_VALUE2A                   "value2 value2a"                                            /**< @brief The string literal a header value that contains two strings separated by a space. */
#define HTTPS_TEST_HEADER_VALUE1_PLUS_CARRIAGE_RETURN      "value1\r"                                                  /**< @brief the string literal for a header value with the carriage return following it. */
#define HTTPS_TEST_HEADER_VALUE1_PLUS_NEWLINE              "value1\r\n"                                                /**< @brief the string ltieral for a header value with the carriage return and newline following it. */

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
 * @brief The request handle that is currently being serviced in the current tests network receive callback.
 *
 * This is needed to replace the parseFunc, in the corresponding response, with a unit test mock.
 * This is needed to check if the network receive or network send buffer is the start of the header or the body buffers
 * to get proper failure coverage.
 * This is needed to check if the network receive buffer is the start of the header buffer, so that we can receive
 * part of the response.
 */
static IotHttpsRequestHandle_t _currentlySendingRequestHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

/**
 * #IotHttpsSyncInfo_t for requests and response to share among the tests.
 *
 * Even though the method is a GET method for the test _reqInfo, we apply a request body for unit testing purposes. A
 * request body is allowed for a GET method according to the HTTP specification, although it is pointless in practice.
 */
static IotHttpsSyncInfo_t _syncRequestInfo =
{
    .pBody   = ( uint8_t * ) ( HTTPS_TEST_REQUEST_BODY ),
    /* Constant C string in macros have a NULL terminating character that we want to omit. */
    .bodyLen = HTTPS_TEST_REQUEST_BODY_LENGTH
}; /**< @brief A #IotHttpsSyncInfo_t for requests to share among the tests. */

static IotHttpsSyncInfo_t _syncResponseInfo =
{
    .pBody   = _pRespBodyBuffer,
    /* Constant C string in macros have a NULL terminating character that we want to omit. */
    .bodyLen = sizeof( _pRespBodyBuffer )
}; /**< @brief A #IotHttpsSyncInfo_t for responses to share among the tests. */

/**
 * @brief A IotHttpsRequestInfo_t to share among the tests.
 */
static IotHttpsRequestInfo_t _reqInfo =
{
    .pPath                = HTTPS_TEST_PATH,
    .pathLen              = sizeof( HTTPS_TEST_PATH ) - 1,
    .method               = IOT_HTTPS_METHOD_GET,
    .pHost                = HTTPS_TEST_ADDRESS,
    .hostLen              = sizeof( HTTPS_TEST_ADDRESS ) - 1,
    .isNonPersistent      = false,
    .userBuffer.pBuffer   = _pReqUserBuffer,
    .userBuffer.bufferLen = sizeof( _pReqUserBuffer ),
    .isAsync              = false,
    .u.pSyncInfo          = &_syncRequestInfo
};

/**
 * @brief A IotHttpsResponseInfo_t to share among the tests.
 */
static IotHttpsResponseInfo_t _respInfo =
{
    .userBuffer.pBuffer   = _pRespUserBuffer,
    .userBuffer.bufferLen = sizeof( _pRespUserBuffer ),
    .pSyncInfo            = &_syncResponseInfo
};

/*-----------------------------------------------------------*/

/**
 * @brief thread that invokes the _networkReceiveCallback internal to the library.
 */
static void _invokeNetworkReceiveCallback( void * pArgument )
{
    void * pNetworkConnection = pArgument;

    /* Sleep for a bit to wait for the rest of test request to finished sending and simulate a network response. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS );

    /* Envoke the network receive callback. */
    IotTestHttps_networkReceiveCallback( pNetworkConnection, _receiveCallbackConnHandle );
}

/*-----------------------------------------------------------*/

/**
 * @brief Mock the http parser execution failing when parsing the HTTP headers buffer.
 */
static size_t _httpParserExecuteFailHeaders( http_parser * parser,
                                             const http_parser_settings * settings,
                                             const char * data,
                                             size_t len )
{
    ( void ) settings;
    ( void ) len;

    if( data == ( char * ) ( _currentlySendingRequestHandle->pHttpsResponse->pHeadersCur ) )
    {
        parser->http_errno = HPE_UNKNOWN;
    }
    else
    {
        http_parser_execute( parser, settings, data, len );
    }

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Mock the http parser execution failing when parsing the HTTP body buffer.
 */
static size_t _httpParserExecuteFailBody( http_parser * parser,
                                          const http_parser_settings * settings,
                                          const char * data,
                                          size_t len )
{
    ( void ) settings;
    ( void ) len;

    if( data == ( char * ) ( _currentlySendingRequestHandle->pHttpsResponse->pBodyCur ) )
    {
        parser->http_errno = HPE_UNKNOWN;
    }
    else
    {
        http_parser_execute( parser, settings, data, len );
    }

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that fails sending the HTTP headers.
 */
static size_t _networkSendFailHeaders( void * pConnection,
                                       const uint8_t * pMessage,
                                       size_t messageLength )
{
    ( void ) pConnection;

    size_t retValue = 0;

    /* Check if we are sending the headers to return failure. */
    if( pMessage == _currentlySendingRequestHandle->pHeaders )
    {
        retValue = 0;
    }
    else
    {
        retValue = messageLength;
    }

    return retValue;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that fails sending the HTTP body.
 */
static size_t _networkSendFailBody( void * pConnection,
                                    const uint8_t * pMessage,
                                    size_t messageLength )
{
    ( void ) pConnection;

    size_t retValue = 0;

    /* Check if we are sending the body to return failure. */
    if( pMessage == _currentlySendingRequestHandle->pBody )
    {
        retValue = 0;
    }
    else
    {
        retValue = messageLength;
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
    ( void ) pMessage;

    /* This thread must be created only once to mimic the behavior of the network abstraction.  */
    if( !_alreadyCreatedReceiveCallbackThread )
    {
        Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                  pConnection,
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE );
        _alreadyCreatedReceiveCallbackThread = true;
    }

    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction receive function that fails when sending the HTTP headers.
 */
static size_t _networkReceiveFailHeaders( void * pConnection,
                                          uint8_t * pBuffer,
                                          size_t bytesRequested )
{
    ( void ) pConnection;

    size_t retValue = 0;

    if( pBuffer == _currentlySendingRequestHandle->pHttpsResponse->pHeadersCur )
    {
        retValue = 0;
    }
    else
    {
        retValue = bytesRequested;
    }

    return retValue;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction receive function that fails when sending the HTTP body.
 */
static size_t _networkReceiveFailBody( void * pConnection,
                                       uint8_t * pBuffer,
                                       size_t bytesRequested )
{
    ( void ) pConnection;

    size_t retValue = 0;

    /* We may have received some body in the headers, so pBodyCur will be incremented during the parsing of
     * data received when receiving into the header buffer. Given this case, when receiving from the network
     * into the body buffer, start of the pBuffer should always be pBodyCur. */
    if( pBuffer == _currentlySendingRequestHandle->pHttpsResponse->pBodyCur )
    {
        retValue = 0;
    }
    else
    {
        retValue = _networkReceiveSuccess( pConnection, pBuffer, bytesRequested );
    }

    return retValue;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network send success that replaces the _httpsResponse_t.httpParserInfo.parseFunc with one from this test.
 *
 * This function relies on _httpsResponse_t.pHttpsResponse getting set, in the currently processing _httpsResponse_t,
 * before the network send function is called.
 */
static size_t _networkSendSuccessWithSettingParseFailForHeaders( void * pConnection,
                                                                 const uint8_t * pMessage,
                                                                 size_t messageLength )
{
    ( void ) pMessage;

    /* Set the response parser function to mock a failure. */
    _currentlySendingRequestHandle->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFailHeaders;

    /* This thread must be created only once to mimic the behavior of the network abstraction.  */
    if( !_alreadyCreatedReceiveCallbackThread )
    {
        Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                  pConnection,
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE );
        _alreadyCreatedReceiveCallbackThread = true;
    }

    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network send success that replaces the _httpsResponse_t.httpParserInfo.parseFunc with one from this test.
 *
 * This function relies on _httpsResponse_t.pHttpsResponse getting set, in the currently processing _httpsResponse_t,
 * before the network send function is called.
 */
static size_t _networkSendSuccessWithSettingParseFailForBody( void * pConnection,
                                                              const uint8_t * pMessage,
                                                              size_t messageLength )
{
    ( void ) pMessage;

    /* Set the response parser function to mock a failure. */
    _currentlySendingRequestHandle->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFailBody;

    /* This thread must be created only once to mimic the behavior of the network abstraction.  */
    if( !_alreadyCreatedReceiveCallbackThread )
    {
        Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                  pConnection,
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE );
        _alreadyCreatedReceiveCallbackThread = true;
    }

    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for HTTPS Client Sync Unit tests.
 */
TEST_GROUP( HTTPS_Client_Unit_Sync );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for HTTPS Client Sync Unit tests.
 */
TEST_SETUP( HTTPS_Client_Unit_Sync )
{
    /* Reset the shared network interface. */
    ( void ) memset( &_networkInterface, 0x00, sizeof( IotNetworkInterface_t ) );
    /* Reset the shared response message buffer. */
    ( void ) memset( _pRespMessageBuffer, 0x00, sizeof( _pRespMessageBuffer ) );
    /* Reset the buffer we receive HTTP body into. */
    ( void ) memset( _pRespBodyBuffer, 0x00, sizeof( _pRespBodyBuffer ) );

    /* Reset some global static variables needed for mimicing the network. */
    _receiveCallbackConnHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    _alreadyCreatedReceiveCallbackThread = false;
    _currentlySendingRequestHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    _nextRespMessageBufferByteToReceive = 0;

    /* This will initialize the library before every test case, which is OK. */
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, IotHttpsClient_Init() );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for HTTPS Client Sync Unit tests..
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_Sync )
{
    IotHttpsClient_Cleanup();
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Client @ref https_client_function_sendsync.
 */
TEST_GROUP_RUNNER( HTTPS_Client_Unit_Sync )
{
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureSendingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureSendingBody )
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureParsingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureParsingBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncSomeBodyInHeaderBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncSomeHeaderInBodyBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncEntireResponseInHeaderBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncBodyTooLarge );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncBodyBufferNull );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncPersistentRequest );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncNonPersistentRequest );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithCarriageReturnSeparator );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithNewlineSeparator );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithColonSeparator );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithSpaceSeparator );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithSpaceAfterHeaderValue );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncChunkedResponse );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invalid parameters input into IotHttpsClient_SendSync().
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    IotHttpsResponseInfo_t testRespInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Test a NULL connHandle parameter. */
    returnCode = IotHttpsClient_SendSync( NULL, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a NULL reqHandle parameter */
    returnCode = IotHttpsClient_SendSync( connHandle, NULL, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a NULL pRespHandle parameter. */
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, NULL, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a NULL pRespInfo parameter. */
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, NULL, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test an async request handle. */
    reqHandle->isAsync = true;
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );
    /* Restore the request handle for other tests. */
    reqHandle->isAsync = false;

    /* A NULL response user buffer. */
    memcpy( &testRespInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    testRespInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &testRespInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a response user buffer that is too small. */
    memcpy( &testRespInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    testRespInfo.userBuffer.bufferLen = responseUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &testRespInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test sending a request on a close connection. */
    memcpy( &testRespInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    connHandle->isConnected = false;
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &testRespInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failures to send over the network the request headers.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureSendingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Test a failure to the send the headers. */
    _networkInterface.send = _networkSendFailHeaders;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _currentlySendingRequestHandle = reqHandle;

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failures to send over the network the request body.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureSendingBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Test a failure to send the body */
    _networkInterface.send = _networkSendFailBody;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _currentlySendingRequestHandle = reqHandle;

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failures to receive over the network the response headers.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Test a failure to receive the headers. */
    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveFailHeaders;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Set the currently sending request to be used in _networkReceiveFailHeaders(). This is to check if the
     * current network receive buffer is the header buffer or the body buffer. */
    _currentlySendingRequestHandle = reqHandle;

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, returnCode );
    /* Verify that the connection was closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failures to receive over the network response body.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Test a failure to receive the body. */
    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveFailBody;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Set the current request handle to be used in _networkReceiveFailBody() for checking if the current network
     * receive buffer is the header buffer or the body buffer. */
    _currentlySendingRequestHandle = reqHandle;

    /* Generate an ideal case header and body message size just for testing a failure to receive. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, returnCode );
    /* Verify that the connection was closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failure to parse over the network the response headers.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureParsingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Test a parsing failure in the headers buffer. */
    _networkInterface.send = _networkSendSuccessWithSettingParseFailForHeaders;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Set the current request handle to be used in _networkSendSuccessWithSettingParseFailForHeaders() to check if the
     * current network receive buffer is the header buffer or the body buffer. */
    _currentlySendingRequestHandle = reqHandle;

    /* Generate an ideal case header and body message size just for testing a failure to parse. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );
    /* Verify that the connection was closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test failure to parse over the network the response body.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncFailureParsingBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    /* Test a parsing failure in the body buffer. */
    _networkInterface.send = _networkSendSuccessWithSettingParseFailForBody;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Set the current request handle to be used in _networkSendSuccessWithSettingParseFailForBody() to check if the
     * current network receive buffer is the header buffer or the body buffer. */
    _currentlySendingRequestHandle = reqHandle;

    /* Generate an ideal case header and body message size just for testing a failure to parse. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );
    /* Verify that the connection was closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we receive the HTTP response body correctly when part of it is network received into the header
 * buffer.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncSomeBodyInHeaderBuffer )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate a response message where part of the body is received into the header buffer while the rest is received
     * into the body buffer. This test relies on the HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH to be less than double the
     * HTTPS_TEST_RESP_BODY_BUFFER_SIZE. */
    TEST_ASSERT_LESS_THAN_MESSAGE( HTTPS_TEST_RESP_BODY_BUFFER_SIZE * 2,
                                   HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH,
                                   "The header buffer size must be less than double of HTTPS_TEST_RESP_BODY_BUFFER_SIZE in order for this test to "
                                   "be valid. Please resize HTTPS_TEST_RESP_USER_BUFFER_SIZE or HTTPS_TEST_RESP_BODY_BUFFER_SIZE." );
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH / 2;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    _verifyHttpResponseBody( bodyLength, _respInfo.pSyncInfo->pBody, 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we receive the HTTP response body correctly when network received headers spill over into the body
 * buffer.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncSomeHeaderInBodyBuffer )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate a response message where part of the headers is received into the body buffer. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH * 2;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    _verifyHttpResponseBody( bodyLength, _respInfo.pSyncInfo->pBody, 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we receive the HTTP response body correctly all of the body is network received into the header
 * buffer.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncEntireResponseInHeaderBuffer )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate a response message where all of the body is in the header buffer. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH / 2;
    bodyLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH / 4;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    _verifyHttpResponseBody( bodyLength, _respInfo.pSyncInfo->pBody, 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we return the correct error code when the HTTP response body is too large to fit into the configured
 * body buffer.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncBodyTooLarge )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate a response message where all of the body is in the header buffer. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE + 1;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_MESSAGE_TOO_LARGE, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a successful synchronous workflow when the body buffer is configured as NULL, but HTTP response body
 * is received on the network.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncBodyBufferNull )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsSyncInfo_t syncInfo = IOT_HTTPS_SYNC_INFO_INITIALIZER;
    IotHttpsResponseInfo_t respInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate a response message with a lot of headers and some body. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH * 2;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE / 2;
    _generateHttpResponseMessage( headerLength, bodyLength );

    /* Set a local testing respInfo to have a NULL body buffer. */
    memcpy( &respInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    syncInfo.pBody = NULL;
    syncInfo.bodyLen = 0;
    respInfo.pSyncInfo = &syncInfo;

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * Test for a successful synchronous workflow that the connection stays open when the request is persistent.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncPersistentRequest )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestInfo_t reqInfo = IOT_HTTPS_REQUEST_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle where the connection is presistent. We memcpy here so that
     * we preserver the global _reqInfo. */
    memcpy( &reqInfo, &_reqInfo, sizeof( IotHttpsRequestInfo_t ) );
    reqInfo.isNonPersistent = false;
    reqHandle = _getReqHandle( &reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate some ideal case header and body. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    _verifyHttpResponseBody( bodyLength, _respInfo.pSyncInfo->pBody, 0 );
    /* Verify that the connection is still open. */
    TEST_ASSERT_TRUE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * Test for a successful synchronous workflow that the connection closes when the request is non-persistent.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncNonPersistentRequest )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestInfo_t reqInfo = IOT_HTTPS_REQUEST_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle where the connection is presistent. We memcpy here so that
     * we preserver the global _reqInfo. */
    memcpy( &reqInfo, &_reqInfo, sizeof( IotHttpsRequestInfo_t ) );
    reqInfo.isNonPersistent = true;
    reqHandle = _getReqHandle( &reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Generate some ideal case header and body. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    _verifyHttpResponseBody( bodyLength, _respInfo.pSyncInfo->pBody, 0 );
    /* Verify that the connection is closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we have the correct header data when it ends on the carriage return of the end of the header lines
 * separator.
 *
 * The end of the header lines separator is "\r\n". This test is important because it makes sure that the pHeaderCur pointer
 * in the internal response context is updated even though the http-parser callback does not notify the library of
 * these characters.
 *
 * These tests are separated into difference cases so that I can easily free the memory allocated.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithCarriageReturnSeparator )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseInfo_t respInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    uint32_t userBufferWithHeaderSpaceUpToCarriageReturnLength =
        responseUserBufferMinimumSize + sizeof( HTTPS_TEST_SMALL_RESPONSE_UP_TO_CARRIAGE_RETURN ) - 1;
    uint8_t * pUserBufferWithHeaderSpaceUpToCarriageReturn = NULL;
    char pValueBuffer[ sizeof( HTTPS_TEST_HEADER_VALUE1 ) ] = { 0 };

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Setup the test response message to receive a small test message. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_SMALL_RESPONSE, sizeof( HTTPS_TEST_SMALL_RESPONSE ) );

    /* Test the header buffer receives up to a carriage return in the header separator.  */
    memcpy( &respInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    pUserBufferWithHeaderSpaceUpToCarriageReturn =
        ( uint8_t * ) IotTest_Malloc( userBufferWithHeaderSpaceUpToCarriageReturnLength );
    TEST_ASSERT_NOT_NULL( pUserBufferWithHeaderSpaceUpToCarriageReturn );

    /* Test protect here so that we can free the memory after a failure. */
    if( TEST_PROTECT() )
    {
        respInfo.userBuffer.pBuffer = pUserBufferWithHeaderSpaceUpToCarriageReturn;
        respInfo.userBuffer.bufferLen = userBufferWithHeaderSpaceUpToCarriageReturnLength;

        returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respInfo, timeout );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* If the internal pointers were incremented correctly then we should successfully find the "value1" header value. */
        returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_TEST_HEADER1, FAST_MACRO_STRLEN( HTTPS_TEST_HEADER1 ), pValueBuffer, sizeof( pValueBuffer ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* Assert that ReadHeader() found the correct "value1" header value. */
        TEST_ASSERT_EQUAL( 0, strncmp( pValueBuffer, HTTPS_TEST_HEADER_VALUE1, sizeof( HTTPS_TEST_HEADER_VALUE1 ) ) );
        /* Assert that "value1\r" is found in the header buffer. */
        TEST_ASSERT_NOT_NULL( strstr( ( char * ) ( respHandle->pHeaders ), HTTPS_TEST_HEADER_VALUE1_PLUS_CARRIAGE_RETURN ) );
        /* Assert that the final character in the header buffer is '\r'. */
        TEST_ASSERT_EQUAL( CARRIAGE_RETURN_CHARACTER, ( char ) ( *( respHandle->pHeadersCur - 1 ) ) );
    }

    IotTest_Free( pUserBufferWithHeaderSpaceUpToCarriageReturn );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we have the correct header data when it ends on the newline of the end of the header lines
 * separator.
 *
 * The end of the header lines separator is "\r\n". This test is important because it makes sure that the pHeaderCur pointer
 * in the internal response context is updated even though the http-parser callback does not notify the library of
 * these characters.
 *
 * These tests are separated into difference cases so that I can easily free the memory allocated.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithNewlineSeparator )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseInfo_t respInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    uint32_t userBufferWithHeaderSpaceUpToNewLineLength =
        responseUserBufferMinimumSize + sizeof( HTTPS_TEST_SMALL_RESPONSE_UP_TO_NEWLINE ) - 1;
    uint8_t * pUserBufferWithHeaderSpaceUpToNewLine = NULL;
    char pValueBuffer[ sizeof( HTTPS_TEST_HEADER_VALUE1 ) ] = { 0 };

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Setup the test response message to receive a small test message. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_SMALL_RESPONSE, sizeof( HTTPS_TEST_SMALL_RESPONSE ) );

    /* Test the header buffer receives up to a carriage return in the header separator.  */
    memcpy( &respInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    pUserBufferWithHeaderSpaceUpToNewLine =
        ( uint8_t * ) IotTest_Malloc( userBufferWithHeaderSpaceUpToNewLineLength );
    TEST_ASSERT_NOT_NULL( pUserBufferWithHeaderSpaceUpToNewLine );

    /* Test protect here so that we can free the memory after a failure. */
    if( TEST_PROTECT() )
    {
        respInfo.userBuffer.pBuffer = pUserBufferWithHeaderSpaceUpToNewLine;
        respInfo.userBuffer.bufferLen = userBufferWithHeaderSpaceUpToNewLineLength;

        returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respInfo, timeout );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* If the internal pointers were incremented correctly then we should successfully find the "value1" header value. */
        returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_TEST_HEADER1, FAST_MACRO_STRLEN( HTTPS_TEST_HEADER1 ), pValueBuffer, sizeof( pValueBuffer ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* Assert that ReadHeader() found the correct "value1" header value. */
        TEST_ASSERT_EQUAL( 0, strncmp( pValueBuffer, HTTPS_TEST_HEADER_VALUE1, sizeof( HTTPS_TEST_HEADER_VALUE1 ) ) );
        /* Assert that "value1\r\n" is found in the header buffer. */
        TEST_ASSERT_NOT_NULL( strstr( ( char * ) ( respHandle->pHeaders ), HTTPS_TEST_HEADER_VALUE1_PLUS_NEWLINE ) );
        /* Assert that the final string in the header buffer is "\r\n". */
        TEST_ASSERT_EQUAL( 0,
                           strncmp( HTTPS_END_OF_HEADER_LINES_INDICATOR,
                                    ( char * ) ( respHandle->pHeadersCur - HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH ),
                                    HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH ) );
    }

    IotTest_Free( pUserBufferWithHeaderSpaceUpToNewLine );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we have the correct header data when it ends on the colon character of the header field value
 * separator.
 *
 * The header field value separator is ": ". This test is important because it makes sure that the pHeaderCur pointer
 * in the internal response context is updated even though the http-parser callback does not notify the library of
 * these characters.
 *
 * These tests are separated into difference cases so that I can easily free the memory allocated.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithColonSeparator )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseInfo_t respInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    uint32_t userBufferWithHeaderSpaceUpToColonLength =
        responseUserBufferMinimumSize + sizeof( HTTPS_TEST_SMALL_RESPONSE_UP_TO_COLON ) - 1;
    uint8_t * pUserBufferWithHeaderSpaceUpToColon = NULL;
    char pValueBuffer[ sizeof( HTTPS_TEST_HEADER_VALUE1 ) ] = { 0 };

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Setup the test response message to receive a small test message. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_SMALL_RESPONSE, HTTPS_TEST_SMALL_RESPONSE_LENGTH );

    /* Test the header buffer receives up to a carriage return in the header separator.  */
    memcpy( &respInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    pUserBufferWithHeaderSpaceUpToColon =
        ( uint8_t * ) IotTest_Malloc( userBufferWithHeaderSpaceUpToColonLength );
    TEST_ASSERT_NOT_NULL( pUserBufferWithHeaderSpaceUpToColon );

    /* Test protect here so that we can free the memory after a failure. */
    if( TEST_PROTECT() )
    {
        respInfo.userBuffer.pBuffer = pUserBufferWithHeaderSpaceUpToColon;
        respInfo.userBuffer.bufferLen = userBufferWithHeaderSpaceUpToColonLength;

        returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respInfo, timeout );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* If the internal pointers were incremented correctly then we should NOT find value1. */
        returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_TEST_HEADER1, FAST_MACRO_STRLEN( HTTPS_TEST_HEADER1 ), pValueBuffer, sizeof( pValueBuffer ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
        /* Assert that "header1:" is found in the header buffer. */
        TEST_ASSERT_NOT_NULL( strstr( ( char * ) ( respHandle->pHeaders ), HTTPS_TEST_HEADER1_PLUS_COLON ) );
        /* Assert that the final character in the header buffer is ':'. */
        TEST_ASSERT_EQUAL( COLON_CHARACTER, ( char ) ( *( respHandle->pHeadersCur - 1 ) ) );
    }

    IotTest_Free( pUserBufferWithHeaderSpaceUpToColon );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we have the correct header data when it ends on the space character of the header field value
 * separator.
 *
 * The header field value separator is ": ". This test is important because it makes sure that the pHeaderCur pointer
 * in the internal response context is updated even though the http-parser callback does not notify the library of
 * these characters.
 *
 * These tests are separated into difference cases so that I can easily free the memory allocated.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithSpaceSeparator )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseInfo_t respInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    uint32_t userBufferWithHeaderSpaceUpToSpaceLength =
        responseUserBufferMinimumSize + sizeof( HTTPS_TEST_SMALL_RESPONSE_UP_TO_SPACE ) - 1;
    uint8_t * pUserBufferWithHeaderSpaceUpToSpace = NULL;
    char pValueBuffer[ sizeof( HTTPS_TEST_HEADER_VALUE1 ) ] = { 0 };

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Setup the test response message to receive a small test message. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_SMALL_RESPONSE, HTTPS_TEST_SMALL_RESPONSE_LENGTH );

    /* Test the header buffer receives up to a carriage return in the header separator.  */
    memcpy( &respInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    pUserBufferWithHeaderSpaceUpToSpace =
        ( uint8_t * ) IotTest_Malloc( userBufferWithHeaderSpaceUpToSpaceLength );
    TEST_ASSERT_NOT_NULL( pUserBufferWithHeaderSpaceUpToSpace );

    /* Test protect here so that we can free the memory after a failure. */
    if( TEST_PROTECT() )
    {
        respInfo.userBuffer.pBuffer = pUserBufferWithHeaderSpaceUpToSpace;
        respInfo.userBuffer.bufferLen = userBufferWithHeaderSpaceUpToSpaceLength;

        returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respInfo, timeout );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* If the internal pointers were incremented correctly then we should NOT find value1. */
        returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_TEST_HEADER1, FAST_MACRO_STRLEN( HTTPS_TEST_HEADER1 ), pValueBuffer, sizeof( pValueBuffer ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
        /* Assert that "header1:" is found in the header buffer. */
        TEST_ASSERT_NOT_NULL( strstr( ( char * ) ( respHandle->pHeaders ), HTTPS_TEST_HEADER1_PLUS_SPACE ) );
        /* Assert that the final string in the header buffer is ": ". */
        TEST_ASSERT_EQUAL( 0,
                           strncmp( HTTPS_HEADER_FIELD_SEPARATOR,
                                    ( char * ) ( respHandle->pHeadersCur - HTTPS_HEADER_FIELD_SEPARATOR_LENGTH ),
                                    HTTPS_HEADER_FIELD_SEPARATOR_LENGTH ) );
    }

    IotTest_Free( pUserBufferWithHeaderSpaceUpToSpace );
}

/*-----------------------------------------------------------*/

/**
 * Test that when the header buffer ends on a space after a header value that this space is treated as parted of
 * the header value.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithSpaceAfterHeaderValue )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseInfo_t respInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;
    uint32_t userBufferWithHeaderSpaceUpToSpaceInBetweenValueLength =
        responseUserBufferMinimumSize + sizeof( HTTPS_TEST_SMALL_RESPONSE_UP_TO_SPACE_IN_BETWEEN_VALUE ) - 1;
    uint8_t * pUserBufferWithHeaderSpaceUpToSpaceInBetweenValue = NULL;
    char pValueBuffer[ sizeof( HTTPS_TEST_HEADER_VALUE2_PLUS_SPACE ) ] = { 0 };

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Setup the test response message to receive a small test message. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_SMALL_RESPONSE, HTTPS_TEST_SMALL_RESPONSE_LENGTH );

    /* Test the header buffer receives up to a carriage return in the header separator.  */
    memcpy( &respInfo, &_respInfo, sizeof( IotHttpsResponseInfo_t ) );
    pUserBufferWithHeaderSpaceUpToSpaceInBetweenValue =
        ( uint8_t * ) IotTest_Malloc( userBufferWithHeaderSpaceUpToSpaceInBetweenValueLength );
    TEST_ASSERT_NOT_NULL( pUserBufferWithHeaderSpaceUpToSpaceInBetweenValue );

    /* Test protect here so that we can free the memory after a failure. */
    if( TEST_PROTECT() )
    {
        respInfo.userBuffer.pBuffer = pUserBufferWithHeaderSpaceUpToSpaceInBetweenValue;
        respInfo.userBuffer.bufferLen = userBufferWithHeaderSpaceUpToSpaceInBetweenValueLength;

        returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respInfo, timeout );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* If the internal pointers were incremented correctly and so we can find the correct "value2 " header value. */
        returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_TEST_HEADER2, FAST_MACRO_STRLEN( HTTPS_TEST_HEADER2 ), pValueBuffer, sizeof( pValueBuffer ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
        /* Assert that ReadHeader() found the correct "value2 " header value. */
        TEST_ASSERT_EQUAL( 0, strncmp( pValueBuffer, HTTPS_TEST_HEADER_VALUE2_PLUS_SPACE, sizeof( HTTPS_TEST_HEADER_VALUE2_PLUS_SPACE ) ) );
    }

    IotTest_Free( pUserBufferWithHeaderSpaceUpToSpaceInBetweenValue );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test receiving a chunked HTTP response message.
 */
TEST( HTTPS_Client_Unit_Sync, SendSyncChunkedResponse )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    uint32_t timeout = HTTPS_TEST_SYNC_TIMEOUT_MS;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    /* Set the global test connection handle to be passed to the library network receive callback. */
    _receiveCallbackConnHandle = connHandle;

    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Setup the test response message to receive the HTTP rest chinked response. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_CHUNKED_RESPONSE, sizeof( HTTPS_TEST_CHUNKED_RESPONSE ) - 1 );
    returnCode = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &_respInfo, timeout );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    _verifyHttpResponseBody( HTTPS_TEST_CHUNKED_RESPONSE_BODY_LENGTH, _respInfo.pSyncInfo->pBody, 0 );
}
