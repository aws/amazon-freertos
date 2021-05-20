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
 * @file iot_tests_https_async.c
 * @brief Tests for IotHttpsClient_SendAsync().
 * in iot_https_client.h.
 */

#include "iot_tests_https_common.h"
#include "platform/iot_clock.h"

/**
 * @brief Timeout for all requests to finish on a single connection.
 */
#define HTTPS_TEST_ASYNC_TIMEOUT_MS                    ( ( uint32_t ) 30000 )

/**
 * @brief The maximum number of asynchronous requests on the same connection.
 *
 * This is used to separate the user buffers needed for each request.
 */
#define HTTPS_TEST_MAX_ASYNC_REQUESTS                  ( 3 )

/**
 * @brief Wait time before the network receive callback is invoked.
 *
 * This wait time is to mimic not only response being received on the network, but also needs to incorporate the time
 * it takes to complete sending the request.
 */
#define HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS    ( ( uint32_t ) 300 )

/*-----------------------------------------------------------*/

/**
 * @brief Context to the HTTP asynchronous workflow callbacks to verify operation.
 */
typedef struct _asyncVerificationParams
{
    IotSemaphore_t completeSem;                                       /**< @brief Semaphore to signal to the test that the asynchronous requests have all finished. */
    uint8_t numRequestsTotal;                                         /**< @brief The starting total of scheduled request. */
    int8_t numRequestsLeft;                                           /**< @brief The number of scheduled requests left that have not finished. */
    uint8_t appendHeaderCallbackCount;                                /**< @brief A count of the times #IotHttpsClientCallbacks_t.appendHeaderCallback has been called. */
    uint8_t writeCallbackCount;                                       /**< @brief A count of the times #IotHttpsClientCallbacks_t.writeCallback has been called. */
    uint8_t readReadyCallbackCount;                                   /**< @brief A count of the times #IotHttpsClientCallbacks_t.readReadyCallback has been called. */
    uint8_t responseCompleteCallbackCount;                            /**< @brief A count of the times #IotHttpsClientCallbacks_t.responseCompleteCallback has been called. */
    uint8_t connectionClosedCallbackCount;                            /**< @brief A count of the times #IotHttpsClientCallbacks_t.connectionClosedCallback has been called. */
    uint8_t errorCallbackCount;                                       /**< @brief A count of the times #IotHttpsClientCallbacks_t.errorCallback has been called. */
    IotHttpsReturnCode_t returnCode[ HTTPS_TEST_MAX_ASYNC_REQUESTS ]; /**< @brief The final return status of the async request. This is set during each decrement of numRequestsLeft. */

    /**
     * @brief readReadyCallback invocation count per response.
     *
     * This is needed for verification of the body. The readReadyCallback is invoked a second time for the same
     * response because there is more data in the body that could fit in the provided buffer. In this case we want to
     * verify the correct body of 'a'-'z' repeating starting from where verification left off last. When it is a new
     * request verification starts at 0 or 'a'.
     */
    uint8_t readReadyCallbackCountPerResponse[ HTTPS_TEST_MAX_ASYNC_REQUESTS ];
} _asyncVerificationParams_t;

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous workflow callback context to share among the tests.
 *
 * This is reset during TEST_SETUP.
 */
static _asyncVerificationParams_t _verifParams = { 0 };

/**
 * HTTPS response and request user buffers for scheduling multiple requests.
 */
static uint8_t _pAsyncRespUserBuffers[ HTTPS_TEST_MAX_ASYNC_REQUESTS ][ HTTPS_TEST_RESP_USER_BUFFER_SIZE ] = { 0 }; /**< @brief HTTPS response user buffers for scheduling multiple requests. */
static uint8_t _pAsyncReqUserBuffers[ HTTPS_TEST_MAX_ASYNC_REQUESTS ][ HTTPS_TEST_REQ_USER_BUFFER_SIZE ] = { 0 };   /**< @brief HTTPS request user buffers for scheduling multiple requests. */

/**
 * HTTPS request and response information configrations for scheduling multiple requests.
 */
static IotHttpsRequestInfo_t _pAsyncReqInfos[ HTTPS_TEST_MAX_ASYNC_REQUESTS ] = { 0 };   /**< @brief HTTPS request information configuration for scheduling multiple requests. */
static IotHttpsResponseInfo_t _pAsyncRespInfos[ HTTPS_TEST_MAX_ASYNC_REQUESTS ] = { 0 }; /**< @brief HTTPS response information configuration for scheduling multiple requests. */

/**
 * HTTPS request and response handles for scheduling multiple requests.
 *
 * Each of these handles corresponse to each of the user buffers _pAsyncRespUserBuffers and _pAsyncReqUserBuffers.
 */
static IotHttpsRequestHandle_t _pAsyncRequestHandles[ HTTPS_TEST_MAX_ASYNC_REQUESTS ];   /**< @brief Request handles for scheduling multiple requests. */
static IotHttpsResponseHandle_t _pAsyncResponseHandles[ HTTPS_TEST_MAX_ASYNC_REQUESTS ]; /**< @brief Response handles for scheduling multiple requests. */

/**
 * @brief A base IotHttpsAsyncInfo_t to copy to each of the request information configurations for each request.
 *
 * The tests will replace callbacks in .callbacks as needed and add pPrivData as need for the test.
 */
static IotHttpsAsyncInfo_t _asyncInfoBase =
{
    .callbacks = { 0 }, /* To be updated as needed during testing. */
    .pPrivData = NULL   /* To be updated as needed during testing. */
};

/**
 * @brief A base IotHttpsRequestInfo_t to copy to each of the request information configurations for each request.
 */
static IotHttpsRequestInfo_t _reqInfoBase =
{
    .pPath                = HTTPS_TEST_PATH,
    .pathLen              = sizeof( HTTPS_TEST_PATH ) - 1,
    .method               = IOT_HTTPS_METHOD_GET,
    .pHost                = HTTPS_TEST_ADDRESS,
    .hostLen              = sizeof( HTTPS_TEST_ADDRESS ) - 1,
    .isNonPersistent      = false,
    .userBuffer.pBuffer   = NULL, /* To be updated during TEST_SETUP. */
    .userBuffer.bufferLen = 0,    /* To be updated during TEST_SETUP. */
    .isAsync              = true,
    .u.pAsyncInfo         = &_asyncInfoBase
};

/**
 * @brief A IotHttpsResponseInfo_t for an asynchronous response.
 */
static IotHttpsResponseInfo_t _respInfoBase =
{
    .userBuffer.pBuffer   = NULL, /* To be updated during TEST_SETUP */
    .userBuffer.bufferLen = 0,    /* To be updated during TEST_SETUP */
    .pSyncInfo            = NULL
};

/*-----------------------------------------------------------*/

/**
 * @brief thread that invokes the _networkReceiveCallback internal to the library.
 */
static void _invokeNetworkReceiveCallback( void * pArgument )
{
    void * pNetworkConnection = pArgument;
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pNetworkConnection;
    size_t responseMessageLengthLeft = 0;

    /* The response associated with the request is set in the network interface context now. This is so that we
     * can simulate a failure on receiving depending on the buffer received to in the response. */
    pHttpsRequest->pHttpsConnection->pNetworkConnection = ( void * ) ( pHttpsRequest->pHttpsResponse );

    /* Start over next bytes to receive in the _respMessageBuffer. It is an error in the libray if the full response
     * was not read from the network before the next response processing is to read from the network. When the
     * connection closes there should be no data on the socket. */
    responseMessageLengthLeft = strlen( ( char * ) _pRespMessageBuffer ) - _nextRespMessageBufferByteToReceive;

    /* If the next byte to receive is not zero, it must be the end of the buffer to ensure the full response was read
     * from the network in the last response processing. */
    if( ( _nextRespMessageBufferByteToReceive > 0 ) && ( responseMessageLengthLeft > 0 ) )
    {
        TEST_FAIL_MESSAGE( "The full response message was not read/flushed in the last request." );
    }

    _nextRespMessageBufferByteToReceive = 0;

    /* Sleep for a bit to wait for the rest of test request to finished sending and simulate a network response. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS );

    /* Envoke the network receive callback. */
    IotTestHttps_networkReceiveCallback( pNetworkConnection, pHttpsRequest->pHttpsConnection );
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
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pConnection;

    /* A closed connection will return an error when trying to send. */
    if( pHttpsRequest->pHttpsConnection->isConnected == false )
    {
        return 0;
    }

    /* This thread must be created only once per request to mimic the behavior of the network abstraction. In the
     * HTTPS Client library network sending is called multiple times, but it is called a final time when sending the
     * body. In this test group all implementation of _writeCallback must send some dummy body. */
    if( pHttpsRequest->pBody == pMessage )
    {
        Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                  pConnection,
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE );
    }

    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction send function that fails sending the HTTP headers.
 */
static size_t _networkSendFailHeaders( void * pConnection,
                                       const uint8_t * pMessage,
                                       size_t messageLength )
{
    size_t retValue = 0;

    /* The currently sending request is to be set during the test implemented
     #IotHttpsClientCallbacks_t.appendHeaderCallback(). */
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pConnection;

    /* Check if we are sending the headers to return failure. */
    if( pHttpsRequest->pHeaders == pMessage )
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
    size_t retValue = 0;

    /* The currently sending request is to be set during the test implemented
     #IotHttpsClientCallbacks_t.appendHeaderCallback(). */
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pConnection;

    /* Check if we are sending the headers to return failure. */
    if( pHttpsRequest->pBody == pMessage )
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
 * @brief Network abstraction receive function that fails when sending the HTTP headers.
 */
static size_t _networkReceiveFailHeaders( void * pConnection,
                                          uint8_t * pBuffer,
                                          size_t bytesRequested )
{
    size_t retValue = 0;

    /* The currently receiving response is to be set during the mocked network receive callback
     * _invokeNetworkReceiveCallback(). */
    _httpsResponse_t * pHttpsResponse = ( _httpsResponse_t * ) pConnection;

    if( pBuffer == pHttpsResponse->pHeadersCur )
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
    size_t retValue = 0;

    /* The currently sending response is to be set during the mocked network receive callback
     * _invokeNetworkReceiveCallback(). */
    _httpsResponse_t * pHttpsResponse = ( _httpsResponse_t * ) pConnection;

    /* We may have received some body in the headers, so pBodyCur will be incremented during the parsing of
     * data received when receiving into the header buffer. Given this case, when receiving from the network
     * into the body buffer, start of the pBuffer should always be pBodyCur. */
    if( pBuffer == pHttpsResponse->pBodyCur )
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
 * @brief Mock the http parser execution failing when parsing the HTTP headers buffer.
 */
static size_t _httpParserExecuteFailHeaders( http_parser * parser,
                                             const http_parser_settings * settings,
                                             const char * data,
                                             size_t len )
{
    _httpsResponse_t * pHttpsResponse = ( _httpsResponse_t * ) ( parser->data );

    ( void ) settings;
    ( void ) len;

    if( data == ( char * ) ( pHttpsResponse->pHeadersCur ) )
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
    _httpsResponse_t * pHttpsResponse = ( _httpsResponse_t * ) ( parser->data );

    ( void ) settings;
    ( void ) len;

    if( data == ( char * ) ( pHttpsResponse->pBodyCur ) )
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
 * @brief A network send function that fails on the second call to receive the headers.
 */
size_t _networkSendFailsOnSecondHeaderSend( void * pConnection,
                                            const uint8_t * pMessage,
                                            size_t messageLength )
{
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pConnection;
    static int headerSendCount = 0;
    size_t returnValue = messageLength;

    if( pHttpsRequest->pHeaders == pMessage )
    {
        headerSendCount++;
    }

    if( headerSendCount == 2 )
    {
        returnValue = 0;
    }
    else
    {
        /* This thread must be created only once per request to mimic the behavior of the network abstraction. In the
         * HTTPS Client library network sending is called multiple times, but it is called a final time when sending the
         * body. In this test group all implementation of _writeCallback must send some dummy body. */
        if( pHttpsRequest->pBody == pMessage )
        {
            Iot_CreateDetachedThread( _invokeNetworkReceiveCallback,
                                      pConnection,
                                      IOT_THREAD_DEFAULT_PRIORITY,
                                      IOT_THREAD_DEFAULT_STACK_SIZE );
        }
    }

    return returnValue;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network send success that replaces the _httpsResponse_t.httpParserInfo.parseFunc with one for the current test.
 *
 * This function relies on _httpsResponse_t.pHttpsResponse getting set, in the currently processing _httpsResponse_t,
 * before the network send function is called.
 */
static size_t _networkSendSuccessWithSettingParseFailForHeaders( void * pConnection,
                                                                 const uint8_t * pMessage,
                                                                 size_t messageLength )
{
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pConnection;

    /* Set the response parser function to mock a failure. */
    pHttpsRequest->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFailHeaders;

    return _networkSendSuccess( pConnection, pMessage, messageLength );
}

/*-----------------------------------------------------------*/

/**
 * @brief Network send success that replaces the _httpsResponse_t.httpParserInfo.parseFunc with one for the current test.
 *
 * This function relies on _httpsResponse_t.pHttpsResponse getting set, in the currently processing _httpsResponse_t,
 * before the network send function is called.
 */
static size_t _networkSendSuccessWithSettingParseFailForBody( void * pConnection,
                                                              const uint8_t * pMessage,
                                                              size_t messageLength )
{
    _httpsRequest_t * pHttpsRequest = ( _httpsRequest_t * ) pConnection;

    /* Set the response parser function to mock a failure. */
    pHttpsRequest->pHttpsResponse->httpParserInfo.parseFunc = _httpParserExecuteFailBody;

    return _networkSendSuccess( pConnection, pMessage, messageLength );
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.appendHeaderCallback implementation to share among the tests.
 */
static void _appendHeaderCallback( void * pPrivData,
                                   IotHttpsRequestHandle_t reqHandle )
{
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;

    /* Set the currently sending request as the pConnection to use in the mocked networkSend functions.
     * This is needed to check which buffer to being filled, so that we can mock a network failure on the buffer
     * of interest (either the header buffer or the body buffer). */
    reqHandle->pHttpsConnection->pNetworkConnection = ( void * ) reqHandle;

    verifParams->appendHeaderCallbackCount++;
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.writeCallback implementation to share among the tests.
 */
static void _writeCallback( void * pPrivData,
                            IotHttpsRequestHandle_t reqHandle )
{
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;

    /* Write a dummy body in reqHandle, so that the _networkSendSuccess mock function knows to create a single
     * network receive callback mocking thread for the current request. */
    IotHttpsClient_WriteRequestBody( reqHandle, ( uint8_t * ) ( HTTPS_TEST_REQUEST_BODY ), HTTPS_TEST_REQUEST_BODY_LENGTH, 1 );

    verifParams->writeCallbackCount++;
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.readReadyCallback implementation to share among the tests.
 */
static void _readReadyCallback( void * pPrivData,
                                IotHttpsResponseHandle_t respHandle,
                                IotHttpsReturnCode_t rc,
                                uint16_t status )
{
    /* Disable unused parameter warning. */
    ( void ) rc;
    ( void ) status;

    IotHttpsReturnCode_t returnCode;
    uint32_t bodyLen = HTTPS_TEST_RESP_BODY_BUFFER_SIZE;
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;
    int currentRequestIndex = verifParams->numRequestsTotal - verifParams->numRequestsLeft;

    /* Reset the buffer we receive HTTP body into. */
    ( void ) memset( _pRespBodyBuffer, 0x00, sizeof( _pRespBodyBuffer ) );

    /* Read from the network the data in the body buffer. */
    returnCode = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, &bodyLen );

    /* Verify the body for the current test. */
    if( returnCode == IOT_HTTPS_OK )
    {
        _verifyHttpResponseBody( bodyLen,
                                 _pRespBodyBuffer,
                                 ( verifParams->readReadyCallbackCountPerResponse[ currentRequestIndex ] * HTTPS_TEST_RESP_BODY_BUFFER_SIZE ) );
    }

    verifParams->readReadyCallbackCountPerResponse[ currentRequestIndex ]++;
    verifParams->readReadyCallbackCount++;
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.responseCompleteCallback implementation to share among the tests.
 */
static void _responseCompleteCallback( void * pPrivData,
                                       IotHttpsResponseHandle_t respHandle,
                                       IotHttpsReturnCode_t rc,
                                       uint16_t status )
{
    /* Disable unused parameter warning. */
    ( void ) status;

    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;

    verifParams->responseCompleteCallbackCount++;
    verifParams->returnCode[ verifParams->numRequestsTotal - verifParams->numRequestsLeft ] = rc;
    verifParams->numRequestsLeft--;

    /* If this asserts while another test is going on in the main thread, then that is an indicator that not all of the
     * requests exited properly. */
    if( TEST_PROTECT() )
    {
        TEST_ASSERT_TRUE_MESSAGE( verifParams->numRequestsLeft >= 0, "verifParams->numRequestsLeft < 0, something went wrong. _responseCompleteCallback should only be called once per request." );
    }

    /* If this is the last request to complete, then post to the semaphore to let the test know everything is done. */
    if( ( verifParams->numRequestsLeft == 0 ) ||
        ( rc == IOT_HTTPS_NETWORK_ERROR ) ||
        ( rc == IOT_HTTPS_PARSING_ERROR ) ||
        ( ( respHandle != NULL ) && ( respHandle->isNonPersistent ) ) )
    {
        IotSemaphore_Post( &( verifParams->completeSem ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.connectionClosedCallback implementation to share among the tests.
 */
static void _connectionClosedCallback( void * pPrivData,
                                       IotHttpsConnectionHandle_t connHandle,
                                       IotHttpsReturnCode_t rc )
{
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;

    /* Disable unused parameter warnings. */
    ( void ) rc;
    ( void ) connHandle;

    verifParams->connectionClosedCallbackCount++;
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.errorCallback implementation to share among the tests.
 */
static void _errorCallback( void * pPrivData,
                            IotHttpsRequestHandle_t reqHandle,
                            IotHttpsResponseHandle_t respHandle,
                            IotHttpsReturnCode_t rc )
{
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;

    /* Disable unused parameter warnings. */
    ( void ) reqHandle;
    ( void ) respHandle;
    ( void ) rc;

    verifParams->errorCallbackCount++;
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.appendHeaderCallback implementation that cancels the request.
 */
static void _appendHeaderCallbackThatCancels( void * pPrivData,
                                              IotHttpsRequestHandle_t reqHandle )
{
    IotHttpsClient_CancelRequestAsync( reqHandle );
    _appendHeaderCallback( pPrivData, reqHandle );
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.writeCallback implementation that cancels the request.
 */
static void _writeCallbackThatCancels( void * pPrivData,
                                       IotHttpsRequestHandle_t reqHandle )
{
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;


    IotHttpsClient_CancelRequestAsync( reqHandle );
    verifParams->writeCallbackCount++;
}

/*-----------------------------------------------------------*/

/**
 * @brief Asynchronous #IotHttpsClientCallbacks_t.readReadyCallback implementation that cancels the request.
 */
static void _readReadyCallbackThatCancels( void * pPrivData,
                                           IotHttpsResponseHandle_t respHandle,
                                           IotHttpsReturnCode_t rc,
                                           uint16_t status )
{
    _asyncVerificationParams_t * verifParams = ( _asyncVerificationParams_t * ) pPrivData;

    /* Disable unused parameter warnings. */
    ( void ) rc;
    ( void ) status;

    IotHttpsClient_CancelResponseAsync( respHandle );
    verifParams->readReadyCallbackCount++;
}

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
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, IotHttpsClient_Init() );

    memset( &_verifParams, 0, sizeof( _asyncVerificationParams_t ) );

    int reqIndex = 0;

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        memcpy( &_pAsyncReqInfos[ reqIndex ], &_reqInfoBase, sizeof( IotHttpsRequestInfo_t ) );
        _pAsyncReqInfos[ reqIndex ].userBuffer.pBuffer = _pAsyncReqUserBuffers[ reqIndex ];
        _pAsyncReqInfos[ reqIndex ].userBuffer.bufferLen = HTTPS_TEST_REQ_USER_BUFFER_SIZE;
        memcpy( &_pAsyncRespInfos[ reqIndex ], &_respInfoBase, sizeof( IotHttpsResponseInfo_t ) );
        _pAsyncRespInfos[ reqIndex ].userBuffer.pBuffer = _pAsyncRespUserBuffers[ reqIndex ];
        _pAsyncRespInfos[ reqIndex ].userBuffer.bufferLen = HTTPS_TEST_RESP_USER_BUFFER_SIZE;
        _verifParams.returnCode[ reqIndex ] = IOT_HTTPS_OK;
    }

    TEST_ASSERT_TRUE( IotSemaphore_Create( &( _verifParams.completeSem ), 0, 1 ) );

    /* All of the tests use the same IotHttpsClientCallbacks_t instantiation. */
    _asyncInfoBase.callbacks.appendHeaderCallback = _appendHeaderCallback;
    _asyncInfoBase.callbacks.connectionClosedCallback = _connectionClosedCallback;
    _asyncInfoBase.callbacks.errorCallback = _errorCallback;
    _asyncInfoBase.callbacks.readReadyCallback = _readReadyCallback;
    _asyncInfoBase.callbacks.responseCompleteCallback = _responseCompleteCallback;
    _asyncInfoBase.callbacks.writeCallback = _writeCallback;
    _asyncInfoBase.pPrivData = &_verifParams;

    /* Reset the variables that mimic the network. */
    ( void ) memset( &_networkInterface, 0x00, sizeof( IotNetworkInterface_t ) );
    ( void ) memset( _pRespMessageBuffer, 0x00, sizeof( _pRespMessageBuffer ) );
    _nextRespMessageBufferByteToReceive = 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test teardown for the HTTP Client Async unit tests.
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_Async )
{
    IotSemaphore_Destroy( &( _verifParams.completeSem ) );
    IotHttpsClient_Cleanup();
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Client function @ref https_client_function_sendasync
 */
TEST_GROUP_RUNNER( HTTPS_Client_Unit_Async )
{
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncFailureSendingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncFailureSendingBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncFailureReceivingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncFailureReceivingBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncFailureParsingHeaders );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncFailureParsingBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncSomeBodyInHeaderBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncSomeHeaderInBodyBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncEntireResponseInHeaderBuffer );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncBodyTooLarge );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncIgnoreResponseBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncCancelBeforeScheduled );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncCancelDuringAppendHeaderCallback );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncCancelDuringWriteCallback );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncCancelDuringReadReadyCallback );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsSuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsSecondHasNetworkSendFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstHasNetworkReceiveFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstHasParsingFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstIsNonPersistent );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstIgnoresPresentResponseBody );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsOneGetsCancelled );
    RUN_TEST_CASE( HTTPS_Client_Unit_Async, SendAsyncChunkedResponse );
}

/*-----------------------------------------------------------*/

/**
 * Tests IotHttpsClient_SendAsync() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsResponseInfo_t testRespInfo = IOT_HTTPS_RESPONSE_INFO_INITIALIZER;

    /* Get valid connection and request handles for testing one input NULL at a time. */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &_pAsyncReqInfos[ 0 ] );
    TEST_ASSERT_NOT_NULL( reqHandle );

    memcpy( &testRespInfo, &_pAsyncRespInfos[ 0 ], sizeof( IotHttpsResponseInfo_t ) );

    /* Test a NULL connHandle parameter. */
    returnCode = IotHttpsClient_SendAsync( NULL, reqHandle, &respHandle, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a NULL reqHandle parameters. */
    returnCode = IotHttpsClient_SendAsync( connHandle, NULL, &respHandle, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a NULL respHandle parameter. */
    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, NULL, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a aNULL pRespInfo parameter. */
    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a sync request handle. */
    reqHandle->isAsync = false;
    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );
    /* Restore the request handle for other tests. */
    reqHandle->isAsync = true;

    /* Test a NULL response user buffer. */
    memcpy( &testRespInfo, &_pAsyncRespInfos[ 0 ], sizeof( IotHttpsResponseInfo_t ) );
    testRespInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test a response user buffer that is too small. */
    memcpy( &testRespInfo, &_pAsyncRespInfos[ 0 ], sizeof( IotHttpsResponseInfo_t ) );
    testRespInfo.userBuffer.bufferLen = responseUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode );
    TEST_ASSERT_NULL( respHandle );

    /* Test sending a request on a close connection. */
    memcpy( &testRespInfo, &_pAsyncRespInfos[ 0 ], sizeof( IotHttpsResponseInfo_t ) );
    connHandle->isConnected = false;
    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &testRespInfo );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );
    TEST_ASSERT_NULL( respHandle );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the correct asynchronous workflow when the headers fails to send on the network.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncFailureSendingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a failure to send the headers. */
    _networkInterface.send = _networkSendFailHeaders;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );

    /* Verify that the network is disconnected. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the correct asynchronous workflow when the body fails to send on the network.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncFailureSendingBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a failure to send the body. */
    _networkInterface.send = _networkSendFailBody;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );

    /* Verify that the network is disconnected. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the correct asynchronous workflow when the headers fail to be received on the network.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncFailureReceivingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a failure to receive the headers. */
    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveFailHeaders;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );

    /* Verify that the network is disconnected. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/**
 * @brief Verify the correct asynchronous workflow when the body fails to be received on the network.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncFailureReceivingBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a failure to receive the headers. */
    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveFailBody;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case header and body message size just for testing a failure to receive. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );

    /* Verify that the network is disconnected. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the correct asynchronous workflow when the response headers received fail parsing.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncFailureParsingHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a failure to parse the received headers. */
    _networkInterface.send = _networkSendSuccessWithSettingParseFailForHeaders;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case header and body message size just for testing a failure to parse. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );

    /* Verify that the network is disconnected. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the correct asynchronous workflow when the response body received fails parsing.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncFailureParsingBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Test a failure to parse the received body. */
    _networkInterface.send = _networkSendSuccessWithSettingParseFailForBody;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case header and body message size just for testing a failure to parse. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );

    /* Verify that the network is disconnected. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that the response body is received correctly when part of it received into the header buffer.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncSomeBodyInHeaderBuffer )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

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

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that the response body is received correctly when there are some headers received into the body buffer.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncSomeHeaderInBodyBuffer )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate a response message where part of the headers is received into the body buffer. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH * 2;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that the response body is received correctly when the whole response is received into the header
 * buffer.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncEntireResponseInHeaderBuffer )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate a response message where all of the body is in the header buffer. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH / 2;
    bodyLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH / 4;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that the #IotHttpsClientCallbacks_t.readReadyCallback is invoked multiple times when the body is
 * too large to fit into the buffer supplied to read.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncBodyTooLarge )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    int headerLength = 0;
    int bodyLength = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate a response message where all of the body is in the header buffer. */
    headerLength = HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH;
    bodyLength = HTTPS_TEST_RESP_BODY_BUFFER_SIZE + 1;
    _generateHttpResponseMessage( headerLength, bodyLength );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 2, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify a successful asynchronous workflow when the #IotHttpsClientCallbacks_t.readReadyCallback is set to
 * NULL.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncIgnoreResponseBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    /* Test ignoring the response body. */
    _asyncInfoBase.callbacks.readReadyCallback = NULL;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case response, just to test the body being ignored. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the asynchronous workflow when the request is cancelled before it is scheduled.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncCancelBeforeScheduled )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* This test is only valid if there are 2 or more async requests available to schedule. */
    TEST_ASSERT_GREATER_THAN( 1, HTTPS_TEST_MAX_ASYNC_REQUESTS );

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    _pAsyncRequestHandles[ 0 ] = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ 0 ] );
    _pAsyncRequestHandles[ 1 ] = _getReqHandle( &( _pAsyncReqInfos[ 1 ] ) );
    TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ 1 ] );

    _verifParams.numRequestsTotal = 2;
    _verifParams.numRequestsLeft = 2;

    /* Generate an ideal case response. Only the first request scheduled should invoke any of the callbacks. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule two requests here, then immediately cancel the second one. It takes greater than 300 ms
     * for the network receive callback to be invoked for the first request.  This is configured in
     * HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS. If HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS is too small
     * and the second request is scheduled, this failure will be reflected in the async callback counts verification. */
    returnCode = IotHttpsClient_SendAsync( connHandle,
                                           _pAsyncRequestHandles[ 0 ],
                                           &( _pAsyncResponseHandles[ 0 ] ),
                                           &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    returnCode = IotHttpsClient_SendAsync( connHandle,
                                           _pAsyncRequestHandles[ 1 ],
                                           &( _pAsyncResponseHandles[ 1 ] ),
                                           &( _pAsyncRespInfos[ 1 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    IotHttpsClient_CancelRequestAsync( _pAsyncRequestHandles[ 1 ] );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( IOT_HTTPS_SEND_ABORT, _verifParams.returnCode[ 1 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    /* The response complete callback will be called for a cancelled request. */
    TEST_ASSERT_EQUAL( 2, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the asynchronous workflow when the request is cancelled during the
 * #IotHttpsClientCallbacks_t.appendHeaderCallback.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncCancelDuringAppendHeaderCallback )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Assign a test custom appendHeaderCallback that cancels the request. */
    _asyncInfoBase.callbacks.appendHeaderCallback = _appendHeaderCallbackThatCancels;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* When this numRequestsLeft becomes zero, then _verifParams.completeSem is posted to in _responseCompleteCallbackCount. */
    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case response. Only the first request scheduled should invoke any of the callbacks. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_SEND_ABORT, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the asynchronous workflow when the request is cancelled during the
 * #IotHttpsClientCallbacks_t.writeCallback.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncCancelDuringWriteCallback )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Assign a test custom writeCallback that cancels the request. */
    _asyncInfoBase.callbacks.writeCallback = _writeCallbackThatCancels;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* When this numRequestsLeft becomes zero, then _verifParams.completeSem is posted to in _responseCompleteCallbackCount. */
    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case response. Only the first request scheduled should invoke any of the callbacks. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_SEND_ABORT, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the asynchronous workflow when the request is cancelled during the
 * #IotHttpsClientCallbacks_t.readReadyCallback.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncCancelDuringReadReadyCallback )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Assign a test custom writeCallback that cancels the request. */
    _asyncInfoBase.callbacks.readReadyCallback = _readReadyCallbackThatCancels;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* When this parameter becomes zero, then _verifParams.completeSem is posted to in _responseCompleteCallbackCount. */
    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Generate an ideal case response. Only the first request scheduled should invoke any of the callbacks. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_RECEIVE_ABORT, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify the data received when scheduling multiple asynchronous requests in the happy path case.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    /* All the requests except the one that was cancelled before it was scheduled will decrement this parameters. */
    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ reqIndex ] );
    }

    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that all pending requests after are not sent when the first request on the connection has a network
 * send failure.
 *
 * This test relies on there being some request after the one with a network failure to send. Since the send happens
 * soon after the first request is schedule, this test does the send failure on the second request. With the 300 ms
 * (HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS) this is enough time to schedule another one or more requests after
 * to verify that requests following do get dropped.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsSecondHasNetworkSendFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;

    /* This test is only valid if there are 3 or more async requests available to schedule. */
    TEST_ASSERT_GREATER_THAN( 2, HTTPS_TEST_MAX_ASYNC_REQUESTS );

    _networkInterface.send = _networkSendFailsOnSecondHeaderSend;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    /* Make sure that the test is valid by asserting that the request after the one that fails exists in the queue. */
    TEST_ASSERT_EQUAL_PTR( &( _pAsyncRequestHandles[ 2 ]->link ), IotListDouble_FindFirstMatch( ( IotListDouble_t * ) ( &( connHandle->reqQ ) ),
                                                                                                NULL,
                                                                                                NULL,
                                                                                                &( _pAsyncRequestHandles[ 2 ]->link ) ) );
    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* Wait for any errors that may arise from straggling requests that should not exist. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS * 2 );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, _verifParams.returnCode[ 1 ] );
    TEST_ASSERT_EQUAL( 2, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 2, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 2, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
    /* Verify that the connection is closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
    /* Verify that there are no pending requests or responses. */
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->reqQ ) ) );
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->respQ ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that all pending requests after are not sent when the first request on the connection has a network
 * receive failure.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstHasNetworkReceiveFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;

    /* This test is only valid if there are 2 or more async requests available to schedule. */
    TEST_ASSERT_GREATER_THAN( 1, HTTPS_TEST_MAX_ASYNC_REQUESTS );

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveFailHeaders;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. There is a 300 ms delay after the first request sends until the network receive
     * callback is invoked. This should be enough time to place all other requests into the queue pending scheduling. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    /* Make sure the test is valid that there are 2 or more items in the request queue. */
    TEST_ASSERT_GREATER_THAN( 1, IotDeQueue_Count( &( connHandle->reqQ ) ) );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* Wait for any errors that may arise from straggling requests that should not exist. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS * 2 );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_NETWORK_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    /* If reading the headers of the first request fails, then no reading of the body callback is invoked. */
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
    /* Verify that the connection is closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
    /* Verify that there are no pending requests or responses. */
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->reqQ ) ) );
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->respQ ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that all pending requests after are not sent when the first request on the connection has a failure
 * to parse the response.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstHasParsingFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;

    /* This test is only valid if there are 2 or more async requests available to schedule. */
    TEST_ASSERT_GREATER_THAN( 1, HTTPS_TEST_MAX_ASYNC_REQUESTS );

    _networkInterface.send = _networkSendSuccessWithSettingParseFailForHeaders;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. There is a 300 ms delay after the first request sends until the network receive
     * callback is invoked. This should be enough time to place all other requests into the queue pending scheduling. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    /* Make sure the test is valid that there are 2 or more items in the request queue. */
    TEST_ASSERT_GREATER_THAN( 1, IotDeQueue_Count( &( connHandle->reqQ ) ) );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* Wait for any errors that may arise from straggling requests that should not exist. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS * 2 );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    /* Parsing of the headers occurs after reading the headers, so no reading of the body is invoked. */
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
    /* Verify that the connection is closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
    /* Verify that there are no pending requests or responses. */
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->reqQ ) ) );
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->respQ ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that all pending requests after are not sent when the first request on the connection is
 * non-persistent.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstIsNonPersistent )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;

    /* This test is only valid if there are 2 or more async requests available to schedule. */
    TEST_ASSERT_GREATER_THAN( 1, HTTPS_TEST_MAX_ASYNC_REQUESTS );

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    /* Set the first request as non-persistent to test that all other requests after are not sent because the
     * connection closed. */
    _pAsyncReqInfos[ 0 ].isNonPersistent = true;

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. There is a 300 ms delay after the first request sends until the network receive
     * callback is invoked. This should be enough time to place all other requests into the queue pending scheduling. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    /* Make sure the test is valid that there are 2 or more items in the request queue. */
    TEST_ASSERT_GREATER_THAN( 1, IotDeQueue_Count( &( connHandle->reqQ ) ) );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* Wait for any errors that may arise from straggling requests that should not exist. */
    IotClock_SleepMs( HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS * 2 );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
    /* Verify that the connection is closed. */
    TEST_ASSERT_FALSE( connHandle->isConnected );
    /* Verify that there are no pending requests or responses. */
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->reqQ ) ) );
    TEST_ASSERT_EQUAL( true, IotDeQueue_IsEmpty( &( connHandle->respQ ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that all pending requests after read the response body correct when the first response ignores it.
 *
 * This test makes sure the response flushing is working so that the workflow is not corrupted.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstIgnoresPresentResponseBody )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;
    IotHttpsAsyncInfo_t testAsyncInfo;

    /* Set the read ready callback as NULL in a test async info to replace the reference in the first request. */
    memcpy( &testAsyncInfo, &_asyncInfoBase, sizeof( IotHttpsAsyncInfo_t ) );
    testAsyncInfo.callbacks.readReadyCallback = NULL;
    _pAsyncReqInfos[ 0 ].u.pAsyncInfo = &testAsyncInfo;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    /* All the requests except the one that was cancelled before it was scheduled will decrement this parameters. */
    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ reqIndex ] );
    }

    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS - 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that when an earlier request gets cancelled the requests after still get schedule to run.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsOneGetsCancelled )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    int reqIndex = 0;
    /* Cancelling request 1, which is after request 0. */
    int reqToCancel = 1;

    /* This test is only valid if there are 3 or more async requests available to schedule. */
    TEST_ASSERT_GREATER_THAN( 2, HTTPS_TEST_MAX_ASYNC_REQUESTS );

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        _pAsyncRequestHandles[ reqIndex ] = _getReqHandle( &( _pAsyncReqInfos[ reqIndex ] ) );
        TEST_ASSERT_NOT_NULL( _pAsyncRequestHandles[ reqIndex ] );
    }

    /* All the requests except the one that was cancelled before it was scheduled will decrement this parameters. */
    _verifParams.numRequestsTotal = HTTPS_TEST_MAX_ASYNC_REQUESTS;
    _verifParams.numRequestsLeft = HTTPS_TEST_MAX_ASYNC_REQUESTS;

    /* Generate an ideal case response. */
    _generateHttpResponseMessage( HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH, HTTPS_TEST_RESP_BODY_BUFFER_SIZE );

    /* Schedule all of the requests. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        returnCode = IotHttpsClient_SendAsync( connHandle,
                                               _pAsyncRequestHandles[ reqIndex ],
                                               &( _pAsyncResponseHandles[ reqIndex ] ),
                                               &( _pAsyncRespInfos[ reqIndex ] ) );
        TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    }

    IotHttpsClient_CancelRequestAsync( _pAsyncRequestHandles[ reqToCancel ] );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    for( reqIndex = 0; reqIndex < HTTPS_TEST_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        if( reqIndex == reqToCancel )
        {
            TEST_ASSERT_EQUAL( IOT_HTTPS_SEND_ABORT, _verifParams.returnCode[ reqIndex ] );
        }
        else
        {
            TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ reqIndex ] );
        }
    }

    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS - 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS - 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS - 1, _verifParams.readReadyCallbackCount );
    /* The response complete callback will be called for a cancelled request. */
    TEST_ASSERT_EQUAL( HTTPS_TEST_MAX_ASYNC_REQUESTS, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.errorCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.readReadyCallbackCountPerResponse[ reqToCancel ] );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test receiving a chunked HTTP response message in the asynchronous workflow.
 */
TEST( HTTPS_Client_Unit_Async, SendAsyncChunkedResponse )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    _networkInterface.send = _networkSendSuccess;
    _networkInterface.receiveUpto = _networkReceiveSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle( &( _pAsyncReqInfos[ 0 ] ) );
    TEST_ASSERT_NOT_NULL( reqHandle );

    _verifParams.numRequestsTotal = 1;
    _verifParams.numRequestsLeft = 1;

    /* Setup the test response message to receive the HTTP rest chinked response. */
    memcpy( _pRespMessageBuffer, HTTPS_TEST_CHUNKED_RESPONSE, sizeof( HTTPS_TEST_CHUNKED_RESPONSE ) - 1 );

    returnCode = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &( _pAsyncRespInfos[ 0 ] ) );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );

    /* Wait on the async request to finish. */
    TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &( _verifParams.completeSem ), HTTPS_TEST_ASYNC_TIMEOUT_MS ) );

    /* If we made it here, then we indeed finished. Verify all of the parameters. */
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, _verifParams.returnCode[ 0 ] );
    TEST_ASSERT_EQUAL( 1, _verifParams.appendHeaderCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.writeCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.readReadyCallbackCount );
    TEST_ASSERT_EQUAL( 1, _verifParams.responseCompleteCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.connectionClosedCallbackCount );
    TEST_ASSERT_EQUAL( 0, _verifParams.errorCallbackCount );
}
