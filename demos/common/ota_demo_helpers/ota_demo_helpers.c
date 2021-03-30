/*
 * AWS IoT Device SDK for Embedded C V202009.00
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
 */

/* Standard includes. */
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Include header for OTA demo helper functions. */
#include "ota_demo_helpers.h"

#define SEMAPHORE_NOT_INITIALIZED    ( 0U )
#define SEMAPHORE_INIT_PENDING       ( 1U )
#define SEMAPHORE_INITIALIZED        ( 2U )

/*-----------------------------------------------------------*/

/**
 * @brief Represents a registered record of the topic filter and its associated callback
 * in the subscription manager registry.
 */
typedef struct SubscriptionManagerRecord
{
    const char * pTopicFilter;
    uint16_t topicFilterLength;
    SubscriptionManagerCallback_t callback;
} SubscriptionManagerRecord_t;

/**
 * @brief The default value for the maximum size of the callback registry in the
 * subscription manager.
 */
#ifndef MAX_SUBSCRIPTION_CALLBACK_RECORDS
    #define MAX_SUBSCRIPTION_CALLBACK_RECORDS    5
#endif

/**
 * @brief The registry to store records of topic filters and their subscription callbacks.
 */
static SubscriptionManagerRecord_t callbackRecordList[ MAX_SUBSCRIPTION_CALLBACK_RECORDS ] = { 0 };

struct AgentMessageContext
{
    QueueHandle_t queue;
};

/**
 * @brief The pool of command structures used to hold information on commands (such
 * as PUBLISH or SUBSCRIBE) between the command being created by an API call and
 * completion of the command by the execution of the command's callback.
 */
static Command_t commandStructurePool[ MQTT_COMMAND_CONTEXTS_POOL_SIZE ];

/**
 * @brief A counting semaphore used to guard the pool of Command_t structures.  To
 * obtain a structure first decrement the semaphore count.  To return a structure
 * increment the semaphore count after the structure is back in the pool.
 */
static SemaphoreHandle_t freeCommandStructMutex = NULL;

static volatile uint8_t initStatus = SEMAPHORE_NOT_INITIALIZED;

/*-----------------------------------------------------------*/

void SubscriptionManager_DispatchHandler( MQTTContext_t * pContext,
                                          MQTTPublishInfo_t * pPublishInfo )
{
    bool matchStatus = false;
    size_t listIndex = 0u;

    assert( pPublishInfo != NULL );
    assert( pContext != NULL );

    /* Iterate through record list to find matching topics, and invoke their callbacks. */
    for( listIndex = 0; listIndex < MAX_SUBSCRIPTION_CALLBACK_RECORDS; listIndex++ )
    {
        if( ( callbackRecordList[ listIndex ].pTopicFilter != NULL ) &&
            ( MQTT_MatchTopic( pPublishInfo->pTopicName,
                               pPublishInfo->topicNameLength,
                               callbackRecordList[ listIndex ].pTopicFilter,
                               callbackRecordList[ listIndex ].topicFilterLength,
                               &matchStatus ) == MQTTSuccess ) &&
            ( matchStatus == true ) )
        {
            LogInfo( ( "Invoking subscription callback of matching topic filter: "
                       "TopicFilter=%.*s, TopicName=%.*s",
                       callbackRecordList[ listIndex ].topicFilterLength,
                       callbackRecordList[ listIndex ].pTopicFilter,
                       pPublishInfo->topicNameLength,
                       pPublishInfo->pTopicName ) );

            /* Invoke the callback associated with the record as the topics match. */
            callbackRecordList[ listIndex ].callback( pContext, pPublishInfo );
        }
    }
}

/*-----------------------------------------------------------*/

SubscriptionManagerStatus_t SubscriptionManager_RegisterCallback( const char * pTopicFilter,
                                                                  uint16_t topicFilterLength,
                                                                  SubscriptionManagerCallback_t callback )
{
    assert( pTopicFilter != NULL );
    assert( topicFilterLength != 0 );
    assert( callback != NULL );

    SubscriptionManagerStatus_t returnStatus;
    size_t availableIndex = MAX_SUBSCRIPTION_CALLBACK_RECORDS;
    bool recordExists = false;
    size_t index = 0u;

    /* Search for the first available spot in the list to store the record, and also check if
     * a record for the topic filter already exists. */
    while( ( recordExists == false ) && ( index < MAX_SUBSCRIPTION_CALLBACK_RECORDS ) )
    {
        /* Check if the index represents an empty spot in the registry. If we had already
         * found an empty spot in the list, we will not update it. */
        if( ( availableIndex == MAX_SUBSCRIPTION_CALLBACK_RECORDS ) &&
            ( callbackRecordList[ index ].pTopicFilter == NULL ) )
        {
            availableIndex = index;
        }

        /* Check if the current record's topic filter in the registry matches the topic filter
         * we are trying to register. */
        else if( ( callbackRecordList[ index ].topicFilterLength == topicFilterLength ) &&
                 ( strncmp( pTopicFilter, callbackRecordList[ index ].pTopicFilter, topicFilterLength )
                   == 0 ) )
        {
            recordExists = true;
        }

        index++;
    }

    if( recordExists == true )
    {
        /* The record for the topic filter already exists. */
        LogError( ( "Failed to register callback: Record for topic filter already exists: TopicFilter=%.*s",
                    topicFilterLength,
                    pTopicFilter ) );

        returnStatus = SUBSCRIPTION_MANAGER_RECORD_EXISTS;
    }
    else if( availableIndex == MAX_SUBSCRIPTION_CALLBACK_RECORDS )
    {
        /* The registry is full. */
        LogError( ( "Unable to register callback: Registry list is full: TopicFilter=%.*s, MaxRegistrySize=%u",
                    topicFilterLength,
                    pTopicFilter,
                    MAX_SUBSCRIPTION_CALLBACK_RECORDS ) );

        returnStatus = SUBSCRIPTION_MANAGER_REGISTRY_FULL;
    }
    else
    {
        callbackRecordList[ availableIndex ].pTopicFilter = pTopicFilter;
        callbackRecordList[ availableIndex ].topicFilterLength = topicFilterLength;
        callbackRecordList[ availableIndex ].callback = callback;

        returnStatus = SUBSCRIPTION_MANAGER_SUCCESS;

        LogDebug( ( "Added callback to registry: TopicFilter=%.*s",
                    topicFilterLength,
                    pTopicFilter ) );
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

void SubscriptionManager_RemoveCallback( const char * pTopicFilter,
                                         uint16_t topicFilterLength )
{
    assert( pTopicFilter != NULL );
    assert( topicFilterLength != 0 );

    size_t index;
    SubscriptionManagerRecord_t * pRecord = NULL;

    /* Iterate through the records list to find the matching record. */
    for( index = 0; index < MAX_SUBSCRIPTION_CALLBACK_RECORDS; index++ )
    {
        pRecord = &callbackRecordList[ index ];

        /* Only match the non-empty records. */
        if( pRecord->pTopicFilter != NULL )
        {
            if( ( topicFilterLength == pRecord->topicFilterLength ) &&
                ( strncmp( pTopicFilter, pRecord->pTopicFilter, topicFilterLength ) == 0 ) )
            {
                break;
            }
        }
    }

    /* Delete the record by clearing the found entry in the records list. */
    if( index < MAX_SUBSCRIPTION_CALLBACK_RECORDS )
    {
        pRecord->pTopicFilter = NULL;
        pRecord->topicFilterLength = 0u;
        pRecord->callback = NULL;

        LogDebug( ( "Deleted callback record for topic filter: TopicFilter=%.*s",
                    topicFilterLength,
                    pTopicFilter ) );
    }
    else
    {
        LogWarn( ( "Attempted to remove callback for un-registered topic filter: TopicFilter=%.*s",
                   topicFilterLength,
                   pTopicFilter ) );
    }
}
/*-----------------------------------------------------------*/

bool Agent_MessageSend( const AgentMessageContext_t * pMsgCtx,
                        const void * pData,
                        uint32_t blockTimeMs )
{
    BaseType_t queueStatus = pdFAIL;

    if( ( pMsgCtx != NULL ) && ( pData != NULL ) )
    {
        queueStatus = xQueueSendToBack( pMsgCtx->queue, pData, pdMS_TO_TICKS( blockTimeMs ) );
    }

    return ( queueStatus == pdPASS ) ? true : false;
}

/*-----------------------------------------------------------*/

bool Agent_MessageReceive( const AgentMessageContext_t * pMsgCtx,
                           void * pBuffer,
                           uint32_t blockTimeMs )
{
    BaseType_t queueStatus = pdFAIL;

    if( ( pMsgCtx != NULL ) && ( pBuffer != NULL ) )
    {
        queueStatus = xQueueReceive( pMsgCtx->queue, pBuffer, pdMS_TO_TICKS( blockTimeMs ) );
    }

    return ( queueStatus == pdPASS ) ? true : false;
}

/*-----------------------------------------------------------*/

static void initializePool()
{
    bool owner = false;

    taskENTER_CRITICAL();
    {
        if( initStatus == SEMAPHORE_NOT_INITIALIZED )
        {
            owner = true;
            initStatus = SEMAPHORE_INIT_PENDING;
        }
    }
    taskEXIT_CRITICAL();

    if( owner )
    {
        memset( ( void * ) commandStructurePool, 0x00, sizeof( commandStructurePool ) );
        freeCommandStructMutex = xSemaphoreCreateCounting( MQTT_COMMAND_CONTEXTS_POOL_SIZE, MQTT_COMMAND_CONTEXTS_POOL_SIZE );
        initStatus = SEMAPHORE_INITIALIZED;
    }
}

/*-----------------------------------------------------------*/

Command_t * Agent_GetCommand( uint32_t blockTimeMs )
{
    Command_t * structToUse = NULL;
    size_t i;

    /* Check here so we do not enter a critical section every time. */
    if( initStatus == SEMAPHORE_NOT_INITIALIZED )
    {
        initializePool();
        configASSERT( freeCommandStructMutex ); /*_RB_ Create all objects here statically. */
    }

    /* Check counting semaphore has been created. */
    if( freeCommandStructMutex != NULL )
    {
        /* If the semaphore count is not zero then a command context is available. */
        if( xSemaphoreTake( freeCommandStructMutex, pdMS_TO_TICKS( blockTimeMs ) ) == pdPASS )
        {
            for( i = 0; i < MQTT_COMMAND_CONTEXTS_POOL_SIZE; i++ )
            {
                taskENTER_CRITICAL();
                {
                    /* If the commandType is NONE then the structure is not in use. */
                    if( commandStructurePool[ i ].commandType == NONE )
                    {
                        structToUse = &( commandStructurePool[ i ] );

                        /* To show the struct is no longer available to be returned
                         * by calls to Agent_ReleaseCommand(). */
                        structToUse->commandType = !NONE;
                        taskEXIT_CRITICAL();
                        break;
                    }
                }
                taskEXIT_CRITICAL();
            }
        }
    }

    return structToUse;
}

/*-----------------------------------------------------------*/

bool Agent_ReleaseCommand( Command_t * pCommandToRelease )
{
    size_t i;
    bool structReturned = false;

    /* See if the structure being returned is actually from the pool. */
    for( i = 0; i < MQTT_COMMAND_CONTEXTS_POOL_SIZE; i++ )
    {
        if( pCommandToRelease == &( commandStructurePool[ i ] ) )
        {
            taskENTER_CRITICAL();

            /* Yes its from the pool.  Clearing it to zero not only removes the old
             * data it also sets the structure's commandType parameter to NONE to
             * mark the structure as free again. */
            memset( ( void * ) pCommandToRelease, 0x00, sizeof( Command_t ) );
            taskEXIT_CRITICAL();

            /* Give back the counting semaphore after returning the structure so the
             * semaphore count equals the number of available structures. */
            xSemaphoreGive( freeCommandStructMutex );
            structReturned = true;

            LogDebug( ( "Returned Command Context %d to pool", ( int ) i ) );

            break;
        }
    }

    return structReturned;
}

/*-----------------------------------------------------------*/