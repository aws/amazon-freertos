/*
 * FreeRTOS V202007.00
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

/* Demo configuration includes. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Demo logging include. */
#include "iot_demo_logging.h"

/* Platform includes for demo. */
#include "platform/iot_clock.h"
#include "platform/iot_network.h"

/* Defender includes. */
#include "aws_iot_defender.h"

/* Includes for initialization. */
#include "iot_mqtt.h"
#include "platform/iot_metrics.h"

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define KEEP_ALIVE_SECONDS           ( ( uint16_t ) 60 )

/**
 * @brief The timeout for Defender and MQTT operations in this demo.
 */
#define TIMEOUT_MS                   ( ( uint32_t ) 5000 )

/**
 * @brief Defender metrics publish interval, 5 minutes (300 seconds) is minumum.
 */
#define DEFENDER_PUBLISH_INTERVAL    ( ( uint32_t ) 300 )

/*-----------------------------------------------------------*/

/**
 * @brief Callback used to get notification of defender's events.
 *
 * @param[in] pCallbackContext context pointer passed by the application
 * when callback is regiested in AwsIotDefender_Start()
 *
 * @param[im] pointer to AwsIotDefenderCallbackInfo_t containing status of
 * publish
 */
static void _defenderCallback( void * pCallbackContext,
                               AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    ( void ) pCallbackContext;

    IotLogInfo( "User's callback is invoked on event: %s.", AwsIotDefender_EventType( pCallbackInfo->eventType ) );

    if( pCallbackInfo != NULL )
    {
        /*  Callback info processing example . */
        if( pCallbackInfo->pMetricsReport != NULL )
        {
            IotLogInfo( "Published metrics report." );
        }
        else
        {
            IotLogError( "No metrics report was generated." );
        }

        if( pCallbackInfo->pPayload != NULL )
        {
            IotLogInfo( "Received MQTT message." );
        }
        else
        {
            IotLogError( "No message has been returned from subscribed topic." );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Establish a new connection to the MQTT server for the Defender demo.
 *
 * @param[in] pIdentifier NULL-terminated MQTT client identifier. The Defender
 * demo will use the Thing Name as the client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 * @param[out] pMqttConnection Set to the handle to the new MQTT connection.
 *
 * @return `EXIT_SUCCESS` if the connection is successfully established; `EXIT_FAILURE`
 * otherwise.
 */
static int _establishMqttConnection( const char * pIdentifier,
                                     void * pNetworkServerInfo,
                                     void * pNetworkCredentialInfo,
                                     const IotNetworkInterface_t * pNetworkInterface,
                                     IotMqttConnection_t * pMqttConnection )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t connectStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

    if( pIdentifier == NULL )
    {
        IotLogError( "Defender Thing Name must be provided." );

        status = EXIT_FAILURE;
    }

    if( status == EXIT_SUCCESS )
    {
        /* Set the members of the network info not set by the initializer. This
         * struct provided information on the transport layer to the MQTT connection. */
        networkInfo.createNetworkConnection = true;
        networkInfo.u.setup.pNetworkServerInfo = pNetworkServerInfo;
        networkInfo.u.setup.pNetworkCredentialInfo = pNetworkCredentialInfo;
        networkInfo.pNetworkInterface = pNetworkInterface;

        /* Set the members of the connection info not set by the initializer. */
        connectInfo.awsIotMqttMode = true;
        connectInfo.cleanSession = true;
        connectInfo.keepAliveSeconds = KEEP_ALIVE_SECONDS;

        /* AWS IoT recommends the use of the Thing Name as the MQTT client ID. */
        connectInfo.pClientIdentifier = pIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( pIdentifier );

        IotLogInfo( "Defender Thing Name is %.*s (length %hu).",
                    connectInfo.clientIdentifierLength,
                    connectInfo.pClientIdentifier,
                    connectInfo.clientIdentifierLength );

        /* Establish the MQTT connection. */
        connectStatus = IotMqtt_Connect( &networkInfo,
                                         &connectInfo,
                                         TIMEOUT_MS,
                                         pMqttConnection );

        if( connectStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT CONNECT returned error %s.",
                         IotMqtt_strerror( connectStatus ) );

            status = EXIT_FAILURE;
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the Defender demo, called by the demo runner.
 *
 * @param[in] awsIotMqttMode Ignored for the Defender demo.
 * @param[in] pIdentifier NULL-terminated Defender Thing Name.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection for Defender.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection for Defender.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunDefenderDemo( bool awsIotMqttMode,
                     const char * pIdentifier,
                     void * pNetworkServerInfo,
                     void * pNetworkCredentialInfo,
                     const IotNetworkInterface_t * pNetworkInterface )
{
    int status = EXIT_SUCCESS;
    bool metricsInitStatus = false;
    IotMqttError_t mqttStatus = IOT_MQTT_INIT_FAILED;
    AwsIotDefenderError_t defenderResult = AWS_IOT_DEFENDER_INTERNAL_FAILURE;
    AwsIotDefenderStartInfo_t startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;
    const AwsIotDefenderCallback_t callback = { .function = _defenderCallback, .pCallbackContext = NULL };
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    /* Unused parameters. */
    ( void ) awsIotMqttMode;

    IotLogInfo( "----Device Defender Demo Start----" );

    /* Check parameter(s). */
    if( ( pIdentifier == NULL ) || ( pIdentifier[ 0 ] == '\0' ) )
    {
        IotLogError( "The length of the Thing Name (identifier) must be nonzero." );
        status = EXIT_FAILURE;
    }

    if( status == EXIT_SUCCESS )
    {
        /* Initialize the MQTT library. */
        mqttStatus = IotMqtt_Init();

        if( mqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT Initialization Failed." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Initialize Metrics. */
        metricsInitStatus = IotMetrics_Init();

        if( !metricsInitStatus )
        {
            IotLogError( "IOT Metrics Initialization Failed." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Specify all metrics in "tcp connections" group */
        defenderResult =
            AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_ALL );

        if( defenderResult != AWS_IOT_DEFENDER_SUCCESS )
        {
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Set metrics report period */
        defenderResult = AwsIotDefender_SetPeriod( DEFENDER_PUBLISH_INTERVAL );

        if( defenderResult != AWS_IOT_DEFENDER_SUCCESS )
        {
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Create MQTT Connection */
        mqttStatus = _establishMqttConnection( pIdentifier,
                                               pNetworkServerInfo,
                                               pNetworkCredentialInfo,
                                               pNetworkInterface,
                                               &mqttConnection );

        if( mqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "Failed to Create MQTT Connection, error: %s", IotMqtt_strerror( mqttStatus ) );
            IotMqtt_Cleanup();
            defenderResult = AWS_IOT_DEFENDER_INTERNAL_FAILURE;
        }
        else
        {
            /* Initialize start info and call defender Start API */
            startInfo.pClientIdentifier = pIdentifier;
            startInfo.clientIdentifierLength = ( uint16_t ) strlen( pIdentifier );
            startInfo.callback = callback;
            startInfo.mqttConnection = mqttConnection;
            defenderResult = AwsIotDefender_Start( &startInfo );
        }

        if( defenderResult == AWS_IOT_DEFENDER_SUCCESS )
        {
            /* Let it run for 3 seconds */
            IotClock_SleepMs( 3000 );
            /* Stop the defender agent. */
            AwsIotDefender_Stop();
            /* Disconnect MQTT */
            IotMqtt_Disconnect( mqttConnection, false );
        }
    }

    /* Cleanup. */
    if( metricsInitStatus )
    {
        IotMetrics_Cleanup();
    }

    if( mqttStatus == IOT_MQTT_SUCCESS )
    {
        IotMqtt_Cleanup();
    }

    IotLogInfo( "----Device Defender Demo End. Status: %s----.", AwsIotDefender_strerror( defenderResult ) );
    return status;
}

/*-----------------------------------------------------------*/
