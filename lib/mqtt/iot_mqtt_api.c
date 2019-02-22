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
 * @file iot_mqtt_api.c
 * @brief Implements most user-facing functions of the MQTT library.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Validate MQTT configuration settings. */
#if IOT_MQTT_ENABLE_ASSERTS != 0 && IOT_MQTT_ENABLE_ASSERTS != 1
    #error "IOT_MQTT_ENABLE_ASSERTS must be 0 or 1."
#endif
#if IOT_MQTT_ENABLE_METRICS != 0 && IOT_MQTT_ENABLE_METRICS != 1
    #error "IOT_MQTT_ENABLE_METRICS must be 0 or 1."
#endif
#if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES != 0 && IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES != 1
    #error "IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES must be 0 or 1."
#endif
#if IOT_MQTT_TEST != 0 && IOT_MQTT_TEST != 1
    #error "IOT_MQTT_MQTT_TEST must be 0 or 1."
#endif
#if IOT_MQTT_RESPONSE_WAIT_MS <= 0
    #error "IOT_MQTT_RESPONSE_WAIT_MS cannot be 0 or negative."
#endif
#if IOT_MQTT_RETRY_MS_CEILING <= 0
    #error "IOT_MQTT_RETRY_MS_CEILING cannot be 0 or negative."
#endif

/*-----------------------------------------------------------*/

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
 * @brief Decrement the reference count of an MQTT operation and attempt to
 * destroy it.
 *
 * @param[in] pData The operation data to destroy. This parameter is of type
 * `void*` for compatibility with [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
static void _mqttOperation_tryDestroy( void * pData );

/**
 * @brief Create a keep-alive job for an MQTT connection.
 *
 * @param[in] awsIotMqttMode Specifies if this connection is to an AWS IoT MQTT
 * server.
 * @param[in] pMqttConnection The MQTT connection associated with the keep-alive.
 * @param[in] keepAliveSeconds User-provided keep-alive interval.
 *
 * @return `true` if the keep-alive job was successfully created; `false` otherwise.
 */
static bool _createKeepAliveJob( bool awsIotMqttMode,
                                 _mqttConnection_t * pMqttConnection,
                                 uint16_t keepAliveSeconds );

/**
 * @brief Creates a new MQTT connection and initializes its members.
 *
 * @param[in] awsIotMqttMode Specifies if this connection is to an AWS IoT MQTT server.
 * @param[in] pNetworkInterface User-provided network interface for the new
 * connection.
 * @param[in] keepAliveSeconds User-provided keep-alive interval for the new connection.
 *
 * @return Pointer to a newly-allocated MQTT connection on success; `NULL` on
 * failure.
 */
static _mqttConnection_t * _createMqttConnection( bool awsIotMqttMode,
                                                  const IotMqttNetIf_t * pNetworkInterface,
                                                  uint16_t keepAliveSeconds );

/**
 * @brief Destroys the members of an MQTT connection.
 *
 * @param[in] pMqttConnection Which connection to destroy.
 */
static void _destroyMqttConnection( _mqttConnection_t * pMqttConnection );

/**
 * @brief The common component of both @ref mqtt_function_subscribe and @ref
 * mqtt_function_unsubscribe.
 *
 * See @ref mqtt_function_subscribe or @ref mqtt_function_unsubscribe for a
 * description of the parameters and return values.
 */
static IotMqttError_t _subscriptionCommon( IotMqttOperationType_t operation,
                                           IotMqttConnection_t mqttConnection,
                                           const IotMqttSubscription_t * pSubscriptionList,
                                           size_t subscriptionCount,
                                           uint32_t flags,
                                           const IotMqttCallbackInfo_t * pCallbackInfo,
                                           IotMqttReference_t * pSubscriptionRef );

/*-----------------------------------------------------------*/

/**
 * @brief Ensures that only one CONNECT operation is in-progress at any time.
 *
 * Because CONNACK contains no data about which CONNECT packet it acknowledges,
 * only one CONNECT operation may be in-progress at any time.
 */
static IotMutex_t _connectMutex;

/*-----------------------------------------------------------*/

static bool _mqttSubscription_shouldRemove( const IotLink_t * pSubscriptionLink,
                                            void * pMatch )
{
    bool match = false;

    /* Because this function is called from a container function, the given link
     * must never be NULL. */
    IotMqtt_Assert( pSubscriptionLink != NULL );

    _mqttSubscription_t * pSubscription = IotLink_Container( _mqttSubscription_t,
                                                             pSubscriptionLink,
                                                             link );

    /* Silence warnings about unused parameters. */
    ( void ) pMatch;

    /* Reference count must not be negative. */
    IotMqtt_Assert( pSubscription->references >= 0 );

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

static void _mqttOperation_tryDestroy( void * pData )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pData;

    /* Decrement reference count and destroy operation if possible. */
    if( _IotMqtt_DecrementOperationReferences( pOperation, true ) == true )
    {
        _IotMqtt_DestroyOperation( pOperation );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }
}

/*-----------------------------------------------------------*/

static bool _createKeepAliveJob( bool awsIotMqttMode,
                                 _mqttConnection_t * pMqttConnection,
                                 uint16_t keepAliveSeconds )
{
    bool status = true;
    IotMqttError_t serializeStatus = IOT_MQTT_SUCCESS;
    IotTaskPoolError_t jobStatus = IOT_TASKPOOL_SUCCESS;

    /* Default PINGREQ serializer function. */
    IotMqttError_t ( * serializePingreq )( uint8_t **,
                                           size_t * ) = _IotMqtt_SerializePingreq;

    /* AWS IoT service limits set minimum and maximum values for keep-alive interval.
     * Adjust the user-provided keep-alive interval based on these requirements. */
    if( awsIotMqttMode == true )
    {
        if( keepAliveSeconds < _AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE )
        {
            keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE;
        }
        else if( keepAliveSeconds > _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE )
        {
            keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE;
        }
        else if( keepAliveSeconds == 0 )
        {
            keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Convert the keep-alive interval to milliseconds. */
    pMqttConnection->keepAliveMs = keepAliveSeconds * 1000;
    pMqttConnection->nextKeepAliveMs = pMqttConnection->keepAliveMs;

    /* Choose a PINGREQ serializer function. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pMqttConnection->network.serialize.pingreq != NULL )
        {
            serializePingreq = pMqttConnection->network.serialize.pingreq;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    #endif

    /* Generate a PINGREQ packet. */
    serializeStatus = serializePingreq( &( pMqttConnection->pPingreqPacket ),
                                        &( pMqttConnection->pingreqPacketSize ) );

    if( serializeStatus != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to allocate PINGREQ packet for new connection." );

        status = false;
    }
    else
    {
        /* Create the task pool job that processes keep-alive. */
        jobStatus = IotTaskPool_CreateJob( _IotMqtt_ProcessKeepAlive,
                                           pMqttConnection,
                                           &( pMqttConnection->keepAliveJob ) );

        /* Task pool job creation for a pre-allocated job should never fail.
         * Abort the program if it does. */
        if( jobStatus != IOT_TASKPOOL_SUCCESS )
        {
            IotLogError( "Failed to create keep-alive job for new connection." );

            IotMqtt_Assert( false );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        /* Keep-alive references its MQTT connection, so increment reference. */
        ( pMqttConnection->references )++;
    }

    return status;
}

/*-----------------------------------------------------------*/

static _mqttConnection_t * _createMqttConnection( bool awsIotMqttMode,
                                                  const IotMqttNetIf_t * pNetworkInterface,
                                                  uint16_t keepAliveSeconds )
{
    _IOT_FUNCTION_ENTRY( bool, true );
    bool referencesMutexCreated = false, subscriptionMutexCreated = false;
    _mqttConnection_t * pNewMqttConnection = NULL;

    /* Allocate memory to store data for the new MQTT connection. */
    pNewMqttConnection = ( _mqttConnection_t * )
                         IotMqtt_MallocConnection( sizeof( _mqttConnection_t ) );

    if( pNewMqttConnection == NULL )
    {
        IotLogError( "Failed to allocate memory for new MQTT connection." );

        _IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Clear the MQTT connection, then copy the MQTT server mode and network
     * interface. */
    ( void ) memset( pNewMqttConnection, 0x00, sizeof( _mqttConnection_t ) );
    pNewMqttConnection->awsIotMqttMode = awsIotMqttMode;
    pNewMqttConnection->network = *pNetworkInterface;

    /* Start a new MQTT connection with a reference count of 1. */
    pNewMqttConnection->references = 1;

    /* Create the references mutex for a new connection. It is a recursive mutex. */
    referencesMutexCreated = IotMutex_Create( &( pNewMqttConnection->referencesMutex ), true );

    if( referencesMutexCreated == false )
    {
        IotLogError( "Failed to create references mutex for new connection." );

        _IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Create the subscription mutex for a new connection. */
    subscriptionMutexCreated = IotMutex_Create( &( pNewMqttConnection->subscriptionMutex ), false );

    if( subscriptionMutexCreated == false )
    {
        IotLogError( "Failed to create subscription mutex for new connection." );

        _IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Create the new connection's subscription and operation lists. */
    IotListDouble_Create( &( pNewMqttConnection->subscriptionList ) );
    IotListDouble_Create( &( pNewMqttConnection->pendingProcessing ) );
    IotListDouble_Create( &( pNewMqttConnection->pendingResponse ) );

    /* Check if keep-alive is active for this connection. */
    if( keepAliveSeconds != 0 )
    {
        if( _createKeepAliveJob( awsIotMqttMode,
                                 pNewMqttConnection,
                                 keepAliveSeconds ) == false )
        {
            _IOT_SET_AND_GOTO_CLEANUP( false );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Clean up mutexes and connection if this function failed. */
    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status == false )
    {
        if( subscriptionMutexCreated == true )
        {
            IotMutex_Destroy( &( pNewMqttConnection->subscriptionMutex ) );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( referencesMutexCreated == true )
        {
            IotMutex_Destroy( &( pNewMqttConnection->referencesMutex ) );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( pNewMqttConnection != NULL )
        {
            IotMqtt_FreeConnection( pNewMqttConnection );

            pNewMqttConnection = NULL;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    return pNewMqttConnection;
}

/*-----------------------------------------------------------*/

static void _destroyMqttConnection( _mqttConnection_t * pMqttConnection )
{
    /* Clean up keep-alive if still allocated. */
    if( pMqttConnection->keepAliveMs != 0 )
    {
        IotLogDebug( "(MQTT connection %p) Cleaning up keep-alive.", pMqttConnection );

        _IotMqtt_FreePacket( pMqttConnection->pPingreqPacket );
        IotTaskPool_DestroyJob( &( _IotMqttTaskPool ),
                                &( pMqttConnection->keepAliveJob ) );

        /* Clear data about the keep-alive. */
        pMqttConnection->keepAliveMs = 0;
        pMqttConnection->pPingreqPacket = NULL;
        pMqttConnection->pingreqPacketSize = 0;

        /* Decrement reference count. */
        pMqttConnection->references--;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* A connection to be destroyed should have no keep-alive and at most 1
     * reference. */
    IotMqtt_Assert( pMqttConnection->references <= 1 );
    IotMqtt_Assert( pMqttConnection->keepAliveMs == 0 );
    IotMqtt_Assert( pMqttConnection->pPingreqPacket == NULL );
    IotMqtt_Assert( pMqttConnection->pingreqPacketSize == 0 );

    /* Remove all subscriptions. */
    IotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );
    IotListDouble_RemoveAllMatches( &( pMqttConnection->subscriptionList ),
                                    _mqttSubscription_shouldRemove,
                                    NULL,
                                    IotMqtt_FreeSubscription,
                                    offsetof( _mqttSubscription_t, link ) );
    IotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );

    /* Destroy mutexes and free connection. */
    IotMutex_Destroy( &( pMqttConnection->referencesMutex ) );
    IotMutex_Destroy( &( pMqttConnection->subscriptionMutex ) );
    IotMqtt_FreeConnection( pMqttConnection );

    IotLogDebug( "(MQTT connection %p) Connection destroyed.", pMqttConnection );
}

/*-----------------------------------------------------------*/

static IotMqttError_t _subscriptionCommon( IotMqttOperationType_t operation,
                                           IotMqttConnection_t mqttConnection,
                                           const IotMqttSubscription_t * pSubscriptionList,
                                           size_t subscriptionCount,
                                           uint32_t flags,
                                           const IotMqttCallbackInfo_t * pCallbackInfo,
                                           IotMqttReference_t * pSubscriptionRef )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    _mqttOperation_t * pSubscriptionOperation = NULL;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;

    /* Subscription serializer function. */
    IotMqttError_t ( * serializeSubscription )( const IotMqttSubscription_t *,
                                                size_t,
                                                uint8_t **,
                                                size_t *,
                                                uint16_t * ) = NULL;

    /* This function should only be called for subscribe or unsubscribe. */
    IotMqtt_Assert( ( operation == IOT_MQTT_SUBSCRIBE ) ||
                    ( operation == IOT_MQTT_UNSUBSCRIBE ) );

    /* Check that all elements in the subscription list are valid. */
    if( _IotMqtt_ValidateSubscriptionList( operation,
                                           pMqttConnection->awsIotMqttMode,
                                           pSubscriptionList,
                                           subscriptionCount ) == false )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check that a reference pointer is provided for a waitable operation. */
    if( ( flags & IOT_MQTT_FLAG_WAITABLE ) == IOT_MQTT_FLAG_WAITABLE )
    {
        if( pSubscriptionRef == NULL )
        {
            IotLogError( "Reference must be provided for a waitable %s.",
                         IotMqtt_OperationType( operation ) );

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Choose a subscription serialize function. */
    if( operation == IOT_MQTT_SUBSCRIBE )
    {
        serializeSubscription = _IotMqtt_SerializeSubscribe;

        #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pMqttConnection->network.serialize.subscribe != NULL )
            {
                serializeSubscription = pMqttConnection->network.serialize.subscribe;
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        #endif
    }
    else
    {
        serializeSubscription = _IotMqtt_SerializeUnsubscribe;

        #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pMqttConnection->network.serialize.unsubscribe != NULL )
            {
                serializeSubscription = pMqttConnection->network.serialize.unsubscribe;
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        #endif
    }

    /* Remove the MQTT subscription list for an UNSUBSCRIBE. */
    if( operation == IOT_MQTT_UNSUBSCRIBE )
    {
        _IotMqtt_RemoveSubscriptionByTopicFilter( pMqttConnection,
                                                  pSubscriptionList,
                                                  subscriptionCount );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Create a subscription operation. */
    status = _IotMqtt_CreateOperation( pMqttConnection,
                                       flags,
                                       pCallbackInfo,
                                       &pSubscriptionOperation );

    if( status != IOT_MQTT_SUCCESS )
    {
        _IOT_GOTO_CLEANUP();
    }

    /* Check the subscription operation data and set the operation type. */
    IotMqtt_Assert( pSubscriptionOperation->status == IOT_MQTT_STATUS_PENDING );
    IotMqtt_Assert( pSubscriptionOperation->retry.limit == 0 );
    pSubscriptionOperation->operation = operation;

    /* Generate a subscription packet from the subscription list. */
    status = serializeSubscription( pSubscriptionList,
                                    subscriptionCount,
                                    &( pSubscriptionOperation->pMqttPacket ),
                                    &( pSubscriptionOperation->packetSize ),
                                    &( pSubscriptionOperation->packetIdentifier ) );

    if( status != IOT_MQTT_SUCCESS )
    {
        _IOT_GOTO_CLEANUP();
    }

    /* Check the serialized MQTT packet. */
    IotMqtt_Assert( pSubscriptionOperation->pMqttPacket != NULL );
    IotMqtt_Assert( pSubscriptionOperation->packetSize > 0 );

    /* Add the subscription list for a SUBSCRIBE. */
    if( operation == IOT_MQTT_SUBSCRIBE )
    {
        status = _IotMqtt_AddSubscriptions( pMqttConnection,
                                            pSubscriptionOperation->packetIdentifier,
                                            pSubscriptionList,
                                            subscriptionCount );

        if( status != IOT_MQTT_SUCCESS )
        {
            _IOT_GOTO_CLEANUP();
        }
    }

    /* Set the reference, if provided. */
    if( pSubscriptionRef != NULL )
    {
        *pSubscriptionRef = pSubscriptionOperation;
    }

    /* Schedule the subscription operation for network transmission. */
    status = _IotMqtt_ScheduleOperation( pSubscriptionOperation,
                                         _IotMqtt_ProcessSend,
                                         0 );

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "(MQTT connection %p) Failed to schedule %s for sending.",
                     pMqttConnection,
                     IotMqtt_OperationType( operation ) );

        if( operation == IOT_MQTT_SUBSCRIBE )
        {
            _IotMqtt_RemoveSubscriptionByPacket( pMqttConnection,
                                                 pSubscriptionOperation->packetIdentifier,
                                                 -1 );
        }

        /* Clear the previously set (and now invalid) reference. */
        if( pSubscriptionRef != NULL )
        {
            *pSubscriptionRef = IOT_MQTT_REFERENCE_INITIALIZER;
        }

        _IOT_GOTO_CLEANUP();
    }

    /* Clean up if this function failed. */
    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_MQTT_SUCCESS )
    {
        if( pSubscriptionOperation != NULL )
        {
            _IotMqtt_DestroyOperation( pSubscriptionOperation );
        }
    }
    else
    {
        status = IOT_MQTT_STATUS_PENDING;

        IotLogInfo( "(MQTT connection %p) %s operation scheduled.",
                    pMqttConnection,
                    IotMqtt_OperationType( operation ) );
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

bool _IotMqtt_IncrementConnectionReferences( _mqttConnection_t * pMqttConnection )
{
    bool disconnected = false;

    /* Lock the mutex protecting the reference count. */
    IotMutex_Lock( &( pMqttConnection->referencesMutex ) );

    /* Reference count must not be negative. */
    IotMqtt_Assert( pMqttConnection->references >= 0 );

    /* Read connection status. */
    disconnected = pMqttConnection->disconnected;

    /* Increment the connection's reference count if it is not disconnected. */
    if( disconnected == false )
    {
        ( pMqttConnection->references )++;
        IotLogDebug( "(MQTT connection %p) Reference count changed from %ld to %ld.",
                     pMqttConnection,
                     ( long int ) pMqttConnection->references - 1,
                     ( long int ) pMqttConnection->references );
    }
    else
    {
        IotLogWarn( "(MQTT connection %p) Attempt to use closed connection.", pMqttConnection );
    }

    IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

    return( disconnected == false );
}

/*-----------------------------------------------------------*/

void _IotMqtt_DecrementConnectionReferences( _mqttConnection_t * pMqttConnection )
{
    bool destroyConnection = false;

    /* Lock the mutex protecting the reference count. */
    IotMutex_Lock( &( pMqttConnection->referencesMutex ) );

    /* Decrement reference count. It must not be negative. */
    ( pMqttConnection->references )--;
    IotMqtt_Assert( pMqttConnection->references >= 0 );

    IotLogDebug( "(MQTT connection %p) Reference count changed from %ld to %ld.",
                 pMqttConnection,
                 ( long int ) pMqttConnection->references + 1,
                 ( long int ) pMqttConnection->references );

    /* Check if this connection may be destroyed. */
    if( pMqttConnection->references == 0 )
    {
        destroyConnection = true;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

    /* Destroy an unreferenced MQTT connection. */
    if( destroyConnection == true )
    {
        IotLogDebug( "(MQTT connection %p) Connection will be destroyed now.",
                     pMqttConnection );
        _destroyMqttConnection( pMqttConnection );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Init( void )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    bool taskPoolCreated = false, connectMutexCreated = false;
    const IotTaskPoolInfo_t taskPoolInfo = IOT_TASKPOOL_INFO_INITIALIZER_MEDIUM;

    /* Create MQTT library task pool. */
    if( IotTaskPool_Create( &taskPoolInfo, &_IotMqttTaskPool ) != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( "Failed to initialize MQTT library task pool." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_INIT_FAILED );
    }
    else
    {
        taskPoolCreated = true;
    }

    /* Create CONNECT mutex. */
    connectMutexCreated = IotMutex_Create( &( _connectMutex ), false );

    if( connectMutexCreated == false )
    {
        IotLogError( "Failed to initialize MQTT library connect mutex." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_INIT_FAILED );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Initialize MQTT serializer. */
    if( _IotMqtt_InitSerialize() != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to initialize MQTT library serializer. " );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_INIT_FAILED );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    IotLogInfo( "MQTT library successfully initialized." );

    /* Clean up if this function failed. */
    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_MQTT_SUCCESS )
    {
        if( taskPoolCreated == true )
        {
            IotTaskPool_Destroy( &( _IotMqttTaskPool ) );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( connectMutexCreated == true )
        {
            IotMutex_Destroy( &( _connectMutex ) );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

void IotMqtt_Cleanup()
{
    /* Clean up MQTT library task pool. */
    IotTaskPool_Destroy( &_IotMqttTaskPool );

    /* Clean up MQTT serializer. */
    _IotMqtt_CleanupSerialize();

    /* Clean up CONNECT mutex. */
    IotMutex_Destroy( &( _connectMutex ) );

    IotLogInfo( "MQTT library cleanup done." );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Connect( IotMqttConnection_t * pMqttConnection,
                                const IotMqttNetIf_t * pNetworkInterface,
                                const IotMqttConnectInfo_t * pConnectInfo,
                                uint64_t timeoutMs )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    IotTaskPoolError_t taskPoolStatus = IOT_TASKPOOL_SUCCESS;
    _mqttConnection_t * pNewMqttConnection = NULL;
    _mqttOperation_t * pConnectOperation = NULL;

    /* Default CONNECT serializer function. */
    IotMqttError_t ( * serializeConnect )( const IotMqttConnectInfo_t *,
                                           uint8_t **,
                                           size_t * ) = _IotMqtt_SerializeConnect;

    /* Validate network interface and connect info. */
    if( _IotMqtt_ValidateNetIf( pNetworkInterface ) == false )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else if( _IotMqtt_ValidateConnect( pConnectInfo ) == false )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* If will info is provided, check that it is valid. */
    if( pConnectInfo->pWillInfo != NULL )
    {
        if( _IotMqtt_ValidatePublish( pConnectInfo->awsIotMqttMode,
                                      pConnectInfo->pWillInfo ) == false )
        {
            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else if( pConnectInfo->pWillInfo->payloadLength > UINT16_MAX )
        {
            /* Will message payloads cannot be larger than 65535. This restriction
             * applies only to will messages, and not normal PUBLISH messages. */
            IotLogError( "Will payload cannot be larger than 65535." );

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* If previous subscriptions are provided, check that they are valid. */
    if( pConnectInfo->cleanSession == false )
    {
        if( pConnectInfo->pPreviousSubscriptions != NULL )
        {
            if( _IotMqtt_ValidateSubscriptionList( IOT_MQTT_SUBSCRIBE,
                                                   pConnectInfo->awsIotMqttMode,
                                                   pConnectInfo->pPreviousSubscriptions,
                                                   pConnectInfo->previousSubscriptionCount ) == false )
            {
                _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Choose a CONNECT serializer function. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pNetworkInterface->serialize.connect != NULL )
        {
            serializeConnect = pNetworkInterface->serialize.connect;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    #endif

    IotLogInfo( "Establishing new MQTT connection." );

    /* Allocate memory to store data for the new MQTT connection. */
    pNewMqttConnection = _createMqttConnection( pConnectInfo->awsIotMqttMode,
                                                pNetworkInterface,
                                                pConnectInfo->keepAliveSeconds );

    if( pNewMqttConnection == NULL )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Create a CONNECT operation. */
    status = _IotMqtt_CreateOperation( pNewMqttConnection,
                                       IOT_MQTT_FLAG_WAITABLE,
                                       NULL,
                                       &pConnectOperation );

    if( status != IOT_MQTT_SUCCESS )
    {
        _IOT_GOTO_CLEANUP();
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Ensure the members set by operation creation and serialization
     * are appropriate for a blocking CONNECT. */
    IotMqtt_Assert( pConnectOperation->status == IOT_MQTT_STATUS_PENDING );
    IotMqtt_Assert( ( pConnectOperation->flags & IOT_MQTT_FLAG_WAITABLE )
                    == IOT_MQTT_FLAG_WAITABLE );
    IotMqtt_Assert( pConnectOperation->retry.limit == 0 );

    /* Set the operation type. */
    pConnectOperation->operation = IOT_MQTT_CONNECT;

    /* Add previous session subscriptions. */
    if( pConnectInfo->pPreviousSubscriptions != NULL )
    {
        /* Previous subscription count should have been validated as nonzero. */
        IotMqtt_Assert( pConnectInfo->previousSubscriptionCount > 0 );

        status = _IotMqtt_AddSubscriptions( pNewMqttConnection,
                                            2,
                                            pConnectInfo->pPreviousSubscriptions,
                                            pConnectInfo->previousSubscriptionCount );

        if( status != IOT_MQTT_SUCCESS )
        {
            _IOT_GOTO_CLEANUP();
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Convert the connect info and will info objects to an MQTT CONNECT packet. */
    status = serializeConnect( pConnectInfo,
                               &( pConnectOperation->pMqttPacket ),
                               &( pConnectOperation->packetSize ) );

    if( status != IOT_MQTT_SUCCESS )
    {
        _IOT_GOTO_CLEANUP();
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the serialized MQTT packet. */
    IotMqtt_Assert( pConnectOperation->pMqttPacket != NULL );
    IotMqtt_Assert( pConnectOperation->packetSize > 0 );

    /* Set the output parameter so it may be used by the network receive callback. */
    *pMqttConnection = pNewMqttConnection;

    /* Prevent another CONNECT operation from using the network. */
    IotMutex_Lock( &_connectMutex );

    /* Add the CONNECT operation to the send queue for network transmission. */
    status = _IotMqtt_ScheduleOperation( pConnectOperation,
                                         _IotMqtt_ProcessSend,
                                         0 );

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to enqueue CONNECT for sending." );
    }
    else
    {
        /* Wait for the CONNECT operation to complete, i.e. wait for CONNACK. */
        status = IotMqtt_Wait( ( IotMqttReference_t ) pConnectOperation,
                               timeoutMs );

        /* The call to wait cleans up the CONNECT operation, so set the pointer
         * to NULL. */
        pConnectOperation = NULL;
    }

    /* Unlock the CONNECT mutex. */
    IotMutex_Unlock( &_connectMutex );

    /* When a connection is successfully established, schedule keep-alive job. */
    if( status == IOT_MQTT_SUCCESS )
    {
        /* Check if a keep-alive job should be scheduled. */
        if( pNewMqttConnection->keepAliveMs != 0 )
        {
            IotLogDebug( "Scheduling first MQTT keep-alive job." );

            taskPoolStatus = IotTaskPool_ScheduleDeferred( &( _IotMqttTaskPool ),
                                                           &( pNewMqttConnection->keepAliveJob ),
                                                           pNewMqttConnection->nextKeepAliveMs );

            if( taskPoolStatus != IOT_TASKPOOL_SUCCESS )
            {
                _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_SCHEDULING_ERROR );
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to establish new MQTT connection, error %s.",
                     IotMqtt_strerror( status ) );

        if( pConnectOperation != NULL )
        {
            _IotMqtt_DestroyOperation( pConnectOperation );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( pNewMqttConnection != NULL )
        {
            _destroyMqttConnection( pNewMqttConnection );
            *pMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        IotLogInfo( "New MQTT connection %p established.", pNewMqttConnection );
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

void IotMqtt_Disconnect( IotMqttConnection_t mqttConnection,
                         bool cleanupOnly )
{
    bool disconnected = false;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;
    _mqttOperation_t * pDisconnectOperation = NULL;

    IotLogInfo( "(MQTT connection %p) Disconnecting connection.", pMqttConnection );

    /* Read the connection status. */
    IotMutex_Lock( &( pMqttConnection->referencesMutex ) );
    disconnected = pMqttConnection->disconnected;
    IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

    /* Only send a DISCONNECT packet if the connection is active and the "cleanup only"
     * option is false. */
    if( disconnected == false )
    {
        if( cleanupOnly == false )
        {
            /* Create a DISCONNECT operation. This function blocks until the DISCONNECT
             * packet is sent, so it sets IOT_MQTT_FLAG_WAITABLE. */
            status = _IotMqtt_CreateOperation( pMqttConnection,
                                               IOT_MQTT_FLAG_WAITABLE,
                                               NULL,
                                               &pDisconnectOperation );

            if( status == IOT_MQTT_SUCCESS )
            {
                /* Ensure that the members set by operation creation and serialization
                 * are appropriate for a blocking DISCONNECT. */
                IotMqtt_Assert( pDisconnectOperation->status == IOT_MQTT_STATUS_PENDING );
                IotMqtt_Assert( ( pDisconnectOperation->flags & IOT_MQTT_FLAG_WAITABLE )
                                == IOT_MQTT_FLAG_WAITABLE );
                IotMqtt_Assert( pDisconnectOperation->retry.limit == 0 );

                /* Set the operation type. */
                pDisconnectOperation->operation = IOT_MQTT_DISCONNECT;

                /* Choose a disconnect serializer. */
                IotMqttError_t ( * serializeDisconnect )( uint8_t **,
                                                          size_t * ) = _IotMqtt_SerializeDisconnect;

                #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
                    if( pMqttConnection->network.serialize.disconnect != NULL )
                    {
                        serializeDisconnect = pMqttConnection->network.serialize.disconnect;
                    }
                    else
                    {
                        _EMPTY_ELSE_MARKER;
                    }
                #endif

                /* Generate a DISCONNECT packet. */
                status = serializeDisconnect( &( pDisconnectOperation->pMqttPacket ),
                                              &( pDisconnectOperation->packetSize ) );
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }

            if( status == IOT_MQTT_SUCCESS )
            {
                /* Check the serialized MQTT packet. */
                IotMqtt_Assert( pDisconnectOperation->pMqttPacket != NULL );
                IotMqtt_Assert( pDisconnectOperation->packetSize > 0 );

                /* Schedule the DISCONNECT operation for network transmission. */
                if( _IotMqtt_ScheduleOperation( pDisconnectOperation,
                                                _IotMqtt_ProcessSend,
                                                0 ) != IOT_MQTT_SUCCESS )
                {
                    IotLogWarn( "(MQTT connection %p) Failed to schedule DISCONNECT for sending.",
                                pMqttConnection );
                    _IotMqtt_DestroyOperation( pDisconnectOperation );
                }
                else
                {
                    /* Wait a short time for the DISCONNECT packet to be transmitted. */
                    status = IotMqtt_Wait( ( IotMqttReference_t ) pDisconnectOperation,
                                           IOT_MQTT_RESPONSE_WAIT_MS );

                    /* A wait on DISCONNECT should only ever return SUCCESS, TIMEOUT,
                     * or NETWORK ERROR. */
                    if( status == IOT_MQTT_SUCCESS )
                    {
                        IotLogInfo( "(MQTT connection %p) Connection disconnected.", pMqttConnection );
                    }
                    else
                    {
                        IotMqtt_Assert( ( status == IOT_MQTT_TIMEOUT ) ||
                                        ( status == IOT_MQTT_NETWORK_ERROR ) );

                        IotLogWarn( "(MQTT connection %p) DISCONNECT not sent, error %s.",
                                    pMqttConnection,
                                    IotMqtt_strerror( status ) );
                    }
                }
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Close the underlying network connection. This also cleans up keep-alive. */
    _IotMqtt_CloseNetworkConnection( pMqttConnection );

    /* Check if the connection may be destroyed. */
    IotMutex_Lock( &( pMqttConnection->referencesMutex ) );

    /* At this point, the connection should be marked disconnected. */
    IotMqtt_Assert( pMqttConnection->disconnected == true );

    /* Attempt cancel and destroy each operation in the connection's lists. */
    IotListDouble_RemoveAll( &( pMqttConnection->pendingProcessing ),
                             _mqttOperation_tryDestroy,
                             offsetof( _mqttOperation_t, link ) );

    IotListDouble_RemoveAll( &( pMqttConnection->pendingResponse ),
                             _mqttOperation_tryDestroy,
                             offsetof( _mqttOperation_t, link ) );

    IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

    /* Decrement the connection reference count and destroy it if possible. */
    _IotMqtt_DecrementConnectionReferences( pMqttConnection );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Subscribe( IotMqttConnection_t mqttConnection,
                                  const IotMqttSubscription_t * pSubscriptionList,
                                  size_t subscriptionCount,
                                  uint32_t flags,
                                  const IotMqttCallbackInfo_t * pCallbackInfo,
                                  IotMqttReference_t * pSubscribeRef )
{
    return _subscriptionCommon( IOT_MQTT_SUBSCRIBE,
                                mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                flags,
                                pCallbackInfo,
                                pSubscribeRef );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_TimedSubscribe( IotMqttConnection_t mqttConnection,
                                       const IotMqttSubscription_t * pSubscriptionList,
                                       size_t subscriptionCount,
                                       uint32_t flags,
                                       uint64_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttReference_t subscribeRef = IOT_MQTT_REFERENCE_INITIALIZER;

    /* Flags are not used, but the parameter is present for future compatibility. */
    ( void ) flags;

    /* Call the asynchronous SUBSCRIBE function. */
    status = IotMqtt_Subscribe( mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                IOT_MQTT_FLAG_WAITABLE,
                                NULL,
                                &subscribeRef );

    /* Wait for the SUBSCRIBE operation to complete. */
    if( status == IOT_MQTT_STATUS_PENDING )
    {
        status = IotMqtt_Wait( subscribeRef, timeoutMs );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Ensure that a status was set. */
    IotMqtt_Assert( status != IOT_MQTT_STATUS_PENDING );

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Unsubscribe( IotMqttConnection_t mqttConnection,
                                    const IotMqttSubscription_t * pSubscriptionList,
                                    size_t subscriptionCount,
                                    uint32_t flags,
                                    const IotMqttCallbackInfo_t * pCallbackInfo,
                                    IotMqttReference_t * pUnsubscribeRef )
{
    return _subscriptionCommon( IOT_MQTT_UNSUBSCRIBE,
                                mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                flags,
                                pCallbackInfo,
                                pUnsubscribeRef );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_TimedUnsubscribe( IotMqttConnection_t mqttConnection,
                                         const IotMqttSubscription_t * pSubscriptionList,
                                         size_t subscriptionCount,
                                         uint32_t flags,
                                         uint64_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttReference_t unsubscribeRef = IOT_MQTT_REFERENCE_INITIALIZER;

    /* Flags are not used, but the parameter is present for future compatibility. */
    ( void ) flags;

    /* Call the asynchronous UNSUBSCRIBE function. */
    status = IotMqtt_Unsubscribe( mqttConnection,
                                  pSubscriptionList,
                                  subscriptionCount,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  &unsubscribeRef );

    /* Wait for the UNSUBSCRIBE operation to complete. */
    if( status == IOT_MQTT_STATUS_PENDING )
    {
        status = IotMqtt_Wait( unsubscribeRef, timeoutMs );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Ensure that a status was set. */
    IotMqtt_Assert( status != IOT_MQTT_STATUS_PENDING );

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Publish( IotMqttConnection_t mqttConnection,
                                const IotMqttPublishInfo_t * pPublishInfo,
                                uint32_t flags,
                                const IotMqttCallbackInfo_t * pCallbackInfo,
                                IotMqttReference_t * pPublishRef )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    _mqttOperation_t * pPublishOperation = NULL;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;

    /* Default PUBLISH serializer function. */
    IotMqttError_t ( * serializePublish )( const IotMqttPublishInfo_t *,
                                           uint8_t **,
                                           size_t *,
                                           uint16_t * ) = _IotMqtt_SerializePublish;

    /* Check that the PUBLISH information is valid. */
    if( _IotMqtt_ValidatePublish( pMqttConnection->awsIotMqttMode,
                                  pPublishInfo ) == false )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check that no notification is requested for a QoS 0 publish. */
    if( pPublishInfo->qos == IOT_MQTT_QOS_0 )
    {
        if( pCallbackInfo != NULL )
        {
            IotLogError( "QoS 0 PUBLISH should not have notification parameters set." );

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else if( ( flags & IOT_MQTT_FLAG_WAITABLE ) != 0 )
        {
            IotLogError( "QoS 0 PUBLISH should not have notification parameters set." );

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( pPublishRef != NULL )
        {
            IotLogWarn( "Ignoring reference parameter for QoS 0 publish." );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check that a reference pointer is provided for a waitable operation. */
    if( ( flags & IOT_MQTT_FLAG_WAITABLE ) == IOT_MQTT_FLAG_WAITABLE )
    {
        if( pPublishRef == NULL )
        {
            IotLogError( "Reference must be provided for a waitable PUBLISH." );

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Create a PUBLISH operation. */
    status = _IotMqtt_CreateOperation( pMqttConnection,
                                       flags,
                                       pCallbackInfo,
                                       &pPublishOperation );

    if( status != IOT_MQTT_SUCCESS )
    {
        _IOT_GOTO_CLEANUP();
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the PUBLISH operation data and set the operation type. */
    IotMqtt_Assert( pPublishOperation->status == IOT_MQTT_STATUS_PENDING );
    pPublishOperation->operation = IOT_MQTT_PUBLISH_TO_SERVER;

    /* Choose a PUBLISH serializer function. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pMqttConnection->network.serialize.publish != NULL )
        {
            serializePublish = pMqttConnection->network.serialize.publish;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    #endif

    /* Generate a PUBLISH packet from pPublishInfo. */
    status = serializePublish( pPublishInfo,
                               &( pPublishOperation->pMqttPacket ),
                               &( pPublishOperation->packetSize ),
                               &( pPublishOperation->packetIdentifier ) );

    if( status != IOT_MQTT_SUCCESS )
    {
        _IOT_GOTO_CLEANUP();
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the serialized MQTT packet. */
    IotMqtt_Assert( pPublishOperation->pMqttPacket != NULL );
    IotMqtt_Assert( pPublishOperation->packetSize > 0 );

    /* Initialize PUBLISH retry if retryLimit is set. */
    if( pPublishInfo->retryLimit > 0 )
    {
        /* A QoS 0 PUBLISH may not be retried. */
        if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
        {
            pPublishOperation->retry.limit = pPublishInfo->retryLimit;
            pPublishOperation->retry.nextPeriod = pPublishInfo->retryMs;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Set the reference, if provided. */
    if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
    {
        if( pPublishRef != NULL )
        {
            *pPublishRef = pPublishOperation;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Add the PUBLISH operation to the send queue for network transmission. */
    status = _IotMqtt_ScheduleOperation( pPublishOperation,
                                         _IotMqtt_ProcessSend,
                                         0 );

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "(MQTT connection %p) Failed to enqueue PUBLISH for sending.",
                     pMqttConnection );

        /* Clear the previously set (and now invalid) reference. */
        if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
        {
            if( pPublishRef != NULL )
            {
                *pPublishRef = IOT_MQTT_REFERENCE_INITIALIZER;
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        _IOT_GOTO_CLEANUP();
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Clean up the PUBLISH operation if this function fails. Otherwise, set the
     * appropriate return code based on QoS. */
    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_MQTT_SUCCESS )
    {
        if( pPublishOperation != NULL )
        {
            _IotMqtt_DestroyOperation( pPublishOperation );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        if( pPublishInfo->qos > IOT_MQTT_QOS_0 )
        {
            status = IOT_MQTT_STATUS_PENDING;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        IotLogInfo( "(MQTT connection %p) MQTT PUBLISH operation queued.",
                    pMqttConnection );
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_TimedPublish( IotMqttConnection_t mqttConnection,
                                     const IotMqttPublishInfo_t * pPublishInfo,
                                     uint32_t flags,
                                     uint64_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttReference_t publishRef = IOT_MQTT_REFERENCE_INITIALIZER,
                       * pPublishRef = NULL;

    /* Clear the flags. */
    flags = 0;

    /* Set the waitable flag and reference for QoS 1 PUBLISH. */
    if( pPublishInfo->qos == IOT_MQTT_QOS_1 )
    {
        flags = IOT_MQTT_FLAG_WAITABLE;
        pPublishRef = &publishRef;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Call the asynchronous PUBLISH function. */
    status = IotMqtt_Publish( mqttConnection,
                              pPublishInfo,
                              flags,
                              NULL,
                              pPublishRef );

    /* Wait for a queued QoS 1 PUBLISH to complete. */
    if( pPublishInfo->qos == IOT_MQTT_QOS_1 )
    {
        if( status == IOT_MQTT_STATUS_PENDING )
        {
            status = IotMqtt_Wait( publishRef, timeoutMs );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Wait( IotMqttReference_t reference,
                             uint64_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) reference;
    _mqttConnection_t * pMqttConnection = pOperation->pMqttConnection;

    /* Validate the given reference. */
    if( _IotMqtt_ValidateReference( reference ) == false )
    {
        status = IOT_MQTT_BAD_PARAMETER;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the MQTT connection status. */
    if( status == IOT_MQTT_SUCCESS )
    {
        IotMutex_Lock( &( pMqttConnection->referencesMutex ) );

        if( pMqttConnection->disconnected == true )
        {
            IotLogError( "(MQTT connection %p, %s operation %p) MQTT connection is closed. "
                         "Operation cannot be waited on.",
                         pMqttConnection,
                         IotMqtt_OperationType( pOperation->operation ),
                         pOperation );

            status = IOT_MQTT_NETWORK_ERROR;
        }
        else
        {
            IotLogInfo( "(MQTT connection %p, %s operation %p) Waiting for operation completion.",
                        pMqttConnection,
                        IotMqtt_OperationType( pOperation->operation ),
                        pOperation );
        }

        IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

        /* Only wait on an operation if the MQTT connection is active. */
        if( status == IOT_MQTT_SUCCESS )
        {
            if( IotSemaphore_TimedWait( &( pOperation->notify.waitSemaphore ),
                                        timeoutMs ) == false )
            {
                status = IOT_MQTT_TIMEOUT;

                /* Attempt to cancel the job of the timed out operation. */
                ( void ) _IotMqtt_DecrementOperationReferences( pOperation, true );

                /* Clean up lingering subscriptions from a timed-out SUBSCRIBE. */
                if( pOperation->operation == IOT_MQTT_SUBSCRIBE )
                {
                    IotLogDebug( "(MQTT connection %p, SUBSCRIBE operation %p) Cleaning up"
                                 " subscriptions of timed-out SUBSCRIBE."
                                 pMqttConnection,
                                 pOperation );

                    IotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );
                    _IotMqtt_RemoveSubscriptionByPacket( pMqttConnection,
                                                         pOperation->packetIdentifier,
                                                         -1 );
                    IotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );
                }
                else
                {
                    _EMPTY_ELSE_MARKER;
                }
            }
            else
            {
                /* Retrieve the status of the completed operation. */
                status = pOperation->status;
            }

            IotLogInfo( "(MQTT connection %p, %s operation %p) Wait complete with result %s.",
                        pMqttConnection
                        IotMqtt_OperationType( pOperation->operation ),
                        pOperation,
                        IotMqtt_strerror( status ) );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        /* Wait is finished; decrement operation reference count. */
        if( _IotMqtt_DecrementOperationReferences( pOperation, false ) == true )
        {
            _IotMqtt_DestroyOperation( pOperation );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    return status;
}

/*-----------------------------------------------------------*/

const char * IotMqtt_strerror( IotMqttError_t status )
{
    const char * pMessage = NULL;

    switch( status )
    {
        case IOT_MQTT_SUCCESS:
            pMessage = "SUCCESS";
            break;

        case IOT_MQTT_STATUS_PENDING:
            pMessage = "PENDING";
            break;

        case IOT_MQTT_INIT_FAILED:
            pMessage = "INITIALIZATION FAILED";
            break;

        case IOT_MQTT_BAD_PARAMETER:
            pMessage = "BAD PARAMETER";
            break;

        case IOT_MQTT_NO_MEMORY:
            pMessage = "NO MEMORY";
            break;

        case IOT_MQTT_NETWORK_ERROR:
            pMessage = "NETWORK ERROR";
            break;

        case IOT_MQTT_SCHEDULING_ERROR:
            pMessage = "SCHEDULING ERROR";
            break;

        case IOT_MQTT_BAD_RESPONSE:
            pMessage = "BAD RESPONSE RECEIVED";
            break;

        case IOT_MQTT_TIMEOUT:
            pMessage = "TIMEOUT";
            break;

        case IOT_MQTT_SERVER_REFUSED:
            pMessage = "SERVER REFUSED";
            break;

        case IOT_MQTT_RETRY_NO_RESPONSE:
            pMessage = "NO RESPONSE";
            break;

        default:
            pMessage = "INVALID STATUS";
            break;
    }

    return pMessage;
}

/*-----------------------------------------------------------*/

const char * IotMqtt_OperationType( IotMqttOperationType_t operation )
{
    const char * pMessage = NULL;

    switch( operation )
    {
        case IOT_MQTT_CONNECT:
            pMessage = "CONNECT";
            break;

        case IOT_MQTT_PUBLISH_TO_SERVER:
            pMessage = "PUBLISH";
            break;

        case IOT_MQTT_PUBACK:
            pMessage = "PUBACK";
            break;

        case IOT_MQTT_SUBSCRIBE:
            pMessage = "SUBSCRIBE";
            break;

        case IOT_MQTT_UNSUBSCRIBE:
            pMessage = "UNSUBSCRIBE";
            break;

        case IOT_MQTT_PINGREQ:
            pMessage = "PINGREQ";
            break;

        case IOT_MQTT_DISCONNECT:
            pMessage = "DISCONNECT";
            break;

        default:
            pMessage = "INVALID OPERATION";
            break;
    }

    return pMessage;
}

/*-----------------------------------------------------------*/

/* If the MQTT library is being tested, include a file that allows access to
 * internal functions and variables. */
#if IOT_MQTT_TEST == 1
    #include "iot_test_access_mqtt_api.c"
#endif
