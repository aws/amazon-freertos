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
 * @file aws_iot_mqtt_operation.c
 * @brief Implements functions that process MQTT operations.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/*-----------------------------------------------------------*/

/**
 * @brief First parameter to #_mqttOperation_match.
 */
typedef struct _operationMatchParam
{
    AwsIotMqttOperationType_t operation; /**< @brief The operation to look for. */
    const uint16_t * pPacketIdentifier;  /**< @brief The packet identifier associated with the operation.
                                          * Set to `NULL` to ignore packet identifier. */
} _operationMatchParam_t;

/*-----------------------------------------------------------*/

/**
 * @brief Match an MQTT operation by type and packet identifier.
 *
 * @param[in] pOperationLink Pointer to the link member of an #_mqttOperation_t.
 * @param[in] pMatch Pointer to an #_operationMatchParam_t.
 *
 * @return `true` if the operation matches the parameters in `pArgument`; `false`
 * otherwise.
 */
static bool _mqttOperation_match( const IotLink_t * pOperationLink,
                                  void * pMatch );

/**
 * @brief Calculate the timeout and period of the next PUBLISH retry event.
 *
 * Calculates the next time a PUBLISH should be transmitted and places the
 * retry event in a timer event queue.
 *
 * @param[in] pMqttConnection The MQTT connection associated with the PUBLISH.
 * @param[in] pPublishRetry The current PUBLISH retry event.
 *
 * @return true if the retry event was successfully placed in a timer event queue;
 * false otherwise.
 */
static bool _calculateNextPublishRetry( _mqttConnection_t * const pMqttConnection,
                                        _mqttTimerEvent_t * const pPublishRetry );

/**
 * @brief Send the packet associated with an MQTT operation.
 *
 * Transmits the MQTT packet from a pending operation to the MQTT server, then
 * places the operation in the list of MQTT operations awaiting responses from
 * the server.
 *
 * @param[in] pOperation The MQTT operation with the packet to send.
 */
static void _invokeSend( _mqttOperation_t * const pOperation );

/**
 * @brief Invoke user-provided callback functions.
 *
 * Invokes callback functions associated with either a completed MQTT operation
 * or an incoming PUBLISH message.
 *
 * @param[in] pOperation The MQTT operation with the callback to invoke.
 */
static void _invokeCallback( _mqttOperation_t * const pOperation );

/**
 * @brief Process an MQTT operation.
 *
 * This function is a thread routine that either sends an MQTT packet for
 * in-progress operations, notifies of an operation's completion, or notifies
 * of an incoming PUBLISH message.
 *
 * @param[in] pArgument The address of either #_IotMqttCallback (to invoke a user
 * callback) or #_IotMqttSend (to send an MQTT packet).
 */
static void _processOperation( void * pArgument );

/*-----------------------------------------------------------*/

/**
 * Queues of MQTT operations waiting to be processed.
 */
_mqttOperationQueue_t _IotMqttCallback = { 0 }; /**< @brief Queue of MQTT operations waiting for their callback to be invoked. */
_mqttOperationQueue_t _IotMqttSend = { 0 };     /**< @brief Queue of MQTT operations waiting to be sent. */
AwsIotMutex_t _IotMqttQueueMutex;               /**< @brief Protects both #_IotMqttSend and #_IotMqttCallback from concurrent access. */

/* List of MQTT operations awaiting a network response. */
IotListDouble_t _IotMqttPendingResponse = { 0 }; /**< @brief List of MQTT operations awaiting a response from the MQTT server. */
AwsIotMutex_t _IotMqttPendingResponseMutex;      /**< @brief Protects #_IotMqttPendingResponse from concurrent access. */

/*-----------------------------------------------------------*/

static bool _mqttOperation_match( const IotLink_t * pOperationLink,
                                  void * pMatch )
{
    bool match = false;
    _mqttOperation_t * pOperation = IotLink_Container( _mqttOperation_t,
                                                       pOperationLink,
                                                       link );
    _operationMatchParam_t * pParam = ( _operationMatchParam_t * ) pMatch;

    /* Check for matching operations. */
    if( pParam->operation == pOperation->operation )
    {
        /* Check for matching packet identifiers. */
        if( pParam->pPacketIdentifier == NULL )
        {
            match = true;
        }
        else
        {
            match = ( *( pParam->pPacketIdentifier ) == pOperation->packetIdentifier );
        }
    }

    return match;
}

/*-----------------------------------------------------------*/

static bool _calculateNextPublishRetry( _mqttConnection_t * const pMqttConnection,
                                        _mqttTimerEvent_t * const pPublishRetry )
{
    bool status = true;
    _mqttTimerEvent_t * pTimerListHead = NULL;

    /* Check arguments. */
    AwsIotMqtt_Assert( pPublishRetry->pOperation->operation == AWS_IOT_MQTT_PUBLISH_TO_SERVER );
    AwsIotMqtt_Assert( pPublishRetry->retry.limit > 0 );
    AwsIotMqtt_Assert( pPublishRetry->retry.nextPeriod <= AWS_IOT_MQTT_RETRY_MS_CEILING );

    /* Increment the number of retries. */
    pPublishRetry->retry.count++;

    /* Calculate when the PUBLISH retry event should expire relative to the current
     * time. */
    pPublishRetry->expirationTime = AwsIotClock_GetTimeMs();

    if( pPublishRetry->retry.count > pPublishRetry->retry.limit )
    {
        /* Retry limit reached. Check for a response shortly. */
        pPublishRetry->expirationTime += AWS_IOT_MQTT_RESPONSE_WAIT_MS;
    }
    else
    {
        /* Expire at the next retry period. */
        pPublishRetry->expirationTime += pPublishRetry->retry.nextPeriod;

        /* Calculate the next retry period. PUBLISH retries use a truncated exponential
         * backoff strategy. */
        if( pPublishRetry->retry.nextPeriod < AWS_IOT_MQTT_RETRY_MS_CEILING )
        {
            pPublishRetry->retry.nextPeriod *= 2;

            if( pPublishRetry->retry.nextPeriod > AWS_IOT_MQTT_RETRY_MS_CEILING )
            {
                pPublishRetry->retry.nextPeriod = AWS_IOT_MQTT_RETRY_MS_CEILING;
            }
        }
    }

    /* Lock the connection timer mutex to block the timer thread. */
    AwsIotMutex_Lock( &( pMqttConnection->timerMutex ) );

    /* Peek the current head of the timer event list. If the PUBLISH retry expires
     * sooner, re-arm the timer to expire at the PUBLISH retry's expiration time. */
    pTimerListHead = IotLink_Container( _mqttTimerEvent_t,
                                        IotListDouble_PeekHead( &( pMqttConnection->timerEventList ) ),
                                        link );

    if( ( pTimerListHead == NULL ) ||
        ( pTimerListHead->expirationTime > pPublishRetry->expirationTime ) )
    {
        status = AwsIotClock_TimerArm( &( pMqttConnection->timer ),
                                       pPublishRetry->expirationTime - AwsIotClock_GetTimeMs(),
                                       0 );

        if( status == false )
        {
            AwsIotLogError( "Failed to re-arm timer for connection %p.", pMqttConnection );
        }
    }

    /* Insert the PUBLISH retry into the timer event list only if the timer
     * is guaranteed to expire before its expiration time. */
    if( status == true )
    {
        IotListDouble_InsertSorted( &( pMqttConnection->timerEventList ),
                                    &( pPublishRetry->link ),
                                    AwsIotMqttInternal_TimerEventCompare );
    }

    AwsIotMutex_Unlock( &( pMqttConnection->timerMutex ) );

    return status;
}

/*-----------------------------------------------------------*/

static void _invokeSend( _mqttOperation_t * const pOperation )
{
    bool waitableOperation = false;

    AwsIotLogDebug( "Operation %s received from queue. Sending data over network.",
                    AwsIotMqtt_OperationType( pOperation->operation ) );

    /* Check if this is a waitable operation. */
    waitableOperation = ( ( pOperation->flags & AWS_IOT_MQTT_FLAG_WAITABLE )
                          == AWS_IOT_MQTT_FLAG_WAITABLE );

    /* Transmit the MQTT packet from the operation over the network. */
    if( pOperation->pMqttConnection->network.send( pOperation->pMqttConnection->network.pSendContext,
                                                   pOperation->pMqttPacket,
                                                   pOperation->packetSize ) != pOperation->packetSize )
    {
        /* Transmission failed. */
        AwsIotLogError( "Failed to send data for operation %s.",
                        AwsIotMqtt_OperationType( pOperation->operation ) );

        pOperation->status = AWS_IOT_MQTT_SEND_ERROR;
    }
    else
    {
        /* DISCONNECT operations are considered successful upon successful
         * transmission. */
        if( pOperation->operation == AWS_IOT_MQTT_DISCONNECT )
        {
            pOperation->status = AWS_IOT_MQTT_SUCCESS;
        }
        /* Calculate the details of the next PUBLISH retry event. */
        else if( pOperation->pPublishRetry != NULL )
        {
            /* Only a PUBLISH may have a retry event. */
            AwsIotMqtt_Assert( pOperation->operation == AWS_IOT_MQTT_PUBLISH_TO_SERVER );

            if( _calculateNextPublishRetry( pOperation->pMqttConnection,
                                            pOperation->pPublishRetry ) == false )
            {
                /* PUBLISH retry failed to re-arm connection timer. Retransmission
                 * will not be sent. */
                pOperation->status = AWS_IOT_MQTT_SEND_ERROR;
            }
        }
    }

    /* Once the MQTT packet has been sent, it may be freed if it will not be
     * retransmitted and it's not a PINGREQ. Additionally, the entire operation
     * may be destroyed if no notification method is set. */
    if( ( pOperation->pPublishRetry == NULL ) &&
        ( pOperation->operation != AWS_IOT_MQTT_PINGREQ ) )
    {
        /* Choose a free packet function. */
        void ( * freePacket )( uint8_t * ) = AwsIotMqttInternal_FreePacket;

        #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pOperation->pMqttConnection->network.freePacket != NULL )
            {
                freePacket = pOperation->pMqttConnection->network.freePacket;
            }
        #endif

        freePacket( pOperation->pMqttPacket );
        pOperation->pMqttPacket = NULL;

        if( ( waitableOperation == false ) &&
            ( pOperation->notify.callback.function == NULL ) )
        {
            AwsIotMqttInternal_DestroyOperation( pOperation );

            return;
        }
    }

    /* If a status was set by this function, notify of completion. */
    if( pOperation->status != AWS_IOT_MQTT_STATUS_PENDING )
    {
        AwsIotMqttInternal_Notify( pOperation );
    }
    /* Otherwise, add operation to the list of MQTT operations pending responses. */
    else
    {
        AwsIotMutex_Lock( &( _IotMqttPendingResponseMutex ) );
        IotListDouble_InsertTail( &( _IotMqttPendingResponse ), &( pOperation->link ) );
        AwsIotMutex_Unlock( &( _IotMqttPendingResponseMutex ) );
    }

    /* Notify a waitable operation that it has been sent. */
    if( waitableOperation == true )
    {
        AwsIotSemaphore_Post( &( pOperation->notify.waitSemaphore ) );
    }
}

/*-----------------------------------------------------------*/

static void _invokeCallback( _mqttOperation_t * const pOperation )
{
    _mqttOperation_t * i = NULL, * pCurrent = NULL;
    AwsIotMqttCallbackParam_t callbackParam = { .operation = { 0 } };

    if( pOperation->incomingPublish == true )
    {
        /* Set the pointer to the first PUBLISH. */
        i = pOperation;

        /* Process each PUBLISH in the operation. */
        while( i != NULL )
        {
            /* Save a pointer to the current PUBLISH and move the iterating
             * pointer. */
            pCurrent = i;
            i = i->pNextPublish;

            /* Process the current PUBLISH. */
            if( ( pCurrent->publishInfo.pPayload != NULL ) &&
                ( pCurrent->publishInfo.pTopicName != NULL ) )
            {
                callbackParam.message.info = pCurrent->publishInfo;

                AwsIotMqttInternal_ProcessPublish( pCurrent->pMqttConnection,
                                                   &callbackParam );
            }

            /* Free any buffers associated with the current PUBLISH message. */
            if( pCurrent->freeReceivedData != NULL )
            {
                AwsIotMqtt_Assert( pCurrent->pReceivedData != NULL );
                pCurrent->freeReceivedData( ( void * ) pCurrent->pReceivedData );
            }

            /* Free the current PUBLISH. */
            AwsIotMqtt_FreeOperation( pCurrent );
        }
    }
    else
    {
        /* Operations with no callback should not have been passed. */
        AwsIotMqtt_Assert( pOperation->notify.callback.function != NULL );

        AwsIotLogDebug( "Operation %s received from queue. Invoking user callback.",
                        AwsIotMqtt_OperationType( pOperation->operation ) );

        /* Set callback parameters. */
        callbackParam.mqttConnection = pOperation->pMqttConnection;
        callbackParam.operation.type = pOperation->operation;
        callbackParam.operation.reference = pOperation;
        callbackParam.operation.result = pOperation->status;

        /* Invoke user callback function. */
        pOperation->notify.callback.function( pOperation->notify.callback.param1,
                                              &callbackParam );

        /* Once the user-provided callback returns, the operation can be destroyed. */
        AwsIotMqttInternal_DestroyOperation( pOperation );
    }
}

/*-----------------------------------------------------------*/

static void _processOperation( void * pArgument )
{
    _mqttOperation_t * pOperation = NULL;
    _mqttOperationQueue_t * pQueue = ( _mqttOperationQueue_t * ) pArgument;

    /* There are only two valid values for this function's argument. */
    AwsIotMqtt_Assert( ( pQueue == &( _IotMqttCallback ) ) ||
                       ( pQueue == &( _IotMqttSend ) ) );

    AwsIotLogDebug( "New thread for processing MQTT operations started." );

    while( true )
    {
        AwsIotLogDebug( "Removing oldest operation from MQTT pending operations." );

        /* Remove the oldest operation from the queue of pending MQTT operations. */
        AwsIotMutex_Lock( &( _IotMqttQueueMutex ) );

        pOperation = IotLink_Container( _mqttOperation_t,
                                        IotQueue_Dequeue( &( pQueue->queue ) ),
                                        link );

        /* If no operation was received, this thread will terminate. Increment
         * number of available threads. */
        if( pOperation == NULL )
        {
            AwsIotSemaphore_Post( &( pQueue->availableThreads ) );
        }

        AwsIotMutex_Unlock( &( _IotMqttQueueMutex ) );

        /* Terminate thread if no operation was received. */
        if( pOperation == NULL )
        {
            break;
        }

        /* Run thread routine based on given queue. */
        if( pQueue == &( _IotMqttCallback ) )
        {
            /* Only incoming PUBLISH messages and completed operations should invoke
             * the callback routine. */
            AwsIotMqtt_Assert( ( pOperation->incomingPublish == true ) ||
                               ( pOperation->status != AWS_IOT_MQTT_STATUS_PENDING ) );

            _invokeCallback( pOperation );
        }
        else
        {
            /* Only operations with an allocated packet should be sent. */
            AwsIotMqtt_Assert( pOperation->status == AWS_IOT_MQTT_STATUS_PENDING );
            AwsIotMqtt_Assert( pOperation->pMqttPacket != NULL );
            AwsIotMqtt_Assert( pOperation->packetSize != 0 );

            _invokeSend( pOperation );
        }
    }

    AwsIotLogDebug( "No more pending operations. Terminating MQTT processing thread." );
}

/*-----------------------------------------------------------*/

int AwsIotMqttInternal_TimerEventCompare( const IotLink_t * const pTimerEventLink1,
                                          const IotLink_t * const pTimerEventLink2 )
{
    const _mqttTimerEvent_t * pTimerEvent1 = IotLink_Container( _mqttTimerEvent_t,
                                                                pTimerEventLink1,
                                                                link );
    const _mqttTimerEvent_t * pTimerEvent2 = IotLink_Container( _mqttTimerEvent_t,
                                                                pTimerEventLink2,
                                                                link );

    if( pTimerEvent1->expirationTime < pTimerEvent2->expirationTime )
    {
        return -1;
    }

    if( pTimerEvent1->expirationTime > pTimerEvent2->expirationTime )
    {
        return 1;
    }

    return 0;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_CreateOperation( _mqttOperation_t ** const pNewOperation,
                                                      uint32_t flags,
                                                      const AwsIotMqttCallbackInfo_t * const pCallbackInfo )
{
    _mqttOperation_t * pOperation = NULL;

    AwsIotLogDebug( "Creating new MQTT operation record." );

    /* If the waitable flag is set, make sure that there's no callback. */
    if( ( flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE )
    {
        if( pCallbackInfo != NULL )
        {
            AwsIotLogError( "Callback should not be set for a waitable operation." );

            return AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    /* Allocate memory for a new operation. */
    pOperation = AwsIotMqtt_MallocOperation( sizeof( _mqttOperation_t ) );

    if( pOperation == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for new MQTT operation." );

        return AWS_IOT_MQTT_NO_MEMORY;
    }

    /* Clear the operation data. */
    ( void ) memset( pOperation, 0x00, sizeof( _mqttOperation_t ) );

    /* Check if the waitable flag is set. If it is, create a semaphore to
     * wait on. */
    if( ( flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE )
    {
        /* The wait semaphore counts up to 2: once for when the send thread completes,
         * and once for when the entire operation completes. */
        if( AwsIotSemaphore_Create( &( pOperation->notify.waitSemaphore ), 0, 2 ) == false )
        {
            AwsIotLogError( "Failed to create semaphore for waitable MQTT operation." );

            AwsIotMqtt_FreeOperation( pOperation );

            return AWS_IOT_MQTT_NO_MEMORY;
        }
    }
    else
    {
        /* If the waitable flag isn't set but a callback is, copy the callback
         * information. */
        if( pCallbackInfo != NULL )
        {
            pOperation->notify.callback = *pCallbackInfo;
        }
    }

    /* Initialize the flags and status members of the new operation. */
    pOperation->flags = flags;
    pOperation->status = AWS_IOT_MQTT_STATUS_PENDING;

    /* Set the output parameter. */
    *pNewOperation = pOperation;

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

void AwsIotMqttInternal_DestroyOperation( void * pData )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pData;

    AwsIotLogDebug( "Destroying operation %s.",
                    AwsIotMqtt_OperationType( pOperation->operation ) );

    /* If the MQTT packet is still allocated, free it. */
    if( pOperation->pMqttPacket != NULL )
    {
        /* Choose a free packet function. */
        void ( * freePacket )( uint8_t * ) = AwsIotMqttInternal_FreePacket;

        #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
            if( pOperation->pMqttConnection->network.freePacket != NULL )
            {
                freePacket = pOperation->pMqttConnection->network.freePacket;
            }
        #endif

        freePacket( pOperation->pMqttPacket );
    }

    /* Check if this operation is a PUBLISH. Clean up its retry event if
     * necessary. */
    if( ( pOperation->operation == AWS_IOT_MQTT_PUBLISH_TO_SERVER ) &&
        ( pOperation->pPublishRetry != NULL ) )
    {
        AwsIotMutex_Lock( &( pOperation->pMqttConnection->timerMutex ) );

        /* Remove the timer event from the timer event list before freeing it.
         * The return value of this function is not checked because it always
         * equals the publish retry event or is NULL. */
        ( void ) IotListDouble_RemoveFirstMatch( &( pOperation->pMqttConnection->timerEventList ),
                                                 NULL,
                                                 NULL,
                                                 pOperation->pPublishRetry );

        AwsIotMutex_Unlock( &( pOperation->pMqttConnection->timerMutex ) );

        AwsIotMqtt_FreeTimerEvent( pOperation->pPublishRetry );
    }

    /* Check if a wait semaphore was created for this operation. */
    if( ( pOperation->flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE )
    {
        AwsIotSemaphore_Destroy( &( pOperation->notify.waitSemaphore ) );
    }

    /* Free the memory used to hold operation data. */
    AwsIotMqtt_FreeOperation( pOperation );
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_EnqueueOperation( _mqttOperation_t * const pOperation,
                                                       _mqttOperationQueue_t * const pQueue )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_SUCCESS;

    /* The given operation must not already be queued. */
    AwsIotMqtt_Assert( IotLink_IsLinked( &( pOperation->link ) ) == false );

    /* Check that the queue argument is either the callback queue or send queue. */
    AwsIotMqtt_Assert( ( pQueue == &( _IotMqttCallback ) ) ||
                       ( pQueue == &( _IotMqttSend ) ) );

    /* Lock mutex for exclusive access to queues. */
    AwsIotMutex_Lock( &( _IotMqttQueueMutex ) );

    /* Add operation to queue. */
    IotQueue_Enqueue( &( pQueue->queue ), &( pOperation->link ) );

    /* Check if a new thread can be created. */
    if( AwsIotSemaphore_TryWait( &( pQueue->availableThreads ) ) == true )
    {
        /* Create new thread. */
        if( AwsIot_CreateDetachedThread( _processOperation,
                                         pQueue ) == false )
        {
            /* New thread could not be created. Remove enqueued operation and
             * report error. */
            IotQueue_Remove( &( pOperation->link ) );
            AwsIotSemaphore_Post( &( pQueue->availableThreads ) );
            status = AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    AwsIotMutex_Unlock( &( _IotMqttQueueMutex ) );

    return status;
}

/*-----------------------------------------------------------*/

_mqttOperation_t * AwsIotMqttInternal_FindOperation( AwsIotMqttOperationType_t operation,
                                                     const uint16_t * const pPacketIdentifier )
{
    _mqttOperation_t * pResult = NULL;
    _operationMatchParam_t param = { 0 };

    AwsIotLogDebug( "Searching for in-progress operation %s in MQTT operations pending response.",
                    AwsIotMqtt_OperationType( operation ) );

    if( pPacketIdentifier != NULL )
    {
        AwsIotLogDebug( "Searching for packet identifier %hu.", *pPacketIdentifier );
    }

    /* Set the search parameters. */
    param.operation = operation;
    param.pPacketIdentifier = pPacketIdentifier;

    /* Find the first matching element in the list. */
    AwsIotMutex_Lock( &( _IotMqttPendingResponseMutex ) );
    pResult = IotLink_Container( _mqttOperation_t,
                                 IotListDouble_RemoveFirstMatch( &( _IotMqttPendingResponse ),
                                                                 NULL,
                                                                 _mqttOperation_match,
                                                                 &param ),
                                 link );
    AwsIotMutex_Unlock( &( _IotMqttPendingResponseMutex ) );

    /* The result will be NULL if no corresponding operation was found in the
     * list. */
    if( pResult == NULL )
    {
        AwsIotLogDebug( "In-progress operation %s not found.",
                        AwsIotMqtt_OperationType( operation ) );
    }
    else
    {
        AwsIotLogDebug( "Found in-progress operation %s.",
                        AwsIotMqtt_OperationType( operation ) );
    }

    return pResult;
}

/*-----------------------------------------------------------*/

void AwsIotMqttInternal_Notify( _mqttOperation_t * const pOperation )
{
    /* Remove any lingering subscriptions if a SUBSCRIBE failed. Rejected
     * subscriptions are removed by the deserializer, so not removed here. */
    if( ( pOperation->operation == AWS_IOT_MQTT_SUBSCRIBE ) &&
        ( pOperation->status != AWS_IOT_MQTT_SUCCESS ) &&
        ( pOperation->status != AWS_IOT_MQTT_SERVER_REFUSED ) )
    {
        AwsIotMqttInternal_RemoveSubscriptionByPacket( pOperation->pMqttConnection,
                                                       pOperation->packetIdentifier,
                                                       -1 );
    }

    /* If the operation is waitable, post to its wait semaphore and return.
     * Otherwise, enqueue it for callback. */
    if( ( pOperation->flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE )
    {
        AwsIotSemaphore_Post( &( pOperation->notify.waitSemaphore ) );
    }
    else
    {
        if( pOperation->notify.callback.function != NULL )
        {
            if( AwsIotMqttInternal_EnqueueOperation( pOperation,
                                                     &( _IotMqttCallback ) ) != AWS_IOT_MQTT_SUCCESS )
            {
                AwsIotLogWarn( "Failed to create new callback thread." );
            }
        }
        else
        {
            /* Destroy the operation if no callback was set. */
            AwsIotMqttInternal_DestroyOperation( pOperation );
        }
    }
}

/*-----------------------------------------------------------*/
