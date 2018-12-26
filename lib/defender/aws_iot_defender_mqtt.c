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

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

/*
 * 30 seconds is the minimum allowed in AFR MQTT
 */
#define _DEFENDER_MQTT_KEEP_ALIVE_SECONDS             30

/* Define topics segments used by defender. */
#define _TOPIC_PREFIX             "$aws/things/"

#define _TOPIC_SUFFIX_PUBLISH     "/defender/metrics/" _DEFENDER_FORMAT

#define _TOPIC_SUFFIX_ACCEPTED    _TOPIC_SUFFIX_PUBLISH "/accepted"

#define _TOPIC_SUFFIX_REJECTED    _TOPIC_SUFFIX_PUBLISH "/rejected"

/* defender internally manages network and mqtt connection */
static AwsIotNetworkConnection_t _networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
static AwsIotMqttConnection_t _mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;

static char * _pPublishTopic = NULL;

static char * _pAcceptTopic = NULL;

static char * _pRejectTopic = NULL;

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_BuildTopicsNames( const char * pThingName,
                                              uint16_t thingNameLength,
                                              AwsIotDefenderError_t * error )
{
    /* calculate topics lengths. */
    size_t publishTopicLength = strlen( _TOPIC_PREFIX ) + thingNameLength + strlen( _TOPIC_SUFFIX_PUBLISH ) + 1;
    size_t acceptTopicLength = strlen( _TOPIC_PREFIX ) + thingNameLength + strlen( _TOPIC_SUFFIX_ACCEPTED ) + 1;
    size_t rejectTopicLength = strlen( _TOPIC_PREFIX ) + thingNameLength + strlen( _TOPIC_SUFFIX_REJECTED ) + 1;

    /* allocate memory for each of them. */
    char * pPublishTopic = pvPortMalloc( publishTopicLength * sizeof( char ) );
    char * pAcceptTopic = pvPortMalloc( acceptTopicLength * sizeof( char ) );
    char * pRejectTopic = pvPortMalloc( rejectTopicLength * sizeof( char ) );

    bool result = false;

    /* free memory if any allocation failed */
    if( ( pPublishTopic == NULL ) || ( pAcceptTopic == NULL ) || ( pRejectTopic == NULL ) )
    {
        vPortFree( pPublishTopic );
        vPortFree( pAcceptTopic );
        vPortFree( pRejectTopic );
        result = false;
        *error = AWS_IOT_DEFENDER_ERROR_NO_MEMORY;
    }
    else
    {
        _pPublishTopic = pPublishTopic;
        _pAcceptTopic = pAcceptTopic;
        _pRejectTopic = pRejectTopic;

        strcpy( _pPublishTopic, _TOPIC_PREFIX );
        strncat( _pPublishTopic, pThingName, thingNameLength );
        strcat( _pPublishTopic, _TOPIC_SUFFIX_PUBLISH );

        strcpy( _pAcceptTopic, _TOPIC_PREFIX );
        strncat( _pAcceptTopic, pThingName, thingNameLength );
        strcat( _pAcceptTopic, _TOPIC_SUFFIX_ACCEPTED );

        strcpy( _pRejectTopic, _TOPIC_PREFIX );
        strncat( _pRejectTopic, pThingName, thingNameLength );
        strcat( _pRejectTopic, _TOPIC_SUFFIX_REJECTED );

        result = true;
    }

    return result;
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_DeleteTopicsNames()
{
    vPortFree( _pPublishTopic );
    vPortFree( _pAcceptTopic );
    vPortFree( _pRejectTopic );
    _pPublishTopic = NULL;
    _pAcceptTopic = NULL;
    _pRejectTopic = NULL;
}

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_NetworkConnect( const char * pAwsIotEndpoint,
                                            uint16_t port,
                                            AwsIotNetworkTlsInfo_t * pTlsInfo,
                                            AwsIotDefenderEventType_t * pEventType )
{
    bool connectSuccessful = false;

    if( AwsIotNetwork_CreateConnection( &_networkConnection,
                                        pAwsIotEndpoint,
                                        port,
                                        pTlsInfo ) == AWS_IOT_NETWORK_SUCCESS )
    {
        connectSuccessful = true;
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_NETWORK_CONNECTION_FAILED;
    }

    return connectSuccessful;
}

/**
 * TODO: this will be simplified.
 */
bool AwsIotDefenderInternal_SetMqttCallback( AwsIotDefenderEventType_t * pEventType )
{
    bool setCallbackSuccessful = false;

    if( AwsIotNetwork_SetMqttReceiveCallback(
            _networkConnection,
            &_mqttConnection,
            AwsIotMqtt_ReceiveCallback ) == AWS_IOT_NETWORK_SUCCESS )
    {
        setCallbackSuccessful = true;
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_NETWORK_SET_CALLBACK_FAILED;
    }

    return setCallbackSuccessful;
}

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_MqttConnect( const char * pThingName,
                                         uint16_t thingNameLength,
                                         AwsIotDefenderEventType_t * pEventType )
{
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;

    bool connectSuccessful = false;

    networkInterface.pDisconnectContext = ( void * ) _networkConnection;
    networkInterface.pSendContext = ( void * ) _networkConnection;
    networkInterface.disconnect = AwsIotNetwork_CloseConnection;
    networkInterface.send = AwsIotNetwork_Send;

    /* Set the members of the connection info (password and username not used). */
    connectInfo.cleanSession = true;
    connectInfo.keepAliveSeconds = _DEFENDER_MQTT_KEEP_ALIVE_SECONDS;
    connectInfo.pClientIdentifier = pThingName;
    connectInfo.clientIdentifierLength = thingNameLength;

    if( AwsIotMqtt_Connect(
            &_mqttConnection,
            &networkInterface,
            &connectInfo,
            NULL,
            _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS ) ) == AWS_IOT_MQTT_SUCCESS )
    {
        connectSuccessful = true;
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_MQTT_CONNECTION_FAILED;
    }

    return connectSuccessful;
}

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_MqttSubscribe( AwsIotMqttCallbackInfo_t acceptCallback,
                                           AwsIotMqttCallbackInfo_t rejectCallback,
                                           AwsIotDefenderEventType_t * pEventType )
{
    bool subscribeSuccessful = false;

    /* subscribe to two topics: accept and reject. */
    AwsIotMqttSubscription_t subscriptions[ 2 ] = { AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    subscriptions[ 0 ].QoS = 0;
    subscriptions[ 0 ].pTopicFilter = _pAcceptTopic;
    subscriptions[ 0 ].topicFilterLength = ( uint16_t ) strlen( _pAcceptTopic );
    subscriptions[ 0 ].callback.function = acceptCallback.function;
    subscriptions[ 0 ].callback.param1 = acceptCallback.param1;

    subscriptions[ 1 ].QoS = 0;
    subscriptions[ 1 ].pTopicFilter = _pRejectTopic;
    subscriptions[ 1 ].topicFilterLength = ( uint16_t ) strlen( _pRejectTopic );
    subscriptions[ 1 ].callback.function = rejectCallback.function;
    subscriptions[ 1 ].callback.param1 = rejectCallback.param1;

    if( AwsIotMqtt_TimedSubscribe( _mqttConnection,
                                   subscriptions,
                                   2,
                                   0,
                                   _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS ) ) == AWS_IOT_MQTT_SUCCESS )
    {
        subscribeSuccessful = true;
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_MQTT_SUBSCRIPTION_FAILED;
    }

    return subscribeSuccessful;
}

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_MqttPublish( uint8_t * pData,
                                         size_t dataLength,
                                         AwsIotDefenderEventType_t * pEventType )
{
    bool publishSuccessful = false;

    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    publishInfo.QoS = 0;
    publishInfo.pTopicName = _pPublishTopic;
    publishInfo.topicNameLength = ( uint16_t ) strlen( _pPublishTopic );
    publishInfo.pPayload = pData;
    publishInfo.payloadLength = dataLength;

    if( AwsIotMqtt_TimedPublish( _mqttConnection,
                                 &publishInfo,
                                 0,
                                 _defenderToMilliseconds( AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS ) ) == AWS_IOT_MQTT_SUCCESS )
    {
        publishSuccessful = true;
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_MQTT_PUBLISH_FAILED;
    }

    return publishSuccessful;
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_MqttDisconnect()
{
    AwsIotMqtt_Disconnect( _mqttConnection, false );
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_NetworkDestroy()
{
    AwsIotNetwork_DestroyConnection( _networkConnection );
}
