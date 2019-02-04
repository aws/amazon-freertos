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
 * run over both a secured TCP/IP connection over WIFI network or over a secured BLE connection using
 * Mobile phone SDK as MQTT Proxy to the cloud.
 */

#include <stdbool.h>
#include <string.h>

/* Build using a config header, if provided. */
#include "aws_iot_demo.h"

/*
 * FreeRTOS includes.
 */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* MQTT library includes */
#include "aws_iot_mqtt.h"

/* Network connection includes */
#include "aws_iot_network_manager.h"
#include "aws_iot_demo_network.h"

/* Configuration includes */
#include "aws_demo_config.h"
#include "aws_clientcredential.h"

/**
 * @brief Transport types supported for MQTT Echo demo.
 */
#define echoDemoNETWORK_TYPES          ( AWSIOT_NETWORK_TYPE_BLE | AWSIOT_NETWORK_TYPE_WIFI )

/**
 * @brief The topic that the MQTT client both subscribes and publishes to.
 */
#define echoDemoMQTT_TOPIC              "freertos/demos/echo"

#define echoDemoPUBLISH_DATA            "HelloWorld %d"

/**
 * @brief Dimension of the character array buffers used to hold data (strings in
 * this case) that is published to and received from the MQTT broker (in the cloud).
 */
#define echoDemoPUBLISH_DATA_LENGTH            ( sizeof( echoDemoPUBLISH_DATA ) + 4 )

/**
 * @brief The string appended to messages that are echoed back to the MQTT broker.
 *
 * It is also used to detect if a received message has already been acknowledged.
 */
#define echoDEMO_ACK_STR                           " ACK"

#define echoDemoACK_STR_LENGTH                     ( 4 )

#define echoDemoACK_DATA_LENGTH                    ( echoDemoPUBLISH_DATA_LENGTH + echoDemoACK_STR_LENGTH )

#define echoDemoMQTT_QOS                           ( 1 )

#define echoDemoMQTT_KEEPALIVE_SECONDS             ( 120 )

#define echoDemoMAX_PUBLISH_MESSAGES               ( 120 )

#define echoDemoPUBLISH_INTERVAL_MS                ( 1000 )

#define echoDemoPUBLISH_TIMEOUT_DELAY_MS           ( 5000 )

#define echoDemoPUBLISH_TIMEOUT_RETRIES            ( 2 )

#define echoDemoPUBLISH_INTERVAL_MS                ( 1000 )

#define echoDemoMQTT_OPERATION_TIMEOUT_MS          ( 5000 )

#define echoDemoCONNECTION_RETRY_INTERVAL_SECONDS  ( 5 )

#define echoDemoCONNECTION_RETRY_LIMIT             ( 100 )


/*-----------------------------------------------------------*/
/**
 * @brief Callback invoked when a message is received from the cloud on the demo topic.
 *
 * If not already echoed, the function echoes back the same message by appending an ACK
 * to the original message. If it's an already echoed message, function discards the message.
 *
 * @param pvUserParam[in] User param for the callback
 * @param pxPublishParam[in] Publish param which contains the topic, the payload and other details.
 */
static void prvEchoMessage( void* pvUserParam, AwsIotMqttCallbackParam_t* pxPublishParam );


/**
 * @brief Create the network connection.
 *
 * Waits for a physical transport type to available and then creates a new network connection.
 *
 * @param pxConnection Pointer to hold the connection.
 * @return New network type connected.
 */
 static BaseType_t prxCreateNetworkConnection( void );

/**
 * @brief Recreates the network connection.
 *
 * Tears down the existing network connection, waits for a physical transport type to available and then creates
 * a new network connection.
 *
 * @param pxConnection Pointer to hold the connection.
 * @return New network type connected.
 */
 static BaseType_t prxReCreateConnection( void );

/**
 * @brief Opens a new MQTT connection with the given physical transport type.
 *
 * Function first creates an MQTT connection with an IOT broker endpoint over a physical transport type which
 * can be either WIFI or Bluetooth Low energy.
 *
 * @return true if all the steps succeeded.
 */
 static AwsIotMqttError_t prxOpenMqttConnection( void );

 /**
  * @brief Subscribes or unsubscribes to the demo topic.
  * @param bSubscribe[in] Set to true if its a subscribe or false for unsubscribe.
  *
  * @return true if operation was successful.
  */
 static AwsIotMqttError_t prxSubscribeorUnsubscribeToTopic( BaseType_t xSubscribe );

/**
 * @brief Closes an MQTT connection with the broker endpoint.
 *
 * Function first closes the MQTT connection with the broker by optionally (set in the parameter) sending a
 * DISCONNECT message to the broker. It also tears down the physical connectivity between the device and the
 * broker.
 *
 * @param bSendDisconnect[in] Should send a DISCONNECT message to the broker.
 */
static void prvCloseMqttConnection( BaseType_t bCleanupOnly );

/**
 * @brief Publishes a message using QoS0 or QoS1 to the specified topic.
 *
 * @param pcMesg Pointer to the message
 * @param xLength Length of the message
 * @return AWS_IOT_MQTT_SUCCESS if the publish was successful.
 */
static AwsIotMqttError_t prxPublishMQTTMessage( const char* pcMesg, size_t xLength );

/**
 * @brief Main task used to publish MQTT messages to the cloud. Tasks waits for a network connection,
 * creates an MQTT connection over the network connection and then loops through to publish the MQTT
 * messages to cloud. It also handles timeouts and network disconnects and then recreates the network
 * connection.
 *
 * @param pvParam[in] Parameter to the task.
 */
static void prvMqttPublishTask( void* pvParam );


static void prvNetworkStateChangeCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext );

/**
 * @brief Callback invoked when a disconnect event was received for the connection.
 *
 * @param xConnection The handle for the connection to the network.
 */
static void prvNetworkDisconnectCallback( void* pvContext );

/* Declaration of snprintf. The header stdio.h is not included because it
 * includes conflicting symbols on some platforms. */
extern int snprintf( char * pcS,
                     size_t xN,
                     const char * pcFormat,
                     ... );

/**
 * @brief Network manager subscription
 */
static SubscriptionHandle_t xSubscriptionHandle = AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER;

/**
 * @brief Structure which holds the context for an MQTT connection within Demo.
 */
static MqttConnectionContext_t xConnection =
{
     .pvNetworkConnection = NULL,
     .ulNetworkType       = AWSIOT_NETWORK_TYPE_NONE,
     .xNetworkInterface   = AWS_IOT_MQTT_NETIF_INITIALIZER,
     .xMqttConnection     = AWS_IOT_MQTT_CONNECTION_INITIALIZER,
     .xDisconnectCallback = prvNetworkDisconnectCallback
};

/**
 * @brief Semaphore to indicate a new network is available.
 */
static SemaphoreHandle_t xNetworkAvailableLock = NULL;

/**
 * @brief Flag used to unset, during disconnection of currently connected network. This will
 * trigger a reconnection from the main MQTT task.
 */
static BaseType_t xNetworkConnected = pdFALSE;


AwsIotMqttError_t prxPublishMQTTMessage( const char* pcMesg, size_t xLength )
{
    AwsIotMqttPublishInfo_t xPublishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttReference_t xOperationLock = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    AwsIotMqttError_t xStatus;

    xPublishInfo.QoS = echoDemoMQTT_QOS;
    xPublishInfo.pTopicName = echoDemoMQTT_TOPIC;
    xPublishInfo.topicNameLength = strlen( echoDemoMQTT_TOPIC );

    xPublishInfo.pPayload = ( void * ) pcMesg;
    xPublishInfo.payloadLength = xLength;
    xPublishInfo.retryLimit = 0;
    xPublishInfo.retryMs = 0;

    if( echoDemoMQTT_QOS == 0 )
    {
        xStatus = AwsIotMqtt_Publish(
        		xConnection.xMqttConnection,
                &xPublishInfo,
                0,
                NULL,
                NULL );
    }
    else if( echoDemoMQTT_QOS == 1 || echoDemoMQTT_QOS == 2 )
    {
        xStatus = AwsIotMqtt_Publish(
        		xConnection.xMqttConnection,
                &xPublishInfo,
                AWS_IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );
        if( xStatus == AWS_IOT_MQTT_STATUS_PENDING )
        {
            xStatus = AwsIotMqtt_Wait( xOperationLock, echoDemoMQTT_OPERATION_TIMEOUT_MS );
        }
    }

    return xStatus;
}

void prvEchoMessage( void* pvUserParam, AwsIotMqttCallbackParam_t* pxPublishParam )
{

    size_t xAckPos, xPayloadLen = pxPublishParam->message.info.payloadLength;
    const char *pcPayload = ( const char *) pxPublishParam->message.info.pPayload;
    char cAck[ echoDemoACK_DATA_LENGTH ] = { 0 };
    AwsIotMqttError_t xStatus;

    /* User parameters are not used */
    ( void ) pvUserParam;

    xAckPos = xPayloadLen - echoDemoACK_STR_LENGTH;
    if( strncmp( ( pcPayload + xAckPos ), echoDEMO_ACK_STR, echoDemoACK_STR_LENGTH ) != 0 )
    {
        AwsIotLogInfo( "Received Message: %.*s.", xPayloadLen, pcPayload);

        if( xPayloadLen < echoDemoPUBLISH_DATA_LENGTH )
        {
            memcpy(cAck, pcPayload,  xPayloadLen );
            strcat( cAck, echoDEMO_ACK_STR );
            xStatus = prxPublishMQTTMessage( cAck, strlen( cAck ) );
            if( xStatus != AWS_IOT_MQTT_SUCCESS )
            {
                AwsIotLogInfo(" Failed to send ACK Message, reason: %s.", AwsIotMqtt_strerror( xStatus ));
            }
            else
            {
                AwsIotLogInfo( "Sent ACK message: %s.\n", cAck );
            }
        }
    }
}

static void prvNetworkStateChangeCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext )
{
    if( xNetworkState == eNetworkStateEnabled )
    {
        /* Release the semaphore, to indicate other tasks that a network is available */
        xSemaphoreGive( xNetworkAvailableLock );
    }
    else if ( xNetworkState == eNetworkStateDisabled )
    {
       if( ( AwsIotNetworkManager_GetConnectedNetworks()
               & echoDemoNETWORK_TYPES ) == AWSIOT_NETWORK_TYPE_NONE )
        {
            /* Take the semaphore if not taken already */
            xSemaphoreTake( xNetworkAvailableLock, 0 );
        }

        /* If the publish task is already connected to this network, set connected network flag to none,
         * to trigger a reconnect.
         */
        if( xConnection.ulNetworkType == ulNetworkType )
        {
            xNetworkConnected = pdFALSE;
        }
    }
}

static void prvNetworkDisconnectCallback( void* pvContext )
{
    ( void ) pvContext;
    xNetworkConnected = pdFALSE;
}

static BaseType_t prxCreateNetworkConnection( void )
{

    BaseType_t xRet = pdFALSE;

    /* If no networks are available, block for a physical network connection */
    if( ( AwsIotNetworkManager_GetConnectedNetworks() & echoDemoNETWORK_TYPES ) == 0 )
    {
        /* Block for a Network Connection. */
        AwsIotLogInfo( "Waiting for a network connection.");
        xSemaphoreTake( xNetworkAvailableLock, portMAX_DELAY );
    }

    /* At least one network type is available. Connect to the network type. */
    xRet = xMqttDemoCreateNetworkConnection(
    		&xConnection,
			echoDemoNETWORK_TYPES,
            echoDemoCONNECTION_RETRY_INTERVAL_SECONDS,
            echoDemoCONNECTION_RETRY_LIMIT );

    return xRet;
}


static BaseType_t prxReCreateConnection( void )
{
    vMqttDemoDeleteNetworkConnection( &xConnection );
    return prxCreateNetworkConnection();
}

static AwsIotMqttError_t prxOpenMqttConnection()
{

    AwsIotMqttConnectInfo_t xConnectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttError_t xMqttStatus;

    if( xConnection.ulNetworkType == AWSIOT_NETWORK_TYPE_BLE )
    {
        xConnectInfo.awsIotMqttMode = false;
        xConnectInfo.keepAliveSeconds = 0;
    }
    else
    {
        xConnectInfo.awsIotMqttMode = true;
        xConnectInfo.keepAliveSeconds = echoDemoMQTT_KEEPALIVE_SECONDS;
    }

    xConnectInfo.cleanSession = true;
    xConnectInfo.clientIdentifierLength = strlen( clientcredentialIOT_THING_NAME );
    xConnectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;

    /* Connect to the IoT broker endpoint */
    xMqttStatus = AwsIotMqtt_Connect(
            &( xConnection.xMqttConnection ),
            &( xConnection.xNetworkInterface ),
            &xConnectInfo,
            NULL,
            echoDemoMQTT_OPERATION_TIMEOUT_MS );


    if( xMqttStatus == AWS_IOT_MQTT_SUCCESS )
    {
        /* MQTT Connection succeeded, subscribe to the topic */
        xMqttStatus = prxSubscribeorUnsubscribeToTopic( pdTRUE );
    }

    if( xMqttStatus != AWS_IOT_MQTT_SUCCESS )
    {
        /* Close the MQTT connection to perform any cleanup */
        prvCloseMqttConnection( pdFALSE );
    }

    return xMqttStatus;
}

static AwsIotMqttError_t prxSubscribeorUnsubscribeToTopic( BaseType_t xSubscribe )
{
    AwsIotMqttReference_t xOperationLock = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    AwsIotMqttSubscription_t xSubscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttError_t xMqttStatus;

    xSubscription.QoS = echoDemoMQTT_QOS;
    xSubscription.callback.function = prvEchoMessage;
    xSubscription.callback.param1 = NULL;
    xSubscription.pTopicFilter = echoDemoMQTT_TOPIC;
    xSubscription.topicFilterLength = strlen( echoDemoMQTT_TOPIC );

    if( xSubscribe )
    {
        xMqttStatus = AwsIotMqtt_Subscribe(
                xConnection.xMqttConnection,
                &xSubscription,
                1,
                AWS_IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );
    }
    else
    {
        xMqttStatus = AwsIotMqtt_Unsubscribe(
        		xConnection.xMqttConnection,
                &xSubscription,
                1,
                AWS_IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );

    }

    if( xMqttStatus == AWS_IOT_MQTT_STATUS_PENDING )
    {
        xMqttStatus = AwsIotMqtt_Wait( xOperationLock, echoDemoMQTT_OPERATION_TIMEOUT_MS );
    }

    return xMqttStatus;
}

void prvCloseMqttConnection( BaseType_t xCleanupOnly )
{
    /* Close the MQTT connection either by sending a DISCONNECT operation or not */
    if( xConnection.xMqttConnection != AWS_IOT_MQTT_CONNECTION_INITIALIZER )
    {
        AwsIotMqtt_Disconnect( xConnection.xMqttConnection, xCleanupOnly );
        xConnection.xMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    }
}

void prvMqttPublishTask( void* pvParam )
{

    uint32_t ulPublishCount = 0, ulPublishRetriesLeft = echoDemoPUBLISH_TIMEOUT_RETRIES;
    char cMessage[ echoDemoPUBLISH_DATA_LENGTH ];
    size_t xMessageLength;

    TickType_t xPublishRetryDelay = pdMS_TO_TICKS( echoDemoPUBLISH_TIMEOUT_DELAY_MS );
    TickType_t xPublishDelay = pdMS_TO_TICKS( echoDemoPUBLISH_INTERVAL_MS );

    AwsIotMqttError_t xMqttStatus = AWS_IOT_MQTT_SUCCESS ;

    /* Avoid compiler warnings about the parameters */
    (void ) pvParam;

    /* Creates a physical networks connection */
    xNetworkConnected = prxCreateNetworkConnection();
    if( xNetworkConnected )
    {
        /* Open an MQTT connection */
        xMqttStatus = prxOpenMqttConnection();
        if( xMqttStatus == AWS_IOT_MQTT_SUCCESS )
        {
            /* Start publishing MQTT Messages in a loop */
            while( ulPublishCount < echoDemoMAX_PUBLISH_MESSAGES )
            {
                if( xNetworkConnected )
                {
                    xMessageLength = snprintf(
                            cMessage,
                            echoDemoPUBLISH_DATA_LENGTH,
                            echoDemoPUBLISH_DATA,
                            ( int ) ( ulPublishCount + 1 ) );

                    xMqttStatus = prxPublishMQTTMessage( cMessage, xMessageLength );

                    if( xMqttStatus ==  AWS_IOT_MQTT_SUCCESS )
                    {
                        AwsIotLogInfo( "Published Message: %.*s.", xMessageLength, cMessage );
                        ulPublishCount++;
                        ulPublishRetriesLeft = echoDemoPUBLISH_TIMEOUT_RETRIES;
                        vTaskDelay( xPublishDelay );
                    }
                    else if( ( xMqttStatus == AWS_IOT_MQTT_SEND_ERROR  )
                            || ( xMqttStatus == AWS_IOT_MQTT_TIMEOUT ) )
                    {
                        AwsIotLogInfo( "Published failed, reason: %s.",  AwsIotMqtt_strerror( xMqttStatus ) );

                        if( ulPublishRetriesLeft > 0 )
                        {
                            ulPublishRetriesLeft--;
                            AwsIotLogInfo( "Retrying publish, Number of retries left before reconnecting: %d.", ulPublishRetriesLeft );
                            vTaskDelay( xPublishRetryDelay );
                        }
                        else
                        {
                            /* Set connected network to none to trigger a reconnection, as at this point we assume network is dead */
                            AwsIotLogInfo( "Retry failed %d times, recreating the network connection.",echoDemoPUBLISH_TIMEOUT_RETRIES );
                            xNetworkConnected = pdFALSE;
                        }
                    }
                    else
                    {
                        AwsIotLogError( "Publish failed due to reason: %s, exiting demo.", AwsIotMqtt_strerror( xMqttStatus ) );
                        break;
                    }
                }
                else
                {
                    /* Recreate the Network Connection */
                    prvCloseMqttConnection( pdTRUE );
                    xNetworkConnected = prxReCreateConnection();

                    /* Create an MQTT connection over the network connection */
                    xMqttStatus = prxOpenMqttConnection();
                    if( xMqttStatus != AWS_IOT_MQTT_SUCCESS )
                    {
                        AwsIotLogError("Failed to create an MQTT connection.");
                        break;
                    }
                }
            }
        }
        else
        {
            AwsIotLogError("Failed to create an MQTT connection.");
        }

        if( xMqttStatus == AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogInfo( "Demo successful" );
            prxSubscribeorUnsubscribeToTopic( pdFALSE );
            prvCloseMqttConnection( pdFALSE );
        }
    }
    else
    {
        AwsIotLogError("Failed to create a network connection.");
    }

    if( xNetworkConnected )
    {
        vMqttDemoDeleteNetworkConnection( &xConnection );
    }

    AwsIotNetworkManager_RemoveSubscription( xSubscriptionHandle );
    vSemaphoreDelete( xNetworkAvailableLock );
    vTaskDelete( NULL );
}


void vStartMQTTBLEEchoDemo( void )
{

    BaseType_t xRet = pdTRUE;

    if( echoDemoNETWORK_TYPES == AWSIOT_NETWORK_TYPE_NONE )
    {
        AwsIotLogError(( "There are no networks configured for the demo." ));
        xRet = pdFALSE;
    }

    /* Create semaphore to notify network available */
    if( xRet == pdTRUE )
    {
        xNetworkAvailableLock = xSemaphoreCreateBinary();
        if( xNetworkAvailableLock == NULL )
        {
            AwsIotLogError(( "Failed to create semaphore." ));
            xRet = pdFALSE;
        }
    }

    /**
     * Create a Network Manager Subscription for all the network types supported.
     */
    if( xRet == pdTRUE )
    {
        xRet = AwsIotNetworkManager_SubscribeForStateChange( echoDemoNETWORK_TYPES, prvNetworkStateChangeCallback, NULL, &xSubscriptionHandle );
        if( xRet == pdFALSE )
        {
            AwsIotLogError(( "Failed to create Network Manager subscription." ));
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate(
                prvMqttPublishTask,
                "MQTTEcho",                          /* The name to assign to the task being created. */
                democonfigMQTT_ECHO_TASK_STACK_SIZE, /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                NULL,                                /* The task parameter is not being used. */
                democonfigMQTT_ECHO_TASK_PRIORITY,   /* The priority at which the task being created will run. */
                NULL );                             /* Not storing the task's handle. */

        if( xRet == pdFALSE )
        {
            AwsIotLogError(( "Failed to subscribe for network state change callback." ));
        }

    }

    if(  xRet == pdFALSE )
    {
        if( xSubscriptionHandle != AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER )
        {
            AwsIotNetworkManager_RemoveSubscription( xSubscriptionHandle );
        }

        if( xNetworkAvailableLock != NULL )
        {
            vSemaphoreDelete( xNetworkAvailableLock );
        }
    }
}
