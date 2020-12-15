/*
 * FreeRTOS MQTT V2.3.1
 * Copyright(C) 2020 Amazon.com, Inc. or its affiliates.All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this softwareand associated documentation files(the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_array_container.c
 * @brief Implements array data structure functions for holding subscriptions and opeartions in the shim.
 */
/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/*-----------------------------------------------------------*/

/**
 * @brief Matches a packet identifier and order.
 *
 * @param[in] pSubscription Pointer to #_mqttSubscription_t.
 * @param[in] pMatch Pointer to a #_packetMatchParams_t.
 *
 * @return `true` if the arguments match the subscription's packet info; `false`
 * otherwise.
 */
static bool _packetMatch( _mqttSubscription_t * pSubscription,
                          const void * pMatch );

/**
 * @brief Matches a topic name (from a publish) with a topic filter (from a
 * subscription).
 *
 * @param[in] pSubscription Pointer to #_mqttSubscription_t.
 * @param[in] pMatch Pointer to a #_topicMatchParams_t.
 *
 * @return `true` if the arguments match the subscription topic filter; `false`
 * otherwise.
 */
static bool _topicMatch( _mqttSubscription_t * pSubscription,
                         void * pMatch );

/*-----------------------------------------------------------*/

static bool _packetMatch( _mqttSubscription_t * pSubscription,
                          const void * pMatch )
{
    bool match = false;

    /* The given subscription must never be NULL. */
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
    }

    return match;
}

/*-----------------------------------------------------------*/

static bool _topicMatch( _mqttSubscription_t * pSubscription,
                         void * pMatch )
{
    IOT_FUNCTION_ENTRY( bool, false );
    uint16_t nameIndex = 0, filterIndex = 0;

    /* The given subscription must never be NULL. */
    IotMqtt_Assert( pSubscription != NULL );

    _topicMatchParams_t * pParam = ( _topicMatchParams_t * ) pMatch;

    /* Extract the relevant strings and lengths from parameters. */
    const char * pTopicName = pParam->pTopicName;
    const char * pTopicFilter = pSubscription->pTopicFilter;
    const uint16_t topicNameLength = pParam->topicNameLength;
    const uint16_t topicFilterLength = pSubscription->topicFilterLength;

    /* Check for an exact match. */
    if( topicNameLength == topicFilterLength )
    {
        status = ( strncmp( pTopicName, pTopicFilter, topicNameLength ) == 0 );

        IOT_GOTO_CLEANUP();
    }

    /* If the topic lengths are different but an exact match is required, return
     * false. */
    if( pParam->exactMatchOnly == true )
    {
        IOT_SET_AND_GOTO_CLEANUP( false );
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
                    }
                }
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
                }
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

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

int8_t IotMqtt_GetFreeIndexInSubscriptionArray( _mqttSubscription_t * pSubscriptionArray )
{
    /* The shim supports only upto 128 subscriptions as the implementation uses 8 bit index. */
    int8_t index = 0;

    IotMqtt_Assert( pSubscriptionArray != NULL );

    /* Finding the free index in subscription array to insert the subscription. */
    while( index < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        /* Using topicFilterLength as a unique parameter to check whether index is free or not. */
        if( pSubscriptionArray[ index ].topicFilterLength == 0 )
        {
            break;
        }

        index++;
    }

    if( index == MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        index = -1;
    }

    return index;
}

/*-----------------------------------------------------------*/

bool IotMqtt_RemoveSubscription( _mqttSubscription_t * pSubscriptionArray,
                                 int8_t deleteIndex )
{
    bool status = false;

    IotMqtt_Assert( pSubscriptionArray != NULL );
    IotMqtt_Assert( deleteIndex > -1 && deleteIndex < MAX_NO_OF_MQTT_SUBSCRIPTIONS );

    /* Remove the subscription from the subscription array. */
    if( deleteIndex != -1 )
    {
        /* Using topicFilterLength as a unique parameter to free index and make it available for other subscriptions. */
        pSubscriptionArray[ deleteIndex ].topicFilterLength = 0;
        status = true;
    }
    else
    {
        IotLogWarn( "Failed to remove subscription from the subscription array." );
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveAllMatches( _mqttSubscription_t * pSubscriptionArray,
                               const _packetMatchParams_t * pMatch )
{
    size_t index = 0;

    IotMqtt_Assert( pSubscriptionArray != NULL );

    while( index < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        if( pMatch != NULL )
        {
            /* Removing the subscription if it matches the given params. */
            if( _packetMatch( &( pSubscriptionArray[ index ] ), pMatch ) == true )
            {
                /* Using topicFilterLength as a unique parameter to free index and make it available for other subscriptions.
                 * As topicFilterLength will be non zero for the currently used subscriptions. */
                pSubscriptionArray[ index ].topicFilterLength = 0;
            }
        }
        else
        {
            pSubscriptionArray[ index ].topicFilterLength = 0;

            pSubscriptionArray[ index ].unsubscribed = true;
        }

        index++;
    }
}

/*-----------------------------------------------------------*/

int8_t IotMqtt_FindFirstMatch( _mqttSubscription_t * pSubscriptionArray,
                               int8_t startIndex,
                               _topicMatchParams_t * pMatch )
{
    /* This function must not be called with a NULL pSubscriptionArray parameter. */
    IotMqtt_Assert( pSubscriptionArray != NULL );
    IotMqtt_Assert( startIndex >= 0 );

    /* Finding the first Match. */
    while( startIndex < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        /* Check whether the subscription match the given params. */
        if( _topicMatch( &( pSubscriptionArray[ startIndex ] ), pMatch ) == true )
        {
            break;
        }

        startIndex++;
    }

    if( startIndex == MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        startIndex = -1;
    }

    return startIndex;
}

/*-----------------------------------------------------------*/

/* Provide access to internal functions and variables if testing. */
#if IOT_BUILD_TESTS == 1
    #include "iot_test_access_mqtt_subscription.c"
#endif
