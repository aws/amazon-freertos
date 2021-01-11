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

/**
 * @file aws_greengrass_discovery_demo.c
 * @brief A simple Greengrass discovery example.
 *
 * A simple example that perform discovery of the greengrass core device.
 * The JSON file is retrieved.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Demo include. */
#include "aws_demo_config.h"

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "platform/iot_network.h"
#include "platform/iot_network_freertos.h"
#include "platform/iot_clock.h"

/* Greengrass includes. */
#include "aws_ggd_config.h"
#include "aws_ggd_config_defaults.h"
#include "aws_greengrass_discovery.h"

/* Includes for initialization. */
#include "iot_mqtt.h"

#define ggdDEMO_MAX_MQTT_MESSAGES              3
#define ggdDEMO_MAX_MQTT_MSG_SIZE              500
#define ggdDEMO_DISCOVERY_FILE_SIZE            2500
#define ggdDEMO_MQTT_MSG_TOPIC                 "freertos/demos/ggd"
#define ggdDEMO_MQTT_MSG_DISCOVERY             "{\"message\":\"Hello #%lu from FreeRTOS to Greengrass Core.\"}"

#define ggdDEMO_KEEP_ALIVE_INTERVAL_SECONDS    1200
/* Number of times to try MQTT connection. */
#define ggdDEMO_NUM_TRIES                      3
#define ggdDEMO_RETRY_WAIT_MS                  2000

/**
 * @brief Contains the user data for callback processing.
 */
typedef struct
{
    const char * pcExpectedString;      /**< Informs the MQTT callback of the next expected string. */
    BaseType_t xCallbackStatus;         /**< Used to communicate the success or failure of the callback function.
                                         * xCallbackStatus is set to pdFALSE before the callback is executed, and is
                                         * set to pdPASS inside the callback only if the callback receives the expected
                                         * data. */
    SemaphoreHandle_t xWakeUpSemaphore; /**< Handle of semaphore to wake up the task. */
    char * pcTopic;                     /**< Topic to subscribe and publish to. */
} GGDUserData_t;

/* The maximum time to wait for an MQTT operation to complete.  Needs to be
 * long enough for the TLS negotiation to complete. */
static const uint32_t _maxCommandTimeMs = 20000UL;
static const uint32_t _timeBetweenPublishMs = 1500UL;
static char pcJSONFile[ ggdDEMO_DISCOVERY_FILE_SIZE ];

static IotMqttError_t _mqttConnect( GGD_HostAddressData_t * pxHostAddressData,
                                    const IotNetworkInterface_t * pNetworkInterface,
                                    IotMqttConnection_t * pMqttConnection );
static void _sendMessageToGGC( IotMqttConnection_t mqttConnection );
static int _discoverGreengrassCore( const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

static void _sendMessageToGGC( IotMqttConnection_t mqttConnection )
{
    const char * pcTopic = ggdDEMO_MQTT_MSG_TOPIC;
    uint32_t ulMessageCounter;
    char cBuffer[ ggdDEMO_MAX_MQTT_MSG_SIZE ];
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttPublishInfo_t xPublishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;


    /* Publish to the topic to which this task is subscribed in order
     * to receive back the data that was published. */
    xPublishInfo.qos = IOT_MQTT_QOS_0;
    xPublishInfo.pTopicName = pcTopic;
    xPublishInfo.topicNameLength = ( uint16_t ) ( strlen( pcTopic ) );

    for( ulMessageCounter = 0; ulMessageCounter < ( uint32_t ) ggdDEMO_MAX_MQTT_MESSAGES; ulMessageCounter++ )
    {
        /* Set the members of the publish info. */
        xPublishInfo.payloadLength = ( uint32_t ) sprintf( cBuffer, ggdDEMO_MQTT_MSG_DISCOVERY, ( long unsigned int ) ulMessageCounter ); /*lint !e586 sprintf can be used for specific demo. */
        xPublishInfo.pPayload = ( const void * ) cBuffer;

        /* Call the MQTT blocking PUBLISH function. */
        xMqttStatus = IotMqtt_TimedPublish( mqttConnection,
                                            &xPublishInfo,
                                            0,
                                            _maxCommandTimeMs );

        if( xMqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "mqtt_client - Failure to publish. Error %s.",
                         IotMqtt_strerror( xMqttStatus ) );
        }

        IotClock_SleepMs( _timeBetweenPublishMs );
    }
}

/*-----------------------------------------------------------*/

static IotMqttError_t _mqttConnect( GGD_HostAddressData_t * pxHostAddressData,
                                    const IotNetworkInterface_t * pNetworkInterface,
                                    IotMqttConnection_t * pMqttConnection )
{
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    IotNetworkServerInfo_t xServerInfo = { 0 };
    IotNetworkCredentials_t xCredentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;
    IotMqttNetworkInfo_t xNetworkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t xMqttConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    uint32_t connectAttempt = 0;

    /* Set the server certificate for a secured connection. Other credentials
     * are set by the initializer. */
    xCredentials.pRootCa = pxHostAddressData->pcCertificate;
    xCredentials.rootCaSize = ( size_t ) pxHostAddressData->ulCertificateSize;
    /* Disable SNI. */
    xCredentials.disableSni = true;
    /* ALPN is not needed. */
    xCredentials.pAlpnProtos = NULL;

    /* Set the server info. */
    xServerInfo.pHostName = pxHostAddressData->pcHostAddress;
    xServerInfo.port = clientcredentialMQTT_BROKER_PORT;

    /* Set the members of the network info. */
    xNetworkInfo.createNetworkConnection = true;
    xNetworkInfo.u.setup.pNetworkServerInfo = &xServerInfo;
    xNetworkInfo.u.setup.pNetworkCredentialInfo = &xCredentials;
    xNetworkInfo.pNetworkInterface = pNetworkInterface;

    /* Connect to the broker. */
    xMqttConnectInfo.awsIotMqttMode = true;
    xMqttConnectInfo.cleanSession = true;
    xMqttConnectInfo.pClientIdentifier = ( const char * ) ( clientcredentialIOT_THING_NAME );
    xMqttConnectInfo.clientIdentifierLength = ( uint16_t ) ( strlen( clientcredentialIOT_THING_NAME ) );
    xMqttConnectInfo.keepAliveSeconds = ggdDEMO_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Call MQTT's CONNECT function. */
    for( connectAttempt = 0; connectAttempt < ggdDEMO_NUM_TRIES; connectAttempt++ )
    {
        if( connectAttempt > 0 )
        {
            IotLogError( "Failed to establish MQTT connection, retrying in %d ms.",
                         ggdDEMO_RETRY_WAIT_MS );
            IotClock_SleepMs( ggdDEMO_RETRY_WAIT_MS );
        }

        IotLogInfo( "Attempting to establish MQTT connection to Greengrass." );
        xMqttStatus = IotMqtt_Connect( &xNetworkInfo,
                                       &xMqttConnectInfo,
                                       _maxCommandTimeMs,
                                       pMqttConnection );

        if( xMqttStatus != IOT_MQTT_NETWORK_ERROR )
        {
            break;
        }
    }

    return xMqttStatus;
}

/*-----------------------------------------------------------*/

static int _discoverGreengrassCore( const IotNetworkInterface_t * pNetworkInterface )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttStatus = IOT_MQTT_SUCCESS;
    GGD_HostAddressData_t xHostAddressData;
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    memset( &xHostAddressData, 0, sizeof( xHostAddressData ) );

    /* Demonstrate automated connection. */
    IotLogInfo( "Attempting automated selection of Greengrass device\r\n" );

    if( GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                    clientcredentialGREENGRASS_DISCOVERY_PORT,
                                    clientcredentialIOT_THING_NAME,
                                    pcJSONFile,
                                    ggdDEMO_DISCOVERY_FILE_SIZE,
                                    &xHostAddressData )
        == pdPASS )
    {
        IotLogInfo( "Greengrass device discovered." );
        mqttStatus = _mqttConnect( &xHostAddressData, pNetworkInterface, &mqttConnection );

        if( mqttStatus == IOT_MQTT_SUCCESS )
        {
            _sendMessageToGGC( mqttConnection );

            IotLogInfo( "Disconnecting from broker." );

            /* Call MQTT v2's DISCONNECT function. */
            IotMqtt_Disconnect( mqttConnection, 0 );
            mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
            IotLogInfo( "Disconnected from the broker." );
        }
        else
        {
            IotLogError( "Could not connect to the Broker. Error %s.",
                         IotMqtt_strerror( mqttStatus ) );
            status = EXIT_FAILURE;
        }

        /* Report on space efficiency of this demo task. */
        #if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
            {
                configPRINTF( ( "Heap low watermark: %u. Stack high watermark: %u.\r\n",
                                xPortGetMinimumEverFreeHeapSize(),
                                uxTaskGetStackHighWaterMark( NULL ) ) );
            }
        #endif
    }
    else
    {
        IotLogError( "Auto-connect: Failed to retrieve Greengrass address and certificate." );
        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

int vStartGreenGrassDiscoveryTask( bool awsIotMqttMode,
                                   const char * pIdentifier,
                                   void * pNetworkServerInfo,
                                   void * pNetworkCredentialInfo,
                                   const IotNetworkInterface_t * pNetworkInterface )
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;

    /* Unused parameters */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;

    mqttInitStatus = IotMqtt_Init();

    if( mqttInitStatus == IOT_MQTT_SUCCESS )
    {
        status = _discoverGreengrassCore( pNetworkInterface );
        IotMqtt_Cleanup();
        IotLogInfo( "Cleaned up MQTT library." );
    }
    else
    {
        IotLogError( "Failed to initialize MQTT library." );
        status = EXIT_FAILURE;
    }

    return status;
}
