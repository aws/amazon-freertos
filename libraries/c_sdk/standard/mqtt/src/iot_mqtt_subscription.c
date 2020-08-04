/*
 * FreeRTOS MQTT V2.1.1
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_subscription.c
 * @brief Implements functions that manage subscriptions for an MQTT connection.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* Using initialized connToContext variable. */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/*-----------------------------------------------------------*/

/**
 * @brief Matches a topic name (from a publish) with a topic filter (from a
 * subscription).
 *
 * @param[in] pSubscriptionLink Pointer to the link member of an #_mqttSubscription_t.
 * @param[in] pMatch Pointer to a #_topicMatchParams_t.
 *
 * @return `true` if the arguments match the subscription topic filter; `false`
 * otherwise.
 */
static bool _topicMatch( _mqttSubscription_t pSubscription,
                         void * pMatch );

/**
 * @brief Matches a packet identifier and order.
 *
 * @param[in] pSubscriptionLink Pointer to the link member of an #_mqttSubscription_t.
 * @param[in] pMatch Pointer to a #_packetMatchParams_t.
 *
 * @return `true` if the arguments match the subscription's packet info; `false`
 * otherwise.
 */
static bool _packetMatch( _mqttSubscription_t * pSubscription,
                          void * pMatch );

/*-----------------------------------------------------------*/

static bool _topicMatch( _mqttSubscription_t pSubscription,
                         void * pMatch )
{
    IOT_FUNCTION_ENTRY( bool, false );
    uint16_t nameIndex = 0, filterIndex = 0;

    /* Because this function is called from a container function, the given link
     * must never be NULL. */
    /*IotMqtt_Assert( pSubscription != NULL ); */

    _topicMatchParams_t * pParam = ( _topicMatchParams_t * ) pMatch;

    /* Extract the relevant strings and lengths from parameters. */
    const char * pTopicName = pParam->pTopicName;
    const char * pTopicFilter = pSubscription.pTopicFilter;
    const uint16_t topicNameLength = pParam->topicNameLength;
    const uint16_t topicFilterLength = pSubscription.topicFilterLength;

    /* Check for an exact match. */
    if( topicNameLength == topicFilterLength )
    {
        status = ( strncmp( pTopicName, pTopicFilter, topicNameLength ) == 0 );

        IOT_GOTO_CLEANUP();
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* If the topic lengths are different but an exact match is required, return
     * false. */
    if( pParam->exactMatchOnly == true )
    {
        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    while( ( nameIndex < topicNameLength ) && ( filterIndex < topicFilterLength ) )
    {
        /* Check if the character in the topic name matches the corresponding
         * character in the topic filter string. */
        if( pTopicName[ nameIndex ] == pTopicFilter[ filterIndex ] )
        {
            /* Handle special corner cases as documented by the MQTT protocol spec. */

            /* Filter "sport/#" also matches "sport" since # includes the parent level. */
            if( nameIndex == topicNameLength - 1 )
            {
                if( filterIndex == topicFilterLength - 3 )
                {
                    if( pTopicFilter[ filterIndex + 1 ] == '/' )
                    {
                        if( pTopicFilter[ filterIndex + 2 ] == '#' )
                        {
                            IOT_SET_AND_GOTO_CLEANUP( true );
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
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }

            /* Filter "sport/+" also matches the "sport/" but not "sport". */
            if( nameIndex == topicNameLength - 1 )
            {
                if( filterIndex == topicFilterLength - 2 )
                {
                    if( pTopicFilter[ filterIndex + 1 ] == '+' )
                    {
                        IOT_SET_AND_GOTO_CLEANUP( true );
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
        }
        else
        {
            /* Check for wildcards. */
            if( pTopicFilter[ filterIndex ] == '+' )
            {
                /* Move topic name index to the end of the current level.
                 * This is identified by '/'. */
                while( nameIndex < topicNameLength && pTopicName[ nameIndex ] != '/' )
                {
                    nameIndex++;
                }

                /* Increment filter index to skip '/'. */
                filterIndex++;
                continue;
            }
            else if( pTopicFilter[ filterIndex ] == '#' )
            {
                /* Subsequent characters don't need to be checked if the for the
                 * multi-level wildcard. */
                IOT_SET_AND_GOTO_CLEANUP( true );
            }
            else
            {
                /* Any character mismatch other than '+' or '#' means the topic
                 * name does not match the topic filter. */
                IOT_SET_AND_GOTO_CLEANUP( false );
            }
        }

        /* Increment indexes. */
        nameIndex++;
        filterIndex++;
    }

    /* If the end of both strings has been reached, they match. */
    if( ( nameIndex == topicNameLength ) && ( filterIndex == topicFilterLength ) )
    {
        IOT_SET_AND_GOTO_CLEANUP( true );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static bool _packetMatch( _mqttSubscription_t * pSubscription,
                          void * pMatch )
{
    bool match = false;

    /* Because this function is called from a container function, the given link
     * must never be NULL. */
    IotMqtt_Assert( pSubscription != NULL );

    _packetMatchParams_t * pParam = ( _packetMatchParams_t * ) pMatch;

    /* Compare packet identifiers. */
    if( pParam->packetIdentifier == pSubscription->packetInfo.identifier )
    {
        /* Compare orders if order is not -1. */
        if( pParam->order == -1 )
        {
            match = true;
        }
        else
        {
            match = ( ( size_t ) pParam->order ) == pSubscription->packetInfo.order;
        }
    }

    /* If this subscription should be removed, check the reference count. */
    if( match == true )
    {
        /* Reference count must not be negative. */
        IotMqtt_Assert( pSubscription->references >= 0 );

        /* If the reference count is positive, this subscription cannot be
         * removed yet because there are subscription callbacks using it. */
        if( pSubscription->references > 0 )
        {
            match = false;

            /* Set the unsubscribed flag. The last active subscription callback
             * will remove and clean up this subscription. */
            pSubscription->unsubscribed = true;
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

    return match;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_AddSubscriptions( _mqttConnection_t * pMqttConnection,
                                          uint16_t subscribePacketIdentifier,
                                          const IotMqttSubscription_t * pSubscriptionList,
                                          size_t subscriptionCount )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t i = 0;
    _mqttSubscription_t * pNewSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _topicMatchParams_t topicMatchParams = { .exactMatchOnly = true };
    int8_t contextIndex = -1;
    int8_t matchedIndex = -1;
    int8_t index = -1;

    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ), portMAX_DELAY ) == pdTRUE )
    {
        for( i = 0; i < subscriptionCount; i++ )
        {
            /* Check if this topic filter is already registered. */
            topicMatchParams.pTopicName = pSubscriptionList[ i ].pTopicFilter;
            topicMatchParams.topicNameLength = pSubscriptionList[ i ].topicFilterLength;

            if( contextIndex >= 0 )
            {
                matchedIndex = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray ), 0, &topicMatchParams );
            }

            if( matchedIndex >= 0 )
            {
                /* The lengths of exactly matching topic filters must match. */
                IotMqtt_Assert( connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].topicFilterLength == pSubscriptionList[ i ].topicFilterLength );

                /* Replace the callback and packet info with the new parameters. */
                connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].callback = pSubscriptionList[ i ].callback;
                connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].packetInfo.identifier = subscribePacketIdentifier;
                connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].packetInfo.order = i;
            }
            else
            {
                /* Allocate memory for a new subscription. */
                index = IotMqtt_InsertSubscription( ( connToContext[ contextIndex ].subscriptionArray ) );

                if( index == -1 )
                {
                    status = IOT_MQTT_NO_MEMORY;
                    IotLogError( "(MQTT connection %p) Subscription array is full.Need to update the config. ",
                                 pMqttConnection );
                    break;
                }
                else
                {
                    /* Set the members of the new subscription and add it to the list. */
                    connToContext[ contextIndex ].subscriptionArray[ index ].packetInfo.identifier = subscribePacketIdentifier;
                    connToContext[ contextIndex ].subscriptionArray[ index ].packetInfo.order = i;
                    connToContext[ contextIndex ].subscriptionArray[ index ].callback = pSubscriptionList[ i ].callback;
                    connToContext[ contextIndex ].subscriptionArray[ index ].topicFilterLength = pSubscriptionList[ i ].topicFilterLength;
                    connToContext[ contextIndex ].subscriptionArray[ index ].pTopicFilter = IotMqtt_MallocMessage( pSubscriptionList[ i ].topicFilterLength );
                    ( void ) memcpy( connToContext[ contextIndex ].subscriptionArray[ index ].pTopicFilter,
                                     pSubscriptionList[ i ].pTopicFilter,
                                     ( size_t ) ( pSubscriptionList[ i ].topicFilterLength ) );
                }
            }
        }

        if( xSemaphoreGive( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ) ) == pdFALSE )
        {
            IotLogError( "(MQTT connection %p) Failed to unlock subsription mutex. Semaphores are implemented using queues."
                         "An error occured due to no space on the queue to post a message.",
                         pMqttConnection );
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to take lock on subsription mutex within specified time.",
                     pMqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_TIMEOUT );
    }

    /* If memory allocation failed, remove all previously added subscriptions. */
    if( status != IOT_MQTT_SUCCESS )
    {
        _IotMqtt_RemoveSubscriptionByTopicFilter( pMqttConnection,
                                                  pSubscriptionList,
                                                  i );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    return status;

    /* Clean up if this function failed. */
    IOT_FUNCTION_CLEANUP_BEGIN();

    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

void _IotMqtt_InvokeSubscriptionCallback( _mqttConnection_t * pMqttConnection,
                                          IotMqttCallbackParam_t * pCallbackParam )
{
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pCurrentLink = NULL, * pNextLink = NULL;
    void * pCallbackContext = NULL;
    int8_t contextIndex = -1;
    int8_t index = 0;

    void ( * callbackFunction )( void *,
                                 IotMqttCallbackParam_t * ) = NULL;
    _topicMatchParams_t topicMatchParams =
    {
        .pTopicName      = pCallbackParam->u.message.info.pTopicName,
        .topicNameLength = pCallbackParam->u.message.info.topicNameLength,
        .exactMatchOnly  = false
    };

    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    /* Prevent any other thread from modifying the subscription list while this
     * function is searching. */
    xSemaphoreTake( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ), portMAX_DELAY );

    /* Search the subscription list for all matching subscriptions starting at
     * the array head. */

    while( index < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        if( contextIndex >= 0 )
        {
            index = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray ),
                                            index,
                                            &topicMatchParams );

            /* No subscription found. Exit loop. */
            if( index == -1 )
            {
                break;
            }
            else
            {
                pSubscription = &( connToContext[ contextIndex ].subscriptionArray[ index ] );
            }

            /* Subscription validation should not have allowed a NULL callback function. */
            IotMqtt_Assert( pSubscription->callback.function != NULL );

            /* Increment the subscription's reference count. */
            ( pSubscription->references )++;

            /* Copy the necessary members of the subscription before releasing the
             * subscription list mutex. */
            pCallbackContext = pSubscription->callback.pCallbackContext;
            callbackFunction = pSubscription->callback.function;

            /* Unlock the subscription list mutex. */
            xSemaphoreGive( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ) );

            /* Set the members of the callback parameter. */
            pCallbackParam->mqttConnection = pMqttConnection;
            pCallbackParam->u.message.pTopicFilter = pSubscription->pTopicFilter;
            pCallbackParam->u.message.topicFilterLength = pSubscription->topicFilterLength;

            /* Invoke the subscription callback. */
            callbackFunction( pCallbackContext, pCallbackParam );

            /* Lock the subscription list mutex to decrement the reference count. */
            xSemaphoreTake( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ), portMAX_DELAY );

            /* Decrement the reference count. It must still be positive. */
            ( pSubscription->references )--;
            IotMqtt_Assert( pSubscription->references >= 0 );

            /* Remove this subscription if it has no references and the unsubscribed
             * flag is set. */
            if( pSubscription->unsubscribed == true )
            {
                /* Free subscriptions with no references. */
                if( pSubscription->references == 0 )
                {
                    /* Free the subscription by setting the topicfilterlength to 0. */
                    pSubscription->topicFilterLength = 0;
                }
            }

            index++;
        }
        else
        {
            break;
        }
    }

    xSemaphoreGive( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ) );

    _IotMqtt_DecrementConnectionReferences( pMqttConnection );
}

/*-----------------------------------------------------------*/

void _IotMqtt_RemoveSubscriptionByPacket( _mqttConnection_t * pMqttConnection,
                                          uint16_t packetIdentifier,
                                          int32_t order )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    const _packetMatchParams_t packetMatchParams =
    {
        .packetIdentifier = packetIdentifier,
        .order            = order
    };
    int8_t contextIndex = -1;

    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ), portMAX_DELAY ) == pdTRUE )
    {
        if( contextIndex >= 0 )
        {
            if( ( IotMqtt_RemoveAllMatches( ( connToContext[ contextIndex ].subscriptionArray ), _packetMatch,
                                            ( &packetMatchParams ) ) ) == false )
            {
                IotLogError( "(MQTT connection %p) Failed to remove all matched subscriptions from the subscription array. ",
                             pMqttConnection );
                IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
            }
        }

        if( xSemaphoreGive( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ) ) == pdFALSE )
        {
            IotLogError( "(MQTT connection %p) Failed to unlock subsription mutex. Semaphores are implemented using queues."
                         "An error occured due to no space on the queue to post a message.",
                         pMqttConnection );
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to take lock on subsription mutex within specified time.",
                     pMqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_TIMEOUT );
    }

    /* Clean up if this function failed. */
    IOT_FUNCTION_CLEANUP_BEGIN();

    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

void _IotMqtt_RemoveSubscriptionByTopicFilter( _mqttConnection_t * pMqttConnection,
                                               const IotMqttSubscription_t * pSubscriptionList,
                                               size_t subscriptionCount )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t i = 0;
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _topicMatchParams_t topicMatchParams = { 0 };
    int8_t contextIndex = -1;

    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    /* Prevent any other thread from modifying the subscription list while this
     * function is running. */
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ), portMAX_DELAY ) == pdTRUE )
    {
        int8_t matchedIndex = -1;

        /* Find and remove each topic filter from the list. */
        for( i = 0; i < subscriptionCount; i++ )
        {
            topicMatchParams.pTopicName = pSubscriptionList[ i ].pTopicFilter;
            topicMatchParams.topicNameLength = pSubscriptionList[ i ].topicFilterLength;
            topicMatchParams.exactMatchOnly = true;

            if( contextIndex >= 0 )
            {
                matchedIndex = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray ), 0,
                                                       &topicMatchParams );
            }

            if( matchedIndex != -1 )
            {
                pSubscription = &( connToContext[ contextIndex ].subscriptionArray[ matchedIndex ] );
                /* Reference count must not be negative. */
                IotMqtt_Assert( pSubscription->references >= 0 );

                /* Remove subscription from list. */
                if( ( IotMqtt_RemoveSubscription( ( connToContext[ contextIndex ].subscriptionArray ), matchedIndex ) ) == false )
                {
                    IotLogError( "(MQTT connection %p) Failed to remove the subscription from the subscription array. ",
                                 pMqttConnection );
                    IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
                }

                /* Check the reference count. This subscription cannot be removed if
                 * there are subscription callbacks using it. */
                if( pSubscription->references > 0 )
                {
                    /* Set the unsubscribed flag. The last active subscription callback
                     * will remove and clean up this subscription. */
                    pSubscription->unsubscribed = true;
                }
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }

        if( xSemaphoreGive( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ) ) == pdFALSE )
        {
            IotLogError( "(MQTT connection %p) Failed to unlock subsription mutex. Semaphores are implemented using queues."
                         "An error occured due to no space on the queue to post a message.",
                         pMqttConnection );
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to take lock on subsription mutex within specified time.",
                     pMqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_TIMEOUT );
    }

    /* Clean up if this function failed. */
    IOT_FUNCTION_CLEANUP_BEGIN();

    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

bool IotMqtt_IsSubscribed( IotMqttConnection_t mqttConnection,
                           const char * pTopicFilter,
                           uint16_t topicFilterLength,
                           IotMqttSubscription_t * pCurrentSubscription )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    bool subscribedStatus = false;
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _topicMatchParams_t topicMatchParams =
    {
        .pTopicName      = pTopicFilter,
        .topicNameLength = topicFilterLength,
        .exactMatchOnly  = true
    };
    int8_t contextIndex = -1;
    int8_t matchedIndex = -1;

    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );

    /* Prevent any other thread from modifying the subscription list while this
     * function is running. */
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ), portMAX_DELAY ) == pdTRUE )
    {
        /* Search for a matching subscription. */

        if( contextIndex >= 0 )
        {
            matchedIndex = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray ), 0
                                                   , &topicMatchParams );
        }

        /* Check if a matching subscription was found. */
        if( matchedIndex != -1 )
        {
            /* Copy the matching subscription to the output parameter. */
            if( pCurrentSubscription != NULL )
            {
                pCurrentSubscription->pTopicFilter = pTopicFilter;
                pCurrentSubscription->topicFilterLength = topicFilterLength;
                pCurrentSubscription->qos = IOT_MQTT_QOS_0;
                pCurrentSubscription->callback = pSubscription->callback;
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }

            subscribedStatus = true;
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        if( xSemaphoreGive( ( SemaphoreHandle_t ) &( connToContext[ contextIndex ].subscriptionMutex ) ) == pdFALSE )
        {
            IotLogError( "(MQTT connection %p) Failed to unlock subsription mutex. Semaphores are implemented using queues."
                         "An error occured due to no space on the queue to post a message.",
                         mqttConnection );
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to take lock on subsription mutex within specified time.",
                     mqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_TIMEOUT );
    }

    return subscribedStatus;

    /* Clean up if this function failed. */
    IOT_FUNCTION_CLEANUP_BEGIN();

    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

/* Provide access to internal functions and variables if testing. */
#if IOT_BUILD_TESTS == 1
    /*#include "iot_test_access_mqtt_subscription.c" */
#endif
