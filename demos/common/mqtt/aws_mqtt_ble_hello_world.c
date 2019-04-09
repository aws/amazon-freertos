/*
 * Amazon FreeRTOS
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


/**
 * @file aws_mqtt_ble_hello_world.c
 * @brief Hello world Mqtt demo over multiple network types.
 *
 * This demonstrates a MQTT hello world echo demo over different network types like Wi-Fi, BLE etc.
 * It uses network agnostic MQTT library, Network Interface abstraction and network manager to select a network
 * type and to dynamically switch between network types during network termination without interrupting the MQTT application.
 * 
 */


#include <stdbool.h>
#include <string.h>

/* Demo logging and configuration includes */
#include "iot_demo_logging.h"
#include "aws_demo_config.h"


/* FreeRTOS  includes. */
#include "FreeRTOS.h"
#include "task.h"


#include "aws_clientcredential.h"


/* MQTT library includes */
#include "iot_common.h"
#include "iot_mqtt.h"
#include "iot_ble_mqtt.h"

/* Platform includes */
#include "platform/iot_threads.h"
#include "platform/iot_clock.h"

/* Networking includes */
#include "types/iot_network_types.h"
#include "platform/iot_network.h"
#include "platform/iot_network_afr.h"
#include "iot_network_manager_private.h"

/**
 * @brief MQTT topic used to publish and subscribe to messages.
 */
#define _TOPIC             "freertos/demos/echo"

/**
 * @brief QOS value used for the MQTT echo demo.
 */
#define _QOS               ( 1 )

/**
 * @brief Echo message sent to the broker by the MQTT echo demo.
 */
#define _MESSAGE           "HelloWorld %d"

/**
 * @brief Length of the echo message sent to the broker by the MQTT echo demo.
 */
#define _MESSAGE_LENGTH               ( sizeof( _MESSAGE ) + 5 )

/**
 * @brief Suffix for ACK messages echoed back to the broker.
 */
#define _ACK                           " ACK"

/**
 * @brief Length of the ACK messages echoed back to the broker.
 */
#define _ACK_LENGTH                   ( _MESSAGE_LENGTH + sizeof( _ACK  ) )

#define _ACK_INDEX( length )          ( length - strlen( _ACK ) )

/**
 * @brie MQTT Keepalive seconds used by the demo.
 */
#define _MQTT_KEEPALIVE_SECONDS       ( 120 )

/**
 * @brief Maximum number of echo messages sent to the broker before the demo completes.
 */
#define _MAX_MESSAGES                 ( 3600 )

/**
 * @brief Interval in milliseconds between the echo messages sent to the broker.
 */
#define _MESSAGE_INTERVAL_MS         ( 1000 )

/**
 * @brief Delay between the retries for a publish message.
 */
#define _CONNECTION_RETRY_DELAY_MS   ( 5000 )

/**
 * @brief Timeout for any MQTT operation.
 */
#define _MQTT_TIMEOUT_MS             ( 5000 )

/**
 * @brief Prefix used to generate a unique client Identifier for an Mqtt connection.
 */ 
#define _CLIENT_IDENTIFIER_PREFIX                 "mqttEcho"

/**
 * @brief Maximum length for the Mqtt client identifier ( including the prefix and the unique ID ).
 */ 
#define _CLIENT_IDENTIFIER_MAX_LENGTH             ( 24 )

/**
 * @brief Keep alive interval for Mqtt connection. The value is used
 * only if awsIotMqttMode is set to true.
 */ 
#define _KEEP_ALIVE_SECONDS                       ( 60 )

/**
 * @brief Number of topic filter subscriptions made by the demo.
 */ 
#define _TOPIC_FILTER_COUNT                       ( 1 )

/* -------------------------------------------------------------------------------------------- */

/* Contains various states for the mqtt application */
typedef enum BLEMqttDemoState
{
    BLE_MQTT_ECHO_DEMO_STATE_NETWORK_DISCONNECTED = 0,
    BLE_MQTT_ECHO_DEMO_STATE_CONNECTING,
    BLE_MQTT_ECHO_DEMO_STATE_CONNECTED,
    BLE_MQTT_ECHO_DEMO_STATE_ERROR
} BLEMqttDemoState_t;

/* A wrapper structure to hold the networking information for the demo. */
typedef struct BleMqttDemoNetworkInfo
{
    bool       awsIotMqttMode;
    const char *pClientIdentifier;
    void       *pNetworkServerInfo;
    void       *pNetworkCredentialInfo;
    const IotNetworkInterface_t *pNetworkInterface;
} BleMqttDemoNetworkInfo_t;


void BLEMqttEchoDemoNetworkConnectedCallback( bool awsIotMqttMode,
                                              const char *pIdentifier,
                                              void *pNetworkServerInfo,
                                              void *pNetworkCredentialInfo,
                                              const IotNetworkInterface_t *pNetworkInterface );

void BLEMqttEchoDemoNetworkDisconnectedCallback( const IotNetworkInterface_t * pNetworkInterface );

int RunBLEMqttEchoDemo( bool awsIotMqttMode,
                        const char *pIdentifier,
                        void *pNetworkServerInfo,
                        void *pNetworkCredentialInfo,
                        const IotNetworkInterface_t *pNetworkInterface );

/* ------------------------------------------------------------------------------------------- */
static int _establishMqttConnection( bool awsIotMqttMode,
                                     const char * pIdentifier,
                                     void * pNetworkServerInfo,
                                     void * pNetworkCredentialInfo,
                                     const IotNetworkInterface_t * pNetworkInterface,
                                     IotMqttConnection_t * pMqttConnection );

/**
 * @brief Function to publish a message to the broker.
 */
static IotMqttError_t _publishMqttMessage( const char* pMessage, size_t messageLength );

/**
 * @brief Semaphore used by the main task to wait for a network connection before publishing MQTT messages.
 */
static IotSemaphore_t networkConnected;

/**
 * @brief Variable which holds the application state for the demo. Application is assumed to have a network
 * at the startup.
 */
static BLEMqttDemoState_t appState = BLE_MQTT_ECHO_DEMO_STATE_CONNECTING;

/**
 * @brief Global structure which wraps networking information for demo, updated intially and during a
 * new connection.
 */
static BleMqttDemoNetworkInfo_t demoNetworkInfo = { 0 };

/**
 * @brief  MQTT connection used to send/receive MQTT messages.
 */
static IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/*------------------------------------------------------------------------------------*/

static IotMqttError_t _publishMqttMessage( const char* pMessage, size_t messageLength )
{
     IotMqttError_t ret = IOT_MQTT_NETWORK_ERROR;
     IotMqttPublishInfo_t publishInfo =
     {
         .qos = _QOS,
         .pTopicName = _TOPIC,
         .topicNameLength = strlen(_TOPIC),
         .pPayload = (void *)pMessage,
         .payloadLength = messageLength
     };

     if( mqttConnection != IOT_MQTT_CONNECTION_INITIALIZER )
     {

         ret = IotMqtt_TimedPublish(mqttConnection,
                                    &publishInfo,
                                    0,
                                    _MQTT_TIMEOUT_MS);
     }
     
     return ret;
}

static void _receiveMqttMessage( void* pUserParam, IotMqttCallbackParam_t* pPublishParam )
{

    const char * pPayload = ( const char * ) pPublishParam->message.info.pPayload;
    size_t payloadLength = pPublishParam->message.info.payloadLength;
    char ack[ _ACK_LENGTH ] = { 0 };
     int ackLength;
    IotMqttError_t mqttStatus;
   

    /* User parameters are not used */
    ( void ) pUserParam;

    if( ( payloadLength < strlen( _ACK )  ) ||
         strncmp( ( pPayload + _ACK_INDEX( payloadLength ) ), _ACK, strlen( _ACK ) ) != 0 )
    {
        IotLogInfo( "Received ECHO message: %.*s", payloadLength, pPayload );
        /* This is not an ACK message */
        ackLength = snprintf( ack,  _ACK_LENGTH, "%.*s %s", payloadLength, pPayload, _ACK );
        if( ackLength > 0 )
        {
            mqttStatus = _publishMqttMessage(ack, ackLength);
            if (mqttStatus == IOT_MQTT_SUCCESS)
            {
                IotLogInfo( "Sent ACK message: %.*s", ackLength, ack );
            }
            else
            {
                IotLogError( "Failed to publish ACK message, error = %d", IotMqtt_strerror( mqttStatus ) );
                
            }
            
        }
        else
        {
            IotLogError( "Failed to create ACK message" );
        }
    }
}


static int _establishMqttConnection( bool awsIotMqttMode,
                                     const char * pIdentifier,
                                     void * pNetworkServerInfo,
                                     void * pNetworkCredentialInfo,
                                     const IotNetworkInterface_t * pNetworkInterface,
                                     IotMqttConnection_t * pMqttConnection )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    char pClientIdentifierBuffer[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };
    IotMqttSubscription_t subscription = { 0 };

    /* Set the members of the network info not set by the initializer. This
     * struct provided information on the transport layer to the MQTT connection. */
    networkInfo.createNetworkConnection = true;
    networkInfo.pNetworkServerInfo = pNetworkServerInfo;
    networkInfo.pNetworkCredentialInfo = pNetworkCredentialInfo;
    networkInfo.pNetworkInterface = pNetworkInterface;

    /* Set the members of the connection info not set by the initializer. */
    connectInfo.awsIotMqttMode = awsIotMqttMode;
    connectInfo.cleanSession = true;
    if( awsIotMqttMode == true )
    {
        /* Disable keep alive for non mqtt iot mode */
        connectInfo.keepAliveSeconds = _KEEP_ALIVE_SECONDS;
    }
    else
    {
        /* awsiotMqttMode is set to false for BLE network */
        networkInfo.pMqttSerializer = &IotBleMqttSerializer;
    }
    

    /* Use the parameter client identifier if provided. Otherwise, generate a
     * unique client identifier. */
    if( pIdentifier != NULL )
    {
        connectInfo.pClientIdentifier = pIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( pIdentifier );
    }
    else
    {
        /* Every active MQTT connection must have a unique client identifier. The demos
         * generate this unique client identifier by appending a timestamp to a common
         * prefix. */
        status = snprintf( pClientIdentifierBuffer,
                           _CLIENT_IDENTIFIER_MAX_LENGTH,
                           _CLIENT_IDENTIFIER_PREFIX "%lu",
                           ( long unsigned int ) IotClock_GetTimeMs() );

        /* Check for errors from snprintf. */
        if( status < 0 )
        {
            IotLogError( "Failed to generate unique client identifier for demo." );
            status = EXIT_FAILURE;
        }
        else
        {
            /* Set the client identifier buffer and length. */
            connectInfo.pClientIdentifier = pClientIdentifierBuffer;
            connectInfo.clientIdentifierLength = ( uint16_t ) status;

            status = EXIT_SUCCESS;
        }
    }

    /* Establish the MQTT connection. */
    if( status == EXIT_SUCCESS )
    {
        IotLogInfo( "MQTT demo client identifier is %.*s (length %hu).",
                    connectInfo.clientIdentifierLength,
                    connectInfo.pClientIdentifier,
                    connectInfo.clientIdentifierLength );

        mqttStatus = IotMqtt_Connect(&networkInfo,
                                     &connectInfo,
                                     _MQTT_TIMEOUT_MS,
                                     pMqttConnection);

        if( mqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT CONNECT returned error %s.",
                         IotMqtt_strerror( mqttStatus ) );

            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        subscription.qos                       = _QOS;
        subscription.pTopicFilter              = _TOPIC;
        subscription.topicFilterLength         = strlen( _TOPIC );
        subscription.callback.pCallbackContext = NULL;
        subscription.callback.function         = _receiveMqttMessage;

        mqttStatus = IotMqtt_TimedSubscribe(mqttConnection,
                                            &subscription,
                                            _TOPIC_FILTER_COUNT,
                                            0,
                                            _MQTT_TIMEOUT_MS);
        if( mqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT Subscribe returned error %s.",
                         IotMqtt_strerror( mqttStatus ) );

            status = EXIT_FAILURE;
        }
        
    }

    return status;
}

/* ------------------------------------------------------------------------------------ */

void BLEMqttEchoDemoNetworkConnectedCallback( bool awsIotMqttMode,
                                              const char *pIdentifier,
                                              void *pNetworkServerInfo,
                                              void *pNetworkCredentialInfo,
                                              const IotNetworkInterface_t *pNetworkInterface )
{
    if( appState == BLE_MQTT_ECHO_DEMO_STATE_NETWORK_DISCONNECTED )
    {
        demoNetworkInfo.awsIotMqttMode = awsIotMqttMode;
        demoNetworkInfo.pClientIdentifier = pIdentifier;
        demoNetworkInfo.pNetworkCredentialInfo = pNetworkCredentialInfo;
        demoNetworkInfo.pNetworkServerInfo = pNetworkServerInfo;
        demoNetworkInfo.pNetworkInterface = pNetworkInterface;
        IotSemaphore_Post( &networkConnected );
    }
}


void BLEMqttEchoDemoNetworkDisconnectedCallback( const IotNetworkInterface_t * pNetworkInterface )
{
    ( void ) pNetworkInterface;
    if( ( appState == BLE_MQTT_ECHO_DEMO_STATE_CONNECTING ) ||
        ( appState == BLE_MQTT_ECHO_DEMO_STATE_CONNECTED ) )
    {
        appState = BLE_MQTT_ECHO_DEMO_STATE_NETWORK_DISCONNECTED;
    }
}

int RunBLEMqttEchoDemo( bool awsIotMqttMode,
                        const char *pIdentifier,
                        void *pNetworkServerInfo,
                        void *pNetworkCredentialInfo,
                        const IotNetworkInterface_t *pNetworkInterface )
{
    uint32_t messageIdentifier = 1;
    char message[ _MESSAGE_LENGTH ];
    int32_t messageLength;
    IotMqttError_t mqttStatus;
    int i, ret = EXIT_SUCCESS;

    demoNetworkInfo.awsIotMqttMode = awsIotMqttMode;
    demoNetworkInfo.pClientIdentifier = pIdentifier;
    demoNetworkInfo.pNetworkServerInfo = pNetworkServerInfo;
    demoNetworkInfo.pNetworkCredentialInfo = pNetworkCredentialInfo;
    demoNetworkInfo.pNetworkInterface = pNetworkInterface;
    
    /* Create connection Semaphore to signal new connection from the network manager callback */
    if( IotSemaphore_Create( &networkConnected, 0, 1 ) != true )
    {
        ret = EXIT_FAILURE;
    }

    if( ret == EXIT_SUCCESS )
    {
        for( i = 0; i < _MAX_MESSAGES; i++ )
        {
            if( appState == BLE_MQTT_ECHO_DEMO_STATE_CONNECTING )
            {
                ret = _establishMqttConnection(demoNetworkInfo.awsIotMqttMode,
                                               demoNetworkInfo.pClientIdentifier,
                                               demoNetworkInfo.pNetworkServerInfo,
                                               demoNetworkInfo.pNetworkCredentialInfo,
                                               demoNetworkInfo.pNetworkInterface,
                                               &mqttConnection);
                if( ret == IOT_MQTT_SUCCESS )
                {
                    appState = BLE_MQTT_ECHO_DEMO_STATE_CONNECTED;
                }
                else
                {
                    IotClock_SleepMs( _CONNECTION_RETRY_DELAY_MS );
                }
                
            }
            else if( appState == BLE_MQTT_ECHO_DEMO_STATE_CONNECTED )
            {
                messageLength = snprintf(message,
                                         _MESSAGE_LENGTH,
                                         _MESSAGE,
                                         messageIdentifier);

                if( messageLength > 0 )
                {

                    mqttStatus = _publishMqttMessage(message, messageLength);
                    if( mqttStatus == IOT_MQTT_SUCCESS )
                    {
                        IotLogInfo( "Sent ECHO message: %.*s", messageLength, message );
                        messageIdentifier++;
                        IotClock_SleepMs(_MESSAGE_INTERVAL_MS);
                    }
                    else
                    {
                                 
                        if( ( mqttStatus == IOT_MQTT_NETWORK_ERROR ) || 
                            ( mqttStatus == IOT_MQTT_TIMEOUT ) || 
                            ( mqttStatus == IOT_MQTT_RETRY_NO_RESPONSE ) )
                        {
                            /* These MQTT errors are related to transient network issues
                               or a network disconnect. Close the MQTT connection and 
                               retry the connection. */
                            if( appState == BLE_MQTT_ECHO_DEMO_STATE_CONNECTED )
                            {
                                IotLogError( "Failed to send ECHO message, error = %d, retrying MQTT connection.", IotMqtt_strerror( mqttStatus ) );    
                                IotMqtt_Disconnect( mqttConnection, 0 );
                                appState = BLE_MQTT_ECHO_DEMO_STATE_CONNECTING;
                            }
                        }
                        else
                        {
                            /* All other MQTT errors causes the demo to exit with a failure return code. */
                            IotLogError( "Failed to send ECHO message, error = %d, exiting demo.", IotMqtt_strerror( mqttStatus ) ); 
                            appState = BLE_MQTT_ECHO_DEMO_STATE_ERROR;
                            ret = EXIT_FAILURE;
                            break;
                        }
                    }
                }
                else
                {
                    /* Failed to create an MQTT message buffer. Exit the demo with
                     * a failure return code.
                     */
                    IotLogError( "Failed to create an MQTT ECHO message, exiting demo." );
                    appState = BLE_MQTT_ECHO_DEMO_STATE_ERROR;
                    ret = EXIT_FAILURE;
                    break;
                }
            }
            else if( appState == BLE_MQTT_ECHO_DEMO_STATE_NETWORK_DISCONNECTED )
            {
                /* The physical network to which demo was connected is terminated. */
                
                if( mqttConnection != IOT_MQTT_CONNECTION_INITIALIZER )
                {
                    IotMqtt_Disconnect( mqttConnection, 0 );
                    mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
                }
                IotLogInfo( "Network disconnected. Waiting for a new network to be connected." );
                IotSemaphore_Wait( &networkConnected );
                appState = BLE_MQTT_ECHO_DEMO_STATE_CONNECTING;
            }
        }

        if( appState == BLE_MQTT_ECHO_DEMO_STATE_CONNECTED )
        {
            IotMqtt_Disconnect( mqttConnection, 0 );
            mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
        }

         IotSemaphore_Destroy( &networkConnected );
    }

    return ret;
}