/*
 * Amazon FreeRTOS SNTP V1.0.0
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

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/* IOT includes */
#include "iot_init.h"
#include "iot_taskpool.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* sntp include */
#include "iot_sntp_config.h"
#include "iot_sntp_client.h"
#include "iot_sntp_date_time.h"

#define TEST_NTP_MAX_GET_TIME_ATTEMPTS    ( ( uint32_t ) 1000 )

static IotSemaphore_t testSntpSem;
static bool testSemCreated = false;
static IotTaskPoolJob_t _testSntpGetTimeJob = IOT_TASKPOOL_JOB_INITIALIZER;
static IotTaskPoolJobStorage_t _testSntpJobStorage = { 0 };
static IotSntpConfig_t _testSntpConfig = { 0 };

/* Group declaration required by Unity framework. */
TEST_GROUP( Full_SNTP );

/* Setup required by Unity framework. */
TEST_SETUP( Full_SNTP )
{
    TEST_ASSERT_EQUAL( true, IotSdk_Init() );

    /* Setup config */

    /* Zero polling periond for only one fetch from server. This is useful for running multiple tests */
    _testSntpConfig.sntpPollingPeriod = 0;
    _testSntpConfig.sntpSocketTimeout = IOT_SNTP_SOCKET_TIMEOUT;
    _testSntpConfig.sntpUseRecvCallback = true;
    _testSntpConfig.sntpServerCount = 3;
    _testSntpConfig.pActiveNTPServers[ 0 ] = "0.north-america.pool.ntp.org";
    _testSntpConfig.pActiveNTPServers[ 1 ] = "1.north-america.pool.ntp.org";
    _testSntpConfig.pActiveNTPServers[ 2 ] = "2.north-america.pool.ntp.org";
}

/* Tear down required by Unity framework. */
TEST_TEAR_DOWN( Full_SNTP )
{
    if( TEST_PROTECT() )
    {
        if( testSemCreated )
        {
            IotSemaphore_Destroy( &testSntpSem );
            testSemCreated = false;
        }
    }
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_SNTP )
{
    RUN_TEST_CASE( Full_SNTP, AFR_SNTP_SendRecv );
    RUN_TEST_CASE( Full_SNTP, AFR_SNTP_SendRecvWithCallBack );
    RUN_TEST_CASE( Full_SNTP, AFR_SNTP_InvalidConfig );
}
/*-----------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*-----------------------Begin Tests---------------------------------*/
/*-------------------------------------------------------------------*/

/*
 * @brief function to get time and make sure library returns success
 */
static void prvTestGetTime()
{
    IotSntpError_t status = IOT_SNTP_ERROR_NONE;
    IotSntpTimeval_t tv = { 0 };

    /* Get time of day */
    status = Iot_SntpGetTimeOfDay( &tv );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_SNTP_ERROR_NONE, status, "SNTP Get time failed" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, tv.tv_sec, "SNTP Get time zero second and no error" );
    /* Post Semaphore so that calling test thread completes */
    TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
    IotSemaphore_Post( &testSntpSem );
}

static void prvTestGetTimeCallback( bool sntpStatus )
{
    TEST_ASSERT_EQUAL_MESSAGE( true, sntpStatus, "SNTP callback indicated error " );
    prvTestGetTime();
}

static void prvTestGetTimeTask( IotTaskPool_t pTaskPool,
                                IotTaskPoolJob_t pJob,
                                void * pUserContext )
{
    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pUserContext;

    prvTestGetTime( );
}
/*-------------------------------------------------------------------*/

TEST( Full_SNTP, AFR_SNTP_SendRecv )
{
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;
    IotSntpError_t sntpStatus = IOT_SNTP_ERROR_NONE;
    bool status = true;
    IotSntpTimeval_t tv = { 0 };

    /* Make sure SntpGettime returns error without initialization */
    sntpStatus = Iot_SntpGetTimeOfDay( &tv );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( IOT_SNTP_ERROR_NONE, sntpStatus, "SNTP Get time return success without init" );
    TEST_ASSERT_EQUAL_MESSAGE( 0, tv.tv_sec, "SNTP Get time set value whithout init" );

    /* Initialize SNTP and trigger fetch cycle from server */
    sntpStatus = Iot_SntpStart( NULL, &_testSntpConfig );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_SNTP_ERROR_NONE, sntpStatus, "SNTP fetch time failed" );
    
    if( TEST_PROTECT() )
    {
        /* Create semaphore */
        status = IotSemaphore_Create( &testSntpSem, 0, 1 );
        TEST_ASSERT_EQUAL_MESSAGE( true, status, "Failed to create semaphore." );
        testSemCreated = true;

        /* Schedule a taskpool job to fetch time */
        taskPoolError = IotTaskPool_CreateJob( prvTestGetTimeTask, ( void * ) NULL, &_testSntpJobStorage, &_testSntpGetTimeJob );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_TASKPOOL_SUCCESS, taskPoolError, "IotTaskPool_CreateJon failed" );

        taskPoolError = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, _testSntpGetTimeJob, 2000 );
        TEST_ASSERT_EQUAL_MESSAGE( IOT_TASKPOOL_SUCCESS, taskPoolError, "IotTaskPool_ScheduleDeferred failed" );

        /* Wait on semaphore */
        TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
        IotSemaphore_Wait( &testSntpSem );

        /* destroy semaphore */
        IotSemaphore_Destroy( &testSntpSem );
        testSemCreated = false;
    }
}
/*-----------------------------------------------------------*/


TEST( Full_SNTP, AFR_SNTP_SendRecvWithCallBack )
{
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;
    IotSntpError_t sntpStatus = IOT_SNTP_ERROR_NONE;
    bool status = true;
    IotSntpTimeval_t tv = { 0 };

    /* Initialize SNTP and trigger fetch cycle from server with callback */
    sntpStatus = Iot_SntpStart( prvTestGetTimeCallback, &_testSntpConfig );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_SNTP_ERROR_NONE, sntpStatus, "SNTP fetch time failed" );

    if( TEST_PROTECT() )
    {
        /* Create semaphore */
        status = IotSemaphore_Create( &testSntpSem, 0, 1 );
        TEST_ASSERT_EQUAL_MESSAGE( true, status, "Failed to create semaphore." );
        testSemCreated = true;

        /* Wait on semaphore */
        TEST_ASSERT_EQUAL_MESSAGE( true, testSemCreated, " Test semaphore was not created" );
        IotSemaphore_Wait( &testSntpSem );

        /* destroy semaphore */
        IotSemaphore_Destroy( &testSntpSem );
        testSemCreated = false;
    }
}

/*-------------------------------------------------------------------*/
TEST( Full_SNTP, AFR_SNTP_InvalidConfig )
{
    IotSntpError_t sntpStatus = IOT_SNTP_ERROR_NONE;

    _testSntpConfig.sntpServerCount = IOT_MAX_SNTP_SERVERS + 1;
    /* Initialize SNTP and trigger fetch cycle from server with callback */
    sntpStatus = Iot_SntpStart( NULL, &_testSntpConfig );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_SNTP_INIT_FAILED, sntpStatus, "SNTP fetch time succeeded with invalid server count" );
    /* reset server count */
    _testSntpConfig.sntpServerCount = 3;

    /* set invalid polling period */
    _testSntpConfig.sntpPollingPeriod = IOT_SNTP_MIN_POLLING_PERIOD - 1;
    sntpStatus = Iot_SntpStart( NULL, &_testSntpConfig );
    TEST_ASSERT_EQUAL_MESSAGE( IOT_SNTP_INIT_FAILED, sntpStatus, "SNTP fetch time succeeded with invalid polling period" );
}
