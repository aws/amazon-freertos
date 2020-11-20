/*
 * FreeRTOS V202011.00
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
 */

/**
 * @file core_http_system_test.c
 * @brief Integration tests for the coreHTTP library, for the FreeRTOS platform
 * over the Secure Sockets transport layer.
 */

/* Standard header includes. */
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

/* Unity Test framework include. */
#include "unity_fixture.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Include PKCS11 headers. */
#include "core_pkcs11.h"
#include "pkcs11.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

/* Retry parameters. */
#include "retry_utils.h"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "TEST"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

/* The logging configuration macros are defined above to ensure they are not
 * superceded by definitions in the following header files. */

/* Include paths for public enums, structures, and macros. */
#include "core_http_client.h"

/* Include Secure Sockets implementation of transport interface. */
#include "transport_secure_sockets.h"

/******************* Default Configurations values ***********************/

/* Ensure that config macros, required for TLS connection, have been defined. */
#ifndef SERVER_HOST_NAME
    #define SERVER_HOST_NAME    "localhost"
#endif

/* Check that TLS port of the server is defined. */
#ifndef HTTPS_PORT
    #define HTTPS_PORT    443
#endif

/* Use Starfield Root CA as the default Root CA because the TI C3220 Launchpad board
 * requires that the Root CA certificate have its certificate self-signed. The Amazon Root CAs
 * are cross-signed by Starfield Root CA.*/
#ifndef ROOT_CA_CERT
    #define ROOT_CA_CERT    tlsSTARFIELD_ROOT_CERTIFICATE_PEM
#endif

/* Check that size of the user buffer is defined. */
#ifndef USER_BUFFER_LENGTH
    #define USER_BUFFER_LENGTH    ( 2048 )
#endif

/**********************End Configurations********************************/

/**
 * @brief Length of HTTP server host name.
 */
#define SERVER_HOST_LENGTH                       ( ( sizeof( SERVER_HOST_NAME ) - 1 ) )

/**
 * @brief The maximum number of connection retries with server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS            ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retry connection attempts
 * with server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS    ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS         ( 500U )

/**
 * @brief The maximum number of retries to attempt on network error.
 */
#define MAX_RETRY_COUNT                          ( 3 )

/**
 * @brief Paths for different HTTP methods for the specified host.
 */
#define GET_PATH                                 "/get"
#define HEAD_PATH                                "/get"
#define PUT_PATH                                 "/put"
#define POST_PATH                                "/post"

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define TRANSPORT_SEND_RECV_TIMEOUT_MS           ( 5000 )

/**
 * @brief Request body to send for PUT and POST requests.
 */
#define REQUEST_BODY                             "Hello, world!"

/**
 * @brief Length of the request body.
 */
#define REQUEST_BODY_LENGTH                      ( sizeof( REQUEST_BODY ) - 1 )

/**
 * @brief The total length, of the chunked HTTP response body, to test receiving.
 * This length is inserted as a string into the request path, so avoid putting
 * parenthesis around it.
 */
#define CHUNKED_BODY_LENGTH                      128

/**
 * @brief The path used for testing receiving a transfer encoding chunked
 * response.
 */
#define STR_HELPER( x )    # x
#define TO_STR( x )        STR_HELPER( x )
#define GET_CHUNKED_PATH    "/stream-bytes/" TO_STR( CHUNKED_BODY_LENGTH )

/**
 * @brief A test response for http-parser that ends header lines with linefeeds
 * only.
 */
#define HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY \
    "HTTP/1.1 200 OK\n"                    \
    "Content-Length: 27\n"                 \
    "\n"                                   \
    "HTTP/0.0 0\n"                         \
    "test-header0: ab"
#define HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY_BODY_LENGTH       ( 27 )
#define HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY_HEADERS_LENGTH    ( 18 )

/*-----------------------------------------------------------*/

/**
 * @brief Global variable used by the pseudo random number generator.
 * The random number generator is used for calculating exponential back-off
 * with jitter for retry attempts of failed network operations with the broker.
 */
static uint32_t nextRand;

/**
 * @brief Represents the network context used for the TLS session with the
 * server.
 */
static NetworkContext_t networkContext;

/**
 * @brief The transport layer interface used by the HTTP Client library.
 */
static TransportInterface_t transportInterface;

/**
 * @brief Represents the hostname and port of the broker.
 */
static ServerInfo_t serverInfo;

/**
 * @brief A shared buffer used in the tests for storing HTTP request headers and
 * HTTP response headers and body.
 */
static uint8_t userBuffer[ USER_BUFFER_LENGTH ];

/**
 * @brief A HTTPResponse_t to share among the tests. This is used to verify
 * custom expected output.
 */
static HTTPResponse_t response;

/**
 * @brief Network data that is returned in the transportRecvStub.
 */
static uint8_t * pNetworkData = NULL;

/**
 * @brief The length of the network data to return in the transportRecvStub.
 */
static size_t networkDataLen = 0U;

/*-----------------------------------------------------------*/

/**
 * @brief The pseudo random number generator to use for exponential backoff with
 * jitter calculation for connection retries.
 * This function is an implementation the #RetryUtils_RNG_t interface type
 * of the retry utils library API.
 *
 * @return The generated random number. This function ALWAYS succeeds
 * in generating a random number.
 */
static int32_t generateRandomNumber();

/**
 * @brief Seed the random number generator used for exponential backoff
 * used in connection retry attempts.
 *
 * The PKCS11 module is used to seed the random number generator
 * as it allows access to a True Random Number Generator (TRNG) if the
 * vendor platform supports it.
 */
static void seedRandomNumberGenerator();

/**
 * @brief Connect to HTTP server with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[out] pNetworkContext The output parameter to return the created network context.
 *
 */
static void connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext );

/**
 * @brief Send an HTTP request based on a specified method and path, then
 * print the response received from the server.
 *
 * @param[in] pTransportInterface The transport interface for making network calls.
 * @param[in] pMethod The HTTP request method.
 * @param[in] pPath The Request-URI to the objects of interest.
 */
static void sendHttpRequest( const TransportInterface_t * pTransportInterface,
                             const char * pMethod,
                             const char * pPath );

/**
 * @brief A stub for receiving test network data. This always returns success.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pBuffer Buffer to receive the data into.
 * @param[in] bytesToRecv Number of bytes requested from the network.
 *
 * @return The number of bytes received. This will be the minimum of
 * networkDataLen or bytesToRead.
 */
static int32_t transportRecvStub( NetworkContext_t * pNetworkContext,
                                  void * pBuffer,
                                  size_t bytesToRead );

/**
 * @brief A stub for sending data over the network. This always returns success.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pBuffer Buffer containing the bytes to send over the network stack.
 * @param[in] bytesToSend Number of bytes to send over the network.
 *
 * @return This function always returns bytesToWrite.
 */
static int32_t transportSendStub( NetworkContext_t * pNetworkContext,
                                  const void * pBuffer,
                                  size_t bytesToWrite );

/*-----------------------------------------------------------*/


static int32_t generateRandomNumber()
{
    const uint32_t multiplier = 0x015a4e35UL, increment = 1UL;

    nextRand = ( multiplier * nextRand ) + increment;
    return( ( int32_t ) ( nextRand >> 16UL ) & 0x7fffUL );
}

static void seedRandomNumberGenerator()
{
    /* Seed the global variable ONLY if it wasn't already seeded. */
    if( nextRand == 0 )
    {
        CK_FUNCTION_LIST_PTR pFunctionList = NULL;
        CK_SESSION_HANDLE session = CK_INVALID_HANDLE;

        /* Get list of functions supported by the PKCS11 port. */
        TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &pFunctionList ) );
        TEST_ASSERT_TRUE( pFunctionList != NULL );

        /* Initialize PKCS11 module and create a new session. */
        TEST_ASSERT_EQUAL( CKR_OK, xInitializePkcs11Session( &session ) );
        TEST_ASSERT_TRUE( session != CK_INVALID_HANDLE );

        /*
         * Seed random number generator with PKCS11.
         * Use of PKCS11 can allow use of True Random Number Generator (TRNG)
         * if the platform supports it.
         */
        TEST_ASSERT_EQUAL( CKR_OK, pFunctionList->C_GenerateRandom( session,
                                                                    ( unsigned char * ) &nextRand,
                                                                    sizeof( nextRand ) ) );


        /* Close PKCS11 session. */
        TEST_ASSERT_EQUAL( CKR_OK, pFunctionList->C_CloseSession( session ) );
    }
}

static void connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext )
{
    /* Status returned by the retry utilities. */
    RetryUtilsStatus_t retryUtilsStatus = RetryUtilsSuccess;
    /* Struct containing Sockets configurations. */
    SocketsConfig_t socketsConfig = { 0 };
    /* Struct containing the next backoff time. */
    RetryUtilsContext_t reconnectParams;
    uint16_t nextRetryBackOff = 0U;
    /* Status returned by transport implementation. */
    TransportSocketStatus_t transportStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;

    /* Initializer server information. */
    serverInfo.pHostName = SERVER_HOST_NAME;
    serverInfo.hostNameLength = SERVER_HOST_LENGTH;
    serverInfo.port = HTTPS_PORT;

    /* Configure credentials for TLS server-authenticated session. */
    socketsConfig.enableTls = true;
    socketsConfig.pAlpnProtos = NULL;
    socketsConfig.maxFragmentLength = 0;
    socketsConfig.disableSni = false;
    socketsConfig.pRootCa = ROOT_CA_CERT;
    socketsConfig.rootCaSize = sizeof( ROOT_CA_CERT );
    socketsConfig.sendTimeoutMs = TRANSPORT_SEND_RECV_TIMEOUT_MS;
    socketsConfig.recvTimeoutMs = TRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Initialize reconnect attempts and interval. */
    RetryUtils_InitializeParams( &reconnectParams,
                                 CONNECTION_RETRY_BACKOFF_BASE_MS,
                                 CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                 CONNECTION_RETRY_MAX_ATTEMPTS,
                                 generateRandomNumber );

    /* Attempt to connect to HTTP server. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase until maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TLS session, on top of TCP connection, with the HTTP server. */
        transportStatus = SecureSocketsTransport_Connect( pNetworkContext,
                                                          &serverInfo,
                                                          &socketsConfig );

        if( transportStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            /* Get back-off value for the next connection retry. */
            retryUtilsStatus = RetryUtils_GetNextBackOff( &reconnectParams, &nextRetryBackOff );
            TEST_ASSERT_TRUE( retryUtilsStatus != RetryUtilsRngFailure );

            if( retryUtilsStatus == RetryUtilsRetriesExhausted )
            {
                LogError( ( "Connection to the server failed, all attempts exhausted." ) );
            }

            else if( retryUtilsStatus == RetryUtilsSuccess )
            {
                LogWarn( ( "Connection to the server failed. Retrying connection after backoff delay." ) );
                vTaskDelay( pdMS_TO_TICKS( nextRetryBackOff ) );
            }
        }
    } while( ( transportStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( retryUtilsStatus == RetryUtilsSuccess ) );

    TEST_ASSERT_EQUAL( transportStatus, TRANSPORT_SOCKET_STATUS_SUCCESS );
}

/*-----------------------------------------------------------*/

static void sendHttpRequest( const TransportInterface_t * pTransportInterface,
                             const char * pMethod,
                             const char * pPath )
{
    /* Status returned by methods in HTTP Client Library API. */
    HTTPStatus_t httpStatus = HTTPNetworkError;
    /* Tracks number of retry requests made to the HTTP server. */
    uint8_t retryCount = 0;

    /* Configurations of the initial request headers that are passed to
     * #HTTPClient_InitializeRequestHeaders. */
    HTTPRequestInfo_t requestInfo;
    /* Represents header data that will be sent in an HTTP request. */
    HTTPRequestHeaders_t requestHeaders;

    assert( pMethod != NULL );
    assert( pPath != NULL );

    /* Initialize all HTTP Client library API structs to 0. */
    ( void ) memset( &requestInfo, 0, sizeof( requestInfo ) );
    ( void ) memset( &response, 0, sizeof( response ) );
    ( void ) memset( &requestHeaders, 0, sizeof( requestHeaders ) );

    /* Initialize the request object. */
    requestInfo.pHost = SERVER_HOST_NAME;
    requestInfo.hostLen = SERVER_HOST_LENGTH;
    requestInfo.pMethod = pMethod;
    requestInfo.methodLen = strlen( pMethod );
    requestInfo.pPath = pPath;
    requestInfo.pathLen = strlen( pPath );

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. */
    requestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers. */
    requestHeaders.pBuffer = userBuffer;
    requestHeaders.bufferLen = USER_BUFFER_LENGTH;

    /* Initialize the response object. The same buffer used for storing
     * request headers is reused here. */
    response.pBuffer = userBuffer;
    response.bufferLen = USER_BUFFER_LENGTH;

    LogDebug( ( "Sending HTTP %.*s request to %.*s%.*s...",
                ( int32_t ) requestInfo.methodLen, requestInfo.pMethod,
                ( int32_t ) SERVER_HOST_LENGTH, SERVER_HOST_NAME,
                ( int32_t ) requestInfo.pathLen, requestInfo.pPath ) );

    /* Send request to HTTP server. If a network error is found, retry request for a
     * count of MAX_RETRY_COUNT. */
    do
    {
        /* Since the request and response headers share a buffer, request headers should
         * be re-initialized after a failed request. */
        httpStatus = HTTPClient_InitializeRequestHeaders( &requestHeaders,
                                                          &requestInfo );
        TEST_ASSERT_EQUAL( HTTPSuccess, httpStatus );
        TEST_ASSERT_NOT_EQUAL( 0, requestHeaders.headersLen );

        LogDebug( ( "Request Headers:\n%.*s\n"
                    "Request Body:\n%.*s\n",
                    ( int32_t ) requestHeaders.headersLen,
                    ( char * ) requestHeaders.pBuffer,
                    ( int32_t ) REQUEST_BODY_LENGTH, REQUEST_BODY ) );

        httpStatus = HTTPClient_Send( pTransportInterface,
                                      &requestHeaders,
                                      ( uint8_t * ) REQUEST_BODY,
                                      REQUEST_BODY_LENGTH,
                                      &response,
                                      0 );

        if( httpStatus == HTTPNetworkError )
        {
            LogDebug( ( "A network error has occured, retrying request." ) );
            TEST_TEAR_DOWN( coreHTTP_Integration );
            TEST_SETUP( coreHTTP_Integration );
        }

        retryCount++;
    } while( ( httpStatus == HTTPNetworkError ) && ( retryCount < MAX_RETRY_COUNT ) );

    TEST_ASSERT_EQUAL( HTTPSuccess, httpStatus );

    LogDebug( ( "Received HTTP response from %.*s%.*s...\n"
                "Response Headers:\n%.*s\n"
                "Response Status:\n%u\n"
                "Response Body:\n%.*s\n",
                ( int32_t ) SERVER_HOST_LENGTH, SERVER_HOST_NAME,
                ( int32_t ) requestInfo.pathLen, requestInfo.pPath,
                ( int32_t ) response.headersLen, response.pHeaders,
                response.statusCode,
                ( int32_t ) response.bodyLen, response.pBody ) );

    /* Verify that content length is greater than 0 for GET requests. */
    if( strcmp( pMethod, HTTP_METHOD_GET ) )
    {
        TEST_ASSERT_GREATER_THAN( 0, response.contentLength );
    }

    /* Verify response body is present */
    if( strcmp( pMethod, HTTP_METHOD_HEAD ) )
    {
        TEST_ASSERT_GREATER_THAN( 0, response.bodyLen );
    }
}

/*-----------------------------------------------------------*/

static int32_t transportRecvStub( NetworkContext_t * pNetworkContext,
                                  void * pBuffer,
                                  size_t bytesToRead )
{
    size_t bytesToCopy = ( bytesToRead < networkDataLen ) ? bytesToRead : networkDataLen;

    ( void ) pNetworkContext;

    memcpy( pBuffer, pNetworkData, bytesToCopy );
    pNetworkData += bytesToCopy;
    networkDataLen -= bytesToCopy;
    return bytesToCopy;
}

/*-----------------------------------------------------------*/

static int32_t transportSendStub( NetworkContext_t * pNetworkContext,
                                  const void * pBuffer,
                                  size_t bytesToWrite )
{
    ( void ) pNetworkContext;
    ( void ) pBuffer;
    return bytesToWrite;
}


/* ============================   UNITY FIXTURES ============================ */

/**
 * @brief Test group for running coreHTTP system tests.
 */
TEST_GROUP( coreHTTP_Integration );

/* Called before each test method. */
TEST_SETUP( coreHTTP_Integration )
{
    /* Clear the global response before each test. */
    memset( &response, 0, sizeof( HTTPResponse_t ) );
    /* Clear the network context before each test. */
    memset( &networkContext, 0, sizeof( networkContext ) );

    /* Apply defaults and reset the transport receive data globals. */
    pNetworkData = NULL;
    networkDataLen = 0U;

    /* Establish TLS session with HTTP server on top of a newly-created TCP connection. */
    connectToServerWithBackoffRetries( &networkContext );

    /* Define the transport interface. */
    ( void ) memset( &transportInterface, 0, sizeof( transportInterface ) );
    transportInterface.recv = SecureSocketsTransport_Recv;
    transportInterface.send = SecureSocketsTransport_Send;
    transportInterface.pNetworkContext = &networkContext;
}

/* Called after each test method. */
TEST_TEAR_DOWN( coreHTTP_Integration )
{
    TransportSocketStatus_t transportStatus;

    /* End TLS session, then close TCP connection. */
    transportStatus = SecureSocketsTransport_Disconnect( &networkContext );

    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, transportStatus );
}

/* ========================== Test Cases ============================ */

/**
 * @brief Test group runner for HTTP system tests.
 */
TEST_GROUP_RUNNER( coreHTTP_Integration )
{
    RUN_TEST_CASE( coreHTTP_Integration, test_HTTP_GET_Request );
    RUN_TEST_CASE( coreHTTP_Integration, test_HTTP_HEAD_Request );
    RUN_TEST_CASE( coreHTTP_Integration, test_HTTP_POST_Request );
    RUN_TEST_CASE( coreHTTP_Integration, test_HTTP_PUT_Request );
    RUN_TEST_CASE( coreHTTP_Integration, test_HTTP_Chunked_Response );
    RUN_TEST_CASE( coreHTTP_Integration, test_HTTP_LineFeedOnly_Response );
}

/**
 * @brief Sends a GET request synchronously and verifies the result.
 */
void test_HTTP_GET_Request( void )
{
    sendHttpRequest( &transportInterface,
                     HTTP_METHOD_GET,
                     GET_PATH );
}

TEST( coreHTTP_Integration, test_HTTP_GET_Request )
{
    test_HTTP_GET_Request();
}

/**
 * @brief Sends a HEAD request synchronously and verifies the result.
 */
void test_HTTP_HEAD_Request( void )
{
    sendHttpRequest( &transportInterface,
                     HTTP_METHOD_HEAD,
                     HEAD_PATH );
}

TEST( coreHTTP_Integration, test_HTTP_HEAD_Request )
{
    test_HTTP_HEAD_Request();
}

/**
 * @brief Sends a POST request synchronously and verifies the result.
 */
void test_HTTP_POST_Request( void )
{
    sendHttpRequest( &transportInterface,
                     HTTP_METHOD_POST,
                     POST_PATH );
}

TEST( coreHTTP_Integration, test_HTTP_POST_Request )
{
    test_HTTP_POST_Request();
}

/**
 * @brief Sends a PUT request synchronously and verifies the result.
 */
void test_HTTP_PUT_Request( void )
{
    sendHttpRequest( &transportInterface,
                     HTTP_METHOD_PUT,
                     PUT_PATH );
}

TEST( coreHTTP_Integration, test_HTTP_PUT_Request )
{
    test_HTTP_PUT_Request();
}

/**
 * @brief Receive a Transfer-Encoding chunked response.
 */
void test_HTTP_Chunked_Response( void )
{
    sendHttpRequest( &transportInterface,
                     HTTP_METHOD_GET,
                     GET_CHUNKED_PATH );

    /* Verify the response is of the length configured, which means the
     * chunk header was overwritten. */
    TEST_ASSERT_EQUAL( CHUNKED_BODY_LENGTH, response.bodyLen );
}

TEST( coreHTTP_Integration, test_HTTP_Chunked_Response )
{
    test_HTTP_Chunked_Response();
}

/**
 * @brief Test how http-parser responds with a response of line-feeds only and
 * no carriage returns.
 */
void test_HTTP_LineFeedOnly_Response( void )
{
    HTTPRequestHeaders_t requestHeaders = { 0 };
    HTTPStatus_t status = HTTPSuccess;
    TransportInterface_t transport = { 0 };
    const char * dummyRequestHeaders = "GET something.txt HTTP/1.1\r\n"
                                       "Host: integration-test\r\n\r\r\n";

    requestHeaders.pBuffer = userBuffer;
    requestHeaders.bufferLen = USER_BUFFER_LENGTH;
    response.pBuffer = userBuffer;
    response.bufferLen = USER_BUFFER_LENGTH;

    transport.send = transportSendStub;
    transport.recv = transportRecvStub;

    /* Setup the data to receive on the network. */
    pNetworkData = ( uint8_t * ) HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY;
    networkDataLen = sizeof( HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY ) - 1U;

    /* Setup the request headers to the predefined headers. */
    requestHeaders.headersLen = strlen( dummyRequestHeaders );
    memcpy( requestHeaders.pBuffer, dummyRequestHeaders, requestHeaders.headersLen );

    status = HTTPClient_Send( &transport,
                              &requestHeaders,
                              NULL,
                              0,
                              &response,
                              HTTP_SEND_DISABLE_CONTENT_LENGTH_FLAG );

    TEST_ASSERT_EQUAL( HTTPSuccess, status );
    TEST_ASSERT_EQUAL( HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY_BODY_LENGTH, response.bodyLen );
    TEST_ASSERT_EQUAL( HTTP_TEST_RESPONSE_LINE_FEEDS_ONLY_HEADERS_LENGTH, response.headersLen );
}

TEST( coreHTTP_Integration, test_HTTP_LineFeedOnly_Response )
{
    test_HTTP_LineFeedOnly_Response();
}
