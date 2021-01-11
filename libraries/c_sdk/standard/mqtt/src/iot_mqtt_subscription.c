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

IotMqttError_t _IotMqtt_AddSubscriptions( _mqttConnection_t * pMqttConnection,
                                          uint16_t subscribePacketIdentifier,
                                          const IotMqttSubscription_t * pSubscriptionList,
                                          size_t subscriptionCount )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t i = 0;
    _topicMatchParams_t topicMatchParams = { .exactMatchOnly = true };
    int8_t contextIndex = -1;
    int8_t matchedIndex = -1;
    int8_t index = -1;
    char * pTopicFilter = NULL;

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    if( IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) ) == true )
    {
        for( i = 0; i < subscriptionCount; i++ )
        {
            /* Check if this topic filter is already registered. */
            topicMatchParams.pTopicName = pSubscriptionList[ i ].pTopicFilter;
            topicMatchParams.topicNameLength = pSubscriptionList[ i ].topicFilterLength;
            /* Finding the matching subscription if it exists. */
            matchedIndex = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray[ 0 ] ), 0, &topicMatchParams );

            if( matchedIndex != -1 )
            {
                IotMqtt_Assert( connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].topicFilterLength == pSubscriptionList[ i ].topicFilterLength );

                /* Replace the callback and packet info with the new parameters. */
                connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].callback = pSubscriptionList[ i ].callback;
                connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].packetInfo.identifier = subscribePacketIdentifier;
                connToContext[ contextIndex ].subscriptionArray[ matchedIndex ].packetInfo.order = i;
            }
            else
            {
                /* Getting index to insert the subscription in the subscription array. */
                index = IotMqtt_GetFreeIndexInSubscriptionArray( ( connToContext[ contextIndex ].subscriptionArray ) );

                if( index == -1 )
                {
                    status = IOT_MQTT_NO_MEMORY;
                    IotLogError( "(MQTT connection %p) Subscription array is full. "
                                 "Consider updating the MAX_NO_OF_MQTT_SUBSCRIPTIONS config to resolve the issue. ",
                                 pMqttConnection );
                    break;
                }
                else
                {
                    /* Clear the new subscription. */
                    memset( &( connToContext[ contextIndex ].subscriptionArray[ index ] ),
                            0x00,
                            sizeof( _mqttSubscription_t ) );

                    /* Set the members of the new subscription and add it to the list. */
                    connToContext[ contextIndex ].subscriptionArray[ index ].packetInfo.identifier = subscribePacketIdentifier;
                    connToContext[ contextIndex ].subscriptionArray[ index ].packetInfo.order = i;
                    connToContext[ contextIndex ].subscriptionArray[ index ].callback = pSubscriptionList[ i ].callback;
                    connToContext[ contextIndex ].subscriptionArray[ index ].topicFilterLength = pSubscriptionList[ i ].topicFilterLength;
                    pTopicFilter = IotMqtt_MallocMessage( pSubscriptionList[ i ].topicFilterLength );

                    if( pTopicFilter != NULL )
                    {
                        connToContext[ contextIndex ].subscriptionArray[ index ].pTopicFilter = pTopicFilter;
                        ( void ) memcpy( connToContext[ contextIndex ].subscriptionArray[ index ].pTopicFilter,
                                         pSubscriptionList[ i ].pTopicFilter,
                                         ( size_t ) ( pSubscriptionList[ i ].topicFilterLength ) );
                    }
                    else
                    {
                        status = IOT_MQTT_NO_MEMORY;
                        IotLogError( "(MQTT connection %p) Failed to allocate memory for topic filter. ",
                                     pMqttConnection );
                        break;
                    }
                }
            }
        }

        if( IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) ) == false )
        {
            /* Fail to give subscription mutex as no space is available on queue. */
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
    }
    else
    {
        /* Fail to take context mutex due to timeout. */
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

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

void _IotMqtt_InvokeSubscriptionCallback( _mqttConnection_t * pMqttConnection,
                                          IotMqttCallbackParam_t * pCallbackParam )
{
    _mqttSubscription_t * pSubscription = NULL;
    void * pCallbackContext = NULL;
    int8_t contextIndex = -1;
    int8_t index = 0;
    bool mutexStatus = true;

    void ( * callbackFunction )( void *,
                                 IotMqttCallbackParam_t * ) = NULL;
    _topicMatchParams_t topicMatchParams =
    {
        .pTopicName      = pCallbackParam->u.message.info.pTopicName,
        .topicNameLength = pCallbackParam->u.message.info.topicNameLength,
        .exactMatchOnly  = false
    };

    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    /* Prevent any other thread from modifying the subscription array while this
     * function is searching. */
    mutexStatus = IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) );

    /* Assert to check mutex has been taken successfully. */
    IotMqtt_Assert( mutexStatus == true );

    /* Search the subscription list for all matching subscriptions starting at
     * the array head. */

    while( index < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        if( contextIndex >= 0 )
        {
            index = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray[ 0 ] ),
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

            /* Unlock the subscription array mutex. */
            mutexStatus = IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) );

            /* Assert to check mutex has been given successfully. */
            IotMqtt_Assert( mutexStatus == true );
            /* Set the members of the callback parameter. */
            pCallbackParam->mqttConnection = pMqttConnection;
            pCallbackParam->u.message.pTopicFilter = pSubscription->pTopicFilter;
            pCallbackParam->u.message.topicFilterLength = pSubscription->topicFilterLength;

            /* Invoke the subscription callback. */
            callbackFunction( pCallbackContext, pCallbackParam );

            /* Lock the subscription array mutex to decrement the reference count. */
            mutexStatus = IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) );

            /* Assert to check mutex has been taken successfully. */
            IotMqtt_Assert( mutexStatus == true );

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

    mutexStatus = IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) );

    /* Assert to check mutex has been given successfully. */
    IotMqtt_Assert( mutexStatus == true );
    _IotMqtt_DecrementConnectionReferences( pMqttConnection );
}

/*-----------------------------------------------------------*/

void _IotMqtt_RemoveSubscriptionByPacket( _mqttConnection_t * pMqttConnection,
                                          uint16_t packetIdentifier,
                                          int32_t order )
{
    const _packetMatchParams_t packetMatchParams =
    {
        .packetIdentifier = packetIdentifier,
        .order            = order
    };
    int8_t contextIndex = -1;
    bool mutexStatus = true;

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    mutexStatus = IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) );

    if( mutexStatus == true )
    {
        IotMqtt_RemoveAllMatches( ( connToContext[ contextIndex ].subscriptionArray ),
                                  ( &packetMatchParams ) );

        mutexStatus = IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) );
    }

    IotMqtt_Assert( mutexStatus == true );
}

/*-----------------------------------------------------------*/

void _IotMqtt_RemoveSubscriptionByTopicFilter( _mqttConnection_t * pMqttConnection,
                                               const IotMqttSubscription_t * pSubscriptionList,
                                               size_t subscriptionCount )
{
    size_t i = 0;
    _mqttSubscription_t * pSubscription = NULL;
    _topicMatchParams_t topicMatchParams = { 0 };
    int8_t contextIndex = -1;
    int8_t matchedIndex = -1;
    bool mutexStatus = true;
    bool subscriptionStatus = true;

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( pMqttConnection );

    /* Prevent any other thread from modifying the subscription list while this
     * function is running. */
    if( IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) ) == true )
    {
        /* Find and remove each topic filter from the list. */
        for( i = 0; i < subscriptionCount; i++ )
        {
            topicMatchParams.pTopicName = pSubscriptionList[ i ].pTopicFilter;
            topicMatchParams.topicNameLength = pSubscriptionList[ i ].topicFilterLength;
            topicMatchParams.exactMatchOnly = true;

            matchedIndex = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray[ 0 ] ), 0,
                                                   &topicMatchParams );

            if( matchedIndex != -1 )
            {
                pSubscription = &( connToContext[ contextIndex ].subscriptionArray[ matchedIndex ] );

                /* Reference count must not be negative. */
                IotMqtt_Assert( pSubscription->references >= 0 );

                /* Remove subscription from list. */
                subscriptionStatus = IotMqtt_RemoveSubscription( ( connToContext[ contextIndex ].subscriptionArray ), matchedIndex );

                /* Assert to check that subscription has been removed successfully. */
                IotMqtt_Assert( subscriptionStatus == true );

                /* Check the reference count. This subscription cannot be removed if
                 * there are subscription callbacks using it. */
                if( pSubscription->references > 0 )
                {
                    /* Set the unsubscribed flag. The last active subscription callback
                     * will remove and clean up this subscription. */
                    pSubscription->unsubscribed = true;
                }
                else
                {
                    /* Free a subscription by setting topicFilterLength to 0 as it indicates that subscription is unsubscribed. */
                    pSubscription->topicFilterLength = 0;
                }
            }
            else
            {
                EMPTY_ELSE_MARKER;
            }
        }

        mutexStatus = IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) );
    }
    else
    {
        /* Fail to take context mutex due to timeout. */
        mutexStatus = false;
    }

    IotMqtt_Assert( mutexStatus == true );
}

/*-----------------------------------------------------------*/

bool IotMqtt_IsSubscribed( IotMqttConnection_t mqttConnection,
                           const char * pTopicFilter,
                           uint16_t topicFilterLength,
                           IotMqttSubscription_t * pCurrentSubscription )
{
    bool status = false, mutexStatus = true;
    _mqttSubscription_t * pSubscription = NULL;
    _topicMatchParams_t topicMatchParams =
    {
        .pTopicName      = pTopicFilter,
        .topicNameLength = topicFilterLength,
        .exactMatchOnly  = true
    };
    int8_t contextIndex = -1;
    int8_t matchedIndex = -1;

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );
    IotMqtt_Assert( contextIndex != -1 );

    /* Prevent any other thread from modifying the subscription list while this
     * function is running. */
    mutexStatus = IotMutex_Take( &( connToContext[ contextIndex ].subscriptionMutex ) );

    if( mutexStatus == true )
    {
        matchedIndex = IotMqtt_FindFirstMatch( &( connToContext[ contextIndex ].subscriptionArray[ 0 ] ), 0
                                               , &topicMatchParams );

        /* Check if a matching subscription was found. */
        if( matchedIndex != -1 )
        {
            pSubscription = &( connToContext[ contextIndex ].subscriptionArray[ 0 ] );

            /* Copy the matching subscription to the output parameter. */
            if( pCurrentSubscription != NULL )
            {
                pCurrentSubscription->pTopicFilter = pTopicFilter;
                pCurrentSubscription->topicFilterLength = topicFilterLength;
                pCurrentSubscription->qos = IOT_MQTT_QOS_0;
                pCurrentSubscription->callback = pSubscription->callback;
            }

            status = true;
        }

        mutexStatus = IotMutex_Give( &( connToContext[ contextIndex ].subscriptionMutex ) );
    }

    /* Assert to check whether mutex was given successfully or not. */
    IotMqtt_Assert( mutexStatus == true );

    return status;
}

/*-----------------------------------------------------------*/
