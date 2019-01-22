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
 * @file aws_iot_mqtt_api.c
 * @brief Implements the user-facing functions of the MQTT library.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/* Validate MQTT configuration settings. */
#if AWS_IOT_MQTT_ENABLE_ASSERTS != 0 && AWS_IOT_MQTT_ENABLE_ASSERTS != 1
    #error "AWS_IOT_MQTT_ENABLE_ASSERTS must be 0 or 1."
#endif
#if AWS_IOT_MQTT_ENABLE_METRICS != 0 && AWS_IOT_MQTT_ENABLE_METRICS != 1
    #error "AWS_IOT_MQTT_ENABLE_METRICS must be 0 or 1."
#endif
#if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES != 0 && AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES != 1
    #error "AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES must be 0 or 1."
#endif
#if AWS_IOT_MQTT_MAX_CALLBACK_THREADS <= 0
    #error "AWS_IOT_MQTT_MAX_CALLBACK_THREADS cannot be 0 or negative."
#endif
#if AWS_IOT_MQTT_MAX_SEND_THREADS <= 0
    #error "AWS_IOT_MQTT_MAX_SEND_THREADS cannot be 0 or negative."
#endif
#if AWS_IOT_MQTT_TEST != 0 && AWS_IOT_MQTT_TEST != 1
    #error "AWS_IOT_MQTT_MQTT_TEST must be 0 or 1."
#endif
#if AWS_IOT_MQTT_RESPONSE_WAIT_MS <= 0
    #error "AWS_IOT_MQTT_RESPONSE_WAIT_MS cannot be 0 or negative."
#endif
#if AWS_IOT_MQTT_RETRY_MS_CEILING <= 0
    #error "AWS_IOT_MQTT_RETRY_MS_CEILING cannot be 0 or negative."
#endif
#if AWS_IOT_MQTT_TIMER_EVENT_THRESHOLD_MS <= 0
    #error "AWS_IOT_MQTT_TIMER_EVENT_THRESHOLD_MS cannot be 0 or negative."
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Match #_mqttOperation_t by their associated MQTT connection.
 *
 * @param[in] pOperationLink Pointer to the link member of an #_mqttOperation_t.
 * @param[in] pMatch Pointer to an #_mqttConnection_t.
 *
 * @return `true` if the [connection associated with the given operation]
 * (@ref _mqttOperation_t.pMqttConnection) is equal to `pMatch`; `false`
 * otherwise.
 */
static bool _mqttOperation_matchConnection( const IotLink_t * pOperationLink,
                                            void * pMatch );

/**
 * @brief Determines if an MQTT subscription is safe to remove based on its
 * reference count.
 *
 * @param[in] pSubscriptionLink Pointer to the link member of an #_mqttSubscription_t.
 * @param[in] pMatch Not used.
 *
 * @return `true` if the given subscription has no references; `false` otherwise.
 */
static bool _mqttSubscription_shouldRemove( const IotLink_t * pSubscriptionLink,
                                            void * pMatch );

/**
 * @brief Process a keep-alive event received from a timer event queue.
 *
 * @param[in] pMqttConnection The MQTT connection associated with the keep-alive.
 * @param[in] pKeepAliveEvent The keep-alive event to process.
 *
 * @return `true` if the event was successful; false if an error was encountered
 * while processing the keep-alive. If this function returns false, the MQTT
 * connection should be closed.
 */
static bool _processKeepAliveEvent( _mqttConnection_t * const pMqttConnection,
                                    _mqttTimerEvent_t * const pKeepAliveEvent );

/**
 * @brief Process a PUBLISH retry event received from a timer event queue.
 *
 * @param[in] awsIotMqttMode Specifies if this connection is to an AWS IoT MQTT server.
 * @param[in] pPublishRetryEvent The PUBLISH retry event to process.
 */
static void _processPublishRetryEvent( bool awsIotMqttMode,
                                       _mqttTimerEvent_t * const pPublishRetryEvent );

/**
 * @brief Send a PUBACK for a received QoS 1 PUBLISH packet.
 *
 * @param[in] pMqttConnection Which connection the PUBACK should be sent over.
 * @param[in] packetIdentifier Which packet identifier to include in PUBACK.
 */
static void _sendPuback( _mqttConnection_t * const pMqttConnection,
                         uint16_t packetIdentifier );

/**
 * @brief Handles timer expirations for an MQTT connection.
 *
 * This function is invoked when the MQTT connection timer expires. Based on
 * pending timer events, it then sends PINGREQ, checks for PINGRESP, or resends
 * an unacknowledged QoS 1 PUBLISH.
 *
 * @param[in] pArgument The MQTT connection for which PINGREQ is sent.
 */
static void _timerThread( void * pArgument );

/**
 * @brief Creates a new MQTT connection and initializes its members.
 *
 * @param[in] awsIotMqttMode Specifies if this connection is to an AWS IoT MQTT server.
 * @param[in] pNetworkInterface User-provided network interface for the new
 * connection.
 * @param[in] keepAliveSeconds User-provided keep-alive interval for the new
 * connection.
 *
 * @return Pointer to a newly-allocated MQTT connection on success; `NULL` on
 * failure.
 */
static _mqttConnection_t * _createMqttConnection( bool awsIotMqttMode,
                                                  const AwsIotMqttNetIf_t * const pNetworkInterface,
                                                  uint16_t keepAliveSeconds );

/**
 * @brief Destroys the members of an MQTT connection.
 *
 * @param[in] pMqttConnection Which connection to destroy.
 */
static void _destroyMqttConnection( _mqttConnection_t * const pMqttConnection );

/**
 * @brief The common component of both @ref mqtt_function_connect and @ref
 * mqtt_function_connectrestoresession.
 *
 * See @ref mqtt_function_connectrestoresession for a description of the
 * parameters and return values.
 */
static AwsIotMqttError_t _connectCommon( AwsIotMqttConnection_t * pMqttConnection,
                                         const AwsIotMqttNetIf_t * const pNetworkInterface,
                                         const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                         const AwsIotMqttPublishInfo_t * const pWillInfo,
                                         const AwsIotMqttSubscription_t * const pSessionSubscriptions,
                                         size_t sessionSubscriptionsCount,
                                         uint64_t timeoutMs );

/**
 * @brief The common component of both @ref mqtt_function_subscribe and @ref
 * mqtt_function_unsubscribe.
 *
 * See @ref mqtt_function_subscribe or @ref mqtt_function_unsubscribe for a
 * description of the parameters and return values.
 */
static AwsIotMqttError_t _subscriptionCommon( AwsIotMqttOperationType_t operation,
                                              AwsIotMqttConnection_t mqttConnection,
                                              const AwsIotMqttSubscription_t * const pSubscriptionList,
                                              size_t subscriptionCount,
                                              uint32_t flags,
                                              const AwsIotMqttCallbackInfo_t * const pCallbackInfo,
                                              AwsIotMqttReference_t * const pSubscriptionRef );

/*-----------------------------------------------------------*/

/**
 * @brief Ensures that only one CONNECT operation is in-progress at any time.
 *
 * Because CONNACK contains no data about which CONNECT packet it acknowledges,
 * only one CONNECT operation may be in-progress at any time.
 */
static AwsIotMutex_t _connectMutex;

/*-----------------------------------------------------------*/

static bool _mqttOperation_matchConnection( const IotLink_t * pOperationLink,
                                            void * pMatch )
{
    bool match = false;
    _mqttOperation_t * pOperation = IotLink_Container( _mqttOperation_t,
                                                       pOperationLink,
                                                       link );

    /* Ignore PINGREQ operations. PINGREQs will be cleaned up with the MQTT
     * connection and not here. */
    if( pOperation->operation != AWS_IOT_MQTT_PINGREQ )
    {
        match = ( pMatch == pOperation->pMqttConnection );
    }

    return match;
}

/*-----------------------------------------------------------*/

static bool _mqttSubscription_shouldRemove( const IotLink_t * pSubscriptionLink,
                                            void * pMatch )
{
    bool match = false;
    _mqttSubscription_t * pSubscription = IotLink_Container( _mqttSubscription_t,
                                                             pSubscriptionLink,
                                                             link );

    /* Silence warnings about unused parameters. */
    ( void ) pMatch;

    /* Reference count must not be negative. */
    AwsIotMqtt_Assert( pSubscription->references >= 0 );

    /* Check if any subscription callbacks are using this subscription. */
    if( pSubscription->references > 0 )
    {
        /* Set the unsubscribed flag, but do not remove the subscription yet. */
        pSubscription->unsubscribed = true;
    }
    else
    {
        /* No references for this subscription; it can be removed. */
        match = true;
    }

    return match;
}

/*-----------------------------------------------------------*/

static bool _processKeepAliveEvent( _mqttConnection_t * const pMqttConnection,
                                    _mqttTimerEvent_t * const pKeepAliveEvent )
{
    bool status = true;

    /* Check if the keep-alive is waiting for a PINGRESP. */
    if( pKeepAliveEvent->checkPingresp == false )
    {
        /* If keep-alive isn't waiting for PINGRESP, send PINGREQ. */
        AwsIotLogDebug( "Sending PINGREQ." );

        /* Add the PINGREQ operation to the send queue for network transmission. */
        if( AwsIotMqttInternal_EnqueueOperation( pMqttConnection->pPingreqOperation,
                                                 &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogWarn( "Failed to enqueue PINGREQ for sending." );
            status = false;
        }
        else
        {
            /* Check for a PINGRESP after AWS_IOT_MQTT_RESPONSE_WAIT_MS. */
            pKeepAliveEvent->expirationTime = AwsIotClock_GetTimeMs() + AWS_IOT_MQTT_RESPONSE_WAIT_MS;
            pKeepAliveEvent->checkPingresp = true;
        }
    }
    else
    {
        /* Check that a PINGRESP is immediately available. */
        if( AwsIotMqtt_Wait( pMqttConnection->pPingreqOperation, 0 ) == AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogDebug( "PINGRESP received." );

            /* The next keep-alive event should send another PINGREQ. */
            pKeepAliveEvent->expirationTime = AwsIotClock_GetTimeMs() +
                                              pMqttConnection->keepAliveSeconds * 1000ULL;
            pKeepAliveEvent->checkPingresp = false;
        }
        else
        {
            /* PINGRESP was not received within AWS_IOT_MQTT_PINGRESP_WAIT_MS. */
            AwsIotLogError( "Timeout waiting on PINGRESP." );

            /* Set the error flag. The MQTT connection will be closed. */
            pMqttConnection->errorOccurred = true;

            /* Free the keep-alive event and destroy the PINGREQ operation, as they
             * will no longer be used by a closed connection. */
            AwsIotMqtt_FreeTimerEvent( pMqttConnection->pKeepAliveEvent );
            pMqttConnection->pKeepAliveEvent = NULL;

            AwsIotMqttInternal_DestroyOperation( pMqttConnection->pPingreqOperation );
            pMqttConnection->pPingreqOperation = NULL;

            status = false;
        }
    }

    /* Add the next keep-alive event to the timer event list if the keep-alive
     * was successfully processed. */
    if( status == true )
    {
        IotListDouble_InsertSorted( &( pMqttConnection->timerEventList ),
                                    &( pMqttConnection->pKeepAliveEvent->link ),
                                    AwsIotMqttInternal_TimerEventCompare );
    }

    return status;
}

/*-----------------------------------------------------------*/

static void _processPublishRetryEvent( bool awsIotMqttMode,
                                       _mqttTimerEvent_t * const pPublishRetryEvent )
{
    _mqttOperation_t * pOperation = pPublishRetryEvent->pOperation;

    /* This function should only be called for PUBLISH operations with retry. */
    AwsIotMqtt_Assert( pOperation->operation == AWS_IOT_MQTT_PUBLISH_TO_SERVER );
    AwsIotMqtt_Assert( pOperation->pPublishRetry == pPublishRetryEvent );
    AwsIotMqtt_Assert( pPublishRetryEvent->retry.limit > 0 );

    /* Check if the PUBLISH operation is still waiting for a response. */
    if( AwsIotMqttInternal_FindOperation( pOperation->operation,
                                          &( pOperation->packetIdentifier ) ) == pOperation )
    {
        /* Check if the retry limit is reached. */
        if( pPublishRetryEvent->retry.count > pPublishRetryEvent->retry.limit )
        {
            AwsIotLogError( "No PUBACK received for PUBLISH %hu after %d retransmissions.",
                            pOperation->packetIdentifier,
                            pPublishRetryEvent->retry.limit );

            /* Set a status of "No response to retries" and notify. */
            pOperation->status = AWS_IOT_MQTT_RETRY_NO_RESPONSE;
            AwsIotMqttInternal_Notify( pOperation );
        }
        else
        {
            /* Choose a set DUP function. */
            void ( * publishSetDup )( bool,
                                      uint8_t * const,
                                      uint16_t * const ) = AwsIotMqttInternal_PublishSetDup;

            #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                if( pOperation->pMqttConnection->network.serialize.publishSetDup != NULL )
                {
                    publishSetDup = pOperation->pMqttConnection->network.serialize.publishSetDup;
                }
            #endif

            /* For the AWS IoT MQTT server, AwsIotMqttInternal_PublishSetDup changes the
             * packet identifier; this must be done on every retry. For a compliant MQTT
             * server, the function sets the DUP flag; this only needs to be done on the
             * first retry. */
            if( awsIotMqttMode == true )
            {
                if( pPublishRetryEvent->retry.count <= pPublishRetryEvent->retry.limit )
                {
                    publishSetDup( true,
                                   pOperation->pMqttPacket,
                                   &( pOperation->packetIdentifier ) );
                }
            }
            else
            {
                if( pPublishRetryEvent->retry.count == 1 )
                {
                    publishSetDup( false,
                                   pOperation->pMqttPacket,
                                   &( pOperation->packetIdentifier ) );
                }
            }

            /* Print debug log messages about this PUBLISH retry. */
            AwsIotLogDebug( "No PUBACK received for PUBLISH %hu. Attempting retransmission"
                            " %d of %d.",
                            pOperation->packetIdentifier,
                            pPublishRetryEvent->retry.count,
                            pPublishRetryEvent->retry.limit );

            if( pPublishRetryEvent->retry.count < pPublishRetryEvent->retry.limit )
            {
                AwsIotLogDebug( "Next retry for PUBLISH %hu in %llu ms.",
                                pOperation->packetIdentifier,
                                pPublishRetryEvent->retry.nextPeriod );
            }
            else
            {
                AwsIotLogDebug( "Final retry for PUBLISH %hu. Will check in %llu ms "
                                "for response.",
                                pOperation->packetIdentifier,
                                AWS_IOT_MQTT_RESPONSE_WAIT_MS );
            }

            /* Add the PUBLISH to the send queue for network transmission. */
            if( AwsIotMqttInternal_EnqueueOperation( pOperation,
                                                     &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
            {
                AwsIotLogWarn( "Failed to enqueue PUBLISH retry for sending." );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void _sendPuback( _mqttConnection_t * const pMqttConnection,
                         uint16_t packetIdentifier )
{
    _mqttOperation_t * pPubackOperation = NULL;

    /* Choose a PUBACK serializer function. */
    AwsIotMqttError_t ( * serializePuback )( uint16_t,
                                             uint8_t ** const,
                                             size_t * const ) = AwsIotMqttInternal_SerializePuback;

    #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pMqttConnection->network.serialize.puback != NULL )
        {
            serializePuback = pMqttConnection->network.serialize.puback;
        }
    #endif

    AwsIotLogDebug( "Sending PUBACK for received PUBLISH %hu.", packetIdentifier );

    /* Create a PUBACK operation. */
    if( AwsIotMqttInternal_CreateOperation( &pPubackOperation,
                                            0,
                                            NULL ) != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogWarn( "Failed to create PUBACK operation." );

        return;
    }

    /* Generate a PUBACK packet from the packet identifier. */
    if( serializePuback( packetIdentifier,
                         &( pPubackOperation->pMqttPacket ),
                         &( pPubackOperation->packetSize ) ) != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogWarn( "Failed to generate PUBACK packet." );
        AwsIotMqttInternal_DestroyOperation( pPubackOperation );

        return;
    }

    /* Set the remaining members of the PUBACK operation and push it to the
     * send queue for network transmission. */
    pPubackOperation->operation = AWS_IOT_MQTT_PUBACK;
    pPubackOperation->pMqttConnection = pMqttConnection;

    if( AwsIotMqttInternal_EnqueueOperation( pPubackOperation,
                                             &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogWarn( "Failed to enqueue PUBACK for sending." );
        AwsIotMqttInternal_DestroyOperation( pPubackOperation );
    }
}

/*-----------------------------------------------------------*/

static void _timerThread( void * pArgument )
{
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) pArgument;
    _mqttTimerEvent_t * pTimerEvent = NULL;

    AwsIotLogDebug( "Timer thread started for connection %p.", pMqttConnection );

    /* Attempt to lock the timer mutex before this thread does anything.
     * Return immediately if the mutex couldn't be locked. */
    if( AwsIotMutex_TryLock( &( pMqttConnection->timerMutex ) ) == false )
    {
        AwsIotLogWarn( "Failed to lock connection timer mutex in timer thread. Exiting." );

        return;
    }

    while( true )
    {
        /* Peek the first event in the timer event list. */
        pTimerEvent = IotLink_Container( _mqttTimerEvent_t,
                                         IotListDouble_PeekHead( &( pMqttConnection->timerEventList ) ),
                                         link );

        if( pTimerEvent != NULL )
        {
            /* Check if the first event should be processed now. */
            if( pTimerEvent->expirationTime <= AwsIotClock_GetTimeMs() + AWS_IOT_MQTT_TIMER_EVENT_THRESHOLD_MS )
            {
                /*  Remove the timer event for immediate processing. */
                IotListDouble_Remove( &( pTimerEvent->link ) );
            }
            else
            {
                /* The first element in the timer queue shouldn't be processed yet.
                 * Arm the timer for when it should be processed. */
                if( AwsIotClock_TimerArm( &( pMqttConnection->timer ),
                                          pTimerEvent->expirationTime - AwsIotClock_GetTimeMs(),
                                          0 ) == false )
                {
                    AwsIotLogWarn( "Failed to re-arm timer for connection %p.",
                                   pMqttConnection );
                }

                pTimerEvent = NULL;
            }
        }

        /* If there are no timer events to process, terminate this thread. */
        if( pTimerEvent == NULL )
        {
            AwsIotLogDebug( "No further timer events to process. Exiting timer thread." );

            break;
        }

        AwsIotLogDebug( "Processing timer event for %s.",
                        AwsIotMqtt_OperationType( pTimerEvent->pOperation->operation ) );

        /* Process the received timer event. Currently, only PINGREQ and PUBLISH
         * operations send timer events. */
        switch( pTimerEvent->pOperation->operation )
        {
            case AWS_IOT_MQTT_PINGREQ:

                /* Process the PINGREQ event. If it fails to process, close the MQTT
                 * connection. */
                if( _processKeepAliveEvent( pMqttConnection, pTimerEvent ) == false )
                {
                    if( pMqttConnection->network.disconnect != NULL )
                    {
                        pMqttConnection->network.disconnect( pMqttConnection->network.pDisconnectContext );
                    }
                    else
                    {
                        AwsIotLogWarn( "No disconnect function was set. Network connection not closed." );
                    }
                }

                break;

            case AWS_IOT_MQTT_PUBLISH_TO_SERVER:

                _processPublishRetryEvent( pMqttConnection->awsIotMqttMode,
                                           pTimerEvent );
                break;

            default:

                /* No other operation may send a timer event. Abort the program
                 * if this case is reached. */
                AwsIotMqtt_Assert( 0 );
                break;
        }
    }

    AwsIotMutex_Unlock( &( pMqttConnection->timerMutex ) );
}

/*-----------------------------------------------------------*/

static _mqttConnection_t * _createMqttConnection( bool awsIotMqttMode,
                                                  const AwsIotMqttNetIf_t * const pNetworkInterface,
                                                  uint16_t keepAliveSeconds )
{
    _mqttConnection_t * pNewMqttConnection = NULL;

    /* AWS IoT service limits set minimum and maximum values for keep-alive interval.
     * Adjust the user-provided keep-alive interval based on these requirements. */
    if( awsIotMqttMode == true )
    {
        if( keepAliveSeconds < _AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE )
        {
            keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE;
        }
        else if( ( keepAliveSeconds > _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE ) || ( keepAliveSeconds == 0 ) )
        {
            keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE;
        }
    }

    /* Allocate memory to store data for the new MQTT connection. */
    pNewMqttConnection = ( _mqttConnection_t * )
                         AwsIotMqtt_MallocConnection( sizeof( _mqttConnection_t ) );

    if( pNewMqttConnection == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for new MQTT connection." );

        return NULL;
    }

    /* Clear the MQTT connection, then copy the network interface and MQTT server
     * mode. */
    ( void ) memset( pNewMqttConnection, 0x00, sizeof( _mqttConnection_t ) );
    pNewMqttConnection->network = *pNetworkInterface;
    pNewMqttConnection->awsIotMqttMode = awsIotMqttMode;

    /* Create the timer mutex for a new connection. */
    if( AwsIotMutex_Create( &( pNewMqttConnection->timerMutex ) ) == false )
    {
        AwsIotLogError( "Failed to create timer mutex for new connection." );
        AwsIotMqtt_FreeConnection( pNewMqttConnection );

        return NULL;
    }

    if( AwsIotMutex_Create( &( pNewMqttConnection->subscriptionMutex ) ) == false )
    {
        AwsIotLogError( "Failed to create subscription mutex for new connection." );
        AwsIotMutex_Destroy( &( pNewMqttConnection->timerMutex ) );
        AwsIotMqtt_FreeConnection( pNewMqttConnection );

        return NULL;
    }

    /* Create the new connection's subscription and timer event lists. */
    IotListDouble_Create( &( pNewMqttConnection->subscriptionList ) );
    IotListDouble_Create( &( pNewMqttConnection->timerEventList ) );

    /* Create the timer mutex for a new connection. */
    if( AwsIotClock_TimerCreate( &( pNewMqttConnection->timer ),
                                 _timerThread,
                                 pNewMqttConnection ) == false )
    {
        AwsIotLogError( "Failed to create timer for new connection." );
        AwsIotMutex_Destroy( &( pNewMqttConnection->timerMutex ) );
        AwsIotMutex_Destroy( &( pNewMqttConnection->subscriptionMutex ) );
        AwsIotMqtt_FreeConnection( pNewMqttConnection );

        return NULL;
    }

    /* Check if keep-alive is active for this connection. */
    if( keepAliveSeconds != 0 )
    {
        /* Save the keep-alive interval. */
        pNewMqttConnection->keepAliveSeconds = keepAliveSeconds;

        /* Allocate memory for keep-alive timer event. */
        pNewMqttConnection->pKeepAliveEvent = AwsIotMqtt_MallocTimerEvent( sizeof( _mqttTimerEvent_t ) );

        if( pNewMqttConnection->pKeepAliveEvent == NULL )
        {
            AwsIotLogError( "Failed to allocate keep-alive event for new connection." );
            _destroyMqttConnection( pNewMqttConnection );

            return NULL;
        }

        /* Create PINGREQ operation. */
        if( AwsIotMqttInternal_CreateOperation( &( pNewMqttConnection->pPingreqOperation ),
                                                AWS_IOT_MQTT_FLAG_WAITABLE,
                                                NULL ) != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogError( "Failed to allocate PINGREQ operation for new connection." );
            _destroyMqttConnection( pNewMqttConnection );

            return NULL;
        }

        /* Set the members of the PINGREQ operations. */
        pNewMqttConnection->pPingreqOperation->operation = AWS_IOT_MQTT_PINGREQ;
        pNewMqttConnection->pPingreqOperation->pMqttConnection = pNewMqttConnection;

        /* Choose a PINGREQ serializer function. */
        AwsIotMqttError_t ( * serializePingreq )( uint8_t ** const,
                                                  size_t * const ) = AwsIotMqttInternal_SerializePingreq;

        #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pNewMqttConnection->network.serialize.pingreq != NULL )
            {
                serializePingreq = pNewMqttConnection->network.serialize.pingreq;
            }
        #endif

        if( serializePingreq( &( pNewMqttConnection->pPingreqOperation->pMqttPacket ),
                              &( pNewMqttConnection->pPingreqOperation->packetSize ) ) != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize PINGREQ packet for new connection." );
            _destroyMqttConnection( pNewMqttConnection );

            return NULL;
        }

        /* Set the members of the keep-alive timer event. */
        ( void ) memset( pNewMqttConnection->pKeepAliveEvent, 0x00, sizeof( _mqttTimerEvent_t ) );
        pNewMqttConnection->pKeepAliveEvent->pOperation = pNewMqttConnection->pPingreqOperation;
        pNewMqttConnection->pKeepAliveEvent->expirationTime = AwsIotClock_GetTimeMs() + keepAliveSeconds * 1000ULL;

        /* Add the PINGREQ to the timer event list. */
        IotListDouble_InsertSorted( &( pNewMqttConnection->timerEventList ),
                                    &( pNewMqttConnection->pKeepAliveEvent->link ),
                                    AwsIotMqttInternal_TimerEventCompare );
    }

    return pNewMqttConnection;
}

/*-----------------------------------------------------------*/

static void _destroyMqttConnection( _mqttConnection_t * const pMqttConnection )
{
    /* Destroy keep-alive timer event. */
    if( pMqttConnection->pKeepAliveEvent != NULL )
    {
        AwsIotMqtt_FreeTimerEvent( pMqttConnection->pKeepAliveEvent );
        pMqttConnection->pKeepAliveEvent = NULL;
    }

    /* Destroy keep-alive operation. */
    if( pMqttConnection->pPingreqOperation != NULL )
    {
        AwsIotMqttInternal_DestroyOperation( pMqttConnection->pPingreqOperation );
        pMqttConnection->pPingreqOperation = NULL;
    }

    /* Remove any previous session subscriptions. */
    AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );
    IotListDouble_RemoveAll( &( pMqttConnection->subscriptionList ),
                             AwsIotMqtt_FreeSubscription,
                             offsetof( _mqttSubscription_t, link ) );
    AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );

    /* Destroy timer and mutexes. */
    AwsIotClock_TimerDestroy( &( pMqttConnection->timer ) );
    AwsIotMutex_Destroy( &( pMqttConnection->timerMutex ) );
    AwsIotMutex_Destroy( &( pMqttConnection->subscriptionMutex ) );
    AwsIotMqtt_FreeConnection( pMqttConnection );
}

/*-----------------------------------------------------------*/

static AwsIotMqttError_t _connectCommon( AwsIotMqttConnection_t * pMqttConnection,
                                         const AwsIotMqttNetIf_t * const pNetworkInterface,
                                         const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                         const AwsIotMqttPublishInfo_t * const pWillInfo,
                                         const AwsIotMqttSubscription_t * const pSessionSubscriptions,
                                         size_t sessionSubscriptionsCount,
                                         uint64_t timeoutMs )
{
    AwsIotMqttError_t connectStatus = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pNewMqttConnection = NULL;
    _mqttOperation_t * pConnectOperation = NULL;

    /* Default CONNECT serializer function. */
    AwsIotMqttError_t ( * serializeConnect )( const AwsIotMqttConnectInfo_t * const,
                                              const AwsIotMqttPublishInfo_t * const,
                                              uint8_t ** const,
                                              size_t * const ) = AwsIotMqttInternal_SerializeConnect;

    /* Check that the network interface is valid. */
    if( AwsIotMqttInternal_ValidateNetIf( pNetworkInterface ) == false )
    {
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* Choose a CONNECT serializer function. */
    #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pNetworkInterface->serialize.connect != NULL )
        {
            serializeConnect = pNetworkInterface->serialize.connect;
        }
    #endif

    /* Check that the connection info is valid. */
    if( AwsIotMqttInternal_ValidateConnect( pConnectInfo ) == false )
    {
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* If will info is provided, check that it is valid. */
    if( pWillInfo != NULL )
    {
        if( AwsIotMqttInternal_ValidatePublish( pConnectInfo->awsIotMqttMode,
                                                pWillInfo ) == false )
        {
            return AWS_IOT_MQTT_BAD_PARAMETER;
        }

        /* Will message payloads cannot be larger than 65535. This restriction
         * applies only to will messages, and not normal PUBLISH messages. */
        if( pWillInfo->payloadLength > UINT16_MAX )
        {
            AwsIotLogError( "Will payload cannot be larger than 65535." );

            return AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    AwsIotLogInfo( "Establishing new MQTT connection." );

    /* Create a CONNECT operation. */
    connectStatus = AwsIotMqttInternal_CreateOperation( &pConnectOperation,
                                                        AWS_IOT_MQTT_FLAG_WAITABLE,
                                                        NULL );

    if( connectStatus != AWS_IOT_MQTT_SUCCESS )
    {
        return connectStatus;
    }

    /* Ensure the members set by operation creation and serialization
     * are appropriate for a blocking CONNECT. */
    AwsIotMqtt_Assert( pConnectOperation->pPublishRetry == NULL );
    AwsIotMqtt_Assert( pConnectOperation->status == AWS_IOT_MQTT_STATUS_PENDING );
    AwsIotMqtt_Assert( ( pConnectOperation->flags & AWS_IOT_MQTT_FLAG_WAITABLE )
                       == AWS_IOT_MQTT_FLAG_WAITABLE );

    /* Set the operation type. */
    pConnectOperation->operation = AWS_IOT_MQTT_CONNECT;

    /* Allocate memory to store data for the new MQTT connection. */
    pNewMqttConnection = _createMqttConnection( pConnectInfo->awsIotMqttMode,
                                                pNetworkInterface,
                                                pConnectInfo->keepAliveSeconds );

    if( pNewMqttConnection == NULL )
    {
        AwsIotMqttInternal_DestroyOperation( pConnectOperation );

        return AWS_IOT_MQTT_NO_MEMORY;
    }

    /* Set the MQTT connection. */
    pConnectOperation->pMqttConnection = pNewMqttConnection;

    /* Add previous session subscriptions. */
    if( ( pSessionSubscriptions != NULL ) && ( sessionSubscriptionsCount > 0 ) )
    {
        connectStatus = AwsIotMqttInternal_AddSubscriptions( pNewMqttConnection,
                                                             2,
                                                             pSessionSubscriptions,
                                                             sessionSubscriptionsCount );

        if( connectStatus != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotMqttInternal_DestroyOperation( pConnectOperation );
            _destroyMqttConnection( pNewMqttConnection );

            return connectStatus;
        }
    }

    /* Convert the connect info and will info objects to an MQTT CONNECT packet. */
    connectStatus = serializeConnect( pConnectInfo,
                                      pWillInfo,
                                      &( pConnectOperation->pMqttPacket ),
                                      &( pConnectOperation->packetSize ) );

    if( connectStatus != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotMqttInternal_DestroyOperation( pConnectOperation );
        _destroyMqttConnection( pNewMqttConnection );

        return connectStatus;
    }

    /* Check the serialized MQTT packet. */
    AwsIotMqtt_Assert( pConnectOperation->pMqttPacket != NULL );
    AwsIotMqtt_Assert( pConnectOperation->packetSize > 0 );

    /* Set the output parameter so it may be used by the network receive callback. */
    *pMqttConnection = pNewMqttConnection;

    /* Prevent another CONNECT operation from using the network. */
    AwsIotMutex_Lock( &_connectMutex );

    /* Add the CONNECT operation to the send queue for network transmission. */
    if( AwsIotMqttInternal_EnqueueOperation( pConnectOperation,
                                             &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogError( "Failed to enqueue CONNECT for sending." );
        connectStatus = AWS_IOT_MQTT_NO_MEMORY;
        AwsIotMqttInternal_DestroyOperation( pConnectOperation );
    }
    else
    {
        /* Wait for the CONNECT operation to complete, i.e. wait for CONNACK. */
        connectStatus = AwsIotMqtt_Wait( ( AwsIotMqttReference_t ) pConnectOperation,
                                         timeoutMs );
    }

    /* Unlock the CONNECT mutex. */
    AwsIotMutex_Unlock( &_connectMutex );

    /* Arm the timer for the first keep alive expiration if keep-alive is
     * active for this connection. */
    if( ( connectStatus == AWS_IOT_MQTT_SUCCESS ) &&
        ( pNewMqttConnection->keepAliveSeconds > 0 ) )
    {
        AwsIotLogDebug( "Starting new MQTT connection timer." );

        if( AwsIotClock_TimerArm( &( pNewMqttConnection->timer ),
                                  pNewMqttConnection->pKeepAliveEvent->expirationTime - AwsIotClock_GetTimeMs(),
                                  0 ) == false )
        {
            AwsIotLogError( "Failed to start connection timer for new MQTT connection" );

            connectStatus = AWS_IOT_MQTT_INIT_FAILED;
        }
    }

    /* Check the status of the CONNECT operation. */
    if( connectStatus == AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogInfo( "New MQTT connection %p established.", pNewMqttConnection );
    }
    else
    {
        /* Otherwise, free resources and log an error. */
        _destroyMqttConnection( pNewMqttConnection );
        *pMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;

        AwsIotLogError( "Failed to establish new MQTT connection, error %s.",
                        AwsIotMqtt_strerror( connectStatus ) );
    }

    return connectStatus;
}

/*-----------------------------------------------------------*/

static AwsIotMqttError_t _subscriptionCommon( AwsIotMqttOperationType_t operation,
                                              AwsIotMqttConnection_t mqttConnection,
                                              const AwsIotMqttSubscription_t * const pSubscriptionList,
                                              size_t subscriptionCount,
                                              uint32_t flags,
                                              const AwsIotMqttCallbackInfo_t * const pCallbackInfo,
                                              AwsIotMqttReference_t * const pSubscriptionRef )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttOperation_t * pSubscriptionOperation = NULL;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;

    /* Subscription serializer function. */
    AwsIotMqttError_t ( * serializeSubscription )( const AwsIotMqttSubscription_t * const,
                                                   size_t,
                                                   uint8_t ** const,
                                                   size_t * const,
                                                   uint16_t * const ) = NULL;

    /* This function should only be called for subscribe or unsubscribe. */
    AwsIotMqtt_Assert( ( operation == AWS_IOT_MQTT_SUBSCRIBE ) ||
                       ( operation == AWS_IOT_MQTT_UNSUBSCRIBE ) );

    /* Choose a subscription serialize function. */
    if( operation == AWS_IOT_MQTT_SUBSCRIBE )
    {
        serializeSubscription = AwsIotMqttInternal_SerializeSubscribe;

        #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pMqttConnection->network.serialize.subscribe != NULL )
            {
                serializeSubscription = pMqttConnection->network.serialize.subscribe;
            }
        #endif
    }
    else
    {
        serializeSubscription = AwsIotMqttInternal_SerializeUnsubscribe;

        #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pMqttConnection->network.serialize.unsubscribe != NULL )
            {
                serializeSubscription = pMqttConnection->network.serialize.unsubscribe;
            }
        #endif
    }

    /* Check that a reference pointer is provided for a waitable operation. */
    if( ( ( flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE ) &&
        ( pSubscriptionRef == NULL ) )
    {
        AwsIotLogError( "Reference must be provided for a waitable %s.",
                        AwsIotMqtt_OperationType( operation ) );

        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* Check that all elements in the subscription list are valid. */
    if( AwsIotMqttInternal_ValidateSubscriptionList( operation,
                                                     pMqttConnection->awsIotMqttMode,
                                                     pSubscriptionList,
                                                     subscriptionCount ) == false )
    {
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* Remove the MQTT subscription list for an UNSUBSCRIBE. */
    if( operation == AWS_IOT_MQTT_UNSUBSCRIBE )
    {
        AwsIotMqttInternal_RemoveSubscriptionByTopicFilter( pMqttConnection,
                                                            pSubscriptionList,
                                                            subscriptionCount );
    }

    /* Create a subscription operation. */
    status = AwsIotMqttInternal_CreateOperation( &pSubscriptionOperation,
                                                 flags,
                                                 pCallbackInfo );

    if( status != AWS_IOT_MQTT_SUCCESS )
    {
        return status;
    }

    /* Check the subscription operation data and set the remaining members. */
    AwsIotMqtt_Assert( pSubscriptionOperation->pPublishRetry == NULL );
    AwsIotMqtt_Assert( pSubscriptionOperation->status == AWS_IOT_MQTT_STATUS_PENDING );
    pSubscriptionOperation->operation = operation;
    pSubscriptionOperation->pMqttConnection = pMqttConnection;

    /* Generate a subscription packet from the subscription list. */
    status = serializeSubscription( pSubscriptionList,
                                    subscriptionCount,
                                    &( pSubscriptionOperation->pMqttPacket ),
                                    &( pSubscriptionOperation->packetSize ),
                                    &( pSubscriptionOperation->packetIdentifier ) );

    if( status != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotMqttInternal_DestroyOperation( pSubscriptionOperation );

        return status;
    }

    /* Check the serialized MQTT packet. */
    AwsIotMqtt_Assert( pSubscriptionOperation->pMqttPacket != NULL );
    AwsIotMqtt_Assert( pSubscriptionOperation->packetSize > 0 );

    /* Add the subscription list for a SUBSCRIBE. */
    if( operation == AWS_IOT_MQTT_SUBSCRIBE )
    {
        status = AwsIotMqttInternal_AddSubscriptions( pMqttConnection,
                                                      pSubscriptionOperation->packetIdentifier,
                                                      pSubscriptionList,
                                                      subscriptionCount );

        if( status != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotMqttInternal_DestroyOperation( pSubscriptionOperation );

            return status;
        }
    }

    /* Set the reference, if provided. This must be set before the subscription
     * is pushed to the network queue to avoid a race condition. */
    if( pSubscriptionRef != NULL )
    {
        *pSubscriptionRef = pSubscriptionOperation;
    }

    /* Add the subscription operation to the send queue for network transmission. */
    if( AwsIotMqttInternal_EnqueueOperation( pSubscriptionOperation,
                                             &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogError( "Failed to enqueue %s for sending.",
                        AwsIotMqtt_OperationType( operation ) );

        if( operation == AWS_IOT_MQTT_SUBSCRIBE )
        {
            AwsIotMqttInternal_RemoveSubscriptionByPacket( pMqttConnection,
                                                           pSubscriptionOperation->packetIdentifier,
                                                           -1 );
        }

        AwsIotMqttInternal_DestroyOperation( pSubscriptionOperation );

        /* Clear the previously set (and now invalid) reference. */
        if( pSubscriptionRef != NULL )
        {
            *pSubscriptionRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
        }

        return AWS_IOT_MQTT_NO_MEMORY;
    }

    AwsIotLogInfo( "MQTT %s operation queued.",
                   AwsIotMqtt_OperationType( operation ) );

    /* The subscription operation is waiting for a network response. */
    return AWS_IOT_MQTT_STATUS_PENDING;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_Init( void )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_SUCCESS;

    /* Create mutex protecting MQTT operation queues. */
    if( AwsIotMutex_Create( &( _IotMqttQueueMutex ) ) == false )
    {
        AwsIotLogError( "Failed to initialize MQTT operation queue mutex." );
        status = AWS_IOT_MQTT_INIT_FAILED;
    }

    /* Create mutex protecting list of operations pending network responses. */
    if( status == AWS_IOT_MQTT_SUCCESS )
    {
        if( AwsIotMutex_Create( &( _IotMqttPendingResponseMutex ) ) == false )
        {
            AwsIotLogError( "Failed to initialize MQTT library pending response mutex." );
            AwsIotMutex_Destroy( &( _IotMqttQueueMutex ) );

            status = AWS_IOT_MQTT_INIT_FAILED;
        }
    }

    /* Create CONNECT mutex. */
    if( status == AWS_IOT_MQTT_SUCCESS )
    {
        if( AwsIotMutex_Create( &( _connectMutex ) ) == false )
        {
            AwsIotLogError( "Failed to initialize MQTT library connect mutex." );
            AwsIotMutex_Destroy( &( _IotMqttPendingResponseMutex ) );
            AwsIotMutex_Destroy( &( _IotMqttQueueMutex ) );

            status = AWS_IOT_MQTT_INIT_FAILED;
        }
    }

    /* Create semaphores that count active MQTT library threads. */
    if( status == AWS_IOT_MQTT_SUCCESS )
    {
        /* Create semaphore that counts active callback threads. */
        if( AwsIotSemaphore_Create( &( _IotMqttCallback.availableThreads ),
                                    AWS_IOT_MQTT_MAX_CALLBACK_THREADS,
                                    AWS_IOT_MQTT_MAX_CALLBACK_THREADS ) == false )
        {
            AwsIotLogError( "Failed to initialize record of active MQTT callback threads." );
            status = AWS_IOT_MQTT_INIT_FAILED;
        }
        else
        {
            /* Create semaphore that counts active send threads. */
            if( AwsIotSemaphore_Create( &( _IotMqttSend.availableThreads ),
                                        AWS_IOT_MQTT_MAX_SEND_THREADS,
                                        AWS_IOT_MQTT_MAX_SEND_THREADS ) == false )
            {
                AwsIotLogError( "Failed to initialize record of active MQTT send threads." );
                status = AWS_IOT_MQTT_INIT_FAILED;

                AwsIotSemaphore_Destroy( &( _IotMqttCallback.availableThreads ) );
            }
        }

        /* Destroy previously created mutexes if thread counter semaphores could
         * not be created. */
        if( status == AWS_IOT_MQTT_INIT_FAILED )
        {
            AwsIotMutex_Destroy( &( _connectMutex ) );
            AwsIotMutex_Destroy( &( _IotMqttPendingResponseMutex ) );
            AwsIotMutex_Destroy( &( _IotMqttQueueMutex ) );
        }
    }

    /* Initialize MQTT serializer. */
    if( status == AWS_IOT_MQTT_SUCCESS )
    {
        if( AwsIotMqttInternal_InitSerialize() != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogError( "Failed to initialize MQTT library serializer. " );
            AwsIotSemaphore_Destroy( &( _IotMqttCallback.availableThreads ) );
            AwsIotSemaphore_Destroy( &( _IotMqttSend.availableThreads ) );
            AwsIotMutex_Destroy( &( _connectMutex ) );
            AwsIotMutex_Destroy( &( _IotMqttPendingResponseMutex ) );
            AwsIotMutex_Destroy( &( _IotMqttQueueMutex ) );

            status = AWS_IOT_MQTT_INIT_FAILED;
        }
    }

    /* Create MQTT library linear containers. */
    if( status == AWS_IOT_MQTT_SUCCESS )
    {
        IotQueue_Create( &( _IotMqttCallback.queue ) );
        IotQueue_Create( &( _IotMqttSend.queue ) );
        IotListDouble_Create( &( _IotMqttPendingResponse ) );

        AwsIotLogInfo( "MQTT library successfully initialized." );
    }

    return status;
}

/*-----------------------------------------------------------*/

void AwsIotMqtt_Cleanup()
{
    /* Wait for termination of any active MQTT library threads. */
    AwsIotMutex_Lock( &( _IotMqttQueueMutex ) );

    while( AwsIotSemaphore_GetCount( &( _IotMqttCallback.availableThreads ) ) > 0 )
    {
        AwsIotMutex_Unlock( &( _IotMqttQueueMutex ) );
        AwsIotSemaphore_Wait( &( _IotMqttCallback.availableThreads ) );
        AwsIotMutex_Lock( &( _IotMqttQueueMutex ) );
    }

    while( AwsIotSemaphore_GetCount( &( _IotMqttSend.availableThreads ) ) > 0 )
    {
        AwsIotMutex_Unlock( &( _IotMqttQueueMutex ) );
        AwsIotSemaphore_Wait( &( _IotMqttSend.availableThreads ) );
        AwsIotMutex_Lock( &( _IotMqttQueueMutex ) );
    }

    AwsIotMutex_Unlock( &( _IotMqttQueueMutex ) );

    /* This API requires all MQTT connections to be terminated. If the MQTT library
     * linear containers are not empty, there is an active MQTT connection and the
     * library cannot be safely shut down. */
    AwsIotMqtt_Assert( IotQueue_IsEmpty( &( _IotMqttCallback.queue ) ) == true );
    AwsIotMqtt_Assert( IotQueue_IsEmpty( &( _IotMqttSend.queue ) ) == true );
    AwsIotMqtt_Assert( IotListDouble_IsEmpty( &( _IotMqttPendingResponse ) ) == true );

    /* Clean up MQTT library mutexes. */
    AwsIotMutex_Destroy( &( _connectMutex ) );
    AwsIotMutex_Destroy( &( _IotMqttPendingResponseMutex ) );
    AwsIotMutex_Destroy( &( _IotMqttQueueMutex ) );

    /* Clean up thread counter semaphores. */
    AwsIotSemaphore_Destroy( &( _IotMqttCallback.availableThreads ) );
    AwsIotSemaphore_Destroy( &( _IotMqttSend.availableThreads ) );

    /* Clean up MQTT serializer. */
    AwsIotMqttInternal_CleanupSerialize();

    AwsIotLogInfo( "MQTT library cleanup done." );
}

/*-----------------------------------------------------------*/

int32_t AwsIotMqtt_ReceiveCallback( AwsIotMqttConnection_t * pMqttConnection,
                                    const void * pReceivedData,
                                    size_t offset,
                                    size_t dataLength,
                                    void ( *freeReceivedData )( void * ) )
{
    size_t bytesProcessed = 0, totalBytesProcessed = 0, remainingDataLength = 0;
    _mqttConnection_t * pConnectionInfo = *( ( _mqttConnection_t ** ) ( pMqttConnection ) );
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    uint16_t packetIdentifier = 0;
    const uint8_t * pNextPacket = ( const uint8_t * ) pReceivedData;
    _mqttOperation_t * pOperation = NULL, * pFirstPublish = NULL, * pLastPublish = NULL;

    /* Choose a packet type decoder function. */
    uint8_t ( * getPacketType )( const uint8_t * const,
                                 size_t ) = AwsIotMqttInternal_GetPacketType;

    #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pConnectionInfo->network.getPacketType != NULL )
        {
            getPacketType = pConnectionInfo->network.getPacketType;
        }
    #endif

    /* Ensure that offset is smaller than dataLength. */
    if( offset >= dataLength )
    {
        return 0;
    }

    /* Adjust the packet pointer based on the offset. */
    pNextPacket += offset;
    remainingDataLength = dataLength - offset;

    /* Process the stream of data until the entire stream is proccessed or an
     * incomplete packet is found. */
    while( ( totalBytesProcessed < remainingDataLength ) && ( status != AWS_IOT_MQTT_BAD_RESPONSE ) )
    {
        switch( getPacketType( pNextPacket, remainingDataLength - totalBytesProcessed ) )
        {
            case _MQTT_PACKET_TYPE_CONNACK:
                AwsIotLog_PrintBuffer( "CONNACK in data stream:", pNextPacket, remainingDataLength - totalBytesProcessed );

                /* Deserialize the CONNACK. */
                AwsIotMqttError_t ( * deserializeConnack )( const uint8_t * const,
                                                            size_t,
                                                            size_t * const ) = AwsIotMqttInternal_DeserializeConnack;

                #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pConnectionInfo->network.deserialize.connack != NULL )
                    {
                        deserializeConnack = pConnectionInfo->network.deserialize.connack;
                    }
                #endif

                status = deserializeConnack( pNextPacket,
                                             remainingDataLength - totalBytesProcessed,
                                             &bytesProcessed );

                /* If a complete CONNACK was deserialized, check if there's an
                 * in-progress CONNECT operation. */
                if( bytesProcessed > 0 )
                {
                    pOperation = AwsIotMqttInternal_FindOperation( AWS_IOT_MQTT_CONNECT, NULL );

                    if( pOperation != NULL )
                    {
                        pOperation->status = status;
                        AwsIotMqttInternal_Notify( pOperation );
                    }
                }

                break;

            case _MQTT_PACKET_TYPE_PUBLISH:
                AwsIotLog_PrintBuffer( "PUBLISH in data stream:", pNextPacket, remainingDataLength - totalBytesProcessed );

                /* Allocate memory to handle the incoming PUBLISH. */
                pOperation = AwsIotMqtt_MallocOperation( sizeof( _mqttOperation_t ) );

                if( pOperation == NULL )
                {
                    AwsIotLogWarn( "Failed to allocate memory for incoming PUBLISH." );
                    bytesProcessed = 0;

                    break;
                }

                /* Set the members of the incoming PUBLISH operation. */
                ( void ) memset( pOperation, 0x00, sizeof( _mqttOperation_t ) );
                pOperation->incomingPublish = true;
                pOperation->pMqttConnection = pConnectionInfo;

                /* Deserialize the PUBLISH into an AwsIotMqttPublishInfo_t. */
                AwsIotMqttError_t ( * deserializePublish )( const uint8_t * const,
                                                            size_t,
                                                            AwsIotMqttPublishInfo_t * const,
                                                            uint16_t * const,
                                                            size_t * const ) = AwsIotMqttInternal_DeserializePublish;

                #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pConnectionInfo->network.deserialize.publish != NULL )
                    {
                        deserializePublish = pConnectionInfo->network.deserialize.publish;
                    }
                #endif

                status = deserializePublish( pNextPacket,
                                             remainingDataLength - totalBytesProcessed,
                                             &( pOperation->publishInfo ),
                                             &packetIdentifier,
                                             &bytesProcessed );

                /* If a complete PUBLISH was deserialized, process it. */
                if( ( bytesProcessed > 0 ) && ( status == AWS_IOT_MQTT_SUCCESS ) )
                {
                    /* If a QoS 1 PUBLISH was received, send a PUBACK. */
                    if( pOperation->publishInfo.QoS == 1 )
                    {
                        _sendPuback( pConnectionInfo, packetIdentifier );
                    }

                    /* Change the first and last PUBLISH pointers. */
                    if( pFirstPublish == NULL )
                    {
                        pFirstPublish = pOperation;
                        pLastPublish = pOperation;
                    }
                    else
                    {
                        pLastPublish->pNextPublish = pOperation;
                        pLastPublish = pOperation;
                    }
                }
                else
                {
                    /* Free the PUBLISH operation here if the PUBLISH packet isn't
                     * valid. */
                    AwsIotMqtt_FreeOperation( pOperation );
                }

                break;

            case _MQTT_PACKET_TYPE_PUBACK:
                AwsIotLog_PrintBuffer( "PUBACK in data stream:", pNextPacket, remainingDataLength - totalBytesProcessed );

                /* Deserialize the PUBACK to get the packet identifier. */
                AwsIotMqttError_t ( * deserializePuback )( const uint8_t * const,
                                                           size_t,
                                                           uint16_t * const,
                                                           size_t * const ) = AwsIotMqttInternal_DeserializePuback;

                #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pConnectionInfo->network.deserialize.puback != NULL )
                    {
                        deserializePuback = pConnectionInfo->network.deserialize.puback;
                    }
                #endif

                status = deserializePuback( pNextPacket,
                                            remainingDataLength - totalBytesProcessed,
                                            &packetIdentifier,
                                            &bytesProcessed );

                /* If a complete PUBACK packet was deserialized, find an in-progress
                 * PUBLISH with a matching client identifier. */
                if( bytesProcessed > 0 )
                {
                    pOperation = AwsIotMqttInternal_FindOperation( AWS_IOT_MQTT_PUBLISH_TO_SERVER, &packetIdentifier );

                    if( pOperation != NULL )
                    {
                        pOperation->status = status;
                        AwsIotMqttInternal_Notify( pOperation );
                    }
                }

                break;

            case _MQTT_PACKET_TYPE_SUBACK:
                AwsIotLog_PrintBuffer( "SUBACK in data stream:", pNextPacket, remainingDataLength - totalBytesProcessed );

                /* Deserialize the SUBACK to get the packet identifier. */
                AwsIotMqttError_t ( * deserializeSuback )( AwsIotMqttConnection_t,
                                                           const uint8_t * const,
                                                           size_t,
                                                           uint16_t * const,
                                                           size_t * const ) = AwsIotMqttInternal_DeserializeSuback;

                #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pConnectionInfo->network.deserialize.suback != NULL )
                    {
                        deserializeSuback = pConnectionInfo->network.deserialize.suback;
                    }
                #endif

                status = deserializeSuback( pConnectionInfo,
                                            pNextPacket,
                                            remainingDataLength - totalBytesProcessed,
                                            &packetIdentifier,
                                            &bytesProcessed );

                /* If a complete SUBACK was deserialized, find an in-progress
                 * SUBACK with a matching client identifier. */
                if( bytesProcessed > 0 )
                {
                    pOperation = AwsIotMqttInternal_FindOperation( AWS_IOT_MQTT_SUBSCRIBE, &packetIdentifier );

                    if( pOperation != NULL )
                    {
                        pOperation->status = status;
                        AwsIotMqttInternal_Notify( pOperation );
                    }
                }

                break;

            case _MQTT_PACKET_TYPE_UNSUBACK:
                AwsIotLog_PrintBuffer( "UNSUBACK in data stream:", pNextPacket, remainingDataLength - totalBytesProcessed );

                /* Deserialize the UNSUBACK to get the packet identifier. */
                AwsIotMqttError_t ( * deserializeUnsuback )( const uint8_t * const,
                                                             size_t,
                                                             uint16_t * const,
                                                             size_t * const ) = AwsIotMqttInternal_DeserializeUnsuback;

                #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pConnectionInfo->network.deserialize.unsuback != NULL )
                    {
                        deserializeUnsuback = pConnectionInfo->network.deserialize.unsuback;
                    }
                #endif

                status = deserializeUnsuback( pNextPacket,
                                              remainingDataLength - totalBytesProcessed,
                                              &packetIdentifier,
                                              &bytesProcessed );

                /* If a complete UNSUBACK was deserialized, find an in-progress
                 * UNSUBACK with a matching client identifier. */
                if( bytesProcessed > 0 )
                {
                    pOperation = AwsIotMqttInternal_FindOperation( AWS_IOT_MQTT_UNSUBSCRIBE, &packetIdentifier );

                    if( pOperation != NULL )
                    {
                        pOperation->status = status;
                        AwsIotMqttInternal_Notify( pOperation );
                    }
                }

                break;

            case _MQTT_PACKET_TYPE_PINGRESP:
                AwsIotLog_PrintBuffer( "PINGRESP in data stream:", pNextPacket, remainingDataLength - totalBytesProcessed );

                /* Deserialize the PINGRESP. */
                AwsIotMqttError_t ( * deserializePingresp )( const uint8_t * const,
                                                             size_t,
                                                             size_t * const ) = AwsIotMqttInternal_DeserializePingresp;

                #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pConnectionInfo->network.deserialize.pingresp != NULL )
                    {
                        deserializePingresp = pConnectionInfo->network.deserialize.pingresp;
                    }
                #endif

                status = deserializePingresp( pNextPacket,
                                              remainingDataLength - totalBytesProcessed,
                                              &bytesProcessed );

                /* If a complete PINGRESP was deserialized, check if there's an
                 * in-progress PINGREQ operation. */
                if( bytesProcessed > 0 )
                {
                    pOperation = AwsIotMqttInternal_FindOperation( AWS_IOT_MQTT_PINGREQ, NULL );

                    if( pOperation != NULL )
                    {
                        pOperation->status = status;
                        AwsIotMqttInternal_Notify( pOperation );
                    }
                }

                break;

            default:

                /* If an unknown packet is received, stop processing pReceivedData. */
                AwsIotLogError( "Unknown packet type %02x received.",
                                pNextPacket[ 0 ] );

                bytesProcessed = 1;
                status = AWS_IOT_MQTT_BAD_RESPONSE;

                break;
        }

        /* Check if a protocol violation was encountered. */
        if( ( bytesProcessed > 0 ) && ( status == AWS_IOT_MQTT_BAD_RESPONSE ) )
        {
            AwsIotLogError( "MQTT protocol violation encountered. Closing network connection" );

            /* Clean up any previously allocated incoming PUBLISH operations. */
            while( pFirstPublish != NULL )
            {
                pLastPublish = pFirstPublish;
                pFirstPublish = pFirstPublish->pNextPublish;

                AwsIotMqtt_FreeOperation( pLastPublish );
            }

            pLastPublish = NULL;

            /* Prevent the timer thread from running, then set the error flag. */
            AwsIotMutex_Lock( &( pConnectionInfo->timerMutex ) );

            pConnectionInfo->errorOccurred = true;

            if( pConnectionInfo->network.disconnect != NULL )
            {
                pConnectionInfo->network.disconnect( pConnectionInfo->network.pDisconnectContext );
            }
            else
            {
                AwsIotLogWarn( "No disconnect function was set. Network connection not closed." );
            }

            AwsIotMutex_Unlock( &( pConnectionInfo->timerMutex ) );

            return -1;
        }

        /* Check if a partial packet was encountered. */
        if( bytesProcessed == 0 )
        {
            break;
        }

        /* Move the "next packet" pointer and increment the number of bytes processed. */
        pNextPacket += bytesProcessed;
        totalBytesProcessed += bytesProcessed;

        /* Number of bytes processed should never exceed remainingDataLength. */
        AwsIotMqtt_Assert( pNextPacket - totalBytesProcessed - offset == pReceivedData );
        AwsIotMqtt_Assert( totalBytesProcessed <= remainingDataLength );
    }

    /* Only free pReceivedData if all bytes were processed and no PUBLISH messages
     * were in the data stream. */
    if( ( freeReceivedData != NULL ) &&
        ( totalBytesProcessed == remainingDataLength ) &&
        ( pFirstPublish == NULL ) )
    {
        AwsIotMqtt_Assert( pLastPublish == NULL );

        freeReceivedData( ( void * ) pReceivedData );
    }

    /* Add all PUBLISH messages to the pending operations queue. */
    if( pLastPublish != NULL )
    {
        /* If all bytes of the receive buffer were processed, set the function
         * to free the receive buffer. */
        if( ( totalBytesProcessed == remainingDataLength ) && ( freeReceivedData != NULL ) )
        {
            pLastPublish->pReceivedData = pReceivedData;
            pLastPublish->freeReceivedData = freeReceivedData;
        }
        else
        {
            /* When some of the receive buffer is unprocessed, the receive buffer is
             * given back to the calling function. The MQTT library cannot guarantee
             * that the calling function will keep the receive buffer in scope;
             * therefore, data in the receive buffer must be copied for the MQTT
             * library's use. */
            for( pOperation = pFirstPublish; pOperation != NULL; pOperation = pOperation->pNextPublish )
            {
                /* Neither the buffer pointer nor the free function should be set. */
                AwsIotMqtt_Assert( pOperation->pReceivedData == NULL );
                AwsIotMqtt_Assert( pOperation->freeReceivedData == NULL );

                /* Allocate a new buffer to hold the topic name and payload. */
                pOperation->pReceivedData = AwsIotMqtt_MallocMessage( pOperation->publishInfo.topicNameLength +
                                                                      pOperation->publishInfo.payloadLength );

                if( pOperation->pReceivedData != NULL )
                {
                    /* Copy the topic name and payload. */
                    ( void ) memcpy( ( void * ) pOperation->pReceivedData,
                                     pOperation->publishInfo.pTopicName,
                                     pOperation->publishInfo.topicNameLength );
                    ( void ) memcpy( ( uint8_t * ) ( pOperation->pReceivedData ) +
                                     pOperation->publishInfo.topicNameLength,
                                     pOperation->publishInfo.pPayload,
                                     pOperation->publishInfo.payloadLength );

                    /* Set the topic name and payload pointers into the new buffer.
                     * Also set the free function. */
                    pOperation->publishInfo.pTopicName = pOperation->pReceivedData;
                    pOperation->publishInfo.pPayload = ( uint8_t * ) ( pOperation->pReceivedData ) +
                                                       pOperation->publishInfo.topicNameLength;
                    pOperation->freeReceivedData = AwsIotMqtt_FreeMessage;
                }
                else
                {
                    /* If a new buffer couldn't be allocated, clear the topic name and
                     * payload pointers so that this PUBLISH message will be ignored. */
                    AwsIotLogWarn( "Failed to allocate memory for incoming PUBLISH message." );
                    pOperation->publishInfo.pTopicName = NULL;
                    pOperation->publishInfo.topicNameLength = 0;
                    pOperation->publishInfo.pPayload = NULL;
                    pOperation->publishInfo.payloadLength = 0;
                }
            }
        }

        if( AwsIotMqttInternal_EnqueueOperation( pFirstPublish,
                                                 &( _IotMqttCallback ) ) != AWS_IOT_MQTT_SUCCESS )
        {
            AwsIotLogWarn( "Failed to enqueue incoming PUBLISH for callback." );
        }
    }

    return ( int32_t ) totalBytesProcessed;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_Connect( AwsIotMqttConnection_t * pMqttConnection,
                                      const AwsIotMqttNetIf_t * const pNetworkInterface,
                                      const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                      const AwsIotMqttPublishInfo_t * const pWillInfo,
                                      uint64_t timeoutMs )
{
    return _connectCommon( pMqttConnection,
                           pNetworkInterface,
                           pConnectInfo,
                           pWillInfo,
                           NULL,
                           0,
                           timeoutMs );
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_ConnectRestoreSession( AwsIotMqttConnection_t * pMqttConnection,
                                                    const AwsIotMqttNetIf_t * const pNetworkInterface,
                                                    const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                                    const AwsIotMqttPublishInfo_t * const pWillInfo,
                                                    const AwsIotMqttSubscription_t * const pSessionSubscriptions,
                                                    size_t sessionSubscriptionsCount,
                                                    uint64_t timeoutMs )
{
    /* Check that clean session is false. */
    if( pConnectInfo->cleanSession != false )
    {
        AwsIotLogError( "AwsIotMqtt_ConnectRestoreSession must have pConnectInfo->cleanSession == false. "
                        "AwsIotMqtt_Connect should be used to establish a clean session." );

        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* Validate previous session subscriptions. */
    if( AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE,
                                                     pConnectInfo->awsIotMqttMode,
                                                     pSessionSubscriptions,
                                                     sessionSubscriptionsCount ) == false )
    {
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    return _connectCommon( pMqttConnection,
                           pNetworkInterface,
                           pConnectInfo,
                           pWillInfo,
                           pSessionSubscriptions,
                           sessionSubscriptionsCount,
                           timeoutMs );
}

/*-----------------------------------------------------------*/

void AwsIotMqtt_Disconnect( AwsIotMqttConnection_t mqttConnection,
                            bool cleanupOnly )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;
    _mqttOperation_t * pDisconnectOperation = NULL;

    AwsIotLogInfo( "Disconnecting MQTT connection %p.", pMqttConnection );

    /* Purge all of this connection's subscriptions. */
    AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );
    IotListDouble_RemoveAllMatches( &( pMqttConnection->subscriptionList ),
                                    _mqttSubscription_shouldRemove,
                                    NULL,
                                    AwsIotMqtt_FreeSubscription,
                                    offsetof( _mqttSubscription_t, link ) );
    AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );

    /* Lock the connection timer mutex to block the timer thread. */
    AwsIotMutex_Lock( &( pMqttConnection->timerMutex ) );

    /* Purge all of this connection's pending operations and timer events. */
    IotQueue_RemoveAllMatches( &( _IotMqttSend.queue ),
                               _mqttOperation_matchConnection,
                               pMqttConnection,
                               AwsIotMqttInternal_DestroyOperation,
                               offsetof( _mqttOperation_t, link ) );
    IotListDouble_RemoveAllMatches( &( _IotMqttPendingResponse ),
                                    _mqttOperation_matchConnection,
                                    pMqttConnection,
                                    AwsIotMqttInternal_DestroyOperation,
                                    offsetof( _mqttOperation_t, link ) );
    IotQueue_RemoveAllMatches( &( _IotMqttCallback.queue ),
                               _mqttOperation_matchConnection,
                               pMqttConnection,
                               AwsIotMqttInternal_DestroyOperation,
                               offsetof( _mqttOperation_t, link ) );
    IotListDouble_RemoveAll( &( pMqttConnection->timerEventList ),
                             AwsIotMqtt_FreeTimerEvent,
                             offsetof( _mqttTimerEvent_t, link ) );

    /* Stop the connection timer. */
    AwsIotLogDebug( "Stopping connection timer." );
    AwsIotClock_TimerDestroy( &( pMqttConnection->timer ) );

    /* Only send a DISCONNECT packet if no error occurred and the "cleanup only"
     * option is false. */
    if( ( pMqttConnection->errorOccurred == false ) && ( cleanupOnly == false ) )
    {
        /* Create a DISCONNECT operation. This function blocks until the DISCONNECT
         * packet is sent, so it sets AWS_IOT_MQTT_FLAG_WAITABLE. */
        status = AwsIotMqttInternal_CreateOperation( &pDisconnectOperation,
                                                     AWS_IOT_MQTT_FLAG_WAITABLE,
                                                     NULL );

        if( status == AWS_IOT_MQTT_SUCCESS )
        {
            /* Ensure that the members set by operation creation and serialization
             * are appropriate for a blocking DISCONNECT. */
            AwsIotMqtt_Assert( pDisconnectOperation->pPublishRetry == NULL );
            AwsIotMqtt_Assert( pDisconnectOperation->status == AWS_IOT_MQTT_STATUS_PENDING );
            AwsIotMqtt_Assert( ( pDisconnectOperation->flags & AWS_IOT_MQTT_FLAG_WAITABLE )
                               == AWS_IOT_MQTT_FLAG_WAITABLE );

            /* Set the remaining members of the DISCONNECT operation. */
            pDisconnectOperation->operation = AWS_IOT_MQTT_DISCONNECT;
            pDisconnectOperation->pMqttConnection = pMqttConnection;

            /* Choose a disconnect serializer. */
            AwsIotMqttError_t ( * serializeDisconnect )( uint8_t ** const,
                                                         size_t * const ) = AwsIotMqttInternal_SerializeDisconnect;

            #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                if( pMqttConnection->network.serialize.disconnect != NULL )
                {
                    serializeDisconnect = pMqttConnection->network.serialize.disconnect;
                }
            #endif

            /* Generate a DISCONNECT packet. */
            status = serializeDisconnect( &( pDisconnectOperation->pMqttPacket ),
                                          &( pDisconnectOperation->packetSize ) );
        }

        if( status == AWS_IOT_MQTT_SUCCESS )
        {
            /* Check the serialized MQTT packet. */
            AwsIotMqtt_Assert( pDisconnectOperation->pMqttPacket != NULL );
            AwsIotMqtt_Assert( pDisconnectOperation->packetSize > 0 );

            /* Add the DISCONNECT operation to the send queue for network transmission. */
            if( AwsIotMqttInternal_EnqueueOperation( pDisconnectOperation,
                                                     &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
            {
                AwsIotLogWarn( "Failed to enqueue DISCONNECT for sending." );
                AwsIotMqttInternal_DestroyOperation( pDisconnectOperation );
            }
            else
            {
                /* Wait until the DISCONNECT packet has been transmitted. DISCONNECT
                 * should always be successful because it does not rely on any incoming
                 * data. */
                status = AwsIotMqtt_Wait( ( AwsIotMqttReference_t ) pDisconnectOperation,
                                          0 );

                /* A wait on DISCONNECT should only ever return SUCCESS or SEND ERROR. */
                AwsIotMqtt_Assert( ( status == AWS_IOT_MQTT_SUCCESS ) ||
                                   ( status == AWS_IOT_MQTT_SEND_ERROR ) );

                AwsIotLogInfo( "MQTT connection %p disconnected.", pMqttConnection );
            }
        }
    }

    /* Free the memory in use by the keep-alive operation. */
    if( pMqttConnection->pPingreqOperation != NULL )
    {
        AwsIotMqttInternal_DestroyOperation( pMqttConnection->pPingreqOperation );
    }

    /* Unlock the connection timer mutex. */
    AwsIotMutex_Unlock( &( pMqttConnection->timerMutex ) );

    /* Close the network connection regardless of whether an MQTT DISCONNECT
     * packet was sent. */
    if( pMqttConnection->network.disconnect != NULL )
    {
        pMqttConnection->network.disconnect( pMqttConnection->network.pDisconnectContext );
    }
    else
    {
        AwsIotLogWarn( "No disconnect function was set. Network connection not closed." );
    }

    /* Destroy the MQTT connection's mutexes. */
    AwsIotMutex_Destroy( &( pMqttConnection->timerMutex ) );
    AwsIotMutex_Destroy( &( pMqttConnection->subscriptionMutex ) );

    /* Free the memory used by this connection. */
    AwsIotMqtt_FreeConnection( pMqttConnection );
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_Subscribe( AwsIotMqttConnection_t mqttConnection,
                                        const AwsIotMqttSubscription_t * const pSubscriptionList,
                                        size_t subscriptionCount,
                                        uint32_t flags,
                                        const AwsIotMqttCallbackInfo_t * const pCallbackInfo,
                                        AwsIotMqttReference_t * const pSubscribeRef )
{
    return _subscriptionCommon( AWS_IOT_MQTT_SUBSCRIBE,
                                mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                flags,
                                pCallbackInfo,
                                pSubscribeRef );
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_TimedSubscribe( AwsIotMqttConnection_t mqttConnection,
                                             const AwsIotMqttSubscription_t * const pSubscriptionList,
                                             size_t subscriptionCount,
                                             uint32_t flags,
                                             uint64_t timeoutMs )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttReference_t subscribeRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;

    /* Flags are not used, but the parameter is present for future compatibility. */
    ( void ) flags;

    /* Call the asynchronous SUBSCRIBE function. */
    status = AwsIotMqtt_Subscribe( mqttConnection,
                                   pSubscriptionList,
                                   subscriptionCount,
                                   AWS_IOT_MQTT_FLAG_WAITABLE,
                                   NULL,
                                   &subscribeRef );

    /* Wait for the SUBSCRIBE operation to complete. */
    if( status == AWS_IOT_MQTT_STATUS_PENDING )
    {
        status = AwsIotMqtt_Wait( subscribeRef, timeoutMs );
    }

    /* Ensure that a status was set. */
    AwsIotMqtt_Assert( status != AWS_IOT_MQTT_STATUS_PENDING );

    return status;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_Unsubscribe( AwsIotMqttConnection_t mqttConnection,
                                          const AwsIotMqttSubscription_t * const pSubscriptionList,
                                          size_t subscriptionCount,
                                          uint32_t flags,
                                          const AwsIotMqttCallbackInfo_t * const pCallbackInfo,
                                          AwsIotMqttReference_t * const pUnsubscribeRef )
{
    return _subscriptionCommon( AWS_IOT_MQTT_UNSUBSCRIBE,
                                mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                flags,
                                pCallbackInfo,
                                pUnsubscribeRef );
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_TimedUnsubscribe( AwsIotMqttConnection_t mqttConnection,
                                               const AwsIotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint32_t flags,
                                               uint64_t timeoutMs )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttReference_t unsubscribeRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;

    /* Flags are not used, but the parameter is present for future compatibility. */
    ( void ) flags;

    /* Call the asynchronous UNSUBSCRIBE function. */
    status = AwsIotMqtt_Unsubscribe( mqttConnection,
                                     pSubscriptionList,
                                     subscriptionCount,
                                     AWS_IOT_MQTT_FLAG_WAITABLE,
                                     NULL,
                                     &unsubscribeRef );

    /* Wait for the UNSUBSCRIBE operation to complete. */
    if( status == AWS_IOT_MQTT_STATUS_PENDING )
    {
        status = AwsIotMqtt_Wait( unsubscribeRef, timeoutMs );
    }

    /* Ensure that a status was set. */
    AwsIotMqtt_Assert( status != AWS_IOT_MQTT_STATUS_PENDING );

    return status;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_Publish( AwsIotMqttConnection_t mqttConnection,
                                      const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                      uint32_t flags,
                                      const AwsIotMqttCallbackInfo_t * const pCallbackInfo,
                                      AwsIotMqttReference_t * const pPublishRef )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttOperation_t * pPublishOperation = NULL;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;

    /* Default PUBLISH serializer function. */
    AwsIotMqttError_t ( * serializePublish )( const AwsIotMqttPublishInfo_t * const,
                                              uint8_t ** const,
                                              size_t * const,
                                              uint16_t * const ) = AwsIotMqttInternal_SerializePublish;

    /* Choose a PUBLISH serializer function. */
    #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pMqttConnection->network.serialize.publish != NULL )
        {
            serializePublish = pMqttConnection->network.serialize.publish;
        }
    #endif

    /* Check that the PUBLISH information is valid. */
    if( AwsIotMqttInternal_ValidatePublish( pMqttConnection->awsIotMqttMode,
                                            pPublishInfo ) == false )
    {
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* Check that no notification is requested for a QoS 0 publish. */
    if( pPublishInfo->QoS == 0 )
    {
        if( ( ( flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE ) ||
            ( pCallbackInfo != NULL ) )
        {
            AwsIotLogError( "QoS 0 PUBLISH should not have notification parameters set." );

            return AWS_IOT_MQTT_BAD_PARAMETER;
        }

        if( pPublishRef != NULL )
        {
            AwsIotLogWarn( "Ignoring pPublishRef parameter for QoS 0 publish." );
        }
    }

    /* Check that a reference pointer is provided for a waitable operation. */
    if( ( ( flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE ) &&
        ( pPublishRef == NULL ) )
    {
        AwsIotLogError( "Reference must be provided for a waitable PUBLISH." );

        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* Create a PUBLISH operation. */
    status = AwsIotMqttInternal_CreateOperation( &pPublishOperation,
                                                 flags,
                                                 pCallbackInfo );

    if( status != AWS_IOT_MQTT_SUCCESS )
    {
        return status;
    }

    /* Check the PUBLISH operation data and set the remaining members. */
    AwsIotMqtt_Assert( pPublishOperation->status == AWS_IOT_MQTT_STATUS_PENDING );
    pPublishOperation->operation = AWS_IOT_MQTT_PUBLISH_TO_SERVER;
    pPublishOperation->pMqttConnection = pMqttConnection;

    /* Generate a PUBLISH packet from pPublishInfo. */
    status = serializePublish( pPublishInfo,
                               &( pPublishOperation->pMqttPacket ),
                               &( pPublishOperation->packetSize ),
                               &( pPublishOperation->packetIdentifier ) );

    if( status != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotMqttInternal_DestroyOperation( pPublishOperation );

        return status;
    }

    /* Check the serialized MQTT packet. */
    AwsIotMqtt_Assert( pPublishOperation->pMqttPacket != NULL );
    AwsIotMqtt_Assert( pPublishOperation->packetSize > 0 );

    if( pPublishInfo->QoS == 0 )
    {
        AwsIotMqtt_Assert( pPublishOperation->packetIdentifier == 0 );
    }
    else
    {
        AwsIotMqtt_Assert( pPublishOperation->packetIdentifier != 0 );
    }

    /* Initialize PUBLISH retry for QoS 1 PUBLISH if retryLimit is set. */
    if( ( pPublishInfo->QoS > 0 ) && ( pPublishInfo->retryLimit > 0 ) )
    {
        /* Allocate a timer event to handle retries. */
        pPublishOperation->pPublishRetry = AwsIotMqtt_MallocTimerEvent( sizeof( _mqttTimerEvent_t ) );

        if( pPublishOperation->pPublishRetry == NULL )
        {
            AwsIotMqttInternal_DestroyOperation( pPublishOperation );

            return AWS_IOT_MQTT_NO_MEMORY;
        }

        /* Set the members of the retry timer event. */
        ( void ) ( memset( pPublishOperation->pPublishRetry, 0x00, sizeof( _mqttTimerEvent_t ) ) );
        pPublishOperation->pPublishRetry->pOperation = pPublishOperation;
        pPublishOperation->pPublishRetry->retry.limit = pPublishInfo->retryLimit;
        pPublishOperation->pPublishRetry->retry.nextPeriod = pPublishInfo->retryMs;
    }

    /* Set the reference, if provided. This should be set before the publish
     * is pushed to the send queue to avoid a race condition. */
    if( ( pPublishInfo->QoS > 0 ) && ( pPublishRef != NULL ) )
    {
        *pPublishRef = pPublishOperation;
    }

    /* Add the PUBLISH operation to the send queue for network transmission. */
    if( AwsIotMqttInternal_EnqueueOperation( pPublishOperation,
                                             &( _IotMqttSend ) ) != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogError( "Failed to enqueue PUBLISH for sending." );

        AwsIotMqttInternal_DestroyOperation( pPublishOperation );

        /* Clear the previously set (and now invalid) reference. */
        if( ( pPublishInfo->QoS > 0 ) && ( pPublishRef != NULL ) )
        {
            *pPublishRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
        }

        return AWS_IOT_MQTT_NO_MEMORY;
    }

    /* A QoS 0 PUBLISH is considered successful as soon as it is added to the
     * send queue. */
    if( pPublishInfo->QoS == 0 )
    {
        return AWS_IOT_MQTT_SUCCESS;
    }

    AwsIotLogInfo( "MQTT PUBLISH operation queued." );

    /* QoS 1 and QoS 2 PUBLISH messages are awaiting responses. */
    return AWS_IOT_MQTT_STATUS_PENDING;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_TimedPublish( AwsIotMqttConnection_t mqttConnection,
                                           const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                           uint32_t flags,
                                           uint64_t timeoutMs )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttReference_t publishRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER,
                          * pPublishRef = NULL;

    /* Clear the flags. */
    flags = 0;

    /* Set the waitable flag and reference for QoS 1 PUBLISH. */
    if( pPublishInfo->QoS > 0 )
    {
        flags = AWS_IOT_MQTT_FLAG_WAITABLE;
        pPublishRef = &publishRef;
    }

    /* Call the asynchronous PUBLISH function. */
    status = AwsIotMqtt_Publish( mqttConnection,
                                 pPublishInfo,
                                 flags,
                                 NULL,
                                 pPublishRef );

    /* Wait for a queued QoS 1 PUBLISH to complete. */
    if( ( pPublishInfo->QoS > 0 ) && ( status == AWS_IOT_MQTT_STATUS_PENDING ) )
    {
        status = AwsIotMqtt_Wait( publishRef, timeoutMs );
    }

    return status;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqtt_Wait( AwsIotMqttReference_t reference,
                                   uint64_t timeoutMs )
{
    bool publishRetryActive = false;
    uint64_t startTime = 0, currentTime = 0, elapsedTime = 0, remainingMs = timeoutMs;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) reference;

    /* Check reference. */
    if( AwsIotMqttInternal_ValidateReference( reference ) == false )
    {
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    AwsIotLogInfo( "Waiting for operation %s to complete.",
                   AwsIotMqtt_OperationType( pOperation->operation ) );

    /* Wait for the operation to be sent once. This wait should return quickly. */
    AwsIotSemaphore_Wait( &( pOperation->notify.waitSemaphore ) );

    /* Check any status set by the send thread. Block the receive callback
     * during this check by locking the mutex for operations pending responses. */
    AwsIotMutex_Lock( &( _IotMqttPendingResponseMutex ) );
    status = pOperation->status;
    AwsIotMutex_Unlock( &( _IotMqttPendingResponseMutex ) );

    if( status == AWS_IOT_MQTT_STATUS_PENDING )
    {
        /* Check if this operation is a PUBLISH with retry. Block the timer
         * thread during this check by locking the connection mutex. */
        if( pOperation->operation == AWS_IOT_MQTT_PUBLISH_TO_SERVER )
        {
            AwsIotMutex_Lock( &( pOperation->pMqttConnection->timerMutex ) );
            publishRetryActive = ( pOperation->pPublishRetry != NULL );
            AwsIotMutex_Unlock( &( pOperation->pMqttConnection->timerMutex ) );
        }

        /* Wait for a response to be received from the network. Record when
         * the wait begins for a PUBLISH with retry. */
        if( publishRetryActive == true )
        {
            startTime = AwsIotClock_GetTimeMs();
            AwsIotMqtt_Assert( startTime > 0 );
        }

        /* All MQTT operations except PUBLISH with retry will have a status after
         * the second block on the wait semaphore. PUBLISH with retry may require
         * multiple blocks (once more per each retransmission). */
        while( status == AWS_IOT_MQTT_STATUS_PENDING )
        {
            /* Only perform the remaining time calculation for PUBLISH with retry. */
            if( publishRetryActive == true )
            {
                /* Get current time. */
                currentTime = AwsIotClock_GetTimeMs();
                AwsIotMqtt_Assert( currentTime >= startTime );

                /* Calculate elapsed time. */
                elapsedTime = currentTime - startTime;

                /* Check for timeout with elapsed time. */
                if( elapsedTime > timeoutMs )
                {
                    status = AWS_IOT_MQTT_TIMEOUT;
                    break;
                }

                /* Calculate the remaining wait time. */
                remainingMs = timeoutMs - elapsedTime;
            }

            /* Block on the wait semaphore. */
            if( AwsIotSemaphore_TimedWait( &( pOperation->notify.waitSemaphore ),
                                           remainingMs ) == false )
            {
                /* No status received before timeout. */
                status = AWS_IOT_MQTT_TIMEOUT;

                /* A timed out operation may still pending a network response. */
                ( void ) IotListDouble_RemoveFirstMatch( &( _IotMqttPendingResponse ),
                                                         NULL,
                                                         NULL,
                                                         pOperation );
            }
            else
            {
                /* For a PUBLISH with retry, block the timer thread before reading a
                 * status. */
                if( publishRetryActive == true )
                {
                    AwsIotMutex_Lock( &( pOperation->pMqttConnection->timerMutex ) );
                }

                /* Successfully received a notification of completion. Retrieve the
                 * status. */
                status = pOperation->status;

                if( publishRetryActive == true )
                {
                    AwsIotMutex_Unlock( &( pOperation->pMqttConnection->timerMutex ) );
                }
            }
        }
    }
    else
    {
        /* If a status was set by the send thread, wait for the send thread to be
         * completely done with the operation. */
        AwsIotSemaphore_Wait( &( pOperation->notify.waitSemaphore ) );
    }

    /* A completed operation should not be linked. */
    AwsIotMqtt_Assert( IotLink_IsLinked( &( pOperation->link ) ) == false );

    /* Remove any lingering subscriptions from a timed out SUBSCRIBE. If
     * a SUBSCRIBE fails for any other reason, its subscription have already
     * been removed. */
    if( ( pOperation->operation == AWS_IOT_MQTT_SUBSCRIBE ) &&
        ( status == AWS_IOT_MQTT_TIMEOUT ) )
    {
        AwsIotMqttInternal_RemoveSubscriptionByPacket( pOperation->pMqttConnection,
                                                       pOperation->packetIdentifier,
                                                       -1 );
    }

    AwsIotLogInfo( "MQTT operation %s complete with result %s.",
                   AwsIotMqtt_OperationType( pOperation->operation ),
                   AwsIotMqtt_strerror( status ) );

    /* The operation is complete; it can be destroyed. PINGREQ operations are
     * destroyed by AwsIotMqtt_Disconnect and not here. If the operation is a
     * PINGRESP, reset it. */
    if( pOperation->operation != AWS_IOT_MQTT_PINGREQ )
    {
        AwsIotMqttInternal_DestroyOperation( pOperation );
    }
    else
    {
        AwsIotMqtt_Assert( AwsIotSemaphore_GetCount( &( pOperation->notify.waitSemaphore ) ) == 0 );
        pOperation->status = AWS_IOT_MQTT_STATUS_PENDING;
    }

    /* A complete operation status (not pending) should be set. */
    AwsIotMqtt_Assert( status != AWS_IOT_MQTT_STATUS_PENDING );

    return status;
}

/*-----------------------------------------------------------*/

const char * AwsIotMqtt_strerror( AwsIotMqttError_t status )
{
    switch( status )
    {
        case AWS_IOT_MQTT_SUCCESS:

            return "SUCCESS";

        case AWS_IOT_MQTT_STATUS_PENDING:

            return "PENDING";

        case AWS_IOT_MQTT_INIT_FAILED:

            return "INITIALIZATION FAILED";

        case AWS_IOT_MQTT_BAD_PARAMETER:

            return "BAD PARAMETER";

        case AWS_IOT_MQTT_NO_MEMORY:

            return "NO MEMORY";

        case AWS_IOT_MQTT_SEND_ERROR:

            return "NETWORK SEND ERROR";

        case AWS_IOT_MQTT_BAD_RESPONSE:

            return "BAD RESPONSE RECEIVED";

        case AWS_IOT_MQTT_TIMEOUT:

            return "TIMEOUT";

        case AWS_IOT_MQTT_SERVER_REFUSED:

            return "SERVER REFUSED";

        case AWS_IOT_MQTT_RETRY_NO_RESPONSE:

            return "NO RESPONSE";

        default:

            return "INVALID STATUS";
    }
}

/*-----------------------------------------------------------*/

const char * AwsIotMqtt_OperationType( AwsIotMqttOperationType_t operation )
{
    switch( operation )
    {
        case AWS_IOT_MQTT_CONNECT:

            return "CONNECT";

        case AWS_IOT_MQTT_PUBLISH_TO_SERVER:

            return "PUBLISH";

        case AWS_IOT_MQTT_PUBACK:

            return "PUBACK";

        case AWS_IOT_MQTT_SUBSCRIBE:

            return "SUBSCRIBE";

        case AWS_IOT_MQTT_UNSUBSCRIBE:

            return "UNSUBSCRIBE";

        case AWS_IOT_MQTT_PINGREQ:

            return "PINGREQ";

        case AWS_IOT_MQTT_DISCONNECT:

            return "DISCONNECT";

        default:

            return "INVALID OPERATION";
    }
}

/*-----------------------------------------------------------*/

/* If the MQTT library is being tested, include a file that allows access to
 * internal functions and variables. */
#if AWS_IOT_MQTT_TEST == 1
    #include "aws_iot_test_access_mqtt_api.c"
#endif
