/*
 * Amazon FreeRTOS V201906.00 Major
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

/**
 * @file iot_demo_aiot.c
 * @brief Demonstrates usage of the MQTT and ESP-WHO library.
 */

/* The config header is always included first. */
#include "iot_config.h"
#include "aiot_esp_config.h"

/* FreeRTOS includes */
#include "freertos/queue.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* MQTT include. */
#include "iot_mqtt.h"

/* AIoT include */
#include "sdkconfig.h"
#include "app_camera.h"
#include "app_facenet.h"
#include "app_speech_srcif.h"


/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration settings.
 */
#ifndef IOT_DEMO_MQTT_TOPIC_PREFIX
    #define IOT_DEMO_MQTT_TOPIC_PREFIX    "SmartLock"
#endif
/** @endcond */

#define IOT_DEMO_MQTT_PUBLISH_BURST_SIZE     ( 1 )

#define IOT_DEMO_MQTT_PUBLISH_BURST_COUNT    ( 1 )

/* Validate MQTT demo configuration settings. */
#if IOT_DEMO_MQTT_PUBLISH_BURST_SIZE <= 0
    #error "IOT_DEMO_MQTT_PUBLISH_BURST_SIZE cannot be 0 or negative."
#endif
#if IOT_DEMO_MQTT_PUBLISH_BURST_COUNT <= 0
    #error "IOT_DEMO_MQTT_PUBLISH_BURST_COUNT cannot be 0 or negative."
#endif

/**
 * @brief The first characters in the client identifier. A timestamp is appended
 * to this prefix to create a unique client identifer.
 *
 * This prefix is also used to generate topic names and topic filters used in this
 * demo.
 */
#define CLIENT_IDENTIFIER_PREFIX                 "iotdemo"

/**
 * @brief The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 * terminator.
 */
#define CLIENT_IDENTIFIER_MAX_LENGTH             ( 24 )

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define KEEP_ALIVE_SECONDS                       ( 60 )

/**
 * @brief The timeout for MQTT operations in this demo.
 */
#define MQTT_TIMEOUT_MS                          ( 5000 )

/**
 * @brief The Last Will and Testament topic name in this demo.
 *
 * The MQTT server will publish a message to this topic name if this client is
 * unexpectedly disconnected.
 */
#define WILL_TOPIC_NAME                          IOT_DEMO_MQTT_TOPIC_PREFIX "/will"

/**
 * @brief The length of #WILL_TOPIC_NAME.
 */
#define WILL_TOPIC_NAME_LENGTH                   ( ( uint16_t ) ( sizeof( WILL_TOPIC_NAME ) - 1 ) )

/**
 * @brief The message to publish to #WILL_TOPIC_NAME.
 */
#define WILL_MESSAGE                             "MQTT demo unexpectedly disconnected."

/**
 * @brief The length of #WILL_MESSAGE.
 */
#define WILL_MESSAGE_LENGTH                      ( ( size_t ) ( sizeof( WILL_MESSAGE ) - 1 ) )

/**
 * @brief How many topic filters will be used in this demo.
 */
#define TOPIC_FILTER_COUNT                       ( 1 )

/**
 * @brief The length of each topic filter.
 *
 * For convenience, all topic filters are the same length.
 */
#define TOPIC_FILTER_LENGTH                      ( ( uint16_t ) ( sizeof( IOT_DEMO_MQTT_TOPIC_PREFIX "/Logs" ) - 1 ) )

/**
 * @brief Format string of the PUBLISH messages in this demo.
 */
#define PUBLISH_PAYLOAD_FORMAT                   "{\"message\": \"Subject %d on the door\", \"intruder\": %d, \"datetime\": \"test200\"}"

/**
 * @brief Size of the buffer that holds the PUBLISH messages in this demo.
 */
#define PUBLISH_PAYLOAD_BUFFER_LENGTH            ( sizeof( PUBLISH_PAYLOAD_FORMAT ) + 2 )

/**
 * @brief The maximum number of times each PUBLISH in this demo will be retried.
 */
#define PUBLISH_RETRY_LIMIT                      ( 10 )

/**
 * @brief A PUBLISH message is retried if no response is received within this
 * time.
 */
#define PUBLISH_RETRY_MS                         ( 1000 )

/**
 * @brief The topic name on which acknowledgement messages for incoming publishes
 * should be published.
 */
#define ACKNOWLEDGEMENT_TOPIC_NAME               IOT_DEMO_MQTT_TOPIC_PREFIX "/acknowledgements"

/**
 * @brief The length of #ACKNOWLEDGEMENT_TOPIC_NAME.
 */
#define ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH        ( ( uint16_t ) ( sizeof( ACKNOWLEDGEMENT_TOPIC_NAME ) - 1 ) )

/**
 * @brief Format string of PUBLISH acknowledgement messages in this demo.
 */
#define ACKNOWLEDGEMENT_MESSAGE_FORMAT           "Client has received PUBLISH %.*s from server."

/**
 * @brief Size of the buffers that hold acknowledgement messages in this demo.
 */
#define ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH    ( sizeof( ACKNOWLEDGEMENT_MESSAGE_FORMAT ) + 2 )

/*-----------------------------------------------------------*/

/* Declaration of demo function. */
int RunAIoTDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

QueueHandle_t xResultQueue;

en_fsm_state g_state = WAIT_FOR_WAKEUP;

static face_id_list id_list = { 0 };
/*-----------------------------------------------------------*/

/**
 * @brief Called by the MQTT library when an operation completes.
 *
 * The demo uses this callback to determine the result of PUBLISH operations.
 * @param[in] param1 The number of the PUBLISH that completed, passed as an intptr_t.
 * @param[in] pOperation Information about the completed operation passed by the
 * MQTT library.
 */
static void _operationCompleteCallback( void * param1,
                                        IotMqttCallbackParam_t * const pOperation )
{
    intptr_t publishCount = ( intptr_t ) param1;

    /* Silence warnings about unused variables. publishCount will not be used if
     * logging is disabled. */
    ( void ) publishCount;

    /* Print the status of the completed operation. A PUBLISH operation is
     * successful when transmitted over the network. */
    if( pOperation->u.operation.result == IOT_MQTT_SUCCESS )
    {
        IotLogInfo( "MQTT %s %d successfully sent.",
                    IotMqtt_OperationType( pOperation->u.operation.type ),
                    ( int ) publishCount );
    }
    else
    {
        IotLogError( "MQTT %s %d could not be sent. Error %s.",
                     IotMqtt_OperationType( pOperation->u.operation.type ),
                     ( int ) publishCount,
                     IotMqtt_strerror( pOperation->u.operation.result ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Called by the MQTT library when an incoming PUBLISH message is received.
 *
 * The demo uses this callback to handle incoming PUBLISH messages. This callback
 * prints the contents of an incoming message and publishes an acknowledgement
 * to the MQTT server.
 * @param[in] param1 Counts the total number of received PUBLISH messages. This
 * callback will increment this counter.
 * @param[in] pPublish Information about the incoming PUBLISH message passed by
 * the MQTT library.
 */
static void _mqttSubscriptionCallback( void * param1,
                                       IotMqttCallbackParam_t * const pPublish )
{
    int acknowledgementLength = 0;
    size_t messageNumberIndex = 0, messageNumberLength = 1;
    IotSemaphore_t * pPublishesReceived = ( IotSemaphore_t * ) param1;
    const char * pPayload = pPublish->u.message.info.pPayload;
    char pAcknowledgementMessage[ ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH ] = { 0 };
    IotMqttPublishInfo_t acknowledgementInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo( "Incoming PUBLISH received:\r\n"
                "Subscription topic filter: %.*s\r\n"
                "Publish topic name: %.*s\r\n"
                "Publish retain flag: %d\r\n"
                "Publish QoS: %d\r\n"
                "Publish payload: %.*s",
                pPublish->u.message.topicFilterLength,
                pPublish->u.message.pTopicFilter,
                pPublish->u.message.info.topicNameLength,
                pPublish->u.message.info.pTopicName,
                pPublish->u.message.info.retain,
                pPublish->u.message.info.qos,
                pPublish->u.message.info.payloadLength,
                pPayload );

    /* Find the message number inside of the PUBLISH message. */
    for( messageNumberIndex = 0; messageNumberIndex < pPublish->u.message.info.payloadLength; messageNumberIndex++ )
    {
        /* The payload that was published contained ASCII characters, so find
         * beginning of the message number by checking for ASCII digits. */
        if( ( pPayload[ messageNumberIndex ] >= '0' ) &&
            ( pPayload[ messageNumberIndex ] <= '9' ) )
        {
            break;
        }
    }

    /* Check that a message number was found within the PUBLISH payload. */
    if( messageNumberIndex < pPublish->u.message.info.payloadLength )
    {
        /* Compute the length of the message number. */
        while( ( messageNumberIndex + messageNumberLength < pPublish->u.message.info.payloadLength ) &&
               ( *( pPayload + messageNumberIndex + messageNumberLength ) >= '0' ) &&
               ( *( pPayload + messageNumberIndex + messageNumberLength ) <= '9' ) )
        {
            messageNumberLength++;
        }

        /* Generate an acknowledgement message. */
        acknowledgementLength = snprintf( pAcknowledgementMessage,
                                          ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH,
                                          ACKNOWLEDGEMENT_MESSAGE_FORMAT,
                                          ( int ) messageNumberLength,
                                          pPayload + messageNumberIndex );

        /* Check for errors from snprintf. */
        if( acknowledgementLength < 0 )
        {
            IotLogWarn( "Failed to generate acknowledgement message for PUBLISH *.*s.",
                        ( int ) messageNumberLength,
                        pPayload + messageNumberIndex );
        }
        else
        {
            /* Set the members of the publish info for the acknowledgement message. */
            acknowledgementInfo.qos = IOT_MQTT_QOS_1;
            acknowledgementInfo.pTopicName = ACKNOWLEDGEMENT_TOPIC_NAME;
            acknowledgementInfo.topicNameLength = ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH;
            acknowledgementInfo.pPayload = pAcknowledgementMessage;
            acknowledgementInfo.payloadLength = ( size_t ) acknowledgementLength;
            acknowledgementInfo.retryMs = PUBLISH_RETRY_MS;
            acknowledgementInfo.retryLimit = PUBLISH_RETRY_LIMIT;

            /* Send the acknowledgement for the received message. This demo program
             * will not be notified on the status of the acknowledgement because
             * neither a callback nor IOT_MQTT_FLAG_WAITABLE is set. However,
             * the MQTT library will still guarantee at-least-once delivery (subject
             * to the retransmission strategy) because the acknowledgement message is
             * sent at QoS 1. */
            if( IotMqtt_Publish( pPublish->mqttConnection,
                                 &acknowledgementInfo,
                                 0,
                                 NULL,
                                 NULL ) == IOT_MQTT_STATUS_PENDING )
            {
                IotLogInfo( "Acknowledgment message for PUBLISH %.*s will be sent.",
                            ( int ) messageNumberLength,
                            pPayload + messageNumberIndex );
            }
            else
            {
                IotLogWarn( "Acknowledgment message for PUBLISH %.*s will NOT be sent.",
                            ( int ) messageNumberLength,
                            pPayload + messageNumberIndex );
            }
        }
    }

    /* Increment the number of PUBLISH messages received. */
    IotSemaphore_Post( pPublishesReceived );
}

/*-----------------------------------------------------------*/

/**
 * @brief Initialize the MQTT library.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */
static int _initializeDemo( void )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;

    mqttInitStatus = IotMqtt_Init();

    if( mqttInitStatus != IOT_MQTT_SUCCESS )
    {
        /* Failed to initialize MQTT library. */
        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Clean up the MQTT library.
 */
static void _cleanupDemo( void )
{
    IotMqtt_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Establish a new connection to the MQTT server.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
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
static int _establishMqttConnection( bool awsIotMqttMode,
                                     const char * pIdentifier,
                                     void * pNetworkServerInfo,
                                     void * pNetworkCredentialInfo,
                                     const IotNetworkInterface_t * pNetworkInterface,
                                     IotMqttConnection_t * pMqttConnection )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t connectStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    char pClientIdentifierBuffer[ CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };

    /* Set the members of the network info not set by the initializer. This
     * struct provided information on the transport layer to the MQTT connection. */
    networkInfo.createNetworkConnection = true;
    networkInfo.u.setup.pNetworkServerInfo = pNetworkServerInfo;
    networkInfo.u.setup.pNetworkCredentialInfo = pNetworkCredentialInfo;
    networkInfo.pNetworkInterface = pNetworkInterface;

    #if ( IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1 ) && defined( IOT_DEMO_MQTT_SERIALIZER )
        networkInfo.pMqttSerializer = IOT_DEMO_MQTT_SERIALIZER;
    #endif

    /* Set the members of the connection info not set by the initializer. */
    connectInfo.awsIotMqttMode = awsIotMqttMode;
    connectInfo.cleanSession = true;
    connectInfo.keepAliveSeconds = KEEP_ALIVE_SECONDS;
    connectInfo.pWillInfo = &willInfo;

    /* Set the members of the Last Will and Testament (LWT) message info. The
     * MQTT server will publish the LWT message if this client disconnects
     * unexpectedly. */
    willInfo.pTopicName = WILL_TOPIC_NAME;
    willInfo.topicNameLength = WILL_TOPIC_NAME_LENGTH;
    willInfo.pPayload = WILL_MESSAGE;
    willInfo.payloadLength = WILL_MESSAGE_LENGTH;

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
                           CLIENT_IDENTIFIER_MAX_LENGTH,
                           CLIENT_IDENTIFIER_PREFIX "%lu",
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

        connectStatus = IotMqtt_Connect( &networkInfo,
                                         &connectInfo,
                                         MQTT_TIMEOUT_MS,
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
 * @brief Add or remove subscriptions by either subscribing or unsubscribing.
 *
 * @param[in] mqttConnection The MQTT connection to use for subscriptions.
 * @param[in] operation Either #IOT_MQTT_SUBSCRIBE or #IOT_MQTT_UNSUBSCRIBE.
 * @param[in] pTopicFilters Array of topic filters for subscriptions.
 * @param[in] pCallbackParameter The parameter to pass to the subscription
 * callback.
 *
 * @return `EXIT_SUCCESS` if the subscription operation succeeded; `EXIT_FAILURE`
 * otherwise.
 */
static int _modifySubscriptions( IotMqttConnection_t mqttConnection,
                                 IotMqttOperationType_t operation,
                                 const char ** pTopicFilters,
                                 void * pCallbackParameter )
{
    int status = EXIT_SUCCESS;
    int32_t i = 0;
    IotMqttError_t subscriptionStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t pSubscriptions[ TOPIC_FILTER_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Set the members of the subscription list. */
    for( i = 0; i < TOPIC_FILTER_COUNT; i++ )
    {
        pSubscriptions[ i ].qos = IOT_MQTT_QOS_1;
        pSubscriptions[ i ].pTopicFilter = pTopicFilters[ i ];
        pSubscriptions[ i ].topicFilterLength = TOPIC_FILTER_LENGTH;
        pSubscriptions[ i ].callback.pCallbackContext = pCallbackParameter;
        pSubscriptions[ i ].callback.function = _mqttSubscriptionCallback;
    }

    /* Modify subscriptions by either subscribing or unsubscribing. */
    if( operation == IOT_MQTT_SUBSCRIBE )
    {
        subscriptionStatus = IotMqtt_TimedSubscribe( mqttConnection,
                                                     pSubscriptions,
                                                     TOPIC_FILTER_COUNT,
                                                     0,
                                                     MQTT_TIMEOUT_MS );

        /* Check the status of SUBSCRIBE. */
        switch( subscriptionStatus )
        {
            case IOT_MQTT_SUCCESS:
                IotLogInfo( "All demo topic filter subscriptions accepted." );
                break;

            case IOT_MQTT_SERVER_REFUSED:

                /* Check which subscriptions were rejected before exiting the demo. */
                for( i = 0; i < TOPIC_FILTER_COUNT; i++ )
                {
                    if( IotMqtt_IsSubscribed( mqttConnection,
                                              pSubscriptions[ i ].pTopicFilter,
                                              pSubscriptions[ i ].topicFilterLength,
                                              NULL ) == true )
                    {
                        IotLogInfo( "Topic filter %.*s was accepted.",
                                    pSubscriptions[ i ].topicFilterLength,
                                    pSubscriptions[ i ].pTopicFilter );
                    }
                    else
                    {
                        IotLogError( "Topic filter %.*s was rejected.",
                                     pSubscriptions[ i ].topicFilterLength,
                                     pSubscriptions[ i ].pTopicFilter );
                    }
                }

                status = EXIT_FAILURE;
                break;

            default:

                status = EXIT_FAILURE;
                break;
        }
    }
    else if( operation == IOT_MQTT_UNSUBSCRIBE )
    {
        subscriptionStatus = IotMqtt_TimedUnsubscribe( mqttConnection,
                                                       pSubscriptions,
                                                       TOPIC_FILTER_COUNT,
                                                       0,
                                                       MQTT_TIMEOUT_MS );

        /* Check the status of UNSUBSCRIBE. */
        if( subscriptionStatus != IOT_MQTT_SUCCESS )
        {
            status = EXIT_FAILURE;
        }
    }
    else
    {
        /* Only SUBSCRIBE and UNSUBSCRIBE are valid for modifying subscriptions. */
        IotLogError( "MQTT operation %s is not valid for modifying subscriptions.",
                     IotMqtt_OperationType( operation ) );

        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Transmit a message and wait for it to be received.
 *
 * @param[in] mqttConnection The MQTT connection to use for publishing.
 * @param[in] pTopicNames Array of topic names for publishing. These were previously
 * subscribed to as topic filters.
 * @param[in] pPublishReceivedCounter Counts the number of messages received on
 * topic filters.
 *
 * @return `EXIT_SUCCESS` if all messages are published and received; `EXIT_FAILURE`
 * otherwise.
 */
static int _publishResult( IotMqttConnection_t mqttConnection,
                           const char ** pTopicNames,
                           IotSemaphore_t * pPublishReceivedCounter,
                           uint32_t pSubject )
{
    int status = EXIT_SUCCESS;
    intptr_t publishCount = 0;
    IotMqttError_t publishStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttCallbackInfo_t publishComplete = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
    char pPublishPayload[ PUBLISH_PAYLOAD_BUFFER_LENGTH ] = { 0 };

    /* The MQTT library should invoke this callback when a PUBLISH message
     * is successfully transmitted. */
    publishComplete.function = _operationCompleteCallback;

    /* Set the common members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.topicNameLength = TOPIC_FILTER_LENGTH;
    publishInfo.pPayload = pPublishPayload;
    publishInfo.retryMs = PUBLISH_RETRY_MS;
    publishInfo.retryLimit = PUBLISH_RETRY_LIMIT;

    /* Pass the PUBLISH number to the operation complete callback. */
    publishComplete.pCallbackContext = ( void * ) publishCount;

    /* Choose a topic name (round-robin through the array of topic names). */
    publishInfo.pTopicName = pTopicNames[ 0 ];

    /* Generate the payload for the PUBLISH. */
    if( pSubject == -1 )
    {
        status = snprintf( pPublishPayload,
                           PUBLISH_PAYLOAD_BUFFER_LENGTH,
                           PUBLISH_PAYLOAD_FORMAT,
                           ( int ) pSubject,
                           ( int ) 1 );
    }
    else
    {
        status = snprintf( pPublishPayload,
                           PUBLISH_PAYLOAD_BUFFER_LENGTH,
                           PUBLISH_PAYLOAD_FORMAT,
                           ( int ) pSubject,
                           ( int ) 0 );
    }

    /* Check for errors from snprintf. */
    if( status < 0 )
    {
        IotLogError( "Failed to generate MQTT PUBLISH payload for PUBLISH %d.",
                     ( int ) publishCount );
        status = EXIT_FAILURE;
    }
    else
    {
        publishInfo.payloadLength = ( size_t ) status;
        status = EXIT_SUCCESS;
    }

    /* PUBLISH a message. This is an asynchronous function that notifies of
     * completion through a callback. */
    publishStatus = IotMqtt_Publish( mqttConnection,
                                     &publishInfo,
                                     0,
                                     &publishComplete,
                                     NULL );

    if( publishStatus != IOT_MQTT_STATUS_PENDING )
    {
        IotLogError( "MQTT PUBLISH %d returned error %s.",
                     ( int ) publishCount,
                     IotMqtt_strerror( publishStatus ) );
        status = EXIT_FAILURE;
    }

    /* If a complete burst of messages has been published, wait for an equal
     * number of messages to be received. Note that messages may be received
     * out-of-order, especially if a message was lost and had to be retried. */
    if( status == EXIT_SUCCESS )
    {
        IotLogInfo( "Waiting for publish to be received." );

        if( IotSemaphore_TimedWait( pPublishReceivedCounter,
                                    MQTT_TIMEOUT_MS ) == false )
        {
            IotLogError( "Timed out waiting for incoming PUBLISH messages." );
            status = EXIT_FAILURE;
        }
        else
        {
            IotLogInfo( "Publish received." );
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

void vShowAvailableMemory( char cMessage[] )
{
    int64_t ulMemory = heap_caps_get_free_size( MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL );

    IotLogInfo( "%s: %lld", cMessage, ulMemory );
}
/*-----------------------------------------------------------*/

mtmn_config_t init_config()
{
    mtmn_config_t mtmn_config = { 0 };

    mtmn_config.type = FAST;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.707;
    mtmn_config.pyramid_times = 4;
    mtmn_config.p_threshold.score = 0.3;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 20;
    mtmn_config.r_threshold.score = 0.3;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 10;
    mtmn_config.o_threshold.score = 0.4;
    mtmn_config.o_threshold.nms = 0.3;
    mtmn_config.o_threshold.candidate_number = 1;

    return mtmn_config;
}
/*-----------------------------------------------------------*/

void vTestTask( void * arg )
{
    int ulVar = -1;

    IotLogInfo( "Value of the message: %d", ulVar );

    if( xResultQueue != 0 )
    {
        if( xQueueSendToBack( xResultQueue,
                              ( void * ) &ulVar,
                              ( TickType_t ) 10 ) != pdPASS )
        {
            IotLogInfo( "Unable to push the message to the queue" );
        }

        IotLogInfo( "Message sucessfully pushed to the queue" );
    }

    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/

void vImageProcessingTask( int initialized )
{
    IotLogInfo( "Starting the face recognition task" );

    size_t frame_num = 0;
    dl_matrix3du_t * image_matrix = NULL;
    camera_fb_t * fb = NULL;

    /* 1. Load configuration for detection */
    mtmn_config_t mtmn_config = init_config();

    /* 2. Preallocate matrix to store aligned face 56x56  */
    dl_matrix3du_t * aligned_face = dl_matrix3du_alloc( 1,
                                                        FACE_WIDTH,
                                                        FACE_HEIGHT,
                                                        3 );
    int8_t count_down_second; /*second */
    int8_t is_enrolling;

    if( initialized )
    {
        count_down_second = 0;
        is_enrolling = 0;
    }
    else
    {
        count_down_second = 3;
        is_enrolling = 1;
    }

    int32_t next_enroll_index = 0;
    int8_t left_sample_face;
    int8_t free_resources = 1;
    IotLogInfo( "Starting the processing" );

    do
    {
        free_resources = 1;
        int64_t start_time = esp_timer_get_time();
        /* 3. Get one image with camera */
        fb = esp_camera_fb_get();

        if( !fb )
        {
            IotLogError( "Camera capture failed" );
            continue;
        }

        int64_t fb_get_time = esp_timer_get_time();
        /*IotLogInfo( "Get one frame in %lld ms.", (fb_get_time - start_time) / 1000); */

        /* 4. Allocate image matrix to store RGB data */
        image_matrix = dl_matrix3du_alloc( 1, fb->width, fb->height, 3 );

        /* 5. Transform image to RGB */
        uint32_t res = fmt2rgb888( fb->buf, fb->len, fb->format, image_matrix->item );

        if( true != res )
        {
            IotLogError( "fmt2rgb888 failed, fb: %d", fb->len );
            dl_matrix3du_free( image_matrix );
            continue;
        }

        esp_camera_fb_return( fb );

        /* 6. Do face detection */
        box_array_t * net_boxes = face_detect( image_matrix, &mtmn_config );
        /*IotLogInfo( "Detection time consumption: %lldms", (esp_timer_get_time() - fb_get_time) / 1000); */

        if( net_boxes )
        {
            frame_num++;
            /*IotLogInfo( "Face Detection Count: %d", frame_num); */

            /* 5. Do face alignment */
            if( align_face( net_boxes, image_matrix, aligned_face ) == ESP_OK )
            {
                /*count down */
                while( count_down_second > 0 )
                {
                    IotLogInfo( "Face ID Enrollment Starts in %ds.\n", count_down_second );

                    vTaskDelay( 1000 / portTICK_PERIOD_MS );

                    count_down_second--;

                    if( count_down_second == 0 )
                    {
                        IotLogInfo( "\n>>> Face ID Enrollment Starts <<<\n" );
                    }
                }

                /* 6. Do face enrollment */
                if( is_enrolling == 1 )
                {
                    heap_caps_print_heap_info( MALLOC_CAP_SPIRAM );
                    left_sample_face = enroll_face( &id_list, aligned_face );
                    IotLogInfo( "Face ID Enrollment: Taking sample: %d",
                                ENROLL_CONFIRM_TIMES - left_sample_face );

                    if( left_sample_face == 0 )
                    {
                        next_enroll_index++;
                        IotLogInfo( "\nEnrolled Face ID: %d", id_list.tail );

                        if( id_list.count == FACE_ID_SAVE_NUMBER )
                        {
                            is_enrolling = 0;
                            IotLogInfo( "\n>>> Face Recognition Starts <<<\n" );
                            vTaskDelay( 2000 / portTICK_PERIOD_MS );
                        }
                        else
                        {
                            IotLogInfo( "Please log in another one." );
                            vTaskDelay( 2500 / portTICK_PERIOD_MS );
                        }
                    }
                }
                /* 6. Do face recognition */
                else
                {
                    int64_t recog_match_time = esp_timer_get_time();

                    int matched_id = recognize_face( &id_list, aligned_face );

                    if( matched_id >= 0 )
                    {
                        free( net_boxes->score );
                        free( net_boxes->box );
                        free( net_boxes->landmark );
                        free( net_boxes );
                        dl_matrix3du_free( image_matrix );
                        free_resources = 0;
                        IotLogInfo( "Matched Face ID: %d\n", matched_id );

                        if( xResultQueue != 0 )
                        {
                            if( xQueueSendToBack( xResultQueue,
                                                  ( void * ) &matched_id,
                                                  ( TickType_t ) 5 ) != pdPASS )
                            {
                                IotLogInfo( "Unable to push the message to the queue" );
                            }

                            IotLogInfo( "Message pushed to queue: Familiar face" );
                            vTaskSuspend(NULL);
                        }
                    }
                    else
                    {
                        matched_id = -1;
                        IotLogInfo( "No Matched Face ID\n" );

                        if( xResultQueue != 0 )
                        {
                            free( net_boxes->score );
                            free( net_boxes->box );
                            free( net_boxes->landmark );
                            free( net_boxes );
                            dl_matrix3du_free( image_matrix );
                            free_resources = 0;

                            if( xQueueSendToBack( xResultQueue,
                                                  ( void * ) &matched_id,
                                                  ( TickType_t ) 5 ) != pdPASS )
                            {
                                IotLogInfo( "Unable to push the message to the queue" );
                            }

                            IotLogInfo( "Message pushed to queue: Intruder" );
                            vTaskSuspend(NULL);
                        }
                    }

                    IotLogInfo( "Recognition time consumption: %lldms",
                                ( esp_timer_get_time() - recog_match_time ) / 1000 );
                }
            }
            else
            {
                IotLogInfo( "Detected face is not proper." );
            }

            if( free_resources )
            {
                free( net_boxes->score );
                free( net_boxes->box );
                free( net_boxes->landmark );
                free( net_boxes );
            }
        }

        if( free_resources )
        {
            dl_matrix3du_free( image_matrix );
        }
    } while( 1 );
}
/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the MQTT demo, called by the demo runner.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunAIoTDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface )
{
    uint16_t usMessage = 0;
    TaskHandle_t xImageTaskHandle = NULL;
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;

    /* Handle of the MQTT connection used in this demo. */
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    /* Counts the number of incoming PUBLISHES received (and allows the demo
     * application to wait on incoming PUBLISH messages). */
    IotSemaphore_t publishesReceived;

    /* Topics used as both topic filters and topic names in this demo. */
    const char * pTopics[ TOPIC_FILTER_COUNT ] =
    {
        IOT_DEMO_MQTT_TOPIC_PREFIX "/Logs",
    };

    /* Flags for tracking which cleanup functions must be called. */
    bool librariesInitialized = false, connectionEstablished = false;


    IotLogInfo( "Start the AIoT demo!" );

    heap_caps_print_heap_info( MALLOC_CAP_INTERNAL );
    vShowAvailableMemory( "At Startup:" );
    /*heap_caps_print_heap_info(MALLOC_CAP_SPIRAM); */
    IotLogInfo( "\n Starting heap trace \n" );
    vSpeechWakeupInit();
    g_state = WAIT_FOR_WAKEUP;
    vShowAvailableMemory( "After speech init" );

    vTaskDelay( 30 / portTICK_PERIOD_MS );
    IotLogInfo( "Please say 'Alexa' to the board" );

    while( g_state != START_RECOGNITION )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    heap_caps_print_heap_info( MALLOC_CAP_INTERNAL );


    vShowAvailableMemory( "At camera init:" );
    xResultQueue = xQueueCreate( 1, sizeof( int ) );
    vCameraInit();
    /* / *Run the face detection demo* / */
    face_id_init( &id_list,
                  FACE_ID_SAVE_NUMBER,
                  ENROLL_CONFIRM_TIMES );

    IotLogInfo( "Camera init done" );
    vShowAvailableMemory( "Before task creation" );
    xTaskCreatePinnedToCore( &vImageProcessingTask,
                             "ImageProcessing",
                             8 * 1024,
                             ( void * ) 0,
                             5,
                             &xImageTaskHandle,
                             1 );

    while( 1 )
    {
        vShowAvailableMemory( "Inside loop :" );

        if( xResultQueue != 0 )
        {
            xQueueReceive( xResultQueue,
                           &usMessage,
                           portMAX_DELAY );

            IotLogInfo( "Got the message: %d", usMessage );

            if( xImageTaskHandle != NULL )
            {
                vCameraDeInit();
                vTaskDelete( xImageTaskHandle );
                vShowAvailableMemory( "At MQTT startup:" );
                /* Initialize the libraries required for this demo. */
                status = _initializeDemo();

                if( status == EXIT_SUCCESS )
                {
                    /* Mark the libraries as initialized. */
                    librariesInitialized = true;

                    /* Establish a new MQTT connection. */
                    status = _establishMqttConnection( awsIotMqttMode,
                                                       pIdentifier,
                                                       pNetworkServerInfo,
                                                       pNetworkCredentialInfo,
                                                       pNetworkInterface,
                                                       &mqttConnection );
                }

                if( status == EXIT_SUCCESS )
                {
                    /* Mark the MQTT connection as established. */
                    connectionEstablished = true;

                    /* Add the topic filter subscriptions used in this demo. */
                    status = _modifySubscriptions( mqttConnection,
                                                   IOT_MQTT_SUBSCRIBE,
                                                   pTopics,
                                                   &publishesReceived );
                }

                if( status == EXIT_SUCCESS )
                {
                    /* Create the semaphore to count incoming PUBLISH messages. */
                    if( IotSemaphore_Create( &publishesReceived,
                                             0,
                                             IOT_DEMO_MQTT_PUBLISH_BURST_SIZE ) == true )
                    {
                        /* PUBLISH (and wait) for all messages. */
                        status = _publishResult( mqttConnection,
                                                 pTopics,
                                                 &publishesReceived,
                                                 usMessage );

                        /* Destroy the incoming PUBLISH counter. */
                        IotSemaphore_Destroy( &publishesReceived );
                    }
                    else
                    {
                        /* Failed to create incoming PUBLISH counter. */
                        status = EXIT_FAILURE;
                    }
                }

                if( status == EXIT_SUCCESS )
                {
                    /* Remove the topic subscription filters used in this demo. */
                    status = _modifySubscriptions( mqttConnection,
                                                   IOT_MQTT_UNSUBSCRIBE,
                                                   pTopics,
                                                   NULL );
                }

                /* Disconnect the MQTT connection if it was established. */
                if( connectionEstablished == true )
                {
                    IotMqtt_Disconnect( mqttConnection, 0 );
                }

                /* Clean up libraries if they were initialized. */
                if( librariesInitialized == true )
                {
                    _cleanupDemo();
                }

                return status;
            }
        }

        xTaskCreatePinnedToCore( &vImageProcessingTask,
                                 "ImageProcessing",
                                 6 * 1024,
                                 ( void * ) 1,
                                 5,
                                 xImageTaskHandle,
                                 0 );
    }

    vCameraDeInit();
    vTaskSuspend( NULL );



    return 0;
}
/*-----------------------------------------------------------*/
