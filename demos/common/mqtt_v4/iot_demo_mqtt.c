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
 * @file iot_demo_mqtt.c
 * @brief Demonstrates usage of the MQTT library.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <string.h>

/* Common demo include. */
#include "iot_demo.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* MQTT include. */
#include "iot_mqtt.h"

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Including stdio.h also brings in unwanted (and conflicting) symbols on some
 * platforms. Therefore, any functions in stdio.h needed in this file have an
 * extern declaration here. */
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );
/** @endcond */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration settings.
 */
#ifndef IOT_DEMO_MQTT_TOPIC_PREFIX
    #define IOT_DEMO_MQTT_TOPIC_PREFIX           "iotdemo"
#endif
#ifndef IOT_DEMO_MQTT_PUBLISH_BURST_SIZE
    #define IOT_DEMO_MQTT_PUBLISH_BURST_SIZE     ( 10 )
#endif
#ifndef IOT_DEMO_MQTT_PUBLISH_BURST_COUNT
    #define IOT_DEMO_MQTT_PUBLISH_BURST_COUNT    ( 10 )
#endif
/** @endcond */

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
#define _CLIENT_IDENTIFIER_PREFIX                 "iotdemo"

/**
 * @brief The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 * terminator.
 */
#define _CLIENT_IDENTIFIER_MAX_LENGTH             ( 24 )

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define _KEEP_ALIVE_SECONDS                       ( 60 )

/**
 * @brief The timeout for MQTT operations in this demo.
 */
#define _MQTT_TIMEOUT_MS                          ( 5000 )

/**
 * @brief The Last Will and Testament topic name in this demo.
 *
 * The MQTT server will publish a message to this topic name if this client is
 * unexpectedly disconnected.
 */
#define _WILL_TOPIC_NAME                          IOT_DEMO_MQTT_TOPIC_PREFIX "/will"

/**
 * @brief The length of #_WILL_TOPIC_NAME.
 */
#define _WILL_TOPIC_NAME_LENGTH                   ( ( uint16_t ) ( sizeof( _WILL_TOPIC_NAME ) - 1 ) )

/**
 * @brief The message to publish to #_WILL_TOPIC_NAME.
 */
#define _WILL_MESSAGE                             "MQTT demo unexpectedly disconnected."

/**
 * @brief The length of #_WILL_MESSAGE.
 */
#define _WILL_MESSAGE_LENGTH                      ( ( size_t ) ( sizeof( _WILL_MESSAGE ) - 1 ) )

/**
 * @brief How many topic filters will be used in this demo.
 */
#define _TOPIC_FILTER_COUNT                       ( 4 )

/**
 * @brief The length of each topic filter.
 *
 * For convenience, all topic filters are the same length.
 */
#define _TOPIC_FILTER_LENGTH                      ( ( uint16_t ) ( sizeof( IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/1" ) - 1 ) )

/**
 * @brief Format string of the PUBLISH messages in this demo.
 */
#define _PUBLISH_PAYLOAD_FORMAT                   "Hello world %d!"

/**
 * @brief Size of the buffer that holds the PUBLISH messages in this demo.
 */
#define _PUBLISH_PAYLOAD_BUFFER_LENGTH            ( sizeof( _PUBLISH_PAYLOAD_FORMAT ) + 2 )

/**
 * @brief The maximum number of times each PUBLISH in this demo will be retried.
 */
#define _PUBLISH_RETRY_LIMIT                      ( 10 )

/**
 * @brief A PUBLISH message is retried if no response is received within this
 * time.
 */
#define _PUBLISH_RETRY_MS                         ( 1000 )

/**
 * @brief The topic name on which acknowledgement messages for incoming publishes
 * should be published.
 */
#define _ACKNOWLEDGEMENT_TOPIC_NAME               IOT_DEMO_MQTT_TOPIC_PREFIX "/acknowledgements"

/**
 * @brief The length of #_ACKNOWLEDGEMENT_TOPIC_NAME.
 */
#define _ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH        ( ( uint16_t ) ( sizeof( _ACKNOWLEDGEMENT_TOPIC_NAME ) - 1 ) )

/**
 * @brief Format string of PUBLISH acknowledgement messages in this demo.
 */
#define _ACKNOWLEDGEMENT_MESSAGE_FORMAT           "Client has received PUBLISH %.*s from server."

/**
 * @brief Size of the buffers that hold acknowledgement messages in this demo.
 */
#define _ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH    ( sizeof( _ACKNOWLEDGEMENT_MESSAGE_FORMAT ) + 2 )

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
    if( pOperation->operation.result == IOT_MQTT_SUCCESS )
    {
        IotLogInfo( "MQTT %s %d successfully sent.",
                    IotMqtt_OperationType( pOperation->operation.type ),
                    ( int ) publishCount );
    }
    else
    {
        IotLogError( "MQTT %s %d could not be sent. Error %s.",
                     IotMqtt_OperationType( pOperation->operation.type ),
                     ( int ) publishCount,
                     IotMqtt_strerror( pOperation->operation.result ) );
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
    const char * pPayload = pPublish->message.info.pPayload;
    char pAcknowledgementMessage[ _ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH ] = { 0 };
    IotMqttPublishInfo_t acknowledgementInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo( "Incoming PUBLISH received:\n"
                "Subscription topic filter: %.*s\n"
                "Publish topic name: %.*s\n"
                "Publish retain flag: %d\n"
                "Publish QoS: %d\n"
                "Publish payload: %.*s",
                pPublish->message.topicFilterLength,
                pPublish->message.pTopicFilter,
                pPublish->message.info.topicNameLength,
                pPublish->message.info.pTopicName,
                pPublish->message.info.retain,
                pPublish->message.info.qos,
                pPublish->message.info.payloadLength,
                pPayload );

    /* Find the message number inside of the PUBLISH message. */
    for( messageNumberIndex = 0; messageNumberIndex < pPublish->message.info.payloadLength; messageNumberIndex++ )
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
    if( messageNumberIndex < pPublish->message.info.payloadLength )
    {
        /* Compute the length of the message number. */
        while( ( messageNumberIndex + messageNumberLength < pPublish->message.info.payloadLength ) &&
               ( *( pPayload + messageNumberIndex + messageNumberLength ) >= '0' ) &&
               ( *( pPayload + messageNumberIndex + messageNumberLength ) <= '9' ) )
        {
            messageNumberLength++;
        }

        /* Generate an acknowledgement message. */
        acknowledgementLength = snprintf( pAcknowledgementMessage,
                                          _ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH,
                                          _ACKNOWLEDGEMENT_MESSAGE_FORMAT,
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
            acknowledgementInfo.pTopicName = _ACKNOWLEDGEMENT_TOPIC_NAME;
            acknowledgementInfo.topicNameLength = _ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH;
            acknowledgementInfo.pPayload = pAcknowledgementMessage;
            acknowledgementInfo.payloadLength = ( size_t ) acknowledgementLength;
            acknowledgementInfo.retryMs = _PUBLISH_RETRY_MS;
            acknowledgementInfo.retryLimit = _PUBLISH_RETRY_LIMIT;

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
 * @brief The function that runs the MQTT demo.
 *
 * This function is called to run the MQTT demo once a network connection has
 * been established.
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to false if using another MQTT server.
 * @param[in] pClientIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pMqttConnection Pointer to the MQTT connection to use. This MQTT
 * connection must be initialized to IOT_MQTT_CONNECTION_INITIALIZER.
 * @param[in] pNetworkInterface Pointer to an MQTT network interface to use.
 * All necessary members of the network interface should be set before calling
 * this function.
 *
 * @return 0 if the demo completes successfully; -1 if some part of it fails.
 */
int IotDemo_RunMqttDemo( bool awsIotMqttMode,
                         const char * const pClientIdentifier,
                         IotMqttConnection_t * const pMqttConnection,
                         const IotMqttNetIf_t * const pNetworkInterface )
{
    int status = 0, i = 0;
    intptr_t publishCount = 0;
    char pClientIdentifierBuffer[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };
    char pPublishPayload[ _PUBLISH_PAYLOAD_BUFFER_LENGTH ] = { 0 };
    IotMqttError_t mqttStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER,
                         publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttSubscription_t pSubscriptions[ _TOPIC_FILTER_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };
    IotMqttCallbackInfo_t publishComplete = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
    IotSemaphore_t publishesReceived;
    const char * pTopicFilters[ _TOPIC_FILTER_COUNT ] =
    {
        IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/1",
        IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/2",
        IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/3",
        IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/4",
    };

    /* Set the common members of the connection info. */
    connectInfo.awsIotMqttMode = awsIotMqttMode;
    connectInfo.cleanSession = true;
    connectInfo.keepAliveSeconds = _KEEP_ALIVE_SECONDS;
    connectInfo.pWillInfo = &willInfo;

    /* Set the members of the Last Will and Testament (LWT) message info. The
     * MQTT server will publish the LWT message if this client disconnects
     * unexpectedly. */
    willInfo.pTopicName = _WILL_TOPIC_NAME;
    willInfo.topicNameLength = _WILL_TOPIC_NAME_LENGTH;
    willInfo.pPayload = _WILL_MESSAGE;
    willInfo.payloadLength = _WILL_MESSAGE_LENGTH;

    /* Use the parameter client identifier if provided. Otherwise, generate a
     * unique client identifier. */
    if( pClientIdentifier != NULL )
    {
        connectInfo.pClientIdentifier = pClientIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( pClientIdentifier );
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
            status = -1;
        }
        else
        {
            /* Set the client identifier buffer and length. */
            connectInfo.pClientIdentifier = pClientIdentifierBuffer;
            connectInfo.clientIdentifierLength = ( uint16_t ) status;

            status = 0;
        }
    }

    if( status == 0 )
    {
        IotLogInfo( "MQTT demo client identifier is %.*s (length %hu).",
                    connectInfo.clientIdentifierLength,
                    connectInfo.pClientIdentifier,
                    connectInfo.clientIdentifierLength );

        /* Establish the MQTT connection. */
        mqttStatus = IotMqtt_Connect( pMqttConnection,
                                      pNetworkInterface,
                                      &connectInfo,
                                      _MQTT_TIMEOUT_MS );

        if( mqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT CONNECT returned error %s.",
                         IotMqtt_strerror( mqttStatus ) );

            status = -1;
        }
    }

    if( status == 0 )
    {
        /* Set the members of the subscription list. */
        for( i = 0; i < _TOPIC_FILTER_COUNT; i++ )
        {
            pSubscriptions[ i ].qos = IOT_MQTT_QOS_1;
            pSubscriptions[ i ].pTopicFilter = pTopicFilters[ i ];
            pSubscriptions[ i ].topicFilterLength = _TOPIC_FILTER_LENGTH;
            pSubscriptions[ i ].callback.param1 = &publishesReceived;
            pSubscriptions[ i ].callback.function = _mqttSubscriptionCallback;
        }

        /* Subscribe to all the topic filters in the subscription list. The
         * blocking SUBSCRIBE function is used because the demo should not
         * continue until SUBSCRIBE completes. */
        mqttStatus = IotMqtt_TimedSubscribe( *pMqttConnection,
                                             pSubscriptions,
                                             _TOPIC_FILTER_COUNT,
                                             0,
                                             _MQTT_TIMEOUT_MS );

        switch( mqttStatus )
        {
            case IOT_MQTT_SUCCESS:
                IotLogInfo( "All demo topic filter subscriptions accepted." );
                break;

            case IOT_MQTT_SERVER_REFUSED:

                /* Check which subscriptions were rejected before exiting the demo. */
                for( i = 0; i < _TOPIC_FILTER_COUNT; i++ )
                {
                    if( IotMqtt_IsSubscribed( *pMqttConnection,
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
                        IotLogInfo( "Topic filter %.*s was rejected.",
                                    pSubscriptions[ i ].topicFilterLength,
                                    pSubscriptions[ i ].pTopicFilter );
                    }
                }

                status = -1;
                break;

            default:

                status = -1;
                break;
        }
    }

    if( status == 0 )
    {
        /* The MQTT library should invoke this callback when a PUBLISH message
         * is successfully transmitted. */
        publishComplete.function = _operationCompleteCallback;

        /* Set the common members of the publish info. */
        publishInfo.qos = IOT_MQTT_QOS_1;
        publishInfo.topicNameLength = _TOPIC_FILTER_LENGTH;
        publishInfo.pPayload = pPublishPayload;
        publishInfo.retryMs = _PUBLISH_RETRY_MS;
        publishInfo.retryLimit = _PUBLISH_RETRY_LIMIT;

        /* Create the semaphore that counts received PUBLISH messages.*/
        if( IotSemaphore_Create( &publishesReceived,
                                 0,
                                 IOT_DEMO_MQTT_PUBLISH_BURST_SIZE ) == true )
        {
            for( publishCount = 0;
                 publishCount < IOT_DEMO_MQTT_PUBLISH_BURST_SIZE * IOT_DEMO_MQTT_PUBLISH_BURST_COUNT;
                 publishCount++ )
            {
                if( publishCount % IOT_DEMO_MQTT_PUBLISH_BURST_SIZE == 0 )
                {
                    IotLogInfo( "Publishing messages %d to %d.",
                                publishCount,
                                publishCount + IOT_DEMO_MQTT_PUBLISH_BURST_SIZE - 1 );
                }

                /* Pass the PUBLISH number to the operation complete callback. */
                publishComplete.param1 = ( void * ) publishCount;

                /* Choose a topic name. */
                publishInfo.pTopicName = pTopicFilters[ publishCount % _TOPIC_FILTER_COUNT ];

                /* Generate the payload for the PUBLISH. */
                status = snprintf( pPublishPayload,
                                   _PUBLISH_PAYLOAD_BUFFER_LENGTH,
                                   _PUBLISH_PAYLOAD_FORMAT,
                                   ( int ) publishCount );

                /* Check for errors from snprintf. */
                if( status < 0 )
                {
                    IotLogError( "Failed to generate MQTT PUBLISH payload for PUBLISH %d.",
                                 ( int ) publishCount );
                    status = -1;

                    break;
                }
                else
                {
                    publishInfo.payloadLength = ( size_t ) status;
                    status = 0;
                }

                /* PUBLISH a message. */
                mqttStatus = IotMqtt_Publish( *pMqttConnection,
                                              &publishInfo,
                                              0,
                                              &publishComplete,
                                              NULL );

                if( mqttStatus != IOT_MQTT_STATUS_PENDING )
                {
                    IotLogError( "MQTT PUBLISH %d returned error %s.",
                                 ( int ) publishCount,
                                 IotMqtt_strerror( mqttStatus ) );
                    status = -1;
                    break;
                }

                /* If a complete burst of messages has been published, wait for
                 * an equal number of messages to be received. Note that messages
                 * may be received out-of-order, especially if a message was
                 * dropped and had to be retried. */
                if( ( publishCount > 0 ) &&
                    ( publishCount % IOT_DEMO_MQTT_PUBLISH_BURST_SIZE == 0 ) )
                {
                    IotLogInfo( "Waiting for %d publishes to be received.",
                                IOT_DEMO_MQTT_PUBLISH_BURST_SIZE );

                    for( i = 0; i < IOT_DEMO_MQTT_PUBLISH_BURST_SIZE; i++ )
                    {
                        if( IotSemaphore_TimedWait( &publishesReceived,
                                                    _MQTT_TIMEOUT_MS ) == false )
                        {
                            IotLogError( "Timed out waiting for incoming PUBLISH messages." );
                            status = -1;
                            break;
                        }
                    }

                    IotLogInfo( "%d publishes received.",
                                IOT_DEMO_MQTT_PUBLISH_BURST_SIZE );
                }

                /* Stop publishing if there was an error. */
                if( status == -1 )
                {
                    break;
                }
            }

            /* Wait for the messages in the last burst to be received. This
             * should also wait for all previously published messages. */
            if( status == 0 )
            {
                IotLogInfo( "Waiting for all publishes to be received." );

                for( i = 0; i < IOT_DEMO_MQTT_PUBLISH_BURST_SIZE; i++ )
                {
                    if( IotSemaphore_TimedWait( &publishesReceived,
                                                _MQTT_TIMEOUT_MS ) == false )
                    {
                        IotLogError( "Timed out waiting for incoming PUBLISH messages." );
                        status = -1;
                        break;
                    }
                }

                IotLogInfo( "All publishes received." );
            }

            /* Destroy the received message counter. */
            IotSemaphore_Destroy( &publishesReceived );
        }
        else
        {
            /* Couldn't create received message counter. */
            status = -1;
        }
    }

    if( status == 0 )
    {
        /* Unsubscribe from all demo topic filters. */
        mqttStatus = IotMqtt_TimedUnsubscribe( *pMqttConnection,
                                               pSubscriptions,
                                               _TOPIC_FILTER_COUNT,
                                               0,
                                               _MQTT_TIMEOUT_MS );

        if( mqttStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT UNSUBSCRIBE returned error %s.",
                         IotMqtt_strerror( mqttStatus ) );
            status = -1;
        }
    }

    /* Disconnect the MQTT connection if it was established. */
    if( *pMqttConnection != IOT_MQTT_CONNECTION_INITIALIZER )
    {
        IotMqtt_Disconnect( *pMqttConnection, false );
    }

    return status;
}

/*-----------------------------------------------------------*/
