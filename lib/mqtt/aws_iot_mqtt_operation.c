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
 * @param[in] pArgument Pointer to an #_operationMatchParam_t.
 * @param[in] pData Pointer to an #_mqttOperation_t.
 *
 * @return `true` if `pData` matches the parameters in `pArgument`; `false`
 * otherwise.
 *
 * @note The arguments of this function are of type `void*` for compatibility
 * with @ref queue_function_removefirstmatch.
 */
static inline bool _mqttOperation_match( void * pArgument,
                                         void * pData );

/**
 * @brief Send data over the network.
 *
 * Whenever an operation is added to the send queue, a send thread is created.
 * This thread processes operations from the send queue until it is empty. The
 * number of concurrent send threads is limited by @ref AWS_IOT_MQTT_MAX_SEND_THREADS.
 *
 * @param[in] pArgument Not used.
 */
static void _sendThread( void * pArgument );

/**
 * @brief Invoke user-provided callback functions.
 *
 * Callback functions are invoked in their own thread so they don't block the
 * MQTT library's threads. The number of concurrent callback threads is limited
 * by @ref AWS_IOT_MQTT_MAX_CALLBACK_THREADS.
 *
 * @param[in] pArgument Not used.
 */
static void _callbackThread( void * pArgument );

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

/*-----------------------------------------------------------*/

/*
 * MQTT operation queues.
 */
AwsIotQueue_t _AwsIotMqttSendQueue = { 0 };     /**< @brief Queues data to be sent on the network. */
AwsIotQueue_t _AwsIotMqttReceiveQueue = { 0 };  /**< @brief Queues responses received from the network. */
AwsIotQueue_t _AwsIotMqttCallbackQueue = { 0 }; /**< @brief Queues pending user callbacks. */

/*-----------------------------------------------------------*/

static inline bool _mqttOperation_match( void * pArgument,
                                         void * pData )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pData;
    _operationMatchParam_t * pParam = ( _operationMatchParam_t * ) pArgument;

    if( pParam->operation == pOperation->operation )
    {
        if( pParam->pPacketIdentifier == NULL )
        {
            return true;
        }
        else
        {
            return *( pParam->pPacketIdentifier ) == pOperation->packetIdentifier;
        }
    }

    return false;
}

/*-----------------------------------------------------------*/

static void _sendThread( void * pArgument )
{
    bool waitableOperation = false;
    _mqttOperation_t * pOperation = NULL;

    /* Silence warnings about unused parameters. */
    ( void ) pArgument;

    AwsIotLogDebug( "New send thread started." );

    /* Receive messages from the send queue until the queue is empty. */
    while( true )
    {
        AwsIotLogDebug( "Removing oldest operation from MQTT send queue." );

        /* Get the oldest operation in the MQTT send queue. */
        pOperation = AwsIotQueue_RemoveTail( &_AwsIotMqttSendQueue,
                                             _OPERATION_LINK_OFFSET,
                                             true );

        /* If no operation was received, terminate the loop. */
        if( pOperation == NULL )
        {
            break;
        }

        AwsIotLogDebug( "Operation %s received from queue. Sending data over network.",
                        AwsIotMqtt_OperationType( pOperation->operation ) );

        /* The received operation should be waiting for a status and have a valid packet. */
        AwsIotMqtt_Assert( pOperation->status == AWS_IOT_MQTT_STATUS_PENDING );
        AwsIotMqtt_Assert( pOperation->pMqttPacket != NULL );
        AwsIotMqtt_Assert( pOperation->packetSize != 0 );

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
                continue;
            }
        }

        /* If a status was set by this function, notify of completion. */
        if( pOperation->status != AWS_IOT_MQTT_STATUS_PENDING )
        {
            AwsIotMqttInternal_Notify( pOperation );
        }
        /* Otherwise, add the operation to the receive queue. */
        else
        {
            /* This function call will not fail because the receive queue has no
             * notification routine. */
            ( void ) AwsIotQueue_InsertHead( &_AwsIotMqttReceiveQueue,
                                             &( pOperation->link ) );
        }

        /* Notify a waitable operation that the send thread is finished. */
        if( waitableOperation == true )
        {
            AwsIotSemaphore_Post( &( pOperation->notify.waitSemaphore ) );
        }
    }

    AwsIotLogDebug( "Send queue empty. Send thread terminating." );
}

/*-----------------------------------------------------------*/

static void _callbackThread( void * pArgument )
{
    _mqttOperation_t * pOperation = NULL, * i = NULL, * pCurrent = NULL;
    AwsIotMqttCallbackParam_t callbackParam = { .operation = { 0 } };

    /* Silence warnings about unused parameters. */
    ( void ) pArgument;

    AwsIotLogDebug( "New callback thread started." );

    while( true )
    {
        AwsIotLogDebug( "Removing oldest operation from MQTT callback queue." );

        /* Get the oldest operation in the MQTT callback queue. */
        pOperation = AwsIotQueue_RemoveTail( &_AwsIotMqttCallbackQueue,
                                             _OPERATION_LINK_OFFSET,
                                             true );

        /* If no operation was received, terminate the loop. */
        if( pOperation == NULL )
        {
            break;
        }

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
            /* Operations with no callback should not have been passed to the
             * callback queue. */
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

    AwsIotLogDebug( "Callback queue empty. Callback thread terminating." );
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

    AwsIotMutex_Lock( &( pMqttConnection->timerEventList.mutex ) );

    /* Peek the current head of the timer event list. If the PUBLISH retry expires
     * sooner, re-arm the timer to expire at the PUBLISH retry's expiration time. */
    pTimerListHead = AwsIotLink_Container( pMqttConnection->timerEventList.pHead,
                                           _TIMER_EVENT_LINK_OFFSET );

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
        AwsIotList_InsertSorted( &( pMqttConnection->timerEventList ),
                                 &( pPublishRetry->link ),
                                 _TIMER_EVENT_LINK_OFFSET,
                                 AwsIotMqttInternal_TimerEventCompare );
    }

    AwsIotMutex_Unlock( &( pMqttConnection->timerEventList.mutex ) );

    return status;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_CreateQueues( void )
{
    AwsIotQueueNotifyParams_t notifyParams = { 0 };

    /* Create the send queue. */
    notifyParams.notifyRoutine = _sendThread;

    if( AwsIotQueue_Create( &_AwsIotMqttSendQueue,
                            &notifyParams,
                            AWS_IOT_MQTT_MAX_SEND_THREADS ) == false )
    {
        return AWS_IOT_MQTT_INIT_FAILED;
    }

    /* Create the receive queue. */
    if( AwsIotQueue_Create( &_AwsIotMqttReceiveQueue,
                            NULL,
                            0 ) == false )
    {
        AwsIotQueue_Destroy( &_AwsIotMqttSendQueue );

        return AWS_IOT_MQTT_INIT_FAILED;
    }

    /* Create the callback queue. */
    notifyParams.notifyRoutine = _callbackThread;

    if( AwsIotQueue_Create( &_AwsIotMqttCallbackQueue,
                            &notifyParams,
                            AWS_IOT_MQTT_MAX_CALLBACK_THREADS ) == false )
    {
        AwsIotQueue_Destroy( &_AwsIotMqttSendQueue );
        AwsIotQueue_Destroy( &_AwsIotMqttReceiveQueue );

        return AWS_IOT_MQTT_INIT_FAILED;
    }

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

int AwsIotMqttInternal_TimerEventCompare( void * pData1,
                                          void * pData2 )
{
    _mqttTimerEvent_t * pTimerEvent1 = ( _mqttTimerEvent_t * ) pData1,
        *pTimerEvent2 = ( _mqttTimerEvent_t * ) pData2;

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
        AwsIotMutex_Lock( &( pOperation->pMqttConnection->timerEventList.mutex ) );

        /* Remove the timer event from the timer event list before freeing it. */
        if( AwsIotList_FindFirstMatch( pOperation->pMqttConnection->timerEventList.pHead,
                                       _TIMER_EVENT_LINK_OFFSET,
                                       pOperation->pPublishRetry,
                                       NULL ) != NULL )
        {
            AwsIotList_Remove( &( pOperation->pMqttConnection->timerEventList ),
                               &( pOperation->pPublishRetry->link ),
                               _TIMER_EVENT_LINK_OFFSET );
        }

        AwsIotMutex_Unlock( &( pOperation->pMqttConnection->timerEventList.mutex ) );

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

void AwsIotMqttInternal_Notify( _mqttOperation_t * const pOperation )
{
    /* If the operation is waiting, post to its wait semaphore and return. */
    if( ( pOperation->flags & AWS_IOT_MQTT_FLAG_WAITABLE ) == AWS_IOT_MQTT_FLAG_WAITABLE )
    {
        AwsIotSemaphore_Post( &( pOperation->notify.waitSemaphore ) );

        return;
    }

    /* Add the operation to the callback queue if a callback was set. */
    if( pOperation->notify.callback.function != NULL )
    {
        if( AwsIotQueue_InsertHead( &_AwsIotMqttCallbackQueue,
                                    &( pOperation->link ) ) != true )
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

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_OperationFind( AwsIotQueue_t * const pQueue,
                                                    AwsIotMqttOperationType_t operation,
                                                    const uint16_t * const pPacketIdentifier,
                                                    _mqttOperation_t ** const pOutput )
{
    _mqttOperation_t * pResult = NULL;
    _operationMatchParam_t param = { 0 };

    AwsIotLogDebug( "Searching for in-progress operation %s in MQTT receive queue.",
                    AwsIotMqtt_OperationType( operation ) );

    if( pPacketIdentifier != NULL )
    {
        AwsIotLogDebug( "Searching for packet identifier %hu.", *pPacketIdentifier );
    }

    /* Set the search parameters. */
    param.operation = operation;
    param.pPacketIdentifier = pPacketIdentifier;

    /* Find the first matching element in the queue. */
    pResult = AwsIotQueue_RemoveFirstMatch( pQueue,
                                            _OPERATION_LINK_OFFSET,
                                            &param,
                                            _mqttOperation_match );

    /* The result will be NULL if no corresponding operation was found in the
     * queue. */
    if( pResult == NULL )
    {
        AwsIotLogDebug( "In-progress operation %s not found.",
                        AwsIotMqtt_OperationType( operation ) );

        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    /* If a corresponding operation was found, set the output parameter. */
    *pOutput = pResult;

    AwsIotLogDebug( "Found in-progress operation %s.",
                    AwsIotMqtt_OperationType( operation ) );

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/
