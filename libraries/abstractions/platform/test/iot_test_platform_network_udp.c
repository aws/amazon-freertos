/*
 * Amazon FreeRTOS Platform V1.0.0
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
 * @file iot_test_platform_network_udp.c
 * @brief Tests for the functions iot_network_udp.h
 */

#include "iot_config.h"

/* Test framework includes. */
#include <string.h>
#include "unity_fixture.h"
#include "platform/iot_network_udp.h"
#include "platform/iot_threads.h"

/* UDP server */
#include "aws_test_tcp.h"
/*-----------------------------------------------------------*/

#define _TEST_UDP_SERVER_DNS_NAME    "pool.ntp.org"
#define _ECHO_STRING                 "hello world"
#define _ECHO_STRING_SIZE            sizeof( _ECHO_STRING )

/*-----------------------------------------------------------*/
static IotNetworkConnectionUdp_t * pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
/* setup network interface to be UDP interface */
static IotNetworkInterface_t const * pNetworkInterface = IOT_NETWORK_INTERFACE_UDP;
static IotNetworkServerInfoUdp_t udpServerInfo = IOT_NETWORK_SERVER_INFO_UDP_INITIALIZER;
static IotSemaphore_t testUdpAbstractionSem;
static bool testSemCreated = false;
/*-----------------------------------------------------------*/

static void prvUdpAddressHelper( IotNetworkServerInfoUdp_t * server,
                                 const char * address,
                                 const uint16_t port )
{
    TEST_ASSERT_NOT_EQUAL( NULL, server );
    TEST_ASSERT_NOT_EQUAL( NULL, address );

    server->pHostName = address;
    server->port = port;
}
/*-----------------------------------------------------------*/

/**
 * @brief Test group for  UDP abstraction layer tests.
 */
TEST_GROUP( UTIL_Platform_Network_UDP );

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/**
 * @brief Test setup for UDP abstraction layer tests.
 */
TEST_SETUP( UTIL_Platform_Network_UDP )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for udp abstraction layer tests.
 */
TEST_TEAR_DOWN( UTIL_Platform_Network_UDP )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;

    if( TEST_PROTECT() )
    {
        if( pUdpConnection != IOT_NETWORK_INVALID_CONNECTION )
        {
            networkError = pNetworkInterface->destroy( pUdpConnection );
            pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
            TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy failed." );
        }

        if( testSemCreated )
        {
            IotSemaphore_Destroy( &testUdpAbstractionSem );
            testSemCreated = false;
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Platform Threads tests.
 */
TEST_GROUP_RUNNER( UTIL_Platform_Network_UDP )
{
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_CreateConnectionInvalidParams );
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_CreateConnectionValidParams );
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_SyncSendReceive );
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncReceiveDestroy );
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncSendDestroy );
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncReceiveCloseAndDestroy );
    RUN_TEST_CASE( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncSendCloseAndDestroy );
}
/*-----------------------------------------------------------*/

/**
 * @brief check if connection creation fails with bad parameters
 */
TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_CreateConnectionInvalidParams )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;

    /* No Server address */
    networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation succeeded with bad params" );

    /* Invalid server address */
    udpServerInfo.pHostName = "";
    networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation succeeded with bad params" );

    /* create with connection other than IOT_NETWORK_INVALID_CONNECTION */
    pUdpConnection = ( IotNetworkConnectionUdp_t * ) 0xFFFFFFFF;
    udpServerInfo.pHostName = "";
    networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_BAD_PARAMETER, networkError, "UDP abstraction connection creation succeeded with bad params" );
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
}
/*-----------------------------------------------------------*/

/**
 * @brief check if connection creation succeeds with correct parameters
 */

TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_CreateConnectionValidParams )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    char serverAddress[ 16 ];
    uint32_t saddr = 0x00;

    if( TEST_PROTECT() )
    {
        /* IP */
        saddr = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                         udptestECHO_SERVER_ADDR1,
                                         udptestECHO_SERVER_ADDR2,
                                         udptestECHO_SERVER_ADDR3 );
        SOCKETS_inet_ntoa( saddr, serverAddress );
        prvUdpAddressHelper( &udpServerInfo, serverAddress, udptestECHO_PORT );
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );

        networkError = pNetworkInterface->destroy( pUdpConnection );
        pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy failed" );

        /* DNS */
        prvUdpAddressHelper( &udpServerInfo, _TEST_UDP_SERVER_DNS_NAME, udptestECHO_PORT );
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );
    }

    /* Close network connection */
    networkError = pNetworkInterface->destroy( pUdpConnection );
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
    TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy failed" );
}
/*-----------------------------------------------------------*/

/*
 * @brief Synchronous send and receive using network abstraction .
 */
TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_SyncSendReceive )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    uint32_t saddr = 0x00;
    char serverAddr[ 16 ] = { 0 };
    char * echoString = _ECHO_STRING;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };
    int32_t result = 0;

    if( TEST_PROTECT() )
    {
        /* Set up server information */
        saddr = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                         udptestECHO_SERVER_ADDR1,
                                         udptestECHO_SERVER_ADDR2,
                                         udptestECHO_SERVER_ADDR3 );
        SOCKETS_inet_ntoa( saddr, serverAddr );
        prvUdpAddressHelper( &udpServerInfo, serverAddr, udptestECHO_PORT );
        udpServerInfo.socketTimeout = 100;
        udpServerInfo.strictServerAddrCheck = false;

        /* Create connection to echo server */
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );

        /* Send string to echo server */
        result = pNetworkInterface->send( pUdpConnection, echoString, _ECHO_STRING_SIZE );
        TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction send  failed" );

        /* Receive data */
        result = pNetworkInterface->receive( pUdpConnection, returnString, sizeof( returnString ) );
        TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction receive  failed" );

        /* Make sure correct data is received */
        result = memcmp( echoString, returnString, _ECHO_STRING_SIZE );
        TEST_ASSERT_EQUAL_MESSAGE( 0, result, "UDP abstraction echo compare failed" );
    }

    /* Destroy connection */
    networkError = pNetworkInterface->destroy( pUdpConnection );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy connection failed" );
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
}
/*-----------------------------------------------------------*/

/* @brief Async receive callback handler. The handler destroys the connection */
void prvAsyncReceiveCallback( void * pConnection,
                              void * pContext )
{
    int32_t result = 0;
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    char * echoString = ( char * ) pContext;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };

    /* Receive data, use connection parameter */
    result = pNetworkInterface->receive( pConnection, returnString, sizeof( returnString ) );
    TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction receive  failed" );

    /* Make sure correct data is received */
    result = memcmp( echoString, returnString, _ECHO_STRING_SIZE );
    TEST_ASSERT_EQUAL_MESSAGE( 0, result, "UDP abstraction echo compare failed" );

    /* Destroy connection */
    networkError = pNetworkInterface->destroy( pConnection );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy  failed" );

    /* Post Semaphore */
    IotSemaphore_Post( &testUdpAbstractionSem );
}

/*
 * @brief Send and Recieve asynchronously using UDP network abstraction layer.
 * The receive handler destroys the connection.
 */
TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncReceiveDestroy )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    uint32_t saddr = 0x00;
    char serverAddr[ 16 ] = { 0 };
    bool status = true;

    char * echoString = _ECHO_STRING;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };
    int32_t result = 0;

    if( TEST_PROTECT() )
    {
        /* Create semaphore */
        status = IotSemaphore_Create( &testUdpAbstractionSem, 0, 1 );
        TEST_ASSERT_EQUAL_MESSAGE( true, status, "Failed to create semaphore." );
        testSemCreated = true;

        /* Set up server information */
        saddr = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                         udptestECHO_SERVER_ADDR1,
                                         udptestECHO_SERVER_ADDR2,
                                         udptestECHO_SERVER_ADDR3 );
        SOCKETS_inet_ntoa( saddr, serverAddr );
        prvUdpAddressHelper( &udpServerInfo, serverAddr, udptestECHO_PORT );
        udpServerInfo.socketTimeout = 100;
        udpServerInfo.strictServerAddrCheck = false;

        /* Create connection to echo server */
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );

        /* Set receive callback and set context as echo string */
        networkError = pNetworkInterface->setReceiveCallback( pUdpConnection, prvAsyncReceiveCallback, ( void * ) echoString );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction set receive callback failed" );

        /* Send string to echo server */
        result = pNetworkInterface->send( pUdpConnection, echoString, _ECHO_STRING_SIZE );
        TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction send  failed" );

        /* Wait on semaphore */
        TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
        IotSemaphore_Wait( &testUdpAbstractionSem );

        /* destroy semaphore */
        IotSemaphore_Destroy( &testUdpAbstractionSem );
        testSemCreated = false;
    }

    /* Receive handler destroys the connection */
    /* Set global to invalid to avoid test teardown to fail */
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
}
/*-----------------------------------------------------------*/

/* @brief Async Callback handler without destroy */
void prvAsyncReceiveCallbackNoDestroy( void * pConnection,
                                       void * pContext )
{
    int32_t result = 0;
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    char * echoString = ( char * ) pContext;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };

    /* Receive data, use connection parameter */
    result = pNetworkInterface->receive( pConnection, returnString, sizeof( returnString ) );

    /* make sure the size matches */
    TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction receive  failed" );
    /* Make sure correct data is received */
    result = memcmp( echoString, returnString, _ECHO_STRING_SIZE );
    TEST_ASSERT_EQUAL_MESSAGE( 0, result, "UDP abstraction echo compare failed" );

    /* Post Semaphore to indicate callback function is done */
    IotSemaphore_Post( &testUdpAbstractionSem );
}

/*
 * @brief Send and Recieve asynchronously using UDP network abstraction layer.
 * The sending thread destroys the connection. The receive handler does not destroy the connection.
 */
TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncSendDestroy )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    uint32_t saddr = 0x00;
    char serverAddr[ 16 ] = { 0 };

    char * echoString = _ECHO_STRING;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };
    int32_t result = 0;
    bool status = true;

    if( TEST_PROTECT() )
    {
        /* Create semaphore */
        status = IotSemaphore_Create( &testUdpAbstractionSem, 0, 1 );
        TEST_ASSERT_EQUAL_MESSAGE( true, status, "Failed to create semaphore." );
        testSemCreated = true;

        /* Set up server information */
        saddr = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                         udptestECHO_SERVER_ADDR1,
                                         udptestECHO_SERVER_ADDR2,
                                         udptestECHO_SERVER_ADDR3 );
        SOCKETS_inet_ntoa( saddr, serverAddr );
        prvUdpAddressHelper( &udpServerInfo, serverAddr, udptestECHO_PORT );
        udpServerInfo.socketTimeout = 100;
        udpServerInfo.strictServerAddrCheck = false;

        /* Create connection to echo server */
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );

        /* Set receive callback and set context as echo string for comparison in receive callback */
        networkError = pNetworkInterface->setReceiveCallback( pUdpConnection, prvAsyncReceiveCallbackNoDestroy, ( void * ) echoString );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction set receive callback failed" );

        /* Send string to echo server */
        result = pNetworkInterface->send( pUdpConnection, echoString, _ECHO_STRING_SIZE );
        TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction send  failed" );

        /* Destroy connection , destroy will block till receive thread exits */
        networkError = pNetworkInterface->destroy( pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy  failed" );

        /* Wait on semaphore for receive thread to finish */
        TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
        IotSemaphore_Wait( &testUdpAbstractionSem );

        /* destroy semaphore */
        IotSemaphore_Destroy( &testUdpAbstractionSem );
        testSemCreated = false;
    }

    /* Receive handler destroys the connection */
    /* Set global to invalid to avoid test teardown to fail */
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
}
/*-----------------------------------------------------------*/

/* @brief Async receive callback handler. The handler closes and then destroys the connection */
void prvAsyncReceiveCallbackCloseAndDestroy( void * pConnection,
                                             void * pContext )
{
    int32_t result = 0;
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    char * echoString = ( char * ) pContext;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };

    /* Receive data, use connection parameter */
    result = pNetworkInterface->receive( pConnection, returnString, sizeof( returnString ) );
    TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction receive  failed" );

    /* Make sure correct data is received */
    result = memcmp( echoString, returnString, _ECHO_STRING_SIZE );
    TEST_ASSERT_EQUAL_MESSAGE( 0, result, "UDP abstraction echo compare failed" );

    /* first close connection */
    networkError = pNetworkInterface->close( pConnection );

    /* Destroy connection */
    networkError = pNetworkInterface->destroy( pConnection );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy  failed" );

    /* Post Semaphore to unblock send task */
    IotSemaphore_Post( &testUdpAbstractionSem );
}

/*
 * @brief Send and Recieve asynchronously using UDP network abstraction layer.
 * The receive handler closes the connection and then destroys. The purpose of the test is to make sure
 * close + destoy works
 */
TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncReceiveCloseAndDestroy )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    uint32_t saddr = 0x00;
    char serverAddr[ 16 ] = { 0 };

    char * echoString = _ECHO_STRING;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };
    int32_t result = 0;
    bool status = true;

    if( TEST_PROTECT() )
    {
        /* Create semaphore */
        status = IotSemaphore_Create( &testUdpAbstractionSem, 0, 1 );
        TEST_ASSERT_EQUAL_MESSAGE( true, status, "Failed to create semaphore." );
        testSemCreated = true;

        /* Set up server information */
        saddr = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                         udptestECHO_SERVER_ADDR1,
                                         udptestECHO_SERVER_ADDR2,
                                         udptestECHO_SERVER_ADDR3 );
        SOCKETS_inet_ntoa( saddr, serverAddr );
        prvUdpAddressHelper( &udpServerInfo, serverAddr, udptestECHO_PORT );
        udpServerInfo.socketTimeout = 100;
        udpServerInfo.strictServerAddrCheck = false;

        /* Create connection to echo server */
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );

        /* Set receive callback and set context as echo string for comparison in receive callback */
        networkError = pNetworkInterface->setReceiveCallback( pUdpConnection, prvAsyncReceiveCallbackCloseAndDestroy, ( void * ) echoString );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction set receive callback failed" );

        /* Send string to echo server */
        result = pNetworkInterface->send( pUdpConnection, echoString, _ECHO_STRING_SIZE );
        TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction send  failed" );

        /* Wait on semaphore for receive thread to finish */
        TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
        IotSemaphore_Wait( &testUdpAbstractionSem );

        /* destroy semaphore */
        IotSemaphore_Destroy( &testUdpAbstractionSem );
        testSemCreated = false;
    }

    /* Receive handler destroys the connection */
    /* Set global to invalid to avoid test teardown to fail */
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
}
/*-----------------------------------------------------------*/


/*
 * @brief Send and Recieve asynchronously using UDP network abstraction layer.
 * The receive handler does not close or does not destroy the connection.
 * The sending thread closes and destroys the connection .
 * The purpose of the test is to make sure close + destoy works when called from
 * sending thread.
 */
TEST( UTIL_Platform_Network_UDP, IotUpdAbstraction_AsyncSendCloseAndDestroy )
{
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;
    uint32_t saddr = 0x00;
    char serverAddr[ 16 ] = { 0 };

    char * echoString = _ECHO_STRING;
    char returnString[ _ECHO_STRING_SIZE ] = { 0 };
    int32_t result = 0;
    bool status = true;

    if( TEST_PROTECT() )
    {
        /* Create semaphore */
        status = IotSemaphore_Create( &testUdpAbstractionSem, 0, 1 );
        TEST_ASSERT_EQUAL_MESSAGE( true, status, "Failed to create semaphore." );
        testSemCreated = true;

        /* Set up server information */
        saddr = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                         udptestECHO_SERVER_ADDR1,
                                         udptestECHO_SERVER_ADDR2,
                                         udptestECHO_SERVER_ADDR3 );
        SOCKETS_inet_ntoa( saddr, serverAddr );
        prvUdpAddressHelper( &udpServerInfo, serverAddr, udptestECHO_PORT );
        udpServerInfo.socketTimeout = 100;
        udpServerInfo.strictServerAddrCheck = false;

        /* Create connection to echo server */
        networkError = pNetworkInterface->create( ( void * ) &udpServerInfo, NULL, ( void ** ) &pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction connection creation failed" );

        /* Set receive callback and set context as echo string for comparison in receive callback */
        networkError = pNetworkInterface->setReceiveCallback( pUdpConnection, prvAsyncReceiveCallbackNoDestroy, ( void * ) echoString );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction set receive callback failed" );

        /* Send string to echo server */
        result = pNetworkInterface->send( pUdpConnection, echoString, _ECHO_STRING_SIZE );
        TEST_ASSERT_EQUAL_MESSAGE( _ECHO_STRING_SIZE, result, "UDP abstraction send failed" );

        /* Wait on semaphore for receive thread to finish */
        TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
        IotSemaphore_Wait( &testUdpAbstractionSem );

        /* close the connection, close should prompt  receive thread exit */
        networkError = pNetworkInterface->close( pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction close  failed" );

        networkError = pNetworkInterface->destroy( pUdpConnection );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_NETWORK_SUCCESS, networkError, "UDP abstraction destroy failed" );

        /* destroy semaphore */
        IotSemaphore_Destroy( &testUdpAbstractionSem );
        testSemCreated = false;
    }

    /* Receive handler destroys the connection */
    /* Set global to invalid to avoid test teardown to fail */
    pUdpConnection = IOT_NETWORK_INVALID_CONNECTION;
}
/*-----------------------------------------------------------*/
