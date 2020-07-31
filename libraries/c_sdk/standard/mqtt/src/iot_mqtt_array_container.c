/*
 * FreeRTOS MQTT SHIM V1.0.0
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
 * http://aws.amazon.com/freertos
 * http ://www.FreeRTOS.org
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

static bool _packetMatch( _mqttSubscription_t * pSubscription,
                          _packetMatchParams_t * pMatch )
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

static bool _topicMatch( _mqttSubscription_t * pSubscription,
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
    const char * pTopicFilter = pSubscription->pTopicFilter;
    const uint16_t topicNameLength = pParam->topicNameLength;
    const uint16_t topicFilterLength = pSubscription->topicFilterLength;

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

int8_t IotMqtt_InsertSubscription( _mqttSubscription_t * pSubscriptionArray )
{
    int8_t index = 0;

    IotMqtt_Assert( pSubscriptionArray != NULL );

    /* Finding the free index in subscription array to insert the subscription. */
    while( index < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        if( pSubscriptionArray[ index ].i == 0 )
        {
            pSubscriptionArray[ index ].i = 1;
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

void IotMqtt_RemoveSubscription( _mqttSubscription_t * pSubscriptionArray,
                                 int8_t deleteIndex )
{
    IotMqtt_Assert( pSubscriptionArray != NULL );

    /* Remove the subscription from the subscription array. */
    pSubscriptionArray[ deleteIndex ].i = 0;
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveAllMatches( _mqttSubscription_t * pSubscriptionArray,
                               _packetMatchParams_t * pMatch )
{
    size_t i = 0;

    IotMqtt_Assert( pSubscriptionArray != NULL );

    if( pMatch != NULL )
    {
        while( i < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
        {
            /* Removing the subscription if it matches the given params. */
            if( _packetMatch( &( pSubscriptionArray[ i ] ), pMatch ) == true )
            {
                pSubscriptionArray[ i ].i = 0;
            }

            i++;
        }
    }
    else
    {
        /* Removing all the subscriptions from the subscription array. */
        while( i < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
        {
            pSubscriptionArray[ i ].i == 0;

            pSubscriptionArray[ i ].unsubscribed = true;
            i++;
        }
    }
}

/*-----------------------------------------------------------*/

int8_t IotMqtt_FindFirstMatch( _mqttSubscription_t * pSubscriptionArray,
                               size_t startIndex,
                               _topicMatchParams_t * pMatch )
{
    /* This function must not be called with a NULL pSubscriptionArray parameter. */
    IotMqtt_Assert( pSubscriptionArray != NULL );

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

void IotMqtt_InsertOperation( _mqttOperation_t ** pOperationArray,
                              _mqttOperation_t * pOperation )
{
    size_t i = 0;

    IotMqtt_Assert( pOperationArray != NULL );
    IotMqtt_Assert( pOperation != NULL );

    /* Finding the free index in operation array to insert the operation. */
    for( i = 0; i < MAX_NO_OF_MQTT_OPERATIONS; i++ )
    {
        if( pOperationArray[ i ] == NULL )
        {
            pOperationArray[ i ] = pOperation;
            break;
        }
    }
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveOperation( _mqttOperation_t ** pOperationArray,
                              _mqttOperation_t * pOperation )
{
    size_t i = 0;

    IotMqtt_Assert( pOperationArray != NULL );
    IotMqtt_Assert( pOperation != NULL );

    /* Finding the index matching the input operation to remove it from the operation array. */
    for( i = 0; i < MAX_NO_OF_MQTT_OPERATIONS; i++ )
    {
        if( pOperationArray[ i ] == pOperation )
        {
            pOperationArray[ i ] = NULL;
            break;
        }
    }
}

/*-----------------------------------------------------------*/

_mqttOperation_t * IotMqtt_FindFirstMatchOperation( _mqttOperation_t ** pOperationArray,
                                                    _mqttOperation_t * pStartPoint,
                                                    bool ( * isMatch )( _mqttOperation_t *, void * ),
                                                    void * pMatch )
{
    _mqttOperation_t * pCurrent = ( _mqttOperation_t * ) pStartPoint;
    _mqttOperation_t * pMatchedOperation = NULL;
    size_t nextIndex = 0;

    /* This function must not be called with a NULL pOperationArray parameter. */
    IotMqtt_Assert( pOperationArray != NULL );

    /* Find the next Index to start searching for match. */
    if( pCurrent == NULL )
    {
        nextIndex = 0;
    }
    else
    {
        /* Getting the next index after the starting point to continue matching for other opeartions. */
        while( nextIndex < MAX_NO_OF_MQTT_OPERATIONS )
        {
            if( pOperationArray[ nextIndex ] == pCurrent )
            {
                nextIndex = nextIndex + 1;
                break;
            }

            nextIndex++;
        }
    }

    /* Finding the first Match. */
    while( nextIndex < MAX_NO_OF_MQTT_OPERATIONS )
    {
        if( pOperationArray[ nextIndex ] != NULL )
        {
            if( isMatch != NULL )
            {
                if( isMatch( pOperationArray[ nextIndex ], pMatch ) == true )
                {
                    pMatchedOperation = pOperationArray[ nextIndex ];
                    break;
                }
            }
            else
            {
                if( pOperationArray[ nextIndex ] == pMatch )
                {
                    pMatchedOperation = pOperationArray[ nextIndex ];
                    break;
                }
            }
        }

        nextIndex++;
    }

    return pMatchedOperation;
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveAllOperation( _mqttOperation_t ** pOperationArray,
                                 void ( * freeElement )( void * ) )
{
    size_t i = 0;

    IotMqtt_Assert( pOperationArray != NULL );

    for( i = 0; i < MAX_NO_OF_MQTT_OPERATIONS; i++ )
    {
        if( pOperationArray[ i ] != NULL )
        {
            if( freeElement != NULL )
            {
                freeElement( ( pOperationArray[ i ] ) );
            }

            pOperationArray[ i ] = NULL;
        }
    }
}

/*-----------------------------------------------------------*/
