/*
 * FreeRTOS Secure Sockets V1.2.0
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "unity.h"

#include "mock_iot_secure_sockets.h"

/* Transport interface include. */
#include "transport_interface.h"
#include "transport_secure_sockets.h"
#include "iot_secure_sockets.h"

/* The send and receive timeout to set for the socket. */
#define SEND_RECV_TIMEOUT          0

/* The host and port from which to establish the connection. */
#define HOSTNAME                   "amazon.com"
#define INVALID_HOSTNAME           "amazon.com9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
#define PORT                       80

/* Configuration parameters for the TLS connection. */
#define MFLN                       42
#define ALPN_PROTOS                "x-amzn-mqtt-ca"

#define MOCK_ROOT_CA               "mockRootCA"
#define MOCK_SERVER_ADDRESS        100
#define MOCT_TCP_SOCKET            100
#define MOC_SECURE_SOCKET_ERROR    -1


/* Parameters to pass to #SecureSocketsTransport_Send and #SecureSocketsTransport_Recv. */
#define BYTES_TO_SEND                      4
#define BYTES_TO_RECV                      4
#define SECURE_SOCKETS_READ_WRITE_ERROR    -1

/* The size of the buffer passed to #SecureSocketsTransport_Send and #SecureSocketsTransport_Recv. */
#define BUFFER_LEN                         4

/**
 * @brief Transport timeout in milliseconds for transport send.
 */
#define TEST_TRANSPORT_SND_TIMEOUT_MS      ( 5000U )

/**
 * @brief Transport timeout in milliseconds for transport receive.
 */
#define TEST_TRANSPORT_RCV_TIMEOUT_MS      ( 5000U )

/* Objects used by the transport secure sockets implementation. */
static ServerInfo_t serverInfo =
{
    .pHostName      = HOSTNAME,
    .hostNameLength = strlen( HOSTNAME ),
    .port           = PORT
};
static SocketsConfig_t socketsConfig =
{
    .enableTls         = true,
    .pAlpnProtos       = ALPN_PROTOS,
    .maxFragmentLength = MFLN,
    .disableSni        = false,
    .pRootCa           = NULL,
    .rootCaSize        = 0,
    .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
    .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
};

static uint8_t networkBuffer[ BUFFER_LEN ] = { 0 };
static Socket_t mockTcpSocket = ( Socket_t ) MOCT_TCP_SOCKET;
static NetworkContext_t networkContext = { 0 };

/* ============================   UNITY FIXTURES ============================ */

/* Called before each test method. */
void setUp()
{
    networkContext.tcpSocket = mocTcpSocket;
}

/* Called after each test method. */
void tearDown()
{
}

/* Called at the beginning of the whole suite. */
void suiteSetUp()
{
}

/* Called at the end of the whole suite. */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/* ========================================================================== */


/**
 * @brief Test that #SecureSocketsTransport_Connect fails with NULL parameters
 * and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Invalid_Params( void )
{
    TransportSocketStatus_t returnStatus;
    ServerInfo_t invalidServerInfo = { 0 };

    invalidServerInfo.port = PORT;
    returnStatus = SecureSocketsTransport_Connect( NULL,
                                                   &serverInfo,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );

    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   NULL,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );

    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   NULL );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );

    invalidServerInfo.pHostName = NULL;

    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &invalidServerInfo,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );

    invalidServerInfo.pHostName = INVALID_HOSTNAME;
    invalidServerInfo.hostNameLength = 0;

    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &invalidServerInfo,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );

    invalidServerInfo.hostNameLength = strlen( INVALID_HOSTNAME );

    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );

    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &invalidServerInfo,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect fails on socket creation,
 *  and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Insufficient_Memory( void )
{
    TransportSocketStatus_t returnStatus;

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_INVALID_SOCKET );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INSUFFICIENT_MEMORY, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to set requires TLS and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Invalid_Credentials_SetRequireTLS( void )
{
    TransportSocketStatus_t returnStatus;

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &socketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_CREDENTIALS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to set AlpnProtos and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Invalid_Credentials_AlpnProtos( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = true,
        .pAlpnProtos       = ALPN_PROTOS,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_CREDENTIALS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to set SNI and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Invalid_Credentials_SNI( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = true,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = false,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_CREDENTIALS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to set RootCA and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Invalid_Credentials_RootCA( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = true,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = MOCK_ROOT_CA,
        .rootCaSize        = strlen( MOCK_ROOT_CA ),
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_CREDENTIALS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to resolve host name and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Dns_Failure( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = false,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( 0 );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_DNS_FAILURE, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter network error and verify the error code.
 */
void test_SecureSocketsTransport_Connect_Fail_to_Connect( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = false,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_CONNECT_FAILURE, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to set receive timeout and verify the error code.
 */
void test_SecureSocketsTransport_Connect_TimeOutSetup_Failure_Recv( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = false,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = 0XFFFFFFFF,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Connect with parameters that encounter fail to set send timeout and verify the error code.
 */
void test_SecureSocketsTransport_Connect_TimeOutSetup_Failure_Send( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = false,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = 0XFFFFFFFF
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case to connect to server without TLS.
 */
void test_SecureSocketsTransport_Connect_Succeeds_without_TLS( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = false,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case to connect to server without TLS and set send/receive timeout with zero value.
 */
void test_SecureSocketsTransport_Connect_Succeeds_Set_Timeout_Zero( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = false,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = 0,
        .recvTimeoutMs     = 0
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case to connect to server with TLS.
 */
void test_SecureSocketsTransport_Connect_Succeeds_with_TLS( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = true,
        .pAlpnProtos       = ALPN_PROTOS,
        .maxFragmentLength = MFLN,
        .disableSni        = false,
        .pRootCa           = MOCK_ROOT_CA,
        .rootCaSize        = strlen( MOCK_ROOT_CA ),
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case to connect to server with TLS but RootCa and ALPN, SNI are not set.
 */
void test_SecureSocketsTransport_Connect_Credentials_NotSet( void )
{
    TransportSocketStatus_t returnStatus;
    SocketsConfig_t localSocketsConfig =
    {
        .enableTls         = true,
        .pAlpnProtos       = NULL,
        .maxFragmentLength = MFLN,
        .disableSni        = true,
        .pRootCa           = NULL,
        .rootCaSize        = 0,
        .sendTimeoutMs     = TEST_TRANSPORT_SND_TIMEOUT_MS,
        .recvTimeoutMs     = TEST_TRANSPORT_RCV_TIMEOUT_MS
    };

    SOCKETS_Socket_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_GetHostByName_ExpectAnyArgsAndReturn( MOCK_SERVER_ADDRESS );
    SOCKETS_Connect_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_SetSockOpt_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Connect( &networkContext,
                                                   &serverInfo,
                                                   &localSocketsConfig );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test #SecureSocketsTransport_Disconnect with invalid parameters.
 */
void test_SecureSocketsTransport_Disconnect_NULL_Network_Context( void )
{
    TransportSocketStatus_t returnStatus;

    returnStatus = SecureSocketsTransport_Disconnect( NULL );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the #SecureSocketsTransport_Disconnect and encounterd #SOCKETS_Shutdown failure.
 */
void test_SecureSocketsTransport_Disconnect_Fail_to_ShutDown( void )
{
    TransportSocketStatus_t returnStatus;

    SOCKETS_Shutdown_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    returnStatus = SecureSocketsTransport_Disconnect( &networkContext );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test when #SecureSocketsTransport_Disconnect encounters a
 * #SOCKETS_Close failure.
 */
void test_SecureSocketsTransport_Disconnect_Fail_to_Close( void )
{
    TransportSocketStatus_t returnStatus;

    SOCKETS_Shutdown_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_Close_ExpectAnyArgsAndReturn( MOC_SECURE_SOCKET_ERROR );
    returnStatus = SecureSocketsTransport_Disconnect( &networkContext );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief A happy path #SecureSocketsTransport_Disconnect test.
 */
void test_SecureSocketsTransport_Disconnect_Sucess( void )
{
    TransportSocketStatus_t returnStatus;

    SOCKETS_Shutdown_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    SOCKETS_Close_ExpectAnyArgsAndReturn( SOCKETS_ERROR_NONE );
    returnStatus = SecureSocketsTransport_Disconnect( &networkContext );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, returnStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Send is able to return that 0 bytes are sent over
 * the network stack, when passing any invalid parameters.
 */
void test_SecureSocketsTransport_Send_Invalid_Params( void )
{
    int32_t bytesSent;
    NetworkContext_t invalidNetworkContext = { 0 };

    invalidNetworkContext.tcpSocket = SOCKETS_INVALID_SOCKET;

    bytesSent = SecureSocketsTransport_Send( NULL, networkBuffer, BYTES_TO_SEND );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesSent );

    bytesSent = SecureSocketsTransport_Send( &invalidNetworkContext, NULL, BYTES_TO_SEND );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesSent );

    bytesSent = SecureSocketsTransport_Send( &invalidNetworkContext, networkBuffer, 0 );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesSent );

    bytesSent = SecureSocketsTransport_Send( &invalidNetworkContext, networkBuffer, BYTES_TO_SEND );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesSent );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Send returns an error when #SOCKETS_Send fails to send
 * data over the network stack.
 */
void test_SecureSocketsTransport_Send_Network_Error( void )
{
    int32_t bytesSent;

    SOCKETS_Send_ExpectAnyArgsAndReturn( SECURE_SOCKETS_READ_WRITE_ERROR );
    bytesSent = SecureSocketsTransport_Send( &networkContext, networkBuffer, BYTES_TO_SEND );
    TEST_ASSERT_EQUAL( SECURE_SOCKETS_READ_WRITE_ERROR, bytesSent );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case when #SecureSocketsTransport_Send is able to send all bytes
 * over the network stack successfully.
 */
void test_SecureSocketsTransport_Send_All_Bytes_Sent_Successfully( void )
{
    int32_t bytesSent;

    SOCKETS_Send_ExpectAnyArgsAndReturn( BYTES_TO_SEND );
    bytesSent = SecureSocketsTransport_Send( &networkContext, networkBuffer, BYTES_TO_SEND );
    TEST_ASSERT_EQUAL( BYTES_TO_SEND, bytesSent );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case of when #SecureSocketsTransport_Send sends
 * fewer bytes then BYTES_TO_SEND.
 */
void test_SecureSocketsTransport_Send_Bytes_Sent_Partially( void )
{
    int32_t bytesSent;

    SOCKETS_Send_ExpectAnyArgsAndReturn( BYTES_TO_SEND - 1 );
    bytesSent = SecureSocketsTransport_Send( &networkContext, networkBuffer, BYTES_TO_SEND );
    TEST_ASSERT_EQUAL( BYTES_TO_SEND - 1, bytesSent );
}


/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Recv is able to return that 0 bytes are received
 * from the network stack when passing any invalid parameters.
 */
void test_SecureSocketsTransport_Recv_Invalid_Params( void )
{
    int32_t bytesReceived;
    NetworkContext_t invalidNetworkContext = { 0 };

    invalidNetworkContext.tcpSocket = SOCKETS_INVALID_SOCKET;

    bytesReceived = SecureSocketsTransport_Recv( NULL, networkBuffer, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesReceived );

    bytesReceived = SecureSocketsTransport_Recv( &invalidNetworkContext, NULL, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesReceived );

    bytesReceived = SecureSocketsTransport_Recv( &invalidNetworkContext, networkBuffer, 0 );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesReceived );

    bytesReceived = SecureSocketsTransport_Recv( &invalidNetworkContext, networkBuffer, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( SOCKETS_EINVAL, bytesReceived );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case when #SecureSocketsTransport_Recv is able to receive all
 * expected bytes over the network stack.
 */
void test_SecureSocketsTransport_Recv_All_Bytes_Received_Successfully( void )
{
    int32_t bytesReceived;

    SOCKETS_Recv_ExpectAnyArgsAndReturn( BYTES_TO_RECV );
    bytesReceived = SecureSocketsTransport_Recv( &networkContext, networkBuffer, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( BYTES_TO_RECV, bytesReceived );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that #SecureSocketsTransport_Recv returns an error when #SOCKETS_Recv fails to
 * receive data over the network stack.
 */
void test_SecureSocketsTransport_Recv_Network_Error( void )
{
    int32_t bytesReceived;

    SOCKETS_Recv_ExpectAnyArgsAndReturn( SOCKETS_EWOULDBLOCK );
    bytesReceived = SecureSocketsTransport_Recv( &networkContext, networkBuffer, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( 0, bytesReceived );

    SOCKETS_Recv_ExpectAnyArgsAndReturn( SECURE_SOCKETS_READ_WRITE_ERROR );
    bytesReceived = SecureSocketsTransport_Recv( &networkContext, networkBuffer, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( SECURE_SOCKETS_READ_WRITE_ERROR, bytesReceived );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test the happy path case of when #SecureSocketsTransport_Recv received
 * fewer bytes than BYTES_TO_RECV.
 */
void test_SecureSocketsTransport_Recv_Bytes_Received_Partially( void )
{
    int32_t bytesReceived;

    SOCKETS_Recv_ExpectAnyArgsAndReturn( BYTES_TO_RECV - 1 );
    bytesReceived = SecureSocketsTransport_Recv( &networkContext, networkBuffer, BYTES_TO_RECV );
    TEST_ASSERT_EQUAL( BYTES_TO_RECV - 1, bytesReceived );
}


/*-------------------------------------------------------------------*/
/*-----------------------End Tests-----------------------------------*/
/*-------------------------------------------------------------------*/
