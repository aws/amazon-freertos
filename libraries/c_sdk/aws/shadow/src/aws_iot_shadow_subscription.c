/*
 * FreeRTOS Shadow V2.2.0
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
 * @file aws_iot_shadow_subscription.c
 * @brief Implements functions for interacting with the Shadow library's
 * subscription list.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* MQTT include. */
#include "iot_mqtt.h"

/*-----------------------------------------------------------*/

/**
 * @brief First parameter to #_shadowSubscription_match.
 */
typedef struct _thingName
{
    const char * pThingName; /**< @brief Thing Name to compare. */
    size_t thingNameLength;  /**< @brief Length of `pThingName`. */
} _thingName_t;

/*-----------------------------------------------------------*/

/**
 * @brief Match two #_shadowSubscription_t by Thing Name.
 *
 * @param[in] pSubscriptionLink Pointer to the link member of a #_shadowSubscription_t
 * containing the Thing Name to check.
 * @param[in] pMatch Pointer to a #_thingName_t.
 *
 * @return `true` if the Thing Names match; `false` otherwise.
 */
static bool _shadowSubscription_match( const IotLink_t * pSubscriptionLink,
                                       void * pMatch );

/**
 * @brief Modify Shadow subscriptions, either by unsubscribing or subscribing.
 *
 * @param[in] mqttConnection The MQTT connection to use.
 * @param[in] pTopicFilter The topic filter to modify.
 * @param[in] topicFilterLength The length of `pTopicFilter`.
 * @param[in] callback The callback function to execute for an incoming message.
 * @param[in] mqttOperation Either @ref mqtt_function_timedsubscribe or @ref
 * mqtt_function_timedunsubscribe.
 *
 * @return #AWS_IOT_SHADOW_STATUS_PENDING on success; otherwise
 * #AWS_IOT_SHADOW_NO_MEMORY or #AWS_IOT_SHADOW_MQTT_ERROR.
 */
static AwsIotShadowError_t _modifyOperationSubscriptions( IotMqttConnection_t mqttConnection,
                                                          const char * pTopicFilter,
                                                          uint16_t topicFilterLength,
                                                          _mqttCallbackFunction_t callback,
                                                          _mqttOperationFunction_t mqttOperation );

/*-----------------------------------------------------------*/

/**
 * @brief List of active Shadow subscriptions objects.
 */
IotListDouble_t _AwsIotShadowSubscriptions = { 0 };

/**
 * @brief Protects #_AwsIotShadowSubscriptions from concurrent access.
 */
IotMutex_t _AwsIotShadowSubscriptionsMutex;

/*-----------------------------------------------------------*/

static bool _shadowSubscription_match( const IotLink_t * pSubscriptionLink,
                                       void * pMatch )
{
    bool match = false;

    /* Because this function is called from a container function, the given link
     * must never be NULL. */
    AwsIotShadow_Assert( pSubscriptionLink != NULL );

    const _shadowSubscription_t * pSubscription = IotLink_Container( _shadowSubscription_t,
                                                                     pSubscriptionLink,
                                                                     link );
    const _thingName_t * pThingName = ( _thingName_t * ) pMatch;

    if( pThingName->thingNameLength == pSubscription->thingNameLength )
    {
        /* Check for matching Thing Names. */
        match = ( strncmp( pThingName->pThingName,
                           pSubscription->pThingName,
                           pThingName->thingNameLength ) == 0 );
    }

    return match;
}

/*-----------------------------------------------------------*/

static AwsIotShadowError_t _modifyOperationSubscriptions( IotMqttConnection_t mqttConnection,
                                                          const char * pTopicFilter,
                                                          uint16_t topicFilterLength,
                                                          _mqttCallbackFunction_t callback,
                                                          _mqttOperationFunction_t mqttOperation )
{
    IotMqttError_t mqttStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* The MQTT operation function pointer must be either Subscribe or Unsubscribe. */
    AwsIotShadow_Assert( ( mqttOperation == IotMqtt_TimedSubscribe ) ||
                         ( mqttOperation == IotMqtt_TimedUnsubscribe ) );

    /* Per the AWS IoT documentation, Shadow topic subscriptions are QoS 1. */
    subscription.qos = IOT_MQTT_QOS_1;

    IotLogDebug( "%s Shadow subscription for %.*s",
                 mqttOperation == IotMqtt_TimedSubscribe ? "Adding" : "Removing",
                 topicFilterLength,
                 pTopicFilter );

    /* Set the members of the subscription parameter. */
    subscription.pTopicFilter = pTopicFilter;
    subscription.topicFilterLength = topicFilterLength;
    subscription.callback.pCallbackContext = NULL;
    subscription.callback.function = callback;

    /* Call the MQTT operation function. */
    mqttStatus = mqttOperation( mqttConnection,
                                &subscription,
                                1,
                                0,
                                _AwsIotShadowMqttTimeoutMs );

    /* Check the result of the MQTT operation. */
    if( mqttStatus != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to %s %.*s, error %s.",
                     mqttOperation == IotMqtt_TimedSubscribe ? "subscribe to" : "unsubscribe from",
                     topicFilterLength,
                     pTopicFilter,
                     IotMqtt_strerror( mqttStatus ) );

        /* Convert the MQTT "NO MEMORY" error to a Shadow "NO MEMORY" error. */
        if( mqttStatus == IOT_MQTT_NO_MEMORY )
        {
            return AWS_IOT_SHADOW_NO_MEMORY;
        }

        return AWS_IOT_SHADOW_MQTT_ERROR;
    }
    else
    {
        IotLogDebug( "Successfully %s %.*s",
                     mqttOperation == IotMqtt_TimedSubscribe ? "subscribed to" : "unsubscribed from",
                     topicFilterLength,
                     pTopicFilter );
    }

    return AWS_IOT_SHADOW_STATUS_PENDING;
}

/*-----------------------------------------------------------*/

_shadowSubscription_t * _AwsIotShadow_FindSubscription( const char * pThingName,
                                                        size_t thingNameLength )
{
    _shadowSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _thingName_t thingName =
    {
        .pThingName      = pThingName,
        .thingNameLength = thingNameLength
    };

    /* Search the list for an existing subscription for Thing Name. */
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _AwsIotShadowSubscriptions ),
                                                      NULL,
                                                      _shadowSubscription_match,
                                                      &thingName );

    /* Check if a subscription was found. */
    if( pSubscriptionLink == NULL )
    {
        /* No subscription found. Allocate a new subscription. */
        pSubscription = AwsIotShadow_MallocSubscription( sizeof( _shadowSubscription_t ) + thingNameLength );

        if( pSubscription != NULL )
        {
            /* Clear the new subscription. */
            ( void ) memset( pSubscription, 0x00, sizeof( _shadowSubscription_t ) + thingNameLength );

            /* Set the Thing Name length and copy the Thing Name into the new subscription. */
            pSubscription->thingNameLength = thingNameLength;
            ( void ) strncpy( pSubscription->pThingName, pThingName, thingNameLength );

            /* Add the new subscription to the subscription list. */
            IotListDouble_InsertHead( &( _AwsIotShadowSubscriptions ),
                                      &( pSubscription->link ) );

            IotLogDebug( "Created new Shadow subscriptions object for %.*s.",
                         thingNameLength,
                         pThingName );
        }
        else
        {
            IotLogError( "Failed to allocate memory for %.*s Shadow subscriptions.",
                         thingNameLength,
                         pThingName );
        }
    }
    else
    {
        IotLogDebug( "Found existing Shadow subscriptions object for %.*s.",
                     thingNameLength,
                     pThingName );

        pSubscription = IotLink_Container( _shadowSubscription_t, pSubscriptionLink, link );
    }

    return pSubscription;
}

/*-----------------------------------------------------------*/

void _AwsIotShadow_RemoveSubscription( _shadowSubscription_t * pSubscription,
                                       _shadowSubscription_t ** pRemovedSubscription )
{
    int i = 0;

    IotLogDebug( "Checking if subscription object for %.*s can be removed.",
                 pSubscription->thingNameLength,
                 pSubscription->pThingName );

    /* If any Shadow operation's subscription reference count is not 0, then the
     * subscription cannot be removed. */
    for( i = 0; i < SHADOW_OPERATION_COUNT; i++ )
    {
        if( pSubscription->references[ i ] > 0 )
        {
            IotLogDebug( "Reference count %ld for %.*s subscription object. "
                         "Subscription cannot be removed yet.",
                         ( long int ) pSubscription->references[ i ],
                         pSubscription->thingNameLength,
                         pSubscription->pThingName );

            return;
        }
        else if( pSubscription->references[ i ] == PERSISTENT_SUBSCRIPTION )
        {
            IotLogDebug( "Subscription object for %.*s has persistent subscriptions. "
                         "Subscription will not be removed.",
                         pSubscription->thingNameLength,
                         pSubscription->pThingName );

            return;
        }
    }

    /* If any Shadow callbacks are active, then the subscription cannot be removed. */
    for( i = 0; i < SHADOW_CALLBACK_COUNT; i++ )
    {
        if( pSubscription->callbacks[ i ].function != NULL )
        {
            IotLogDebug( "Found active Shadow %s callback for %.*s subscription object. "
                         "Subscription cannot be removed yet.",
                         _pAwsIotShadowCallbackNames[ i ],
                         pSubscription->thingNameLength,
                         pSubscription->pThingName );

            return;
        }
    }

    /* No Shadow operation subscription references or active Shadow callbacks.
     * Remove the subscription object. */
    IotListDouble_Remove( &( pSubscription->link ) );

    IotLogDebug( "Removed subscription object for %.*s.",
                 pSubscription->thingNameLength,
                 pSubscription->pThingName );

    /* If the caller requested the removed subscription, set the output parameter.
     * Otherwise, free the memory used by the subscription. */
    if( pRemovedSubscription != NULL )
    {
        *pRemovedSubscription = pSubscription;
    }
    else
    {
        _AwsIotShadow_DestroySubscription( pSubscription );
    }
}

/*-----------------------------------------------------------*/

void _AwsIotShadow_DestroySubscription( void * pData )
{
    _shadowSubscription_t * pSubscription = ( _shadowSubscription_t * ) pData;

    /* Free the topic buffer. It should not be NULL. */
    AwsIotShadow_Assert( pSubscription->pTopicBuffer != NULL );
    AwsIotShadow_FreeString( pSubscription->pTopicBuffer );

    /* Free memory used by subscription. */
    AwsIotShadow_FreeSubscription( pSubscription );
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t _AwsIotShadow_IncrementReferences( _shadowOperation_t * pOperation,
                                                       char * pTopicBuffer,
                                                       uint16_t operationTopicLength,
                                                       _mqttCallbackFunction_t callback )
{
    uint16_t topicFilterLength = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    const _shadowOperationType_t type = pOperation->type;
    _shadowSubscription_t * pSubscription = pOperation->pSubscription;

    /* Do nothing if this operation has persistent subscriptions. */
    if( pSubscription->references[ type ] == PERSISTENT_SUBSCRIPTION )
    {
        IotLogDebug( "Shadow %s for %.*s has persistent subscriptions. Reference "
                     "count will not be incremented.",
                     _pAwsIotShadowOperationNames[ type ],
                     pSubscription->thingNameLength,
                     pSubscription->pThingName );

        return AWS_IOT_SHADOW_STATUS_PENDING;
    }

    /* When persistent subscriptions are not present, the reference count must
     * not be negative. */
    AwsIotShadow_Assert( pSubscription->references[ type ] >= 0 );

    /* Check if there are any existing references for this operation. */
    if( pSubscription->references[ type ] == 0 )
    {
        /* Place the topic "accepted" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         SHADOW_ACCEPTED_SUFFIX,
                         SHADOW_ACCEPTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + SHADOW_ACCEPTED_SUFFIX_LENGTH );

        /* There should not be an active subscription for the accepted topic. */
        AwsIotShadow_Assert( IotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                   pTopicBuffer,
                                                   topicFilterLength,
                                                   NULL ) == false );

        /* Add a subscription to the Shadow "accepted" topic. */
        status = _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                callback,
                                                IotMqtt_TimedSubscribe );

        if( status != AWS_IOT_SHADOW_STATUS_PENDING )
        {
            return status;
        }

        /* Place the topic "rejected" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         SHADOW_REJECTED_SUFFIX,
                         SHADOW_REJECTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + SHADOW_REJECTED_SUFFIX_LENGTH );

        /* There should not be an active subscription for the rejected topic. */
        AwsIotShadow_Assert( IotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                   pTopicBuffer,
                                                   topicFilterLength,
                                                   NULL ) == false );

        /* Add a subscription to the Shadow "rejected" topic. */
        status = _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                callback,
                                                IotMqtt_TimedSubscribe );

        if( status != AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* Failed to add subscription to Shadow "rejected" topic. Remove
             * subscription for the Shadow "accepted" topic. */
            ( void ) memcpy( pTopicBuffer + operationTopicLength,
                             SHADOW_ACCEPTED_SUFFIX,
                             SHADOW_ACCEPTED_SUFFIX_LENGTH );
            topicFilterLength = ( uint16_t ) ( operationTopicLength + SHADOW_ACCEPTED_SUFFIX_LENGTH );

            ( void ) _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                    pTopicBuffer,
                                                    topicFilterLength,
                                                    callback,
                                                    IotMqtt_TimedUnsubscribe );

            return status;
        }
    }

    /* Increment the number of subscription references for this operation when
     * the keep subscriptions flag is not set. */
    if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS ) == 0 )
    {
        ( pSubscription->references[ type ] )++;

        IotLogDebug( "Shadow %s subscriptions for %.*s now has count %d.",
                     _pAwsIotShadowOperationNames[ type ],
                     pSubscription->thingNameLength,
                     pSubscription->pThingName,
                     pSubscription->references[ type ] );
    }
    /* Otherwise, set the persistent subscriptions flag. */
    else
    {
        pSubscription->references[ type ] = PERSISTENT_SUBSCRIPTION;

        IotLogDebug( "Set persistent subscriptions flag for Shadow %s of %.*s.",
                     _pAwsIotShadowOperationNames[ type ],
                     pSubscription->thingNameLength,
                     pSubscription->pThingName );
    }

    return status;
}

/*-----------------------------------------------------------*/

void _AwsIotShadow_DecrementReferences( _shadowOperation_t * pOperation,
                                        char * pTopicBuffer,
                                        _shadowSubscription_t ** pRemovedSubscription )
{
    uint16_t topicFilterLength = 0;
    const _shadowOperationType_t type = pOperation->type;
    _shadowSubscription_t * pSubscription = pOperation->pSubscription;
    uint16_t operationTopicLength = 0;

    /* Do nothing if this Shadow operation has persistent subscriptions. */
    if( pSubscription->references[ type ] == PERSISTENT_SUBSCRIPTION )
    {
        IotLogDebug( "Shadow %s for %.*s has persistent subscriptions. Reference "
                     "count will not be decremented.",
                     _pAwsIotShadowOperationNames[ type ],
                     pSubscription->thingNameLength,
                     pSubscription->pThingName );

        return;
    }

    /* Decrement the number of subscription references for this operation.
     * Ensure that it's positive. */
    ( pSubscription->references[ type ] )--;
    AwsIotShadow_Assert( pSubscription->references[ type ] >= 0 );

    /* Check if the number of references has reached 0. */
    if( pSubscription->references[ type ] == 0 )
    {
        IotLogDebug( "Reference count for %.*s %s is 0. Unsubscribing.",
                     pSubscription->thingNameLength,
                     pSubscription->pThingName,
                     _pAwsIotShadowOperationNames[ type ] );

        /* Subscription must have a topic buffer. */
        AwsIotShadow_Assert( pSubscription->pTopicBuffer != NULL );

        /* Generate the prefix of the Shadow topic. This function will not
         * fail when given a buffer. */
        ( void ) _AwsIotShadow_GenerateShadowTopic( ( _shadowOperationType_t ) type,
                                                    pSubscription->pThingName,
                                                    pSubscription->thingNameLength,
                                                    &( pSubscription->pTopicBuffer ),
                                                    &operationTopicLength );

        /* Place the topic "accepted" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         SHADOW_ACCEPTED_SUFFIX,
                         SHADOW_ACCEPTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + SHADOW_ACCEPTED_SUFFIX_LENGTH );

        /* There should be an active subscription for the accepted topic. */
        AwsIotShadow_Assert( IotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                   pTopicBuffer,
                                                   topicFilterLength,
                                                   NULL ) == true );

        /* Remove the subscription from the Shadow "accepted" topic. */
        ( void ) _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                NULL,
                                                IotMqtt_TimedUnsubscribe );

        /* Place the topic "rejected" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         SHADOW_REJECTED_SUFFIX,
                         SHADOW_REJECTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + SHADOW_ACCEPTED_SUFFIX_LENGTH );

        /* There should be an active subscription for the accepted topic. */
        AwsIotShadow_Assert( IotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                   pTopicBuffer,
                                                   topicFilterLength,
                                                   NULL ) == true );

        /* Remove the subscription from the Shadow "rejected" topic. */
        ( void ) _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                NULL,
                                                IotMqtt_TimedUnsubscribe );
    }

    /* Check if this subscription should be deleted. */
    _AwsIotShadow_RemoveSubscription( pSubscription,
                                      pRemovedSubscription );
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t AwsIotShadow_RemovePersistentSubscriptions( IotMqttConnection_t mqttConnection,
                                                                const char * pThingName,
                                                                size_t thingNameLength,
                                                                uint32_t flags )
{
    int i = 0;
    uint16_t operationTopicLength = 0, topicFilterLength = 0;
    AwsIotShadowError_t removeAcceptedStatus = AWS_IOT_SHADOW_STATUS_PENDING,
                        removeRejectedStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    _shadowSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _thingName_t thingName =
    {
        .pThingName      = pThingName,
        .thingNameLength = thingNameLength
    };

    IotLogInfo( "Removing persistent subscriptions for %.*s.",
                thingNameLength,
                pThingName );

    IotMutex_Lock( &( _AwsIotShadowSubscriptionsMutex ) );

    /* Search the list for an existing subscription for Thing Name. */
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _AwsIotShadowSubscriptions ),
                                                      NULL,
                                                      _shadowSubscription_match,
                                                      &thingName );

    /* Unsubscribe from operation subscriptions if found. */
    if( pSubscriptionLink != NULL )
    {
        IotLogDebug( "Found subscription object for %.*s. Checking for persistent "
                     "subscriptions to remove.",
                     thingNameLength,
                     pThingName );

        pSubscription = IotLink_Container( _shadowSubscription_t, pSubscriptionLink, link );

        for( i = 0; i < SHADOW_OPERATION_COUNT; i++ )
        {
            if( ( flags & ( 0x1UL << i ) ) != 0 )
            {
                IotLogDebug( "Removing %.*s %s persistent subscriptions.",
                             thingNameLength,
                             pThingName,
                             _pAwsIotShadowOperationNames[ i ] );

                /* Subscription must have a topic buffer. */
                AwsIotShadow_Assert( pSubscription->pTopicBuffer != NULL );

                if( pSubscription->references[ i ] == PERSISTENT_SUBSCRIPTION )
                {
                    /* Generate the prefix of the Shadow topic. This function will not
                     * fail when given a buffer. */
                    ( void ) _AwsIotShadow_GenerateShadowTopic( ( _shadowOperationType_t ) i,
                                                                pThingName,
                                                                thingNameLength,
                                                                &( pSubscription->pTopicBuffer ),
                                                                &operationTopicLength );

                    /* Remove the "accepted" topic. */
                    ( void ) memcpy( pSubscription->pTopicBuffer + operationTopicLength,
                                     SHADOW_ACCEPTED_SUFFIX,
                                     SHADOW_ACCEPTED_SUFFIX_LENGTH );
                    topicFilterLength = ( uint16_t ) ( operationTopicLength + SHADOW_ACCEPTED_SUFFIX_LENGTH );

                    removeAcceptedStatus = _modifyOperationSubscriptions( mqttConnection,
                                                                          pSubscription->pTopicBuffer,
                                                                          topicFilterLength,
                                                                          NULL,
                                                                          IotMqtt_TimedUnsubscribe );

                    if( removeAcceptedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
                    {
                        break;
                    }

                    /* Remove the "rejected" topic. */
                    ( void ) memcpy( pSubscription->pTopicBuffer + operationTopicLength,
                                     SHADOW_REJECTED_SUFFIX,
                                     SHADOW_ACCEPTED_SUFFIX_LENGTH );
                    topicFilterLength = ( uint16_t ) ( operationTopicLength +
                                                       SHADOW_REJECTED_SUFFIX_LENGTH );

                    removeRejectedStatus = _modifyOperationSubscriptions( mqttConnection,
                                                                          pSubscription->pTopicBuffer,
                                                                          topicFilterLength,
                                                                          NULL,
                                                                          IotMqtt_TimedUnsubscribe );

                    if( removeRejectedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
                    {
                        break;
                    }

                    /* Clear the persistent subscriptions flag. */
                    pSubscription->references[ i ] = 0;
                }
                else
                {
                    IotLogDebug( "%.*s %s does not have persistent subscriptions.",
                                 thingNameLength,
                                 pThingName,
                                 _pAwsIotShadowOperationNames[ i ] );
                }
            }
        }
    }
    else
    {
        IotLogWarn( "No subscription object found for %.*s",
                    thingNameLength,
                    pThingName );
    }

    IotMutex_Unlock( &( _AwsIotShadowSubscriptionsMutex ) );

    /* Check the results of the MQTT unsubscribes. */
    if( removeAcceptedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
    {
        return removeAcceptedStatus;
    }

    if( removeRejectedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
    {
        return removeRejectedStatus;
    }

    return AWS_IOT_SHADOW_SUCCESS;
}

/*-----------------------------------------------------------*/
