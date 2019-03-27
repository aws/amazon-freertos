/*
 * Amazon FreeRTOS V1.3.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * FreeRTOS tasks are used with FreeRTOS+TCP to create a HTTP server.  
 * Use the Windows Web browser to creates a communication between
 * the FreeRTOS+TCP TCP/IP stack and the Windows
 * TCP/IP stack.
 *
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* Demo specific includes */
#include "aws_demo_config.h"

/* Specifies the size of the data sent to the server in MSS units. */
#define httpBUFFER_SIZE_MULTIPLIER    3

/* The maximum time to wait for a closing socket to close. */
#define httpSHUTDOWN_DELAY            ( pdMS_TO_TICKS( 5000 ) )

/* The standard port number. */
#define httpPORT_NUMBER    80

/* The size of the Rx and Tx buffers. */
#define httpBUFFER_SIZE    ( ipconfigTCP_MSS * httpBUFFER_SIZE_MULTIPLIER )

/*-----------------------------------------------------------*/

/*
 * Uses FreeRTOS+TCP to listen for incoming echo connections, creating a task
 * to handle each connection.
 */
static void prvConnectionListeningTask( void * pvParameters );

/*
 * Created by the connection listening task to handle a single connection.
 */
static void prvServerConnectionInstance( void * pvParameters );

/*-----------------------------------------------------------*/

/* Stores the stack size passed into vStartSimpleHttpServerTasks() so it can be
 * reused when the server listening task creates tasks to handle connections. */
static uint16_t usUsedStackSize = 0;

/* Used for error reporting. */
static uint32_t ulConnectionCount = 0;

/*-----------------------------------------------------------*/

void vStartSimpleHttpServerTasks( void )
{
    /* Create the HTTP server.  The server uses FreeRTOS+TCP through
     * the spoofed IP and MAC address.  The WinSock client tasks are created from
     * inside the listening task. */
    xTaskCreate( prvConnectionListeningTask,
                 "ServerListener",
                 democonfigTCP_ECHO_SERVER_TASK_STACK_SIZE,
                 NULL,
                 democonfigTCP_ECHO_SERVER_TASK_PRIORITY + 1,
                 NULL );

    /* Remember the requested stack size so it can be re-used by the server
     * listening task when it creates tasks to handle connections. */
    usUsedStackSize = democonfigTCP_ECHO_SERVER_TASK_STACK_SIZE;
}
/*-----------------------------------------------------------*/

static void prvConnectionListeningTask( void * pvParameters )
{
    struct freertos_sockaddr xClient, xBindAddress;
    Socket_t xListeningSocket, xConnectedSocket;
    socklen_t xSize = sizeof( xClient );
    static const TickType_t xReceiveTimeOut = portMAX_DELAY;
    const BaseType_t xBacklog = 20;
    WinProperties_t xWinProps;

    /* Just to prevent compiler warnings. */
    ( void ) pvParameters;

    /* Attempt to open the socket. */
    xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );
    configASSERT( xListeningSocket != FREERTOS_INVALID_SOCKET );

    /* Set a time out so accept() will just wait for a connection. */
    FreeRTOS_setsockopt( xListeningSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );

    /* Fill in the buffer and window sizes that will be used by the socket. */
    xWinProps.lTxBufSize = 6 * ipconfigTCP_MSS;
    xWinProps.lTxWinSize = 3;
    xWinProps.lRxBufSize = 6 * ipconfigTCP_MSS;
    xWinProps.lRxWinSize = 3;

    /* Set the window and buffer sizes. */
    FreeRTOS_setsockopt( xListeningSocket, 0, FREERTOS_SO_WIN_PROPERTIES, ( void * ) &xWinProps, sizeof( xWinProps ) );

    /* Bind the socket to the port that the client task will send to, then
     * listen for incoming connections. */
    xBindAddress.sin_port = httpPORT_NUMBER;
    xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );
    FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );
    FreeRTOS_listen( xListeningSocket, xBacklog );

    for( ;; )
    {
        /* Wait for a client to connect. */
        xConnectedSocket = FreeRTOS_accept( xListeningSocket, &xClient, &xSize );
        configASSERT( xConnectedSocket != FREERTOS_INVALID_SOCKET );

        /* Spawn a task to handle the connection. */
        xTaskCreate( prvServerConnectionInstance, "EchoServer", usUsedStackSize, ( void * ) xConnectedSocket, tskIDLE_PRIORITY, NULL );
    }
}
/*-----------------------------------------------------------*/

static void prvServerConnectionInstance( void * pvParameters )
{
    int32_t lBytes, lSent, lTotalSent;
    uint8_t buffer[ ipconfigTCP_MSS ]; /* Big stack usage, for Windows port only! */
    Socket_t xConnectedSocket;
    char *pch1, *pch2, *pch3;
    char ssid[32];
    char passwd[32];
    int len;
    int bWellRequest=0;
    static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 5000 );
    static const TickType_t xSendTimeOut = pdMS_TO_TICKS( 5000 );
    TickType_t xTimeOnShutdown;

    ulConnectionCount++;
    xConnectedSocket = ( Socket_t ) pvParameters;
    FreeRTOS_setsockopt( xConnectedSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
    FreeRTOS_setsockopt( xConnectedSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xReceiveTimeOut ) );

    for( ;; )
    {
        /* Zero out the receive array so there is NULL at the end of the string
         * when it is printed out. */
        memset( buffer, 0x00, sizeof( buffer ) );

        /* Receive data on the socket. */
        lBytes = FreeRTOS_recv( xConnectedSocket, buffer, sizeof( buffer ), 0 );
        bWellRequest = 0;
        /* If data was received, echo it back. */
        if( lBytes > 0 )
        {
            buffer[lBytes] = '\0';
            printf("recv buf: %s\r\n", buffer);
            pch1 = strstr(buffer, "ssid=");
            if (pch1) {
                    pch2 = strstr(buffer, "&password=");
                    pch3 = strstr(buffer, " HTTP/1.1");
                    len = pch2 - pch1 - 5;
                    strncpy(ssid, pch1 + 5, len);
                    ssid[len] = '\0';
                    len = pch3 - pch2 - 10;
                    strncpy(passwd, pch2 + 10, len);
                    passwd[len] = '\0';
                    printf("[SSID] = \"%s\"\n", ssid);
                    printf("[PASSWD] = \"%s\"\n", passwd);

                    strcpy(buffer, "<html><h2>Set SSID and Password successful.<br>The system is working...</h2></html>\r\n");
                    bWellRequest =1;
            } else {
                    strcpy(buffer, "<h1>Nuvoton Web Server</h1>\n<html><form method=get>\n");
                    strcat(buffer, "<input name=ssid type=text> SSID<br>\n");
                    strcat(buffer, "<input name=password type=text> Password<br>\n");
                    strcat(buffer, "<input type=submit value=Connect>\n</form></html>\r\n");
            }
                
            lSent = 0;
            lTotalSent = 0;

            /* Call send() until all the data has been sent. */
            lBytes= strlen(buffer);
            while( ( lSent >= 0 ) && ( lTotalSent < lBytes ) )
            {
                lSent = FreeRTOS_send( xConnectedSocket, buffer, lBytes - lTotalSent, 0 );
                lTotalSent += lSent;
            }

            if( (lSent < 0) || (bWellRequest) )
            {
                /* Socket closed? */
                break;
            }
        }
        else
        {
            /* Socket closed? */
            break;
        }
    }

    /* Initiate a shutdown in case it has not already been initiated. */
    FreeRTOS_shutdown( xConnectedSocket, FREERTOS_SHUT_RDWR );

    /* Wait for the shutdown to take effect, indicated by FreeRTOS_recv()
     * returning an error. */
    xTimeOnShutdown = xTaskGetTickCount();

    do
    {
        if( FreeRTOS_recv( xConnectedSocket, buffer, ipconfigTCP_MSS, 0 ) < 0 )
        {
            break;
        }
    } while( ( xTaskGetTickCount() - xTimeOnShutdown ) < httpSHUTDOWN_DELAY );

    /* Finished with the socket and the task. */
    FreeRTOS_closesocket( xConnectedSocket );
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

BaseType_t xAreHttpServersStillRunning( void )
{
    static uint32_t ulLastConnectionCount = 0;
    BaseType_t xReturn = pdPASS;

    if( ulConnectionCount == ulLastConnectionCount )
    {
        xReturn = pdFAIL;
    }
    else
    {
        ulLastConnectionCount = ulConnectionCount;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/
