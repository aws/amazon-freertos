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
 * @file aws_iot_mqtt_subscription.c
 * @brief Implements functions that manage subscriptions for an MQTT connection.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <string.h>

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/*-----------------------------------------------------------*/

/**
 * @brief First parameter to #_topicMatch.
 */
typedef struct _topicMatchParams
{
    const char * pTopicName;  /**< @brief The topic name to parse. */
    uint16_t topicNameLength; /**< @brief Length of #_topicMatchParams_t.pTopicName. */
    bool exactMatchOnly;      /**< @brief Whether to allow wildcards or require exact matches. */
} _topicMatchParams_t;

/**
 * @brief First parameter to #_packetMatch.
 */
typedef struct _packetMatchParams
{
    uint16_t packetIdentifier; /**< Packet identifier to match. */
    long order;                /**< Order to match. Set to `-1` to ignore. */
} _packetMatchParams_t;

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
static bool _topicMatch( const IotLink_t * pSubscriptionLink,
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
static bool _packetMatch( const IotLink_t * pSubscriptionLink,
                          void * pMatch );

/*-----------------------------------------------------------*/

static bool _topicMatch( const IotLink_t * pSubscriptionLink,
                         void * pMatch )
{
    uint16_t nameIndex = 0, filterIndex = 0;
    _mqttSubscription_t * pSubscription = IotLink_Container( _mqttSubscription_t,
                                                             pSubscriptionLink,
                                                             link );
    _topicMatchParams_t * pParam = ( _topicMatchParams_t * ) pMatch;

    /* Extract the relevant strings and lengths from parameters. */
    const char * const pTopicName = pParam->pTopicName;
    const char * const pTopicFilter = pSubscription->pTopicFilter;
    const uint16_t topicNameLength = pParam->topicNameLength;
    const uint16_t topicFilterLength = pSubscription->topicFilterLength;

    /* Check for an exact match. */
    if( topicNameLength == topicFilterLength )
    {
        return( strncmp( pTopicName, pTopicFilter, topicNameLength ) == 0 );
    }

    /* If the topic lengths are different but an exact match is required, return
     * false. */
    if( pParam->exactMatchOnly == true )
    {
        return false;
    }

    while( ( nameIndex < topicNameLength ) && ( filterIndex < topicFilterLength ) )
    {
        /* Check if the character in the topic name matches the corresponding
         * character in the topic filter string. */
        if( pTopicName[ nameIndex ] == pTopicFilter[ filterIndex ] )
        {
            /* Handle special corner cases as documented by the MQTT protocol spec. */

            /* Filter "sport/#" also matches "sport" since # includes the parent level. */
            if( ( nameIndex == topicNameLength - 1 ) &&
                ( filterIndex == topicFilterLength - 3 ) &&
                ( pTopicFilter[ filterIndex + 1 ] == '/' ) &&
                ( pTopicFilter[ filterIndex + 2 ] == '#' ) )
            {
                return true;
            }

            /* Filter "sport/+" also matches the "sport/" but not "sport". */
            if( ( nameIndex == topicNameLength - 1 ) &&
                ( filterIndex == topicFilterLength - 2 ) &&
                ( pTopicFilter[ filterIndex + 1 ] == '+' ) )
            {
                return true;
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
                return true;
            }
            else
            {
                /* Any character mismatch other than '+' or '#' means the topic
                 * name does not match the topic filter. */
                return false;
            }
        }

        /* Increment indexes. */
        nameIndex++;
        filterIndex++;
    }

    /* If the end of both strings has been reached, they match. */
    if( ( nameIndex == topicNameLength ) && ( filterIndex == topicFilterLength ) )
    {
        return true;
    }

    return false;
}

/*-----------------------------------------------------------*/

static bool _packetMatch( const IotLink_t * pSubscriptionLink,
                          void * pMatch )
{
    bool match = false;
    _mqttSubscription_t * pSubscription = IotLink_Container( _mqttSubscription_t,
                                                             pSubscriptionLink,
                                                             link );
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
        AwsIotMqtt_Assert( pSubscription->references >= 0 );

        /* If the reference count is positive, this subscription cannot be
         * removed yet because there are subscription callbacks using it. */
        if( pSubscription->references > 0 )
        {
            match = false;

            /* Set the unsubscribed flag. The last active subscription callback
             * will remove and clean up this subscription. */
            pSubscription->unsubscribed = true;
        }
    }

    return match;
}

/*-----------------------------------------------------------*/

AwsIotMqttError_t AwsIotMqttInternal_AddSubscriptions( _mqttConnection_t * const pMqttConnection,
                                                       uint16_t subscribePacketIdentifier,
                                                       const AwsIotMqttSubscription_t * const pSubscriptionList,
                                                       size_t subscriptionCount )
{
    size_t i = 0;
    _mqttSubscription_t * pNewSubscription = NULL;
    _topicMatchParams_t topicMatchParams = { .exactMatchOnly = true };

    AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );

    for( i = 0; i < subscriptionCount; i++ )
    {
        /* Check if this topic filter is already registered. */
        topicMatchParams.pTopicName = pSubscriptionList[ i ].pTopicFilter;
        topicMatchParams.topicNameLength = pSubscriptionList[ i ].topicFilterLength;
        pNewSubscription = IotLink_Container( _mqttSubscription_t,
                                              IotListDouble_FindFirstMatch( &( pMqttConnection->subscriptionList ),
                                                                            NULL,
                                                                            _topicMatch,
                                                                            &topicMatchParams ),
                                              link );

        if( pNewSubscription != NULL )
        {
            /* The lengths of exactly matching topic filters must match. */
            AwsIotMqtt_Assert( pNewSubscription->topicFilterLength == pSubscriptionList[ i ].topicFilterLength );

            /* Replace the callback and packet info with the new parameters. */
            pNewSubscription->callback = pSubscriptionList[ i ].callback;
            pNewSubscription->packetInfo.identifier = subscribePacketIdentifier;
            pNewSubscription->packetInfo.order = i;
        }
        else
        {
            /* Allocate memory for a new subscription. */
            pNewSubscription = AwsIotMqtt_MallocSubscription( sizeof( _mqttSubscription_t ) +
                                                              pSubscriptionList[ i ].topicFilterLength );

            /* If memory allocation failed, remove all previously added subscriptions. */
            if( pNewSubscription == NULL )
            {
                AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );
                AwsIotMqttInternal_RemoveSubscriptionByTopicFilter( pMqttConnection,
                                                                    pSubscriptionList,
                                                                    i );

                return AWS_IOT_MQTT_NO_MEMORY;
            }

            /* Clear the new subscription. */
            ( void ) memset( pNewSubscription,
                             0x00,
                             sizeof( _mqttSubscription_t ) + pSubscriptionList[ i ].topicFilterLength );

            /* Set the members of the new subscription and add it to the list. */
            pNewSubscription->packetInfo.identifier = subscribePacketIdentifier;
            pNewSubscription->packetInfo.order = i;
            pNewSubscription->callback = pSubscriptionList[ i ].callback;
            pNewSubscription->topicFilterLength = pSubscriptionList[ i ].topicFilterLength;
            ( void ) strncpy( pNewSubscription->pTopicFilter,
                              pSubscriptionList[ i ].pTopicFilter,
                              pSubscriptionList[ i ].topicFilterLength );

            IotListDouble_InsertHead( &( pMqttConnection->subscriptionList ),
                                      &( pNewSubscription->link ) );
        }
    }

    AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );

    return AWS_IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

void AwsIotMqttInternal_ProcessPublish( _mqttConnection_t * const pMqttConnection,
                                        AwsIotMqttCallbackParam_t * const pCallbackParam )
{
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pStartPoint = NULL;
    void * pParam1 = NULL;

    void ( * callbackFunction )( void *,
                                 AwsIotMqttCallbackParam_t * const ) = NULL;
    const _topicMatchParams_t topicMatchParams =
    {
        .pTopicName      = pCallbackParam->message.info.pTopicName,
        .topicNameLength = pCallbackParam->message.info.topicNameLength,
        .exactMatchOnly  = false
    };

    /* Prevent any other thread from modifying the subscription list while this
     * function is searching. */
    AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );
    pStartPoint = IotListDouble_PeekHead( &( pMqttConnection->subscriptionList ) );

    /* Search the subscription list for all matching subscriptions. */
    while( pStartPoint != NULL )
    {
        pSubscription = IotLink_Container( _mqttSubscription_t,
                                           IotListDouble_FindFirstMatch( &( pMqttConnection->subscriptionList ),
                                                                         pStartPoint,
                                                                         _topicMatch,
                                                                         ( void * ) ( &topicMatchParams ) ),
                                           link );

        /* No subscription found. Exit loop. */
        if( pSubscription == NULL )
        {
            break;
        }

        /* Subscription validation should not have allowed a NULL callback function. */
        AwsIotMqtt_Assert( pSubscription->callback.function != NULL );

        /* Increment the subscription's reference count. */
        ( pSubscription->references )++;

        /* Copy the necessary members of the subscription before releasing the
         * subscription list mutex. */
        pParam1 = pSubscription->callback.param1;
        callbackFunction = pSubscription->callback.function;

        /* Unlock the subscription list mutex. */
        AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );

        /* Set the members of the callback parameter. */
        pCallbackParam->mqttConnection = pMqttConnection;
        pCallbackParam->message.pTopicFilter = pSubscription->pTopicFilter;
        pCallbackParam->message.topicFilterLength = pSubscription->topicFilterLength;

        /* Invoke the subscription callback. */
        callbackFunction( pParam1, pCallbackParam );

        /* Lock the subscription list mutex to decrement the reference count. */
        AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );

        /* Decrement the reference count. It must still be positive. */
        ( pSubscription->references )--;
        AwsIotMqtt_Assert( pSubscription->references >= 0 );

        /* Restart the search at the next subscription. */
        pStartPoint = pSubscription->link.pNext;

        /* Remove this subscription if it has no references and the unsubscribed
         * flag is set. */
        if( ( pSubscription->references == 0 ) && ( pSubscription->unsubscribed == true ) )
        {
            IotListDouble_Remove( &( pSubscription->link ) );
            AwsIotMqtt_FreeSubscription( pSubscription );
        }
    }

    AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );
}

/*-----------------------------------------------------------*/

void AwsIotMqttInternal_RemoveSubscriptionByPacket( _mqttConnection_t * const pMqttConnection,
                                                    uint16_t packetIdentifier,
                                                    long order )
{
    const _packetMatchParams_t packetMatchParams =
    {
        .packetIdentifier = packetIdentifier,
        .order            = order
    };

    IotListDouble_RemoveAllMatches( &( pMqttConnection->subscriptionList ),
                                    _packetMatch,
                                    ( void * ) ( &packetMatchParams ),
                                    AwsIotMqtt_FreeSubscription,
                                    offsetof( _mqttSubscription_t, link ) );
}

/*-----------------------------------------------------------*/

void AwsIotMqttInternal_RemoveSubscriptionByTopicFilter( _mqttConnection_t * const pMqttConnection,
                                                         const AwsIotMqttSubscription_t * const pSubscriptionList,
                                                         size_t subscriptionCount )
{
    size_t i = 0;
    _mqttSubscription_t * pSubscription = NULL;
    _topicMatchParams_t topicMatchParams = { 0 };

    /* Prevent any other thread from modifying the subscription list while this
     * function is running. */
    AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );

    /* Find and remove each topic filter from the list. */
    for( i = 0; i < subscriptionCount; i++ )
    {
        topicMatchParams.pTopicName = pSubscriptionList[ i ].pTopicFilter;
        topicMatchParams.topicNameLength = pSubscriptionList[ i ].topicFilterLength;
        topicMatchParams.exactMatchOnly = true;

        pSubscription = IotLink_Container( _mqttSubscription_t,
                                           IotListDouble_FindFirstMatch( &( pMqttConnection->subscriptionList ),
                                                                         NULL,
                                                                         _topicMatch,
                                                                         &topicMatchParams ),
                                           link );

        if( pSubscription != NULL )
        {
            /* Reference count must not be negative. */
            AwsIotMqtt_Assert( pSubscription->references >= 0 );

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
                /* No subscription callbacks are using this subscription. Remove it. */
                IotListDouble_Remove( &( pSubscription->link ) );
                AwsIotMqtt_FreeSubscription( pSubscription );
            }
        }
    }

    AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );
}

/*-----------------------------------------------------------*/

bool AwsIotMqtt_IsSubscribed( AwsIotMqttConnection_t mqttConnection,
                              const char * const pTopicFilter,
                              uint16_t topicFilterLength,
                              AwsIotMqttSubscription_t * pCurrentSubscription )
{
    bool status = false;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;
    _mqttSubscription_t * pSubscription = NULL;
    _topicMatchParams_t topicMatchParams =
    {
        .pTopicName      = pTopicFilter,
        .topicNameLength = topicFilterLength,
        .exactMatchOnly  = true
    };

    /* Prevent any other thread from modifying the subscription list while this
     * function is running. */
    AwsIotMutex_Lock( &( pMqttConnection->subscriptionMutex ) );

    /* Search for a matching subscription. */
    pSubscription = IotLink_Container( _mqttSubscription_t,
                                       IotListDouble_FindFirstMatch( &( pMqttConnection->subscriptionList ),
                                                                     NULL,
                                                                     _topicMatch,
                                                                     &topicMatchParams ),
                                       link );

    /* Check if a matching subscription was found. */
    if( pSubscription != NULL )
    {
        /* Copy the matching subscription to the output parameter. */
        if( pCurrentSubscription != NULL )
        {
            pCurrentSubscription->pTopicFilter = pTopicFilter;
            pCurrentSubscription->topicFilterLength = topicFilterLength;
            pCurrentSubscription->QoS = 0;
            pCurrentSubscription->callback = pSubscription->callback;
        }

        status = true;
    }

    AwsIotMutex_Unlock( &( pMqttConnection->subscriptionMutex ) );

    return status;
}

/*-----------------------------------------------------------*/

/* If the MQTT library is being tested, include a file that allows access to
 * internal functions and variables. */
#if AWS_IOT_MQTT_TEST == 1
    #include "aws_iot_test_access_mqtt_subscription.c"
#endif
