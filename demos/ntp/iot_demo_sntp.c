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

/**
 * @file iot_demo_mqtt.c
 * @brief Demonstrates usage of the MQTT library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Set up logging for this demo. */
#include "iot_demo_logging.h"
#include "iot_sntp_client.h"
#include "iot_sntp_date_time.h"
#include "iot_taskpool.h"
#include "iot_mqtt.h"

/*-----------------------------------------------------------*/

/* Demo print routine interval */
#define IOT_SNTP_DEMO_PRINT_INTERVAL    ( ( uint32_t ) 5000 )

/*-----------------------------------------------------------*/

static IotTaskPoolJob_t _iotSntpPrintJob = IOT_TASKPOOL_JOB_INITIALIZER;
static IotTaskPoolJobStorage_t _iotSntpPrintJobStorage = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief This is a task pool function prints UTC time periodically
 * It schedules itself to run after specified amount of time
 */
static void _sntpPrintTimeTask( struct IotTaskPool * pTaskPool,
                                struct IotTaskPoolJob * pJob,
                                void * pUserContext )
{
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;
    IotSntpTimeval_t currentTime = { 0 };
    IotSntpError_t status;

    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pUserContext;

    status = Iot_SntpGetTimeOfDay( &currentTime );

    if( status == IOT_SNTP_ERROR_NONE )
    {
        Iot_SntpPrintDateTime( "Current UTC time:", currentTime.tv_sec, currentTime.tv_usec / 1000 );
    }
    else
    {
        IotLogError( " GetTimeofDay failed " );
    }

    _iotSntpPrintJob = ( const IotTaskPoolJob_t ) {
        0
    };
    IotTaskPool_CreateJob( _sntpPrintTimeTask, NULL, &_iotSntpPrintJobStorage, &_iotSntpPrintJob );
    taskPoolError = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, _iotSntpPrintJob, ( IOT_SNTP_DEMO_PRINT_INTERVAL ) );

    if( taskPoolError != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( "SNTP taskpool failed to schedule display time ( %d )", taskPoolError );
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief The callback function invoked by NTP library indicating successful start of NTP module.
 * @params status[in] status passed by the caller indicating if the NTP client has successfully started.
 * @note This functions schedules _sntpPrintTasks upon successful status that periodically prints current time.
 */

void _sntpDemoProcessCallback( bool status )
{
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;

    if( status == true )
    {
        taskPoolError = IotTaskPool_CreateJob( _sntpPrintTimeTask, ( void * ) NULL, &_iotSntpPrintJobStorage, &_iotSntpPrintJob );

        if( taskPoolError == IOT_TASKPOOL_SUCCESS )
        {
            taskPoolError = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, _iotSntpPrintJob, 0 );

            if( taskPoolError != IOT_TASKPOOL_SUCCESS )
            {
                IotLogError( ( "SNTP Demo failed to schedule print task" ) );
            }
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief The function starts the SNTP module.
 */
void _sntpDemo()
{
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;
    IotSntpError_t status = IOT_SNTP_ERROR_NONE;

    IotLogInfo( ( "SNTP DEMO starts" ) );
    /* Use static config */
    status = Iot_SntpStart( _sntpDemoProcessCallback, NULL );

    if( status != IOT_SNTP_ERROR_NONE )
    {
        IotLogError( ( "SNTP Start Failed" ) );
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the SNTP demo, called by the demo runner.
 * The demo starts NTP library and prints time UTC periodically.
 * @note Log messages display time fetched by NTP client (NTP time) as well as time
 * obtained by the application ( Current UTC time ). Log messages also display correction
 * made after each fetch by the client.
 * @warning all parameters are ignored as they are not used by SNTP module.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunSntpDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface )
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;

    ( void ) pIdentifier;
    ( void ) awsIotMqttMode;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;


    _sntpDemo();

    /* Wait till demo is terminated */
    while( 1 )
    {
        IotClock_SleepMs( 100 );
    }

    return status;
}
