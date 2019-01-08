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
 * @file aws_hello_world.c
 * @brief Simple Hello world double echo example.
 *
 * This application publishes MQTT "HelloWorld" messages to a topic using AWS Iot MQTT broker. It receives
 * acknowledgment packets from the broker and echoes back the message to the broker.
 *
 * The demo uses underlying network agnostic MQTT library to perform all MQTT operations. Hence the demo can
 * run over both a secured TCP/IP connection over WIFI network as well as over a secured BLE connection, using
 * a Mobile phone as MQTT proxy to the cloud.
 */

#include <stdbool.h>
#include <string.h>

/* Build using a config header, if provided. */
#include "aws_iot_demo.h"

/*
 * FreeRTOS header includes
 */
#include "FreeRTOS.h"
#include "semphr.h"

/* MQTT library header includes */
#include "aws_iot_mqtt.h"

/* POSIX and Platform layer includes. */
#include "platform/aws_iot_clock.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/pthread.h"

/* Network connection includes */
#include "aws_iot_network.h"
#include "aws_iot_demo_network.h"

#include "aws_clientcredential.h"

/**
 * @brief The topic that the MQTT client both subscribes and publishes to.
 */
#define echoTOPIC_NAME      "freertos/demos/echo"

#define echoDATA            "HelloWorld %d"

/**
 * @brief Dimension of the character array buffers used to hold data (strings in
 * this case) that is published to and received from the MQTT broker (in the cloud).
 */
#define echoMAX_DATA_LENGTH            ( sizeof( echoDATA ) + 4 )

/**
 * @brief The string appended to messages that are echoed back to the MQTT broker.
 *
 * It is also used to detect if a received message has already been acknowledged.
 */
#define echoACK_STRING                     " ACK"

#define echoACK_LENGTH                     ( 4 )

#define echoMAX_ACK_MSG_LENGTH             ( echoMAX_DATA_LENGTH + echoACK_LENGTH )

#define echoQOS                            ( 0 )

#define echoKEEPALIVE_SECONDS              ( 120 )

#define echoNUM_MESSAGES                   ( 120 )

#define echoMSG_INTERVAL_SECONDS           ( 1 )

#define echoMQTT_TIMEOUT_MS                ( 5000 )

#define echoCONN_RETRY_INTERVAL_SECONDS    ( 5 )

#define echoCONN_RETRY_LIMIT               ( 100 )

#define echoTASK_STACK_SIZE                ( configMINIMAL_STACK_SIZE * 7 +  echoMAX_DATA_LENGTH )

/*-----------------------------------------------------------*/
/**
 * @brief Callback invoked when a message is recieved from the cloud on the demo topic.
 *
 * If not already echoed, the function echoes back the same message by appending an ACK
 * to the original message. If it's an already echoed message, function discards the message.
 *
 * @param pvUserParam[in] User param for the callback
 * @param pxPublishParam[in] Publish param which contains the topic, the payload and other details.
 */
static void prvEchoMessage( void* pvUserParam, AwsIotMqttCallbackParam_t* pxPublishParam );

/**
 * @brief Opens a new MQTT connection with the IOT broker endpoint.
 *
 * Function first creates a network connection which can either be a Bluetooth Low Energy connection with
 * a compatible device or a secure socket connection over WIFI. It then performs MQTT connect with the broker
 * endpoint, and subscribes to the demo topic configured.
 *
 * @return true if all the steps succeeded.
 */
bool prbOpenMqttConnection( void );
/**
 * @brief Closes an MQTT connection with the broker endpoint.
 *
 * Function first closes the MQTT connection with the broker by optionally (set in the parameter) sending a
 * DISCONNECT message to the broker. It also tears down the physical connectivity betweend the device and the
 * broker.
 *
 * @param bSendDisconnect[in] Should send a DISCONNECT message to the broker.
 */
void prvCloseMqttConnection( bool bSendDisconnect );
/**
 * @brief Subscribes or unsubscribes to the demo topic.
 * @param bSubscribe[in] Set to true if its a subscribe or false for unsubscribe.
 *
 * @return true if operation was successful.
 */
AwsIotMqttError_t prxSubscribeorUnsubscribeToTopic( bool bSubscribe );
/**
 * @brief Publishes a message using QoS0 or QoS1 to the specified topic.
 *
 * @param pcMesg Pointer to the message
 * @param xLength Length of the message
 * @return AWS_IOT_MQTT_SUCCESS if the publish was successful.
 */
static AwsIotMqttError_t prxPublishMessage( const char* pcMesg, size_t xLength );

/**
 * @brief Thread used to publish all the messages to the topic.
 * @param pvParam[in] Thread parameter
 */
static void* prvPublishMessageThread( void* pvParam );

/**
 * @brief Creates and starts a publish message thread in detached state.
 * @return true if thread was started successfully.
 */
static bool prbStartPublishMessageThread( void);
/**
 * @brief Callback invoked when a DISCONNECT event was received for the connected network.
 *
 * @param xConnection The handle for the connection to the network.
 */
static void prvOnNetworkDisconnect( AwsIotDemoNetworkConnection_t xConnection );

/* Declaration of snprintf. The header stdio.h is not included because it
 * includes conflicting symbols on some platforms. */
extern int snprintf( char * pcS,
                     size_t xN,
                     const char * pcFormat,
                     ... );
/**
 * @brief Underlying network interface used for the MQTT connection.
 */
static AwsIotMqttNetIf_t xNetworkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
/**
 * @brief Underlying network Connection used for the MQTT connection.
 */
static AwsIotDemoNetworkConnection_t xNetworkConnection = NULL;

/**
 * @brief Handle to the MQTT connection.
 */
static AwsIotMqttConnection_t xMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief Global variable used to indiacte if the underlying connection is active.
 */
static bool xConnected = false;

AwsIotMqttError_t prxPublishMessage( const char* pcMesg, size_t xLength )
{
    AwsIotMqttPublishInfo_t xPublishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttReference_t xOperationLock = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    AwsIotMqttError_t xStatus;

    xPublishInfo.QoS = echoQOS;
    xPublishInfo.pTopicName = echoTOPIC_NAME;
    xPublishInfo.topicNameLength = strlen( echoTOPIC_NAME );

    xPublishInfo.pPayload = ( void * ) pcMesg;
    xPublishInfo.payloadLength = xLength;
    xPublishInfo.retryLimit = 0;
    xPublishInfo.retryMs = 0;

    if( echoQOS == 0 )
    {
        xStatus = AwsIotMqtt_Publish( xMqttConnection,
                &xPublishInfo,
                0,
                NULL,
                NULL );
    }
    else if( echoQOS == 1 || echoQOS == 2 )
    {
        xStatus = AwsIotMqtt_Publish( xMqttConnection,
                &xPublishInfo,
                AWS_IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );
        if( xStatus == AWS_IOT_MQTT_STATUS_PENDING )
        {
            xStatus = AwsIotMqtt_Wait( xOperationLock, echoMQTT_TIMEOUT_MS );
        }
    }

    return xStatus;
}

void prvEchoMessage( void* pvUserParam, AwsIotMqttCallbackParam_t* pxPublishParam )
{

    size_t xAckPos, xPayloadLen = pxPublishParam->message.info.payloadLength;
    const char *pcPayload = ( const char *) pxPublishParam->message.info.pPayload;
    char cAck[ echoMAX_ACK_MSG_LENGTH ] = { 0 };
    AwsIotMqttError_t xStatus;

    /* User params not used */
    ( void ) pvUserParam;

    xAckPos = xPayloadLen - echoACK_LENGTH;
    if( strncmp( ( pcPayload + xAckPos ), echoACK_STRING, echoACK_LENGTH ) != 0 )
    {
        AwsIotLogInfo( "Received: %.*s\n", xPayloadLen, pcPayload);

        if( xPayloadLen < echoMAX_DATA_LENGTH )
        {
            memcpy(cAck, pcPayload,  xPayloadLen );
            strcat( cAck, echoACK_STRING );
            xStatus = prxPublishMessage( cAck, strlen( cAck ) );
            if( xStatus != AWS_IOT_MQTT_SUCCESS )
            {
                AwsIotLogInfo(" Failed to send: %s, error = %s\n", cAck, AwsIotMqtt_strerror( xStatus ));
            }
            else
            {
                AwsIotLogInfo( "Sent: %s\n", cAck );
            }
        }
    }
}

AwsIotMqttError_t prxSubscribeorUnsubscribeToTopic( bool bSubscribe )
{
    AwsIotMqttReference_t xOperationLock = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    AwsIotMqttSubscription_t xSubscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttError_t xMqttStatus;

    xSubscription.QoS = echoQOS;
    xSubscription.callback.function = prvEchoMessage;
    xSubscription.callback.param1 = NULL;
    xSubscription.pTopicFilter = echoTOPIC_NAME;
    xSubscription.topicFilterLength = strlen( echoTOPIC_NAME );

    if( bSubscribe )
    {
        xMqttStatus = AwsIotMqtt_Subscribe(
                xMqttConnection,
                &xSubscription,
                1,
                AWS_IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );
    }
    else
    {
        xMqttStatus = AwsIotMqtt_Unsubscribe(
                xMqttConnection,
                &xSubscription,
                1,
                AWS_IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );

    }

    if( xMqttStatus == AWS_IOT_MQTT_STATUS_PENDING )
    {
        xMqttStatus = AwsIotMqtt_Wait( xOperationLock, echoMQTT_TIMEOUT_MS );
    }

    return xMqttStatus;
}

bool prbOpenMqttConnection( void )
{
    AwsIotMqttConnectInfo_t xConnectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    bool xStatus = false;

    AwsIotDemo_CreateNetworkConnection(
            &xNetworkInterface,
            &xMqttConnection,
            prvOnNetworkDisconnect,
            &xNetworkConnection,
            echoCONN_RETRY_INTERVAL_SECONDS,
            echoCONN_RETRY_LIMIT );

    if( xNetworkConnection != NULL )
    {
       /*
        * If the network type is BLE, MQTT library connects to the IoT broker using
        * a proxy device as intermediary. So set .awsIotMqttMode to false. Disable keep alive
        * by setting keep alive seconds to zero.
        */
        if( AwsIotDemo_GetNetworkType( xNetworkConnection ) == AWSIOT_NETWORK_TYPE_BLE )
        {
            xConnectInfo.awsIotMqttMode = false;
            xConnectInfo.keepAliveSeconds = 0;
        }
        else
        {
            xConnectInfo.awsIotMqttMode = true;
            xConnectInfo.keepAliveSeconds = echoKEEPALIVE_SECONDS;
        }

        xConnectInfo.cleanSession = true;
        xConnectInfo.clientIdentifierLength = strlen( clientcredentialIOT_THING_NAME );
        xConnectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;

        /* Connect to the IoT broker endpoint */
        if( AwsIotMqtt_Connect( &xMqttConnection,
                &xNetworkInterface,
                &xConnectInfo,
                NULL,
                echoMQTT_TIMEOUT_MS ) == AWS_IOT_MQTT_SUCCESS )
        {
            xStatus = true;
        }

        if( xStatus == true )
        {
            /* MQTT Connection succeeded, subscribe to the topic */
            if( prxSubscribeorUnsubscribeToTopic( true ) != AWS_IOT_MQTT_SUCCESS )
            {
                xStatus = false;
            }
        }

        if( xStatus == false )
        {
        	/* Close the MQTT connection to perform any cleanup */
        	prvCloseMqttConnection( true );
        }

    }

    return xStatus;

}

void prvCloseMqttConnection( bool bSendDisconnect )
{
    /* Close the MQTT connection either by sending a DISCONNECT operation or not */
    if( xMqttConnection != AWS_IOT_MQTT_CONNECTION_INITIALIZER )
    {
        AwsIotMqtt_Disconnect( xMqttConnection, !(bSendDisconnect) );
        xMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    }
    /* Delete the network connection */
    if( xNetworkConnection != NULL )
    {
    	AwsIotDemo_DeleteNetworkConnection( xNetworkConnection );
        xNetworkConnection = NULL;
    }
}

static void prvOnNetworkDisconnect( AwsIotDemoNetworkConnection_t xConnection )
{
    xConnected = false;
}

void* prvPublishMessageThread( void* pvParam )
{
    uint32_t ulMessageID;
    char cMessage[ echoMAX_DATA_LENGTH ];
    size_t xMessageLength;
    struct timespec xMsgInterval =
    {
            .tv_sec = echoMSG_INTERVAL_SECONDS,
            .tv_nsec = 0
    };
    AwsIotMqttError_t xError = AWS_IOT_MQTT_SUCCESS ;

    /* Avoid compiler warnings about the parameters */
    (void ) pvParam;


    xConnected = prbOpenMqttConnection();

    for( ulMessageID = 1; ulMessageID <= echoNUM_MESSAGES;  )
    {
        if( xConnected )
        {
            xMessageLength = snprintf( cMessage, echoMAX_DATA_LENGTH, echoDATA, (int)ulMessageID );
            xError = prxPublishMessage( cMessage, xMessageLength );
            if( xError == AWS_IOT_MQTT_SUCCESS)
            {
                AwsIotLogInfo( "Sent: %.*s", xMessageLength, cMessage );
                ulMessageID++;
                ( void ) clock_nanosleep( CLOCK_REALTIME, 0, &xMsgInterval, NULL );
            }
            else
            {
                if( ( xError == AWS_IOT_MQTT_NO_MEMORY ) || ( xError == AWS_IOT_MQTT_BAD_PARAMETER ) )
                {
                	AwsIotLogError( "Failed to publish Message, error = %s", AwsIotMqtt_strerror( xError ) );
                    break;
                }
                else
                {
                	/* Publish failed due to a timeout. Reopen the Mqtt connection */
                	prvCloseMqttConnection(false );
                	xConnected = prbOpenMqttConnection();
                }
            }
        }
        else
        {
            prvCloseMqttConnection( false );
            xConnected = prbOpenMqttConnection();
        }
    }

    if( xError == AWS_IOT_MQTT_SUCCESS )
    {
        if( prxSubscribeorUnsubscribeToTopic( false ) != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogWarn( "Failed to Unsubscribe from the topic" );
        }
        prvCloseMqttConnection( true );
    }
    else
    {
        AwsIotLogError(( "MQTT Echo demo failed " ));
    }

    pthread_exit( NULL );
    return NULL;
}

static bool prbStartPublishMessageThread( void )
{
    pthread_attr_t xAttr;
    int16_t sStatus;
    pthread_t xThread = NULL;

    sStatus = pthread_attr_init( &xAttr );
    if( sStatus < 0 )
    {
        AwsIotLogInfo("Failed to initialize pthread attribute, status = %d\n", sStatus);
        return false;
    }

    sStatus = pthread_attr_setdetachstate( &xAttr, PTHREAD_CREATE_DETACHED );
    if( sStatus < 0 )
    {
        pthread_attr_destroy( &xAttr );
        AwsIotLogInfo("Failed to set detached state for thread, status = %d\n", sStatus );
        return false;
    }

    sStatus = pthread_attr_setstacksize( &xAttr, echoTASK_STACK_SIZE );
    if( sStatus < 0 )
    {
        pthread_attr_destroy( &xAttr );
        AwsIotLogInfo("Failed to set detached state for thread, status = %d\n", sStatus );
        return false;
    }


    sStatus = pthread_create( &xThread, &xAttr, prvPublishMessageThread, NULL );
    if( sStatus < 0 )
    {
        pthread_attr_destroy( &xAttr );
        AwsIotLogInfo("Failed to create thread, status = %d\n", sStatus);
        return false;
    }

    (void) pthread_attr_destroy( &xAttr );
    return true;
}

void vStartMQTTBLEEchoDemo( void )
{
    if( prbStartPublishMessageThread() == false )
    {
        AwsIotLogInfo( "Failed to create thread for publish message\n");
        return;
    }
}
