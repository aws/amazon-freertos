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

/* Task pool include. */
#include "iot_taskpool.h"

/* Clock include. */
#include "platform/iot_clock.h"

#define _WAIT_METRICS_JOB_MAX_SECONDS    ( 5 )

#if _WAIT_METRICS_JOB_MAX_SECONDS < AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS
    #error "_WAIT_METRICS_JOB_MAX_SECONDS must be greater than AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS."
#endif

/**
 * callback registerd on accept topic.
 */
void _acceptCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish );

/**
 * callback registerd on reject topic.
 */
void _rejectCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish );

/**
 * Callback routine of _metricsPublishJob.
 */
static void _metricsPublishRoutine( IotTaskPool_t * pTaskPool,
                                    IotTaskPoolJob_t * pJob,
                                    void * pUserContext );

/* Callback routine of _disconnectJob. */
static void _disconnectRoutine( IotTaskPool_t * pTaskPool,
                                IotTaskPoolJob_t * pJob,
                                void * pUserContext );


/*------------------- Below are global variables. ---------------------------*/

/* Define global metrics and initialize metrics flags array to zero. */
_defenderMetrics_t _AwsIotDefenderMetrics =
{
    .metricsFlag = { 0 }
};

/**
 * Period between reports in milliseconds.
 * Set default value.
 */
static uint64_t _periodMilliSecond = _defenderToMilliseconds( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );

static IotTaskPoolJob_t _metricsPublishJob = { 0 };

static IotTaskPoolJob_t _disconnectJob = { 0 };

static IotSemaphore_t _doneSem;

/*
 * State variable to indicate if defender has been started successfully, initialized to false.
 * There is no lock to protect it so the functions referencing it are not thread safe.
 */
static bool _started = false;

/* Internal copy of startInfo so that user's input doesn't have to be valid all the time. */
AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

/* For debug purpose. */
const char * _AwsIotDefenderEventError = "None";

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetMetrics( AwsIotDefenderMetricsGroup_t metricsGroup,
                                                 uint32_t metrics )
{
    if( metricsGroup >= _DEFENDER_METRICS_GROUP_COUNT )
    {
        IotLogError( "Input metrics group is invalid. Please use AwsIotDefenderMetricsGroup_t data type." );

        return AWS_IOT_DEFENDER_INVALID_INPUT;
    }

    /* If started, it needs to lock the metrics to protect concurrent read from metrics timer callback. */
    if( _started )
    {
        IotMutex_Lock( &_AwsIotDefenderMetrics.mutex );

        _AwsIotDefenderMetrics.metricsFlag[ metricsGroup ] = metrics;

        IotMutex_Unlock( &_AwsIotDefenderMetrics.mutex );
    }
    else
    {
        _AwsIotDefenderMetrics.metricsFlag[ metricsGroup ] = metrics;
    }

    IotLogInfo( "Metrics are successfully updated." );

    return AWS_IOT_DEFENDER_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo )
{
    if( pStartInfo == NULL )
    {
        IotLogError( "Input start info is invalid." );

        return AWS_IOT_DEFENDER_INVALID_INPUT;
    }

    /* Assert system task pool is pre-created! */
    AwsIotDefender_Assert( IOT_SYSTEM_TASKPOOL != NULL );

    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;

    /* Initialize flow control states to false. */
    bool buildTopicsNamesSuccess = false,
         doneSemaphoreCreateSuccess = false,
         metricsMutexCreateSuccess = false;

    if( !_started )
    {
        /* copy input start info into global variable _startInfo */
        _startInfo = *pStartInfo;

        defenderError = AwsIotDefenderInternal_BuildTopicsNames();

        buildTopicsNamesSuccess = ( defenderError == AWS_IOT_DEFENDER_SUCCESS );

        if( buildTopicsNamesSuccess )
        {
            /* Create a binary semaphore with initial value 1. */
            doneSemaphoreCreateSuccess = IotSemaphore_Create( &_doneSem, 1, 1 );
        }

        if( doneSemaphoreCreateSuccess )
        {
            metricsMutexCreateSuccess = IotMutex_Create( &_AwsIotDefenderMetrics.mutex, false );
        }

        if( metricsMutexCreateSuccess )
        {
            /* Create disconnect job. */
            taskPoolError = IotTaskPool_CreateJob( _disconnectRoutine, NULL, &_disconnectJob );
            AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );

            /* Create metrics job. */
            taskPoolError = IotTaskPool_CreateJob( _metricsPublishRoutine, NULL, &_metricsPublishJob );
            AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );

            /* Schedule metrics job. */
            taskPoolError = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, &_metricsPublishJob, 0 );
            AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );

            /* Everything is good. Declare success. */
            _started = true;
            defenderError = AWS_IOT_DEFENDER_SUCCESS;
            IotLogInfo( "Defender agent has successfully started." );
        }

        /* Do the cleanup jobs if not success. */
        if( defenderError != AWS_IOT_DEFENDER_SUCCESS )
        {
            /* reset _startInfo to empty; otherwise next time defender might start with incorrect information. */
            _startInfo = ( AwsIotDefenderStartInfo_t ) AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

            if( buildTopicsNamesSuccess )
            {
                AwsIotDefenderInternal_DeleteTopicsNames();
            }

            if( doneSemaphoreCreateSuccess )
            {
                IotSemaphore_Destroy( &_doneSem );
            }

            if( metricsMutexCreateSuccess )
            {
                IotMutex_Destroy( &_AwsIotDefenderMetrics.mutex );
            }

            IotLogError( "Defender agent failed to start due to error %s.", AwsIotDefender_strerror( defenderError ) );
        }
    }
    else
    {
        defenderError = AWS_IOT_DEFENDER_ALREADY_STARTED;
    }

    return defenderError;
}

/*-----------------------------------------------------------*/

void AwsIotDefender_Stop( void )
{
    if( !_started )
    {
        IotLogWarn( "Defender has not started yet." );

        return;
    }

    /* First thing to do: wait for all the metrics processing to be done. */
    IotSemaphore_Wait( &_doneSem );

    IotTaskPoolJobStatus_t status;
    IotTaskPoolError_t taskPoolError = IotTaskPool_TryCancel( IOT_SYSTEM_TASKPOOL, &_metricsPublishJob, &status );

    /* If cancel failed, let it sleep for a while and hope everything finishes. */
    if( taskPoolError != IOT_TASKPOOL_SUCCESS )
    {
        IotLogWarn( "Failed to cancel metrics publish job with return code %d and status %d.", taskPoolError, status );
        IotClock_SleepMs( _WAIT_METRICS_JOB_MAX_SECONDS * 1000 );
    }

    /* Destroy metrics' mutex. */
    IotMutex_Destroy( &_AwsIotDefenderMetrics.mutex );

    /* Destroy 'done' semaphore. */
    IotSemaphore_Destroy( &_doneSem );

    /* Delete topics names. */
    AwsIotDefenderInternal_DeleteTopicsNames();

    /* Reset _startInfo to empty; otherwise next time defender might start with incorrect information. */
    _startInfo = ( AwsIotDefenderStartInfo_t ) AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

    /* Reset _periodMilliSecond to default value. */
    _periodMilliSecond = _defenderToMilliseconds( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );

    /* Reset metrics flag array to 0. */
    memset( _AwsIotDefenderMetrics.metricsFlag, 0, sizeof( _AwsIotDefenderMetrics.metricsFlag ) );

    /* Set to not started. */
    _started = false;

    IotLogInfo( "Defender agent has stopped." );
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint64_t periodSeconds )
{
    /* Input period cannot be too long, which will cause integer overflow. */
    AwsIotDefender_Assert( periodSeconds < _defenderToSeconds( UINT64_MAX ) );

    AwsIotDefenderError_t defenderError = AWS_IOT_DEFENDER_INTERNAL_FAILURE;

    /* period can not be too short unless this is test mode. */
    if( periodSeconds < AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS )
    {
        defenderError = AWS_IOT_DEFENDER_PERIOD_TOO_SHORT;
        IotLogError( "Input period is too short. It must be greater than %d seconds.", AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );
    }
    else
    {
        _periodMilliSecond = _defenderToMilliseconds( periodSeconds );

        defenderError = AWS_IOT_DEFENDER_SUCCESS;
        IotLogInfo( "Period has been set to %d seconds successfully.", periodSeconds );
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
        "SUCCESS",          /* AWS_IOT_DEFENDER_SUCCESS */
        "INLVALID INPUT",   /* AWS_IOT_DEFENDER_INVALID_INPUT */
        "ALREADY STARTED",  /* AWS_IOT_DEFENDER_ALREADY_STARTED */
        "PERIOD TOO SHORT", /* AWS_IOT_DEFENDER_PERIOD_TOO_SHORT */
        "NO MEMORY",        /* AWS_IOT_DEFENDER_ERROR_NO_MEMORY */
        "INTERNAL FAILURE"  /* AWS_IOT_DEFENDER_INTERNAL_FAILURE */
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
const char * AwsIotDefender_GetEventError()
{
    return _AwsIotDefenderEventError;
}

/*-----------------------------------------------------------*/
static void _metricsPublishRoutine( IotTaskPool_t * pTaskPool,
                                    IotTaskPoolJob_t * pJob,
                                    void * pUserContext )
{
    /* Unsed parameter; silence the compiler. */
    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pUserContext;

    IotLogDebug( "Metrics publish job starts." );

    if( !IotSemaphore_TryWait( &_doneSem ) )
    {
        IotLogWarn( "Defender has been stopped or the previous metrics is in process. No further action." );

        return;
    }

    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    bool mqttConnected = true, reportCreated = true, reportPublished = false;

    const IotMqttCallbackInfo_t acceptCallbackInfo = { .function = _acceptCallback, .pCallbackContext = NULL };
    const IotMqttCallbackInfo_t rejectCallbackInfo = { .function = _rejectCallback, .pCallbackContext = NULL };

    /* Step 1: connect to MQTT. */
    mqttError = AwsIotDefenderInternal_MqttConnect();

    if( ( mqttConnected = ( mqttError == IOT_MQTT_SUCCESS ) ) )
    {
        mqttError = AwsIotDefenderInternal_MqttSubscribe( acceptCallbackInfo,
                                                          rejectCallbackInfo );

        /* Step 2: subscribe to accept/reject MQTT topics. */
        if( mqttError == IOT_MQTT_SUCCESS )
        {
            /* Step 3: create serialized metrics report. */
            reportCreated = AwsIotDefenderInternal_CreateReport();

            /* If Report is created successfully. */
            if( reportCreated )
            {
                /* Step 4: publish report to defender topic. */
                mqttError = AwsIotDefenderInternal_MqttPublish( AwsIotDefenderInternal_GetReportBuffer(),
                                                                AwsIotDefenderInternal_GetReportBufferSize() );

                if( ( reportPublished = ( mqttError == IOT_MQTT_SUCCESS ) ) )
                {
                    IotLogDebug( "Metrics report has been published successfully." );
                }
            }
        }
    }

    if( reportPublished )
    {
        IotTaskPoolError_t taskPoolError = IotTaskPool_CreateJob( _disconnectRoutine, NULL, &_disconnectJob );
        AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );

        IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL,
                                      &_disconnectJob,
                                      _defenderToMilliseconds( AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS ) );
    }
    /* Something is wrong during the above process. */
    else
    {
        /* Either MQTT functions had problem or report failed to be created. */
        AwsIotDefender_Assert( mqttError != IOT_MQTT_SUCCESS || !reportCreated );

        AwsIotDefenderEventType_t eventType;

        /* Set event type to only two possible categories. */
        if( reportCreated )
        {
            eventType = AWS_IOT_DEFENDER_FAILURE_MQTT;
            _AwsIotDefenderEventError = IotMqtt_strerror( mqttError );
        }
        else
        {
            eventType = AWS_IOT_DEFENDER_FAILURE_METRICS_REPORT;
            /* As of today, no memory is the only reason to fail metrics report creation. */
            _AwsIotDefenderEventError = "Failed to create metrics report due to no memory.";
        }

        /* Invoke user's callback if there is. */
        if( _startInfo.callback.function != NULL )
        {
            AwsIotDefenderCallbackInfo_t callbackInfo;

            callbackInfo.eventType = eventType;

            /* No message to be given to user's callback */
            callbackInfo.pPayload = NULL;
            callbackInfo.payloadLength = 0;

            /* It is possible the report buffer is NULL and size is 0. */
            callbackInfo.pMetricsReport = AwsIotDefenderInternal_GetReportBuffer();
            callbackInfo.metricsReportLength = AwsIotDefenderInternal_GetReportBufferSize();

            _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
        }

        /* Clean up resources conditionally. */
        if( reportCreated )
        {
            AwsIotDefenderInternal_DeleteReport();
        }

        if( mqttConnected )
        {
            AwsIotDefenderInternal_MqttDisconnect();
        }

        IotSemaphore_Post( &_doneSem );
    }

    IotLogDebug( "Metrics publish job ends." );
}

/*-----------------------------------------------------------*/

static void _disconnectRoutine( IotTaskPool_t * pTaskPool,
                                IotTaskPoolJob_t * pJob,
                                void * pUserContext )
{
    /* Unsed parameter; silence the compiler. */
    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pUserContext;

    IotLogDebug( "Disconnect job starts." );

    AwsIotDefenderInternal_DeleteReport();
    AwsIotDefenderInternal_MqttDisconnect();
    /* Re-create metrics job. */
    IotTaskPoolError_t taskPoolError = IotTaskPool_CreateJob( _metricsPublishRoutine, NULL, &_metricsPublishJob );
    AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );

    /* Re-schedule metrics job with period as deferred interval. */
    taskPoolError = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, &_metricsPublishJob, _periodMilliSecond );
    AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );

    IotSemaphore_Post( &_doneSem );

    IotLogDebug( "Disconnect job ends." );
}

/*-----------------------------------------------------------*/

void _acceptCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;

    IotLogInfo( "Metrics report was accepted by defender service." );

    /* In accepted case, report and MQTT message must exist. */
    AwsIotDefender_Assert( AwsIotDefenderInternal_GetReportBuffer() );
    AwsIotDefender_Assert( pPublish->message.info.pPayload );

    /* Invoke user's callback with accept event. */
    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = AWS_IOT_DEFENDER_METRICS_ACCEPTED;

        callbackInfo.pMetricsReport = AwsIotDefenderInternal_GetReportBuffer();
        callbackInfo.metricsReportLength = AwsIotDefenderInternal_GetReportBufferSize();

        callbackInfo.pPayload = pPublish->message.info.pPayload;
        callbackInfo.payloadLength = pPublish->message.info.payloadLength;

        _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
    }
}

/*-----------------------------------------------------------*/

void _rejectCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;

    IotLogError( "Metrics report was rejected by defender service." );

    /* In rejected case, MQTT message must exist. */
    AwsIotDefender_Assert( pPublish->message.info.pPayload );

    /* Invoke user's callback with rejected event. */
    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = AWS_IOT_DEFENDER_METRICS_REJECTED;

        callbackInfo.pMetricsReport = AwsIotDefenderInternal_GetReportBuffer();
        callbackInfo.metricsReportLength = AwsIotDefenderInternal_GetReportBufferSize();

        callbackInfo.pPayload = pPublish->message.info.pPayload;
        callbackInfo.payloadLength = pPublish->message.info.payloadLength;

        _startInfo.callback.function( _startInfo.callback.param1, &callbackInfo );
    }
}
