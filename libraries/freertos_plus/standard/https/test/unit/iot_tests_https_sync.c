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

/**
 * @brief The maximum length of the HTTP response message buffer shared among the test.
 * 
 * The buffer of this length is used to test a few scenarios where the headers or body are found in either the header
 * buffer or body buffer and need to be copied over appropriately. 
 */
#define HTTPS_TEST_RESPONSE_MESSAGE_LENGTH              ( 2048 )

/**
 * @brief The generic response status line and header line information for generating a test HTTP response message.
 */
#define HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE         "HTTP/1.1 200 OK\r\n"
#define HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH  ( sizeof(HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE) - 1 )
#define HTTPS_TEST_GENERIC_HEADER                       "header"
#define HTTPS_TEST_GENERIC_VALUE                        "value"
#define HTTPS_TEST_GENERIC_BODY_REPEATED_CHAR           'a'

/**
 * @brief The maximum and minimum digits in a uint32_t. 
 * 
 * This is used to size an array holding some generic headers when generating a test HTTP response message.
 */
#define MAX_UINT32_DIGITS                               ( 10 )  /* 2^32 = 4294967296 which is 10 digits. */
#define MIN_UINT32_DIGITS                               ( 1 )

/**
 * @brief the maximum and minimum line lengths of a generic HTTP repsonse header of the form "headerN: valueN\r\n".
 * 
 * These are needed for sizing arrays holding some intermediate string processing information when generating a test \
 * HTTP response message.
 */
#define MAX_GENERIC_HEADER_LINE_LENGTH \
        sizeof(HTTPS_TEST_GENERIC_HEADER) - 1 \
        + HTTPS_HEADER_FIELD_SEPARATOR_LENGTH \
        + MAX_UINT32_DIGITS \
        + sizeof(HTTPS_TEST_GENERIC_VALUE) - 1 \
        + MAX_UINT32_DIGITS \
        + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH
#define MIN_GENERIC_HEADER_LINE_LENGTH \
        sizeof(HTTPS_TEST_GENERIC_HEADER) - 1 \
        + HTTPS_HEADER_FIELD_SEPARATOR_LENGTH \
        + MIN_UINT32_DIGITS \
        + sizeof(HTTPS_TEST_GENERIC_VALUE) - 1 \
        + MIN_UINT32_DIGITS \
        + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH

        
/**
 * @brief Test HTTP request body to share among the tests.
 */
#define HTTPS_TEST_REQUEST_BODY \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "\
    "magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo"\
    " consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla paria"\
    "tur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est lab"\
    "orum."
#define HTTPS_TEST_REQUEST_BODY_LENGTH              ( sizeof( HTTPS_TEST_REQUEST_BODY ) - 1 )

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

/**
 * @brief An HTTP response message to share among the tests.
 */
static uint8_t _pResponseMessageBuffer[ HTTPS_TEST_RESPONSE_MESSAGE_LENGTH ] = { 0 };

/**
 * @brief The current place in _pResponseMessageBuffer to receive the next byte.
 * 
 * This is used to mimic receiving the HTTP response message from the network during testing.
 */
static uint32_t _nextResponseMessageByteToReceive = 0;

/**
 * @brief A IotHttpsSyncInfo_t for requests and response to share among the tests.
 * 
 * Even though the method is a GET method for the test _reqInfo, we apply a request body for unit testing purposes. A
 * request body is allowed for a GET method according to the HTTP specification, although it is pointless in practice.
 */ 
static IotHttpsSyncInfo_t _syncRequestInfo = {
    .pBody = HTTPS_TEST_REQUEST_BODY,
    /* Constant C string in macros have a NULL terminating character that we want to omit. */
    .bodyLen = HTTPS_TEST_REQUEST_BODY_LENGTH
};
static IotHttpsSyncInfo_t _syncResponseInfo = {
    .pBody = _pRespBodyBuffer,
    /* Constant C string in macros have a NULL terminating character that we want to omit. */
    .bodyLen = sizeof(_pRespBodyBuffer)
};

/**
 * @brief A IotHttpsRequestInfo_t using the PUT method to share among the tests. 
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
    .isAsync = false,
    .pSyncInfo = &_syncRequestInfo
};

/**
 * @brief A IotHttpsResponseInfo_t for a PUT method to share among the tests. 
 * 
 * A PUT request will typically have no response in the body.
 */
static IotHttpsResponseInfo_t _respInfo = {
    .userBuffer.pBuffer = _pRespUserBuffer,
    .userBuffer.bufferLen = sizeof( _pRespUserBuffer ),
    .pSyncInfo = &_syncResponseInfo
};

/**
 * @brief The current size of the response header buffer space.
 */
static const size_t _currentHeaderBufferSize = sizeof(_pRespUserBuffer) - sizeof(_httpsResponse_t);

/*-----------------------------------------------------------*/

/**
 * @brief Generate an test HTTP response message with the specified header length and the specified body length.
 * 
 * This generates the test HTTP response message into _pResponseMessageBuffer. 
 * The provied buffer must be zeroed out before calling this routine.
 * The length of buf must not exceed headerLength + bodyLength. 
 * The length of headerLength must be greater than HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH + MAX_GENERIC_HEADER_LINE_LENGTH + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH.
 * The headerLength is the length of the raw HTTP headers includes delimiters like ": " and \r\n.
 */
static void _generateHttpsResponseMessage( int headerLength, int bodyLength )
{
    /* The content length header is needed so that http-parser will give the body length back to the application 
       correctly during it's callback. */
    char contentLengthHeaderLine[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH] = { 0 };
    int index = 0;
    int nextCopyIndex = 0;
    int currentHeaderLineLength = 0;
    int debugValue0 = HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
    int debugValue1 = HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH;
    int headerSpaceLeft = headerLength - HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH - HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH;
    /* +1 for the NULL terminator for snprintf. This is not included in the final set of headers. */
    char currentHeaderLine[MAX_GENERIC_HEADER_LINE_LENGTH + 1] = { 0 };

    TEST_ASSERT_LESS_THAN_MESSAGE( HTTPS_TEST_RESPONSE_MESSAGE_LENGTH,
        headerLength + bodyLength,
        "Tried to generate a test HTTP response message into a buffer too small.");

    /* We want the headerLength desired to be at least the size of: "HTTP/1.1 200 OK\r\nheader0: value0\r\n\r\n". This
       is in order to generate meaningful test data. */
    TEST_ASSERT_GREATER_THAN_MESSAGE(HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH + MIN_GENERIC_HEADER_LINE_LENGTH + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH, 
        headerLength,
        "Tried to generate a test HTTP response message with specifying too small a desired headerLength.");

    /* Generate the HTTP response status line. */
    memcpy(&(_pResponseMessageBuffer[nextCopyIndex]), HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE, HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH);
    nextCopyIndex += HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH;

    /* Generate the header lines */
    while( headerSpaceLeft > 0 )
    {
        /* currentHeaderLineLength will equal the amount of characters written without the NULL terminator. */
        currentHeaderLineLength = snprintf(currentHeaderLine, 
            sizeof(currentHeaderLine),
            "%s%d%s%s%d%s",
            HTTPS_TEST_GENERIC_HEADER,
            index,
            HTTPS_HEADER_FIELD_SEPARATOR,
            HTTPS_TEST_GENERIC_VALUE,
            index,
            HTTPS_END_OF_HEADER_LINES_INDICATOR);

        /* We need to check if the current header line will fit into the header space left.
           Given that the input headerLength desired MUST be less than the length of "HTTP/1.1 200 OK\r\nheader0: value0\r\n\r\n", we
           are guaranteed to have written "HTTP/1.1 200 OK\r\nheader0: value0\r\n" before we reach this if case. */
        if(headerSpaceLeft < currentHeaderLineLength)
        {
            /* We want to overwite the last header value with filler data to reach our desired headerLength. We do this
               by writing data at the end of the last header value. Which means we need to go back before the last 
               "\r\n" written. */
            nextCopyIndex -= HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
            memset(&(_pResponseMessageBuffer[nextCopyIndex]), '0', headerSpaceLeft);
            nextCopyIndex += headerSpaceLeft;
            memcpy(&(_pResponseMessageBuffer[nextCopyIndex]), HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH);
            nextCopyIndex += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
            headerSpaceLeft = 0;
        }
        else
        {
            memcpy(&(_pResponseMessageBuffer[nextCopyIndex]), currentHeaderLine, currentHeaderLineLength);
            index++;
            nextCopyIndex += currentHeaderLineLength;
            headerSpaceLeft -= currentHeaderLineLength;
        }
    }

    /* Add the last header indicator. */
    memcpy( &(_pResponseMessageBuffer[nextCopyIndex]), HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH);
    nextCopyIndex += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

    /* Generate the body */
    memset( &(_pResponseMessageBuffer[nextCopyIndex]), HTTPS_TEST_GENERIC_BODY_REPEATED_CHAR, bodyLength); 

    /* Set this to mimic reading from the network. */
    _nextResponseMessageByteToReceive = 0;
}

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
 * @brief Mock the http parser execution failing when parsing the HTTP headers buffer.
 */
static size_t _httpParserExecuteFailHeaders( http_parser *parser,
    const http_parser_settings *settings,
    const char *data,
    size_t len )
{
    ( void )settings;
    ( void )data;
    ( void )len;

    if( data == _currentlySendingRequestHandle->pHttpsResponse->pHeadersCur )
    {
        parser->http_errno = HPE_UNKNOWN;
    }
    else
    {
        parser->http_errno = HPE_OK;
    }

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Mock the http parser execution failing when parsing the HTTP body buffer.
 */
static size_t _httpParserExecuteFailBody( http_parser *parser,
    const http_parser_settings *settings,
    const char *data,
    size_t len )
{
    ( void )settings;
    ( void )data;
    ( void )len;

    if( data == _currentlySendingRequestHandle->pHttpsResponse->pBodyCur )
    {
        parser->http_errno = HPE_UNKNOWN;
    }
    else
    {
        http_parser_execute(parser, settings, data, len);
    }

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction receive function that succeeds. 
 */
static size_t _networkReceiveSuccess( void * pConnection,
                                      uint8_t * pBuffer,
                                      size_t bytesRequested )
{
    size_t responseMessageLengthLeft = sizeof(_pResponseMessageBuffer) - _nextResponseMessageByteToReceive;
    size_t copyLen = 0;

    (void)pConnection;
    (void)pBuffer;
    (void)bytesRequested;

    /* Fill with random response data so that this function succeeds if used in a test where the parser must succeed. */
    if( responseMessageLengthLeft < bytesRequested )
    {
        copyLen = responseMessageLengthLeft;
    }
    else
    {
        copyLen = bytesRequested;
    }
    memcpy(pBuffer, &(_pResponseMessageBuffer[_nextResponseMessageByteToReceive]), copyLen);
    _nextResponseMessageByteToReceive += copyLen;

    return bytesRequested;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that fails sending the HTTP headers. 
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
    (void)pConnection;
    (void)pMessage;
    (void)messageLength;
    
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
 * @brief Network abstraction receive function that timesout when sending the HTTP headers. 
 */
static size_t _networkReceiveFailHeaders( void * pConnection,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested )
{
    (void)pConnection;
    (void)pBuffer;
    (void)bytesRequested;

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
 * @brief Network abstraction receive function that timesout when sending the HTTP body. 
 */
static size_t _networkReceiveFailBody( void * pConnection,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested )
{
    (void)pConnection;
    (void)pBuffer;
    (void)bytesRequested;

    size_t retValue = 0;

    /* We may have recieved some body in the headers, so pBodyCur will be incremented during the parsing of 
       data received when receiving into the header buffer. Given this case, when receiving from the network 
       into the body buffer, start of the pBuffer should always be pBodyCur. */
    if( pBuffer == _currentlySendingRequestHandle->pHttpsResponse->pBodyCur )
    {
        retValue = 0;
    }
    else
    {
        retValue = _networkReceiveSuccess(pConnection, pBuffer, bytesRequested);
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
    (void)pConnection;
    (void)pMessage;

    /* Set the response parser function to mock a failure. */
    _currentlySendingRequestHandle->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFailHeaders;

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
 * @brief Network send success that replaces the _httpsResponse_t.httpParserInfo.parseFunc with one from this test.
 * 
 * This function relies on _httpsResponse_t.pHttpsResponse getting set, in the currently processing _httpsResponse_t,
 * before the network send function is called.
 */
static size_t _networkSendSuccessWithSettingParseFailForBody( void * pConnection,
                                                       const uint8_t * pMessage,
                                                       size_t messageLength )
{
    (void)pConnection;
    (void)pMessage;

    /* Set the response parser function to mock a failure. */
    _currentlySendingRequestHandle->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFailBody;

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
    /* Reset the shared response message buffer. */
    ( void ) memset( &_pResponseMessageBuffer, 0x00, sizeof(_pResponseMessageBuffer) );

    /* Reset some global static variables. */
    _receiveCallbackConnHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    _alreadyCreatedReceiveCallbackThread = false;
    _currentlySendingRequestHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    _nextResponseMessageByteToReceive = 0;

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
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureSendingBody )
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureReceivingBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureParsingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncFailureParsingBody );
    // TODO: Without a content-length header we may never reach the onMessageCompleteCallback() so parsing state PARSER_STATE_BODY_COMPLETE may never be assigned 
    // and we will always return IOT_HTTPS_MESSAGE_TOO_LARGE.
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncSomeBodyInHeaderBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncSomeHeaderInBodyBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncEntireResponseInHeaderBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncBodyTooLarge );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncBodyBufferNull );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncPersistentRequest );
    RUN_TEST_CASE( HTTPS_Client_Unit_Sync, SendSyncNonPersistentRequest );
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
    reqHandle = _getReqHandle( &_reqInfo );
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

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL(reqHandle);

    _currentlySendingRequestHandle = reqHandle;

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
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

    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL(reqHandle);

    _currentlySendingRequestHandle = reqHandle;

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
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
    _networkInterface.receive = _networkReceiveFailHeaders;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL(reqHandle);
    _currentlySendingRequestHandle = reqHandle;

    /* Set the global test connection handle to be passed to the library network receive callback. */
    memcpy(&_receiveCallbackConnHandle, &connHandle, sizeof(IotHttpsConnectionHandle_t));

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
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
    _networkInterface.receive = _networkReceiveFailBody;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL(reqHandle);
    _currentlySendingRequestHandle = reqHandle;

    /* Set the global test connection handle to be passed to the library network receive callback. */
    memcpy(&_receiveCallbackConnHandle, &connHandle, sizeof(IotHttpsConnectionHandle_t));

    /* Generate an ideal case header and body message size just for testing a failure to receive. */
    _generateHttpsResponseMessage( _currentHeaderBufferSize, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );
    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
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
    _networkInterface.receive = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL(reqHandle);
    _currentlySendingRequestHandle = reqHandle;

    /* Set the global test connection handle to be passed to the library network receive callback. */
    memcpy(&_receiveCallbackConnHandle, &connHandle, sizeof(IotHttpsConnectionHandle_t));

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_PARSING_ERROR, returnCode);
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
    _alreadyCreatedReceiveCallbackThread = false;
    _networkInterface.send = _networkSendSuccessWithSettingParseFailForBody;
    _networkInterface.receive = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    /* Get a valid "connected" handled. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    
    /* Get a valid request handle. */
    reqHandle = _getReqHandle( &_reqInfo );
    TEST_ASSERT_NOT_NULL(reqHandle);
    _currentlySendingRequestHandle = reqHandle;

    /* Set the global test connection handle to be passed to the library network receive callback. */
    memcpy(&_receiveCallbackConnHandle, &connHandle, sizeof(IotHttpsConnectionHandle_t));

    /* Generate an ideal case header and body message size just for testing a failure to receive. */
    _generateHttpsResponseMessage( _currentHeaderBufferSize, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendSync(connHandle, reqHandle, &respHandle, &_respInfo, timeout);
    TEST_ASSERT_EQUAL(IOT_HTTPS_PARSING_ERROR, returnCode);
}