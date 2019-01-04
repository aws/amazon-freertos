/*
 * Amazon FreeRTOS Device Defender Demo V1.4.4
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes. */
#include "stdio.h"
#include "string.h"

/* Demo configuration includes. */
#include "aws_iot_demo.h"
#include "aws_demo_config.h"
#include "aws_clientcredential.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Defender includes. */
#include "aws_iot_defender.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"

/* Cbor includes. */
#include "cbor.h"

static void _defenderTask( void * param );

/* Callback used to get notification of defender's events. */
static void _defenderCallback( void * param1,
                               AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

/* Helper function to open a new socket on specific port number. */
static void _listenSocket( uint16_t portNumber );

/* Deserialze the data and print out in more readable way. */
static void _print( uint8_t * pDataBuffer,
                    size_t dataSize );

static void _startDefender();

/*-----------------------------------------------------------*/

void vStartDefenderDemo( void )
{
    ( void ) xTaskCreate( _defenderTask,
                          "Defender Demo",
                          democonfigDEFENDER_TASK_STACK_SIZE,
                          NULL,
                          democonfigDEFENDER_TASK_PRIORITY,
                          NULL );
}

/*-----------------------------------------------------------*/

void _defenderCallback( void * param1,
                        AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    AwsIotLogInfo( "User's callback is invoked on event %d.", pCallbackInfo->eventType );

    /* Print out the sent metrics report if there is. */
    if( pCallbackInfo->pMetricsReport != NULL )
    {
        AwsIotLogInfo( "\nPublished metrics report:" );
        _print( pCallbackInfo->pMetricsReport, pCallbackInfo->metricsReportLength );
    }
    else
    {
        AwsIotLogError( "No metrics report was generated." );
    }

    if( pCallbackInfo->pPayload != NULL )
    {
        AwsIotLogInfo( "\nReceived MQTT message:" );
        _print( pCallbackInfo->pPayload, pCallbackInfo->payloadLength );
    }
    else
    {
        AwsIotLogError( "No message has been returned from subscribed topic." );
    }
}

/*-----------------------------------------------------------*/

static void _defenderTask( void * param )
{
    ( void ) param;

    AwsIotNetwork_Init();

    /* create a socket in listening state, on port 10000. */
    _listenSocket( 10000 );

    AwsIotLogInfo( "----Device Defender Demo Start----.\r\n" );

    /* provide metrics: all metrics in "tcp connections" group
     * and all metrics in "listening tcp" group
     */
    AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                               AWS_IOT_DEFENDER_METRICS_ALL );
    AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_LISTENING_TCP,
                               AWS_IOT_DEFENDER_METRICS_ALL );

    /* set metrics report period to 5 seconds */
    AwsIotDefender_SetPeriod( 5 );

    _startDefender();

    /* let it run for 10 seconds */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );

    /* stop the defender*/
    AwsIotDefender_Stop();

    AwsIotLogInfo( "----Device Defender Demo End----.\r\n" );
}

/*-----------------------------------------------------------*/

static void _startDefender()
{
    const AwsIotDefenderCallback_t callback = { .function = _defenderCallback, .param1 = NULL };

    AwsIotNetworkTlsInfo_t tlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
    AwsIotNetworkTlsInfo_t * pTlsInfo = &tlsInfo;

    /* Use the default root CA provided with Amazon FreeRTOS. */
    tlsInfo.pRootCa = NULL;
    tlsInfo.rootCaLength = 0;

    /* Set client credentials. */
    tlsInfo.pClientCert = clientcredentialCLIENT_CERTIFICATE_PEM;
    tlsInfo.clientCertLength = ( size_t ) clientcredentialCLIENT_CERTIFICATE_LENGTH;
    tlsInfo.pPrivateKey = clientcredentialCLIENT_PRIVATE_KEY_PEM;
    tlsInfo.privateKeyLength = ( size_t ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

    /* If not connecting over port 443, disable ALPN. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        tlsInfo.pAlpnProtos = NULL;
    }

    /* start the defender */
    const AwsIotDefenderStartInfo_t startInfo =
    {
        .pTlsInfo        = pTlsInfo,
        .pAwsIotEndpoint = clientcredentialMQTT_BROKER_ENDPOINT,
        .port            = clientcredentialMQTT_BROKER_PORT,
        .pThingName      = clientcredentialIOT_THING_NAME,
        .thingNameLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME ),
        .callback        = callback
    };

    AwsIotDefender_Start( &startInfo );
}

/*-----------------------------------------------------------*/

static void _listenSocket( uint16_t portNumber )
{
    struct freertos_sockaddr xBindAddress;
    Socket_t xListeningSocket;

    /* Attempt to open the socket. */
    xListeningSocket = FreeRTOS_socket( PF_INET,
                                        SOCK_STREAM, /* SOCK_STREAM for TCP. */
                                        IPPROTO_TCP );

    xBindAddress.sin_port = portNumber;
    xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );

    /* Bind the socket to the port that the client RTOS task will send to. */
    FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );

    /* Set the socket into a listening state so it can accept connections.
     * The maximum number of simultaneous connections is limited to 20. */
    FreeRTOS_listen( xListeningSocket, 10 );
}

/*-----------------------------------------------------------*/

static void _print( uint8_t * pDataBuffer,
                    size_t dataSize )
{
    #if ( AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR )
        CborParser cborParser;
        CborValue cborValue;

        cbor_parser_init(
            pDataBuffer,
            dataSize,
            0,
            &cborParser,
            &cborValue );

        /* output to standard out */
        cbor_value_to_pretty( stdout, &cborValue );
    #endif /* if ( AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR ) */
}
