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

/* POSIX sleep include. */
#include "FreeRTOS_POSIX/unistd.h"

/*
 * Small time value to kick off timer immediately.
 */
#define _DEFENDER_SHORT_RELATIVE_MILLISECONDS    10

/*
 * At the moment AwsIotDefender_Stop function is invoked, the metrics timer callback might be already invoked
 * but not yet locked the timer semaphore. Therefore, before destroying the semaphore, AwsIotDefender_Stop
 * sleeps to wait for the metrics timer callback to return due to failed AwsIotSemaphore_TryWait.
 */
#define _DEFENDER_STOP_SLEEP_SECONDS             1

/*
 * This mode is supposed to be used by test, demo or debug.
 */
#ifndef _DEFENDER_TEST_MODE
    #define _DEFENDER_TEST_MODE    false
#endif

/**
 * Clean up resources in the end of each iteration of timer execution.
 */
static void _cleanUpResources( void );

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
 * callback registerd on guard timer.
 */
static void _guardTimerExpirationRoutine( void * );


/*------------------- Below are global variables. ---------------------------*/

_defenderMetrics_t _AwsIotDefenderMetrics;

/**
 * Period between reports in milliseconds.
 * Set default value.
 */
static uint64_t _periodMilliSecond = _defenderToMilliseconds( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );

/* the timer that connect, subscribe, publish metrics report */
static AwsIotTimer_t _metricsPublishTimer;

/* the timer that disconnect */
static AwsIotTimer_t _guardTimer;

/*
 * This semaphore is to synchronize AwsIotDefender_Stop and metrics timer callback + MQTT callback + guard timer callback.
 * AwsIotDefender_Stop waits on this semaphore.
 * Metrics timer callback tries to wait on this semaphore.
 */
static AwsIotSemaphore_t _timerSem;

/*
 * State variable to indicate if defender has been started successfully, initialized to false.
 * There is no lock to protect it so the functions referencing it are not thread safe.
 *
 */
static bool _started = false;

static _defenderReport_t _report = { 0 };

static AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetMetrics( AwsIotDefenderMetricsGroup_t metricsGroup,
                                                 uint32_t metrics )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;

    if( metricsGroup >= _DEFENDER_METRICS_GROUP_COUNT )
    {
        error = AWS_IOT_DEFENDER_INVALID_INPUT;
        AwsIotLogError( "Input metrics group is invalid. Please use AwsIotDefenderMetricsGroup_t data type." );
    }
    else
    {
        AwsIotMutex_Lock( &_AwsIotDefenderMetrics.mutex );

        _AwsIotDefenderMetrics.metricsFlag[ metricsGroup ] = metrics;

        AwsIotMutex_Unlock( &_AwsIotDefenderMetrics.mutex );
    }

    return error;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo )
{
    if( pStartInfo == NULL )
    {
        return AWS_IOT_DEFENDER_INVALID_INPUT;
    }

    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    /* initialize flow control states to false. */
    bool networkConnectSuccess = false,
         buildTopicsNamesSuccess = false,
         publishTimerCreateSuccess = false,
         publishTimerArmSuccess = false,
         guradTimerCreateSuccess = false,
         timerSempCreateSuccess = false,
         metricsMutexCreateSuccess = false;

    if( !_started )
    {
        /* First, attempt to connect to AWS IoT endpoint to valid input. */
        networkConnectSuccess = AwsIotDefenderInternal_NetworkConnect( pStartInfo->pAwsIotEndpoint,
                                                                       pStartInfo->port,
                                                                       &pStartInfo->tlsInfo,
                                                                       NULL );

        if( networkConnectSuccess )
        {
            /* copy input start info into global variable _startInfo */
            _startInfo = *pStartInfo;

            buildTopicsNamesSuccess = AwsIotDefenderInternal_BuildTopicsNames( _startInfo.pThingName,
                                                                               _startInfo.thingNameLength,
                                                                               &defenderError );
        }
        else
        {
            defenderError = AWS_IOT_DEFENDER_CONNECTION_FAILURE;
        }

        if( buildTopicsNamesSuccess )
        {
            publishTimerCreateSuccess = AwsIotClock_TimerCreate( &_metricsPublishTimer,
                                                                 _metricsPublishTimerExpirationRoutine,
                                                                 NULL );
        }

        if( publishTimerCreateSuccess )
        {
            guradTimerCreateSuccess = AwsIotClock_TimerCreate( &_guardTimer, _guardTimerExpirationRoutine, NULL );
        }

        if( guradTimerCreateSuccess )
        {
            /* Create a unlocked binary semaphore. */
            timerSempCreateSuccess = AwsIotSemaphore_Create( &_timerSem, 1, 1 );
        }

        if( timerSempCreateSuccess )
        {
            metricsMutexCreateSuccess = AwsIotMutex_Create( &_AwsIotDefenderMetrics.mutex );
        }

        if( metricsMutexCreateSuccess )
        {
            /* in test mode, the timer is kicked off almost immediately. */
            publishTimerArmSuccess = AwsIotClock_TimerArm( &_metricsPublishTimer,
                                                           _DEFENDER_TEST_MODE ? _DEFENDER_SHORT_RELATIVE_MILLISECONDS : _periodMilliSecond,
                                                           _periodMilliSecond );
        }

        if( publishTimerArmSuccess )
        {
            _started = true;
            defenderError = AWS_IOT_DEFENDER_SUCCESS;
            AwsIotLogInfo( "Defender agent has successfully started." );
        }

        /* Do the cleanup jobs if not success.
         * It is almost the same work as AwsIotDefender_Stop except here it must "clean" on condition.
         */
        if( defenderError != AWS_IOT_DEFENDER_SUCCESS )
        {
            /* reset _startInfo to empty; otherwise next time defender might start with incorrect information. */
            _startInfo = ( AwsIotDefenderStartInfo_t ) AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

            if (networkConnectSuccess)
            {
                AwsIotDefenderInternal_NetworkDestroy();
            }

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

            if( timerSempCreateSuccess )
            {
                AwsIotSemaphore_Destroy( &_timerSem );
            }

            if( metricsMutexCreateSuccess )
            {
                AwsIotMutex_Destroy( &_AwsIotDefenderMetrics.mutex );
            }

            AwsIotLogError( "Defender agent failed to start due to error %s.", AwsIotDefender_strerror( defenderError ) );
        }
    }
    else
    {
        defenderError = AWS_IOT_DEFENDER_ALREADY_STARTED;
    }

    return defenderError;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Stop( void )
{
    if( !_started )
    {
        return AWS_IOT_DEFENDER_NOT_STARTED;
    }

    /* 'Stop' function needs to wait for the timers to finish execution if they are in process. */
    AwsIotSemaphore_Wait( &_timerSem );

    /* Reset _startInfo to empty; otherwise next time defender might start with incorrect information. */
    _startInfo = ( AwsIotDefenderStartInfo_t ) AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

    /* Destroying will cancel timers that are active but not in process. */
    AwsIotClock_TimerDestroy( &_metricsPublishTimer );
    AwsIotClock_TimerDestroy( &_guardTimer );

    AwsIotDefenderInternal_DeleteTopicsNames();

    /* For the reason of sleeping, see comments on _DEFENDER_STOP_SLEEP_SECONDS.
     * No need to check return value of sleep.
     */
    sleep( _DEFENDER_STOP_SLEEP_SECONDS );

    /* Metrics timer callback should be waiting for _timerSem till now. */
    AwsIotSemaphore_Destroy( &_timerSem );

    AwsIotMutex_Destroy( &_AwsIotDefenderMetrics.mutex );

    _started = false;

    AwsIotLogInfo( "Defender agent has stopped." );

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
        AwsIotLogError( "Input period is too short. It must be greater than %d seconds.", AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );
    }
    /* overflow check for period, although this should be a rare case. */
    else if( periodSeconds > _defenderToSeconds( UINT64_MAX ) )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_LONG;
        AwsIotLogError( "Input period is too large, which will cause integer overflow." );
    }
    else
    {
        _periodMilliSecond = _defenderToMilliseconds( periodSeconds );

        if( _started )
        {
            if( AwsIotClock_TimerArm( &_metricsPublishTimer,
                                      _DEFENDER_TEST_MODE ? _DEFENDER_SHORT_RELATIVE_MILLISECONDS : _periodMilliSecond, _periodMilliSecond ) )
            {
                defenderError = AWS_IOT_DEFENDER_SUCCESS;
                AwsIotLogInfo( "Period has been updated to %d seconds successfully.", periodSeconds );
            }
        }
        else
        {
            /* if defender is not started, simply return success */
            defenderError = AWS_IOT_DEFENDER_SUCCESS;
            AwsIotLogInfo( "Period has been set to %d seconds successfully but defender agent is not started yet.", periodSeconds );
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

const char * AwsIotDefender_strerror( AwsIotDefenderError_t error )
{
    /* The string returned if the parameter is invalid. */
    static const char * pInvalidError = "INVALID ERROR";
    /* Lookup table of Defender errors. */
    static const char * pErrorNames[] =
    {
        "SUCCESS",           /* AWS_IOT_DEFENDER_SUCCESS */
        "INLVALID INPUT",    /* AWS_IOT_DEFENDER_INVALID_INPUT */
        "CONNECTION FAILURE" /* AWS_IOT_DEFENDER_CONNECTION_FAILURE */
        "ALREADY STARTED",   /* AWS_IOT_DEFENDER_ALREADY_STARTED */
        "NOT STARTED",       /* AWS_IOT_DEFENDER_NOT_STARTED */
        "PERIOD TOO SHORT",  /* AWS_IOT_DEFENDER_PERIOD_TOO_SHORT */
        "PERIOD TOO LONG",   /* AWS_IOT_DEFENDER_PERIOD_TOO_LONG */
        "NO MEMORY",         /* AWS_IOT_DEFENDER_ERROR_NO_MEMORY */
        "INTERNAL FAILURE"   /* AWS_IOT_DEFENDER_INTERNAL_FAILURE */
    };

    /* Check that the parameter is valid. */
    if( ( error < 0 ) ||
        ( error >= ( sizeof( pErrorNames ) / sizeof( pErrorNames[ 0 ] ) ) ) )
    {
        return pInvalidError;
    }

    return pErrorNames[ error ];
}

/*-----------------------------------------------------------*/

static void _metricsPublishTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    AwsIotLogDebug( "Metrics timer callback starts." );

    if( !AwsIotSemaphore_TryWait( &_timerSem ) )
    {
        AwsIotLogWarn( "Metrics timer callback fails to acquire the timer semaphore. There is a concurrent 'stop' function invocation." );

        return;
    }

    AwsIotDefenderEventType_t eventType = 0;
    AwsIotDefenderCallbackInfo_t callbackInfo;

    const AwsIotMqttCallbackInfo_t acceptCallbackInfo = { .function = _acceptCallback, .param1 = NULL };
    const AwsIotMqttCallbackInfo_t rejectCallbackInfo = { .function = _rejectCallback, .param1 = NULL };

    if( AwsIotDefenderInternal_NetworkConnect( _startInfo.pAwsIotEndpoint,
                                               _startInfo.port,
                                               &_startInfo.tlsInfo,
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
                    if( AwsIotDefenderInternal_CreateReport( &_report, &eventType ) )
                    {
                        AwsIotDefenderInternal_MqttPublish( _report.pDataBuffer, _report.size, &eventType );
                    }
                }
            }
        }
    }

    /* Something is wrong during the metrics publish process. */
    if( eventType != 0 )
    {
        /* invoke user's callback if there is. */
        if( _startInfo.callback.function != NULL )
        {
            callbackInfo.eventType = eventType;

            /* no message to be given to user's callback */
            callbackInfo.pPayload = NULL;
            callbackInfo.payloadLength = 0;
            callbackInfo.pMetricsReport = _report.pDataBuffer;
            callbackInfo.metricsReportLength = _report.size;

            _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
        }

        AwsIotDefenderInternal_DeleteReport( &_report );
        /* Clean up resources immediately. */
        _cleanUpResources();
    }

    AwsIotLogDebug( "Defender timber callback ends." );
}

/*-----------------------------------------------------------*/

static void _guardTimerExpirationRoutine( void * pArgument )
{
    ( void ) pArgument;

    _cleanUpResources();
}

/*-----------------------------------------------------------*/

void _acceptCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish )
{
    AwsIotLogInfo( "Metrics report was accepted by defender service." );

    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = AWS_IOT_DEFENDER_METRICS_ACCEPTED;
        callbackInfo.pMetricsReport = _report.pDataBuffer;
        callbackInfo.metricsReportLength = _report.size;
        callbackInfo.pPayload = pPublish->message.info.pPayload;
        callbackInfo.payloadLength = pPublish->message.info.payloadLength;

        _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
    }

    AwsIotDefenderInternal_DeleteReport( &_report );
    /* one shot timer */
    AwsIotClock_TimerArm( &_guardTimer, _DEFENDER_SHORT_RELATIVE_MILLISECONDS, 0 );
}

/*-----------------------------------------------------------*/

void _rejectCallback( void * pArgument,
                      AwsIotMqttCallbackParam_t * const pPublish )
{
    AwsIotLogError( "Metrics report was rejected by defender service." );

    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = AWS_IOT_DEFENDER_METRICS_REJECTED;
        callbackInfo.pMetricsReport = _report.pDataBuffer;
        callbackInfo.metricsReportLength = _report.size;
        callbackInfo.pPayload = pPublish->message.info.pPayload;
        callbackInfo.payloadLength = pPublish->message.info.payloadLength;

        _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
    }

    AwsIotDefenderInternal_DeleteReport( &_report );
    /* one shot timer */
    AwsIotClock_TimerArm( &_guardTimer, _DEFENDER_SHORT_RELATIVE_MILLISECONDS, 0 );
}

/*-----------------------------------------------------------*/

static void _cleanUpResources( void )
{
    AwsIotDefenderInternal_MqttDisconnect();
    AwsIotDefenderInternal_NetworkDestroy();

    /* This function indicates the end of each metrics publish iteration.
     * Therefore that last thing is to post the timer semaphore to wake up
     * potential threads invoking 'Stop' function.
     */
    AwsIotSemaphore_Post( &_timerSem );
}
