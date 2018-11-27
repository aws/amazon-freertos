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
 * @file aws_iot_mqtt_queue_access.c
 * @brief Implements access and initialization functions for the MQTT library's
 * queues and lists.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/*-----------------------------------------------------------*/

/**
 * @brief Get the value of #_mqttOperation_t.pNext.
 *
 * @param[in] pPointer Pointer to an #_mqttOperation_t.
 *
 * @return Value of the pPointer's `pNext` pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotQueueParams_t.
 */
static inline void * _mqttOperation_getNext( void * pPointer );

/**
 * @brief Get the value of #_mqttOperation_t.pPrevious.
 *
 * @param[in] pPointer Pointer to an #_mqttOperation_t.
 *
 * @return Value of the pPointer's `pPrevious` pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotQueueParams_t.
 */
static inline void * _mqttOperation_getPrev( void * pPointer );

/**
 * @brief Set #_mqttOperation_t.pNext.
 *
 * @param[in] pPointer Pointer to an #_mqttOperation_t.
 * @param[in] pValue Value to assign to the next pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotQueueParams_t.
 */
static inline void _mqttOperation_setNext( void * pPointer,
                                           void * pValue );

/**
 * @brief Set #_mqttOperation_t.pPrevious.
 *
 * @param[in] pPointer Pointer to an #_mqttOperation_t.
 * @param[in] pValue Value to assign to the previous pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotQueueParams_t.
 */
static inline void _mqttOperation_setPrev( void * pPointer,
                                           void * pValue );

/**
 * @brief Get the value of #_mqttSubscription_t.pNext.
 *
 * @param[in] pPointer Pointer to a #_mqttSubscription_t.
 *
 * @return Value of the pPointer's `pNext` pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void * _mqttSubscription_getNext( void * pPointer );

/**
 * @brief Get the value of #_mqttSubscription_t.pPrevious.
 *
 * @param[in] pPointer Pointer to a #_mqttSubscription_t.
 *
 * @return Value of the pPointer's `pPrevious` pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void * _mqttSubscription_getPrev( void * pPointer );

/**
 * @brief Set #_mqttSubscription_t.pNext.
 *
 * @param[in] pPointer Pointer to a #_mqttSubscription_t.
 * @param[in] pValue Value to assign to the next pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void _mqttSubscription_setNext( void * pPointer,
                                              void * pValue );

/**
 * @brief Set #_mqttSubscription_t.pPrevious.
 *
 * @param[in] pPointer Pointer to a #_mqttSubscription_t.
 * @param[in] pValue Value to assign to the previous pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void _mqttSubscription_setPrev( void * pPointer,
                                              void * pValue );

/**
 * @brief Get the value of #_mqttTimerEvent_t.pNext.
 *
 * @param[in] pPointer Pointer to a #_mqttTimerEvent_t.
 *
 * @return Value of the pPointer's `pNext` pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void * _mqttTimerEvent_getNext( void * pPointer );

/**
 * @brief Get the value of #_mqttTimerEvent_t.pPrevious.
 *
 * @param[in] pPointer Pointer to a #_mqttTimerEvent_t.
 *
 * @return Value of the pPointer's `pPrevious` pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void * _mqttTimerEvent_getPrev( void * pPointer );

/**
 * @brief Set #_mqttTimerEvent_t.pNext.
 *
 * @param[in] pPointer Pointer to a #_mqttTimerEvent_t.
 * @param[in] pValue Value to assign to the next pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void _mqttTimerEvent_setNext( void * pPointer,
                                            void * pValue );

/**
 * @brief Set #_mqttTimerEvent_t.pPrevious.
 *
 * @param[in] pPointer Pointer to a #_mqttTimerEvent_t.
 * @param[in] pValue Value to assign to the previous pointer.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * an #AwsIotListParams_t.
 */
static inline void _mqttTimerEvent_setPrev( void * pPointer,
                                            void * pValue );

/**
 * @brief Compare two #_mqttTimerEvent_t by expiration time.
 *
 * @param[in] pData1 The first #_mqttTimerEvent_t to compare.
 * @param[in] pData2 The second #_mqttTimerEvent_t to compare.
 *
 * @return
 * - Negative value if `pData1` is less than `pData2`.
 * - Zero if `pData1` is equal to `pData2`.
 * - Positive value if `pData1` is greater than `pData2`.
 *
 * @note The arguments of this function are of type `void*` for compatibility with
 * @ref list_function_insertsorted.
 */
static inline int _mqttTimerEvent_compare( void * pData1,
                                           void * pData2 );

/*-----------------------------------------------------------*/

static inline void * _mqttSubscription_getNext( void * pPointer )
{
    _mqttSubscription_t * pSubscription = ( _mqttSubscription_t * ) pPointer;

    return ( void * ) ( pSubscription->pNext );
}

/*-----------------------------------------------------------*/

static inline void * _mqttSubscription_getPrev( void * pPointer )
{
    _mqttSubscription_t * pSubscription = ( _mqttSubscription_t * ) pPointer;

    return ( void * ) ( pSubscription->pPrevious );
}

/*-----------------------------------------------------------*/

static inline void _mqttSubscription_setNext( void * pPointer,
                                              void * pValue )
{
    _mqttSubscription_t * pSubscription = ( _mqttSubscription_t * ) pPointer;

    pSubscription->pNext = ( _mqttSubscription_t * ) pValue;
}

/*-----------------------------------------------------------*/

static inline void _mqttSubscription_setPrev( void * pPointer,
                                              void * pValue )
{
    _mqttSubscription_t * pSubscription = ( _mqttSubscription_t * ) pPointer;

    pSubscription->pPrevious = ( _mqttSubscription_t * ) pValue;
}

/*-----------------------------------------------------------*/

static inline void * _mqttOperation_getNext( void * pPointer )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pPointer;

    return ( void * ) ( pOperation->pNext );
}

/*-----------------------------------------------------------*/

static inline void * _mqttOperation_getPrev( void * pPointer )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pPointer;

    return ( void * ) ( pOperation->pPrevious );
}

/*-----------------------------------------------------------*/

static inline void _mqttOperation_setNext( void * pPointer,
                                           void * pValue )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pPointer;

    pOperation->pNext = ( _mqttOperation_t * ) pValue;
}

/*-----------------------------------------------------------*/

static inline void _mqttOperation_setPrev( void * pPointer,
                                           void * pValue )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pPointer;

    pOperation->pPrevious = ( _mqttOperation_t * ) pValue;
}

/*-----------------------------------------------------------*/

static inline void * _mqttTimerEvent_getNext( void * pPointer )
{
    _mqttTimerEvent_t * pTimerEvent = ( _mqttTimerEvent_t * ) pPointer;

    return ( void * ) ( pTimerEvent->pNext );
}

/*-----------------------------------------------------------*/

static inline void * _mqttTimerEvent_getPrev( void * pPointer )
{
    _mqttTimerEvent_t * pTimerEvent = ( _mqttTimerEvent_t * ) pPointer;

    return ( void * ) ( pTimerEvent->pPrevious );
}

/*-----------------------------------------------------------*/

static inline void _mqttTimerEvent_setNext( void * pPointer,
                                            void * pValue )
{
    _mqttTimerEvent_t * pTimerEvent = ( _mqttTimerEvent_t * ) pPointer;

    pTimerEvent->pNext = ( _mqttTimerEvent_t * ) pValue;
}

/*-----------------------------------------------------------*/

static inline void _mqttTimerEvent_setPrev( void * pPointer,
                                            void * pValue )
{
    _mqttTimerEvent_t * pTimerEvent = ( _mqttTimerEvent_t * ) pPointer;

    pTimerEvent->pPrevious = ( _mqttTimerEvent_t * ) pValue;
}

/*-----------------------------------------------------------*/

static inline int _mqttTimerEvent_compare( void * pData1,
                                           void * pData2 )
{
    _mqttTimerEvent_t * pTimerEvent1 = ( _mqttTimerEvent_t * ) pData1,
                      * pTimerEvent2 = ( _mqttTimerEvent_t * ) pData2;

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

AwsIotMqttError_t AwsIotMqttInternal_CreateOperationQueue( AwsIotQueue_t * const pQueue,
                                                           AwsIotThreadRoutine_t notifyRoutine,
                                                           uint32_t maxNotifyThreads )
{
    const AwsIotQueueParams_t queueParams =
    {
        .notifyRoutine = notifyRoutine,
        .getNext       = _mqttOperation_getNext,
        .getPrev       = _mqttOperation_getPrev,
        .setNext       = _mqttOperation_setNext,
        .setPrev       = _mqttOperation_setPrev
    };

    if( AwsIotQueue_Create( pQueue, &queueParams, maxNotifyThreads ) == false )
    {
        return AWS_IOT_MQTT_INIT_FAILED;
    }

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_CreateSubscriptionList( AwsIotList_t * const pList )
{
    const AwsIotListParams_t listParams =
    {
        .getNext = _mqttSubscription_getNext,
        .getPrev = _mqttSubscription_getPrev,
        .setNext = _mqttSubscription_setNext,
        .setPrev = _mqttSubscription_setPrev
    };

    if( AwsIotList_Create( pList, &listParams ) == false )
    {
        return AWS_IOT_MQTT_INIT_FAILED;
    }

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_CreateTimerEventList( AwsIotList_t * const pList )
{
    const AwsIotListParams_t listParams =
    {
        .getNext = _mqttTimerEvent_getNext,
        .getPrev = _mqttTimerEvent_getPrev,
        .setNext = _mqttTimerEvent_setNext,
        .setPrev = _mqttTimerEvent_setPrev
    };

    if( AwsIotList_Create( pList, &listParams ) == false )
    {
        return AWS_IOT_MQTT_INIT_FAILED;
    }

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

void AwsIotMqttInternal_TimerListInsert( AwsIotList_t * const pTimerList,
                                         _mqttTimerEvent_t * const pTimerEvent )
{
    AwsIotList_InsertSorted( pTimerList,
                             pTimerEvent,
                             _mqttTimerEvent_compare );
}

/*-----------------------------------------------------------*/
