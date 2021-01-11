/*
 * FreeRTOS Common IO V0.1.3
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

/*******************************************************************************
 * @file test_iot_rtc.c
 * @brief Functional Unit Test - RTC
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_rtc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*-----------------------------------------------------------*/
#define testIotRTC_DEFAULT_INVALID_DATE        ( 0xFFFF )
#define testIotRTC_DEFAULT_CURRENT_WEEK_DAY    ( 2 )    /* 1st of January 2019 is a tuesday */
#define testIotRTC_DEFAULT_CURRENT_YEAR        ( 119 )  /* 2019 */
#define testIotRTC_DEFAULT_CURRENT_MONTH       ( 0 )    /* January*/
#define testIotRTC_DEFAULT_CURRENT_DAY         ( 1 )    /* 1st of the month */
#define testIotRTC_DEFAULT_CURRENT_HOUR        ( 0 )    /* 0th hour */
#define testIotRTC_DEFAULT_CURRENT_MINUTE      ( 0 )    /* 0th minute */
#define testIotRTC_DEFAULT_CURRENT_SECOND      ( 10 )   /* 10th second */

#define testIotRTC_SEC_TO_MSEC                 ( 1000 )
#define testIotRTC_DELAY_SECONDS               ( 2 )

#define testIotRTC_DEFAULT_ALARM_TIME          ( 5 )  /* Current time + 5 sec */
#define testIotRTC_DEFAULT_WAKEUP_TIME         ( 15 ) /* 15 milliseconds from now. */

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
int32_t uctestIotRTCInstanceIdx = 0; /**< The RTC instance index to use */


/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotRtcSemaphore = NULL;


/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_RTC );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_RTC )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_RTC )
{
}

/*-----------------------------------------------------------*/
static void prvRtcAlarmCallback( IotRtcStatus_t xStatus,
                                 void * pvUserContext )
{
    xSemaphoreGiveFromISR( xtestIotRtcSemaphore, NULL );
}

/*-----------------------------------------------------------*/
static void prvRtcWakeupCallback( IotRtcStatus_t xStatus,
                                  void * pvUserContext )
{
    xSemaphoreGiveFromISR( xtestIotRtcSemaphore, NULL );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_RTC )
{
    xtestIotRtcSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotRtcSemaphore );

    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcOpenClose );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcOpenCloseFailure );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcSetGetDateTime );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcSetGetAlarm );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcCancelAlarm );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcSetGetWakeup );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcCancelWakeup );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcGetStatus );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcSetWakeupInvalid );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcSetAlarmInvalid );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcSetDateTimeFuzzing );
    RUN_TEST_CASE( TEST_IOT_RTC, AFQP_IotRtcGetDateTimeFuzzing );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_rtc_open and iot_rtc_close.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcOpenClose )
{
    IotRtcHandle_t xRtcHandle;
    int32_t lRetVal;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    /* Open rtc to initialize hardware. */
    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

    /* Try to close with non-null handle */
    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test iot_rtc_open with negative instance
 * to force a failure.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcOpenCloseFailure )
{
    IotRtcHandle_t xRtcHandle1, xRtcHandle2;
    int32_t lRetVal;

    /* Open rtc with invalid instance */
    xRtcHandle1 = iot_rtc_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xRtcHandle1 );

    /* Open rtc to initialize hardware. */
    xRtcHandle1 = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle1 );

    if( TEST_PROTECT() )
    {
        /* Open rtc again. */
        xRtcHandle2 = iot_rtc_open( uctestIotRTCInstanceIdx );
        TEST_ASSERT_EQUAL( NULL, xRtcHandle2 );
    }

    /* Close handle */
    lRetVal = iot_rtc_close( xRtcHandle1 );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

    /* Close stale handle */
    lRetVal = iot_rtc_close( xRtcHandle1 );
    TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test set and get date&time.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcSetGetDateTime )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    IotRtcDatetime_t xGetDateTime;
    int32_t lRetVal, lStatus;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    if( TEST_PROTECT() )
    {
        /* Get the status and check timer is stopped */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eGetRtcStatus,
                                 ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eRtcTimerStopped, lStatus );

        xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
        xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
        xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
        xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
        xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
        xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;
        xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;

        /* Try to get the date and time before the timer is started */
        lRetVal = iot_rtc_get_datetime( xRtcHandle, &xGetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_NOT_STARTED, lRetVal );

        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Get the status and check timer is running */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eGetRtcStatus,
                                 ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eRtcTimerRunning, lStatus );


        /* Delay for 2 sec. */
        vTaskDelay( ( testIotRTC_DELAY_SECONDS * testIotRTC_SEC_TO_MSEC ) / portTICK_PERIOD_MS );

        /* Get the date and time*/
        lRetVal = iot_rtc_get_datetime( xRtcHandle, &xGetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Make sure the current time is greater than what is set. */
        TEST_ASSERT_EQUAL( xSetDateTime.ucWday, xGetDateTime.ucWday );
        TEST_ASSERT_EQUAL( xSetDateTime.usYear, xGetDateTime.usYear );
        TEST_ASSERT_EQUAL( xSetDateTime.ucMonth, xGetDateTime.ucMonth );
        TEST_ASSERT_EQUAL( xSetDateTime.ucDay, xGetDateTime.ucDay );
        TEST_ASSERT_EQUAL( xSetDateTime.ucHour, xGetDateTime.ucHour );
        TEST_ASSERT_EQUAL( xSetDateTime.ucMinute, xGetDateTime.ucMinute );
        TEST_ASSERT_GREATER_THAN_UINT32( xSetDateTime.ucSecond, xGetDateTime.ucSecond );
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set/get Alarm
 * and make sure Alarm triggered correctly
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcSetGetAlarm )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    IotRtcDatetime_t xAlarmSetDateTime;
    IotRtcDatetime_t xAlarmGetDateTime;
    int32_t lRetVal, lStatus;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
    xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
    xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
    xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
    xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
    xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;
    xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;

    if( TEST_PROTECT() )
    {
        /* Try to set the Alarm time before starting the timer */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_NOT_STARTED, lRetVal );

        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up the callback */
        iot_rtc_set_callback( xRtcHandle,
                              prvRtcAlarmCallback,
                              NULL );

        /* Get the date and time*/
        lRetVal = iot_rtc_get_datetime( xRtcHandle, &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up alarm to happen after 5 sec. */
        xAlarmSetDateTime.ucSecond += testIotRTC_DEFAULT_ALARM_TIME;

        /* Set the Alarm time */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Get the Alarm time and verify */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eGetRtcAlarm,
                                 ( void * const ) &xAlarmGetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Make sure Alarm is set correctly */
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.usYear, xAlarmGetDateTime.usYear );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucMonth, xAlarmGetDateTime.ucMonth );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucDay, xAlarmGetDateTime.ucDay );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucHour, xAlarmGetDateTime.ucHour );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucMinute, xAlarmGetDateTime.ucMinute );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucSecond, xAlarmGetDateTime.ucSecond );

        /* Wait for the Alarm to trigger callback */
        lRetVal = xSemaphoreTake( xtestIotRtcSemaphore,
                                  pdMS_TO_TICKS( ( testIotRTC_DEFAULT_ALARM_TIME + 1 ) * testIotRTC_SEC_TO_MSEC ) );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Get the status and check */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eGetRtcStatus,
                                 ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eRtcTimerAlarmTriggered,
                           lStatus & eRtcTimerAlarmTriggered );
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set/cancel Alarm
 * and make sure Alarm is NOT triggered after cancel.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcCancelAlarm )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    IotRtcDatetime_t xAlarmSetDateTime;
    IotRtcDatetime_t xAlarmGetDateTime;
    int32_t lRetVal, lStatus;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
    xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
    xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
    xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
    xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
    xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
    xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;

    if( TEST_PROTECT() )
    {
        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up the callback */
        iot_rtc_set_callback( xRtcHandle,
                              prvRtcAlarmCallback,
                              NULL );

        /* Get the date and time*/
        lRetVal = iot_rtc_get_datetime( xRtcHandle, &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up alarm to happen after 5 sec. */
        xAlarmSetDateTime.ucSecond += testIotRTC_DEFAULT_ALARM_TIME;

        /* Set the Alarm time */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Get the Alarm time and verify */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eGetRtcAlarm,
                                 ( void * const ) &xAlarmGetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Make sure Alarm is set correctly */
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucWday, xAlarmGetDateTime.ucWday );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.usYear, xAlarmGetDateTime.usYear );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucMonth, xAlarmGetDateTime.ucMonth );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucDay, xAlarmGetDateTime.ucDay );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucHour, xAlarmGetDateTime.ucHour );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucMinute, xAlarmGetDateTime.ucMinute );
        TEST_ASSERT_EQUAL( xAlarmSetDateTime.ucSecond, xAlarmGetDateTime.ucSecond );

        /* Cancel the Alarm */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eCancelRtcAlarm,
                                 NULL );

        if( lRetVal != IOT_RTC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

            /* Verify that call-back is NOT called */
            lRetVal = xSemaphoreTake( xtestIotRtcSemaphore,
                                      pdMS_TO_TICKS( ( testIotRTC_DEFAULT_ALARM_TIME + 1 ) * testIotRTC_SEC_TO_MSEC ) );
            TEST_ASSERT_EQUAL( pdFALSE, lRetVal );
        }
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set/get Wakeup
 * timer and verify wake-up
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcSetGetWakeup )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    int32_t lRetVal, lStatus;
    uint32_t lSetWakeupMilliSeconds;
    uint32_t lGetWakeupMilliSeconds;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
    xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
    xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
    xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
    xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
    xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
    xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;

    if( TEST_PROTECT() )
    {
        /* Try to set the wakeup count before the timer is started */
        lSetWakeupMilliSeconds = testIotRTC_DEFAULT_WAKEUP_TIME;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcWakeupTime,
                                 ( void * const ) &lSetWakeupMilliSeconds );

        if( lRetVal != IOT_RTC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_RTC_NOT_STARTED, lRetVal );
        }

        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up the callback */
        iot_rtc_set_callback( xRtcHandle,
                              prvRtcWakeupCallback,
                              NULL );

        /* Set the wakeup count */
        lSetWakeupMilliSeconds = testIotRTC_DEFAULT_WAKEUP_TIME;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcWakeupTime,
                                 ( void * const ) &lSetWakeupMilliSeconds );

        if( lRetVal != IOT_RTC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

            /* Get the wakeup count and check */
            lRetVal = iot_rtc_ioctl( xRtcHandle,
                                     eGetRtcWakeupTime,
                                     ( void * const ) &lGetWakeupMilliSeconds );
            TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
            TEST_ASSERT_NOT_EQUAL( 0, lGetWakeupMilliSeconds );
            TEST_ASSERT_EQUAL( lSetWakeupMilliSeconds, lGetWakeupMilliSeconds );

            /* Wait for the Wakeup to trigger callback */
            lRetVal = xSemaphoreTake( xtestIotRtcSemaphore,
                                      pdMS_TO_TICKS( ( testIotRTC_DEFAULT_WAKEUP_TIME + 2 ) ) );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* Get the status and check */
            lRetVal = iot_rtc_ioctl( xRtcHandle,
                                     eGetRtcStatus,
                                     ( void * const ) &lStatus );
            TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( eRtcTimerWakeupTriggered, lStatus );
        }
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set/cancel Wakeup
 * timer.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcCancelWakeup )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    int32_t lRetVal, lStatus;
    uint32_t lSetWakeupSeconds;
    uint32_t lGetWakeupSeconds;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
    xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
    xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
    xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
    xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
    xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
    xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;

    if( TEST_PROTECT() )
    {
        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up the callback */
        iot_rtc_set_callback( xRtcHandle,
                              prvRtcWakeupCallback,
                              NULL );

        /* Set the wakeup count */
        lSetWakeupSeconds = testIotRTC_DEFAULT_WAKEUP_TIME;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcWakeupTime,
                                 ( void * const ) &lSetWakeupSeconds );

        if( lRetVal != IOT_RTC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

            /* Get the wakeup count and check */
            lRetVal = iot_rtc_ioctl( xRtcHandle,
                                     eGetRtcWakeupTime,
                                     ( void * const ) &lGetWakeupSeconds );
            TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
            TEST_ASSERT_NOT_EQUAL( 0, lGetWakeupSeconds );
            TEST_ASSERT_EQUAL( lSetWakeupSeconds, lGetWakeupSeconds );

            /* Cancel wakeup timer */
            lRetVal = iot_rtc_ioctl( xRtcHandle,
                                     eCancelRtcWakeup,
                                     NULL );

            if( lRetVal != IOT_RTC_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

                /* Verify that call-back is NOT called */
                lRetVal = xSemaphoreTake( xtestIotRtcSemaphore,
                                          pdMS_TO_TICKS( testIotRTC_DEFAULT_WAKEUP_TIME + 1000 ) );
                TEST_ASSERT_EQUAL( pdFALSE, lRetVal );
            }
        }
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test GetRtcStatus
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcGetStatus )
{
    IotRtcHandle_t xRtcHandle;
    int32_t lRetVal, lStatus;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    if( TEST_PROTECT() )
    {
        /* Get the status and check timer is stopped */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eGetRtcStatus,
                                 ( void * const ) &lStatus );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eRtcTimerStopped, lStatus );
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set time, set invalid wake up time
 * verify wakeup is ignored.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcSetWakeupInvalid )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    int32_t lRetVal;
    uint32_t lSetWakeupMilliSeconds;
    uint32_t lGetWakeupMilliSeconds;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    if( TEST_PROTECT() )
    {
        xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
        xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
        xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
        xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
        xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
        xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
        xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;

        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Call ioctl with NULL handle.  Expect error.*/
        lRetVal = iot_rtc_ioctl( NULL,
                                 eSetRtcWakeupTime,
                                 ( void * const ) &lSetWakeupMilliSeconds );
        TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );

        /* Call ioctl with invalid enum.  Expect error.*/
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 -1,
                                 ( void * const ) &lSetWakeupMilliSeconds );
        TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );

        /* Set the wakeup count */
        lSetWakeupMilliSeconds = 0;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcWakeupTime,
                                 ( void * const ) &lSetWakeupMilliSeconds );

        if( lRetVal != IOT_RTC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );
        }
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set time, set Alarm in the past and
 * verify that invalid error is returned.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcSetAlarmInvalid )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    IotRtcDatetime_t xAlarmSetDateTime;
    int32_t lRetVal;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
    xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
    xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
    xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
    xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
    xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
    xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;

    if( TEST_PROTECT() )
    {
        /* Set the date and time*/
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up the callback */
        iot_rtc_set_callback( xRtcHandle,
                              prvRtcAlarmCallback,
                              NULL );

        /* Get the date and time*/
        lRetVal = iot_rtc_get_datetime( xRtcHandle, &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );

        /* Set up alarm in the past. */
        xAlarmSetDateTime.ucSecond -= testIotRTC_DEFAULT_ALARM_TIME;

        /* Set the Alarm time */
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );

        xAlarmSetDateTime.ucSecond = xSetDateTime.ucSecond + testIotRTC_DEFAULT_ALARM_TIME;
        xAlarmSetDateTime.ucWday = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        xAlarmSetDateTime.ucWday = xSetDateTime.ucWday;
        xAlarmSetDateTime.ucMonth = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        xAlarmSetDateTime.ucMonth = xSetDateTime.ucMonth;
        xAlarmSetDateTime.ucDay = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        xAlarmSetDateTime.ucDay = xSetDateTime.ucDay;
        xAlarmSetDateTime.ucHour = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        xAlarmSetDateTime.ucHour = xSetDateTime.ucHour;
        xAlarmSetDateTime.ucMinute = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        xAlarmSetDateTime.ucMinute = xSetDateTime.ucMinute;
        xAlarmSetDateTime.ucSecond = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_ioctl( xRtcHandle,
                                 eSetRtcAlarm,
                                 ( void * const ) &xAlarmSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test set the data&time in the past and verify
 * an invalid error is returned correctly.
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcSetDateTimeFuzzing )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xSetDateTime;
    IotRtcDatetime_t xGetDateTime;
    int32_t lRetVal;

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
    xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
    xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
    xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
    xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
    xSetDateTime.ucSecond = testIotRTC_DEFAULT_CURRENT_SECOND;

    if( TEST_PROTECT() )
    {
        /* Set the date and time with invalid month*/
        xSetDateTime.usYear = testIotRTC_DEFAULT_CURRENT_YEAR;
        xSetDateTime.ucMonth = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        /* Set the date and time with invalid week day*/
        xSetDateTime.ucMonth = testIotRTC_DEFAULT_CURRENT_MONTH;
        xSetDateTime.ucWday = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        /* Set the date and time with invalid day*/
        xSetDateTime.ucWday = testIotRTC_DEFAULT_CURRENT_WEEK_DAY;
        xSetDateTime.ucDay = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        /* Set the date and time with invalid hour*/
        xSetDateTime.ucDay = testIotRTC_DEFAULT_CURRENT_DAY;
        xSetDateTime.ucHour = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        /* Set the date and time with invalid minute*/
        xSetDateTime.ucHour = testIotRTC_DEFAULT_CURRENT_HOUR;
        xSetDateTime.ucMinute = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );

        /* Set the date and time with invalid second*/
        xSetDateTime.ucMinute = testIotRTC_DEFAULT_CURRENT_MINUTE;
        xSetDateTime.ucSecond = testIotRTC_DEFAULT_INVALID_DATE;
        lRetVal = iot_rtc_set_datetime( xRtcHandle, &xSetDateTime );
        TEST_ASSERT_EQUAL( IOT_RTC_SET_FAILED, lRetVal );
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of get datetime
 *
 */
TEST( TEST_IOT_RTC, AFQP_IotRtcGetDateTimeFuzzing )
{
    IotRtcHandle_t xRtcHandle;
    IotRtcDatetime_t xGetDateTime;
    int32_t lRetVal;

    /* Get the date and time with NULL handle*/
    lRetVal = iot_rtc_get_datetime( NULL, &xGetDateTime );
    TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );

    /* Open rtc to initialize hardware. */
    xRtcHandle = iot_rtc_open( uctestIotRTCInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xRtcHandle );

    if( TEST_PROTECT() )
    {
        /* Get the date and time with NULL buffer*/
        lRetVal = iot_rtc_get_datetime( xRtcHandle, NULL );
        TEST_ASSERT_EQUAL( IOT_RTC_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_rtc_close( xRtcHandle );
    TEST_ASSERT_EQUAL( IOT_RTC_SUCCESS, lRetVal );
}
