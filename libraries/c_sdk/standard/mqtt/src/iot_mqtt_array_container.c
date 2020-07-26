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

void IotMqtt_InsertSubscription( _mqttSubscription_t ** pSubscriptionArray,
                                 _mqttSubscription_t * pNewSubscription )
{
    size_t i = 0;

    /* Finding the free index in subscription array to insert the subscription. */
    for( i = 0; i < 20; i++ )
    {
        if( pSubscriptionArray[ i ] == NULL )
        {
            pSubscriptionArray[ i ] = pNewSubscription;
            break;
        }
    }
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveSubscription( _mqttSubscription_t ** pSubscriptionArray,
                                 _mqttSubscription_t * pSubscription )
{
    size_t i = 0;

    /* Finding the index matching the input subscription to remove it from the subscription array. */
    for( i = 0; i < 20; i++ )
    {
        if( pSubscriptionArray[ i ] == pSubscription )
        {
            pSubscriptionArray[ i ] = NULL;
            break;
        }
    }
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveAllMatches( _mqttSubscription_t ** pSubscriptionArray,
                               bool ( * isMatch )( _mqttSubscription_t *, void * ),
                               void * pMatch,
                               void ( * freeElement )( void * ) )
{
    _mqttSubscription_t * pMatchedElement = NULL;

    /* Search the Subscription Array for all matching elements. */
    do
    {
        pMatchedElement = IotMqtt_FindFirstMatch( pSubscriptionArray,
                                                  pMatchedElement,
                                                  isMatch,
                                                  pMatch );

        if( pMatchedElement != NULL )
        {
            /* Match found; remove and free. */
            IotMqtt_RemoveSubscription( pSubscriptionArray, pMatchedElement );

            if( freeElement != NULL )
            {
                freeElement( pMatchedElement );
            }
        }
    } while ( pMatchedElement != NULL );
}

/*-----------------------------------------------------------*/

_mqttSubscription_t * IotMqtt_FindFirstMatch( _mqttSubscription_t ** pSubscriptionArray,
                                              _mqttSubscription_t * pStartPoint,
                                              bool ( * isMatch )( _mqttSubscription_t *, void * ),
                                              void * pMatch )
{
    _mqttSubscription_t * pCurrent = ( _mqttSubscription_t * ) pStartPoint;
    _mqttSubscription_t * matchedSubscription = NULL;

    /* This function must not be called with a NULL pList parameter. */
    IotMqtt_Assert( pSubscriptionArray != NULL );

    /* Find the next Index to start searching for match. */
    size_t nextIndex = 0;

    if( pCurrent == NULL )
    {
        nextIndex = 0;
    }
    else
    {
        while( nextIndex < 20 )
        {
            if( pSubscriptionArray[ nextIndex ] == pCurrent )
            {
                nextIndex = nextIndex + 1;
                break;
            }

            nextIndex++;
        }
    }

    /* Finding the first Match. */
    while( nextIndex < 20 )
    {
        if( pSubscriptionArray[ nextIndex ] != NULL )
        {
            if( isMatch != NULL )
            {
                if( isMatch( pSubscriptionArray[ nextIndex ], pMatch ) == true )
                {
                    matchedSubscription = pSubscriptionArray[ nextIndex ];
                    break;
                }
            }
            else
            {
                if( pSubscriptionArray[ nextIndex ] == pMatch )
                {
                    matchedSubscription = pSubscriptionArray[ nextIndex ];
                    break;
                }
            }
        }

        nextIndex++;
    }

    /* No match found, return NULL. */
    return matchedSubscription;
}

/*-----------------------------------------------------------*/

void IotMqtt_InsertOperation( _mqttOperation_t ** pOperationArray,
                              _mqttOperation_t * pOperation )
{
    size_t i = 0;

    /* Finding the free index in operation array to insert the operation. */
    for( i = 0; i < 30; i++ )
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

    /* Finding the index matching the input operation to remove it from the operation array. */
    for( i = 0; i < 30; i++ )
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

    /* This function must not be called with a NULL pList parameter. */
    IotMqtt_Assert( pOperationArray != NULL );

    size_t nextIndex = 0;

    if( pCurrent == NULL )
    {
        nextIndex = 0;
    }
    else
    {
        for( size_t i = 0; i < 30; i++ )
        {
            if( pOperationArray[ i ] == pCurrent )
            {
                nextIndex = i + 1;
                break;
            }
        }
    }

    for( size_t i = nextIndex; i < 30; i++ )
    {
        if( pOperationArray[ i ] != NULL )
        {
            if( isMatch != NULL )
            {
                if( isMatch( pOperationArray[ i ], pMatch ) == true )
                {
                    return pOperationArray[ i ];
                }
            }
            else
            {
                if( pOperationArray[ i ] == pMatch )
                {
                    return pOperationArray[ i ];
                }
            }
        }
    }

    /* No match found, return NULL. */
    return NULL;
}

/*-----------------------------------------------------------*/

void IotMqtt_RemoveAllOperation( _mqttOperation_t ** pOperationArray,
                                 void ( * freeElement )( void * ) )
{
    size_t i = 0;

    for( i = 0; i < 30; i++ )
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
