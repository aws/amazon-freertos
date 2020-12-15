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
 * @file iot_test_watchdog.c
 * @brief Functional Unit Test - Watchdog
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_watchdog.h"
#include "iot_reset.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*-----------------------------------------------------------*/

#define testIotWATCHDOG_DEFAULT_BARK_TIME          4U    /* 4 msec */
#define testIotWATCHDOG_DEFAULT_BITE_TIME          5000U /* 5 sec */

#define testIotWATCHDOG_DEFAULT_SEMAPHORE_DELAY    100000U
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint8_t ucTestIotWatchdogInstance = 0; /** Default Flash instance for testing */

/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotWatchdogBarkSemaphore = NULL;
static SemaphoreHandle_t xtestIotWatchdogBiteSemaphore = NULL;

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_WATCHDOG );



/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_WATCHDOG )
{
    xtestIotWatchdogBarkSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotWatchdogBarkSemaphore );
    xtestIotWatchdogBiteSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotWatchdogBiteSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_WATCHDOG )
{
    vSemaphoreDelete( xtestIotWatchdogBarkSemaphore );
    vSemaphoreDelete( xtestIotWatchdogBiteSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_WATCHDOG )
{
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenClose );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateBarkTimer );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateBiteInterrupt );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogStartNoTimerSet );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogRestartNoStart );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogSetBarkGreaterThanBiteSetWatchdogBarkFirst );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogSetBarkGreaterThanBiteSetWatchdogBiteFirst );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenOpenClose );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdogClose );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdogStartStop );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdog_ioctl );
    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenCloseClose );
    /* This test will reset the device */
    /* RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateWatchdogReset); */
    /* RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateResetReason); */
}

static void prvWdogCallback( void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;
    int32_t lWdogStatus = 0;
    int32_t lRetVal;

    /* Read out status */
    lRetVal = iot_watchdog_ioctl( ( IotWatchdogHandle_t ) pvUserContext,
                                  eGetWatchdogStatus,
                                  ( void * const ) &lWdogStatus );

    if( lWdogStatus == eWatchdogTimerBarkExpired )
    {
        xSemaphoreGiveFromISR( xtestIotWatchdogBarkSemaphore, &xHigherPriorityTaskWoken );
    }
    else
    {
        /* For bites, we should restart the timer when the callback is received.
         * This as some HW has the interrupt bound to serving the watchdog which
         * means the callback will keep fireing until the watchdog is served or
         * until the device resets. */
        /* Restart the watchdog timer. */
        lRetVal = iot_watchdog_restart( ( IotWatchdogHandle_t ) pvUserContext );

        xSemaphoreGiveFromISR( xtestIotWatchdogBiteSemaphore, &xHigherPriorityTaskWoken );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_watchdog_open and iot_watchdog_close.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenClose )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to setup bark and bite time.
 * Bite is configured as reset, and Bark will generate interrupt.
 * Validate that bark timer will generate interrupt and callback is called.
 * and watchdog can be restarted.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateBarkTimer )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBarkTime;
    int32_t lBiteTime;
    int32_t lWdogStatus = 0;

    /* Open watchdog to initialize watchdog hardware.*/
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Set the bark time to default value */
        lBarkTime = testIotWATCHDOG_DEFAULT_BARK_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBarkTime,
                                      ( void * const ) &lBarkTime );

        if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

            /* Set the bite time to default bite value.*/
            lBiteTime = testIotWATCHDOG_DEFAULT_BITE_TIME;
            lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                          eSetWatchdogBiteTime,
                                          ( void * const ) &lBiteTime );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

            /* Read back the bark and bite time */
            lBarkTime = 0;
            lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                          eGetWatchdogBarkTime,
                                          ( void * const ) &lBarkTime );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( testIotWATCHDOG_DEFAULT_BARK_TIME, lBarkTime );

            /* Get the bite time. */
            lBiteTime = 0;
            lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                          eGetWatchdogBiteTime,
                                          ( void * const ) &lBiteTime );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( testIotWATCHDOG_DEFAULT_BITE_TIME, lBiteTime );

            /* Set up the callback  */
            iot_watchdog_set_callback( xWdogHandle,
                                       prvWdogCallback,
                                       xWdogHandle );
            /* Start the watchdog timer. */
            lRetVal = iot_watchdog_start( xWdogHandle );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

            /* Check the watchdog timer status and ensure that ist running */
            lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                          eGetWatchdogStatus,
                                          ( void * const ) &lWdogStatus );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( eWatchdogTimerRunning, lWdogStatus );

            /*Wait for the watchdog timer bark time to expire. */
            lRetVal = xSemaphoreTake( xtestIotWatchdogBarkSemaphore, testIotWATCHDOG_DEFAULT_SEMAPHORE_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* Stop the watchdog timer. */
            lRetVal = iot_watchdog_stop( xWdogHandle );

            if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            }

            /*Check the watchdog timer status and ensure that it is stopped */
            lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                          eGetWatchdogStatus,
                                          ( void * const ) &lWdogStatus );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( eWatchdogTimerStopped, eWatchdogTimerBarkExpired & lWdogStatus );
        }
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to setup bite time, and validate
 * if bite time expire generates an interrupt and callback is called.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateBiteInterrupt )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBiteTime;
    int32_t lBiteConfig;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Set the bite time to default */
        lBiteTime = testIotWATCHDOG_DEFAULT_BITE_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /* Get the bite time. */
        lBiteTime = 0;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eGetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotWATCHDOG_DEFAULT_BITE_TIME, lBiteTime );

        /* Set up the callback */
        iot_watchdog_set_callback( xWdogHandle,
                                   prvWdogCallback,
                                   xWdogHandle );

        /* Configure bite as interrupt. */
        lBiteConfig = eWatchdogBiteTimerInterrupt;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteBehaviour,
                                      ( void * const ) &lBiteConfig );

        if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

            /* Start the watchdog timer. */
            lRetVal = iot_watchdog_start( xWdogHandle );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

            /* Wait for the watchdog timer bark time to expire. */
            lRetVal = xSemaphoreTake( xtestIotWatchdogBiteSemaphore, testIotWATCHDOG_DEFAULT_SEMAPHORE_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* Stop the watchdog timer. */
            lRetVal = iot_watchdog_stop( xWdogHandle );

            if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            }
        }
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to start the watchdog without setting the timer first.
 * verify that watchdog returns proper error.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogStartNoTimerSet )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Start the watchdog timer. */
        lRetVal = iot_watchdog_start( xWdogHandle );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_TIME_NOT_SET, lRetVal );
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to start the watchdog without setting the timer first.
 * verify that watchdog returns proper error.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogRestartNoStart )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Restart the watchdog timer. */
        lRetVal = iot_watchdog_restart( xWdogHandle );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_TIME_NOT_SET, lRetVal );
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to set the Bark time first,
 * and set Bark time greater than Bite and verify that it returns proper error code
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogSetBarkGreaterThanBiteSetWatchdogBarkFirst )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBarkTime;
    int32_t lBiteTime;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Set the bark time to default value. */
        lBarkTime = testIotWATCHDOG_DEFAULT_BARK_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBarkTime,
                                      ( void * const ) &lBarkTime );

        if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

            /* Set the bite time less than bark. */
            lBiteTime = lBarkTime - 1;
            lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                          eSetWatchdogBiteTime,
                                          ( void * const ) &lBiteTime );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );
        }
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to set the Bite time first,
 * and set Bark time greater than Bite and verify that it returns proper error code
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogSetBarkGreaterThanBiteSetWatchdogBiteFirst )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBarkTime;
    int32_t lBiteTime;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Set the bite time to default bite time. */
        lBiteTime = testIotWATCHDOG_DEFAULT_BITE_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /* Set the bark time to lBiteTime + 1 */
        lBarkTime = lBiteTime + 1;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBarkTime,
                                      ( void * const ) &lBarkTime );

        if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );
        }
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to setup bark and bite time.
 * Bite is configured as reset, and Bark will generate interrupt.
 * Validate that bark timer will generate interrupt and callback is called.
 * and watchdog can be restarted.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateWdogRestart )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBarkTime;
    int32_t lBiteTime;
    int32_t lWdogStatus;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Set the bark time to default default value. */
        lBarkTime = testIotWATCHDOG_DEFAULT_BARK_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBarkTime,
                                      ( void * const ) &lBarkTime );

        if( lRetVal == IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
        {
            lRetVal = iot_watchdog_close( xWdogHandle );
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
            return;
        }

        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /* Set the bite time to default bite value */
        lBiteTime = testIotWATCHDOG_DEFAULT_BITE_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /* Read back the bark and bite time */
        lBarkTime = 0;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eGetWatchdogBarkTime,
                                      ( void * const ) &lBarkTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotWATCHDOG_DEFAULT_BARK_TIME, lBarkTime );

        /* Get the bite time. */
        lBiteTime = 0;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eGetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotWATCHDOG_DEFAULT_BITE_TIME, lBiteTime );

        /* Set up the callback */
        iot_watchdog_set_callback( xWdogHandle,
                                   prvWdogCallback,
                                   xWdogHandle );

        /* Start the watchdog timer. */
        lRetVal = iot_watchdog_start( xWdogHandle );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /* Wait for the watchdog timer bark time to expire. */
        lRetVal = xSemaphoreTake( xtestIotWatchdogBarkSemaphore, testIotWATCHDOG_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Restart the watchdog timer. */
        lRetVal = iot_watchdog_restart( xWdogHandle );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_TIME_NOT_SET, lRetVal );

        /* Wait again for the watchdog timer bark time to expire. */
        lRetVal = xSemaphoreTake( xtestIotWatchdogBarkSemaphore, testIotWATCHDOG_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Check the watchdog timer status */
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eGetWatchdogStatus,
                                      ( void * const ) &lWdogStatus );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eWatchdogTimerBarkExpired, eWatchdogTimerBarkExpired & lWdogStatus );

        /* Stop the watchdog timer. */
        lRetVal = iot_watchdog_stop( xWdogHandle );

        if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
        }
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to setup bite time, and validate
 * if bite time expire resets the device.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateWatchdogReset )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBiteTime;
    int32_t lBiteConfig;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Set the bite time to default */
        lBiteTime = testIotWATCHDOG_DEFAULT_BITE_TIME;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /*Get the bite time. */
        lBiteTime = 0;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eGetWatchdogBiteTime,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotWATCHDOG_DEFAULT_BITE_TIME, lBiteTime );

        /* Configure bite as interrupt. */
        lBiteConfig = eWatchdogBiteTimerReset;
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteBehaviour,
                                      ( void * const ) &lBiteConfig );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        /* Start the watchdog timer. */
        lRetVal = iot_watchdog_start( xWdogHandle );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

        vTaskDelay( lBiteTime * 5 );
        /*Target shall reset */
        /*Execution shall not reach here. */
        TEST_ASSERT_EQUAL( 0, 1 );
    }
}


/*-----------------------------------------------------------*/

/**
 * @brief Test Function to validate the reset reason after the
 * watchdog resets the device and the device is up after reset.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateResetReason )
{
    IotResetReason_t * xResetReason = NULL;
    int32_t lRetVal;

    /* Get the reset-reason post watchdog reset. */
    lRetVal = iot_get_reset_reason( &xResetReason );
    TEST_ASSERT_EQUAL( IOT_RESET_SUCCESS, lRetVal );

    /* Ensure reset-reason is due to watchdog reset */
    TEST_ASSERT_EQUAL( eResetWatchdog, xResetReason );
}


TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenOpenClose )
{
    IotWatchdogHandle_t xWdogHandle;
    IotWatchdogHandle_t xWdogHandleOpen;
    int32_t lRetVal;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Attempt to Re-Open watchdog to initialize watchdog hardware. */
        xWdogHandleOpen = iot_watchdog_open( ucTestIotWatchdogInstance );
        TEST_ASSERT_EQUAL( NULL, xWdogHandleOpen );
    }

    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( NULL, lRetVal );
}

/**
 * @brief Test Function to fuzz iot_watchdog_close.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdogClose )
{
    int32_t lRetVal;

    /* Close the watchdog timer. */
    lRetVal = iot_watchdog_close( NULL );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );
}



/**
 * @brief Test Function to fuzz iot_watchdog_start, iot_watchdog_stop and iot_watchdog_restart.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdogStartStop )
{
    int32_t lRetVal;

    /* Open watchdog to initialize watchdog hardware. */
    lRetVal = iot_watchdog_start( NULL );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );

    lRetVal = iot_watchdog_stop( NULL );

    if( lRetVal != IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );
    }

/*
 *  lRetVal = iot_watchdog_restart(NULL);
 *  TEST_ASSERT_EQUAL(IOT_WATCHDOG_INVALID_VALUE, lRetVal);
 */
}

/**
 * @brief Test Function to fuzz iot_watchdog_ioctl.
 *
 */
TEST( TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdog_ioctl )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;
    int32_t lBiteTime;

    /* Invoke iot_watchdog_ioctl with NULL handle and valid buffer. */
    lBiteTime = 0;
    lRetVal = iot_watchdog_ioctl( NULL,
                                  eGetWatchdogBiteTime,
                                  ( void * const ) &lBiteTime );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );


    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    if( TEST_PROTECT() )
    {
        /* Invoke iot_watchdog_ioctl with NULL buffer and valid handle. */
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      eSetWatchdogBiteTime,
                                      NULL );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );


        /* Invoke iot_watchdog_ioctl with NULL buffer and valid handle and invalid enum. */
        lRetVal = iot_watchdog_ioctl( xWdogHandle,
                                      -1,
                                      ( void * const ) &lBiteTime );
        TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );
    }

    /* Close the watchdog timer. */
    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );
}

/* Test to ensure closing the stale handle is not permitted */

TEST( TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenCloseClose )
{
    IotWatchdogHandle_t xWdogHandle;
    int32_t lRetVal;

    /* Open watchdog to initialize watchdog hardware. */
    xWdogHandle = iot_watchdog_open( ucTestIotWatchdogInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xWdogHandle );

    /* Close the stale watchdog handle. */
    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_SUCCESS, lRetVal );

    /* Attempt to Close the stale watchdog handle. */
    lRetVal = iot_watchdog_close( xWdogHandle );
    TEST_ASSERT_EQUAL( IOT_WATCHDOG_INVALID_VALUE, lRetVal );
}
