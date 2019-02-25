/*
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
 */

/**
 * @file aws_iot_demo_mqtt_afr.c
 * @brief Demo runner for MQTT v4 demo on Amazon FreeRTOS.
 */

/* Demo configuration includes. */
#include "iot_demo.h"
#include "aws_demo_config.h"
#include "aws_clientcredential.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT include. */
#include "iot_mqtt.h"

/* Amazon FreeRTOS network abstraction include. */
#include "platform/iot_network_afr.h"

/*-----------------------------------------------------------*/

static void _mqttDemoTask( void * argument )
{
    int status = 0;
    bool networkCreated = false;
    IotNetworkConnectionAfr_t networkConnection = IOT_NETWORK_CONNECTION_AFR_INITIALIZER;
    IotNetworkServerInfoAfr_t serverInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t credentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;

    /* Silence warnings about unused parameters. */
    ( void ) argument;

    /* This function parses arguments and establishes the network connection
     * before running the MQTT demo. */

    /* If not connecting over port 443, disable ALPN. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        credentials.pAlpnProtos = NULL;
    }

    /* Establish a TCP connection to the MQTT server. */
    if( IotNetworkAfr_Create( &serverInfo,
                              &credentials,
                              &networkConnection ) != IOT_NETWORK_SUCCESS )
    {
        status = -1;
    }
    else
    {
        networkCreated = true;
    }

    if( status == 0 )
    {
        /* Set the MQTT receive callback. */
        if( IotNetworkAfr_SetReceiveCallback( &networkConnection,
                                              IotMqtt_ReceiveCallback,
                                              &mqttConnection ) != IOT_NETWORK_SUCCESS )
        {
            status = -1;
        }
    }

    if( status == 0 )
    {
        /* Set the members of the network interface used by the MQTT connection. */
        networkInterface.pDisconnectContext = ( void * ) &networkConnection;
        networkInterface.pSendContext = ( void * ) &networkConnection;
        networkInterface.disconnect = IotNetworkAfr_Close;
        networkInterface.send = IotNetworkAfr_Send;

        /* Initialize the MQTT library. */
        if( IotMqtt_Init() == IOT_MQTT_SUCCESS )
        {
            /* Run the MQTT demo. */
            status = IotDemo_RunMqttDemo( true, NULL, &mqttConnection, &networkInterface );

            /* Clean up the MQTT library. */
            IotMqtt_Cleanup();
        }
        else
        {
            status = -1;
        }
    }

    /* Close and destroy the network connection (if it was established). */
    if( networkCreated == true )
    {
        /* Note that the MQTT library may have called IotNetworkAfr_Close.
         * However, IotNetworkAfr_Close is safe to call on a closed connection.
         * On the other hand, IotNetworkAfr_Destroy must only be called ONCE.
         */
        IotNetworkAfr_Close( &networkConnection );
        IotNetworkAfr_Destroy( &networkConnection );
    }

    /* Log the demo status. */
    if( status == 0 )
    {
        IotLogInfo( "Demo completed successfully." );
    }
    else
    {
        IotLogError( "Error running demo, status %d.", status );
    }

    IotLogInfo( "Demo minimum ever free heap: %lu bytes.",
                ( unsigned long ) xPortGetMinimumEverFreeHeapSize() );

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

void vStartMQTTv4Demo( void )
{
    /* Create the FreeRTOS task that sets up the network and runs the MQTTv4 demo. */
    ( void ) xTaskCreate( _mqttDemoTask,
                          "MQTTv4Demo",
                          democonfigMQTT_ECHO_TASK_STACK_SIZE,
                          NULL,
                          democonfigMQTT_ECHO_TASK_PRIORITY,
                          NULL );
}

/*-----------------------------------------------------------*/
