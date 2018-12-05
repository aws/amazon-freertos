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
#include "platform/aws_iot_network.h"

/*-----------------------------------------------------------*/

static void _mqttDemoTask( void * argument )
{
    int status = 0;
    AwsIotNetworkConnection_t networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
    AwsIotNetworkTlsInfo_t tlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
    AwsIotMqttConnection_t mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;

    /* Silence warnings about unused parameters. */
    ( void ) argument;

    /* This function parses arguments and establishes the network connection
     * before running the MQTT demo. */

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

    /* Establish a TCP connection to the MQTT server. */
    if( AwsIotNetwork_CreateConnection( &networkConnection,
                                        clientcredentialMQTT_BROKER_ENDPOINT,
                                        clientcredentialMQTT_BROKER_PORT,
                                        &tlsInfo ) != AWS_IOT_NETWORK_SUCCESS )
    {
        status = -1;
    }

    if( status == 0 )
    {
        /* Set the MQTT receive callback. */
        if( AwsIotNetwork_SetMqttReceiveCallback( networkConnection,
                                                  &mqttConnection,
                                                  AwsIotMqtt_ReceiveCallback ) != AWS_IOT_NETWORK_SUCCESS )
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
            status = AwsIotDemo_RunMqttDemo( true, NULL, &mqttConnection, &networkInterface );

            /* Clean up the MQTT library. */
            AwsIotMqtt_Cleanup();
        }
        else
        {
            status = -1;
        }
    }

    /* Close and destroy the network connection (if it was established). */
    if( networkConnection != AWS_IOT_NETWORK_CONNECTION_INITIALIZER )
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
