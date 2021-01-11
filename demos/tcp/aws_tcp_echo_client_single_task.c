/*
 * FreeRTOS V202012.00
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

/*
 * A set of tasks are created that send TCP echo requests to the standard echo
 * port (port 7) on the IP address set by the configECHO_SERVER_ADDR0 to
 * configECHO_SERVER_ADDR3 constants, then wait for and verify the reply
 * (another demo is available that demonstrates the reception being performed in
 * a task other than that from with the request was made).
 *
 * See the following web page for essential demo usage and configuration
 * details:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html
 */

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* TCP/IP abstraction includes. */
#include "iot_secure_sockets.h"
#include "platform/iot_network.h"

/* Demo configuration */
#include "aws_demo_config.h"

/* Dimensions the buffer used to generate the task name. */
#define echoMAX_TASK_NAME_LENGTH        8

/* Maximum connection count. */
#define echoMAXIMUM_CONNECTION_COUNT    10

/* Maximum Loop count. */
#define echoMAX_LOOP_COUNT              10

/* The threshold to declare this demo as successful. Range: 0 - 1. */
#define echoSUCCESS_THRESHOLD           0.95

/* Sanity check the configuration constants required by this demo are
 * present. */
#if !defined( configECHO_SERVER_ADDR0 ) || !defined( configECHO_SERVER_ADDR1 ) || !defined( configECHO_SERVER_ADDR2 ) || !defined( configECHO_SERVER_ADDR3 )
    #error configECHO_SERVER_ADDR0 to configECHO_SERVER_ADDR3 must be defined in FreeRTOSConfig.h to specify the IP address of the echo server.
#endif

/* The echo tasks create a socket, send out a number of echo requests, listen
 * for the echo reply, then close the socket again before starting over.  This
 * delay is used between each iteration to ensure the network does not get too
 * congested. */
#define echoLOOP_DELAY    ( ( TickType_t ) 150 / portTICK_PERIOD_MS )

/* The echo server is assumed to be on port 7, which is the standard echo
 * protocol port. */
#ifndef configTCP_ECHO_CLIENT_PORT
    #define echoECHO_PORT    ( 7 )
#else
    #define echoECHO_PORT    ( configTCP_ECHO_CLIENT_PORT )
#endif

/* The size of the buffers is a multiple of the MSS - the length of the data
 * sent is a pseudo random size between 20 and echoBUFFER_SIZES. */
#define echoBUFFER_SIZE_MULTIPLIER    ( 2 )
#define echoBUFFER_SIZES              ( 2000 ) /*_RB_ Want to be a multiple of the MSS but there is no MSS constant in the bastraction. */

/* The number of instances of the echo client task to create. */
#define echoNUM_ECHO_CLIENTS          ( 1 )

/* If ipconfigUSE_TCP_WIN is 1 then the Tx socket will use a buffer size set by
 * ipconfigTCP_TX_BUFFER_LENGTH, and the Tx window size will be
 * configECHO_CLIENT_TX_WINDOW_SIZE times the buffer size.  Note
 * ipconfigTCP_TX_BUFFER_LENGTH is set in FreeRTOSIPConfig.h as it is a standard TCP/IP
 * stack constant, whereas configECHO_CLIENT_TX_WINDOW_SIZE is set in
 * FreeRTOSConfig.h as it is a demo application constant. */
#ifndef configECHO_CLIENT_TX_WINDOW_SIZE
    #define configECHO_CLIENT_TX_WINDOW_SIZE    2
#endif

/* If ipconfigUSE_TCP_WIN is 1 then the Rx socket will use a buffer size set by
 * ipconfigTCP_RX_BUFFER_LENGTH, and the Rx window size will be
 * configECHO_CLIENT_RX_WINDOW_SIZE times the buffer size.  Note
 * ipconfigTCP_RX_BUFFER_LENGTH is set in FreeRTOSIPConfig.h as it is a standard TCP/IP
 * stack constant, whereas configECHO_CLIENT_RX_WINDOW_SIZE is set in
 * FreeRTOSConfig.h as it is a demo application constant. */
#ifndef configECHO_CLIENT_RX_WINDOW_SIZE
    #define configECHO_CLIENT_RX_WINDOW_SIZE    2
#endif

/* The flag that turns on TLS for secure socket */
#define configTCP_ECHO_TASKS_SINGLE_TASK_TLS_ENABLED    ( 0 )

/*
 * PEM-encoded server certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#if ( configTCP_ECHO_TASKS_SINGLE_TASK_TLS_ENABLED == 1 )
    static const char cTlsECHO_SERVER_CERTIFICATE_PEM[] = "Paste the echo server certificate here.";
    static const uint32_t ulTlsECHO_SERVER_CERTIFICATE_LENGTH = sizeof( cTlsECHO_SERVER_CERTIFICATE_PEM );
#endif

/*-----------------------------------------------------------*/

/*
 * Uses a socket to send data to, then receive data from, the standard echo
 * port number 7.
 */
static void prvEchoClientTask( void * pvParameters );

/*
 * Creates a pseudo random sized buffer of data to send to the echo server.
 */
static BaseType_t prvCreateTxData( char * ucBuffer,
                                   uint32_t ulBufferLength );

/*-----------------------------------------------------------*/

/* Rx and Tx time outs are used to ensure the sockets do not wait too long for
 * missing data. */
static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 2000 );
static const TickType_t xSendTimeOut = pdMS_TO_TICKS( 2000 );

/* Counters for each created task - for inspection only. */
static uint32_t ulTxRxCycles[ echoNUM_ECHO_CLIENTS ] = { 0 },
                ulTxRxFailures[ echoNUM_ECHO_CLIENTS ] = { 0 },
                ulConnections[ echoNUM_ECHO_CLIENTS ] = { 0 };

/* Rx and Tx buffers for each created task. */
static char cTxBuffers[ echoNUM_ECHO_CLIENTS ][ echoBUFFER_SIZES ],
            cRxBuffers[ echoNUM_ECHO_CLIENTS ][ echoBUFFER_SIZES ];

/*-----------------------------------------------------------*/

/* Create a semaphore to sync all Echo task(s). */
static SemaphoreHandle_t EchoSingleSemaphore;
BaseType_t xSuccess[ echoNUM_ECHO_CLIENTS ];

int vStartTCPEchoClientTasks_SingleTasks( bool awsIotMqttMode,
                                          const char * pIdentifier,
                                          void * pNetworkServerInfo,
                                          void * pNetworkCredentialInfo,
                                          const IotNetworkInterface_t * pNetworkInterface )
{
    BaseType_t xX;
    BaseType_t TaskCompleteCounter, SuccessfulConnections = 0;
    char cNameBuffer[ echoMAX_TASK_NAME_LENGTH ];
    float SuccessPercent = 0;

    /* Unused parameters */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    TaskCompleteCounter = 0;
    EchoSingleSemaphore = xSemaphoreCreateCounting( echoNUM_ECHO_CLIENTS, 0 );

    /* Create the echo client tasks. */
    for( xX = 0; xX < echoNUM_ECHO_CLIENTS; xX++ )
    {
        snprintf( cNameBuffer, echoMAX_TASK_NAME_LENGTH, "Echo%ld", ( long int ) xX );
        xTaskCreate( prvEchoClientTask,        /* The function that implements the task. */
                     cNameBuffer,              /* Just a text name for the task to aid debugging. */
                     democonfigDEMO_STACKSIZE, /* The stack size is defined in FreeRTOSIPConfig.h. */
                     ( void * ) xX,            /* The task parameter, not used in this case. */
                     democonfigDEMO_PRIORITY,  /* The priority assigned to the task is defined in FreeRTOSConfig.h. */
                     NULL );                   /* The task handle is not used. */
    }

    /* Wait for all tasks to finish. */
    while( TaskCompleteCounter < echoNUM_ECHO_CLIENTS )
    {
        /* Wait for the semaphore to be 'given' by a child task. */
        xSemaphoreTake( EchoSingleSemaphore, portMAX_DELAY );

        /* Increment the task completion counter variable. */
        TaskCompleteCounter++;
    }

    xX = 0;

    /* Count the number of successes. */
    while( xX < echoNUM_ECHO_CLIENTS )
    {
        SuccessfulConnections += xSuccess[ xX++ ];
    }

    /* Calculate the percentage of successful connections across all connections. */
    SuccessPercent = ( ( float ) SuccessfulConnections / ( echoNUM_ECHO_CLIENTS * echoMAX_LOOP_COUNT * echoMAXIMUM_CONNECTION_COUNT ) );

    if( SuccessPercent > echoSUCCESS_THRESHOLD )
    {
        /* Number of successful connections more than threshold. Return Success. */
        return EXIT_SUCCESS;
    }
    else
    {
        /* Number of successful connections less than threshold. Return Failure. */
        return EXIT_FAILURE;
    }
}
/*-----------------------------------------------------------*/

static void prvEchoClientTask( void * pvParameters )
{
    Socket_t xSocket;
/*_RB_ struct convention is for this not to be typedef'ed, so 'struct' is required. */ SocketsSockaddr_t xEchoServerAddress;
    int32_t lLoopCount = 0UL;
    const int32_t lMaxLoopCount = echoMAX_LOOP_COUNT;
    volatile uint32_t ulTxCount = 0UL;
    BaseType_t xReceivedBytes, xReturned, xInstance;
    BaseType_t xTransmitted, xStringLength;
    char * pcTransmittedString;
    char * pcReceivedString;
    TickType_t xTimeOnEntering;
    BaseType_t lConnectionCount;
    const BaseType_t lMaxConnectionCount = echoMAXIMUM_CONNECTION_COUNT;

    #if ( ipconfigUSE_TCP_WIN == 1 )
        WinProperties_t xWinProps;
    #endif

    #if ( ipconfigUSE_TCP_WIN == 1 )
        {
            /* Fill in the buffer and window sizes that will be used by the socket. */
            xWinProps.lTxBufSize = ipconfigTCP_TX_BUFFER_LENGTH;
            xWinProps.lTxWinSize = configECHO_CLIENT_TX_WINDOW_SIZE;
            xWinProps.lRxBufSize = ipconfigTCP_RX_BUFFER_LENGTH;
            xWinProps.lRxWinSize = configECHO_CLIENT_RX_WINDOW_SIZE;
        }
    #endif /* ipconfigUSE_TCP_WIN */

    /* This task can be created a number of times.  Each instance is numbered
     * to enable each instance to use a different Rx and Tx buffer.  The number is
     * passed in as the task's parameter. */
    xInstance = ( BaseType_t ) pvParameters;

    /* Point to the buffers to be used by this instance of this task. */
    pcTransmittedString = &( cTxBuffers[ xInstance ][ 0 ] );
    pcReceivedString = &( cRxBuffers[ xInstance ][ 0 ] );

    /* Echo requests are sent to the echo server.  The address of the echo
     * server is configured by the constants configECHO_SERVER_ADDR0 to
     * configECHO_SERVER_ADDR3 in FreeRTOSConfig.h. */
    xEchoServerAddress.usPort = SOCKETS_htons( echoECHO_PORT );
    xEchoServerAddress.ulAddress = SOCKETS_inet_addr_quick( configECHO_SERVER_ADDR0,
                                                            configECHO_SERVER_ADDR1,
                                                            configECHO_SERVER_ADDR2,
                                                            configECHO_SERVER_ADDR3 );

    /* Create lMaxConnectionCount distinct connections to the echo server. */
    for( lConnectionCount = 0; lConnectionCount < lMaxConnectionCount; lConnectionCount++ )
    {
        /* Create a TCP socket. */
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
        configASSERT( xSocket != SOCKETS_INVALID_SOCKET );

        /* Set a time out so a missing reply does not cause the task to block
         * indefinitely. */
        SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
        SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

        #if ( ipconfigUSE_TCP_WIN == 1 )
            {
                /* Set the window and buffer sizes. */
                SOCKETS_SetSockOpt( xSocket, 0, FREERTOS_SO_WIN_PROPERTIES, ( void * ) &xWinProps, sizeof( xWinProps ) );
            }
        #endif /* ipconfigUSE_TCP_WIN */

        #if ( configTCP_ECHO_TASKS_SINGLE_TASK_TLS_ENABLED == 1 )
            {
                /* Set the socket to use TLS. */
                SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_REQUIRE_TLS, NULL, ( size_t ) 0 );
                SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE, cTlsECHO_SERVER_CERTIFICATE_PEM, ulTlsECHO_SERVER_CERTIFICATE_LENGTH );
            }
        #endif /* configTCP_ECHO_TASKS_SINGLE_TASK_TLS_ENABLED */

        /* Connect to the echo server. */
        configPRINTF( ( "Connecting to echo server\r\n" ) );

        if( SOCKETS_Connect( xSocket, &xEchoServerAddress, sizeof( xEchoServerAddress ) ) == 0 )
        {
            configPRINTF( ( "Connected to echo server\r\n" ) );
            ulConnections[ xInstance ]++;

            /* Send a number of echo requests. */
            for( lLoopCount = 0; lLoopCount < lMaxLoopCount; lLoopCount++ )
            {
                /* Create the string that is sent to the echo server. */
                xStringLength = prvCreateTxData( pcTransmittedString, echoBUFFER_SIZES );

                /* Add in some unique text at the front of the string. */
                sprintf( pcTransmittedString, "TxRx message number %u", ( unsigned ) ulTxCount );
                ulTxCount++;

                /* Send the string to the socket. */
                xTransmitted = SOCKETS_Send( xSocket,                        /* The socket being sent to. */
                                             ( void * ) pcTransmittedString, /* The data being sent. */
                                             xStringLength,                  /* The length of the data being sent. */
                                             0 );                            /* No flags. */

                configPRINTF( ( "Sending %s of length %d to echo server\r\n", pcTransmittedString, xStringLength ) );

                if( xTransmitted < 0 )
                {
                    /* Error? */
                    configPRINTF( ( "ERROR - Failed to send to echo server\r\n", pcTransmittedString ) );
                    break;
                }

                /* Clear the buffer into which the echoed string will be
                 * placed. */
                memset( ( void * ) pcReceivedString, 0x00, echoBUFFER_SIZES );
                xReceivedBytes = 0;

                /* Receive data echoed back to the socket. */
                while( xReceivedBytes < xTransmitted )
                {
                    xReturned = SOCKETS_Recv( xSocket,                                 /* The socket being received from. */
                                              &( pcReceivedString[ xReceivedBytes ] ), /* The buffer into which the received data will be written. */
                                              xStringLength - xReceivedBytes,          /* The size of the buffer provided to receive the data. */
                                              0 );                                     /* No flags. */

                    if( xReturned < 0 )
                    {
                        /* Error occurred.  Latch it so it can be detected
                         * below. */
                        xReceivedBytes = xReturned;
                        break;
                    }
                    else if( xReturned == 0 )
                    {
                        /* Timed out. */
                        configPRINTF( ( "Timed out receiving from echo server\r\n" ) );
                        break;
                    }
                    else
                    {
                        /* Keep a count of the bytes received so far. */
                        xReceivedBytes += xReturned;
                    }
                }

                /* If an error occurred it will be latched in xReceivedBytes,
                 * otherwise xReceived bytes will be just that - the number of
                 * bytes received from the echo server. */
                if( xReceivedBytes == xTransmitted )
                {
                    /* Compare the transmitted string to the received string. */
                    configASSERT( strncmp( pcReceivedString, pcTransmittedString, xTransmitted ) == 0 );

                    if( strncmp( pcReceivedString, pcTransmittedString, xTransmitted ) == 0 )
                    {
                        /* The echo reply was received without error. */
                        ulTxRxCycles[ xInstance ]++;
                        configPRINTF( ( "Received correct string from echo server.\r\n" ) );

                        /* Increment success count. */
                        xSuccess[ xInstance ]++;
                    }
                    else
                    {
                        /* The received string did not match the transmitted
                         * string. */
                        ulTxRxFailures[ xInstance ]++;
                        configPRINTF( ( "ERROR - Received incorrect string from echo server.\r\n" ) );
                        break;
                    }
                }
                else if( xReceivedBytes == -pdFREERTOS_ERRNO_ENOTCONN )
                {
                    /* The connection got closed. */
                    break;
                }
                else
                {
                    /* The received length did not match the transmitted
                     * length. */
                    ulTxRxFailures[ xInstance ]++;
                    configPRINTF( ( "ERROR: xTransmitted %d xReceivedBytes %d \r\n",
                                    ( int ) xTransmitted, ( int ) xReceivedBytes ) );
                    /* Timed out without receiving anything? */
                    break;
                }
            }

            /* Finished using the connected socket, initiate a graceful close:
             * FIN, FIN+ACK, ACK. */
            configPRINTF( ( "Shutting down connection to echo server.\r\n" ) );
            SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );

            /* Expect SOCKETS_Recv() to return an error once the shutdown is
             * complete. */
            xTimeOnEntering = xTaskGetTickCount();

            do
            {
                xReturned = SOCKETS_Recv( xSocket,                    /* The socket being received from. */
                                          &( pcReceivedString[ 0 ] ), /* The buffer into which the received data will be written. */
                                          echoBUFFER_SIZES,           /* The size of the buffer provided to receive the data. */
                                          0 );

                if( xReturned < 0 )
                {
                    break;
                }
            }
            while( ( xTaskGetTickCount() - xTimeOnEntering ) < xReceiveTimeOut );
        }
        else
        {
            configPRINTF( ( "Echo demo failed to connect to echo server %d.%d.%d.%d.\r\n",
                            configECHO_SERVER_ADDR0,
                            configECHO_SERVER_ADDR1,
                            configECHO_SERVER_ADDR2,
                            configECHO_SERVER_ADDR3 ) );
        }

        /* Close this socket before looping back to create another. */
        xReturned = SOCKETS_Close( xSocket );
        configASSERT( xReturned == SOCKETS_ERROR_NONE );

        /* Pause for a short while to ensure the network is not too
         * congested. */
        vTaskDelay( echoLOOP_DELAY );
    }

    /* Notify the parent task about completion. */
    xSemaphoreGive( EchoSingleSemaphore );

    /* Delete self. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

static BaseType_t prvCreateTxData( char * cBuffer,
                                   uint32_t ulBufferLength )
{
    static uint32_t ulCharactersToAdd = 0UL;
    uint32_t ulCharacter;
    static char cChar = '0';

    /* Fill the buffer with an additional character for each time this
     * function is called, up to a maximum, at which time reset to 1 character. */
    ulCharactersToAdd++;

    if( ulCharactersToAdd > ulBufferLength )
    {
        ulCharactersToAdd = 1UL;
    }

    for( ulCharacter = 0; ulCharacter < ulCharactersToAdd; ulCharacter++ )
    {
        cBuffer[ ulCharacter ] = cChar;
        cChar++;

        if( cChar > '~' )
        {
            cChar = '0';
        }
    }

    return ulCharactersToAdd;
}
/*-----------------------------------------------------------*/

BaseType_t xAreSingleTaskTCPEchoClientsStillRunning( void )
{
    static uint32_t ulLastEchoSocketCount[ echoNUM_ECHO_CLIENTS ] = { 0 }, ulLastConnections[ echoNUM_ECHO_CLIENTS ] = { 0 };
    BaseType_t xReturn = pdPASS, x;

    /* Return fail is the number of cycles does not increment between
     * consecutive calls. */
    for( x = 0; x < echoNUM_ECHO_CLIENTS; x++ )
    {
        if( ulTxRxCycles[ x ] == ulLastEchoSocketCount[ x ] )
        {
            xReturn = pdFAIL;
        }
        else
        {
            ulLastEchoSocketCount[ x ] = ulTxRxCycles[ x ];
        }

        if( ulConnections[ x ] == ulLastConnections[ x ] )
        {
            xReturn = pdFAIL;
        }
        else
        {
            ulConnections[ x ] = ulLastConnections[ x ];
        }
    }

    return xReturn;
}
