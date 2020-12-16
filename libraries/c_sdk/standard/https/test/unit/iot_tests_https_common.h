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
 * @file iot_tests_https_common.h
 * @brief Common definitions for the HTTPS Client unit tests.
 */
#ifndef IOT_TESTS_HTTPS_COMMON_H_
#define IOT_TESTS_HTTPS_COMMON_H_

/* The config header is always included first. */
#include "iot_config.h"

/* C standard includes. */
#include <string.h>

/* SDK initialization include. */
#include "iot_init.h"

/* HTTPS Client internal include. */
#include "private/iot_https_internal.h"

/* Test access include. */
#include "iot_test_access_https.h"

/* Credential includes. */
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief Test TLS TCP port.
 */
#define HTTPS_TEST_PORT             ( ( uint16_t ) 443 )

/**
 * @brief Test address to share among the tests.
 */
#define HTTPS_TEST_ADDRESS          "www.amazon.com"

/**
 * @brief Test path to share among the tests.
 */
#define HTTPS_TEST_PATH             "/path.txt"

/**
 * @brief Test HTTP/1.1 protocol to share among the tests.
 */
#define HTTPS_TEST_ALPN_PROTOCOL    "http/1.1"

/**
 * @brief Baltimore Cybertrust root CA to share among the tests.
 */
#define HTTPS_TEST_ROOT_CA                                               \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"                             \
    "-----END CERTIFICATE-----\n"


/**
 * @brief The size of the connection user buffer to use among the tests.
 */
#define HTTPS_TEST_CONN_USER_BUFFER_SIZE        ( sizeof( _httpsConnection_t ) )

/**
 * @brief The size of the request user buffer to use among the tests.
 */
#define HTTPS_TEST_REQ_USER_BUFFER_SIZE         ( sizeof( _httpsRequest_t ) + 256 )

/**
 * @brief The size of the response user buffer to use among the tests.
 *
 * The header space comes after the response context, hence the addition to the response context.
 */
#define HTTPS_TEST_RESP_USER_BUFFER_SIZE        ( sizeof( _httpsResponse_t ) + 128 )

/**
 * @brief The size of the response body buffer to use among the tests.
 */
#define HTTPS_TEST_RESP_BODY_BUFFER_SIZE        ( 128 )

/**
 * @brief The maximum length of the HTTP response message buffer shared among the test.
 *
 * The buffer of this length is used to test a few scenarios where the headers or body are found in either the header
 * buffer or body buffer and need to be copied over appropriately.
 */
#define HTTPS_TEST_RESPONSE_MESSAGE_LENGTH      ( 512 )

/**
 * @brief The length of the response header buffer shared among the tests.
 */
#define HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH    ( HTTPS_TEST_RESP_USER_BUFFER_SIZE - sizeof( _httpsResponse_t ) )

/**
 * @brief Test HTTP request body to share among the tests.
 */
#define HTTPS_TEST_REQUEST_BODY                                                                                      \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore " \
    "magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo" \
    " consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla paria" \
    "tur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est lab" \
    "orum."
#define HTTPS_TEST_REQUEST_BODY_LENGTH    ( sizeof( HTTPS_TEST_REQUEST_BODY ) - 1 ) /**< @brief The length of the test HTTP request body. */

/**
 * @brief A small test HTTPS response to share among the tests.
 */
#define HTTPS_TEST_SMALL_RESPONSE                               \
    "HTTP/1.1 200 OK\r\nheader0: value0\r\nheader1: value1\r\n" \
    "header2: value2 value2a\r\nContent-Length: 26\r\n\r\nabcdefghijklmnopqrstuvwxyz"
#define HTTPS_TEST_SMALL_RESPONSE_LENGTH    ( sizeof( HTTPS_TEST_SMALL_RESPONSE ) - 1 ) /**< @brief The length of the small test HTTP response. */

/**
 * @brief Test HTTP chunked response message to share among the tests.
 */
#define HTTPS_TEST_CHUNKED_RESPONSE  \
    "HTTP/1.1 403 Forbidden\r\n"     \
    "header0: value0\r\n"            \
    "header1: value1\r\n"            \
    "Transfer-Encoding: chunked\r\n" \
    "\r\n"                           \
    "b\r\n"                          \
    "abcdefghijk\r\n"                \
    "c\r\n"                          \
    "lmnopqrstuvw\r\n"               \
    "3\r\n"                          \
    "xyz\r\n"                        \
    "0\r\n"                          \
    "\r\n"
#define HTTPS_TEST_CHUNKED_RESPONSE_BODY_LENGTH    ( 0xB + 0xC + 3 ) /**< @brief The length of the test chunked HTTP response body. */

/*-----------------------------------------------------------*/

/**
 * @brief HTTPS Client connection user buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
extern uint8_t _pConnUserBuffer[ HTTPS_TEST_CONN_USER_BUFFER_SIZE ];

/**
 * @brief HTTPS Response body buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
extern uint8_t _pRespBodyBuffer[ HTTPS_TEST_RESP_BODY_BUFFER_SIZE ];

/**
 * @brief An HTTP response message to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
extern uint8_t _pRespMessageBuffer[ HTTPS_TEST_RESPONSE_MESSAGE_LENGTH ];

/**
 * @brief HTTPS Request user buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
extern uint8_t _pReqUserBuffer[ HTTPS_TEST_REQ_USER_BUFFER_SIZE ];

/**
 * @brief HTTPS Response user buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
extern uint8_t _pRespUserBuffer[ HTTPS_TEST_RESP_USER_BUFFER_SIZE ];

/**
 * @brief The current place in _pRespMessageBuffer to receive the next byte.
 *
 * This is used to mimic receiving the HTTP response message from the network during testing.
 */
static uint32_t _nextRespMessageBufferByteToReceive = 0;

/**
 * @brief An #IotNetworkInterface_t to share among the tests.
 *
 */
static IotNetworkInterface_t _networkInterface = { 0 };

/**
 * @brief A IotHttpsConnectionInfo_t to share among the tests.
 */
static IotHttpsConnectionInfo_t _connInfo = {
    .pAddress             = HTTPS_TEST_ADDRESS,
    .addressLen           = sizeof( HTTPS_TEST_ADDRESS ) - 1,
    .port                 = HTTPS_TEST_PORT,
    .flags                = 0,
    .pCaCert              = HTTPS_TEST_ROOT_CA,
    .caCertLen            = sizeof( HTTPS_TEST_ROOT_CA ),
    .userBuffer.pBuffer   = _pConnUserBuffer,
    .userBuffer.bufferLen = sizeof( _pConnUserBuffer ),
    .pClientCert          = keyCLIENT_CERTIFICATE_PEM,
    .clientCertLen        = sizeof( keyCLIENT_CERTIFICATE_PEM ),
    .pPrivateKey          = keyCLIENT_PRIVATE_KEY_PEM,
    .privateKeyLen        = sizeof( keyCLIENT_PRIVATE_KEY_PEM ),
    .pAlpnProtocols       = HTTPS_TEST_ALPN_PROTOCOL,
    .alpnProtocolsLen     = sizeof( HTTPS_TEST_ALPN_PROTOCOL ),
    .pNetworkInterface    = &_networkInterface
};

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction create function that succeeds.
 */
static inline IotNetworkError_t _networkCreateSuccess( void * pConnectionInfo,
                                                       void * pCredentialInfo,
                                                       void ** pConnection )
{
    ( void ) pConnectionInfo;
    ( void ) pCredentialInfo;
    ( void ) pConnection;
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction setReceiveCallback that succeeds.
 */
static inline IotNetworkError_t _setReceiveCallbackSuccess( void * pConnection,
                                                            IotNetworkReceiveCallback_t receiveCallback,
                                                            void * pContext )
{
    ( void ) pConnection;
    ( void ) receiveCallback;
    ( void ) pContext;
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Get a valid connected state intialized connection handle using _pConnUserBuffer and _connInfo.
 */
static inline IotHttpsConnectionHandle_t _getConnHandle( void )
{
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    IotHttpsClient_Connect( &connHandle, &_connInfo );
    return connHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Get a valid request handle with the input pReqInfo.
 */
static inline IotHttpsRequestHandle_t _getReqHandle( IotHttpsRequestInfo_t * pReqInfo )
{
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

    IotHttpsClient_InitializeRequest( &reqHandle, pReqInfo );
    return reqHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Get a valid response handle using _pRespUserBuffer, and respInfoGET.
 */
static inline IotHttpsResponseHandle_t _getRespHandle( IotHttpsResponseInfo_t * pRespInfo,
                                                       IotHttpsRequestHandle_t reqHandle )
{
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    IotTestHttps_initializeResponse( &respHandle, pRespInfo, reqHandle );
    return respHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction close function that succeeds.
 */
static inline IotNetworkError_t _networkCloseSuccess( void * pConnection )
{
    ( void ) pConnection;
    /* When the network closes there should be no data on the socket. */
    _nextRespMessageBufferByteToReceive = 0;
    ( void ) memset( _pRespMessageBuffer, 0x00, sizeof( _pRespMessageBuffer ) );
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction destroy function that succeeds.
 */
static inline IotNetworkError_t _networkDestroySuccess( void * pConnection )
{
    ( void ) pConnection;
    return IOT_NETWORK_SUCCESS;
}



/*-----------------------------------------------------------*/

/**
 * @brief Network abstraction receive function that succeeds.
 */
static inline size_t _networkReceiveSuccess( void * pConnection,
                                             uint8_t * pBuffer,
                                             size_t bytesRequested )
{
    size_t responseMessageLengthLeft = strlen( ( char * ) _pRespMessageBuffer ) - _nextRespMessageBufferByteToReceive;
    size_t copyLen = 0;

    ( void ) pConnection;
    ( void ) pBuffer;
    ( void ) bytesRequested;

    if( responseMessageLengthLeft < bytesRequested )
    {
        copyLen = responseMessageLengthLeft;
    }
    else
    {
        copyLen = bytesRequested;
    }

    memcpy( pBuffer, &( _pRespMessageBuffer[ _nextRespMessageBufferByteToReceive ] ), copyLen );
    _nextRespMessageBufferByteToReceive += copyLen;

    return copyLen;
}

/*-----------------------------------------------------------*/

/**
 * @brief Generate an test HTTP response message with the specified header length and the specified body length.
 *
 * This generates the test HTTP response message into _pRespMessageBuffer.
 * _pRespMessageBuffer must be zeroed out before calling this routine.
 * The length of _pRespMessageBuffer must not exceed headerLength + bodyLength.
 * The headerLength is the length of the raw HTTP headers includes delimiters like ": " and "\r\n".
 * The length of headerLength must be greater than:
 *      "HTTP/1.1 200 OK\r\nContent-Length: <bodyLengthStr>\r\nheader0: value0\r\n\r\n"
 * in order to generate meaningful header data.
 */
void _generateHttpResponseMessage( int headerLength,
                                   int bodyLength );

/**
 * @brief Test verify the response body in pBody up to bodyLength.
 *
 * This will verify that 'a' through 'z' (then repeated) are written properly.
 */
void _verifyHttpResponseBody( int bodyLength,
                              uint8_t * pBody,
                              int startIndex );

#endif /* IOT_TESTS_HTTPS_COMMON_H_ */
