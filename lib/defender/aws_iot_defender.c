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

#include <stdbool.h>

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

#include "platform/aws_iot_network.h"

#include "platform/aws_iot_clock.h"

/* helper macros to convert seconds to milliseconds and vice versa. */
#define _toMilliseconds( secondValue )    ( secondValue ) * 1000

#define _toSeconds( millisecondValue )    ( millisecondValue ) / 1000

/* Period between reports in milliseconds. */
static uint64_t _periodMilliSecond = _toMilliseconds( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );

/* the timer that connect, subscribe, publish metrics report */
static AwsIotTimer_t _metricsPublishTimer;

/* the timer that disconnect */
static AwsIotTimer_t _guardTimer;

/* defender internally manages network and mqtt connection */
static AwsIotNetworkConnection_t _networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;

static AwsIotMqttConnection_t _mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;

static AwsIotDefenderStartInfo_t _startInfo = { 0 };

static char * _pPublishTopic = NULL;

static char * _pAcceptTopic = NULL;

static char * _pRejectTopic = NULL;

/**
 * Return the state that whether defender is already started.
 */
static bool isStarted();

/**
 * Construct topics name based on the "thing name" user provided
 */
static bool _setTopicNames();

static bool _networkConnect( AwsIotDefenderEventType_t * pEventType );

static bool _setMqttCallback( AwsIotDefenderEventType_t * pEventType );

static bool _mqttConnect( AwsIotDefenderEventType_t * pEventType );

static bool _mqttSubscribe( AwsIotDefenderEventType_t * pEventType );

static bool _mqttPublish( uint8_t * pData,
                          size_t dataLength,
                          AwsIotDefenderEventType_t * pEventType );

static void _closeAndDestroyConnection();

/**
 * callback registerd on accept topic
 */
void _acceptCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish );

/**
 * callback registerd on reject topic
 */
void _rejectCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish );

/**
 * Create and arm _metricsPublishTimer
 */
static void _metricsPublishTimerExpirationRoutine( void * );
static void _guardTimerExpirationRoutine( void * );

/*-----------------------------------------------------------*/

static bool isStarted()
{
    return _startInfo.pThingName != NULL && _startInfo.thingNameLength > 0;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo )
{
    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    bool setTopicNamesSuccess = false,
         publishTimerCreateSuccess = false,
         publishTimerArmSuccess = false,
         guradTimerCreateSuccess = false;

    /* if current state is not started */
    if( !isStarted() )
    {
        /* copy input start info into global variable _startInfo */
        _startInfo = *pStartInfo;

        setTopicNamesSuccess = _setTopicNames();

        if( setTopicNamesSuccess )
        {
            publishTimerCreateSuccess = AwsIotClock_TimerCreate( &_metricsPublishTimer, _metricsPublishTimerExpirationRoutine, NULL );
        }

        if( publishTimerCreateSuccess )
        {
            guradTimerCreateSuccess = AwsIotClock_TimerCreate( &_guardTimer, _guardTimerExpirationRoutine, NULL );
        }

        if( guradTimerCreateSuccess )
        {
            publishTimerArmSuccess = AwsIotClock_TimerArm( &_metricsPublishTimer,
                                                           _DEFENDER_TEST_MODE ? _SHORT_RELATIVE_MILLISECONDS : _periodMilliSecond,
                                                           _periodMilliSecond );
        }

        if( publishTimerArmSuccess )
        {
            defenderError = AWS_IOT_DEFENDER_SUCCESS;
        }
    }
    else
    {
        defenderError = AWS_IOT_DEFENDER_ALREADY_STARTED;
    }

    /* do the cleanup jobs if not success */
    if( defenderError != AWS_IOT_DEFENDER_SUCCESS )
    {
        _startInfo = ( AwsIotDefenderStartInfo_t ) {
            0
        };

        if( setTopicNamesSuccess )
        {
            vPortFree( _pPublishTopic );
            vPortFree( _pAcceptTopic );
            vPortFree( _pRejectTopic );
            _pPublishTopic = NULL;
            _pAcceptTopic = NULL;
            _pRejectTopic = NULL;
        }

        if( publishTimerCreateSuccess )
        {
            AwsIotClock_TimerDestroy( &_metricsPublishTimer );
        }

        if( guradTimerCreateSuccess )
        {
            AwsIotClock_TimerDestroy( &_guardTimer );
        }
    }

    return defenderError;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Stop( void )
{
    _startInfo = ( AwsIotDefenderStartInfo_t ) {
        0
    };

    /* TODO: currently it assumes the connections are disposed properly */
    AwsIotClock_TimerDestroy( &_metricsPublishTimer );
    AwsIotClock_TimerDestroy( &_guardTimer );

    vPortFree( _pPublishTopic );
    vPortFree( _pAcceptTopic );
    vPortFree( _pRejectTopic );
    _pPublishTopic = NULL;
    _pAcceptTopic = NULL;
    _pRejectTopic = NULL;

    return AWS_IOT_DEFENDER_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint64_t periodSeconds )
{
    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    if( ( periodSeconds < AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS ) && !_DEFENDER_TEST_MODE )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_SHORT;
    }
    else if( periodSeconds > _toSeconds( UINT64_MAX ) )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_LARGE;
    }
    else
    {
        _periodMilliSecond = _toMilliseconds( periodSeconds );

        if( isStarted() )
        {
            if( AwsIotClock_TimerArm( &_metricsPublishTimer, _DEFENDER_TEST_MODE ? _SHORT_RELATIVE_MILLISECONDS : _periodMilliSecond, _periodMilliSecond ) )
            {
                defenderError = AWS_IOT_DEFENDER_SUCCESS;
            }
        }
        else
        {
            /* if defender is not started, simply return success */
            defenderError = AWS_IOT_DEFENDER_SUCCESS;
        }
    }

    return defenderError;
}

/*-----------------------------------------------------------*/

uint64_t AwsIotDefender_GetPeriod( void )
{
    return _toSeconds( _periodMilliSecond );
}

/*-----------------------------------------------------------*/

static void _metricsPublishTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    AwsIotDefenderEventType_t eventType = AWS_IOT_DEFENDER_METRICS_ACCEPTED;
    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _networkConnect( &eventType ) )
    {
        if( _setMqttCallback( &eventType ) )
        {
            if( _mqttConnect( &eventType ) )
            {
                if( _mqttSubscribe( &eventType ) )
                {
                    /* TODO: this will be the serialized metrics report */
                    char * pReport = "payload";
                    _mqttPublish( pReport, 8, &eventType );

                    /* TODO: this will be removed once I have real implementation of report */
                    _closeAndDestroyConnection();
                }
            }
        }
    }

    if( eventType != AWS_IOT_DEFENDER_METRICS_ACCEPTED )
    {
        callbackInfo.eventType = eventType;

        /* no message to be given to user's callback */
        callbackInfo.pPayload = NULL;
        callbackInfo.payloadLength = 0;

        if( _startInfo.callback.function != NULL )
        {
            _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
        }

        _closeAndDestroyConnection();
    }
}

/*-----------------------------------------------------------*/

static void _guardTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    _closeAndDestroyConnection();
}

/*-----------------------------------------------------------*/

static bool _networkConnect( AwsIotDefenderEventType_t * pEventType )
{
    bool connectSuccessful = false;

    if( AwsIotNetwork_CreateConnection( &_networkConnection,
                                        _startInfo.pAwsIotEndpoint,
                                        _startInfo.port,
                                        &( _startInfo.tlsInfo ) ) == AWS_IOT_NETWORK_SUCCESS )
    {
        connectSuccessful = true;
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_NETWORK_CONNECTION_FAILED;
    }

    return connectSuccessful;
}

/* SetMqttReceiveCallback must be invoked to have MQTT agent receive response from broker.
 * TODO: this will be simplified.
 */
static bool _setMqttCallback( AwsIotDefenderEventType_t * pEventType )
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

static bool _mqttConnect( AwsIotDefenderEventType_t * pEventType )
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
    connectInfo.keepAliveSeconds = _KEEP_ALIVE_SECONDS;
    connectInfo.pClientIdentifier = _startInfo.pThingName;
    connectInfo.clientIdentifierLength = _startInfo.thingNameLength;

    if( AwsIotMqtt_Connect(
            &_mqttConnection,
            &networkInterface,
            &connectInfo,
            NULL,
            _toMilliseconds( AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS ) ) == AWS_IOT_MQTT_SUCCESS )
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

static bool _mqttSubscribe( AwsIotDefenderEventType_t * pEventType )
{
    bool subscribeSuccessful = false;

    AwsIotMqttSubscription_t subscriptions[ 2 ] = { AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    subscriptions[ 0 ].QoS = 0;
    subscriptions[ 0 ].pTopicFilter = _pAcceptTopic;
    subscriptions[ 0 ].topicFilterLength = ( uint16_t ) strlen( _pAcceptTopic );
    subscriptions[ 0 ].callback.function = _acceptCallback;

    subscriptions[ 1 ].QoS = 0;
    subscriptions[ 1 ].pTopicFilter = _pRejectTopic;
    subscriptions[ 1 ].topicFilterLength = ( uint16_t ) strlen( _pRejectTopic );
    subscriptions[ 1 ].callback.function = _rejectCallback;

    if( AwsIotMqtt_TimedSubscribe( _mqttConnection,
                                   subscriptions,
                                   2,
                                   0,
                                   _toMilliseconds( AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS ) ) == AWS_IOT_MQTT_SUCCESS )
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

void _acceptCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;
    ( void ) pPublish;

    /* TODO: invoke user's callback */

    AwsIotClock_TimerArm( &_guardTimer, _SHORT_RELATIVE_MILLISECONDS, 0 );
}

/*-----------------------------------------------------------*/

void _rejectCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;
    ( void ) pPublish;

    /* TODO: invoke user's callback */

    AwsIotClock_TimerArm( &_guardTimer, _SHORT_RELATIVE_MILLISECONDS, 0 );
}

/*-----------------------------------------------------------*/

static bool _mqttPublish( uint8_t * pData,
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
                                 _toMilliseconds( AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS ) ) == AWS_IOT_MQTT_SUCCESS )
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

static void _closeAndDestroyConnection()
{
    AwsIotMqtt_Disconnect( _mqttConnection, false );
    AwsIotNetwork_DestroyConnection( _networkConnection );
}

/*-----------------------------------------------------------*/

static bool _setTopicNames()
{
    /* calculate topics lengths. */
    size_t publishTopicLength = strlen( _TOPIC_PREFIX ) + _startInfo.thingNameLength + strlen( _TOPIC_SUFFIX_PUBLISH ) + 1;
    size_t acceptTopicLength = strlen( _TOPIC_PREFIX ) + _startInfo.thingNameLength + strlen( _TOPIC_SUFFIX_ACCEPTED ) + 1;
    size_t rejectTopicLength = strlen( _TOPIC_PREFIX ) + _startInfo.thingNameLength + strlen( _TOPIC_SUFFIX_REJECTED ) + 1;

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
    }
    else
    {
        _pPublishTopic = pPublishTopic;
        _pAcceptTopic = pAcceptTopic;
        _pRejectTopic = pRejectTopic;

        strcpy( _pPublishTopic, _TOPIC_PREFIX );
        strncat( _pPublishTopic, _startInfo.pThingName, _startInfo.thingNameLength );
        strcat( _pPublishTopic, _TOPIC_SUFFIX_PUBLISH );

        strcpy( _pAcceptTopic, _TOPIC_PREFIX );
        strncat( _pAcceptTopic, _startInfo.pThingName, _startInfo.thingNameLength );
        strcat( _pAcceptTopic, _TOPIC_SUFFIX_ACCEPTED );

        strcpy( _pRejectTopic, _TOPIC_PREFIX );
        strncat( _pRejectTopic, _startInfo.pThingName, _startInfo.thingNameLength );
        strcat( _pRejectTopic, _TOPIC_SUFFIX_REJECTED );

        result = true;
    }

    return result;
}
