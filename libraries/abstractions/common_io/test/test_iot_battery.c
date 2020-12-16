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
 * @file test_iot_battery.c
 * @brief Functional Unit Test - BATTERY
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_battery.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_BATTERY );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_BATTERY )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_BATTERY )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_BATTERY )
{
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryOpenClose );
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryVoltage );
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryLevel );
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryCapacity );
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryVoltageFuzz );
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryLevelFuzz );
    RUN_TEST_CASE( TEST_IOT_BATTERY, AFQP_IotBatteryCapacityFuzz );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_battery_open and iot_battery_close.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryOpenClose )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* Open battery to initialize hardware. */
    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test iot_battery_voltage.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryVoltage )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;
    uint16_t usVoltage;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* Get the battery voltage */
    lRetVal = iot_battery_voltage( xBatteryHandle, &usVoltage );

    if( lRetVal != IOT_BATTERY_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
        TEST_ASSERT_GREATER_THAN_UINT32( 0, usVoltage );
    }

    /* Close battery interface */
    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test iot_battery_level.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryLevel )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;
    uint8_t ucLevel;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* Get the battery level */
    lRetVal = iot_battery_chargeLevel( xBatteryHandle, &ucLevel );

    if( lRetVal != IOT_BATTERY_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
        TEST_ASSERT_GREATER_THAN_UINT32( 0, ucLevel );
    }

    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test iot_battery_capacity.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryCapacity )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;
    uint8_t ucCapacity;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* Get the battery capacity */
    lRetVal = iot_battery_capacity( xBatteryHandle, &ucCapacity );

    if( lRetVal != IOT_BATTERY_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
        TEST_ASSERT_GREATER_THAN_UINT32( 0, ucCapacity );
    }

    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing iot_battery_voltage.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryVoltageFuzz )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;
    uint16_t usVoltage;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* read voltage with NULL handle*/
    lRetVal = iot_battery_voltage( NULL, &usVoltage );
    TEST_ASSERT_EQUAL( IOT_BATTERY_INVALID_VALUE, lRetVal );

    /* read voltage with valid handle null buffer*/
    lRetVal = iot_battery_voltage( xBatteryHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_BATTERY_INVALID_VALUE, lRetVal );

    /* Close battery interface */
    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing iot_battery_level.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryLevelFuzz )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;
    uint8_t ucLevel;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* read charge level with NULL handle*/
    lRetVal = iot_battery_chargeLevel( NULL, &ucLevel );
    TEST_ASSERT_EQUAL( IOT_BATTERY_INVALID_VALUE, lRetVal );

    /* read charge level with valid handle, null buffer*/
    lRetVal = iot_battery_chargeLevel( xBatteryHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_BATTERY_INVALID_VALUE, lRetVal );

    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing iot_battery_capacity.
 *
 */
TEST( TEST_IOT_BATTERY, AFQP_IotBatteryCapacityFuzz )
{
    IotBatteryHandle_t xBatteryHandle;
    int32_t lRetVal;
    uint8_t ucCapacity;

    /* Open battery to initialize hardware. */
    xBatteryHandle = iot_battery_open( 0 );
    TEST_ASSERT_NOT_EQUAL( NULL, xBatteryHandle );

    /* read capacity with NULL handle*/
    lRetVal = iot_battery_capacity( NULL, &ucCapacity );
    TEST_ASSERT_EQUAL( IOT_BATTERY_INVALID_VALUE, lRetVal );

    /* read capacity with valid handle, null buffer*/
    lRetVal = iot_battery_capacity( xBatteryHandle, NULL );
    TEST_ASSERT_EQUAL( IOT_BATTERY_INVALID_VALUE, lRetVal );

    lRetVal = iot_battery_close( xBatteryHandle );
    TEST_ASSERT_EQUAL( IOT_BATTERY_SUCCESS, lRetVal );
}
