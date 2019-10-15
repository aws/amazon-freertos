/*
 * Amazon FreeRTOS Secure Sockets V1.1.5
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* Secure Sockets includes. */
#include "iot_secure_sockets.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_runner.h"
#include "aws_test_utils.h"
#include "aws_test_tcp.h"

/* Update this file with port-specific values. */
#include "aws_test_tcp_config.h"

/* Update this file with AWS Credentials. */
#include "aws_clientcredential.h"

/* Verbose printing. */
#define udptestPRINTF( x )
/* In case of test failures, FAILUREPRINTF may provide more detailed information. */
#define udptestFAILUREPRINTF( x )    vLoggingPrintf x
/* Fail the test on tcptestASSERT. */
#define udptestASSERT( x )           configASSERT( x )
/* Take measurements of the FreeRTOS heap before and after every test. */
#define udptestMEMORYWATCH    0
/* Timeout for receivefrom in milliseconds */
#define udptestRECEIVE_TIMEOUT 500
/* The echo tasks create a socket, send out a number of echo requests, listen
 * for the echo reply, then close the socket again before starting over.  This
 * delay is used between each iteration to ensure the network does not get too
 * congested. */
#define udptestLOOP_DELAY_MS    ( ( uint32_t ) 150 )
#define udptestLOOP_DELAY       ( ( TickType_t ) udptestLOOP_DELAY_MS / portTICK_PERIOD_MS )


/* Filler values in the RX and TX buffers used to check for undesirable
 * buffer modification. */
#define udptestRX_BUFFER_FILLER    ( ( uint8_t ) 0xFF )
#define udptestTX_BUFFER_FILLER    ( ( uint8_t ) 0xAA )

/* Size of pcTxBuffer and pcRxBuffer. */
#define udptestBUFFER_SIZE         ( testrunnerBUFFER_SIZE / 2 )

/* Global buffers are shared between tests to reduce RAM usage. */
static char * pcTxBuffer = &cBuffer[ 0 ];
static char * pcRxBuffer = &cBuffer[ udptestBUFFER_SIZE ];

/* Primary socket, declared globally so that it can be closed in test tear down
 * in the event that a test exits/fails before socket is closed. */
static volatile Socket_t xSocket = SOCKETS_INVALID_SOCKET;
static volatile BaseType_t xSocketOpen = pdFALSE;

/* Max UPD payload as per MTU Size of IPv4 + UDP header is 20 */
#define udptest_MAX_FRAME_SIZE    ( ( uint32_t ) ( ipconfigNETWORK_MTU - 28 ) )

/*
 * Uses a socket to send  MTU bytes in one go.
 * The echoed data is received on the same socket but in a
 * different task (see prvEchoClientRxTask() below).
 */
static void prvEchoClientTxTask( void * pvParameters );

/* UDP Echo Client tasks multi-task test parameters. These can be configured in aws_test_tcp_config.h. */
#ifndef udptestUDP_ECHO_TASKS_STACK_SIZE
    #define udptestUDP_ECHO_TASKS_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#endif
#ifndef udptestUDP_ECHO_TASKS_PRIORITY
    #define udptestUDP_ECHO_TASKS_PRIORITY      ( tskIDLE_PRIORITY )
#endif

/* Number of time the test goes through all the modes as described in prvEchoClientTxTask(). */
#define udptestMAX_LOOPS_ECHO_TEST        ( 4 * udptestMAX_ECHO_TEST_MODES )

#define udptestECHO_TEST_LOW_PRIORITY     tskIDLE_PRIORITY
#define udptestECHO_TEST_HIGH_PRIORITY    ( configMAX_PRIORITIES - 1 )

/* static size_t xSyncLoop; */
static BaseType_t xSendSuccess;
static BaseType_t xRecvSuccess;

/* The queue used by prvEchoClientTxTask() to send the next socket to use to
 * prvEchoClientRxTask(). */
static volatile QueueHandle_t xSocketPassingQueue = NULL;

/* The event group used by the prvEchoClientTxTask() and prvEchoClientRxTask()
 * to synchronize prior to commencing a cycle using a new socket. */
static volatile EventGroupHandle_t xSyncEventGroup = NULL;

/* Bit definitions used with the xSyncEventGroup event group to allow the
 * prvEchoClientTxTask() and prvEchoClientRxTask() tasks to synchronize before
 * commencing a new cycle with a different socket. */
#define udptestTX_TASK_BIT    ( 0x01 << 1 )
#define udptestRX_TASK_BIT    ( 0x01 << 2 )

/* All the modes for the test  SOCKETS_Echo_Client_Separate_Tasks */
typedef enum
{
    LARGE_BUFFER_HIGH_PRIORITY,
    SMALL_BUFFER_HIGH_PRIORITY,
    LARGE_BUFFER_LOW_PRIORITY,
    SMALL_BUFFER_LOW_PRIORITY,
    udptestMAX_ECHO_TEST_MODES
} udptestEchoTestModes_t;

typedef struct
{
    uint16_t usTaskTag;
    BaseType_t xResult;
    TaskHandle_t xTaskHandle;
} udptestEchoClientsTaskParams_t;

/* Timeout value for event group sync  use for sync tasks */
#ifndef udptestECHO_TEST_SYNC_TIMEOUT
    #define udptestECHO_TEST_SYNC_TIMEOUT       ( ( uint32_t ) 80000 )
#endif

#define  udptestECHO_TEST_SYNC_TIMEOUT_TICKS    ( pdMS_TO_TICKS( udptestECHO_TEST_SYNC_TIMEOUT ) )

#define udptestNUM_ECHO_CLIENTS                 ( ( uint32_t ) 2 )
#define udptestMAX_LOOPS_ECHO_CLIENTS_LOOP      ( ( uint32_t ) 10 )
#define udptestECHO_CLIENT_EVENT_MASK           ( ( 1 << udptestNUM_ECHO_CLIENTS ) - 1 )


/* Array is defined as const so it can be located in flash. */
static const char cTransmittedString[ udptest_MAX_FRAME_SIZE ] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58, 59, 60, 61, 62, 63, 64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
    123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58, 59, 60, 61, 62, 63, 64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58, 59, 60, 61, 62, 63, 64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58, 59, 60, 61, 62, 63, 64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58, 59, 60, 61, 62, 63, 64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
    123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147
};

/* Group declaration required by Unity framework. */
TEST_GROUP( Full_UDP );

/* Setup required by Unity framework. */
TEST_SETUP( Full_UDP )
{
    #if ( udptestMEMORYWATCH == 1 )
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapB = xPortGetFreeHeapSize();
    #endif
}

/* Tear down required by Unity framework.
 * Closes the global socket if it was left open by the test. */
TEST_TEAR_DOWN( Full_UDP )
{
    int32_t lReturn;

    if( TEST_PROTECT() )
    {
        if( xSocketOpen == pdTRUE )
        {
            lReturn = SOCKETS_Close( xSocket );
            xSocketOpen = pdFALSE;
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, lReturn, "SOCKETS_Close failed in tear down. \r\n" );
        }
    }

    #if ( udptestMEMORYWATCH == 1 )
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapA = xPortGetFreeHeapSize();

        configPRINTF( ( "Heap before: %d, Heap After: %d \r\n", xHeapB, xHeapA ) );
    #endif
}

/*-----------------------------------------------------------*/

/*
 * @brief Creates a UDP Socket.
 * Causes a test failure if socket creation fails.
 */
static Socket_t prvUdpSocketHelper( volatile BaseType_t * pxSocketOpen )
{
    Socket_t xSock = SOCKETS_INVALID_SOCKET;

    TEST_ASSERT_EQUAL( pdFALSE, *pxSocketOpen );
    /* Make socket. */
    xSock = SOCKETS_Socket( SOCKETS_AF_INET,
                            SOCKETS_SOCK_DGRAM,
                            SOCKETS_IPPROTO_UDP );

    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSock, "Socket Creation Failed" );

    *pxSocketOpen = pdTRUE;

    return xSock;
}
/*-----------------------------------------------------------*/

/*
 * @brief Fills Socket address structure with UDP server address.
 */
static void prvUdpAddressHelper( SocketsSockaddr_t * pxHostAddress )
{
    /* Echo requests are sent to the echo server.  The echo server is
    * listening to udptestECHO_PORT on this computer's IP address. */
    pxHostAddress->ulAddress = SOCKETS_inet_addr_quick( udptestECHO_SERVER_ADDR0,
                                                        udptestECHO_SERVER_ADDR1,
                                                        udptestECHO_SERVER_ADDR2,
                                                        udptestECHO_SERVER_ADDR3 );
    pxHostAddress->usPort = SOCKETS_htons( udptestECHO_PORT );
    pxHostAddress->ucLength = sizeof( SocketsSockaddr_t );
    pxHostAddress->ucSocketDomain = SOCKETS_AF_INET;
}
/*-----------------------------------------------------------*/

/*
 * @brief Bind Socket address to socket.
 */
static void prvUdpBindHelper( Socket_t xSock,
                              SocketsSockaddr_t * pxHostAddress )
{
    BaseType_t xResult = pdPASS;

    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxHostAddress, " Host Address should be non NULL" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSock, " Uninitialized socket cannot be bound " );

    /* setup client address */
    pxHostAddress->ulAddress = 0;
    pxHostAddress->usPort = 0;
    pxHostAddress->ucLength = sizeof( SocketsSockaddr_t );
    pxHostAddress->ucSocketDomain = SOCKETS_AF_INET;

    xResult = SOCKETS_Bind( xSock, pxHostAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to bind" );
}
/*-----------------------------------------------------------*/

/*
 * @brief set receive timeout for the socket.
 */
static void prvUdpRecvTimeoutHelper( Socket_t xSock,
                                     TickType_t xTimeout )
{
    BaseType_t xResult;

    xResult = SOCKETS_SetSockOpt( xSock, 0, SOCKETS_SO_RCVTIMEO, &xTimeout, sizeof( TickType_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to set receive timeout" );
}

/*
 * @brief Send data to the host address, returns failure if send fails.
 */
static BaseType_t prvSendHelper( Socket_t xSocket,
                                 uint8_t * pucTxBuffer,
                                 size_t xLength,
                                 SocketsSockaddr_t * pxHostAddress )
{
    BaseType_t xNumBytesSentTotal;
    BaseType_t xNumBytes;
    BaseType_t xResult;

    xResult = pdPASS;
    xNumBytesSentTotal = 0;

    while( ( size_t ) xNumBytesSentTotal < xLength )
    {
        xNumBytes = SOCKETS_SendTo( xSocket,                            /* The socket being sent to. */
                                    &pucTxBuffer[ xNumBytesSentTotal ], /* The data being sent. */
                                    xLength - xNumBytesSentTotal,       /* The length of the data being sent. */
                                    0,                                  /* No flags. */
                                    pxHostAddress,                      /* Peer address*/
                                    sizeof( SocketsSockaddr_t ) );      /* Size of peer address */

        if( xNumBytes <= 0 )
        {
            xResult = pdFAIL;
            udptestFAILUREPRINTF( ( "Error in SOCKETS_SendTo.  Return value: %d \r\n", xNumBytes ) );
            break;
        }

        xNumBytesSentTotal += xNumBytes;
    }

    if( ( size_t ) xNumBytesSentTotal != xLength )
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/*
 * @brief Receive data from socket, returns failure if receive fails.
 */
static BaseType_t prvRecvHelper( Socket_t xSocket,
                                 uint8_t * pucRxBuffer,
                                 size_t xLength,
                                 SocketsSockaddr_t * pxHostAddress )
{
    BaseType_t xNumBytesRecvTotal = 0;
    BaseType_t xNumBytes = 0;
    BaseType_t xResult = pdPASS;
    Socklen_t xHostAdddrSize = 0;

    while( ( size_t ) xNumBytesRecvTotal < xLength )
    {
        xNumBytes = SOCKETS_RecvFrom( xSocket,
                                      &pucRxBuffer[ xNumBytesRecvTotal ],
                                      xLength - xNumBytesRecvTotal,
                                      0,
                                      pxHostAddress,
                                      &xHostAdddrSize );

        if( xNumBytes == 0 )
        {
            udptestFAILUREPRINTF( ( "Timed out receiving from echo server \r\n" ) );
            xResult = pdFAIL;
            break;
        }
        else if( xNumBytes < 0 )
        {
            udptestFAILUREPRINTF( ( "Error %d while receiving from echo server\r\n", xNumBytes ) );
            xResult = pdFAIL;
            break;
        }

        xNumBytesRecvTotal += xNumBytes;
    }

    if( ( size_t ) xNumBytesRecvTotal != xLength )
    {
        xResult = pdFAIL;
    }

    return xResult;
}
/*-----------------------------------------------------------*/

/* @brief close given socket returns error if close fails. */
static BaseType_t prvCloseHelper( Socket_t xSocket,
                                  volatile BaseType_t * pxSocketOpen )
{
    BaseType_t xResult;

    xResult = SOCKETS_Close( xSocket );

    *pxSocketOpen = pdFALSE;

    return xResult;
}
/*-----------------------------------------------------------*/

static BaseType_t prvCheckTimeout( TickType_t xStartTime,
                                   TickType_t xEndTime,
                                   TickType_t xTimeout )
{
    BaseType_t xResult = pdPASS;

    /* Did the function run at least as long as the timeout?
     * An "under tolerance" may be needed when the application timer
     * and the network timer are sourced from separate clocks. */
    if( ( int32_t ) ( xEndTime - xStartTime ) < ( int32_t ) ( xTimeout - integrationtestportableTIMEOUT_UNDER_TOLERANCE ) )
    {
        xResult = pdFAIL;
        udptestFAILUREPRINTF( ( "Start Time: %d, End Time: %d, Timeout: %d \n", xStartTime, xEndTime, xTimeout ) );
    }

    /* Did the function exit after a reasonable amount of time?
     * An "over tolerance" is used to allow for overhead surrounding the timeout. */
    if( ( xEndTime - xStartTime ) > ( xTimeout + integrationtestportableTIMEOUT_OVER_TOLERANCE ) )
    {
        xResult = pdFAIL;
        udptestFAILUREPRINTF( ( "Start Time: %d, End Time: %d, Timeout: %d \n", xStartTime, xEndTime, xTimeout ) );
    }

    return xResult;
}
/*-----------------------------------------------------------*/
static void prvCreateTxData( char * cBuffer,
                             size_t xMessageLength,
                             uint32_t ulTxCount )
{
    size_t xCharacter;
    char cChar = '0';

    /* Set the Tx buffer to a known filler value. */
    memset( cBuffer, udptestTX_BUFFER_FILLER, udptestBUFFER_SIZE );

    /* Fill the end of the buffer with ascending characters. */
    if( xMessageLength > 25 )
    {
        for( xCharacter = 0; xCharacter < xMessageLength; xCharacter++ )
        {
            cBuffer[ xCharacter ] = cChar;
            cChar++;

            if( cChar > '~' )
            {
                cChar = '0';
            }
        }
    }

    /* Write as much of the identifying string as possible to the buffer. */
    snprintf( cBuffer, xMessageLength, "%u TxRx with of length %u", ( unsigned ) ulTxCount, ( unsigned ) xMessageLength );
}

/*-----------------------------------------------------------*/

static BaseType_t prvCheckRxTxBuffers( uint8_t * pucTxBuffer,
                                       uint8_t * pucRxBuffer,
                                       size_t xMessageLength )
{
    BaseType_t xReturn;
    size_t xIndex;

    xReturn = pdPASS;

    /* Check that the message received is correct. */
    for( xIndex = 0; xIndex < xMessageLength; xIndex++ )
    {
        if( pucTxBuffer[ xIndex ] != pucRxBuffer[ xIndex ] )
        {
            xReturn = pdFAIL;
            udptestFAILUREPRINTF( ( "Message bytes received different than those sent. Message Length %d, Byte Index %d \r\n", xMessageLength, xIndex ) );
            break;
        }
    }

    if( xReturn == pdPASS )
    {
        /* Check that neither the Rx nor Tx buffers were modified/overflowed. */
        for( xIndex = xMessageLength; xIndex < udptestBUFFER_SIZE; xIndex++ )
        {
            if( pucRxBuffer[ xIndex ] != udptestRX_BUFFER_FILLER )
            {
                xReturn = pdFAIL;
                udptestFAILUREPRINTF( ( "Rx buffer padding modified byte. Message Length %d, Byte Index %d \r\n", xMessageLength, xIndex ) );
            }

            if( pucTxBuffer[ xIndex ] != udptestTX_BUFFER_FILLER )
            {
                xReturn = pdFAIL;
                udptestFAILUREPRINTF( ( "Tx buffer padding modified. Message Length %d, Byte Index %d \r\n", xMessageLength, xIndex ) );
            }
        }
    }

    /* udptestASSERT( xReturn == pdPASS ); */

    return xReturn;
}
/*-----------------------------------------------------------*/

/*
 * @brief Test SendTo and RecvFrom echo server. Return pdFAIL if one of the operation fails, allowing retry.
 * We expect the socket to not fail during retry.
 */
static BaseType_t prvSendRecvEchoTestHelper( Socket_t xSocket,
                                             size_t xLength,
                                             SocketsSockaddr_t * pxEchoServerAddress,
                                             uint32_t ulTxCount )
{
    BaseType_t xResult;
    BaseType_t xIntermResult;
    uint8_t * pucTxBuffer = ( uint8_t * ) pcTxBuffer;
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    SocketsSockaddr_t xReceiveAddress = { 0 };
    uint32_t ulRetry;

    xIntermResult = pdPASS;
    xResult = pdFAIL;
    ulRetry = 0;

    memset( pucTxBuffer, udptestTX_BUFFER_FILLER, udptestBUFFER_SIZE );

    prvCreateTxData( ( char * ) pucTxBuffer,
                     xLength,
                     ulTxCount );

    for( ; ulRetry < udptestMAX_RETRY; ulRetry++ )
    {
        xIntermResult = prvSendHelper( xSocket,
                                       pucTxBuffer,
                                       xLength,
                                       pxEchoServerAddress );

        if( xIntermResult != pdPASS )
        {
            udptestPRINTF( ( "Tx %d data failed to send on try %d\r\n", ulTxCount, ulRetry ) );
            continue;
        }

        memset( pucRxBuffer, udptestRX_BUFFER_FILLER, udptestBUFFER_SIZE );
        xIntermResult = prvRecvHelper( xSocket,
                                       pucRxBuffer,
                                       xLength,
                                       &xReceiveAddress );

        if( xIntermResult != pdPASS )
        {
            udptestPRINTF( ( "Rx %d data was not received on try %d\r\n", ulTxCount, ulRetry ) );
            continue;
        }

        xIntermResult = prvCheckRxTxBuffers( pucTxBuffer,
                                             pucRxBuffer,
                                             xLength );

        if( xIntermResult != pdPASS )
        {
            udptestPRINTF( ( "Rx %d data not match with Tx data on try %d\r\n", ulTxCount, ulRetry ) );
            continue;
        }

        if( pxEchoServerAddress->ulAddress == xReceiveAddress.ulAddress )
        {
            xResult = pdPASS;
            break;
        }
    }

    return xResult;
}
/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_UDP )
{
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_CloseInvalidParams );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_CloseWithoutReceiving );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_BindToAddressZero );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_BindToInvalidSocket );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_BindToNonZeroPort );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_BindToNullAddress );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_BindToTheSamePort );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_SetSockOpt_RCVTIMEO );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_SendToInvalidParams );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_RecvFromInvalidParams );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_SendRecv_VaryLength );
    // RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_RecvFromWithPeek );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_Threadsafe_SameSocketDifferentTasks );
    RUN_TEST_CASE( Full_UDP, AFQP_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks );
}
/*-----------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*-----------------------Begin Tests---------------------------------*/
/*-------------------------------------------------------------------*/

static void prvSOCKETS_CloseInvalidParams()
{
    BaseType_t xResult;

    /* Try to close with an invalid socket. */
    xResult = SOCKETS_Close( SOCKETS_INVALID_SOCKET );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_EINVAL, xResult, "Socket failed to close" );
}


TEST( Full_UDP, AFQP_SOCKETS_CloseInvalidParams )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_CloseInvalidParams();
}
/*-----------------------------------------------------------*/

static void prvSOCKETS_CloseWithoutReceiving()
{
    BaseType_t xResult;
    SocketsSockaddr_t xEchoServerAddress;

    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );

    prvUdpAddressHelper( &xEchoServerAddress );

    /* Send a lot of data to the echo server but never use the recv. */
    xResult = SOCKETS_SendTo( xSocket, &cTransmittedString, udptest_MAX_FRAME_SIZE, 0, &xEchoServerAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_GREATER_THAN_MESSAGE( 0, xResult, "Socket was not able to send" );

    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}


TEST( Full_UDP, AFQP_SOCKETS_CloseWithoutReceiving )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_CloseWithoutReceiving();
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_BindToAddressZero )
{
    BaseType_t xResult;
    SocketsSockaddr_t xClientAddress = { 0 };

    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );

    xResult = SOCKETS_Bind( xSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to bind" );

    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_BindToNonZeroPort )
{
    BaseType_t xResult;
    SocketsSockaddr_t xClientAddress = { 0 };

    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );

    xClientAddress.usPort = 123;
    xClientAddress.ucSocketDomain = SOCKETS_AF_INET;
    xClientAddress.ucLength = sizeof( SocketsSockaddr_t );

    xResult = SOCKETS_Bind( xSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to bind" );

    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_BindToNullAddress )
{
    BaseType_t xResult;
    SocketsSockaddr_t * pxClientAddress = NULL;

    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );

    xResult = SOCKETS_Bind( xSocket, pxClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_SOCKET_ERROR, xResult, "Socket failed to bind" );

    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_BindToTheSamePort )
{
    BaseType_t xResult;
    SocketsSockaddr_t xClientAddress = { 0 };
    Socket_t xtempSocket = SOCKETS_INVALID_SOCKET;

    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );

    xClientAddress.usPort = 123;
    xClientAddress.ucSocketDomain = SOCKETS_AF_INET;
    xClientAddress.ucLength = sizeof( SocketsSockaddr_t );

    xResult = SOCKETS_Bind( xSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to bind" );

    /* Create another socket */
    xtempSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  SOCKETS_SOCK_DGRAM,
                                  SOCKETS_IPPROTO_UDP );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xtempSocket, "Failed to create socket" );

    /* Bind to the same address */
    xResult = SOCKETS_Bind( xtempSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    /* Make sure bind fails */
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket bound to the same port" );

    /* close open sockets */
    xResult = SOCKETS_Close( xtempSocket );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_BindToInvalidSocket )
{
    BaseType_t xResult;
    SocketsSockaddr_t xClientAddress = { 0 };

    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = SOCKETS_INVALID_SOCKET;
    xResult = SOCKETS_Bind( xSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Invalid Socket bound" );
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_SendToInvalidParams )
{
    BaseType_t xResult;
    SocketsSockaddr_t xServerAddress = { 0 };
    uint8_t ucBuffer;

    /* Note: Without bind, sendTo automatically binds so that's not a failure */

    /* SendTo  with invalid socket */
    xSocket = SOCKETS_INVALID_SOCKET;
    xResult = SOCKETS_SendTo( xSocket, &ucBuffer, 1, 0, &xServerAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Sendto succeeded on an invalid Socket" );

    /* Send to invalid address */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFALSE, xSocketOpen, "Socket was already open" );
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );
    xResult = SOCKETS_SendTo( xSocket, &ucBuffer, 1, 0, &xServerAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Sendto succeeded on an invalid address" );
    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Send with invalid address length */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFALSE, xSocketOpen, "Socket was already open" );
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );
    prvUdpAddressHelper( &xServerAddress );
    /* Call sendto with incorrect address length */
    xResult = SOCKETS_SendTo( xSocket, &ucBuffer, 1, 0, &xServerAddress, 5 );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Sendto succeeded for invalid address length" );
    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}
/*-----------------------------------------------------------*/

TEST( Full_UDP, AFQP_SOCKETS_RecvFromInvalidParams )
{
    BaseType_t xResult;
    SocketsSockaddr_t xServerAddress = { 0 };
    SocketsSockaddr_t xClientAddress = { 0 };
    uint8_t ucBuffer;
    uint32_t ulAddressLength = 0;

    /* Note: Without bind, sendTo automatically binds so that's not a failure */

    /* RecvFrom with invalid socket */
    xSocket = SOCKETS_INVALID_SOCKET;
    xResult = SOCKETS_RecvFrom( xSocket, &ucBuffer, 1, 0, &xServerAddress, &ulAddressLength );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "RecvFrom succeeded on an invalid Socket" );

    /* Receive from  socket with address not bound */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFALSE, xSocketOpen, "Socket was already open" );
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    prvUdpRecvTimeoutHelper(xSocket, udptestRECEIVE_TIMEOUT);
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );
    xResult = SOCKETS_RecvFrom( xSocket, &ucBuffer, 1, 0, &xServerAddress, &ulAddressLength );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "RecvFrom succeeded on socket that was not bound" );
    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Receive from invalid address bound */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFALSE, xSocketOpen, "Socket was already open" );
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    prvUdpRecvTimeoutHelper(xSocket, udptestRECEIVE_TIMEOUT);
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );
    /* bind */
    xResult = SOCKETS_Bind( xSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    xResult = SOCKETS_RecvFrom( xSocket, &ucBuffer, 1, 0, &xServerAddress, &ulAddressLength );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "RecvFrom succeeded on socket bound to an invalid address" );
    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Receive from  closed socket */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFALSE, xSocketOpen, "Socket was already open" );
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    prvUdpRecvTimeoutHelper(xSocket, udptestRECEIVE_TIMEOUT);
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );
    /* close the socket */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    /* Set up valid server address */
    prvUdpAddressHelper( &xServerAddress );
    xResult = SOCKETS_RecvFrom( xSocket, &ucBuffer, 1, 0, &xServerAddress, &ulAddressLength );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "RecvFrom succeeded on closed Socket" );
}
/*-----------------------------------------------------------*/

static void prvSOCKETS_SetSockOpt_RCVTIMEO()
{
    BaseType_t xResult;
    UBaseType_t uxOldPriority;
    TickType_t xStartTime;
    TickType_t xEndTime;
    TickType_t xTimeout;
    TickType_t xTimeouts[] = { 30, 100, 10000 }; /* TODO: Add 0, nonblocking tests */
    uint8_t ucBuffer;
    BaseType_t xIndex;
    SocketsSockaddr_t xEchoServerAddress = { 0 };
    SocketsSockaddr_t xClientAddress = { 0 };
    uint32_t ulAddressLength = 0;


    xResult = pdPASS;
    udptestPRINTF( ( "Starting %s \r\n", __FUNCTION__ ) );
    udptestPRINTF( ( "This tests timeouts, so it takes a while! \r\n" ) );

    /* Create UDP Socket */
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xSocketOpen, "Failed to create socket" );

    /* Bind Socket to address zero */
    xClientAddress.ucLength = sizeof( SocketsSockaddr_t );
    xClientAddress.ucSocketDomain = SOCKETS_AF_INET;
    xResult = SOCKETS_Bind( xSocket, &xClientAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Record the priority of this task. */
    uxOldPriority = uxTaskPriorityGet( NULL );

    /*
     * Set this task to be the highest possible priority.
     * Since we are testing timeouts, we don't want other tasks
     * running in the middle of it.
     */
    vTaskPrioritySet( NULL, configMAX_PRIORITIES - 1 );

    /*
     * Set the receive timeout, check the tick count,
     * call receive (with no data sent), check the tick
     * count again, make sure time elapsed is within
     * time expected.
     */
    for( xIndex = 0; xIndex < sizeof( xTimeouts ) / sizeof( TickType_t ); xIndex++ )
    {
        xTimeout = pdMS_TO_TICKS( xTimeouts[ xIndex ] );
        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_RCVTIMEO, &xTimeout, sizeof( TickType_t ) );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to set receive timeout" );

        xStartTime = xTaskGetTickCount();
        xResult = SOCKETS_RecvFrom( xSocket, &ucBuffer, 1, 0, &xEchoServerAddress, &ulAddressLength );
        xEndTime = xTaskGetTickCount();
        TEST_ASSERT_LESS_THAN_MESSAGE( 1, xResult, "Receive call failed in receive timeout test" );
        xResult = prvCheckTimeout( xStartTime, xEndTime, xTimeout );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Receive timeout was outside of acceptable range" );
    }

    /* Restore the old priority of this task. */
    vTaskPrioritySet( NULL, uxOldPriority );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    udptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}


TEST( Full_UDP, AFQP_SOCKETS_SetSockOpt_RCVTIMEO )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SetSockOpt_RCVTIMEO();
}

/*-----------------------------------------------------------*/

static void prvSOCKETS_SendRecv_VaryLength()
{
    BaseType_t xResult;
    uint32_t ulIndex;
    uint32_t ulTxCount;
    const uint32_t ulMaxLoopCount = 10;
    uint32_t ulI;
    uint8_t * pucTxBuffer = ( uint8_t * ) pcTxBuffer;
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    size_t xMessageLengths[] = { 1, 2, 7, 8, 9, 100, 1000, udptest_MAX_FRAME_SIZE };
    SocketsSockaddr_t xEchoServerAddress = { 0 };
    SocketsSockaddr_t xReceiveAddress = { 0 };
    SocketsSockaddr_t xLocalAddress = { 0 };

    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xResult = pdPASS;
    ulTxCount = 0;

    for( ulIndex = 0; ulIndex < sizeof( xMessageLengths ) / sizeof( size_t ); ulIndex++ )
    {
        /* Initialize socket */
        xSocket = prvUdpSocketHelper( &xSocketOpen );
        prvUdpAddressHelper( &xEchoServerAddress );
        prvUdpBindHelper( xSocket, &xLocalAddress );
        /* setup 100MS timeout */
        prvUdpRecvTimeoutHelper( xSocket, udptestRECEIVE_TIMEOUT );

        /* Send each message length ulMaxLoopCount times. */
        for( ulI = 0; ulI < ulMaxLoopCount; ulI++ )
        {
            memset( pucTxBuffer, udptestTX_BUFFER_FILLER, udptestBUFFER_SIZE );

            prvCreateTxData( ( char * ) pucTxBuffer,
                             xMessageLengths[ ulIndex ],
                             ulTxCount );
            ulTxCount++;

            xResult = prvSendHelper( xSocket,
                                     pucTxBuffer,
                                     xMessageLengths[ ulIndex ],
                                     &xEchoServerAddress );

            TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data failed to send\r\n" );
            memset( pucRxBuffer, udptestRX_BUFFER_FILLER, udptestBUFFER_SIZE );
            xResult = prvRecvHelper( xSocket,
                                     pucRxBuffer,
                                     xMessageLengths[ ulIndex ],
                                     &xReceiveAddress );

            TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data was not received \r\n" );
            xResult = prvCheckRxTxBuffers( pucTxBuffer,
                                           pucRxBuffer,
                                           xMessageLengths[ ulIndex ] );
            TEST_ASSERT_EQUAL( xEchoServerAddress.ulAddress, xReceiveAddress.ulAddress );
        }

        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

        /* Print feedback since this test takes a while! */
        udptestPRINTF( ( " Sending messages with length %d complete\r\n", xMessageLengths[ ulIndex ] ) );

        /* Pause for a short while to ensure the network is not too
         * congested. */
        vTaskDelay( udptestLOOP_DELAY );
    }

    /* Report Test Results. */
    udptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}

TEST( Full_UDP, AFQP_SOCKETS_SendRecv_VaryLength )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SendRecv_VaryLength();
}


/*-----------------------------------------------------------*/

static void prvEchoClientTxTask( void * pvParameters )
{
    BaseType_t xReturned = 0;
    BaseType_t xTransmitted;
    BaseType_t xStatus;
    size_t xLenToSend, xSendLoop;
    udptestEchoTestModes_t xMode;
    SocketsSockaddr_t xEchoServerAddress = { 0 };
    Socklen_t xSocketAddrLen = 0;

    /* Avoid warning about unused parameter. */
    ( void ) pvParameters;

    prvUdpAddressHelper( &xEchoServerAddress );

    for( xSendLoop = 0; xSendLoop < udptestMAX_LOOPS_ECHO_TEST; )
    {
        xMode = ( udptestEchoTestModes_t ) ( xSendLoop % udptestMAX_ECHO_TEST_MODES ); /* % should be optimized to simple masking since only 4 modes are present.*/
        /* Using % to avoid bug in case a new state is unknowingly added. */

        vTaskPrioritySet( NULL, udptestECHO_TEST_HIGH_PRIORITY );
        xLenToSend = udptest_MAX_FRAME_SIZE;

        /* Set low priority if requested . */
        if( ( xMode == LARGE_BUFFER_LOW_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
        {
            vTaskPrioritySet( NULL, udptestECHO_TEST_LOW_PRIORITY );
        }

        /* Set buffer size to 1 if requested. */
        if( ( xMode == SMALL_BUFFER_HIGH_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
        {
            xLenToSend = 10;
        }

        /* Wait for the Rx task to create the socket. */
        if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                             udptestTX_TASK_BIT,                          /* The bit representing the Tx task reaching the rendezvous. */
                             ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ), /* Also wait for the Rx task. */
                             udptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ) )
        {
            break;
        }

        xTransmitted = 0;
        xSendSuccess = pdFALSE;
        xStatus = pdTRUE;

        xReturned = SOCKETS_SendTo( xSocket,                                            /* The socket being sent to. */
                                    ( void * ) &( cTransmittedString[ xTransmitted ] ), /* The data being sent. */
                                    xLenToSend,                                         /* The length of the data being sent. */
                                    0,                                                  /* ulFlags. */
                                    &xEchoServerAddress,
                                    xSocketAddrLen );

        if( xReturned <= 0 )
        {
            /* xStatus = pdFAIL; */
            udptestPRINTF( ( "SendTo Error: code %d\r\n", xReturned ) );
        }
        else
        {
            xSendSuccess = pdTRUE;
        }

        /* Wait for the Rx task to recognize the socket is closing and stop
         * using it. */

        if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                             udptestTX_TASK_BIT,                          /* The bit representing the Tx task reaching the rendezvous. */
                             ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ), /* Also wait for the Rx task. */
                             udptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ) )
        {
            break;
        }

        if( ( xRecvSuccess == pdTRUE ) && ( xSendSuccess == pdTRUE ) )
        {
            xSendLoop++;
        }
    }

    vTaskSuspend( NULL ); /* Delete this task. */
}
/*-----------------------------------------------------------*/

/**
 * @brief This test will create a task that will send data to an echo server.
 * The data coming back will be received in the parent task.
 * The size of receiving buffer, the priority, the size of data send,
 * will keep changing to cover a maximum cases.
 */
static void prvSOCKETS_Threadsafe_SameSocketDifferentTasks()
{
    BaseType_t xTotalReceived, xReturned = 0;
    size_t xRecvLoop = 0, xRecvLen = 0;
    udptestEchoTestModes_t xMode;
    BaseType_t xResult = pdFALSE;
    volatile char * pcReceivedString;
    volatile BaseType_t xReceivedStringAllocated = pdFALSE;
    volatile BaseType_t xSocketPassingQueueAllocated = pdFALSE;
    volatile BaseType_t xSyncEventGroupAllocated = pdFALSE;
    volatile TaskHandle_t xCreatedTask;
    SocketsSockaddr_t xEchoServerAddress = { 0 };
    SocketsSockaddr_t xClientAddress = { 0 };
    Socklen_t xServerAddrLen = 0;
    uint32_t ulRetry = 0;

    if( TEST_PROTECT() )
    {
        pcReceivedString = pvPortMalloc( udptest_MAX_FRAME_SIZE * sizeof( char ) );
        configASSERT( pcReceivedString != NULL );
        xReceivedStringAllocated = pdTRUE;

        /* Create the event group used by the Tx and Rx tasks to synchronize prior
         * to commencing a cycle using a new socket. */
        xSyncEventGroup = xEventGroupCreate();
        configASSERT( xSyncEventGroup );
        xSyncEventGroupAllocated = pdTRUE;

        /* Create the task that sends to an echo server, but lets a different task
         * receive the reply on the same socket. */
        xResult = xTaskCreate( prvEchoClientTxTask,              /* The function that implements the task. */
                               "EchoMultiTx",                    /* Just a text name for the task to aid debugging. */
                               udptestUDP_ECHO_TASKS_STACK_SIZE, /* The stack size is defined in aws_demo_config.h. */
                               NULL,                             /* The task parameter, not used in this case. */
                               udptestUDP_ECHO_TASKS_PRIORITY,   /* The priority assigned to the task is defined in aws_demo_config.h. */
                               ( TaskHandle_t * ) &xCreatedTask );
        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Task creation failed" );

        for( xRecvLoop = 0; xRecvLoop < udptestMAX_LOOPS_ECHO_TEST; )
        {
            xSocket = prvUdpSocketHelper( &xSocketOpen );
            TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xSocketOpen, "Socket Creation Failed" );
            prvUdpBindHelper( xSocket, &xClientAddress );
            prvUdpAddressHelper( &xEchoServerAddress );
            /* setup 100MS timeout */
            prvUdpRecvTimeoutHelper( xSocket, udptestRECEIVE_TIMEOUT);

            /* Nothing received yet. */
            xTotalReceived = 0;
            xRecvSuccess = pdFALSE;

            xMode = ( udptestEchoTestModes_t ) ( xRecvLoop % udptestMAX_ECHO_TEST_MODES );

            xRecvLen = udptest_MAX_FRAME_SIZE;
            vTaskPrioritySet( NULL, udptestECHO_TEST_HIGH_PRIORITY );

            /* Set low priority if requested . */
            if( ( xMode == LARGE_BUFFER_LOW_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
            {
                vTaskPrioritySet( NULL, udptestECHO_TEST_LOW_PRIORITY );
            }

            if( ( xMode == SMALL_BUFFER_HIGH_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
            {
                xRecvLen = 10;
            }

            /* Wait to receive the socket that will be used from the Tx task. */
            if( xEventGroupSync( xSyncEventGroup,                            /* The event group used for the rendezvous. */
                                 udptestRX_TASK_BIT,                         /* The bit representing the Tx task reaching the rendezvous. */
                                 ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ),/* Also wait for the Rx task. */
                                 udptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ) )
            {
                TEST_FAIL();
            }
            
            xReturned = SOCKETS_RecvFrom( ( Socket_t ) xSocket, ( char * ) pcReceivedString, xRecvLen, 0, &xEchoServerAddress, &xServerAddrLen );

            if( xReturned == 0 )
            {
                udptestPRINTF( ( "RecvFrom Error: Timeout on try %d\r\n", ulRetry ) );
            }

            else if( xReturned < 0 )
            {
                udptestPRINTF( ( "RecvFrom Error: code %d on try %d\r\n", xReturned, ulRetry ) );
            }
            else
            {
                TEST_ASSERT_EQUAL_MEMORY( &cTransmittedString[ xTotalReceived ], pcReceivedString, xReturned );
                xRecvSuccess = pdTRUE;

                xTotalReceived += xReturned;
            }

            /* Rendez-vous with the Tx task ready to start a new cycle with a
             * different socket. */
            if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                                 udptestRX_TASK_BIT,                          /* The bit representing the Rx task reaching the rendezvous. */
                                 ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ), /* Also wait for the Tx task. */
                                 udptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( udptestTX_TASK_BIT | udptestRX_TASK_BIT ) )
            {
                TEST_FAIL();
            }

            xResult = prvCloseHelper( xSocket, &xSocketOpen );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

            /* retry if one of the task failed. */
            if( ( xRecvSuccess == pdTRUE ) && ( xSendSuccess == pdTRUE ) )
            {
                ulRetry = 0;
                xRecvLoop++;
            }
            else
            {
                ulRetry++;

                if( ulRetry > udptestMAX_RETRY )
                {
                    TEST_FAIL();
                }
            }
        }
    }

    /* Free all dynamic memory. */
    if( xReceivedStringAllocated == pdTRUE )
    {
        vPortFree( ( char * ) pcReceivedString );
    }

    if( xSyncEventGroupAllocated == pdTRUE )
    {
        vEventGroupDelete( xSyncEventGroup );
    }

    vTaskDelete( xCreatedTask );

    /* Set priority back. */
    vTaskPrioritySet( NULL, tskIDLE_PRIORITY );
}

TEST( Full_UDP, AFQP_SOCKETS_Threadsafe_SameSocketDifferentTasks )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Threadsafe_SameSocketDifferentTasks();
}
/*-----------------------------------------------------------*/

/*
 * @brief Task worker function for sending data to echo server and verifying if it received
 * same number of bytes. Multiple tasks are started simultaneously to make
 */
static void prvThreadSafeDifferentSocketsDifferentTasks( void * pvParameters )
{
    BaseType_t xResult = pdFAIL;
    Socket_t xClientSocket = SOCKETS_INVALID_SOCKET;
    BaseType_t xClientSocketOpen = pdFALSE;
    SocketsSockaddr_t xEchoServerAddress = { 0 };
    SocketsSockaddr_t xClientAddress = { 0 };
    uint8_t * pucRxBuffer = NULL;
    uint16_t uSendLength = 255;
    udptestEchoClientsTaskParams_t * pxUdptestEchoClientsTaskParams;

    pxUdptestEchoClientsTaskParams = ( ( udptestEchoClientsTaskParams_t * ) pvParameters );

    if( TEST_PROTECT() )
    {
        /* Initialize socket */
        xClientSocket = prvUdpSocketHelper( &xClientSocketOpen );
        TEST_ASSERT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xClientSocket, "Invalid Socket Error" );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( pdFAIL, xClientSocketOpen, "Socket Allocation Failed" );

        prvUdpBindHelper( xClientSocket, &xClientAddress );
        /* setup 100MS timeout */
        prvUdpRecvTimeoutHelper( xClientSocket, udptestRECEIVE_TIMEOUT);

        prvUdpAddressHelper( &xEchoServerAddress );
        /* Send data to echo server */
        xResult = prvSendHelper( xClientSocket,
                                 ( void * ) &( cTransmittedString[ 0 ] ), /* The data being sent. */
                                 uSendLength,
                                 &xEchoServerAddress );
        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Send Helper Failed" );

        pucRxBuffer = pvPortMalloc( uSendLength * sizeof( uint8_t ) );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pucRxBuffer, "Memory Allocation Failed" );

        /* Receive data */
        xResult = prvRecvHelper( xClientSocket,
                                 pucRxBuffer,
                                 uSendLength,
                                 &xClientAddress );

        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data was not received \r\n" );

        pxUdptestEchoClientsTaskParams->xResult = SOCKETS_ERROR_NONE;
    }

    if( pucRxBuffer != NULL )
    {
        vPortFree( pucRxBuffer );
    }

    if( xClientSocketOpen != pdFALSE )
    {
        prvCloseHelper( xClientSocket, &xSocketOpen );
    }

    /* Don't wait, just flag it reached that point. */
    xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                     ( 1 << pxUdptestEchoClientsTaskParams->usTaskTag ),
                     udptestECHO_CLIENT_EVENT_MASK,
                     udptestECHO_TEST_SYNC_TIMEOUT_TICKS );

    vTaskDelete( NULL ); /* Delete this task. */
}

static void prvStartUDPEchoClientTasks_DifferentSockets()
{
    uint16_t usIndex;
    udptestEchoClientsTaskParams_t xUdptestEchoClientsTaskParams[ udptestNUM_ECHO_CLIENTS ];
    uint32_t ulEventMask;
    volatile BaseType_t xSyncEventGroupAllocated = pdFALSE;
    BaseType_t xResult;

    if( TEST_PROTECT() )
    {
        /* Create the event group used by the Tx and Rx tasks to synchronize prior
         * to commencing a cycle using a new socket. */
        xSyncEventGroup = xEventGroupCreate();
        configASSERT( xSyncEventGroup );
        xSyncEventGroupAllocated = pdTRUE;

        /* Create the echo client tasks. */
        for( usIndex = 0; usIndex < udptestNUM_ECHO_CLIENTS; usIndex++ )
        {
            xUdptestEchoClientsTaskParams[ usIndex ].usTaskTag = usIndex;
            xUdptestEchoClientsTaskParams[ usIndex ].xResult = SOCKETS_SOCKET_ERROR;

            xResult = xTaskCreate( prvThreadSafeDifferentSocketsDifferentTasks,                 /* The function that implements the task. */
                                   "EchoClientTask",                                            /* Just a text name for the task to aid debugging. */
                                   udptestUDP_ECHO_TASKS_STACK_SIZE,                            /* The stack size is defined in FreeRTOSIPConfig.h. */
                                   &( xUdptestEchoClientsTaskParams[ usIndex ] ),               /* The task parameter, not used in this case. */
                                   udptestUDP_ECHO_TASKS_PRIORITY,                              /* The priority assigned to the task is defined in FreeRTOSConfig.h. */
                                   &( xUdptestEchoClientsTaskParams[ usIndex ].xTaskHandle ) ); /* The task handle is not used. */
            TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Task creation failed" );
        }

        ulEventMask = xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                                       0,
                                       udptestECHO_CLIENT_EVENT_MASK,
                                       udptestECHO_TEST_SYNC_TIMEOUT_TICKS );

        /* For each task not completed, delete the task.  */
        for( usIndex = 0; usIndex < udptestNUM_ECHO_CLIENTS; usIndex++ )
        {
            if( ( ulEventMask & ( 1 << usIndex ) ) == 0 )
            {
                vTaskDelete( xUdptestEchoClientsTaskParams[ usIndex ].xTaskHandle );
            }

            TEST_ASSERT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE,
                                       xUdptestEchoClientsTaskParams[ usIndex ].xResult,
                                       "Check aws_secure_sockets.h for error code" );
        }
    }

    if( xSyncEventGroupAllocated == pdTRUE )
    {
        vEventGroupDelete( xSyncEventGroup );
    }
}


TEST( Full_UDP, AFQP_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvStartUDPEchoClientTasks_DifferentSockets();
}
/*-----------------------------------------------------------*/

static void prvSOCKETS_RecvFrom_Peek()
{
    BaseType_t xResult = pdFAIL;
    SocketsSockaddr_t xEchoServerAddress = { 0 };
    SocketsSockaddr_t xClientAddress = { 0 };
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    uint16_t uSendLength = 255;
    int32_t ulStatus = 0;
    uint32_t ulAddressLength;

    /* Initialize socket */
    xSocket = prvUdpSocketHelper( &xSocketOpen );
    prvUdpBindHelper( xSocket, &xClientAddress );
    /* setup 100MS timeout */
    prvUdpRecvTimeoutHelper( xSocket, udptestRECEIVE_TIMEOUT);
    prvUdpAddressHelper( &xEchoServerAddress );
    /* Send data to echo server */
    xResult = prvSendHelper( xSocket,
                             ( void * ) &( cTransmittedString[ 0 ] ), /* The data being sent. */
                             uSendLength,
                             &xEchoServerAddress );
    TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Send Helper Failed" );

    /* Call receive with peek to read 1 byte */
    ulStatus = SOCKETS_RecvFrom( xSocket,
                                 pucRxBuffer,
                                 1,
                                 SOCKETS_MSG_PEEK,
                                 &xClientAddress,
                                 &ulAddressLength );

    TEST_ASSERT_EQUAL_INT32_MESSAGE( ulStatus, 1, "Message Peek Failed" );

    /* make sure buffer can be still received after peek */
    xResult = prvRecvHelper( xSocket,
                             pucRxBuffer,
                             uSendLength,
                             &xClientAddress );

    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data was not received \r\n" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_UDP, AFQP_SOCKETS_RecvFromWithPeek )
{
    udptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_RecvFrom_Peek();
}
