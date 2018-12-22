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

/* Platform clock include. */
#include "platform/aws_iot_clock.h"

/*
 * Small time value to kick off timer immediately.
 */
#define _DEFENDER_SHORT_RELATIVE_MILLISECONDS    100

/*
 * This mode is supposed to be used by test, demo or debug.
 */
#ifndef _DEFENDER_TEST_MODE
    #define _DEFENDER_TEST_MODE    false
#endif

/**
 * Return the state that whether defender is already started.
 */
static bool isStarted();

/**
 * Disconnect MQTT and clean up network connection.
 */
static void _closeAndDestroyConnection();

/**
 * callback registerd on accept topic.
 */
void _acceptCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish );

/**
 * callback registerd on reject topic.
 */
void _rejectCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish );

/**
 * callback registerd on metrics publish timer.
 */
static void _metricsPublishTimerExpirationRoutine( void * );

/**
 * callback registerd on guard timer, which simply invoke _closeAndDestroyConnection.
 */
static void _guardTimerExpirationRoutine( void * );


/*------------------- Below are global variables. ---------------------------*/

uint32_t _AwsIotDefenderMetricsFlag[ _DEFENDER_METRICS_GROUP_COUNT ];

/**
 * Period between reports in milliseconds.
 * Set default value.
 */
static uint64_t _periodMilliSecond = _defenderToMilliseconds( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );

/* the timer that connect, subscribe, publish metrics report */
static AwsIotTimer_t _metricsPublishTimer;

/* the timer that disconnect */
static AwsIotTimer_t _guardTimer;

AwsIotDefenderStartInfo_t _startInfo = { 0 };

AwsIotNetworkTlsInfo_t _tlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;

/*-----------------------------------------------------------*/

static bool isStarted()
{
    /* if startInfo contains a non-NULL thing name, it implies the defender agent is running. */
    return _startInfo.pThingName != NULL && _startInfo.thingNameLength > 0;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetMetrics( AwsIotDefenderMetricsGroup_t metricsGroup,
                                                 uint32_t metrics )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;

    if( metricsGroup >= _DEFENDER_METRICS_GROUP_COUNT )
    {
        error = AWS_IOT_DEFENDER_INVALID_INPUT;
    }
    else
    {
        _AwsIotDefenderMetricsFlag[ metricsGroup ] = metrics;
    }

    return error;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo )
{
    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    /* initialize flow control states to false. */
    bool buildTopicsNamesSuccess = false,
         publishTimerCreateSuccess = false,
         publishTimerArmSuccess = false,
         guradTimerCreateSuccess = false;

    /* if current state is not started */
    if( !isStarted() )
    {
        /* copy input start info into global variable _startInfo */
        _startInfo = *pStartInfo;
        /* copy input tls info into global variable _tlsInfo */
        _tlsInfo = *( _startInfo.pTlsInfo );

        buildTopicsNamesSuccess = AwsIotDefenderInternal_BuildTopicsNames( _startInfo.pThingName, _startInfo.thingNameLength, &defenderError );

        if( buildTopicsNamesSuccess )
        {
            publishTimerCreateSuccess = AwsIotClock_TimerCreate( &_metricsPublishTimer, _metricsPublishTimerExpirationRoutine, NULL );
        }

        if( publishTimerCreateSuccess )
        {
            guradTimerCreateSuccess = AwsIotClock_TimerCreate( &_guardTimer, _guardTimerExpirationRoutine, NULL );
        }

        if( guradTimerCreateSuccess )
        {
            /* in test mode, the timer is kicked off almost immediately. */
            publishTimerArmSuccess = AwsIotClock_TimerArm( &_metricsPublishTimer,
                                                           _DEFENDER_TEST_MODE ? _DEFENDER_SHORT_RELATIVE_MILLISECONDS : _periodMilliSecond,
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

    /* Do the cleanup jobs if not success.
     * It is almost the same work as AwsIotDefender_Stop except here it must "clean" on condition.
     */
    if( defenderError != AWS_IOT_DEFENDER_SUCCESS )
    {
        /* reset _startInfo to empty; otherwise next time defender might start with incorrect information. */
        _startInfo = ( AwsIotDefenderStartInfo_t ) AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

        _tlsInfo = ( AwsIotNetworkTlsInfo_t ) AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;

        if( buildTopicsNamesSuccess )
        {
            AwsIotDefenderInternal_DeleteTopicsNames();
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

/**
 * TODO: need to think about what happens exactly when timer is destroyed and expiration routine is half-running...
 */
AwsIotDefenderError_t AwsIotDefender_Stop( void )
{
    /* reset _startInfo to empty; otherwise next time defender might start with incorrect information. */
    _startInfo = ( AwsIotDefenderStartInfo_t ) AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

    _tlsInfo = ( AwsIotNetworkTlsInfo_t ) AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;

    /* TODO: currently it assumes the connections are disposed properly */
    AwsIotClock_TimerDestroy( &_metricsPublishTimer );
    AwsIotClock_TimerDestroy( &_guardTimer );

    AwsIotDefenderInternal_DeleteTopicsNames();

    return AWS_IOT_DEFENDER_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint64_t periodSeconds )
{
    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    /* period can not be too short unless this is test mode. */
    if( ( periodSeconds < AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS ) && !_DEFENDER_TEST_MODE )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_SHORT;
    }
    /* overflow check for period, although this should be a rare case. */
    else if( periodSeconds > _defenderToSeconds( UINT64_MAX ) )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_LARGE;
    }
    else
    {
        _periodMilliSecond = _defenderToMilliseconds( periodSeconds );

        if( isStarted() )
        {
            if( AwsIotClock_TimerArm( &_metricsPublishTimer,
                                      _DEFENDER_TEST_MODE ? _DEFENDER_SHORT_RELATIVE_MILLISECONDS : _periodMilliSecond, _periodMilliSecond ) )
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
    return _defenderToSeconds( _periodMilliSecond );
}

/*-----------------------------------------------------------*/

static void _metricsPublishTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    AwsIotDefenderEventType_t eventType = 0;
    AwsIotDefenderCallbackInfo_t callbackInfo;
    _defenderReport_t report = { 0 };
    AwsIotMqttCallbackInfo_t acceptCallbackInfo = { .function = _acceptCallback, .param1 = NULL };
    AwsIotMqttCallbackInfo_t rejectCallbackInfo = { .function = _rejectCallback, .param1 = NULL };

    if( AwsIotDefenderInternal_NetworkConnect( _startInfo.pAwsIotEndpoint,
                                               _startInfo.port,
                                               _startInfo.pTlsInfo,
                                               &eventType ) )
    {
        /* SetMqttReceiveCallback must be invoked to have MQTT agent receive response from broker. */
        if( AwsIotDefenderInternal_SetMqttCallback( &eventType ) )
        {
            if( AwsIotDefenderInternal_MqttConnect( _startInfo.pThingName,
                                                    _startInfo.thingNameLength,
                                                    &eventType ) )
            {
                if( AwsIotDefenderInternal_MqttSubscribe( acceptCallbackInfo,
                                                          rejectCallbackInfo,
                                                          &eventType ) )
                {
                    if( AwsIotDefenderInternal_CreateReport( &report, &eventType ) )
                    {
                        AwsIotDefenderInternal_MqttPublish( report.pDataBuffer, report.size, &eventType );
                    }

                    /* delete report regardless. */
                    AwsIotDefenderInternal_DeleteReport( &report );
                }
            }
        }
    }

    /* Something is wrong during the metrics publish process. */
    if( eventType != 0 )
    {
        callbackInfo.eventType = eventType;

        /* no message to be given to user's callback */
        callbackInfo.pPayload = NULL;
        callbackInfo.payloadLength = 0;

        /* invoke user's callback if there is. */
        if( _startInfo.callback.function != NULL )
        {
            _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
        }

        /* disconnect and destroy immediately. */
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

void _acceptCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;
    ( void ) pPublish;

    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = AWS_IOT_DEFENDER_METRICS_ACCEPTED;
        callbackInfo.pPayload = pPublish->message.info.pPayload;
        callbackInfo.payloadLength = pPublish->message.info.payloadLength;

        _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
    }

    /* one shot timer */
    AwsIotClock_TimerArm( &_guardTimer, _DEFENDER_SHORT_RELATIVE_MILLISECONDS, 0 );
}

/*-----------------------------------------------------------*/

void _rejectCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;
    ( void ) pPublish;

    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = AWS_IOT_DEFENDER_METRICS_REJECTED;
        callbackInfo.pPayload = pPublish->message.info.pPayload;
        callbackInfo.payloadLength = pPublish->message.info.payloadLength;

        _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
    }

    /* one shot timer */
    AwsIotClock_TimerArm( &_guardTimer, _DEFENDER_SHORT_RELATIVE_MILLISECONDS, 0 );
}

/*-----------------------------------------------------------*/

static void _closeAndDestroyConnection()
{
    AwsIotDefenderInternal_MqttDisconnect();
    AwsIotDefenderInternal_NetworkDestroy();
}
