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
 * @file iot_tests_https_client.c
 * @brief Tests for the user-facing API functions declared in iot_https_client.h.
 */

#include <string.h>
#include "iot_tests_https_common.h"

/*-----------------------------------------------------------*/

/**
 * @brief Expected HTTP request line without the method.
 */
#define HTTPS_TEST_REQUEST_LINE_WITHOUT_METHOD                 HTTPS_TEST_PATH " " HTTPS_PROTOCOL_VERSION HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Expected HTTP request line without the path and without the method.
 */
#define HTTPS_TEST_REQUEST_LINE_WITHOUT_PATH_WITHOUT_METHOD    HTTPS_EMPTY_PATH " " HTTPS_PROTOCOL_VERSION HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Expected HTTP User-Agent header line.
 */
#define HTTPS_TEST_USER_AGENT_HEADER_LINE                      HTTPS_USER_AGENT_HEADER HTTPS_HEADER_FIELD_SEPARATOR IOT_HTTPS_USER_AGENT HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Expected HTTP Host header line.
 */
#define HTTPS_TEST_HOST_HEADER_LINE                            HTTPS_HOST_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_TEST_ADDRESS HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief HTTP response header lines to share among the tests.
 */
#define HTTPS_TEST_RESPONSE_HEADER_LINES       \
    "HTTP/1.1 200 OK\r\n"                      \
    "Content-Length: 43\r\n"                   \
    "Date: Sun, 14 Jul 2019 06:07:52 GMT\r\n"  \
    "ETag: \"3356-5233\"\r\n"                  \
    "Vary: *\r\n"                              \
    "P3P: CP=\"This is not a P3P policy\"\r\n" \
    "xserver: www1021\r\n"
#define HTTPS_TEST_RESPONSE_HEADER_LINES_LENGTH    sizeof( HTTPS_TEST_RESPONSE_HEADER_LINES ) - 1 /**< @brief The length of the HTTP response test header lines. */

/**
 * @brief Header lines with no content-length for testing.
 */
#define HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH \
    "HTTP/1.1 200 OK\r\n"                                  \
    "Date: Sun, 14 Jul 2019 06:07:52 GMT\r\n"              \
    "ETag: \"3356-5233\"\r\n"                              \
    "Vary: *\r\n"                                          \
    "P3P: CP=\"This is not a P3P policy\"\r\n"             \
    "xserver: www1021\r\n\r\n"
#define HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH_LENGTH    sizeof( HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH ) - 1 /**< @brief Length of the HTTP test response headers where there is no Content-Length. */

/**
 * Header name and values to verify reading the header.
 */
#define HTTPS_DATE_HEADER                                            "Date"                          /**< @brief "Date" HTTP header field name. */
#define HTTPS_ETAG_HEADER                                            "ETag"                          /**< @brief "ETag" HTTP header field name. */
#define HTTPS_NONEXISTENT_HEADER                                     "Non-Existent-Header"           /**< @brief HTTP header field name of a non-existing header for testing. */
#define HTTPS_DATE_HEADER_VALUE                                      "Sun, 14 Jul 2019 06:07:52 GMT" /**< @brief Test header value for the "Date" header field. */
#define HTTPS_ETAG_HEADER_VALUE                                      "\"3356-5233\""                 /**< @brief Test header value for the "ETag" header field. */
#define HTTPS_CONTENT_LENGTH_VALUE                                   ( ( uint32_t ) 43 )             /**< @brief Test header value as a unsigned integer for the "Content-Length" header field. */

/**
 * The array lengths of local value buffers in the test to store the header values.
 */
#define HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH                  ( 64 ) /**< @brief A large enough test length of a local value buffer to store the returned header value. */
#define HTTPS_TEST_VALUE_BUFFER_LENGTH_TOO_SMALL                     ( 8 )  /**< @brief A too small test length of a local value buffer to store the returned header value. */

/*-----------------------------------------------------------*/

/**
 * @brief A IotHttpsAsyncInfo_t to share among the tests.
 *
 * The tests will replace callbacks in .callbacks as needed and add pPrivData as need for the test.
 */
static IotHttpsAsyncInfo_t _asyncInfo =
{
    .callbacks = { 0 },
    .pPrivData = NULL
};

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
    .isAsync              = true,
    .u.pAsyncInfo         = &_asyncInfo
};

/**
 * @brief A IotHttpsResponseInfo_t for a GET method to share among the tests.
 */
static IotHttpsResponseInfo_t _respInfo =
{
    .userBuffer.pBuffer   = _pRespUserBuffer,
    .userBuffer.bufferLen = sizeof( _pRespUserBuffer ),
    .pSyncInfo            = NULL
};

/*-----------------------------------------------------------*/

/**
 * @brief A network receive callback that fails.
 */
static size_t _networkReceiveFail( void * pConnection,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested )
{
    ( void ) pConnection;
    ( void ) pBuffer;
    ( void ) bytesRequested;

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief A network receive callback that succeeds.
 */
#if 0
    static size_t _networkReceiveSuccess( void * pConnection,
                                          uint8_t * pBuffer,
                                          size_t bytesRequested )
    {
        size_t copyLen = 0;

        ( void ) pConnection;

        if( bytesRequested < HTTPS_TEST_SMALL_RESPONSE_LENGTH )
        {
            copyLen = bytesRequested;
        }
        else
        {
            copyLen = HTTPS_TEST_SMALL_RESPONSE_LENGTH;
        }

        /* Fill the pBuffer with a small message bytesRequested. */
        memcpy( pBuffer, HTTPS_TEST_SMALL_RESPONSE, copyLen );

        return copyLen;
    }
#endif /* if 0 */

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that succeeds.
 */
static size_t _networkSendSuccess( void * pConnection,
                                   const uint8_t * pMessage,
                                   size_t messageLength )
{
    ( void ) pConnection;
    ( void ) pMessage;

    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that fails.
 */
static size_t _networkSendFail( void * pConnection,
                                const uint8_t * pMessage,
                                size_t messageLength )
{
    ( void ) pConnection;
    ( void ) pMessage;
    ( void ) messageLength;

    return 0;
}


/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction close function that fails.
 */
static IotNetworkError_t _networkCloseFail( void * pConnection )
{
    ( void ) pConnection;
    return IOT_NETWORK_FAILURE;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction close function that fails.
 */
static IotNetworkError_t _networkDestroyFail( void * pConnection )
{
    ( void ) pConnection;
    return IOT_NETWORK_FAILURE;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction create function that fails.
 */
static IotNetworkError_t _networkCreateFail( void * pConnectionInfo,
                                             void * pCredentialInfo,
                                             void ** pConnection )
{
    ( void ) pConnectionInfo;
    ( void ) pCredentialInfo;
    ( void ) pConnection;
    return IOT_NETWORK_FAILURE;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction setReceiveCallback that fails.
 */
static IotNetworkError_t _setReceiveCallbackFail( void * pConnection,
                                                  IotNetworkReceiveCallback_t receiveCallback,
                                                  void * pContext )
{
    ( void ) pConnection;
    ( void ) receiveCallback;
    ( void ) pContext;
    return IOT_NETWORK_FAILURE;
}

/*-----------------------------------------------------------*/

/**
 * @brief Mock the http parser execution failing.
 */
static size_t _httpParserExecuteFail( http_parser * parser,
                                      const http_parser_settings * settings,
                                      const char * data,
                                      size_t len )
{
    ( void ) settings;
    ( void ) data;
    ( void ) len;
    parser->http_errno = HPE_UNKNOWN;
    return 0;
}


/*-----------------------------------------------------------*/

/**
 * @brief Test group for HTTPS Client API tests.
 */
TEST_GROUP( HTTPS_Client_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for HTTPS Client API tests.
 */
TEST_SETUP( HTTPS_Client_Unit_API )
{
    /* Initialize the library. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, IotHttpsClient_Init() );

    /* Reset the buffer we receive HTTP body into. */
    ( void ) memset( _pRespBodyBuffer, 0x00, sizeof( _pRespBodyBuffer ) );

    /* Reset the shared response message buffer. */
    ( void ) memset( _pRespMessageBuffer, 0x00, sizeof( _pRespMessageBuffer ) );
    _nextRespMessageBufferByteToReceive = 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for HTTPS Client API tests.
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_API )
{
    /* Clean up the library after the tests. */
    IotHttpsClient_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Client API tests.
 */
TEST_GROUP_RUNNER( HTTPS_Client_Unit_API )
{
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ConnectInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ConnectFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ConnectSuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectSuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, InitializeRequestInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, InitializeRequestFormatCheck );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, AddHeaderInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, AddHeaderFormatCheck );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, AddHeaderMultipleHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadHeaderInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadHeaderVaryingValues );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadContentLengthInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadContentLengthSuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadContentLengthNotFound );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadResponseStatusInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadResponseStatusSuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, WriteRequestBodyInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, WriteRequestBodySuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, WriteRequestBodyNetworkSendFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadResponseBodyInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadResponseBodyNetworkReceiveFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadResponseBodyParsingFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadResponseBodySuccess );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invalid parameters in the @ref https_client_function_connect API.
 */
TEST( HTTPS_Client_Unit_API, ConnectInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionInfo_t testConnInfo = IOT_HTTPS_CONNECTION_INFO_INITIALIZER;
    IotHttpsConnectionInfo_t * pOriginalConnInfo = &_connInfo;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* NULL pConnHandle  */
    returnCode = IotHttpsClient_Connect( NULL, pOriginalConnInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( connHandle );

    /* NULL pConnConfig */
    returnCode = IotHttpsClient_Connect( &connHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( connHandle );

    /* Both pConnHandle and pConnConfig are NULL. */
    returnCode = IotHttpsClient_Connect( NULL, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( connHandle );

    /* IoHttpsConnectInfo_t.userBuffer.pBuffer is NULL. */
    memcpy( &testConnInfo, pOriginalConnInfo, sizeof( IotHttpsConnectionInfo_t ) );
    testConnInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( connHandle );
    /* Restore the testConnInfo for the next test. */
    testConnInfo.userBuffer.pBuffer = pOriginalConnInfo->userBuffer.pBuffer;

    /* IotHttpsConnectionInfo_t.userBuffer.bufferLen < connectionUserBufferMinimumSize */
    testConnInfo.userBuffer.bufferLen = connectionUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    /* Restore the testConnInfo for the next test. */
    testConnInfo.userBuffer.bufferLen = pOriginalConnInfo->userBuffer.bufferLen;

    /* NULL IotHttpsConnectionInfo_t.pAddress in pConnConfig.  */
    testConnInfo.pAddress = NULL;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = pOriginalConnInfo->pAddress;

    /* IotHttpsConnectionInfo_t.addressLen is zero. */
    testConnInfo.addressLen = 0;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* IotHttpsConnectionInfo_t.addressLen is greater than IOT_HTTPS_MAX_HOST_NAME_LENGTH */
    testConnInfo.addressLen = IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Both IotHttpsConnectionInfo_t.addressLen and IotHttpsConnectionInfo_t.pAddress are NULL. */
    testConnInfo.pAddress = NULL;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = pOriginalConnInfo->pAddress;
    testConnInfo.addressLen = pOriginalConnInfo->addressLen;

    /* IotHttpsConnectionInfo_t.alpnProtocolsLen is greater than IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH. */
    testConnInfo.alpnProtocolsLen = IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH + 1;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    testConnInfo.alpnProtocolsLen = pOriginalConnInfo->alpnProtocolsLen;

    /* Unknown IotHttpsConnectionInfo_t.pFlags are ignored, so that is not tested. */

    /* NULL certificates will result in a network error in the underlying network stack, but is allowed in the API so '
     * that is not tested here. */

    /* IotHttpsConnectionInfo_t.pNetworkInterface is NULL. */
    testConnInfo.pNetworkInterface = NULL;
    returnCode = IotHttpsClient_Connect( &connHandle, &testConnInfo );
    TEST_ASSERT_NULL( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
}

/* --------------------------------------------------------- */

/**
 * @brief Test a connection failing in few network abstraction upset scenarios.
 */
TEST( HTTPS_Client_Unit_API, ConnectFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test that we receive an internal error when setReceiveCallback() returns failure. */
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackFail;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    returnCode = IotHttpsClient_Connect( &connHandle, &_connInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INTERNAL_ERROR, returnCode );
    TEST_ASSERT_NULL( connHandle );

    /* Test that we receive a connection error when create() returns failure. */
    _networkInterface.create = _networkCreateFail;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    returnCode = IotHttpsClient_Connect( &connHandle, &_connInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_CONNECTION_ERROR, returnCode );
    TEST_ASSERT_NULL( connHandle );

    /* Test we receive an error if the connection handle is valid and connected but disconnect fails. */

    /* Get a valid connection handle and put a failure condition (a request in the process of sending). */
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    respHandle->reqFinishedSending = false;
    IotDeQueue_EnqueueTail( &( connHandle->respQ ), &( respHandle->link ) );
    returnCode = IotHttpsClient_Connect( &connHandle, &_connInfo );
    TEST_ASSERT_NOT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_NULL( connHandle );
}

/* --------------------------------------------------------- */

/**
 * @brief Test a connection succeeding.
 */
TEST( HTTPS_Client_Unit_API, ConnectSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* Test that we connection successfully, when the connection handle is new. */
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    returnCode = IotHttpsClient_Connect( &connHandle, &_connInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_NOT_NULL( connHandle );

    /* Test that we connect successfully, when the connection handle is is valid and already connected. */
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    returnCode = IotHttpsClient_Connect( &connHandle, &_connInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_NOT_NULL( connHandle );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invalid parameters in the @ref https_client_function_disconnect API.
 */
TEST( HTTPS_Client_Unit_API, DisconnectInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;

    /* NULL connHandle. */
    returnCode = IotHttpsClient_Disconnect( NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we set the connection in the correct state when a disconnect fails.
 */
TEST( HTTPS_Client_Unit_API, DisconnectFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a network close failure. */
    _networkInterface.close = _networkCloseFail;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    returnCode = IotHttpsClient_Disconnect( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    /* The state is disconnected even if the network failed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );

    /* Test a network destroy failure. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroyFail;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    returnCode = IotHttpsClient_Disconnect( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );

    /* Test that IOT_HTTPS_BUSY is returned when a request is in the process of sending. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    respHandle->reqFinishedSending = false;
    IotDeQueue_EnqueueTail( &( connHandle->respQ ), &( respHandle->link ) );
    returnCode = IotHttpsClient_Disconnect( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_BUSY, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we set the connection in the correct state when a disconnect succeeds.
 */
TEST( HTTPS_Client_Unit_API, DisconnectSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a successful disconnect when there are no items in the request queue. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.close = _networkDestroySuccess;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    returnCode = IotHttpsClient_Disconnect( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );

    /* Test a successful disconnect when there is a request in the queue that just finished sending.
     * This case happens if the disconnect called when the network receive callback task is in progress. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.close = _networkDestroySuccess;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    respHandle->reqFinishedSending = true;
    IotDeQueue_EnqueueTail( &( connHandle->respQ ), &( respHandle->link ) );

    returnCode = IotHttpsClient_Disconnect( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test intitializing an HTTP request with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, InitializeRequestInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsRequestInfo_t testReqInfo = IOT_HTTPS_REQUEST_INFO_INITIALIZER;

    /* Test NULL pReqHandle. */
    returnCode = IotHttpsClient_InitializeRequest( NULL, &_reqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );

    /* Test NULL reqInfo. */
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );

    /* Test NULL pReqHandle and NULL reqInfo. */
    returnCode = IotHttpsClient_InitializeRequest( NULL, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );

    /* Test the request context does not fit into the user buffer. */
    memcpy( &testReqInfo, &_reqInfo, sizeof( IotHttpsRequestInfo_t ) );
    testReqInfo.userBuffer.bufferLen = requestUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.userBuffer.bufferLen = _reqInfo.userBuffer.bufferLen;

    /* Test that the first line in the HTTP request message does not fit into the header space of the user buffer. */
    testReqInfo.pathLen = sizeof( _pReqUserBuffer );
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.pathLen = _reqInfo.pathLen;

    /* Test a NULL IotHttpsRequestInfo_t.pHost. */
    testReqInfo.pHost = NULL;
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.pHost = _reqInfo.pHost;

    /* Test that the HTTP Host header does not fit into the request. */
    testReqInfo.hostLen = sizeof( _pReqUserBuffer );
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.hostLen = _reqInfo.hostLen;

    /* Test a NULL request user buffer. */
    testReqInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.userBuffer.pBuffer = _reqInfo.userBuffer.pBuffer;

    /* If IotHttpsRequestInfo_t.isAsync is false, then pSyncInfo must not be NULL. */
    testReqInfo.isAsync = false;
    testReqInfo.u.pSyncInfo = NULL;
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.isAsync = _reqInfo.isAsync;
    testReqInfo.u.pSyncInfo = _reqInfo.u.pSyncInfo;

    /* If IotHttpsRequestInfo_t.isAsync is true, then u.pAsyncInfo must not be NULL. */
    testReqInfo.isAsync = true;
    testReqInfo.u.pAsyncInfo = NULL;
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &testReqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( reqHandle );
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.isAsync = _reqInfo.isAsync;
    testReqInfo.u.pSyncInfo = _reqInfo.u.pSyncInfo;
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that the intialized request is in the standard HTTP format expected in the user buffer header space.
 */
TEST( HTTPS_Client_Unit_API, InitializeRequestFormatCheck )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char * pLocation = NULL;
    int strncmpResult = -1;
    char * pSavedPath = NULL;

    /* Initialize the request using the statically defined configurations. */
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &_reqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Check that the HTTP method is correct at the start of the header buffer space. */
    strncmpResult = strncmp( ( char * ) ( reqHandle->pHeaders ), ( char * ) ( _pHttpsMethodStrings[ _reqInfo.method ] ), strlen( _pHttpsMethodStrings[ _reqInfo.method ] ) );
    TEST_ASSERT_EQUAL( 0, strncmpResult );

    /* Check the request first line in the header buffer space. */
    pLocation = strstr( ( char * ) ( reqHandle->pHeaders ), HTTPS_TEST_REQUEST_LINE_WITHOUT_METHOD );
    TEST_ASSERT_NOT_NULL( pLocation );

    /* Check the User-Agent header line. */
    pLocation = strstr( ( char * ) ( reqHandle->pHeaders ), HTTPS_TEST_USER_AGENT_HEADER_LINE );
    TEST_ASSERT_NOT_NULL( pLocation );

    /* Check the Host header line. */
    pLocation = strstr( ( char * ) ( reqHandle->pHeaders ), HTTPS_TEST_HOST_HEADER_LINE );
    TEST_ASSERT_NOT_NULL( pLocation );

    /* Check that for a NULL IotHttpsRequestInfo_t.pPath, we insert a "/" automatically. */
    pSavedPath = ( char * ) ( _reqInfo.pPath );
    _reqInfo.pPath = NULL;
    returnCode = IotHttpsClient_InitializeRequest( &reqHandle, &_reqInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_NOT_NULL( reqHandle );
    pLocation = strstr( ( char * ) ( reqHandle->pHeaders ), HTTPS_TEST_REQUEST_LINE_WITHOUT_PATH_WITHOUT_METHOD );
    TEST_ASSERT_NOT_NULL( pLocation );
    /* Restore the IotHttpsRequestInfo_t.pPath for other tests. */
    _reqInfo.pPath = pSavedPath;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_AddHeader() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, AddHeaderInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char * pTestName = "Accept";
    char * pTestValue = "text";
    char * pTestContentLengthValueStr = "0";
    uint32_t testValueLen = strlen( pTestValue );
    uint32_t testNameLen = strlen( pTestName );

    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Test parameter reqHandle is NULL. */
    returnCode = IotHttpsClient_AddHeader( NULL, pTestName, testNameLen, pTestValue, testValueLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test parameter pTestName is NULL. */
    returnCode = IotHttpsClient_AddHeader( reqHandle, NULL, testNameLen, pTestValue, testValueLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test parameter pTestValue is NULL. */
    returnCode = IotHttpsClient_AddHeader( reqHandle, pTestName, testNameLen, NULL, testValueLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test all pointer parameters are NULL. */
    returnCode = IotHttpsClient_AddHeader( NULL, NULL, testNameLen, NULL, testValueLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test adding auto-generated headers. */
    returnCode = IotHttpsClient_AddHeader( reqHandle, HTTPS_USER_AGENT_HEADER, FAST_MACRO_STRLEN( HTTPS_USER_AGENT_HEADER ), IOT_HTTPS_USER_AGENT, FAST_MACRO_STRLEN( IOT_HTTPS_USER_AGENT ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    returnCode = IotHttpsClient_AddHeader( reqHandle, HTTPS_HOST_HEADER, FAST_MACRO_STRLEN( HTTPS_HOST_HEADER ), HTTPS_TEST_ADDRESS, FAST_MACRO_STRLEN( HTTPS_TEST_ADDRESS ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    returnCode = IotHttpsClient_AddHeader( reqHandle, HTTPS_CONTENT_LENGTH_HEADER, FAST_MACRO_STRLEN( HTTPS_CONTENT_LENGTH_HEADER ), pTestContentLengthValueStr, strlen( pTestContentLengthValueStr ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    returnCode = IotHttpsClient_AddHeader( reqHandle, HTTPS_CONNECTION_HEADER, FAST_MACRO_STRLEN( HTTPS_CONNECTION_HEADER ), HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE, FAST_MACRO_STRLEN( HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test The length of the resulting header line exceeding the header buffer space. */
    returnCode = IotHttpsClient_AddHeader( reqHandle, pTestName, testNameLen, pTestValue, sizeof( _pReqUserBuffer ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Add a header to the HTTP request and verify the format is correct.
 */
TEST( HTTPS_Client_Unit_API, AddHeaderFormatCheck )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char * pTestName = "Accept";
    char * pTestValue = "text";
    char * pTestHeaderLine = "Accept: text\r\n";
    uint32_t testNameLen = strlen( pTestName );
    uint32_t testValueLen = strlen( pTestValue );
    char * pLocation = NULL;
    uint8_t * headersCurBefore = NULL;

    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    headersCurBefore = reqHandle->pHeadersCur;

    /* Write the test name and value and verify it was written correctly. */
    returnCode = IotHttpsClient_AddHeader( reqHandle, pTestName, testNameLen, pTestValue, testValueLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    pLocation = strstr( ( char * ) ( reqHandle->pHeaders ), pTestHeaderLine );
    TEST_ASSERT_NOT_NULL( pLocation );
    /* Check that the internal headersCur got incremented. */
    TEST_ASSERT_GREATER_THAN( headersCurBefore, reqHandle->pHeadersCur );
}

/*-----------------------------------------------------------*/

/**
 * @brief Add multiple headers to the header buffer space in the request.
 */
TEST( HTTPS_Client_Unit_API, AddHeaderMultipleHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

    /* Instead of iterating in a loop, all the dummy headers are declared immediately because in the unit testing
     * infrastructure and workflow the number of repetitions is typically not changed from the original.  */
    char * pHeader0 = "header0";
    char * pHeader1 = "header1";
    char * pHeader2 = "header2";
    char * pValue0 = "value0";
    char * pValue1 = "value1";
    char * pValue2 = "value2";
    char * pExpectedHeaderLines =
        "header0: value0\r\n"
        "header1: value1\r\n"
        "header2: value2\r\n";
    char * pLocation = NULL;

    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    returnCode = IotHttpsClient_AddHeader( reqHandle, pHeader0, strlen( pHeader0 ), pValue0, strlen( pValue0 ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    returnCode = IotHttpsClient_AddHeader( reqHandle, pHeader1, strlen( pHeader1 ), pValue1, strlen( pValue1 ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    returnCode = IotHttpsClient_AddHeader( reqHandle, pHeader2, strlen( pHeader2 ), pValue2, strlen( pValue2 ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    pLocation = strstr( ( char * ) ( reqHandle->pHeaders ), pExpectedHeaderLines );
    TEST_ASSERT_NOT_NULL( pLocation );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_ReadHeader() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadHeaderInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char valueBuffer[ HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH ] = { 0 };

    /* Get valid respHandle to correctly test other parameters as NULL. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );

    /* Test a NULL response handle. */
    returnCode = IotHttpsClient_ReadHeader( NULL, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBuffer, sizeof( valueBuffer ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL header name parameter. */
    returnCode = IotHttpsClient_ReadHeader( respHandle, NULL, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBuffer, sizeof( valueBuffer ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL header value buffer. */
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), NULL, sizeof( valueBuffer ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test all parameters are NULL. */
    returnCode = IotHttpsClient_ReadHeader( NULL, NULL, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), NULL, sizeof( valueBuffer ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_ReadHeader() with various header search parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadHeaderVaryingValues )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    size_t testHeadersLen = HTTPS_TEST_RESPONSE_HEADER_LINES_LENGTH;
    size_t headersBufferLen = 0;
    size_t copyLen = 0;
    char valueBufferLargeEnough[ HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH ] = { 0 };
    char valueBufferTooSmall[ HTTPS_TEST_VALUE_BUFFER_LENGTH_TOO_SMALL ] = { 0 };
    char * pTestPartialHeadersStart = HTTPS_TEST_RESPONSE_HEADER_LINES;
    char * pTestPartialHeadersEnd = strstr( pTestPartialHeadersStart, HTTPS_DATE_HEADER ) + strlen( HTTPS_DATE_HEADER );
    size_t pTestPartialHeadersLen = ( size_t ) ( pTestPartialHeadersEnd - pTestPartialHeadersStart );

    /* Create a response handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    headersBufferLen = respHandle->pHeadersEnd - respHandle->pHeadersCur;

    /* Fill in with some header data. */
    if( testHeadersLen < headersBufferLen )
    {
        copyLen = testHeadersLen;
    }
    else
    {
        copyLen = headersBufferLen;
    }

    memcpy( respHandle->pHeadersCur, HTTPS_TEST_RESPONSE_HEADER_LINES, copyLen );
    respHandle->pHeadersCur += copyLen;

    /* Test reading some header values successfully. */
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_EQUAL( 0, strncmp( valueBufferLargeEnough, HTTPS_DATE_HEADER_VALUE, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ) ) );
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_ETAG_HEADER, FAST_MACRO_STRLEN( HTTPS_ETAG_HEADER ), valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_EQUAL( 0, strncmp( valueBufferLargeEnough, HTTPS_ETAG_HEADER_VALUE, FAST_MACRO_STRLEN( HTTPS_ETAG_HEADER_VALUE ) ) );

    /* Test reading the header, but the length of the name does not match. */
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ) + 1, valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );

    /* Test reading the header but the value buffer is not large enough. */
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBufferTooSmall, sizeof( valueBufferTooSmall ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_ETAG_HEADER, FAST_MACRO_STRLEN( HTTPS_ETAG_HEADER ), valueBufferTooSmall, sizeof( valueBufferTooSmall ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );

    /* Test reading a header value that does not exist in the header buffer. */
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_NONEXISTENT_HEADER, FAST_MACRO_STRLEN( HTTPS_NONEXISTENT_HEADER ), valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );

    /* Test reading a header value with a failing parseFunc. */
    respHandle->httpParserInfo.parseFunc = _httpParserExecuteFail;
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );

    /* Test looking for a header value when there are no headers available. */
    /* Get a fresh response handle. */
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );

    /* Test reading a header when the value is not available. In this test we have a Date header field, but it ends
     * at that header field. */
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    memcpy( respHandle->pHeadersCur, pTestPartialHeadersStart, pTestPartialHeadersLen );
    respHandle->pHeadersCur += pTestPartialHeadersLen;
    returnCode = IotHttpsClient_ReadHeader( respHandle, HTTPS_DATE_HEADER, FAST_MACRO_STRLEN( HTTPS_DATE_HEADER ), valueBufferLargeEnough, sizeof( valueBufferLargeEnough ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_ReadContentLength() with invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadContentLengthInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t contentLength = 0;

    /* Get valid respHandle to correctly test other parameters as NULL. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );

    /* Test a NULL response handle. */
    returnCode = IotHttpsClient_ReadContentLength( NULL, &contentLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL contentLength return storage parameter. */
    returnCode = IotHttpsClient_ReadContentLength( respHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test that the contentLength is not found on respond handle with an empty header. */
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    returnCode = IotHttpsClient_ReadContentLength( respHandle, &contentLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a successful read of the Content-Length from the HTTP response.
 */
TEST( HTTPS_Client_Unit_API, ReadContentLengthSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t contentLength = 0;
    size_t headersBufferLen = 0;
    size_t copyLen = 0;

    /* Create a response handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    headersBufferLen = respHandle->pHeadersEnd - respHandle->pHeadersCur;

    /* Fill in with some header data. */
    if( HTTPS_TEST_RESPONSE_HEADER_LINES_LENGTH < headersBufferLen )
    {
        copyLen = HTTPS_TEST_RESPONSE_HEADER_LINES_LENGTH;
    }
    else
    {
        copyLen = headersBufferLen;
    }

    memcpy( respHandle->pHeadersCur, HTTPS_TEST_RESPONSE_HEADER_LINES, copyLen );
    respHandle->pHeadersCur += copyLen;

    returnCode = IotHttpsClient_ReadContentLength( respHandle, &contentLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_EQUAL( HTTPS_CONTENT_LENGTH_VALUE, contentLength );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a reading the content length when the header does not exist.
 */
TEST( HTTPS_Client_Unit_API, ReadContentLengthNotFound )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t contentLength = 0;
    size_t headersBufferLen = 0;
    size_t copyLen = 0;

    /* Create a response handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    headersBufferLen = respHandle->pHeadersEnd - respHandle->pHeadersCur;

    /* Fill in with some header data. */
    if( HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH_LENGTH < headersBufferLen )
    {
        copyLen = HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH_LENGTH;
    }
    else
    {
        copyLen = headersBufferLen;
    }

    memcpy( respHandle->pHeadersCur, HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH, copyLen );
    respHandle->pHeadersCur += copyLen;

    returnCode = IotHttpsClient_ReadContentLength( respHandle, &contentLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
    TEST_ASSERT_EQUAL( 0, contentLength );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_ReadResponseStatus() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadResponseStatusInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint16_t responseStatus = 0;

    /* Get valid respHandle to correctly test other parameters as NULL. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );

    /* Test a NULL response handle. */
    returnCode = IotHttpsClient_ReadResponseStatus( NULL, &responseStatus );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL responseStatus return storage parameter. */
    returnCode = IotHttpsClient_ReadResponseStatus( respHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test that the contentLength is not found, when it is equal to zero. */
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    respHandle->status = 0;
    returnCode = IotHttpsClient_ReadResponseStatus( respHandle, &responseStatus );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a successful read of the HTTP response status code from an HTTP response message.
 */
TEST( HTTPS_Client_Unit_API, ReadResponseStatusSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint16_t responseStatus = 0;
    uint16_t testValidResponseStatus = ( uint16_t ) IOT_HTTPS_STATUS_OK;

    /* Test that if the content-length of greater than zero is inside of the structure then it is returned
     * with the API. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    respHandle->status = testValidResponseStatus;
    returnCode = IotHttpsClient_ReadResponseStatus( respHandle, &responseStatus );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_EQUAL( testValidResponseStatus, responseStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_WriteRequestBody() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, WriteRequestBodyInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    int isCompleteSuccess = 1;
    int isCompleteUnsupported = 0;

    /* Get a valid request handle to test other items being with proper coverage. */
    reqHandle = _getReqHandle( &_reqInfo );

    /* Test a NULL request handle parameter. */
    returnCode = IotHttpsClient_WriteRequestBody( NULL, ( uint8_t * ) HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL buf parameter. */
    returnCode = IotHttpsClient_WriteRequestBody( reqHandle, NULL, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test an unsupported isComplete parameter. */
    returnCode = IotHttpsClient_WriteRequestBody( reqHandle, ( uint8_t * ) HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteUnsupported );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_SUPPORTED, returnCode );

    /* Test that for a synchronous request the function fails. */
    _reqInfo.isAsync = false;
    reqHandle = _getReqHandle( &_reqInfo );
    returnCode = IotHttpsClient_WriteRequestBody( reqHandle, ( uint8_t * ) HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    /* Restore the global _reqInfo so other tests can use it. */
    _reqInfo.isAsync = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a successful call to IotHttpsClient_WriteRequestBody().
 */
TEST( HTTPS_Client_Unit_API, WriteRequestBodySuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int isCompleteSuccess = 1;

    /* Simulate a successful write. */
    _networkInterface.send = _networkSendSuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* During the asynchronous workflow the connHandle is associated with the request handle
     * when IotHttpsClient_SendAsync is called. */
    reqHandle->pHttpsConnection = connHandle;

    /* Test a successful call. */
    returnCode = IotHttpsClient_WriteRequestBody( reqHandle, ( uint8_t * ) HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Test that we cannot write twice on the same request handle. */
    returnCode = IotHttpsClient_WriteRequestBody( reqHandle, ( uint8_t * ) HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess );
    TEST_ASSERT_EQUAL( IOT_HTTPS_MESSAGE_FINISHED, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a network send failure in a call to IotHttpsClient_WriteRequestBody().
 */
TEST( HTTPS_Client_Unit_API, WriteRequestBodyNetworkSendFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int isCompleteSuccess = 1;

    /* Simulate a successful write. */
    _networkInterface.send = _networkSendFail;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* During the asynchronous workflow the connHandle is associated with the request handle
     * when IotHttpsClient_SendAsync is called. */
    reqHandle->pHttpsConnection = connHandle;

    /* Test a network send failure on the headers and body. */
    returnCode = IotHttpsClient_WriteRequestBody( reqHandle, ( uint8_t * ) HTTPS_TEST_REQUEST_BODY, HTTPS_TEST_REQUEST_BODY_LENGTH, isCompleteSuccess );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_ReadResponseBody() with NULL parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadResponseBodyInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t bodyLength = sizeof( _pRespBodyBuffer );

    /* Get valid response handle to use for subsequent testing. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );

    /* Test a NULL response handle. */
    returnCode = IotHttpsClient_ReadResponseBody( NULL, _pRespBodyBuffer, &bodyLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL body buffer. */
    returnCode = IotHttpsClient_ReadResponseBody( respHandle, NULL, &bodyLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a NULL length of the body buffer. */
    returnCode = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test that for a synchronous request the function fails. */
    respHandle->isAsync = false;
    returnCode = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, &bodyLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a network error is returned when there is failure to receive on the network.
 */
TEST( HTTPS_Client_Unit_API, ReadResponseBodyNetworkReceiveFailure )
{
    IotHttpsReturnCode_t returnCode;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t bodyLength = sizeof( _pRespBodyBuffer );

    /* Set the network receive function to return a failure. */
    _networkInterface.receiveUpto = _networkReceiveFail;

    /* Get valid response an connection handles to perform this single operation. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    /* The network interface is global and attached to the connHandle. The connHandle
     * is not referenced in the respHandle until IotHttpsClient_SendAsync(). */
    respHandle->pHttpsConnection = connHandle;

    returnCode = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, &bodyLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that there is a parsing error when there is a failure to parse the data received from the
 * network.
 */
TEST( HTTPS_Client_Unit_API, ReadResponseBodyParsingFailure )
{
    IotHttpsReturnCode_t returnCode;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t bodyLength = sizeof( _pRespBodyBuffer );

    /* Set the network receive function to return a succeed. */
    _networkInterface.receiveUpto = _networkReceiveSuccess;

    /* Get valid response and connection handles to perform this single operation. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    /* The network interface is global and attached to the connHandle. The connHandle
     * is not referenced in the respHandle until IotHttpsClient_SendAsync(). */
    respHandle->pHttpsConnection = connHandle;

    /* Replace the parseFunc with one that fails. */
    respHandle->httpParserInfo.parseFunc = _httpParserExecuteFail;

    /* Generate an ideal case header and body message size just for testing a failure to parse. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, &bodyLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test a successful call to IotHttpsClient_ReadResponseBody().
 */
TEST( HTTPS_Client_Unit_API, ReadResponseBodySuccess )
{
    IotHttpsReturnCode_t returnCode;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    uint32_t bodyLength = sizeof( _pRespBodyBuffer );

    /* Set the network receive function to return a succeed. */
    _networkInterface.receiveUpto = _networkReceiveSuccess;

    /* Get valid response and connection handles to perform this single operation. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL( reqHandle );
    respHandle = _getRespHandle( &_respInfo, reqHandle );
    TEST_ASSERT_NOT_NULL( respHandle );
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    /* The network interface is global and attached to the connHandle. The connHandle
     * is not referenced in the respHandle until IotHttpsClient_SendAsync(). */
    respHandle->pHttpsConnection = connHandle;

    /* Generate an ideal case header and body message size just for testing a failure to parse. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, &bodyLength );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
}
