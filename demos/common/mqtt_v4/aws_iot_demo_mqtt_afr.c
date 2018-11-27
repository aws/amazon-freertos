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
#include "aws_iot_demo.h"
#include "aws_demo_config.h"
#include "aws_clientcredential.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT include. */
#include "aws_iot_mqtt.h"

/* Platform layer includes. */
#include "platform/aws_iot_network_afr.h"

/*-----------------------------------------------------------*/

static void _mqttDemoTask( void * argument )
{
    int status = 0;
    AwsIotNetworkConnection_t networkConnection = AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER;
    AwsIotNetworkConnectParams_t connectParams = AWS_IOT_NETWORK_AFR_CONNECT_PARAMS_INITIALIZER;
    AwsIotMqttConnection_t mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;

    /* Silence warnings about unused parameters. */
    ( void ) argument;

    /* This function parses arguments and establishes the network connection
     * before running the MQTT demo. */

    /* Set the endpoint for the MQTT demo. By default, the demo uses a secured
     * connection. */
    connectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
    connectParams.usPort = clientcredentialMQTT_BROKER_PORT;
    connectParams.flags = AWS_IOT_NETWORK_AFR_FLAG_SECURED;

    /* If connecting over port 443, set ALPN flag. */
    if( connectParams.usPort == securesocketsDEFAULT_TLS_DESTINATION_PORT )
    {
        connectParams.flags |= AWS_IOT_NETWORK_AFR_FLAG_ALPN;
    }

    /* Override the default root CA certificates by setting pcServerCertificate
     * and xServerCertificateSize. SNI may need to be disabled if not using the
     * default certificates. */
    connectParams.pcServerCertificate = NULL;
    connectParams.xServerCertificateSize = 0;
    connectParams.flags |= AWS_IOT_NETWORK_AFR_FLAG_SNI;

    /* Establish a TCP connection to the MQTT server. */
    if( AwsIotNetwork_CreateConnection( &connectParams,
                                        &networkConnection ) == false )
    {
        status = -1;
    }

    if( status == 0 )
    {
        /* Create the task that processes incoming MQTT data. */
        if( AwsIotNetwork_CreateMqttReceiveTask( networkConnection,
                                                 &mqttConnection,
                                                 AWS_IOT_MQTT_RECEIVE_TASK_PRIORITY,
                                                 AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE ) == false )
        {
            status = -1;
        }
    }

    if( status == 0 )
    {
        /* Set the members of the network interface used by the MQTT connection. */
        networkInterface.pDisconnectContext = ( void * ) networkConnection;
        networkInterface.pSendContext = ( void * ) networkConnection;
        networkInterface.disconnect = AwsIotNetwork_CloseConnection;
        networkInterface.send = AwsIotNetwork_Send;

        /* Initialize the MQTT library. */
        if( AwsIotMqtt_Init() == AWS_IOT_MQTT_SUCCESS )
        {
            /* Run the MQTT demo. */
            status = AwsIotDemo_Mqtt( true, &mqttConnection, &networkInterface );

            /* Clean up the MQTT library. */
            AwsIotMqtt_Cleanup();
        }
        else
        {
            status = -1;
        }
    }

    /* Close and destroy the network connection (if it was established). */
    if( networkConnection != AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER )
    {
        /* Note that the MQTT library may have called AwsIotNetwork_CloseConnection.
         * However, AwsIotNetwork_CloseConnection is safe to call on a closed connection.
         * On the other hand, AwsIotNetwork_DestroyConnection must only be called ONCE.
         */
        AwsIotNetwork_CloseConnection( networkConnection );
        AwsIotNetwork_DestroyConnection( networkConnection );
    }

    /* Log the demo status. */
    if( status == 0 )
    {
        AwsIotLogInfo( "Demo completed successfully." );
    }
    else
    {
        AwsIotLogError( "Error running demo, status %d.", status );
    }

    AwsIotLogInfo( "Demo minimum ever free heap: %lu bytes.",
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
