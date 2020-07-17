/*
 * FreeRTOS Platform V1.1.2
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

/**
 * @file iot_tests_platform_clock.c
 * @brief Tests for the functions in iot_clock.h
 */

#include "iot_config.h"

/* Test framework includes. */
#include <string.h>
#include "unity_fixture.h"

#include "platform/iot_clock.h"

/*-----------------------------------------------------------*/

/**
 * @brief Test group for Platform Clock tests.
 */
TEST_GROUP( UTIL_Platform_Clock );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Platform Clock tests.
 */
TEST_SETUP( UTIL_Platform_Clock )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Platform Clock tests.
 */
TEST_TEAR_DOWN( UTIL_Platform_Clock )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Platform Clock tests.
 */
TEST_GROUP_RUNNER( UTIL_Platform_Clock )
{
    RUN_TEST_CASE( UTIL_Platform_Clock, IotClock_GetTimestring );
    RUN_TEST_CASE( UTIL_Platform_Clock, IotClock_GetTimeMs );
    RUN_TEST_CASE( UTIL_Platform_Clock, IotClock_Timer );
    RUN_TEST_CASE( UTIL_Platform_Clock, IotClock_TimerCancellation );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Time String generation
 */
TEST( UTIL_Platform_Clock, IotClock_GetTimestring )
{
    char buffer[ 128 ] = { 0 };
    size_t timeStringSize = 0;

    IotClock_GetTimestring(
        buffer,
        sizeof( buffer ),
        &timeStringSize );

    /* Check that we got a string and not empty */
    TEST_ASSERT_NOT_EQUAL( 0, timeStringSize );

    /* Check that the string size matches what was reported */
    TEST_ASSERT_EQUAL( timeStringSize, strlen( buffer ) );

    /* Now make sure that this all works correctly also if we have a tiny buffer */
    IotClock_GetTimestring(
        buffer,
        1,
        &timeStringSize );

    /* Check that we got a string and not empty */
    TEST_ASSERT_GREATER_THAN( 1, timeStringSize );

    /* Check that the string size matches what was reported */
    TEST_ASSERT_EQUAL( 0, strlen( buffer ) );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test IotClock_GetTimeMs
 */
TEST( UTIL_Platform_Clock, IotClock_GetTimeMs )
{
    uint64_t startTime = 0;
    uint64_t endTime = 0;

    startTime = IotClock_GetTimeMs();

    /* delay for 1s */
    vTaskDelay( configTICK_RATE_HZ );

    endTime = IotClock_GetTimeMs();

    /* We expect accuracy to be better than 10%, so these should be within 100ms */
    TEST_ASSERT_INT32_WITHIN( 100, startTime + 1000, endTime );
}


/**
 * @brief Test IotClock_Timer
 */

void IotClock_GetTimeMs_Test_Callback( void * param )
{
    /* Indicate that the timer has expired */
    *( int * ) param = 1;
}

TEST( UTIL_Platform_Clock, IotClock_Timer )
{
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    IotTimer_t testTimer;
    /* Define volatile because otherwise CC-RX (Renesas) compiler will optimize out this local variable. */
    volatile int completionFlag = 0;

    startTime = IotClock_GetTimeMs();

    IotClock_TimerCreate( &testTimer,
                          IotClock_GetTimeMs_Test_Callback,
                          ( void * ) &completionFlag );

    /* Arm the timer for 1s and no repeats */
    IotClock_TimerArm( &testTimer,
                       1000,
                       0 );

    /* We block until the timer has returned or we time out*/
    while( completionFlag == 0 )
    {
    }

    endTime = IotClock_GetTimeMs();
    IotClock_TimerDestroy( &testTimer );

    /* Timer was set for 1000ms, make sure we ended within 100ms of 1000ms */
    TEST_ASSERT_INT32_WITHIN( 100, startTime + 1000, endTime );
}



TEST( UTIL_Platform_Clock, IotClock_TimerCancellation )
{
    IotTimer_t testTimer;
    int completionFlag = 0;

    IotClock_TimerCreate( &testTimer,
                          IotClock_GetTimeMs_Test_Callback,
                          &completionFlag );

    /* Arm the timer for 100ms and no repeats */
    IotClock_TimerArm( &testTimer,
                       100,
                       0 );

    /* We block for 1 tick */
    vTaskDelay( 1 );

    /* Then stop and destroy the timer before the callback can happen */
    IotClock_TimerDestroy( &testTimer );

    /* Wait 2 seconds to ensure the timer callback did not happen anyway */
    vTaskDelay( 2 * configTICK_RATE_HZ );

    /* Pass if we get there and the timer never fired */
    TEST_ASSERT_EQUAL( 0, completionFlag );
}

/*-----------------------------------------------------------*/
