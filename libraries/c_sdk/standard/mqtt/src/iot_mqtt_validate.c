/*
 * Amazon FreeRTOS MQTT V2.1.0
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_validate.c
 * @brief Implements functions that validate the structs of the MQTT library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/*-----------------------------------------------------------*/

bool _IotMqtt_ValidateConnect( const IotMqttConnectInfo_t * pConnectInfo )
{
    IOT_FUNCTION_ENTRY( bool, true );

    /* Check for NULL. */
    if( pConnectInfo == NULL )
    {
        IotLogError( "MQTT connection information cannot be NULL." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check that a client identifier was set. */
    if( pConnectInfo->pClientIdentifier == NULL )
    {
        IotLogError( "Client identifier must be set." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check for a zero-length client identifier. Zero-length client identifiers
     * are not allowed with clean sessions. */
    if( pConnectInfo->clientIdentifierLength == 0 )
    {
        IotLogWarn( "A zero-length client identifier was provided." );

        if( pConnectInfo->cleanSession == true )
        {
            IotLogError( "A zero-length client identifier cannot be used with a clean session." );

            IOT_SET_AND_GOTO_CLEANUP( false );
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

    /* Check that the number of persistent session subscriptions is valid. */
    if( pConnectInfo->cleanSession == false )
    {
        if( pConnectInfo->pPreviousSubscriptions != NULL )
        {
            if( pConnectInfo->previousSubscriptionCount == 0 )
            {
                IotLogError( "Previous subscription count cannot be 0." );

                IOT_SET_AND_GOTO_CLEANUP( false );
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

    /* In MQTT 3.1.1, servers are not obligated to accept client identifiers longer
     * than 23 characters. */
    if( pConnectInfo->clientIdentifierLength > 23 )
    {
        IotLogWarn( "A client identifier length of %hu is longer than 23, which is "
                    "the longest client identifier a server must accept.",
                    pConnectInfo->clientIdentifierLength );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check for compatibility with the AWS IoT MQTT service limits. */
    if( pConnectInfo->awsIotMqttMode == true )
    {
        /* Check that client identifier is within the service limit. */
        if( pConnectInfo->clientIdentifierLength > AWS_IOT_MQTT_SERVER_MAX_CLIENTID )
        {
            IotLogError( "AWS IoT does not support client identifiers longer than %d bytes.",
                         AWS_IOT_MQTT_SERVER_MAX_CLIENTID );

            IOT_SET_AND_GOTO_CLEANUP( false );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        /* Check for compliance with AWS IoT keep-alive limits. */
        if( pConnectInfo->keepAliveSeconds == 0 )
        {
            IotLogWarn( "AWS IoT does not support disabling keep-alive. Default keep-alive "
                        "of %d seconds will be used.",
                        AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE );
        }
        else if( pConnectInfo->keepAliveSeconds < AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE )
        {
            IotLogWarn( "AWS IoT does not support keep-alive intervals less than %d seconds. "
                        "An interval of %d seconds will be used.",
                        AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE,
                        AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE );
        }
        else if( pConnectInfo->keepAliveSeconds > AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE )
        {
            IotLogWarn( "AWS IoT does not support keep-alive intervals greater than %d seconds. "
                        "An interval of %d seconds will be used.",
                        AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE,
                        AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE );
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

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

bool _IotMqtt_ValidatePublish( bool awsIotMqttMode,
                               const IotMqttPublishInfo_t * pPublishInfo )
{
    IOT_FUNCTION_ENTRY( bool, true );

    /* Check for NULL. */
    if( pPublishInfo == NULL )
    {
        IotLogError( "Publish information cannot be NULL." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check topic name for NULL or zero-length. */
    if( pPublishInfo->pTopicName == NULL )
    {
        IotLogError( "Publish topic name must be set." );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    if( pPublishInfo->topicNameLength == 0 )
    {
        IotLogError( "Publish topic name length cannot be 0." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Only allow NULL payloads with zero length. */
    if( pPublishInfo->pPayload == NULL )
    {
        if( pPublishInfo->payloadLength != 0 )
        {
            IotLogError( "Nonzero payload length cannot have a NULL payload." );

            IOT_SET_AND_GOTO_CLEANUP( false );
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

    /* Check for a valid QoS. */
    if( pPublishInfo->qos != IOT_MQTT_QOS_0 )
    {
        if( pPublishInfo->qos != IOT_MQTT_QOS_1 )
        {
            IotLogError( "Publish QoS must be either 0 or 1." );

            IOT_SET_AND_GOTO_CLEANUP( false );
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

    /* Check the retry parameters. */
    if( pPublishInfo->retryLimit > 0 )
    {
        if( pPublishInfo->retryMs == 0 )
        {
            IotLogError( "Publish retry time must be positive." );

            IOT_SET_AND_GOTO_CLEANUP( false );
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

    /* Check for compatibility with AWS IoT MQTT server. */
    if( awsIotMqttMode == true )
    {
        /* Check for retained message. */
        if( pPublishInfo->retain == true )
        {
            IotLogError( "AWS IoT does not support retained publish messages." );

            IOT_SET_AND_GOTO_CLEANUP( false );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        /* Check topic name length. */
        if( pPublishInfo->topicNameLength > AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH )
        {
            IotLogError( "AWS IoT does not support topic names longer than %d bytes.",
                         AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH );

            IOT_SET_AND_GOTO_CLEANUP( false );
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

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

bool _IotMqtt_ValidateOperation( IotMqttOperation_t operation )
{
    IOT_FUNCTION_ENTRY( bool, true );

    /* Check for NULL. */
    if( operation == NULL )
    {
        IotLogError( "Operation reference cannot be NULL." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* Check that reference is waitable. */
    if( ( operation->u.operation.flags & IOT_MQTT_FLAG_WAITABLE ) != IOT_MQTT_FLAG_WAITABLE )
    {
        IotLogError( "Operation is not waitable." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

bool _IotMqtt_ValidateSubscriptionList( IotMqttOperationType_t operation,
                                        bool awsIotMqttMode,
                                        const IotMqttSubscription_t * pListStart,
                                        size_t listSize )
{
    IOT_FUNCTION_ENTRY( bool, true );
    size_t i = 0;
    uint16_t j = 0;
    const IotMqttSubscription_t * pListElement = NULL;

    /* Operation must be either subscribe or unsubscribe. */
    IotMqtt_Assert( ( operation == IOT_MQTT_SUBSCRIBE ) ||
                    ( operation == IOT_MQTT_UNSUBSCRIBE ) );

    /* Check for empty list. */
    if( pListStart == NULL )
    {
        IotLogError( "Subscription list pointer cannot be NULL." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    if( listSize == 0 )
    {
        IotLogError( "Empty subscription list." );

        IOT_SET_AND_GOTO_CLEANUP( false );
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    /* AWS IoT supports at most 8 topic filters in a single SUBSCRIBE packet. */
    if( awsIotMqttMode == true )
    {
        if( listSize > AWS_IOT_MQTT_SERVER_MAX_TOPIC_FILTERS_PER_SUBSCRIBE )
        {
            IotLogError( "AWS IoT does not support more than %d topic filters per "
                         "subscription request.",
                         AWS_IOT_MQTT_SERVER_MAX_TOPIC_FILTERS_PER_SUBSCRIBE );

            IOT_SET_AND_GOTO_CLEANUP( false );
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

    for( i = 0; i < listSize; i++ )
    {
        pListElement = &( pListStart[ i ] );

        /* Check for a valid QoS and callback function when subscribing. */
        if( operation == IOT_MQTT_SUBSCRIBE )
        {
            if( pListElement->qos != IOT_MQTT_QOS_0 )
            {
                if( pListElement->qos != IOT_MQTT_QOS_1 )
                {
                    IotLogError( "Subscription QoS must be either 0 or 1." );

                    IOT_SET_AND_GOTO_CLEANUP( false );
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

            if( pListElement->callback.function == NULL )
            {
                IotLogError( "Callback function must be set." );

                IOT_SET_AND_GOTO_CLEANUP( false );
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

        /* Check subscription topic filter. */
        if( pListElement->pTopicFilter == NULL )
        {
            IotLogError( "Subscription topic filter cannot be NULL." );

            IOT_SET_AND_GOTO_CLEANUP( false );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        if( pListElement->topicFilterLength == 0 )
        {
            IotLogError( "Subscription topic filter length cannot be 0." );

            IOT_SET_AND_GOTO_CLEANUP( false );
        }
        else
        {
            EMPTY_ELSE_MARKER;
        }

        /* Check for compatibility with AWS IoT MQTT server. */
        if( awsIotMqttMode == true )
        {
            /* Check topic filter length. */
            if( pListElement->topicFilterLength > AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH )
            {
                IotLogError( "AWS IoT does not support topic filters longer than %d bytes.",
                             AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH );

                IOT_SET_AND_GOTO_CLEANUP( false );
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

        /* Check that the wildcards '+' and '#' are being used correctly. */
        for( j = 0; j < pListElement->topicFilterLength; j++ )
        {
            switch( pListElement->pTopicFilter[ j ] )
            {
                /* Check that the single level wildcard '+' is used correctly. */
                case '+':

                    /* Unless '+' is the first character in the filter, it must be preceded by '/'. */
                    if( j > 0 )
                    {
                        if( pListElement->pTopicFilter[ j - 1 ] != '/' )
                        {
                            IotLogError( "Invalid topic filter %.*s -- '+' must be preceded by '/'.",
                                         pListElement->topicFilterLength,
                                         pListElement->pTopicFilter );

                            IOT_SET_AND_GOTO_CLEANUP( false );
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

                    /* Unless '+' is the last character in the filter, it must be succeeded by '/'. */
                    if( j < pListElement->topicFilterLength - 1 )
                    {
                        if( pListElement->pTopicFilter[ j + 1 ] != '/' )
                        {
                            IotLogError( "Invalid topic filter %.*s -- '+' must be succeeded by '/'.",
                                         pListElement->topicFilterLength,
                                         pListElement->pTopicFilter );

                            IOT_SET_AND_GOTO_CLEANUP( false );
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

                    break;

                /* Check that the multi-level wildcard '#' is used correctly. */
                case '#':

                    /* '#' must be the last character in the filter. */
                    if( j != pListElement->topicFilterLength - 1 )
                    {
                        IotLogError( "Invalid topic filter %.*s -- '#' must be the last character.",
                                     pListElement->topicFilterLength,
                                     pListElement->pTopicFilter );

                        IOT_SET_AND_GOTO_CLEANUP( false );
                    }
                    else
                    {
                        EMPTY_ELSE_MARKER;
                    }

                    /* Unless '#' is standalone, it must be preceded by '/'. */
                    if( pListElement->topicFilterLength > 1 )
                    {
                        if( pListElement->pTopicFilter[ j - 1 ] != '/' )
                        {
                            IotLogError( "Invalid topic filter %.*s -- '#' must be preceded by '/'.",
                                         pListElement->topicFilterLength,
                                         pListElement->pTopicFilter );

                            IOT_SET_AND_GOTO_CLEANUP( false );
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

                    break;

                default:
                    break;
            }
        }
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/
