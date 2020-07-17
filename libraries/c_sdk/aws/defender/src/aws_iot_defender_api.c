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

/* The config header is always included first. */
#include "iot_config.h"
/* Error handling include. */
#include "private/iot_error.h"

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

/* Task pool include. */
#include "iot_taskpool.h"

/* Clock include. */
#include "platform/iot_clock.h"

#define WAIT_METRICS_JOB_MAX_SECONDS            ( 5 )
#define MAX_DEFENDER_OUTSTANDING_PUBLISH_REQ    ( ( uint32_t ) 1 )
#define MAX_CLIENT_IDENTIFIER_LENGTH            ( ( uint16_t ) 128 )

#if WAIT_METRICS_JOB_MAX_SECONDS < AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS
    #error "WAIT_METRICS_JOB_MAX_SECONDS must be greater than AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS."
#endif

/**
 * callback registered on accept topic.
 */
void _acceptCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish );

/**
 * callback registered on reject topic.
 */
void _rejectCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish );

/**
 * subscribe to defender MQTT topics.
 */
static IotMqttError_t _metricsSubscribeRoutine();

/**
 * Publish to defender MQTT topic.
 */
static void _metricsPublishRoutine( IotTaskPool_t pTaskPool,
                                    IotTaskPoolJob_t pJob,
                                    void * pUserContext );

/* Unsubscribe from defender MQTT topic. */
static void _unsubscribeMqtt();

/* Code to handle application callback */
void _handleApplicationCallback( AwsIotDefenderEventType_t event,
                                 IotMqttCallbackParam_t * const pPublish );


/*------------------- Below are global variables. ---------------------------*/

/* Restart defender state machine unless there is an error */


/* Define global metrics and initialize metrics flags array to zero. */
_defenderMetrics_t _AwsIotDefenderMetrics =
{
    .metricsFlag = { 0 }
};
/* MQTT callback info for reporting accept or reject status for subscribe / unsubscribe */
IotMqttCallbackInfo_t _acceptCallbackInfo = { .function = _acceptCallback, .pCallbackContext = NULL };
IotMqttCallbackInfo_t _rejectCallbackInfo = { .function = _rejectCallback, .pCallbackContext = NULL };

/**
 * Period between reports in milliseconds.
 * Set default value.
 */
static uint32_t _periodMilliSecond = _defenderToMilliseconds( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS );

static IotTaskPoolJobStorage_t _metricsPublishJobStorage = IOT_TASKPOOL_JOB_STORAGE_INITIALIZER;
static IotTaskPoolJob_t _metricsPublishJob = IOT_TASKPOOL_JOB_INITIALIZER;

/* Semaphore to prevent stop during publish */
static IotSemaphore_t _doneSem;

/*
 * State variable to indicate if defender has been started successfully, initialized to false.
 * There is no lock to protect it so the functions referencing it are not thread safe.
 */
static bool _started = false;

/* Internal copy of startInfo so that user's input doesn't have to be valid all the time. */
AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetMetrics( AwsIotDefenderMetricsGroup_t metricsGroup,
                                                 uint32_t metrics )
{
    if( metricsGroup >= ( AwsIotDefenderMetricsGroup_t ) DEFENDER_METRICS_GROUP_COUNT )
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
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    IOT_FUNCTION_ENTRY( AwsIotDefenderError_t, AWS_IOT_DEFENDER_SUCCESS );

    if( ( pStartInfo == NULL ) ||
        ( pStartInfo->mqttConnection == IOT_MQTT_CONNECTION_INITIALIZER ) ||
        ( pStartInfo->pClientIdentifier == NULL ) ||
        ( pStartInfo->clientIdentifierLength > MAX_CLIENT_IDENTIFIER_LENGTH ) )
    {
        IotLogError( "Input startInfo is invalid." );
        IOT_SET_AND_GOTO_CLEANUP( AWS_IOT_DEFENDER_INVALID_INPUT );
    }

    /* Assert system task pool is pre-created! */
    AwsIotDefender_Assert( IOT_SYSTEM_TASKPOOL != NULL );

    /* Silence warnigns when asserts are disabled. */
    ( void ) taskPoolError;

    /* Initialize flow control states to false. */
    bool buildTopicsNamesSuccess = false,
         doneSemaphoreCreateSuccess = false,
         metricsMutexCreateSuccess = false;

    if( !_started )
    {
        /* Get the pointers to the encoder function tables. */
        #if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR
            _pAwsIotDefenderDecoder = &_IotSerializerCborDecoder;
            _pAwsIotDefenderEncoder = &_IotSerializerCborEncoder;
        #else
        #error "AWS IOT Defender library supports only CBOR encoder."
        #endif

        /* copy input start info into global variable _startInfo */
        _startInfo = *pStartInfo;

        status = AwsIotDefenderInternal_BuildTopicsNames();

        buildTopicsNamesSuccess = ( status == AWS_IOT_DEFENDER_SUCCESS );

        if( buildTopicsNamesSuccess )
        {
            /* Create a binary semaphore with initial value 1. */
            doneSemaphoreCreateSuccess = IotSemaphore_Create( &_doneSem, MAX_DEFENDER_OUTSTANDING_PUBLISH_REQ, 1 );
        }
        else
        {
            status = AWS_IOT_DEFENDER_INTERNAL_FAILURE;
        }

        if( doneSemaphoreCreateSuccess )
        {
            metricsMutexCreateSuccess = IotMutex_Create( &_AwsIotDefenderMetrics.mutex, false );
        }
        else
        {
            status = AWS_IOT_DEFENDER_INTERNAL_FAILURE;
        }

        if( metricsMutexCreateSuccess )
        {
            /*  Subscribe to Metrics Publish topic */
            mqttError = _metricsSubscribeRoutine();

            if( mqttError != IOT_MQTT_SUCCESS )
            {
                status = AWS_IOT_DEFENDER_INTERNAL_FAILURE;
            }
        }
        else
        {
            status = AWS_IOT_DEFENDER_INTERNAL_FAILURE;
        }

        if( status == AWS_IOT_DEFENDER_SUCCESS )
        {
            /* Create metrics publish job, which will periodically publish metrics */
            taskPoolError = IotTaskPool_CreateJob( _metricsPublishRoutine, NULL, &_metricsPublishJobStorage, &_metricsPublishJob );
            /* Silence warnigns when asserts are disabled. */
            ( void ) taskPoolError;
            AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );
            /* Schedule Publish Job */
            taskPoolError = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, _metricsPublishJob, 0 );
            AwsIotDefender_Assert( taskPoolError == IOT_TASKPOOL_SUCCESS );
            _started = true;
            status = AWS_IOT_DEFENDER_SUCCESS;
            IotLogInfo( "Defender agent has successfully started." );
        }

        /* Do the cleanup jobs if not success. */
        if( status != AWS_IOT_DEFENDER_SUCCESS )
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

            IotLogError( "Defender agent failed to start due to error %s.", AwsIotDefender_strerror( status ) );
        }
    }
    else
    {
        IOT_SET_AND_GOTO_CLEANUP( AWS_IOT_DEFENDER_ALREADY_STARTED );
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

void AwsIotDefender_Stop( void )
{
    if( !_started )
    {
        IotLogWarn( "Defender has not started yet." );
    }
    else
    {
        /* Wait for all the metrics processing to be done, if there are outstanding requests */
        IotSemaphore_Wait( &_doneSem );

        IotTaskPoolJobStatus_t status;
        IotTaskPoolError_t taskPoolError = IotTaskPool_TryCancel( IOT_SYSTEM_TASKPOOL, _metricsPublishJob, &status );

        /* If cancel failed, let it sleep for a while and hope everything finishes. */
        if( taskPoolError != IOT_TASKPOOL_SUCCESS )
        {
            IotLogWarn( "Failed to cancel metrics publish job with return code %d and status %d.", taskPoolError, status );
            IotClock_SleepMs( WAIT_METRICS_JOB_MAX_SECONDS * 1000 );
        }

        /* Unsubscribe MQTT */
        IotLogInfo( "Unsubscribing from MQTT topics" );
        _unsubscribeMqtt();

        /* Destroy metrics' mutex. */
        IotMutex_Destroy( &_AwsIotDefenderMetrics.mutex );

        /* Post so that taken semaphore is returned */
        IotSemaphore_Post( &_doneSem );
        /* Destroy 'done' semaphore. */
        IotSemaphore_Destroy( &_doneSem );

        /* Delete topics names. */
        AwsIotDefenderInternal_DeleteTopicsNames();

        /* Delete report if it was created */
        AwsIotDefenderInternal_DeleteReport();

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
}

/*-----------------------------------------------------------*/

AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint32_t periodSeconds )
{
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

uint32_t AwsIotDefender_GetPeriod( void )
{
    return _defenderToSeconds( _periodMilliSecond );
}

/*-----------------------------------------------------------*/

const char * AwsIotDefender_strerror( AwsIotDefenderError_t error )
{
    const char * pMessage = NULL;

    switch( error )
    {
        case AWS_IOT_DEFENDER_SUCCESS:
            pMessage = "SUCCESS";
            break;

        case AWS_IOT_DEFENDER_INVALID_INPUT:
            pMessage = "INVALID INPUT";
            break;

        case AWS_IOT_DEFENDER_ALREADY_STARTED:
            pMessage = "ALREADY STARTED";
            break;

        case AWS_IOT_DEFENDER_PERIOD_TOO_SHORT:
            pMessage = "PERIOD TOO SHORT";
            break;

        case AWS_IOT_DEFENDER_ERROR_NO_MEMORY:
            pMessage = "NO MEMORY";
            break;

        case AWS_IOT_DEFENDER_INTERNAL_FAILURE:
            pMessage = "INTERNAL FAILURE";
            break;

        default:
            pMessage = "INVALID STATUS";
            break;
    }

    return pMessage;
}

/*-----------------------------------------------------------*/

const char * AwsIotDefender_EventType( AwsIotDefenderEventType_t eventType )
{
    const char * pEvent = NULL;

    /* Convert defent event to string  */
    switch( eventType )
    {
        case AWS_IOT_DEFENDER_METRICS_ACCEPTED:
            pEvent = "Defender Metrics accepted";
            break;

        case AWS_IOT_DEFENDER_METRICS_REJECTED:
            pEvent = "Defender Metrics rejected";
            break;

        case AWS_IOT_DEFENDER_FAILURE_MQTT:
            pEvent = "Defender MQTT operation failed";
            break;

        case AWS_IOT_DEFENDER_FAILURE_METRICS_REPORT: /**< Defender failed to create metrics report. */
            pEvent = "Defender failed to create metrics Report";
            break;

        default:
            pEvent = "Defender Unknown Event";
    }

    return pEvent;
}

/*-----------------------------------------------------------*/

static IotMqttError_t _metricsSubscribeRoutine()
{
    IotLogDebug( "Metrics Subscribe starts." );

    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* Subscribe to accept/reject MQTT topics. */
    mqttError = AwsIotDefenderInternal_MqttSubscribe();

    if( mqttError != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Failed to perform MQTT operations, with error %s.", IotMqtt_strerror( mqttError ) );
        _unsubscribeMqtt();
        /* IotSemaphore_Post( &_doneSem ); */
    }

    return mqttError;
}

/*-----------------------------------------------------------*/

static void _metricsPublishRoutine( IotTaskPool_t pTaskPool,
                                    IotTaskPoolJob_t pJob,
                                    void * pUserContext )
{
    /* Unsed parameter; silence the compiler. */
    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pUserContext;

    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;
    bool reportCreated = false;

    if( !IotSemaphore_TryWait( &_doneSem ) )
    {
        IotLogError( "Defender has been stopped or the previous metrics is in process. No further action." );
    }
    else
    {
        /* Create serialized metrics report. */
        reportCreated = AwsIotDefenderInternal_CreateReport();

        /* If Report is created successfully. */
        if( reportCreated )
        {
            /* Publish report to defender topic. */
            mqttError = AwsIotDefenderInternal_MqttPublish( AwsIotDefenderInternal_GetReportBuffer(),
                                                            AwsIotDefenderInternal_GetReportBufferSize() );

            if( mqttError == IOT_MQTT_SUCCESS )
            {
                IotLogDebug( "Metrics report has been published successfully." );
            }
            else
            {
                IotLogError( "Failed to perform MQTT publish, with error %s.", IotMqtt_strerror( mqttError ) );
            }
        }
        else
        {
            IotLogError( "Failed to create report" );
        }

        if( ( mqttError != IOT_MQTT_SUCCESS ) || ( !reportCreated ) )
        {
            if( reportCreated )
            {
                AwsIotDefenderInternal_DeleteReport();
            }

            _unsubscribeMqtt();
            /* Invoke user's callback if there is. */
            _handleApplicationCallback( AWS_IOT_DEFENDER_FAILURE_MQTT, NULL );
        }
        else
        {
            /* Re-schedule metrics job with period as deferred interval. */
            ( void ) IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, _metricsPublishJob, _periodMilliSecond );
        }

        /* Give Done semaphore so AwsIotDefender_Stop() can proceed */
        IotSemaphore_Post( &_doneSem );
    }

    IotLogDebug( "Publish job ends." );
}

/*-----------------------------------------------------------*/

void _unsubscribeMqtt()
{
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    mqttError = AwsIotDefenderInternal_MqttUnSubscribe();

    if( mqttError != IOT_MQTT_SUCCESS )
    {
        IotLogError( "Unsubscribe failed for defender agent" );
    }
}

/*-----------------------------------------------------------*/

void _acceptCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;

    IotLogInfo( "Metrics report was accepted by defender service." );

    /* In accepted case, report and MQTT message must exist. */
    AwsIotDefender_Assert( AwsIotDefenderInternal_GetReportBuffer() );
    AwsIotDefender_Assert( pPublish->u.message.info.pPayload );

    /* Invoke user's callback with accept event. */
    _handleApplicationCallback( AWS_IOT_DEFENDER_METRICS_ACCEPTED, pPublish );
    /* Delete report if exists */
    AwsIotDefenderInternal_DeleteReport();
}

/*-----------------------------------------------------------*/

void _rejectCallback( void * pArgument,
                      IotMqttCallbackParam_t * const pPublish )
{
    ( void ) pArgument;

    IotLogError( "Metrics report was rejected by defender service." );

    /* In rejected case, MQTT message must exist. */
    AwsIotDefender_Assert( pPublish->u.message.info.pPayload );

    /* Invoke user's callback with rejected event. */
    _handleApplicationCallback( AWS_IOT_DEFENDER_METRICS_REJECTED, pPublish );
    /* Delete report if exists */
    AwsIotDefenderInternal_DeleteReport();
}

/*-----------------------------------------------------------*/

void _handleApplicationCallback( AwsIotDefenderEventType_t event,
                                 IotMqttCallbackParam_t * const pPublish )
{
    /* Invoke user's callback with  event. */
    AwsIotDefenderCallbackInfo_t callbackInfo;

    if( _startInfo.callback.function != NULL )
    {
        callbackInfo.eventType = event;

        callbackInfo.pMetricsReport = AwsIotDefenderInternal_GetReportBuffer();
        callbackInfo.metricsReportLength = AwsIotDefenderInternal_GetReportBufferSize();

        if( pPublish == NULL )
        {
            callbackInfo.pPayload = NULL;
            callbackInfo.payloadLength = 0;
        }
        else
        {
            callbackInfo.pPayload = pPublish->u.message.info.pPayload;
            callbackInfo.payloadLength = pPublish->u.message.info.payloadLength;
        }

        _startInfo.callback.function( _startInfo.callback.pCallbackContext, &callbackInfo );
    }
}

/*-----------------------------------------------------------*/
