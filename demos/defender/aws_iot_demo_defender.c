/*
 * Amazon FreeRTOS V201908.00
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

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Demo configuration includes. */
#include "aws_demo.h"
#include "aws_demo_config.h"
#include "aws_clientcredential.h"

/* Demo logging include. */
#include "iot_demo_logging.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Secure Socket includes. */
#include "iot_secure_sockets.h"

/* Platform includes for demo. */
#include "platform/iot_clock.h"
#include "platform/iot_network.h"
#include "platform/iot_network_freertos.h"

/* Defender includes. */
#include "aws_iot_defender.h"

/* Includes for initialization. */
#include "iot_mqtt.h"

/* Set to 1 to enable this demo to connect to echo server.
 * Then in the demo output, it is expected to see one more established TCP connection.
 */
#define _DEMO_WITH_SOCKET_CONNECTED_TO_ECHO_SERVER    0

static IotNetworkServerInfo_t _DEFENDER_SERVER_INFO = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
static IotNetworkCredentials_t _AWS_IOT_CREDENTIALS = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

#if _DEMO_WITH_SOCKET_CONNECTED_TO_ECHO_SERVER == 1
    static Socket_t _createSocketToEchoServer();
#endif

static void _defenderTask( void * param );

/* Callback used to get notification of defender's events. */
static void _defenderCallback( void * param1,
                               AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static void _startDefender();

/*-----------------------------------------------------------*/

int vStartDefenderDemo( bool awsIotMqttMode,
                        const char * pIdentifier,
                        void * pNetworkServerInfo,
                        void * pNetworkCredentialInfo,
                        const IotNetworkInterface_t * pNetworkInterface )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus;

    /* Unused parameters */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    /* Initialize the MQTT library. */
    mqttInitStatus = IotMqtt_Init();

    if( mqttInitStatus != IOT_MQTT_SUCCESS )
    {
        status = EXIT_FAILURE;
    }

    if( status == EXIT_SUCCESS )
    {
        _defenderTask( NULL );
    }

    return status;
}

/*-----------------------------------------------------------*/

void _defenderCallback( void * param1,
                        AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    ( void ) param1;

    IotLogInfo( "User's callback is invoked on event %d.", pCallbackInfo->eventType );

    /* Print out the sent metrics report if there is. */
    if( pCallbackInfo->pMetricsReport != NULL )
    {
        IotLogInfo( "\nPublished metrics report." );
    }
    else
    {
        IotLogError( "No metrics report was generated." );
    }

    if( pCallbackInfo->pPayload != NULL )
    {
        IotLogInfo( "\nReceived MQTT message." );
    }
    else
    {
        IotLogError( "No message has been returned from subscribed topic." );
    }
}

/*-----------------------------------------------------------*/

static void _defenderTask( void * param )
{
    ( void ) param;

    /* Expected remote IP of AWS IoT endpoint in defender metrics report. */
    uint32_t expectedIp = 0;
    char expectedIpBuffer[ 16 ] = "";

    IotLogInfo( "----Device Defender Demo Start----.\r\n" );

    #if _DEMO_WITH_SOCKET_CONNECTED_TO_ECHO_SERVER == 1
        /* Create a socket connected to echo server. */
        Socket_t socket = _createSocketToEchoServer();
    #endif

    /* Specify all metrics in "tcp connections" group */
    AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                               AWS_IOT_DEFENDER_METRICS_ALL );

    /* Set metrics report period to 5 minutes(300 seconds) */
    AwsIotDefender_SetPeriod( 300 );

    /* Start the defender agent. */
    _startDefender();

    /* Query DNS for the IP. */
    expectedIp = SOCKETS_GetHostByName( clientcredentialMQTT_BROKER_ENDPOINT );

    /* Convert to string. */
    SOCKETS_inet_ntoa( expectedIp, expectedIpBuffer );
    IotLogInfo( "expected ip: %s", expectedIpBuffer );

    /* Let it run for 3 seconds */
    IotClock_SleepMs( 3000 );

    /* Stop the defender agent. */
    AwsIotDefender_Stop();

    #if _DEMO_WITH_SOCKET_CONNECTED_TO_ECHO_SERVER == 1
        /* Clean up the socket. */
        SOCKETS_Shutdown( socket, SOCKETS_SHUT_RDWR );
        SOCKETS_Close( socket );
    #endif

    IotLogInfo( "----Device Defender Demo End----.\r\n" );

    vTaskDelete( NULL ); /* Delete this task. */
}

/*-----------------------------------------------------------*/

static void _startDefender()
{
    const AwsIotDefenderCallback_t callback = { .function = _defenderCallback, .param1 = NULL };

    AwsIotDefenderStartInfo_t startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

    /* Set network information. */
    startInfo.mqttNetworkInfo = ( IotMqttNetworkInfo_t ) IOT_MQTT_NETWORK_INFO_INITIALIZER;
    startInfo.mqttNetworkInfo.createNetworkConnection = true;
    startInfo.mqttNetworkInfo.u.setup.pNetworkServerInfo = &_DEFENDER_SERVER_INFO;
    startInfo.mqttNetworkInfo.u.setup.pNetworkCredentialInfo = &_AWS_IOT_CREDENTIALS;

    /* Only set ALPN protocol if the connected port is 443. */
    if( ( ( IotNetworkServerInfo_t * ) ( startInfo.mqttNetworkInfo.u.setup.pNetworkServerInfo ) )->port != 443 )
    {
        ( ( IotNetworkCredentials_t * ) ( startInfo.mqttNetworkInfo.u.setup.pNetworkCredentialInfo ) )->pAlpnProtos = NULL;
    }

    /* Set network interface. */
    startInfo.mqttNetworkInfo.pNetworkInterface = IOT_NETWORK_INTERFACE_AFR;

    /* Set MQTT connection information. */
    startInfo.mqttConnectionInfo = ( IotMqttConnectInfo_t ) IOT_MQTT_CONNECT_INFO_INITIALIZER;
    startInfo.mqttConnectionInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;
    startInfo.mqttConnectionInfo.clientIdentifierLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );

    /* Set Defender callback. */
    startInfo.callback = callback;

    /* Invoke defender start API. */
    AwsIotDefender_Start( &startInfo );
}

/*-----------------------------------------------------------*/

#if _DEMO_WITH_SOCKET_CONNECTED_TO_ECHO_SERVER == 1

    static Socket_t _createSocketToEchoServer()
    {
        Socket_t socket;
        SocketsSockaddr_t echoServerAddress;
        int32_t error = 0;

        /* Rx and Tx time outs are used to ensure the sockets do not wait too long for
         * missing data. */
        const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 2000 );
        const TickType_t xSendTimeOut = pdMS_TO_TICKS( 2000 );

        /* Echo requests are sent to the echo server.  The address of the echo
         * server is configured by the constants configECHO_SERVER_ADDR0 to
         * configECHO_SERVER_ADDR3 in FreeRTOSConfig.h. */
        echoServerAddress.usPort = SOCKETS_htons( configTCP_ECHO_CLIENT_PORT );
        echoServerAddress.ulAddress = SOCKETS_inet_addr_quick( configECHO_SERVER_ADDR0,
                                                               configECHO_SERVER_ADDR1,
                                                               configECHO_SERVER_ADDR2,
                                                               configECHO_SERVER_ADDR3 );

        socket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
        configASSERT( socket != SOCKETS_INVALID_SOCKET );

        /* Set a time out so a missing reply does not cause the task to block indefinitely. */
        SOCKETS_SetSockOpt( socket, 0, SOCKETS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
        SOCKETS_SetSockOpt( socket, 0, SOCKETS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

        error = SOCKETS_Connect( socket, &echoServerAddress, sizeof( echoServerAddress ) );
        configASSERT( error == 0 );

        return socket;
    }

#endif /* if _DEMO_WITH_SOCKET_CONNECTED_TO_ECHO_SERVER == 1 */
