/*
 * Amazon FreeRTOS Defender V2.0.1
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

#include <string.h>

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

/* Define topics segments used by defender. */
#define TOPIC_PREFIX             "$aws/things/"

#define TOPIC_SUFFIX_PUBLISH     "/defender/metrics/" DEFENDER_FORMAT

#define TOPIC_SUFFIX_ACCEPTED    TOPIC_SUFFIX_PUBLISH "/accepted"

#define TOPIC_SUFFIX_REJECTED    TOPIC_SUFFIX_PUBLISH "/rejected"

extern AwsIotDefenderStartInfo_t _startInfo;

/* defender internally manages network and mqtt connection */
static IotMqttConnection_t _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

static char * _pPublishTopic = NULL;

static char * _pAcceptTopic = NULL;

static char * _pRejectTopic = NULL;

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefenderInternal_BuildTopicsNames( void )
{
    AwsIotDefenderError_t returnedError = AWS_IOT_DEFENDER_SUCCESS;

    const char * pThingName = _startInfo.mqttConnectionInfo.pClientIdentifier;
    uint16_t thingNameLength = _startInfo.mqttConnectionInfo.clientIdentifierLength;

    /* Calculate topics lengths. Plus one for string terminator. */
    size_t publishTopicLength = strlen( TOPIC_PREFIX ) + thingNameLength + strlen( TOPIC_SUFFIX_PUBLISH ) + 1;
    size_t acceptTopicLength = strlen( TOPIC_PREFIX ) + thingNameLength + strlen( TOPIC_SUFFIX_ACCEPTED ) + 1;
    size_t rejectTopicLength = strlen( TOPIC_PREFIX ) + thingNameLength + strlen( TOPIC_SUFFIX_REJECTED ) + 1;

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

        strcpy( _pPublishTopic, TOPIC_PREFIX );
        strncat( _pPublishTopic, pThingName, thingNameLength );
        strcat( _pPublishTopic, TOPIC_SUFFIX_PUBLISH );

        strcpy( _pAcceptTopic, TOPIC_PREFIX );
        strncat( _pAcceptTopic, pThingName, thingNameLength );
        strcat( _pAcceptTopic, TOPIC_SUFFIX_ACCEPTED );

        strcpy( _pRejectTopic, TOPIC_PREFIX );
        strncat( _pRejectTopic, pThingName, thingNameLength );
        strcat( _pRejectTopic, TOPIC_SUFFIX_REJECTED );
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
}

/*-----------------------------------------------------------*/

IotMqttError_t AwsIotDefenderInternal_MqttConnect( void )
{
    return IotMqtt_Connect( &_startInfo.mqttNetworkInfo,
                            &_startInfo.mqttConnectionInfo,
                            _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS ),
                            &_mqttConnection );
}

/*-----------------------------------------------------------*/

IotMqttError_t AwsIotDefenderInternal_MqttSubscribe( IotMqttCallbackInfo_t acceptCallback,
                                                     IotMqttCallbackInfo_t rejectCallback )
{
    /* subscribe to two topics: accept and reject. */
    IotMqttSubscription_t subscriptions[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER, IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    subscriptions[ 0 ].qos = IOT_MQTT_QOS_0;
    subscriptions[ 0 ].pTopicFilter = _pAcceptTopic;
    subscriptions[ 0 ].topicFilterLength = ( uint16_t ) strlen( _pAcceptTopic );
    subscriptions[ 0 ].callback = acceptCallback;

    subscriptions[ 1 ].qos = IOT_MQTT_QOS_0;
    subscriptions[ 1 ].pTopicFilter = _pRejectTopic;
    subscriptions[ 1 ].topicFilterLength = ( uint16_t ) strlen( _pRejectTopic );
    subscriptions[ 1 ].callback = rejectCallback;

    return IotMqtt_TimedSubscribe( _mqttConnection,
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
    publishInfo.topicNameLength = ( uint16_t ) strlen( _pPublishTopic );
    publishInfo.pPayload = pData;
    publishInfo.payloadLength = dataLength;

    return IotMqtt_TimedPublish( _mqttConnection,
                                 &publishInfo,
                                 0,
                                 _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS ) );
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_MqttDisconnect( void )
{
    IotMqtt_Disconnect( _mqttConnection, false );
}
