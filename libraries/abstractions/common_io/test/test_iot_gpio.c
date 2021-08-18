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
 * @file test_iot_gpio.c
 * @brief Functional Unit Test - GPIO
 *******************************************************************************
 */


/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* GPIO driver include */
#include "iot_gpio.h"
#include "iot_perfcounter.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"

/*-----------------------------------------------------------*/
#define testIotGPIO_TEST_PORT_MASK     ( 0x00FF )
#define testIotGPIO_TEST_DIR_BIT       ( 8 )
#define testIotGPIO_TEST_DIR_MASK      ( 1 << testIotGPIO_TEST_DIR_BIT )
#define testIotGPIO_TEST_IRQ_BIT       ( 9 )
#define testIotGPIO_TEST_IRQ_MASK      ( 1 << testIotGPIO_TEST_IRQ_BIT )
#define testIotGPIO_TEST_VALUE_BIT     ( 10 )
#define testIotGPIO_TEST_VALUE_MASK    ( 1 << testIotGPIO_TEST_VALUE_BIT )
#define testIotGPIO_TEST_READ_DELAY    ( 50 )
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/

int32_t ltestIotGpioPortA = 0;
int32_t ltestIotGpioPortB = 1;
uint32_t ultestIotGpioFunction = 0;

int32_t ltestIotGpioPortInvalid = INT_MAX;

/*
 * ustestIotGpioConfig is used for configuring GPIO test
 * user must set this variable for testing different GPIOs
 *
 * default ustestIotGpioConfig = 0 means reading logic GPIO# 0
 *
 * lower 8 bit for port
 * bit 8 for direction
 * bit 9 for IRQ
 * bit 10 for write value 0 or 1
 */
uint16_t ustestIotGpioConfig = 0;
uint32_t ultestIotGpioWaitTime = 500; /* 0.5s */

uint32_t ultestIotGpioSlowSpeed = 0;
uint32_t ultestIotGpioFastSpeed = 1;
/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotGpioSemaphore = NULL;
static IotGpioHandle_t xtestIotGpioHandleA, xtestIotGpioHandleB;

/* Define Test Group. */
TEST_GROUP( TEST_IOT_GPIO );
/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_GPIO )
{
    xtestIotGpioSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_GPIO )
{
    /* Make sure resources are freed up */
    iot_gpio_close( xtestIotGpioHandleA );
    iot_gpio_close( xtestIotGpioHandleB );

    vSemaphoreDelete( xtestIotGpioSemaphore );
    xtestIotGpioSemaphore = NULL;
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_GPIO )
{
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenClose );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenOpenClose );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenCloseClose );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioIoctlSetGet );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioMode );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioPull );
    /* We don't have a CommonIO perfcounter to use....yet!*/
    /*RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioSpeed ); */
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOperation );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenCloseFuzz );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioReadWriteSyncFuzz );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioIoctlFuzz );
    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioInterrupt );
}

/**
 * @brief Function to test eGpioMode options.
 */
static void testIotGpioOutputMode( IotGpioOutputMode_t eGpioOutputMode,
                                   uint8_t * pxValue )
{
    int32_t lRetVal;
    uint8_t ucValueA, ucValueB;

    IotGpioDirection_t xGpioDirectionA = eGpioDirectionInput;
    IotGpioPull_t xGpioPullA = eGpioPullNone;

    /* When testing open-drain, use pull-up on port A (as the output can not driven high) */
    if( eGpioOutputMode == eGpioOpenDrain )
    {
        xGpioPullA = eGpioPullUp;
    }

    IotGpioDirection_t xGpioDirectionB = eGpioDirectionOutput;
    IotGpioPull_t xGpioPullB = eGpioPullNone;
    IotGpioOutputMode_t xGpioOutputModeB = eGpioOutputMode;

    /* Setup GPIO port A */
    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirectionA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &xGpioPullA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );


        /* Setup GPIO port B with different mode for port A to read */
        ucValueB = *pxValue;
        xtestIotGpioHandleB = iot_gpio_open( ltestIotGpioPortB );
        TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleB );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioDirection, &xGpioDirectionB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioPull, &xGpioPullB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioOutputMode, &xGpioOutputModeB );

        if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        lRetVal = iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        vTaskDelay( pdMS_TO_TICKS( testIotGPIO_TEST_READ_DELAY ) );

        lRetVal = iot_gpio_read_sync( xtestIotGpioHandleA, &ucValueA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( ucValueB, ucValueA );
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleB );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}

/**
 * @brief Function to test gpio_open --> gpio_ioctl(eGpioDirectionInput, eGpioPushPull, eGpioOpenDrain)
 *        --> gpio_read_sync/gpio_write_sync --> gpio_close
 */

TEST( TEST_IOT_GPIO, AFQP_IotGpioMode )
{
    uint8_t ucValue;

    ucValue = 0;
    testIotGpioOutputMode( eGpioPushPull, &ucValue );
    testIotGpioOutputMode( eGpioOpenDrain, &ucValue );

    ucValue = 1;
    testIotGpioOutputMode( eGpioPushPull, &ucValue );
    testIotGpioOutputMode( eGpioOpenDrain, &ucValue );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to test eGpioPull options.
 */
static void testIotGpioPull( IotGpioPull_t eGpioPull,
                             uint8_t * pxValue )
{
    int32_t lRetVal;
    uint8_t ucValueA, ucValueB;

    IotGpioDirection_t xGpioDirectionA = eGpioDirectionInput;
    IotGpioPull_t xGpioPullA = eGpioPullNone;

    IotGpioDirection_t xGpioDirectionB = eGpioDirectionOutput;
    IotGpioOutputMode_t xGpioOutputModeB = eGpioPushPull;
    IotGpioPull_t xGpioPullB = eGpioPull;

    /* Setup GPIO port A */
    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirectionA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &xGpioPullA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        /* Setup GPIO port B with different mode for port A to read */
        ucValueB = *pxValue;
        xtestIotGpioHandleB = iot_gpio_open( ltestIotGpioPortB );
        TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleB );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioDirection, &xGpioDirectionB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioOutputMode, &xGpioOutputModeB );

        if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioPull, &xGpioPullB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        if( pxValue != NULL )
        {
            lRetVal = iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        vTaskDelay( pdMS_TO_TICKS( testIotGPIO_TEST_READ_DELAY ) );

        lRetVal = iot_gpio_read_sync( xtestIotGpioHandleA, &ucValueA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        if( pxValue != NULL )
        {
            /* port B is explicitly driven, expect port A reading to match port B */
            TEST_ASSERT_EQUAL( ucValueB, ucValueA );
        }
        else
        {
            /* expect different port A reading depending on port B pull mode */
            if( eGpioPull == eGpioPullUp )
            {
                TEST_ASSERT_EQUAL( 1, ucValueA );
            }
            else if( eGpioPull == eGpioPullDown )
            {
                TEST_ASSERT_EQUAL( 0, ucValueA );
            }
            else
            {
                /* port A reading is undetermined */
                /* TBD */
            }
        }
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

    lRetVal = iot_gpio_close( xtestIotGpioHandleB );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}

/**
 * @brief Function to test gpio_open --> gpio_ioctl(eGpioPullNone, eGpioPullUp, eGpioPullDown)
 *        --> gpio_read_sync/gpio_write_sync --> gpio_close
 */

TEST( TEST_IOT_GPIO, AFQP_IotGpioPull )
{
    uint8_t ucValue;

    /* port pullup enabled, not explicitly driven */
    /*testIotGpioPull( eGpioPullUp, NULL ); */
    /*testIotGpioClosePorts(); */

    /* port pullup enabled, drive low */
    ucValue = 0;
    testIotGpioPull( eGpioPullUp, &ucValue );

    /* port pullup enabled, drive high */
    ucValue = 1;
    testIotGpioPull( eGpioPullUp, &ucValue );

    /* port pulldown enabled, not explicitly driven */
    /*testIotGpioPull( eGpioPullDown, NULL ); */
    /*testIotGpioClosePorts(); */

    /* port pulldown enabled, drive low */
    ucValue = 0;
    testIotGpioPull( eGpioPullDown, &ucValue );

    /* port pulldown enabled, drive high */
    ucValue = 1;
    testIotGpioPull( eGpioPullDown, &ucValue );

    ucValue = 0;
    testIotGpioPull( eGpioPullNone, &ucValue );

    ucValue = 1;
    testIotGpioPull( eGpioPullNone, &ucValue );
}

/*-----------------------------------------------------------*/

TEST( TEST_IOT_GPIO, AFQP_IotGpioSpeed )
{
    int32_t lRetVal;
    uint8_t ucValueA;
    uint32_t ulPerfCount0, ulPerfCount1, ulPerfCountSlowDelta, ulPerfCountFastDelta;

    IotGpioDirection_t xGpioDirectionA = eGpioDirectionInput;
    IotGpioPull_t xGpioPullA = eGpioPullNone;
    int32_t lSpeed = ultestIotGpioSlowSpeed;

    /* Setup GPIO port A */
    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioSpeed, &lSpeed );

        if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirectionA );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &xGpioPullA );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            iot_perfcounter_open();
            ulPerfCount0 = iot_perfcounter_get_value();
            lRetVal = iot_gpio_read_sync( xtestIotGpioHandleA, &ucValueA );
            ulPerfCount1 = iot_perfcounter_get_value();

            if( ulPerfCount1 > ulPerfCount0 )
            {
                ulPerfCountSlowDelta = ulPerfCount1 - ulPerfCount0;
            }
            else
            {
                ulPerfCountSlowDelta = 0xFFFFFFFF - ulPerfCount0 + ulPerfCount1 + 1;
            }

            lSpeed = ultestIotGpioFastSpeed;
            lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioSpeed, &lSpeed );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            ulPerfCount0 = iot_perfcounter_get_value();
            lRetVal = iot_gpio_read_sync( xtestIotGpioHandleA, &ucValueA );
            ulPerfCount1 = iot_perfcounter_get_value();

            if( ulPerfCount1 > ulPerfCount0 )
            {
                ulPerfCountFastDelta = ulPerfCount1 - ulPerfCount0;
            }
            else
            {
                ulPerfCountFastDelta = 0xFFFFFFFF - ulPerfCount0 + ulPerfCount1 + 1;
            }

            /* The count is a reference to how long the read takes
             *  so when running faster the count should be smaller. */
            TEST_ASSERT_GREATER_THAN( ulPerfCountFastDelta, ulPerfCountSlowDelta );
        }
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Function to test iot_gpio_open --> iot_gpio_close.
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioOpenClose )
{
    int32_t lRetVal;

    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

    /* Test close NULL handle */
    lRetVal = iot_gpio_close( NULL );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to test duplicate iot_gpio_open and single iot_gpio_close.
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioOpenOpenClose )
{
    int32_t lRetVal;

    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    if( TEST_PROTECT() )
    {
        xtestIotGpioHandleB = iot_gpio_open( ltestIotGpioPortA );
        TEST_ASSERT_EQUAL( NULL, xtestIotGpioHandleB );
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to test single iot_gpio_open and duplicate iot_gpio_close.
 */

TEST( TEST_IOT_GPIO, AFQP_IotGpioOpenCloseClose )
{
    int32_t lRetVal;

    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to test iot_gpio_ioctl.
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioIoctlSetGet )
{
    int32_t lRetVal;
    IotGpioDirection_t eGpioDirectionWrite = eGpioDirectionInput;
    IotGpioDirection_t eGpioDirectionRead;
    IotGpioPull_t eGpioPullWrite = eGpioPullNone;
    IotGpioPull_t eGpioPullRead;
    IotGpioInterrupt_t eGpioInterruptWrite = eGpioInterruptEdge;
    IotGpioInterrupt_t eGpioInterruptRead;
    uint32_t ulGpioFunctionWrite = ultestIotGpioFunction, ulGpioFunctionRead;

    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    if( TEST_PROTECT() )
    {
        /* Set the GPIO Direction */
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &eGpioDirectionWrite );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eGetGpioDirection, &eGpioDirectionRead );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        /* Set the GPIO Pull */
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &eGpioPullWrite );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eGetGpioPull, &eGpioPullRead );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        /* Set the GPIO Interrupt */
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioInterrupt, &eGpioInterruptWrite );

        if( IOT_GPIO_FUNCTION_NOT_SUPPORTED != lRetVal )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eGetGpioInterrupt, &eGpioInterruptRead );

        if( IOT_GPIO_FUNCTION_NOT_SUPPORTED != lRetVal )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( eGpioInterruptWrite, eGpioInterruptRead );
        }

        /* Set the GPIO Function */
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioFunction, &ulGpioFunctionWrite );

        if( IOT_GPIO_FUNCTION_NOT_SUPPORTED != lRetVal )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eGetGpioFunction, &ulGpioFunctionRead );

        if( IOT_GPIO_FUNCTION_NOT_SUPPORTED != lRetVal )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( ulGpioFunctionWrite, ulGpioFunctionRead );
        }

        TEST_ASSERT_EQUAL( eGpioDirectionWrite, eGpioDirectionRead );
        TEST_ASSERT_EQUAL( eGpioPullWrite, eGpioPullRead );
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/
static void prvGpioInterruptCallback( uint8_t ucPinState,
                                      void * pvUserContext )
{
    IotGpioInterrupt_t xGpioInterrupt = *( ( IotGpioInterrupt_t * ) pvUserContext );
    BaseType_t xHigherPriorityTaskWoken;
    uint8_t ucValueB;

    /* Need to remove trigger source if GPIO is configured as level trigger */
    if( xGpioInterrupt == eGpioInterruptLow )
    {
        ucValueB = 1;
        iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
    }
    else if( xGpioInterrupt == eGpioInterruptHigh )
    {
        ucValueB = 0;
        iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
    }

    xSemaphoreGiveFromISR( xtestIotGpioSemaphore, &xHigherPriorityTaskWoken );
}


static void testIotGpioInterrupt( IotGpioInterrupt_t eGpioInterrupt )
{
    int32_t lRetVal;
    uint8_t ucValueB;

    IotGpioDirection_t xGpioDirectionA = eGpioDirectionInput;
    IotGpioPull_t xGpioPullA = eGpioPullNone;
    IotGpioInterrupt_t xGpioInterruptA = eGpioInterrupt;

    IotGpioDirection_t xGpioDirectionB = eGpioDirectionOutput;
    IotGpioOutputMode_t xGpioOutputModeB = eGpioPushPull;
    IotGpioPull_t xGpioPullB = eGpioPullNone;

    if( eGpioInterrupt == eGpioInterruptNone )
    {
        TEST_ASSERT_FALSE_MESSAGE( eGpioInterrupt == eGpioInterruptNone, "Port interrupt has to be enabled for testing." );
        return;
    }

    /* set up port A for interrupt */
    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    iot_gpio_set_callback( xtestIotGpioHandleA, prvGpioInterruptCallback, &xGpioInterruptA );

    /* set up GPIO port B with different mode for port A to read */
    xtestIotGpioHandleB = iot_gpio_open( ltestIotGpioPortB );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleB );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioDirection, &xGpioDirectionB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioOutputMode, &xGpioOutputModeB );

        if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioPull, &xGpioPullB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        /* set port B output level so that it will not trigger interrupt immediately */
        if( ( eGpioInterrupt == eGpioInterruptRising ) ||
            ( eGpioInterrupt == eGpioInterruptHigh ) ||
            ( eGpioInterrupt == eGpioInterruptEdge ) )
        {
            /* Set port B to low */
            ucValueB = 0;
        }
        else
        {
            /* Set port B to high */
            ucValueB = 1;
        }

        lRetVal = iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirectionA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &xGpioPullA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioInterrupt, &xGpioInterruptA );

        if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            /* Toggle port B to trigger interrupt for port A */
            ucValueB ^= 1;
            lRetVal = iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            lRetVal = xSemaphoreTake( xtestIotGpioSemaphore, ultestIotGpioWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }
        else
        {
            /* Print out which sub-test was not supported */
            switch( eGpioInterrupt )
            {
                case eGpioInterruptRising:
                    /* TEST_MESSAGE( "Rising edge interrupts not supported." ); */
                    break;

                case eGpioInterruptFalling:
                    /* TEST_MESSAGE( "Falling edge interrupts not supported." ); */
                    break;

                case eGpioInterruptEdge:
                    /* TEST_MESSAGE( "Both edge interrupts not supported." ); */
                    break;

                case eGpioInterruptLow:
                    /* TEST_MESSAGE( "low-level interrupts not supported." ); */
                    break;

                case eGpioInterruptHigh:
                    /* TEST_MESSAGE( "high-level interrupts not supported." ); */
                    break;

                default:
                    /* TEST_MESSAGE( "Interrupt feature not supported." ); */
                    break;
            }
        }
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

    lRetVal = iot_gpio_close( xtestIotGpioHandleB );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}

TEST( TEST_IOT_GPIO, AFQP_IotGpioInterrupt )
{
    testIotGpioInterrupt( eGpioInterruptRising );
    testIotGpioInterrupt( eGpioInterruptFalling );
    testIotGpioInterrupt( eGpioInterruptEdge );
    testIotGpioInterrupt( eGpioInterruptLow );
    testIotGpioInterrupt( eGpioInterruptHigh );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function for testing iot_gpio_set_callback.
 */
static void prvGpioCallback( uint8_t ucPinState,
                             void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR( xtestIotGpioSemaphore, &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to test generic GPIO operations. Test is controlled through a global variable "ustestIotGpioConfig"
 *        see comment at top of this file on how ustestIotGpioConfig is used.
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioOperation )
{
    int32_t lRetVal, lRetValInt = IOT_GPIO_SUCCESS;
    uint8_t ucPort;
    uint8_t ucValue;

    IotGpioDirection_t xGpioDirection = eGpioDirectionInput;
    IotGpioPull_t xGpioPull = eGpioPullNone;
    IotGpioOutputMode_t xGpioOutputMode = eGpioOpenDrain;
    IotGpioInterrupt_t xGpioInterrupt = eGpioInterruptNone;

    /* Get the port to open from the test configuration */
    ucPort = ustestIotGpioConfig & testIotGPIO_TEST_PORT_MASK;

    xtestIotGpioHandleA = iot_gpio_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    iot_gpio_set_callback( xtestIotGpioHandleA, prvGpioCallback, &ucPort );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &xGpioPull );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        if( ustestIotGpioConfig & testIotGPIO_TEST_DIR_MASK )
        {
            xGpioOutputMode = eGpioPushPull;
            xGpioDirection = eGpioDirectionOutput;

            lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirection );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioOutputMode, &xGpioOutputMode );

            if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
            }
        }

        if( ustestIotGpioConfig & testIotGPIO_TEST_IRQ_MASK )
        {
            xGpioInterrupt = eGpioInterruptEdge;

            lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirection );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

            lRetValInt = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioInterrupt, &xGpioInterrupt );

            if( lRetValInt != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
            }
        }

        if( ustestIotGpioConfig & testIotGPIO_TEST_DIR_MASK )
        {
            /* write operation */
            ucValue = ( ustestIotGpioConfig & testIotGPIO_TEST_VALUE_MASK ) >> testIotGPIO_TEST_VALUE_BIT;

            configPRINTF( ( "\nGPIO[%d] write value = 0x%x\n", ucPort, ucValue ) );

            lRetVal = iot_gpio_write_sync( xtestIotGpioHandleA, ucValue );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }
        else
        {
            /* read operation */
            lRetVal = iot_gpio_read_sync( xtestIotGpioHandleA, &ucValue );
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
            configPRINTF( ( "\nGPIO[%d] read value = 0x%x\n", ucPort, ucValue ) );
        }

        if( ( ustestIotGpioConfig & testIotGPIO_TEST_IRQ_MASK ) &&
            ( lRetValInt != IOT_GPIO_FUNCTION_NOT_SUPPORTED ) )
        {
            configPRINTF( ( "press GPIO[%d] to trigger interrupt to continue...\n", ucPort ) );
            lRetVal = xSemaphoreTake( xtestIotGpioSemaphore, ultestIotGpioWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Function to test eGpioMode options.
 * This is a Assisted test
 */
static void testAssistedIotGpioOutputMode( IotGpioOutputMode_t eGpioOutputMode,
                                           uint8_t * pxValue )
{
    int32_t lRetVal;
    uint8_t ucValueB;

    IotGpioDirection_t xGpioDirectionB = eGpioDirectionOutput;
    IotGpioPull_t xGpioPullB = eGpioPullNone;
    IotGpioOutputMode_t xGpioOutputModeB = eGpioOutputMode;

    /* set up GPIO port B to output mode */
    ucValueB = *pxValue;
    xtestIotGpioHandleB = iot_gpio_open( ltestIotGpioPortB );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleB );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioDirection, &xGpioDirectionB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioPull, &xGpioPullB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleB, eSetGpioOutputMode, &xGpioOutputModeB );

        if( lRetVal != IOT_GPIO_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        }

        lRetVal = iot_gpio_write_sync( xtestIotGpioHandleB, ucValueB );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleB );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * This is a assisted test with RP3
 * Assisted test to write from GPIO Pins, and this is being verified form the RP3
 */

TEST( TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWritePushPullTrue )
{
    uint8_t ucValue;

    ucValue = 1;
    testAssistedIotGpioOutputMode( eGpioPushPull, &ucValue );
}
/*-----------------------------------------------------------*/

/**
 * This is a assisted test with RP3
 * Assisted test to write from GPIO Pins, and this is being verified form the RP3
 */

TEST( TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWritePushPullFalse )
{
    uint8_t ucValue;

    ucValue = 0;
    testAssistedIotGpioOutputMode( eGpioPushPull, &ucValue );
}
/*-----------------------------------------------------------*/

/**
 * This is a assisted test with RP3
 * Assisted test to write from GPIO Pins, and this is being verified form the RP3
 */

TEST( TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWriteOpenDrainTrue )
{
    uint8_t ucValue;

    ucValue = 1;
    testAssistedIotGpioOutputMode( eGpioOpenDrain, &ucValue );
}
/*-----------------------------------------------------------*/

/**
 * This is a assisted test with RP3
 * Assisted test to write from GPIO Pins, and this is being verified form the RP3
 */

TEST( TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWriteOpenDrainFalse )
{
    uint8_t ucValue;

    ucValue = 0;
    testAssistedIotGpioOutputMode( eGpioOpenDrain, &ucValue );
}
/*-----------------------------------------------------------*/

/**
 * @brief Function to test eGpioMode options.
 * This is a Assisted test to test the GPIO read modes.
 */
static void testAssistedIotGpioRead( uint8_t * pxValue )
{
    int32_t lRetVal;
    uint8_t ucValueA, ucValueB;

    ucValueB = *pxValue;

    IotGpioDirection_t xGpioDirectionA = eGpioDirectionInput;
    IotGpioPull_t xGpioPullA = eGpioPullNone;


    /* Setup GPIO port A to Input mode*/
    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioDirection, &xGpioDirectionA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_ioctl( xtestIotGpioHandleA, eSetGpioPull, &xGpioPullA );
        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );

        lRetVal = iot_gpio_read_sync( xtestIotGpioHandleA, &ucValueA );

        TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( ucValueB, ucValueA );
    }

    lRetVal = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * This is a assisted test with RP3
 * Assisted test to read from GPIO Pins, these pins are driven from RP3
 * This is a assisted test to read True on the GPIO pin
 */

TEST( TEST_IOT_GPIO, AFQP_AssistedIotGpioModeReadTrue )
{
    uint8_t ucValue;

    ucValue = 1;
    testAssistedIotGpioRead( &ucValue );
    testAssistedIotGpioRead( &ucValue );
}
/*-----------------------------------------------------------*/

/**
 * This is a assisted test to read False on the GPIO pin
 */
TEST( TEST_IOT_GPIO, AFQP_AssistedIotGpioModeReadFalse )
{
    uint8_t ucValue;

    ucValue = 0;
    testAssistedIotGpioRead( &ucValue );
    testAssistedIotGpioRead( &ucValue );
}
/*-----------------------------------------------------------*/

/* Below are Fuzzing tests */

/*-----------------------------------------------------------*/

/**
 * @brief Function to test iot_gpio_open --> iot_gpio_close.
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioOpenCloseFuzz )
{
    int32_t lRetValA, lRetValB;

    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortInvalid );
    TEST_ASSERT_EQUAL( NULL, xtestIotGpioHandleA );

    xtestIotGpioHandleA = iot_gpio_open( -ltestIotGpioPortInvalid );
    TEST_ASSERT_EQUAL( NULL, xtestIotGpioHandleA );

    /* Open GPIO port */
    xtestIotGpioHandleA = iot_gpio_open( ltestIotGpioPortA );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotGpioHandleA );

    lRetValA = iot_gpio_close( xtestIotGpioHandleA );
    TEST_ASSERT_EQUAL( IOT_GPIO_SUCCESS, lRetValA );

    /* Test close NULL handle */
    lRetValB = iot_gpio_close( NULL );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetValB );
}


/*-----------------------------------------------------------*/

/**
 * Test to fuzz iot_gpio_read_sync().
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioReadWriteSyncFuzz )
{
    int32_t lRetValA, lRetValB;
    uint8_t ucValue;

    lRetValA = iot_gpio_read_sync( NULL, &ucValue );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetValA );

    lRetValB = iot_gpio_write_sync( NULL, ucValue );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetValB );
}

/*-----------------------------------------------------------*/

/**
 * Test to fuzz iot_gpio_ioctl().
 */
TEST( TEST_IOT_GPIO, AFQP_IotGpioIoctlFuzz )
{
    int32_t lRetValA, lRetValB;
    uint8_t ucValue;
    uint8_t invalidEnum = 0xFF;


    lRetValA = iot_gpio_ioctl( NULL, eSetGpioPull, &ucValue );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetValA );

    lRetValB = iot_gpio_ioctl( NULL, invalidEnum, &ucValue );
    TEST_ASSERT_EQUAL( IOT_GPIO_INVALID_VALUE, lRetValB );
}
