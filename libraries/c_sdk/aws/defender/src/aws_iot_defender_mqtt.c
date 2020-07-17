/*
 * FreeRTOS Defender V3.0.2
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

#include <stdio.h>
#include <string.h>

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

/* Define topics segments used by defender. */
#define TOPIC_PREFIX             "$aws/things/"

#define TOPIC_SUFFIX_PUBLISH     "/defender/metrics/" DEFENDER_FORMAT

#define TOPIC_SUFFIX_ACCEPTED    TOPIC_SUFFIX_PUBLISH "/accepted"

#define TOPIC_SUFFIX_REJECTED    TOPIC_SUFFIX_PUBLISH "/rejected"

/*-----------------------------------------------------------*/

extern IotMqttCallbackInfo_t _acceptCallbackInfo;
extern IotMqttCallbackInfo_t _rejectCallbackInfo;
extern AwsIotDefenderStartInfo_t _startInfo;

static char * _pPublishTopic = NULL;
static size_t _publishTopicLength = 0;

static char * _pAcceptTopic = NULL;
static size_t _acceptTopicLength = 0;

static char * _pRejectTopic = NULL;
static size_t _rejectTopicLength = 0;

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefenderInternal_BuildTopicsNames( void )
{
    AwsIotDefenderError_t returnedError = AWS_IOT_DEFENDER_SUCCESS;

    const char * pThingName = _startInfo.pClientIdentifier;
    uint16_t thingNameLength = _startInfo.clientIdentifierLength;
    size_t topicPrefixLength = strlen( TOPIC_PREFIX );

    /* Calculate topics lengths. Plus one for string terminator. */
    size_t publishTopicLength = topicPrefixLength + thingNameLength + strlen( TOPIC_SUFFIX_PUBLISH ) + 1;
    size_t acceptTopicLength = topicPrefixLength + thingNameLength + strlen( TOPIC_SUFFIX_ACCEPTED ) + 1;
    size_t rejectTopicLength = topicPrefixLength + thingNameLength + strlen( TOPIC_SUFFIX_REJECTED ) + 1;

    /* Allocate memory for each of them. */
    char * pPublishTopic = AwsIotDefender_MallocTopic( publishTopicLength * sizeof( char ) );
    char * pAcceptTopic = AwsIotDefender_MallocTopic( acceptTopicLength * sizeof( char ) );
    char * pRejectTopic = AwsIotDefender_MallocTopic( rejectTopicLength * sizeof( char ) );

    /* Free memory if any allocation failed. */
    if( ( pPublishTopic == NULL ) || ( pAcceptTopic == NULL ) || ( pRejectTopic == NULL ) )
    {
        /* Null pointer is safe for "free" function. */
        AwsIotDefender_FreeTopic( pPublishTopic );
        AwsIotDefender_FreeTopic( pAcceptTopic );
        AwsIotDefender_FreeTopic( pRejectTopic );
        returnedError = AWS_IOT_DEFENDER_ERROR_NO_MEMORY;
    }
    else
    {
        _pPublishTopic = pPublishTopic;
        _pAcceptTopic = pAcceptTopic;
        _pRejectTopic = pRejectTopic;

        snprintf( _pPublishTopic, publishTopicLength, "%s%s%s",
                  TOPIC_PREFIX,
                  pThingName,
                  TOPIC_SUFFIX_PUBLISH );

        snprintf( _pAcceptTopic, acceptTopicLength, "%s%s%s",
                  TOPIC_PREFIX,
                  pThingName,
                  TOPIC_SUFFIX_ACCEPTED );

        snprintf( _pRejectTopic, rejectTopicLength, "%s%s%s",
                  TOPIC_PREFIX,
                  pThingName,
                  TOPIC_SUFFIX_REJECTED );

        _publishTopicLength = publishTopicLength - 1;
        _acceptTopicLength = acceptTopicLength - 1;
        _rejectTopicLength = rejectTopicLength - 1;
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_DeleteTopicsNames( void )
{
    AwsIotDefender_FreeTopic( _pPublishTopic );
    AwsIotDefender_FreeTopic( _pAcceptTopic );
    AwsIotDefender_FreeTopic( _pRejectTopic );
    _pPublishTopic = NULL;
    _pAcceptTopic = NULL;
    _pRejectTopic = NULL;

    _publishTopicLength = 0;
    _acceptTopicLength = 0;
    _rejectTopicLength = 0;
}

/*-----------------------------------------------------------*/

IotMqttError_t AwsIotDefenderInternal_MqttSubscribe( void )
{
    /* subscribe to two topics: accept and reject. */
    IotMqttSubscription_t subscriptions[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER, IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    subscriptions[ 0 ].qos = IOT_MQTT_QOS_0;
    subscriptions[ 0 ].pTopicFilter = _pAcceptTopic;
    subscriptions[ 0 ].topicFilterLength = ( uint16_t ) _acceptTopicLength;
    subscriptions[ 0 ].callback = _acceptCallbackInfo;

    subscriptions[ 1 ].qos = IOT_MQTT_QOS_0;
    subscriptions[ 1 ].pTopicFilter = _pRejectTopic;
    subscriptions[ 1 ].topicFilterLength = ( uint16_t ) _rejectTopicLength;
    subscriptions[ 1 ].callback = _rejectCallbackInfo;

    return IotMqtt_TimedSubscribe( _startInfo.mqttConnection,
                                   subscriptions,
                                   2,
                                   0,
                                   _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS ) );
}

/*-----------------------------------------------------------*/

IotMqttError_t AwsIotDefenderInternal_MqttPublish( uint8_t * pData,
                                                   size_t dataLength )
{
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    publishInfo.qos = IOT_MQTT_QOS_0;
    publishInfo.pTopicName = _pPublishTopic;
    publishInfo.topicNameLength = ( uint16_t ) _publishTopicLength;
    publishInfo.pPayload = pData;
    publishInfo.payloadLength = dataLength;
    /* Publish Defender Report */
    return IotMqtt_TimedPublish( _startInfo.mqttConnection,
                                 &publishInfo,
                                 0,
                                 _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS ) );
}

/*-----------------------------------------------------------*/

IotMqttError_t AwsIotDefenderInternal_MqttUnSubscribe( void )
{
    /* unsubscribe to two topics: accept and reject. */
    IotMqttSubscription_t subscriptions[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER, IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    subscriptions[ 0 ].qos = IOT_MQTT_QOS_0;
    subscriptions[ 0 ].pTopicFilter = _pAcceptTopic;
    subscriptions[ 0 ].topicFilterLength = ( uint16_t ) _acceptTopicLength;
    subscriptions[ 0 ].callback = _acceptCallbackInfo;

    subscriptions[ 1 ].qos = IOT_MQTT_QOS_0;
    subscriptions[ 1 ].pTopicFilter = _pRejectTopic;
    subscriptions[ 1 ].topicFilterLength = ( uint16_t ) _rejectTopicLength;
    subscriptions[ 1 ].callback = _rejectCallbackInfo;

    return IotMqtt_TimedUnsubscribe( _startInfo.mqttConnection,
                                     subscriptions,
                                     2,
                                     0,
                                     _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS ) );
}

/*-----------------------------------------------------------*/
