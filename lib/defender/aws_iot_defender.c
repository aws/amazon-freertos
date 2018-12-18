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

#define _START_INFO_RESETER    ( AwsIotDefenderStartInfo_t ) { 0 }

/* Period between reports in milliseconds. */
static uint32_t _periodMilliSecond = AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS * 1000;

/* the timer that connect, subscribe, publish metrics report */
static AwsIotTimer_t _metricsPublishTimer;

/* the timer that disconnect */
static AwsIotTimer_t _guardTimer;

static AwsIotMqttConnection_t _mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;

static AwsIotDefenderStartInfo_t _startInfo = { 0 };

/**
 * Return the state that whether defender is already started.
 */
static bool isStarted();

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

    /* if current state is not started */
    if( !isStarted() )
    {
        /* copy input start info into global variable _startInfo */
        _startInfo = *pStartInfo;

        /* metrics publish timer must be created and armed */
        /* guard timer must be created */
        if( AwsIotClock_TimerCreate( &_metricsPublishTimer, _metricsPublishTimerExpirationRoutine, NULL ) )
        {
            if( AwsIotClock_TimerArm( &_metricsPublishTimer, _periodMilliSecond, _periodMilliSecond ) )
            {
                if( AwsIotClock_TimerCreate( &_guardTimer, _guardTimerExpirationRoutine, NULL ) )
                {
                    defenderError = AWS_IOT_DEFENDER_SUCCESS;
                }
            }
            else
            {
                AwsIotClock_TimerDestroy( &_metricsPublishTimer );
            }
        }
    }
    else
    {
        defenderError = AWS_IOT_DEFENDER_ALREADY_STARTED;
    }

    if( defenderError != AWS_IOT_DEFENDER_SUCCESS )
    {
        _startInfo = _START_INFO_RESETER;
    }

    return defenderError;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Stop( void )
{
    AwsIotClock_TimerDestroy( &_metricsPublishTimer );
    AwsIotClock_TimerDestroy( &_guardTimer );
    _startInfo = _START_INFO_RESETER;

    return AWS_IOT_DEFENDER_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint64_t periodSeconds )
{
    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    if( periodSeconds < AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_SHORT;
    }
    else
    {
        _periodMilliSecond = periodSeconds * 1000;

        if( isStarted() )
        {
            if( AwsIotClock_TimerArm( &_metricsPublishTimer, _periodMilliSecond, _periodMilliSecond ) )
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
    return _periodMilliSecond / 1000;
}

/*-----------------------------------------------------------*/

static void _metricsPublishTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;

    /* Set the members of the connection info (password and username not used). */
    connectInfo.cleanSession = true;
    connectInfo.keepAliveSeconds = _DISABLE_KEEP_ALIVE;
    connectInfo.pClientIdentifier = _startInfo.pThingName;
    connectInfo.clientIdentifierLength = _startInfo.thingNameLength;

    AwsIotMqtt_Connect( &_mqttConnection,
                        &( _startInfo.networkInterface ),
                        &connectInfo,
                        NULL,
                        AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS * 1000 );
}

/*-----------------------------------------------------------*/

static void _guardTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    AwsIotMqtt_Disconnect( _mqttConnection, false );
}
