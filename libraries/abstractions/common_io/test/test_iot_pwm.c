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
 * @file iot_test_pwm.c
 * @brief Functional Unit Test - PWM
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "FreeRTOS.h"
#include "semphr.h"

#include "iot_pwm.h"
#include "iot_gpio.h"
#include "iot_test_common_io_internal.h"

/*-----------------------------------------------------------*/

#define PWM_TEST_DURATION_MSEC    5000

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint32_t ultestIotPwmGpioInputPin = 18;
uint32_t ulAssistedTestIotPwmGpioInputPin = 23;

uint32_t ultestIotPwmInstance = 2;            /* Use PWM instance 2 */
uint32_t ultestIotPwmFrequency = 2000UL;      /* 2KHz frequency */
uint32_t ultestIotPwmDutyCycle = 20;          /* Default duty cycle */
uint32_t ultestIotPwmChannel = 0;             /* Default PWM Channel */
uint32_t ulAssistedTestIotPwmInstance = 2;    /* Use PWM instance 2 for assisted test */
uint32_t ulAssistedTestIotPwmChannel = 0;     /* Default PWM Channel for assisted test */

volatile uint32_t ultestIotPwmIrqCounter = 0; /* Count how many pulses we've had */

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_PWM );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_PWM )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_PWM )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_PWM )
{
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_OpenClose );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_SetGetConfig );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_Start );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_Stop );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_NoConfig );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_OpenFuzzing );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_CloseFuzzing );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_SetConfigFuzzing );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_GetConfigFuzzing );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_StartFuzzing );
    RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwmAccuracy );
}


/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test pwm open and close
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_OpenClose )
{
    IotPwmHandle_t xPwmHandle;
    int32_t lRetVal;

    /* Open PWM handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test pwm Set/Get config
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_SetGetConfig )
{
    IotPwmHandle_t xPwmHandle;
    IotPwmConfig_t xSetPwmConfig;
    IotPwmConfig_t * xGetPwmConfig;
    int32_t lRetVal;

    /* Open PWM handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    /* Setup the pwm configuration */
    xSetPwmConfig.ulPwmFrequency = ultestIotPwmFrequency;
    xSetPwmConfig.ucPwmDutyCycle = ultestIotPwmDutyCycle;
    xSetPwmConfig.ucPwmChannel = ultestIotPwmChannel;

    if( TEST_PROTECT() )
    {
        /* Set the pwm configuration */
        lRetVal = iot_pwm_set_config( xPwmHandle, xSetPwmConfig );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

        /* Get the pwm configuration */
        xGetPwmConfig = iot_pwm_get_config( xPwmHandle );
        TEST_ASSERT_NOT_EQUAL( NULL, xGetPwmConfig );

        /* Confim Set and Get are the same */
        TEST_ASSERT_EQUAL( xSetPwmConfig.ulPwmFrequency, xGetPwmConfig->ulPwmFrequency );
        TEST_ASSERT_EQUAL( xSetPwmConfig.ucPwmDutyCycle, xGetPwmConfig->ucPwmDutyCycle );
        TEST_ASSERT_EQUAL( xSetPwmConfig.ucPwmChannel, xGetPwmConfig->ucPwmChannel );
    }

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test pwm start
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_Start )
{
    IotPwmHandle_t xPwmHandle;
    IotPwmConfig_t xSetPwmConfig;
    int32_t lRetVal;

    /* Open PWM handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    /* Setup the pwm configuration */
    xSetPwmConfig.ulPwmFrequency = ultestIotPwmFrequency;
    xSetPwmConfig.ucPwmDutyCycle = ultestIotPwmDutyCycle;
    xSetPwmConfig.ucPwmChannel = ultestIotPwmChannel;

    if( TEST_PROTECT() )
    {
        /* Set the pwm configuration */
        lRetVal = iot_pwm_set_config( xPwmHandle, xSetPwmConfig );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

        /* Start the PWM signal */
        lRetVal = iot_pwm_start( xPwmHandle );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
    }

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test pwm stop
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_Stop )
{
    IotPwmHandle_t xPwmHandle;
    IotPwmConfig_t xSetPwmConfig;
    int32_t lRetVal;

    /* Open PWM handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    /* Setup the pwm configuration */
    xSetPwmConfig.ulPwmFrequency = ultestIotPwmFrequency;
    xSetPwmConfig.ucPwmDutyCycle = ultestIotPwmDutyCycle;
    xSetPwmConfig.ucPwmChannel = ultestIotPwmChannel;

    if( TEST_PROTECT() )
    {
        /* Set the pwm configuration */
        lRetVal = iot_pwm_set_config( xPwmHandle, xSetPwmConfig );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

        /* Start the PWM signal */
        lRetVal = iot_pwm_start( xPwmHandle );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

        /* Stop the PWM signal */
        lRetVal = iot_pwm_stop( xPwmHandle );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
    }

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test pwm with no config
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_NoConfig )
{
    IotPwmHandle_t xPwmHandle;
    int32_t lRetVal;

    /* Open PWM handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    if( TEST_PROTECT() )
    {
        /* Start the PWM signal */
        lRetVal = iot_pwm_start( xPwmHandle );
        TEST_ASSERT_EQUAL( IOT_PWM_NOT_CONFIGURED, lRetVal );
    }

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test pwm open and open again to
 * validate the failure condition
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_OpenFuzzing )
{
    IotPwmHandle_t xPwmHandle1, xPwmHandle2;
    int32_t lRetVal;

    /* Open pwm with invalid instance */
    xPwmHandle1 = iot_pwm_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xPwmHandle1 );

    /* Open pwm handle */
    xPwmHandle1 = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle1 );

    if( TEST_PROTECT() )
    {
        /* Open pwm handle again. */
        xPwmHandle2 = iot_pwm_open( ultestIotPwmInstance );
        TEST_ASSERT_EQUAL( NULL, xPwmHandle2 );
    }

    /* Close the pwm handle */
    lRetVal = iot_pwm_close( xPwmHandle1 );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test pwm close with invalid handles
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_CloseFuzzing )
{
    IotPwmHandle_t xPwmHandle;
    int32_t lRetVal;

    /* Open PWM handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

    /* Close PWM handle with stale handle*/
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_INVALID_VALUE, lRetVal );

    /* Close PWM handle with NULL handle*/
    lRetVal = iot_pwm_close( NULL );
    TEST_ASSERT_EQUAL( IOT_PWM_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test pwm setConfig with invalid instance
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_SetConfigFuzzing )
{
    int32_t lRetVal;
    IotPwmConfig_t xSetPwmConfig;

    lRetVal = iot_pwm_set_config( NULL, xSetPwmConfig );
    TEST_ASSERT_EQUAL( IOT_PWM_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test pwm getConfig with invalid instance
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_GetConfigFuzzing )
{
    IotPwmConfig_t * xGetPwmConfig;

    xGetPwmConfig = iot_pwm_get_config( NULL );
    TEST_ASSERT_EQUAL( NULL, xGetPwmConfig );
}


/**
 * @brief Test Function to test pwm start with out setting
 * the PWM configuration.
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwm_StartFuzzing )
{
    IotPwmHandle_t xPwmHandle;
    int32_t lRetVal;

    /* Open pwm handle */
    xPwmHandle = iot_pwm_open( ultestIotPwmInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

    if( TEST_PROTECT() )
    {
        /* Start the PWM signal */
        lRetVal = iot_pwm_start( xPwmHandle );
        TEST_ASSERT_EQUAL( IOT_PWM_NOT_CONFIGURED, lRetVal );
    }

    /* Close the pwm handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/
static void prvPwmGpioInterruptCallback( uint8_t ucPinState,
                                         void * pvUserContext )
{
    ultestIotPwmIrqCounter++;
}
/*-----------------------------------------------------------*/

void prvTestPwmAccuracy( uint32_t ulGpioInstance,
                         uint32_t ulPwmInstance,
                         uint32_t ulPwmChannel )
{
    int32_t lRetVal;
    uint32_t expected_count = 0;
    IotPwmHandle_t xPwmHandle = NULL;
    IotPwmConfig_t xSetPwmConfig;

    IotGpioHandle_t xtestIotGpioHandleB = NULL;
    IotGpioDirection_t xGpioDirectionB = eGpioDirectionInput;
    IotGpioPull_t xGpioPullB = eGpioPullNone;
    IotGpioInterrupt_t xGpioInterruptB = eGpioInterruptRising;

    /* set up port B for interrupt */
    xtestIotGpioHandleB = iot_gpio_open( ulGpioInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleB );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioDirection, &xGpioDirectionB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioPull, &xGpioPullB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioInterrupt, &xGpioInterruptB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        iot_gpio_set_callback( xtestIotGpioHandleB, prvPwmGpioInterruptCallback, &xGpioInterruptB );

        /* Initialize our counter to 0 */
        ultestIotPwmIrqCounter = 0;

        /* Open PWM handle */
        xPwmHandle = iot_pwm_open( ulPwmInstance );
        TEST_ASSERT_NOT_EQUAL( NULL, xPwmHandle );

        /* Settup the pwm configuration */
        xSetPwmConfig.ulPwmFrequency = ultestIotPwmFrequency;
        xSetPwmConfig.ucPwmDutyCycle = ultestIotPwmDutyCycle;
        xSetPwmConfig.ucPwmChannel = ulPwmChannel;

        /* Set the pwm configuration */
        lRetVal = iot_pwm_set_config( xPwmHandle, xSetPwmConfig );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

        /* Start the PWM signal */
        lRetVal = iot_pwm_start( xPwmHandle );
        TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

        /* Delay for 5 sec. */
        vTaskDelay( PWM_TEST_DURATION_MSEC / portTICK_PERIOD_MS );
    }

    /* Close PWM handle */
    lRetVal = iot_pwm_close( xPwmHandle );
    TEST_ASSERT_EQUAL( IOT_PWM_SUCCESS, lRetVal );

    /* Close the GPIO handle */
    lRetVal = iot_gpio_close( xtestIotGpioHandleB );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

    expected_count = ultestIotPwmFrequency * ( PWM_TEST_DURATION_MSEC / 1000 );
    TEST_ASSERT_GREATER_THAN( ( expected_count - 100 ), ultestIotPwmIrqCounter );
    TEST_ASSERT_GREATER_THAN( ultestIotPwmIrqCounter, ( expected_count + 100 ) );
}

/**
 * @brief Test Function to test pwm accuracy by running the
 * pwm signal for 5 seconds and then counting the number of pulses
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwmAccuracy )
{
    prvTestPwmAccuracy( ultestIotPwmGpioInputPin, ultestIotPwmInstance, ultestIotPwmChannel );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assited Test Function to test pwm accuracy by running the
 * pwm signal for 5 seconds and also counting the number of pulses
 *
 */
TEST( TEST_IOT_PWM, AFQP_IotPwmAccuracyAssisted )
{
    prvTestPwmAccuracy( ulAssistedTestIotPwmGpioInputPin, ulAssistedTestIotPwmInstance, ulAssistedTestIotPwmChannel );
}
