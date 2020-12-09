/*
 * FreeRTOS MQTT V2.3.1
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
 * https://github.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_api.c
 * @brief Implements most user-facing functions of the MQTT library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Atomic operations. */
#include "iot_atomic.h"

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
#if IOT_MQTT_RESPONSE_WAIT_MS <= 0
    #error "IOT_MQTT_RESPONSE_WAIT_MS cannot be 0 or negative."
#endif
#if IOT_MQTT_RETRY_MS_CEILING <= 0
    #error "IOT_MQTT_RETRY_MS_CEILING cannot be 0 or negative."
#endif

/**
 * @brief Fixed Size Array to hold Mapping of MQTT Connection used in MQTT 201906.00 library to MQTT Context
 * used in calling MQTT LTS API from shim to send packets on the network.
 */
_connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/* Static storage for mutex used for synchronized access to #_connContext_t array. */
static StaticSemaphore_t connContextMutexStorage;

/* Handle for mutex used for synchronized access to #_connContext_t array. */
static SemaphoreHandle_t connContextMutex;

/*-----------------------------------------------------------*/

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
 * @param[in] pNetworkInfo User-provided network information for the new
 * connection.
 * @param[in] keepAliveSeconds User-provided keep-alive interval.
 * @param[out] pMqttConnection The MQTT connection associated with the keep-alive.
 *
 * @return `true` if the keep-alive job was successfully created; `false` otherwise.
 */
static bool _createKeepAliveJob( const IotMqttNetworkInfo_t * pNetworkInfo,
                                 uint16_t keepAliveSeconds,
                                 _mqttConnection_t * pMqttConnection );

/**
 * @brief Creates a new MQTT connection and initializes its members.
 *
 * @param[in] awsIotMqttMode Specifies if this connection is to an AWS IoT MQTT server.
 * @param[in] pNetworkInfo User-provided network information for the new
 * connection.
 * @param[in] keepAliveSeconds User-provided keep-alive interval for the new connection.
 *
 * @return Pointer to a newly-created MQTT connection; `NULL` on failure.
 */
static _mqttConnection_t * _createMqttConnection( bool awsIotMqttMode,
                                                  const IotMqttNetworkInfo_t * pNetworkInfo,
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
                                           IotMqttOperation_t * pOperationReference );

/**
 * @brief The time interface provided to the MQTT context used in calling MQTT LTS APIs.
 *
 * Stub for time interface as we need to pass it in setting the MQTT Context used
 * in calling the MQTT API using the MQTT LTS library.
 *
 * @return Time in milliseconds.
 */

static uint32_t getTimeMs( void );

/**
 * @brief The dummy application callback function.
 *
 * This function doesn't need to have any implementation as
 * the receive from network is not handled by the coreMQTT library,
 * but the MQTT shim itself. This function is just a dummy function
 * to be passed as a parameter to `MQTT_Init()`.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pDeserializedInfo Deserialized information from incoming packet.
 */
static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo );

/**
 * @brief A dummy function for transport interface receive.
 *
 * MQTT shim handles the receive from the network and hence transport
 * implementation for receive is not used by the coreMQTT library. This
 * dummy implementation is used for passing a non-NULL parameter to
 * `MQTT_Init()`.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pBuffer Buffer to receive the data into.
 * @param[in] bytesToRecv Number of bytes requested from the network.
 *
 * @return The number of bytes received or a negative error code.
 */
static int32_t transportRecv( NetworkContext_t * pNetworkContext,
                              void * pBuffer,
                              size_t bytesToRecv );

/**
 * @brief Function for sending data over the network.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pBuffer Buffer containing the bytes to send over the network stack.
 * @param[in] bytesToSend Number of bytes to send over the network.
 *
 * @return The number of bytes sent or a negative error code.
 */
static int32_t transportSend( NetworkContext_t * pNetworkContext,
                              const void * pMessage,
                              size_t bytesToSend );

/*-----------------------------------------------------------*/

static void _mqttOperation_tryDestroy( void * pData )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pData;
    IotTaskPoolError_t taskPoolStatus = IOT_TASKPOOL_SUCCESS;

    /* Incoming PUBLISH operations may always be freed. */
    if( pOperation->incomingPublish == true )
    {
        /* Cancel the incoming PUBLISH operation's job. */
        taskPoolStatus = IotTaskPool_TryCancel( IOT_SYSTEM_TASKPOOL,
                                                pOperation->job,
                                                NULL );

        /* If the operation's job was not canceled, it must be already executing.
         * Any other return value is invalid. */
        IotMqtt_Assert( ( taskPoolStatus == IOT_TASKPOOL_SUCCESS ) ||
                        ( taskPoolStatus == IOT_TASKPOOL_CANCEL_FAILED ) );

        /* Check if the incoming PUBLISH job was canceled. */
        if( taskPoolStatus == IOT_TASKPOOL_SUCCESS )
        {
            /* Job was canceled. Process incoming PUBLISH now to clean up. */
            _IotMqtt_ProcessIncomingPublish( IOT_SYSTEM_TASKPOOL,
                                             pOperation->job,
                                             pOperation );
        }
        else
        {
            /* The executing job will process the PUBLISH, so nothing is done here. */
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        /* Decrement reference count and destroy CONNECT operation if possible.
         * In Phase 2 Shim Implementation, only the CONNECT operation is using taskpool to send packets on the network. */
        if( pOperation->u.operation.type == IOT_MQTT_CONNECT )
        {
            if( _IotMqtt_DecrementOperationReferences( pOperation, true ) == true )
            {
                _IotMqtt_DestroyOperation( pOperation );
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }
    }
}

/*-----------------------------------------------------------*/

static bool _createKeepAliveJob( const IotMqttNetworkInfo_t * pNetworkInfo,
                                 uint16_t keepAliveSeconds,
                                 _mqttConnection_t * pMqttConnection )
{
    bool status = true;
    IotMqttError_t serializeStatus = IOT_MQTT_SUCCESS;
    IotTaskPoolError_t jobStatus = IOT_TASKPOOL_SUCCESS;

    /* Network information is not used when MQTT packet serializers are disabled. */
    ( void ) pNetworkInfo;

    /* Default PINGREQ serializer function. */
    IotMqttError_t ( * serializePingreq )( uint8_t **,
                                           size_t * ) = _IotMqtt_pingreqSerializeWrapper;

    /* Convert the keep-alive interval to milliseconds. */
    pMqttConnection->keepAliveMs = keepAliveSeconds * 1000;
    pMqttConnection->nextKeepAliveMs = pMqttConnection->keepAliveMs;

    /* Choose a PINGREQ serializer function. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( pNetworkInfo->pMqttSerializer != NULL )
        {
            if( pNetworkInfo->pMqttSerializer->serialize.pingreq != NULL )
            {
                serializePingreq = pNetworkInfo->pMqttSerializer->serialize.pingreq;
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    #endif /* if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1 */

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
                                           &( pMqttConnection->keepAliveJobStorage ),
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
            EMPTY_ELSE_MARKER;
        }

        /* Keep-alive references its MQTT connection, so increment reference. */
        ( pMqttConnection->references )++;
    }

    return status;
}

/*-----------------------------------------------------------*/

static _mqttConnection_t * _createMqttConnection( bool awsIotMqttMode,
                                                  const IotMqttNetworkInfo_t * pNetworkInfo,
                                                  uint16_t keepAliveSeconds )
{
    IOT_FUNCTION_ENTRY( bool, true );
    _mqttConnection_t * pMqttConnection = NULL;
    bool referencesMutexCreated = false;

    /* Allocate memory for the new MQTT connection. */
    pMqttConnection = IotMqtt_MallocConnection( sizeof( _mqttConnection_t ) );

    if( pMqttConnection == NULL )
    {
        IotLogError( "Failed to allocate memory for new connection." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        /* Clear the MQTT connection, then copy the MQTT server mode, network
         * interface, and disconnect callback. */
        ( void ) memset( pMqttConnection, 0x00, sizeof( _mqttConnection_t ) );
        pMqttConnection->awsIotMqttMode = awsIotMqttMode;
        pMqttConnection->pNetworkInterface = pNetworkInfo->pNetworkInterface;
        pMqttConnection->disconnectCallback = pNetworkInfo->disconnectCallback;

        /* Start a new MQTT connection with a reference count of 1. */
        pMqttConnection->references = 1;
    }

    /* Create the references mutex for a new connection. It is a recursive mutex. */
    referencesMutexCreated = IotMutex_Create( &( pMqttConnection->referencesMutex ), true );

    if( referencesMutexCreated == false )
    {
        IotLogError( "Failed to create references mutex for new connection." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Create the new connection's subscription and operation lists. */
    IotListDouble_Create( &( pMqttConnection->pendingProcessing ) );
    IotListDouble_Create( &( pMqttConnection->pendingResponse ) );

    /* AWS IoT service limits set minimum and maximum values for keep-alive interval.
     * Adjust the user-provided keep-alive interval based on these requirements. */
    if( awsIotMqttMode == true )
    {
        if( keepAliveSeconds == 0 )
        {
            keepAliveSeconds = AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE;
        }
        else if( keepAliveSeconds < AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE )
        {
            keepAliveSeconds = AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE;
        }
        else if( keepAliveSeconds > AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE )
        {
            keepAliveSeconds = AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE;
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check if keep-alive is active for this connection. */
    if( keepAliveSeconds != 0 )
    {
        if( _createKeepAliveJob( pNetworkInfo,
                                 keepAliveSeconds,
                                 pMqttConnection ) == false )
        {
            IOT_SET_AND_GOTO_CLEANUP( false );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Clean up mutexes and connection if this function failed. */
    IOT_FUNCTION_CLEANUP_BEGIN();

    if( status == false )
    {
        if( referencesMutexCreated == true )
        {
            IotMutex_Destroy( &( pMqttConnection->referencesMutex ) );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        if( pMqttConnection != NULL )
        {
            IotMqtt_FreeConnection( pMqttConnection );
            pMqttConnection = NULL;
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    return pMqttConnection;
}

/*-----------------------------------------------------------*/

static void _destroyMqttConnection( _mqttConnection_t * pMqttConnection )
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    int8_t contextIndex = -1;
    bool mutexStatus = true;
    bool connContextMutexStatus = false;

    IotMqtt_Assert( pMqttConnection != NULL );

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    /* An MQTT connection can be created without having MQTT Context associated.
     * This can happen in case of an error in #IotMqtt_Connect() API. */
    if( contextIndex == -1 )
    {
        IotLogWarn( "(MQTT connection %p) does not have an MQTT Context.", pMqttConnection );
    }

    /* Clean up keep-alive if still allocated. */
    if( pMqttConnection->keepAliveMs != 0 )
    {
        IotLogDebug( "(MQTT connection %p) Cleaning up keep-alive.", pMqttConnection );

        _IotMqtt_FreePacket( pMqttConnection->pPingreqPacket );

        /* Clear data about the keep-alive. */
        pMqttConnection->keepAliveMs = 0;
        pMqttConnection->pPingreqPacket = NULL;
        pMqttConnection->pingreqPacketSize = 0;

        /* Decrement reference count. */
        pMqttConnection->references--;
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* A connection to be destroyed should have no keep-alive and at most 1
     * reference. */
    IotMqtt_Assert( pMqttConnection->references <= 1 );
    IotMqtt_Assert( pMqttConnection->keepAliveMs == 0 );
    IotMqtt_Assert( pMqttConnection->pPingreqPacket == NULL );
    IotMqtt_Assert( pMqttConnection->pingreqPacketSize == 0 );

    if( contextIndex != -1 )
    {
        /* Remove all subscriptions. */
        if( IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) ) == true )
        {
            IotMqtt_RemoveAllMatches( ( connToContext[ contextIndex ].subscriptionArray ), NULL );

            mutexStatus = IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) );
        }
        else
        {
            /* Fail to take context mutex due to timeout. */
            mutexStatus = false;
        }

        IotMqtt_Assert( mutexStatus == true );
    }

    /* Destroy an owned network connection. */
    if( pMqttConnection->ownNetworkConnection == true )
    {
        networkStatus = pMqttConnection->pNetworkInterface->destroy( pMqttConnection->pNetworkConnection );

        if( networkStatus != IOT_NETWORK_SUCCESS )
        {
            IotLogWarn( "(MQTT connection %p) Failed to destroy network connection.",
                        pMqttConnection );
        }
        else
        {
            IotLogInfo( "(MQTT connection %p) Network connection destroyed.",
                        pMqttConnection );
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Destroy mutexes. */
    IotMutex_Destroy( &( pMqttConnection->referencesMutex ) );

    if( contextIndex != -1 )
    {
        IotMutex_Delete( &( connToContext[ contextIndex ].contextMutex ) );
        IotMutex_Delete( &( connToContext[ contextIndex ].subscriptionMutex ) );
        /* Lock mutex before updating the #connToContext array. */
        connContextMutexStatus = IotMutex_TakeRecursive( &connContextMutex );

        if( connContextMutexStatus == true )
        {
            /* Free the MQTT Context for the MQTT connection to be destroyed. */
            _IotMqtt_removeContext( pMqttConnection );

            /* Release mutex. */
            connContextMutexStatus = IotMutex_GiveRecursive( &connContextMutex );
        }

        IotMqtt_Assert( connContextMutexStatus == true );
    }

    IotLogDebug( "(MQTT connection %p) Connection destroyed.", pMqttConnection );

    /* Free connection. */
    IotMqtt_FreeConnection( pMqttConnection );
}

/*-----------------------------------------------------------*/

static IotMqttError_t _subscriptionCommon( IotMqttOperationType_t operation,
                                           IotMqttConnection_t mqttConnection,
                                           const IotMqttSubscription_t * pSubscriptionList,
                                           size_t subscriptionCount,
                                           uint32_t flags,
                                           const IotMqttCallbackInfo_t * pCallbackInfo,
                                           IotMqttOperation_t * pOperationReference )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    _mqttOperation_t * pSubscriptionOperation = NULL;

    /* This function should only be called for subscribe or unsubscribe. */
    IotMqtt_Assert( ( operation == IOT_MQTT_SUBSCRIBE ) ||
                    ( operation == IOT_MQTT_UNSUBSCRIBE ) );

    /* Check that all elements in the subscription list are valid. */
    if( _IotMqtt_ValidateSubscriptionList( operation,
                                           mqttConnection->awsIotMqttMode,
                                           pSubscriptionList,
                                           subscriptionCount ) == false )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check that a reference pointer is provided for a waitable operation. */
    if( ( flags & IOT_MQTT_FLAG_WAITABLE ) == IOT_MQTT_FLAG_WAITABLE )
    {
        if( pOperationReference == NULL )
        {
            IotLogError( "Reference must be provided for a waitable %s.",
                         IotMqtt_OperationType( operation ) );

            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Remove the MQTT subscription list for an UNSUBSCRIBE. */
    if( operation == IOT_MQTT_UNSUBSCRIBE )
    {
        _IotMqtt_RemoveSubscriptionByTopicFilter( mqttConnection,
                                                  pSubscriptionList,
                                                  subscriptionCount );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Create a subscription operation. */
    status = _IotMqtt_CreateOperation( mqttConnection,
                                       flags,
                                       pCallbackInfo,
                                       &pSubscriptionOperation );

    if( status != IOT_MQTT_SUCCESS )
    {
        IOT_GOTO_CLEANUP();
    }

    /* Check the subscription operation data and set the operation type. */
    IotMqtt_Assert( pSubscriptionOperation->u.operation.status == IOT_MQTT_STATUS_PENDING );
    IotMqtt_Assert( pSubscriptionOperation->u.operation.retry.limit == 0 );
    pSubscriptionOperation->u.operation.type = operation;

    /* Add the subscription list for a SUBSCRIBE. */
    if( operation == IOT_MQTT_SUBSCRIBE )
    {
        status = _IotMqtt_AddSubscriptions( mqttConnection,
                                            pSubscriptionOperation->u.operation.packetIdentifier,
                                            pSubscriptionList,
                                            subscriptionCount );

        if( status != IOT_MQTT_SUCCESS )
        {
            IOT_GOTO_CLEANUP();
        }
    }

    IotMutex_Lock( &( mqttConnection->referencesMutex ) );

    /* Set the reference, if provided. */
    if( pOperationReference != NULL )
    {
        *pOperationReference = pSubscriptionOperation;
    }

    if( operation == IOT_MQTT_SUBSCRIBE )
    {
        /* Calling SUBSCRIBE wrapper to send SUBSCRIBE packet on the network using MQTT LTS SUBSCRIBE API. */
        status = _IotMqtt_managedSubscribe( mqttConnection,
                                            pSubscriptionOperation,
                                            pSubscriptionList,
                                            subscriptionCount );
    }
    else if( operation == IOT_MQTT_UNSUBSCRIBE )
    {
        /* Calling UNSUBSCRIBE wrapper to send UNSUBSCRIBE packet on the network using MQTT LTS UNSUBSCRIBE API. */
        status = _IotMqtt_managedUnsubscribe( mqttConnection,
                                              pSubscriptionOperation,
                                              pSubscriptionList,
                                              subscriptionCount );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Operation must be linked. */
        IotMqtt_Assert( IotLink_IsLinked( &( pSubscriptionOperation->link ) ) );

        /* Transfer to pending response list. */
        IotListDouble_Remove( &( pSubscriptionOperation->link ) );
        IotListDouble_InsertHead( &( mqttConnection->pendingResponse ),
                                  &( pSubscriptionOperation->link ) );


        /* Processing operation after sending it on the network. */
        _IotMqtt_ProcessOperation( pSubscriptionOperation );
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to send %s packet on the network.",
                     mqttConnection,
                     IotMqtt_OperationType( operation ) );

        if( operation == IOT_MQTT_SUBSCRIBE )
        {
            _IotMqtt_RemoveSubscriptionByPacket( mqttConnection,
                                                 pSubscriptionOperation->u.operation.packetIdentifier,
                                                 -1 );
        }

        /* Clear the previously set (and now invalid) reference. */
        if( pOperationReference != NULL )
        {
            *pOperationReference = IOT_MQTT_OPERATION_INITIALIZER;
        }
    }

    IotMutex_Unlock( &( mqttConnection->referencesMutex ) );

    /* Clean up if this function failed. */
    IOT_FUNCTION_CLEANUP_BEGIN();

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
                    mqttConnection,
                    IotMqtt_OperationType( operation ) );
    }

    IOT_FUNCTION_CLEANUP_END();
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
        EMPTY_ELSE_MARKER;
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
        EMPTY_ELSE_MARKER;
    }
}

/*-----------------------------------------------------------*/

static uint32_t getTimeMs( void )
{
    return 0U;
}

/*-----------------------------------------------------------*/

static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo )
{
    /* This function doesn't need to have any implementation as
     * the receive from network is not handled by the coreMQTT library,
     * but the MQTT shim itself. This function is just a dummy function
     * to be passed as a parameter to `MQTT_Init()`. */
    ( void ) pContext;
    ( void ) pPacketInfo;
    ( void ) pDeserializedInfo;
}

/*-----------------------------------------------------------*/

static int32_t transportSend( NetworkContext_t * pNetworkContext,
                              const void * pMessage,
                              size_t bytesToSend )
{
    int32_t bytesSend = 0;

    IotMqtt_Assert( pNetworkContext != NULL );
    IotMqtt_Assert( pMessage != NULL );

    /* Sending the bytes on the network using Network Interface. */
    bytesSend = pNetworkContext->pNetworkInterface->send( pNetworkContext->pNetworkConnection, ( const uint8_t * ) pMessage, bytesToSend );

    if( bytesSend <= 0 )
    {
        /* Network Send Interface returns a negative value or zero in case of any socket error,
         * unifying the error codes here for socket error and timeout to comply with the MQTT LTS Library.
         */
        bytesSend = -1;
    }

    return bytesSend;
}

/*-----------------------------------------------------------*/

static int32_t transportRecv( NetworkContext_t * pNetworkContext,
                              void * pBuffer,
                              size_t bytesToRecv )
{
    /* MQTT shim handles the receive from the network and hence transport
     * implementation for receive is not used by the coreMQTT library. This
     * dummy implementation is used for passing a non-NULL parameter to
     * `MQTT_Init()`. */
    ( void ) pNetworkContext;
    ( void ) pBuffer;
    ( void ) bytesToRecv;

    /* Always return an error. */
    return -1;
}


/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Init( void )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;
    bool mutexCreated = false;

    /* Create a recursive mutex for synchronized access to #_connContext_t array. */
    mutexCreated = IotMutex_CreateRecursiveMutex( &connContextMutex, &connContextMutexStorage );

    /* Check mutex creation failed. */
    if( mutexCreated == false )
    {
        status = IOT_MQTT_INIT_FAILED;
    }

    /* Log initialization status. */
    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to initialize MQTT library serializer. " );
    }
    else
    {
        IotLogInfo( "MQTT library successfully initialized." );
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotMqtt_Cleanup( void )
{
    /* Delete the recursive mutex for synchronized access to #_connContext_t array. */
    IotMutex_Delete( &connContextMutex );

    IotLogInfo( "MQTT library cleanup done." );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Connect( const IotMqttNetworkInfo_t * pNetworkInfo,
                                const IotMqttConnectInfo_t * pConnectInfo,
                                uint32_t timeoutMs,
                                IotMqttConnection_t * const pMqttConnection )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    bool networkCreated = false, ownNetworkConnection = false;
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    IotTaskPoolError_t taskPoolStatus = IOT_TASKPOOL_SUCCESS;
    void * pNetworkConnection = NULL;
    _mqttOperation_t * pOperation = NULL;
    IotMqttConnection_t newMqttConnection = NULL;
    TransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;
    int8_t contextIndex = -1;
    bool subscriptionMutexCreated = false;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    bool contextMutexCreated = false;
    bool connContextMutexStatus = false;

    /* Default CONNECT serializer function. */
    IotMqttError_t ( * serializeConnect )( const IotMqttConnectInfo_t *,
                                           uint8_t **,
                                           size_t * ) = _IotMqtt_connectSerializeWrapper;

    /* Network info must not be NULL. */
    if( pNetworkInfo == NULL )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Validate network interface and connect info. */
    if( _IotMqtt_ValidateConnect( pConnectInfo ) == false )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* If will info is provided, check that it is valid. */
    if( pConnectInfo->pWillInfo != NULL )
    {
        if( _IotMqtt_ValidatePublish( pConnectInfo->awsIotMqttMode,
                                      pConnectInfo->pWillInfo ) == false )
        {
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else if( pConnectInfo->pWillInfo->payloadLength > UINT16_MAX )
        {
            /* Will message payloads cannot be larger than 65535. This restriction
             * applies only to will messages, and not normal PUBLISH messages. */
            IotLogError( "Will payload cannot be larger than 65535." );

            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
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
                IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Create a new MQTT connection if requested. Otherwise, copy the existing
     * network connection. */
    if( pNetworkInfo->createNetworkConnection == true )
    {
        networkStatus = pNetworkInfo->pNetworkInterface->create( pNetworkInfo->u.setup.pNetworkServerInfo,
                                                                 pNetworkInfo->u.setup.pNetworkCredentialInfo,
                                                                 &pNetworkConnection );

        if( networkStatus == IOT_NETWORK_SUCCESS )
        {
            networkCreated = true;

            /* This MQTT connection owns the network connection it created and
             * should destroy it on cleanup. */
            ownNetworkConnection = true;
        }
        else
        {
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NETWORK_ERROR );
        }
    }
    else
    {
        pNetworkConnection = pNetworkInfo->u.pNetworkConnection;
        networkCreated = true;
    }

    IotLogInfo( "Establishing new MQTT connection." );

    /* Initialize a new MQTT connection object. */
    newMqttConnection = _createMqttConnection( pConnectInfo->awsIotMqttMode,
                                               pNetworkInfo,
                                               pConnectInfo->keepAliveSeconds );

    if( newMqttConnection == NULL )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }
    else
    {
        /* Set the network connection associated with the MQTT connection. */
        newMqttConnection->pNetworkConnection = pNetworkConnection;
        newMqttConnection->ownNetworkConnection = ownNetworkConnection;

        /* Set the MQTT packet serializer overrides. */
        #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            newMqttConnection->pSerializer = pNetworkInfo->pMqttSerializer;
        #endif
    }

    /* Set the MQTT receive callback. */
    networkStatus = newMqttConnection->pNetworkInterface->setReceiveCallback( pNetworkConnection,
                                                                              IotMqtt_ReceiveCallback,
                                                                              newMqttConnection );

    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to set MQTT network receive callback." );

        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NETWORK_ERROR );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Lock mutex before updating the #connToContext array. */
    connContextMutexStatus = IotMutex_TakeRecursive( &connContextMutex );

    if( connContextMutexStatus == false )
    {
        IotLogError( "Failed to lock connContextMutex." );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_TIMEOUT );
    }

    /* Getting the free index from the MQTT connection to MQTT context mapping array. */
    contextIndex = _IotMqtt_getFreeIndexFromContextConnectionArray();

    if( contextIndex < 0 )
    {
        IotLogError( "(MQTT connection %p) Failed to allocate memory for "
                     "the MQTT context and the MQTT Connection Mapping. Update the MAX_NO_OF_MQTT_CONNECTIONS"
                     " config value to resolve the error. ",
                     newMqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }

    /* Clear the array at the index obtained. */
    memset( &( connToContext[ contextIndex ] ), 0x00, sizeof( _connContext_t ) );

    /* Creating Mutex for the synchronization of MQTT Context used for sending the packets
     * on the network using MQTT LTS API. */
    contextMutexCreated = IotMutex_CreateRecursiveMutex( &( connToContext[ contextIndex ].contextMutex ),
                                                         &( connToContext[ contextIndex ].contextMutexStorage ) );
    /* Create the subscription mutex for a new connection. */

    if( contextMutexCreated == true )
    {
        /* Assigning the MQTT Connection. */
        connToContext[ contextIndex ].mqttConnection = newMqttConnection;

        /* Assigning the Network Context to be used by this MQTT Context. */
        connToContext[ contextIndex ].networkContext.pNetworkConnection = pNetworkConnection;
        connToContext[ contextIndex ].networkContext.pNetworkInterface = pNetworkInfo->pNetworkInterface;

        /* Fill in TransportInterface send function pointer. We will not be implementing the
         * TransportInterface receive function pointer as receiving of packets is handled in shim by network
         * receive task. Only using MQTT LTS APIs for transmit path.*/
        transport.pNetworkContext = &( connToContext[ contextIndex ].networkContext );
        transport.send = transportSend;
        transport.recv = transportRecv;

        /* Fill the values for network buffer. */
        networkBuffer.pBuffer = &( connToContext[ contextIndex ].buffer[ 0 ] );
        networkBuffer.size = NETWORK_BUFFER_SIZE;

        subscriptionMutexCreated = IotMutex_CreateNonRecursiveMutex( &( connToContext[ contextIndex ].subscriptionMutex ),
                                                                     &( connToContext[ contextIndex ].subscriptionMutexStorage ) );

        if( subscriptionMutexCreated == false )
        {
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }

        /* Initializing the MQTT context used in calling MQTT LTS API. */
        managedMqttStatus = MQTT_Init( &( connToContext[ contextIndex ].context ), &transport, getTimeMs, eventCallback, &networkBuffer );
        status = convertReturnCode( managedMqttStatus );

        if( status != IOT_MQTT_SUCCESS )
        {
            IotLogError( "(MQTT connection %p) Failed to initialize context for "
                         "the MQTT connection.",
                         newMqttConnection );
            IOT_GOTO_CLEANUP();
        }
    }
    else
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }

    /* Release the connContextMutex mutex. */
    connContextMutexStatus = IotMutex_GiveRecursive( &connContextMutex );

    if( connContextMutexStatus == false )
    {
        IotLogError( "Failed to release connContextMutex." );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }

    /* Create a CONNECT operation. */
    status = _IotMqtt_CreateOperation( newMqttConnection,
                                       IOT_MQTT_FLAG_WAITABLE,
                                       NULL,
                                       &pOperation );

    if( status != IOT_MQTT_SUCCESS )
    {
        IOT_GOTO_CLEANUP();
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Ensure the members set by operation creation and serialization
     * are appropriate for a blocking CONNECT. */
    IotMqtt_Assert( pOperation->u.operation.status == IOT_MQTT_STATUS_PENDING );
    IotMqtt_Assert( ( pOperation->u.operation.flags & IOT_MQTT_FLAG_WAITABLE )
                    == IOT_MQTT_FLAG_WAITABLE );
    IotMqtt_Assert( pOperation->u.operation.retry.limit == 0 );

    /* Set the operation type. */
    pOperation->u.operation.type = IOT_MQTT_CONNECT;

    /* Add previous session subscriptions. */
    if( pConnectInfo->pPreviousSubscriptions != NULL )
    {
        /* Previous subscription count should have been validated as nonzero. */
        IotMqtt_Assert( pConnectInfo->previousSubscriptionCount > 0 );

        status = _IotMqtt_AddSubscriptions( newMqttConnection,
                                            2,
                                            pConnectInfo->pPreviousSubscriptions,
                                            pConnectInfo->previousSubscriptionCount );

        if( status != IOT_MQTT_SUCCESS )
        {
            IOT_GOTO_CLEANUP();
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Choose a CONNECT serializer function. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        if( newMqttConnection->pSerializer != NULL )
        {
            if( newMqttConnection->pSerializer->serialize.connect != NULL )
            {
                serializeConnect = newMqttConnection->pSerializer->serialize.connect;
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    #endif /* if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1 */

    /* Convert the connect info and will info objects to an MQTT CONNECT packet. */
    status = serializeConnect( pConnectInfo,
                               &( pOperation->u.operation.pMqttPacket ),
                               &( pOperation->u.operation.packetSize ) );

    if( status != IOT_MQTT_SUCCESS )
    {
        IOT_GOTO_CLEANUP();
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check the serialized MQTT packet. */
    IotMqtt_Assert( pOperation->u.operation.pMqttPacket != NULL );
    IotMqtt_Assert( pOperation->u.operation.packetSize > 0 );

    /* Add the CONNECT operation to the send queue for network transmission. */
    status = _IotMqtt_ScheduleOperation( pOperation,
                                         _IotMqtt_ProcessSend,
                                         0 );

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to enqueue CONNECT for sending." );
    }
    else
    {
        /* Wait for the CONNECT operation to complete, i.e. wait for CONNACK. */
        status = IotMqtt_Wait( pOperation,
                               timeoutMs );

        /* The call to wait cleans up the CONNECT operation, so set the pointer
         * to NULL. */
        pOperation = NULL;
    }

    /* When a connection is successfully established, schedule keep-alive job. */
    if( status == IOT_MQTT_SUCCESS )
    {
        /* Check if a keep-alive job should be scheduled. */
        if( newMqttConnection->keepAliveMs != 0 )
        {
            IotLogDebug( "Scheduling first MQTT keep-alive job." );

            taskPoolStatus = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL,
                                                           newMqttConnection->keepAliveJob,
                                                           newMqttConnection->nextKeepAliveMs );

            if( taskPoolStatus != IOT_TASKPOOL_SUCCESS )
            {
                IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_SCHEDULING_ERROR );
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to establish new MQTT connection, error %s.",
                     IotMqtt_strerror( status ) );

        /* The network connection must be closed if it was created. */
        if( networkCreated == true )
        {
            networkStatus = pNetworkInfo->pNetworkInterface->close( pNetworkConnection );

            if( networkStatus != IOT_NETWORK_SUCCESS )
            {
                IotLogWarn( "Failed to close network connection." );
            }
            else
            {
                IotLogInfo( "Network connection closed on error." );
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        if( pOperation != NULL )
        {
            _IotMqtt_DestroyOperation( pOperation );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        if( newMqttConnection != NULL )
        {
            _destroyMqttConnection( newMqttConnection );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        IotLogInfo( "New MQTT connection %p established.", pMqttConnection );
        /* Set the output parameter. */
        *pMqttConnection = newMqttConnection;
    }

    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/


void IotMqtt_Disconnect( IotMqttConnection_t mqttConnection,
                         uint32_t flags )
{
    bool disconnected = false;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttOperation_t * pOperation = NULL;

    IotLogInfo( "(MQTT connection %p) Disconnecting connection.", mqttConnection );

    /* Read the connection status. */
    IotMutex_Lock( &( mqttConnection->referencesMutex ) );
    disconnected = mqttConnection->disconnected;
    IotMutex_Unlock( &( mqttConnection->referencesMutex ) );

    /* Only send a DISCONNECT packet if the connection is active and the "cleanup only"
     * flag is not set. */
    if( disconnected == false )
    {
        if( ( flags & IOT_MQTT_FLAG_CLEANUP_ONLY ) == 0 )
        {
            /* Create a DISCONNECT operation. This function blocks until the DISCONNECT
             * packet is sent, so it sets IOT_MQTT_FLAG_WAITABLE. */
            status = _IotMqtt_CreateOperation( mqttConnection,
                                               IOT_MQTT_FLAG_WAITABLE,
                                               NULL,
                                               &pOperation );
            IotMutex_Lock( &( mqttConnection->referencesMutex ) );

            if( status == IOT_MQTT_SUCCESS )
            {
                /* Ensure that the members set by operation creation and serialization
                 * are appropriate for a blocking DISCONNECT. */
                IotMqtt_Assert( pOperation->u.operation.status == IOT_MQTT_STATUS_PENDING );
                IotMqtt_Assert( ( pOperation->u.operation.flags & IOT_MQTT_FLAG_WAITABLE )
                                == IOT_MQTT_FLAG_WAITABLE );
                IotMqtt_Assert( pOperation->u.operation.retry.limit == 0 );

                /* Set the operation type. */
                pOperation->u.operation.type = IOT_MQTT_DISCONNECT;

                /* Calling DISCONNECT wrapper to send DISCONNECT packet on the network using MQTT LTS DISCONNECT API. */
                status = _IotMqtt_managedDisconnect( mqttConnection );
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }

            if( status == IOT_MQTT_SUCCESS )
            {
                /* Processing the operation after sending it on the network. */
                _IotMqtt_ProcessOperation( pOperation );
                IotMutex_Unlock( &( mqttConnection->referencesMutex ) );

                /* Destroying the operation after the DISCONNECT Packet is sent on the network. */
                if( _IotMqtt_DecrementOperationReferences( pOperation, false ) == true )
                {
                    _IotMqtt_DestroyOperation( pOperation );
                }
            }
            else
            {
                IotLogWarn( "(MQTT connection %p) Failed to send DISCONNECT packet. ",
                            mqttConnection );
                _IotMqtt_DestroyOperation( pOperation );
                IotMutex_Unlock( &( mqttConnection->referencesMutex ) );
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Close the underlying network connection. This also cleans up keep-alive. */
    _IotMqtt_CloseNetworkConnection( IOT_MQTT_DISCONNECT_CALLED,
                                     mqttConnection );

    /* Check if the connection may be destroyed. */
    IotMutex_Lock( &( mqttConnection->referencesMutex ) );

    /* At this point, the connection should be marked disconnected. */
    IotMqtt_Assert( mqttConnection->disconnected == true );

    /* Attempt cancel and destroy each operation in the connection's lists. */
    IotListDouble_RemoveAll( &( mqttConnection->pendingProcessing ),
                             _mqttOperation_tryDestroy,
                             offsetof( _mqttOperation_t, link ) );

    IotListDouble_RemoveAll( &( mqttConnection->pendingResponse ),
                             _mqttOperation_tryDestroy,
                             offsetof( _mqttOperation_t, link ) );

    IotMutex_Unlock( &( mqttConnection->referencesMutex ) );

    /* Decrement the connection reference count and destroy it if possible. */
    _IotMqtt_DecrementConnectionReferences( mqttConnection );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Subscribe( IotMqttConnection_t mqttConnection,
                                  const IotMqttSubscription_t * pSubscriptionList,
                                  size_t subscriptionCount,
                                  uint32_t flags,
                                  const IotMqttCallbackInfo_t * pCallbackInfo,
                                  IotMqttOperation_t * pSubscribeOperation )
{
    return _subscriptionCommon( IOT_MQTT_SUBSCRIBE,
                                mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                flags,
                                pCallbackInfo,
                                pSubscribeOperation );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_TimedSubscribe( IotMqttConnection_t mqttConnection,
                                       const IotMqttSubscription_t * pSubscriptionList,
                                       size_t subscriptionCount,
                                       uint32_t flags,
                                       uint32_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttOperation_t subscribeOperation = IOT_MQTT_OPERATION_INITIALIZER;

    /* Flags are not used, but the parameter is present for future compatibility. */
    ( void ) flags;

    /* Call the asynchronous SUBSCRIBE function. */
    status = IotMqtt_Subscribe( mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                IOT_MQTT_FLAG_WAITABLE,
                                NULL,
                                &subscribeOperation );

    /* Wait for the SUBSCRIBE operation to complete. */
    if( status == IOT_MQTT_STATUS_PENDING )
    {
        status = IotMqtt_Wait( subscribeOperation, timeoutMs );
    }
    else
    {
        EMPTY_ELSE_MARKER;
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
                                    IotMqttOperation_t * pUnsubscribeOperation )
{
    return _subscriptionCommon( IOT_MQTT_UNSUBSCRIBE,
                                mqttConnection,
                                pSubscriptionList,
                                subscriptionCount,
                                flags,
                                pCallbackInfo,
                                pUnsubscribeOperation );
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_TimedUnsubscribe( IotMqttConnection_t mqttConnection,
                                         const IotMqttSubscription_t * pSubscriptionList,
                                         size_t subscriptionCount,
                                         uint32_t flags,
                                         uint32_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttOperation_t unsubscribeOperation = IOT_MQTT_OPERATION_INITIALIZER;

    /* Flags are not used, but the parameter is present for future compatibility. */
    ( void ) flags;

    /* Call the asynchronous UNSUBSCRIBE function. */
    status = IotMqtt_Unsubscribe( mqttConnection,
                                  pSubscriptionList,
                                  subscriptionCount,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  &unsubscribeOperation );

    /* Wait for the UNSUBSCRIBE operation to complete. */
    if( status == IOT_MQTT_STATUS_PENDING )
    {
        status = IotMqtt_Wait( unsubscribeOperation, timeoutMs );
    }
    else
    {
        EMPTY_ELSE_MARKER;
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
                                IotMqttOperation_t * pPublishOperation )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    _mqttOperation_t * pOperation = NULL;

    /* Check that the PUBLISH information is valid. */
    if( _IotMqtt_ValidatePublish( mqttConnection->awsIotMqttMode,
                                  pPublishInfo ) == false )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check that no notification is requested for a QoS 0 publish. */
    if( pPublishInfo->qos == IOT_MQTT_QOS_0 )
    {
        if( pCallbackInfo != NULL )
        {
            IotLogError( "QoS 0 PUBLISH should not have notification parameters set." );

            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else if( ( flags & IOT_MQTT_FLAG_WAITABLE ) != 0 )
        {
            IotLogError( "QoS 0 PUBLISH should not have notification parameters set." );

            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        if( pPublishOperation != NULL )
        {
            IotLogWarn( "Ignoring reference parameter for QoS 0 publish." );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check that a reference pointer is provided for a waitable operation. */
    if( ( flags & IOT_MQTT_FLAG_WAITABLE ) == IOT_MQTT_FLAG_WAITABLE )
    {
        if( pPublishOperation == NULL )
        {
            IotLogError( "Reference must be provided for a waitable PUBLISH." );

            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Create a PUBLISH operation. */
    status = _IotMqtt_CreateOperation( mqttConnection,
                                       flags,
                                       pCallbackInfo,
                                       &pOperation );

    if( status != IOT_MQTT_SUCCESS )
    {
        IOT_GOTO_CLEANUP();
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check the PUBLISH operation data and set the operation type. */
    IotMqtt_Assert( pOperation->u.operation.status == IOT_MQTT_STATUS_PENDING );
    pOperation->u.operation.type = IOT_MQTT_PUBLISH_TO_SERVER;

    /* Initialize PUBLISH retry if retryLimit is set. */
    if( pPublishInfo->retryLimit > 0 )
    {
        /* A QoS 0 PUBLISH may not be retried. */
        if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
        {
            pOperation->u.operation.retry.limit = pPublishInfo->retryLimit;
            pOperation->u.operation.retry.nextPeriod = pPublishInfo->retryMs;
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Set the reference, if provided. */
    if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
    {
        if( pPublishOperation != NULL )
        {
            *pPublishOperation = pOperation;
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    IotMutex_Lock( &( mqttConnection->referencesMutex ) );

    /* Calling PUBLISH wrapper to send PUBLISH packet on the network using MQTT LTS PUBLISH API. */
    status = _IotMqtt_managedPublish( mqttConnection,
                                      pOperation,
                                      pPublishInfo );

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Processing operation after sending the packet on the network. */
        _IotMqtt_ProcessOperation( pOperation );
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to send PUBLISH packet on the network.",
                     mqttConnection );

        /* Clear the previously set (and now invalid) reference. */
        if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
        {
            if( pPublishOperation != NULL )
            {
                *pPublishOperation = IOT_MQTT_OPERATION_INITIALIZER;
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }

    IotMutex_Unlock( &( mqttConnection->referencesMutex ) );

    /* Clean up the PUBLISH operation if this function fails. Otherwise, set the
     * appropriate return code based on QoS. */
    IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_MQTT_SUCCESS )
    {
        if( pOperation != NULL )
        {
            _IotMqtt_DestroyOperation( pOperation );
        }
        else
        {
            EMPTY_ELSE_MARKER;
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
            EMPTY_ELSE_MARKER;
        }

        IotLogInfo( "(MQTT connection %p) MQTT PUBLISH operation queued.",
                    mqttConnection );
    }

    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_TimedPublish( IotMqttConnection_t mqttConnection,
                                     const IotMqttPublishInfo_t * pPublishInfo,
                                     uint32_t flags,
                                     uint32_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttOperation_t publishOperation = IOT_MQTT_OPERATION_INITIALIZER,
                       * pPublishOperation = NULL;

    /* Clear the flags. */
    flags = 0;

    /* Set the waitable flag and reference for QoS 1 PUBLISH. */
    if( pPublishInfo->qos == IOT_MQTT_QOS_1 )
    {
        flags = IOT_MQTT_FLAG_WAITABLE;
        pPublishOperation = &publishOperation;
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Call the asynchronous PUBLISH function. */
    status = IotMqtt_Publish( mqttConnection,
                              pPublishInfo,
                              flags,
                              NULL,
                              pPublishOperation );

    /* Wait for a queued QoS 1 PUBLISH to complete. */
    if( pPublishInfo->qos == IOT_MQTT_QOS_1 )
    {
        if( status == IOT_MQTT_STATUS_PENDING )
        {
            status = IotMqtt_Wait( publishOperation, timeoutMs );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t IotMqtt_Wait( IotMqttOperation_t operation,
                             uint32_t timeoutMs )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;
    _mqttConnection_t * pMqttConnection = operation->pMqttConnection;

    /* Validate the given operation reference. */
    if( _IotMqtt_ValidateOperation( operation ) == false )
    {
        status = IOT_MQTT_BAD_PARAMETER;
    }
    else
    {
        EMPTY_ELSE_MARKER;
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
                         IotMqtt_OperationType( operation->u.operation.type ),
                         operation );

            status = IOT_MQTT_NETWORK_ERROR;
        }
        else
        {
            IotLogInfo( "(MQTT connection %p, %s operation %p) Waiting for operation completion.",
                        pMqttConnection,
                        IotMqtt_OperationType( operation->u.operation.type ),
                        operation );
        }

        IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

        /* Only wait on an operation if the MQTT connection is active. */
        if( status == IOT_MQTT_SUCCESS )
        {
            if( IotSemaphore_TimedWait( &( operation->u.operation.notify.waitSemaphore ),
                                        timeoutMs ) == false )
            {
                status = IOT_MQTT_TIMEOUT;

                /* Attempt to cancel the CONNECT job of the timed out operation.
                 * As only CONNECT operation is using taskpool for sending packets on the network. */
                if( operation->u.operation.type == IOT_MQTT_CONNECT )
                {
                    ( void ) _IotMqtt_DecrementOperationReferences( operation, true );
                }

                /* Clean up lingering subscriptions from a timed-out SUBSCRIBE. */
                if( operation->u.operation.type == IOT_MQTT_SUBSCRIBE )
                {
                    IotLogDebug( "(MQTT connection %p, SUBSCRIBE operation %p) Cleaning up"
                                 " subscriptions of timed-out SUBSCRIBE.",
                                 pMqttConnection,
                                 operation );

                    _IotMqtt_RemoveSubscriptionByPacket( pMqttConnection,
                                                         operation->u.operation.packetIdentifier,
                                                         -1 );
                }
                else
                {
                    EMPTY_ELSE_MARKER;
                }
            }
            else
            {
                /* Retrieve the status of the completed operation. */
                status = operation->u.operation.status;
            }

            IotLogInfo( "(MQTT connection %p, %s operation %p) Wait complete with result %s.",
                        pMqttConnection,
                        IotMqtt_OperationType( operation->u.operation.type ),
                        operation,
                        IotMqtt_strerror( status ) );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        /* Wait is finished; decrement operation reference count. */
        if( _IotMqtt_DecrementOperationReferences( operation, false ) == true )
        {
            _IotMqtt_DestroyOperation( operation );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
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

/* Provide access to internal functions and variables if testing. */
#if IOT_BUILD_TESTS == 1
    #include "iot_test_access_mqtt_api.c"
#endif
