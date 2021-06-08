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
 * @file test_iot_timer.c
 * @brief Functional Unit Test - TIMER
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_timer.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define testIotTIMER_DEFAULT_DELAY_US     500
#define testIotTIMER_SLEEP_DELAY_MS       1

/*
 * @brief timeout for the semaphore to be invoked by the iot_timer. 4 times larger to account
 * for timer resolution variances.
 */
#define testIotTIMER_CALLBACK_DELAY_US    ( testIotTIMER_DEFAULT_DELAY_US * 4 )


/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
IotTimerHandle_t gIotTimerHandle;
int32_t ltestIotTimerInstance = 0;

/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotTimerSemaphore = NULL;

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_TIMER );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_TIMER )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_TIMER )
{
}

/*-----------------------------------------------------------*/
static void prvTimerCallback( void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR( xtestIotTimerSemaphore, &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_TIMER )
{
    xtestIotTimerSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotTimerSemaphore );

    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_OpenClose );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_Running );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_Stop );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_Callback );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_NoCallback );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_StartAgain );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_Cancel );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_OpenFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_CloseFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_StartFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_GetFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_StopFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_CallbackFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_CancelFuzzing );
    RUN_TEST_CASE( TEST_IOT_TIMER, AFWP_IotTimer_DelayFuzzing );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_timer_open and iot_timer_close.
 *
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_OpenClose )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

    /* Set the global TimerHandle to NULL as we closed the handle */
    gIotTimerHandle = NULL;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test getMicroSeconds
 *
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_Running )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;
    uint64_t ulMicroSeconds1 = 0;
    uint64_t ulMicroSeconds2 = 0;
    uint64_t ulDelay = testIotTIMER_SLEEP_DELAY_MS * 5;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Delay for 1 msec. */
        vTaskDelay( testIotTIMER_SLEEP_DELAY_MS / portTICK_PERIOD_MS );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds1 );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Make sure MicroSeconds returned is not '0' */
        TEST_ASSERT_NOT_EQUAL( 0, ulMicroSeconds1 );

        /* Delay for 5 msec. */
        vTaskDelay( ( testIotTIMER_SLEEP_DELAY_MS * 5 ) / portTICK_PERIOD_MS );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds2 );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Make sure second value is greater than origin + delay within 2msec
         * range (worst case scenario with 1ms resolution and two sleeps)  */
        TEST_ASSERT_GREATER_THAN_UINT32( ( ulMicroSeconds1 + ( ( ulDelay - 2 ) * 1000 ) ), ulMicroSeconds2 );
        TEST_ASSERT_GREATER_THAN_UINT32( ulMicroSeconds2, ( ulMicroSeconds1 + ( ( ulDelay + 2 ) * 1000 ) ) );

        /* Stop the timer */
        lRetVal = iot_timer_stop( xTimerHandle );

        if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
        }
    }

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test stopping the timre
 *
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_Stop )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;
    uint64_t ulMicroSeconds1 = 0;
    uint64_t ulMicroSeconds2 = 0;
    uint64_t ulDelay = testIotTIMER_SLEEP_DELAY_MS * 5;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Delay for 1 msec. */
        vTaskDelay( testIotTIMER_SLEEP_DELAY_MS / portTICK_PERIOD_MS );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds1 );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Make sure MicroSeconds returned is not '0' */
        TEST_ASSERT_NOT_EQUAL( 0, ulMicroSeconds1 );

        /* Delay for 5 msec. */
        vTaskDelay( ( testIotTIMER_SLEEP_DELAY_MS * 5 ) / portTICK_PERIOD_MS );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds2 );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Make sure second value is greater than origin + delay within 2msec
         * range (worst case scenario with 1ms resolution and two sleeps)  */
        TEST_ASSERT_GREATER_THAN_UINT32( ( ulMicroSeconds1 + ( ( ulDelay - 2 ) * 1000 ) ), ulMicroSeconds2 );
        TEST_ASSERT_GREATER_THAN_UINT32( ulMicroSeconds2, ( ulMicroSeconds1 + ( ( ulDelay + 2 ) * 1000 ) ) );

        /* Stop the timer */
        lRetVal = iot_timer_stop( xTimerHandle );

        if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

            /* Get the time in micro seconds */
            lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds1 );
            TEST_ASSERT_EQUAL( IOT_TIMER_NOT_RUNNING, lRetVal );
        }
    }

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_timer_delay
 * and verify that callback is called correctly.
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_Callback )
{
    IotTimerHandle_t xTimerHandle;
    uint64_t ulMicroSeconds;
    int32_t lRetVal;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Set up the callback */
        iot_timer_set_callback( xTimerHandle,
                                prvTimerCallback,
                                NULL );

        /* Set up the delay */
        lRetVal = iot_timer_delay( xTimerHandle,
                                   testIotTIMER_DEFAULT_DELAY_US );

        /* Expect either "Not running" or "Success" */
        if( lRetVal != IOT_TIMER_NOT_RUNNING )
        {
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
        }

        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Wait for the Delay callback to be called. */
        lRetVal = xSemaphoreTake( xtestIotTimerSemaphore, testIotTIMER_CALLBACK_DELAY_US );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Get the time in micro seconds
         * Make sure the current micro-seconds value is greater than the delay
         */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
        TEST_ASSERT_GREATER_THAN_UINT32( testIotTIMER_DEFAULT_DELAY_US, ulMicroSeconds );
    }

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test no callback when timer stopped
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_NoCallback )
{
    IotTimerHandle_t xTimerHandle;
    uint64_t ulMicroSeconds;
    int32_t lRetVal;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Set up the callback */
        iot_timer_set_callback( xTimerHandle,
                                prvTimerCallback,
                                NULL );

        /* Set up the delay */
        lRetVal = iot_timer_delay( xTimerHandle,
                                   testIotTIMER_DEFAULT_DELAY_US );

        /* Expect either "Not running" or "Success" */
        if( lRetVal != IOT_TIMER_NOT_RUNNING )
        {
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
        }

        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Stop the timer */
        lRetVal = iot_timer_stop( xTimerHandle );

        if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
        {
            /* Wait for the Delay.  Make sure callback is not called. */
            lRetVal = xSemaphoreTake( xtestIotTimerSemaphore, testIotTIMER_CALLBACK_DELAY_US );
            TEST_ASSERT_NOT_EQUAL( pdTRUE, lRetVal );
        }
    }

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test timer stopped, then started again
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_StartAgain )
{
    IotTimerHandle_t xTimerHandle;
    uint64_t ulMicroSeconds;
    int32_t lRetVal;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Set up the callback */
        iot_timer_set_callback( xTimerHandle,
                                prvTimerCallback,
                                NULL );

        /* Set up the delay */
        lRetVal = iot_timer_delay( xTimerHandle,
                                   testIotTIMER_DEFAULT_DELAY_US );

        /* Expect either "Not running" or "Success" */
        if( lRetVal != IOT_TIMER_NOT_RUNNING )
        {
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
        }

        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Get the time in micro seconds */
        lRetVal = iot_timer_get_value( xTimerHandle, &ulMicroSeconds );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Stop the timer */
        lRetVal = iot_timer_stop( xTimerHandle );

        if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
        {
            /* Wait for the Delay.  Make sure callback is not called. */
            lRetVal = xSemaphoreTake( xtestIotTimerSemaphore, testIotTIMER_CALLBACK_DELAY_US );
            TEST_ASSERT_NOT_EQUAL( pdTRUE, lRetVal );

            /* Start the timer again*/
            lRetVal = iot_timer_start( xTimerHandle );
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

            /* Wait for the Delay.  Make sure callback is called. */
            lRetVal = xSemaphoreTake( xtestIotTimerSemaphore, testIotTIMER_CALLBACK_DELAY_US );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }
    }

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test timer cancel
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_Cancel )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Set up the callback */
        iot_timer_set_callback( xTimerHandle,
                                prvTimerCallback,
                                NULL );

        /* Set up the delay */
        lRetVal = iot_timer_delay( xTimerHandle,
                                   testIotTIMER_DEFAULT_DELAY_US );

        /* Expect either "Not running" or "Success" */
        if( lRetVal != IOT_TIMER_NOT_RUNNING )
        {
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
        }

        /* Start the timer */
        lRetVal = iot_timer_start( xTimerHandle );
        TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

        /* Cancel the timer */
        lRetVal = iot_timer_cancel( xTimerHandle );

        if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
        {
            printf( "lRetVal = %d\n", lRetVal );
            TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

            /* Wait for the Delay.  Make sure callback is not called. */
            lRetVal = xSemaphoreTake( xtestIotTimerSemaphore, testIotTIMER_CALLBACK_DELAY_US );
            TEST_ASSERT_NOT_EQUAL( pdTRUE, lRetVal );
        }
    }

    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of open call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_OpenFuzzing )
{
    IotTimerHandle_t xTimerHandle;

    /* Open timer to initialize hardware with invalid instance. */
    xTimerHandle = iot_timer_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xTimerHandle );
}

/**
 * @brief Test Function to test fuzzing of close call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_CloseFuzzing )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;

    /* Close timer with NULL handle */
    lRetVal = iot_timer_close( NULL );
    TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    /* Close once with valid handle */
    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );

    /* Close second time with stale handle */
    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of start call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_StartFuzzing )
{
    int32_t lRetVal;

    /* Call Start Timer with invalid handle */
    lRetVal = iot_timer_start( NULL );
    TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of get call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_GetFuzzing )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;
    uint64_t ulMicroSeconds1 = 0;

    /* Get the time in micro seconds, with invalid Handle*/
    lRetVal = iot_timer_get_value( NULL, &ulMicroSeconds1 );
    TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );

    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    if( TEST_PROTECT() )
    {
        /* Get the time in micro seconds, with valid handle but NULL buffer */
        lRetVal = iot_timer_get_value( xTimerHandle, NULL );
        TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );
    }

    /* Close once with valid handle */
    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of stop call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_StopFuzzing )
{
    int32_t lRetVal;

    /* Call Stop Timer with invalid handle */
    lRetVal = iot_timer_stop( NULL );

    if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );
    }
}

/**
 * @brief Test Function to test fuzzing of set_callback call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_CallbackFuzzing )
{
    IotTimerHandle_t xTimerHandle;
    int32_t lRetVal;

    /* Call set_callback with NULLL handle */
    iot_timer_set_callback( NULL, prvTimerCallback, NULL );


    /* Open timer to initialize hardware. */
    xTimerHandle = iot_timer_open( ltestIotTimerInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTimerHandle );

    /* Call set_callback with NULLL handle */
    iot_timer_set_callback( xTimerHandle, NULL, NULL );

    /* Close once with valid handle */
    lRetVal = iot_timer_close( xTimerHandle );
    TEST_ASSERT_EQUAL( IOT_TIMER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of cancel call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_CancelFuzzing )
{
    int32_t lRetVal;

    /* Call Cancel Timer with invalid handle */
    lRetVal = iot_timer_cancel( NULL );

    if( lRetVal != IOT_TIMER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );
    }
}

/**
 * @brief Test Function to test fuzzing of delay call
 */
TEST( TEST_IOT_TIMER, AFWP_IotTimer_DelayFuzzing )
{
    int32_t lRetVal;

    /* Call Cancel Timer with invalid handle */
    lRetVal = iot_timer_delay( NULL, testIotTIMER_DEFAULT_DELAY_US );
    TEST_ASSERT_EQUAL( IOT_TIMER_INVALID_VALUE, lRetVal );
}
