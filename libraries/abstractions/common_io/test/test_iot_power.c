/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file test_iot_power.c
 * @brief Functional Unit Test - Power
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <time.h>
#include <sys/time.h>

#include "iot_power.h"

/* Defines */
#define testIotPOWER_MAX_WAKEUP_SOURCES    32 /* Maximum wakeup sources 32 * 8 = 256*/
/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint32_t ultestIotPowerDelay = 100;            /* Default Delay used 100 msec*/

uint32_t ultestIotPowerSleepTimeTolerance = 3; /* Sleep time tolerence - should be 1 */
uint32_t ultestIotPowerPcWakeThreshold = 2;    /* Default threshold for PCWake Idle is 2ticks/ms */
uint32_t ultestIotPowerClkSrcOffThreshold = 8; /* Default threshold for ClkSrcOff Idle is 8ticks/ms */
uint32_t ultestIotPowerVddOffThreshold = 9;    /* Default threshold for VddOff Idle is 9ticks/ms */

uint32_t ultestIotPowerInterruptConfig1 = 0;
uint32_t ultestIotPowerInterruptConfig2 = 0;

uint8_t * puctestIotPowerWakeupSources = NULL;
uint16_t ustestIotPowerWakeupSourcesLength = 0;

/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static bool btestIotPowerIdleEnterState = false;
static bool btestIotPowerIdleExitState = false;
static SemaphoreHandle_t xtestIotPowerIdleEnterSemaphore = NULL;
static SemaphoreHandle_t xtestIotPowerIdleExitSemaphore = NULL;

static uint32_t ultestIotPowerSavedInterruptConfig1;
static uint32_t ultestIotPowerSavedInterruptConfig2;

static uint32_t ultestIotPowerSleepEnterTime;
static uint32_t ultestIotPowerSleepExitTime;

static IotPowerIdleMode_t xLastMode = eIdleNoneMode;

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_POWER );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_POWER )
{
    xtestIotPowerIdleEnterSemaphore = xSemaphoreCreateBinary();
    xtestIotPowerIdleExitSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotPowerIdleEnterSemaphore );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotPowerIdleExitSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_POWER )
{
    vSemaphoreDelete( xtestIotPowerIdleEnterSemaphore );
    vSemaphoreDelete( xtestIotPowerIdleExitSemaphore );
    xtestIotPowerIdleEnterSemaphore = NULL;
    xtestIotPowerIdleExitSemaphore = NULL;
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_POWER )
{
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_OpenClose );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeHigh );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeNormal );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeLow );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_EnterIdle );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_EnterIdleCancel );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_IoctlEnum );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_PCWakeThresholdTest );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_ClkSrcOffWakeThresholdTest );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_ResetModeFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_IoctlFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_GetModeFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_CloseFuzzing );

    /* This test may reset the device if Vdd is off
     * Run this test individually from test_iot_main
     * RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_VddOffWakeThresholdTest );
     */
}

/**
 * @brief  Callback function - called when entering and exiting idle state.
 */
static void prvIotPowerCallback( bool bIdleState,
                                 void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;
    IotPowerIdleMode_t xIdleModeEntered;
    int8_t lRetVal;

    if( bIdleState == true )
    {
        /* Take the timetick only when first time it enters the sleep */
        if( ultestIotPowerSleepEnterTime == 0 )
        {
            ultestIotPowerSleepEnterTime = xTaskGetTickCount();
        }

        btestIotPowerIdleEnterState = true;
        xSemaphoreGiveFromISR( xtestIotPowerIdleEnterSemaphore, &xHigherPriorityTaskWoken );
    }
    else
    {
        /* Check the last sleep mode entered */
        lRetVal = iot_power_ioctl( ( IotPowerHandle_t ) pvUserContext, eGetLastIdleMode, &xIdleModeEntered );

        if( ( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED ) && ( xIdleModeEntered > xLastMode ) )
        {
            xLastMode = xIdleModeEntered;
        }

        ultestIotPowerSleepExitTime = xTaskGetTickCount();
        btestIotPowerIdleExitState = true;
        xSemaphoreGiveFromISR( xtestIotPowerIdleExitSemaphore, &xHigherPriorityTaskWoken );
    }
}

/**
 * @brief  Callback function - to vote for a mode and cancel the entering to idle mode
 */
static void prvIotPowerCallbackCancelIdle( bool bIdleState,
                                           void * pvUserContext )
{
    int32_t lRetVal;
    static bool bIdlePreviouslyCanceled = false;
    BaseType_t xHigherPriorityTaskWoken;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;

    if( ( bIdleState == true ) && ( pvUserContext != NULL ) && ( bIdlePreviouslyCanceled == false ) )
    {
        btestIotPowerIdleEnterState = bIdleState;

        /* Set the power mode to normal performance to cancel entering to idle */
        lRetVal = iot_power_set_mode( ( IotPowerHandle_t ) pvUserContext, eNormalPowerPerfMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        lRetVal = iot_power_get_mode( ( IotPowerHandle_t ) pvUserContext, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eNormalPowerPerfMode, xPowerMode );

        bIdlePreviouslyCanceled = true;

        xSemaphoreGiveFromISR( xtestIotPowerIdleEnterSemaphore, &xHigherPriorityTaskWoken );
    }
    else if( bIdlePreviouslyCanceled == false )
    {
        /* Error condition. Wake up shall not happen when Idle is cancelled */
        TEST_ASSERT( 0 );
    }
}

/**
 * @brief  Function to disable all interrupts so that the target does not wakeup with these interrupts
 */
static void prvDisableInterrupts()
{
    #ifdef __ARM_ARCH /* Check for ARM target */
        ultestIotPowerSavedInterruptConfig1 = *( volatile uint32_t * ) 0xE000E100;
        ultestIotPowerSavedInterruptConfig2 = *( volatile uint32_t * ) 0xE000E104;

        /* Clear Enable registers */
        *( volatile uint32_t * ) 0xE000E180 = ~( ultestIotPowerInterruptConfig1 );
        *( volatile uint32_t * ) 0xE000E184 = ~( ultestIotPowerInterruptConfig2 );

        /* Clear all Pending interrupts */
        *( volatile uint32_t * ) 0xE000E280 = 0xFFFFFFFF;
        *( volatile uint32_t * ) 0xE000E284 = 0xFFFFFFFF;

        __asm volatile ( "dsb" ::: "memory" );
        __asm volatile ( "isb" );
    #endif /* ifdef __ARM_ARCH */
}

/**
 * @brief  Function to enable all interrupts which were disabled
 */
static void prvEnableInterrupts()
{
    #ifdef __ARM_ARCH /* Check for ARM target */
        /* Re-Enable the interrupts at nvic */
        *( volatile uint32_t * ) 0xE000E100 = ultestIotPowerSavedInterruptConfig1;
        *( volatile uint32_t * ) 0xE000E104 = ultestIotPowerSavedInterruptConfig2;
        __asm volatile ( "dsb" ::: "memory" );
        __asm volatile ( "isb" );
    #endif
}

/*-----------------------------------------------------------*/

/**
 * @brief   Test Function to test power open and close.
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_OpenClose )
{
    IotPowerHandle_t xPowerHandle;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test setting power modes to high,
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_SetModeHigh )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the power mode to high performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );
    }

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test setting power modes to normal
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_SetModeNormal )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the power mode to normal performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eNormalPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eNormalPowerPerfMode, xPowerMode );
    }

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test setting power modes to low
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_SetModeLow )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the power mode to normal performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eLowPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eLowPowerPerfMode, xPowerMode );
    }

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief   Test Function to test the Idle mode enter and exit callbacks.
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_EnterIdle )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the power mode to high performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );
    }

    /* Register for Idle enter/exit callback */
    iot_power_set_callback( xPowerHandle, prvIotPowerCallback, xPowerHandle );

    /* Reset the power mode */
    lRetVal = iot_power_reset_mode( xPowerHandle );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify unknown mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eUnknownPowerMode, xPowerMode );


        /* Suspend this task for some time (10 sec) for giving a chance to
         * RTOS to enter Idle and call the call-back to set the semaphore */
        vTaskDelay( ultestIotPowerDelay / portTICK_PERIOD_MS );

        /* Wait till the Idle mode is entered */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleEnterSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that idle mode is entering */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleEnterState );
        btestIotPowerIdleEnterState = false;

        /* Wait till the Idle mode is exited */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleExitSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that idle mode is exiting */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleExitState );
        btestIotPowerIdleExitState = false;
    }

    /* Set the power mode to normal performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eNormalPowerPerfMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

    /* Read the performance mode and verify normal mode */
    lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( eNormalPowerPerfMode, xPowerMode );

    /* Close Power handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief   Test Function to test the Idle mode enter and exit with cancel.
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_EnterIdleCancel )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the power mode to high performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );
    }
    else
    {
        /* Set the power mode to normal mode */
        lRetVal = iot_power_set_mode( xPowerHandle, eNormalPowerPerfMode );

        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify normal performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eNormalPowerPerfMode, xPowerMode );
    }

    /* Register for Idle enter/exit callback */
    iot_power_set_callback( xPowerHandle, prvIotPowerCallbackCancelIdle, xPowerHandle );

    /* Reset the power mode */
    lRetVal = iot_power_reset_mode( xPowerHandle );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify unknown mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eUnknownPowerMode, xPowerMode );


        /* Suspend this task for some time (10 sec) for giving a chance to
         * RTOS to enter Idle and call the call-back to set the semaphore */
        vTaskDelay( ultestIotPowerDelay / portTICK_PERIOD_MS );

        /* Wait till the Idle mode is entered */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleEnterSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that idle mode is entering */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleEnterState );
    }

    /* Close Power handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief   Test Function to test the Ioctl enum's
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_IoctlEnum )
{
    IotPowerHandle_t xPowerHandle;
    int32_t lRetVal;
    uint32_t ulMilliSeconds1, ulMilliSeconds2;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call iot_power_ioctl with eGetPCWakeThreshold/eSetPCWakeThreshold */
    lRetVal = iot_power_ioctl( xPowerHandle, eGetPCWakeThreshold, &ulMilliSeconds1 );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        ulMilliSeconds1 += 1;
        lRetVal = iot_power_ioctl( xPowerHandle, eSetPCWakeThreshold, &ulMilliSeconds1 );

        if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

            /* Get new value.  Make sure it incremented */
            lRetVal = iot_power_ioctl( xPowerHandle, eGetPCWakeThreshold, &ulMilliSeconds2 );
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( ulMilliSeconds1, ulMilliSeconds2 );

            /* put it back to original setting */
            ulMilliSeconds1 -= 1;
            lRetVal = iot_power_ioctl( xPowerHandle, eSetPCWakeThreshold, &ulMilliSeconds1 );
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        }
    }

    /* Call iot_power_ioctl with eGetClkSrcOffWakeThreshold/eSetClkSrcOffWakeThreshold */
    lRetVal = iot_power_ioctl( xPowerHandle, eGetClkSrcOffWakeThreshold, &ulMilliSeconds1 );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        ulMilliSeconds1 += 1;
        lRetVal = iot_power_ioctl( xPowerHandle, eSetClkSrcOffWakeThreshold, &ulMilliSeconds1 );

        if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

            /* Get new value.  Make sure it incremented */
            lRetVal = iot_power_ioctl( xPowerHandle, eGetClkSrcOffWakeThreshold, &ulMilliSeconds2 );
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( ulMilliSeconds1, ulMilliSeconds2 );

            /* put it back to original setting */
            ulMilliSeconds1 -= 1;
            lRetVal = iot_power_ioctl( xPowerHandle, eSetClkSrcOffWakeThreshold, &ulMilliSeconds1 );
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        }
    }

    /* Call iot_power_ioctl with eGetVddOfffWakeThreshold / eSetVddOfffWakeThreshold */
    lRetVal = iot_power_ioctl( xPowerHandle, eGetVddOfffWakeThreshold, &ulMilliSeconds1 );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        ulMilliSeconds1 += 1;
        lRetVal = iot_power_ioctl( xPowerHandle, eSetVddOfffWakeThreshold, &ulMilliSeconds1 );

        if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

            /* Get new value.  Make sure it incremented */
            lRetVal = iot_power_ioctl( xPowerHandle, eGetVddOfffWakeThreshold, &ulMilliSeconds2 );
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( ulMilliSeconds1, ulMilliSeconds2 );

            /* put it back to original setting */
            ulMilliSeconds1 -= 1;
            lRetVal = iot_power_ioctl( xPowerHandle, eSetVddOfffWakeThreshold, &ulMilliSeconds1 );
            TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        }
    }

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test setMode fuzzing
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_SetModeFuzzing )
{
    IotPowerHandle_t xPowerHandle;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the mode with invalid handle */
    lRetVal = iot_power_set_mode( NULL, eLowPowerPerfMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SET_FAILED, lRetVal );

    /* Set the mode with invalid setting */
    lRetVal = iot_power_set_mode( xPowerHandle, ( IotPowerMode_t ) -2 );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_INVALID_VALUE, lRetVal );
    }

    /* Set the correct mode first */
    lRetVal = iot_power_set_mode( xPowerHandle, eNormalPowerPerfMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

    /* Set the mode with invalid mode */
    lRetVal = iot_power_set_mode( xPowerHandle, eUnknownPowerMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_INVALID_VALUE, lRetVal );
    }

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test resetMode fuzzing
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_ResetModeFuzzing )
{
    IotPowerHandle_t xPowerHandle;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the mode with invalid handle */
    lRetVal = iot_power_reset_mode( NULL );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_INVALID_VALUE, lRetVal );
    }

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test setCallback fuzzing
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_SetCallbackFuzzing )
{
    IotPowerHandle_t xPowerHandle;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Set the mode with invalid handle */
    iot_power_set_callback( NULL, prvIotPowerCallback, xPowerHandle );

    /* Set the mode with invalid callback */
    iot_power_set_callback( xPowerHandle, NULL, NULL );

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test ioctl fuzzing
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_IoctlFuzzing )
{
    IotPowerHandle_t xPowerHandle;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call the ioctl with invalid handle */
    lRetVal = iot_power_ioctl( NULL, eSetVddOfffWakeThreshold, NULL );
    TEST_ASSERT_EQUAL( IOT_POWER_INVALID_VALUE, lRetVal );

    /* Call the mode with invalid handle */
    lRetVal = iot_power_ioctl( xPowerHandle, ( IotPowerIoctlRequest_t ) -1, NULL );
    TEST_ASSERT_EQUAL( IOT_POWER_INVALID_VALUE, lRetVal );

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test getMode fuzzing
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_GetModeFuzzing )
{
    IotPowerHandle_t xPowerHandle = NULL;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call get_mode  with invalid handle */
    lRetVal = iot_power_get_mode( NULL, &xPowerMode );
    TEST_ASSERT_EQUAL( IOT_POWER_GET_FAILED, lRetVal );

    /* Call get_mode  with invalid handle */
    lRetVal = iot_power_get_mode( xPowerHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_POWER_GET_FAILED, lRetVal );

    /* Close Power handle with a valid handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test close fuzzing
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_CloseFuzzing )
{
    IotPowerHandle_t xPowerHandle = NULL;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call get_mode  with invalid handle */
    lRetVal = iot_power_close( NULL );
    TEST_ASSERT_EQUAL( IOT_POWER_INVALID_VALUE, lRetVal );

    /* Close Power handle with a valid value */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

    /* Close Power handle again with a stale handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_NOT_INITIALIZED, lRetVal );
}

/**
 * @brief   Test Function to test the Idle mode enter and verify the sleep-time.
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_PCWakeThresholdTest )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    IotPowerWakeupSources_t xWakeupSources;
    IotPowerWakeupSources_t xGetWakeupSources;
    uint8_t ucGetWakeupSources[ testIotPOWER_MAX_WAKEUP_SOURCES ] = { 0 };
    uint32_t ulMilliSeconds;
    int32_t lRetVal;
    int8_t i;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call iot_power_ioctl with eSetPCWakeThreshold */
    ulMilliSeconds = ultestIotPowerPcWakeThreshold;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetPCWakeThreshold, &ulMilliSeconds );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Set the power mode to high performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );
    }

    /* Set the entry and exit times to 0 */
    ultestIotPowerSleepEnterTime = 0;
    ultestIotPowerSleepExitTime = 0;

    /* Register for Idle enter/exit callback */
    iot_power_set_callback( xPowerHandle, prvIotPowerCallback, xPowerHandle );

    /* Setup the wake-up sources */
    xWakeupSources.pucWakeupSources = puctestIotPowerWakeupSources;
    xWakeupSources.usLength = ustestIotPowerWakeupSourcesLength;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetWakeupSources, &xWakeupSources );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Get the wake-up sources */
    xGetWakeupSources.pucWakeupSources = ucGetWakeupSources;
    xGetWakeupSources.usLength = ustestIotPowerWakeupSourcesLength;

    lRetVal = iot_power_ioctl( xPowerHandle, eGetWakeupSources, &xGetWakeupSources );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        for( i = 0; i < ustestIotPowerWakeupSourcesLength; i++ )
        {
            if( ucGetWakeupSources[ i ] != puctestIotPowerWakeupSources[ i ] )
            {
                TEST_ASSERT( 0 );
            }
        }
    }

    /* Reset the last mode */
    xLastMode = eIdleNoneMode;
    /* Reset the power mode */
    lRetVal = iot_power_reset_mode( xPowerHandle );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify unknown mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eUnknownPowerMode, xPowerMode );

        /* Suspend this task for some time (10 sec) for giving a chance to
         * RTOS to enter Idle and call the call-back to set the semaphore */
        vTaskDelay( ultestIotPowerDelay / portTICK_PERIOD_MS );

        /* Wait till the Idle mode is entered */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleEnterSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that idle mode is entering */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleEnterState );
        btestIotPowerIdleEnterState = false;

        /* Wait till the Idle mode is exited */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleExitSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        if( xLastMode != eIdleNoneMode )
        {
            TEST_ASSERT_EQUAL( eIdlePCMode, xLastMode );
        }

        /* Make sure time spent in sleep */
        TEST_ASSERT_GREATER_THAN_UINT32( ultestIotPowerDelay - ultestIotPowerSleepTimeTolerance,
                                         ( ultestIotPowerSleepExitTime - ultestIotPowerSleepEnterTime ) );

        /* Verify that idle mode is exiting */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleExitState );
        btestIotPowerIdleExitState = false;
    }

    /* Set the power mode to normal performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eNormalPowerPerfMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

    /* Read the performance mode and verify normal mode */
    lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( eNormalPowerPerfMode, xPowerMode );

    /* Close Power handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief   Test Function to test the Idle mode enter and exit callbacks.
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_ClkSrcOffWakeThresholdTest )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    IotPowerWakeupSources_t xWakeupSources;
    IotPowerWakeupSources_t xGetWakeupSources;
    uint8_t ucGetWakeupSources[ testIotPOWER_MAX_WAKEUP_SOURCES ] = { 0 };
    uint32_t ulMilliSeconds;
    int32_t lRetVal;
    int8_t i;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call iot_power_ioctl with eSetPCWakeThreshold */
    ulMilliSeconds = ultestIotPowerPcWakeThreshold;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetPCWakeThreshold, &ulMilliSeconds );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Call iot_power_ioctl with eSetClkSrcOffWakeThreshold */
    ulMilliSeconds = ultestIotPowerClkSrcOffThreshold;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetClkSrcOffWakeThreshold, &ulMilliSeconds );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Set the power mode to high performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify high performance mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );
    }

    /* Set the entry and exit times to 0 */
    ultestIotPowerSleepEnterTime = 0;
    ultestIotPowerSleepExitTime = 0;

    /* Register for Idle enter/exit callback */
    iot_power_set_callback( xPowerHandle, prvIotPowerCallback, xPowerHandle );

    /* Setup the wake-up sources */
    xWakeupSources.pucWakeupSources = puctestIotPowerWakeupSources;
    xWakeupSources.usLength = ustestIotPowerWakeupSourcesLength;

    lRetVal = iot_power_ioctl( xPowerHandle, eSetWakeupSources, &xWakeupSources );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Get the wake-up sources */
    xGetWakeupSources.pucWakeupSources = ucGetWakeupSources;
    xGetWakeupSources.usLength = ustestIotPowerWakeupSourcesLength;

    lRetVal = iot_power_ioctl( xPowerHandle, eGetWakeupSources, &xGetWakeupSources );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        for( i = 0; i < ustestIotPowerWakeupSourcesLength; i++ )
        {
            if( ucGetWakeupSources[ i ] != puctestIotPowerWakeupSources[ i ] )
            {
                TEST_ASSERT( 0 );
            }
        }
    }

    /* Make sure IotPowerDelay is at-least 2 ticks */
    TEST_ASSERT_GREATER_THAN_UINT32( 2, ultestIotPowerDelay );

    /* Reset the last mode */
    xLastMode = eIdleNoneMode;
    /* Reset the power mode */
    lRetVal = iot_power_reset_mode( xPowerHandle );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify unknown mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eUnknownPowerMode, xPowerMode );

        /* Disable all the interrupts */
        prvDisableInterrupts();

        /* Suspend this task for some time (10 sec) for giving a chance to
         * RTOS to enter Idle and call the call-back to set the semaphore */
        vTaskDelay( ultestIotPowerDelay / portTICK_PERIOD_MS );

        /* Wait till the Idle mode is entered */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleEnterSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that idle mode is entering */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleEnterState );
        btestIotPowerIdleEnterState = false;

        /* Wait till the Idle mode is exited */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleExitSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        if( xLastMode != eIdleNoneMode )
        {
            TEST_ASSERT_EQUAL( eIdleClkSrcOffModeMode, xLastMode );
        }

        /* Make sure time spent in sleep */
        TEST_ASSERT_GREATER_THAN_UINT32( ultestIotPowerDelay - ultestIotPowerSleepTimeTolerance,
                                         ( ultestIotPowerSleepExitTime - ultestIotPowerSleepEnterTime ) );

        /* Re-Enable all the interrupts */
        prvEnableInterrupts();

        /* Verify that idle mode is exiting */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleExitState );
        btestIotPowerIdleExitState = false;
    }

    /* Set the power mode to normal performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eNormalPowerPerfMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

    /* Read the performance mode and verify normal mode */
    lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( eNormalPowerPerfMode, xPowerMode );

    /* Close Power handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}

/**
 * @brief   Test Function to test the Idle mode enter and exit callbacks.
 */
TEST( TEST_IOT_POWER, AFQP_IotPower_VddOffWakeThresholdTest )
{
    IotPowerHandle_t xPowerHandle;
    IotPowerMode_t xPowerMode = eUnknownPowerMode;
    uint32_t ulMilliSeconds;
    int32_t lRetVal;

    /* Open Power handle */
    xPowerHandle = iot_power_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xPowerHandle );

    /* Call iot_power_ioctl with eSetPCWakeThreshold */
    ulMilliSeconds = ultestIotPowerPcWakeThreshold;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetPCWakeThreshold, &ulMilliSeconds );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Call iot_power_ioctl with eSetClkSrcOffWakeThreshold */
    ulMilliSeconds = ultestIotPowerClkSrcOffThreshold;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetClkSrcOffWakeThreshold, &ulMilliSeconds );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Call iot_power_ioctl with  eSetVddOfffWakeThreshold */
    ulMilliSeconds = ultestIotPowerVddOffThreshold;
    lRetVal = iot_power_ioctl( xPowerHandle, eSetVddOfffWakeThreshold, &ulMilliSeconds );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Set the power mode to high performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    }

    /* Read the performance mode and verify high performance mode */
    lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );

    /* Register for Idle enter/exit callback */
    iot_power_set_callback( xPowerHandle, prvIotPowerCallback, xPowerHandle );

    /* Reset the power mode */
    lRetVal = iot_power_reset_mode( xPowerHandle );

    if( lRetVal != IOT_POWER_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

        /* Read the performance mode and verify unknown mode */
        lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
        TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eUnknownPowerMode, xPowerMode );

        /* Disable all the interrupts */
        prvDisableInterrupts();

        /* Suspend this task for some time (10 sec) for giving a chance to
         * RTOS to enter Idle and call the call-back to set the semaphore */
        vTaskDelay( ultestIotPowerDelay / portTICK_PERIOD_MS );

        /* Wait till the Idle mode is entered */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleEnterSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Verify that idle mode is entering */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleEnterState );
        btestIotPowerIdleEnterState = false;

        /* Wait till the Idle mode is exited */
        lRetVal = xSemaphoreTake( xtestIotPowerIdleExitSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Make sure time spent in sleep */
        TEST_ASSERT_GREATER_THAN_UINT32( ultestIotPowerDelay - ultestIotPowerSleepTimeTolerance,
                                         ( ultestIotPowerSleepExitTime - ultestIotPowerSleepEnterTime ) );

        /* Re-Enable all the interrupts */
        prvEnableInterrupts();

        /* Verify that idle mode is exiting */
        TEST_ASSERT_EQUAL( true, btestIotPowerIdleExitState );
        btestIotPowerIdleExitState = false;
    }

    /* Set the power mode to normal performance */
    lRetVal = iot_power_set_mode( xPowerHandle, eHighPowerPerfMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );

    /* Read the performance mode and verify normal mode */
    lRetVal = iot_power_get_mode( xPowerHandle, &xPowerMode );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( eHighPowerPerfMode, xPowerMode );

    /* Close Power handle */
    lRetVal = iot_power_close( xPowerHandle );
    TEST_ASSERT_EQUAL( IOT_POWER_SUCCESS, lRetVal );
}
