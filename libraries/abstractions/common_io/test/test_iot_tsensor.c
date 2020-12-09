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
 * @file test_iot_tsensor.c
 * @brief Functional Unit Test - Tsensor
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_tsensor.h"

#include "FreeRTOS.h"
#include "semphr.h"

#define testIotTSENSOR_DELAY              pdMS_TO_TICKS( 5000 )
#define testIotTSENSOR_ROOM_TEMP          ( 30000 )
#define testIotTSENSOR_ROOM_TEMP_DELTA    ( 15000 )
#define testIotTSENSOR_TEMP_DELTA         ( 5000 )
#define testIotTSENSOR_INVALID_INPUT      ( 99 )
#define TSENSOR_TEMP_ABSOLUTE_ZERO        ( -273150 )

uint8_t uctestIotTsensorInstance = 0;

static SemaphoreHandle_t xtestIotTsensorMinSemaphore = NULL;
static SemaphoreHandle_t xtestIotTsensorMaxSemaphore = NULL;

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_TSENSOR );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_TSENSOR )
{
    xtestIotTsensorMinSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotTsensorMinSemaphore );

    xtestIotTsensorMaxSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotTsensorMaxSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_TSENSOR )
{
    vSemaphoreDelete( xtestIotTsensorMinSemaphore );
    xtestIotTsensorMinSemaphore = NULL;
    vSemaphoreDelete( xtestIotTsensorMaxSemaphore );
    xtestIotTsensorMaxSemaphore = NULL;
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_TSENSOR )
{
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorOpenCloseSuccess );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorOpenCloseFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorMultipleOpenFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorMultipleCloseFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorEnableDisableSuccess );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorEnableDisableFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempSuccess );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempKTimesSuccess );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempInvalidHandleInputFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempDisabledFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorSetGetThresholdSuccess );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorSetGetThresholdInvalidHandleFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorSetGetThresholdNullInputFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorCalibrationSuccess );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorCalibrationInvalidHandleFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorCalibrationInvalidInputFuzz );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorTriggerMinThreshold );
    RUN_TEST_CASE( TEST_IOT_TSENSOR, AFQP_IotTsensorTriggerMaxThreshold );
}

/*-----------------------------------------------------------*/
static void prvTsensorCallback( IotTsensorStatus_t xStatus,
                                void * pvUserContext )
{
    if( eTsensorMinThresholdReached == xStatus )
    {
        xSemaphoreGiveFromISR( xtestIotTsensorMinSemaphore, NULL );
    }
    else
    {
        xSemaphoreGiveFromISR( xtestIotTsensorMaxSemaphore, NULL );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor open/close SUCCESS
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorOpenCloseSuccess )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor open/close Fail on invalid instance
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorOpenCloseFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance + testIotTSENSOR_INVALID_INPUT );
    TEST_ASSERT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor multiple open return NULL Fail
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorMultipleOpenFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    IotTsensorHandle_t xTsensorHandleNULL;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        xTsensorHandleNULL = iot_tsensor_open( uctestIotTsensorInstance );
        TEST_ASSERT_EQUAL( NULL, xTsensorHandleNULL );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor multiple close Fail
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorMultipleCloseFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_close( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor enable/disable SUCCESS
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorEnableDisableSuccess )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_enable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        lRetVal = iot_tsensor_disable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor enable/disable Fail on NULL handle and closed handle
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorEnableDisableFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance + testIotTSENSOR_INVALID_INPUT );
    TEST_ASSERT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_enable( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    lRetVal = iot_tsensor_disable( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

    lRetVal = iot_tsensor_enable( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    lRetVal = iot_tsensor_disable( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor get temp SUCCESS
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempSuccess )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t temp;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_enable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        temp = 0;
        lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
        TEST_ASSERT_INT_WITHIN( testIotTSENSOR_ROOM_TEMP_DELTA, testIotTSENSOR_ROOM_TEMP, temp );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor get k times temp SUCCESS
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempKTimesSuccess )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t temp;
    int32_t temp2;
    uint8_t k;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_enable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        temp = 0;
        lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
        TEST_ASSERT_INT_WITHIN( testIotTSENSOR_ROOM_TEMP_DELTA, testIotTSENSOR_ROOM_TEMP, temp );

        for( k = 5; k > 0; k-- )
        {
            temp2 = temp;
            lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
            TEST_ASSERT_INT_WITHIN( testIotTSENSOR_ROOM_TEMP_DELTA, testIotTSENSOR_ROOM_TEMP, temp );
            TEST_ASSERT_INT_WITHIN( testIotTSENSOR_TEMP_DELTA, temp2, temp );
        }
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor get temp Fail on NULL input, NULL handle and closed handle case
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempInvalidHandleInputFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t temp;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance + testIotTSENSOR_INVALID_INPUT );
    TEST_ASSERT_EQUAL( NULL, xTsensorHandle );

    temp = 0;
    lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_get_temp( xTsensorHandle, NULL );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

    lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor get temp Fail on tsensor disabled case
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorGetTempDisabledFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t temp;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_disable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        temp = 0;
        lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_DISABLED, lRetVal );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor set/get threshold SUCCESS
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorSetGetThresholdSuccess )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t setMinThreshold = -999000;
    int32_t setMaxThreshold = 999000;
    int32_t getMinThreshold = 0;
    int32_t getMaxThreshold = 0;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMinThreshold, &setMinThreshold );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
        }

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMaxThreshold, &setMaxThreshold );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
        }

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMinThreshold, &getMinThreshold );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( setMinThreshold, getMinThreshold );
        }

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMaxThreshold, &getMaxThreshold );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( setMaxThreshold, getMaxThreshold );
        }
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor set/get threshold Fail case on Null Handle and closed handle
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorSetGetThresholdInvalidHandleFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t setMinThreshold = -999000;
    int32_t setMaxThreshold = 999000;
    int32_t getMinThreshold = 0;
    int32_t getMaxThreshold = 0;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance + testIotTSENSOR_INVALID_INPUT );
    TEST_ASSERT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMinThreshold, &setMinThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMaxThreshold, &setMaxThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMinThreshold, &getMinThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMaxThreshold, &getMaxThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMinThreshold, &setMinThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_CLOSED, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMaxThreshold, &setMaxThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_CLOSED, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMinThreshold, &getMinThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_CLOSED, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMaxThreshold, &getMaxThreshold );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_CLOSED, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor set/get threshold Fail case on NULL input buffer
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorSetGetThresholdNullInputFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMinThreshold, NULL );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMaxThreshold, NULL );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMinThreshold, NULL );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorGetMaxThreshold, NULL );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor perform calibration SUCCESS
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorCalibrationSuccess )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    uint8_t param;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorPerformCalibration, &param );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
        }

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorPerformCalibration, NULL );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
        }
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor perform calibration Fail on NULL handle and closed handle case
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorCalibrationInvalidHandleFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    uint8_t param;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance + testIotTSENSOR_INVALID_INPUT );
    TEST_ASSERT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorPerformCalibration, &param );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_INVALID_VALUE, lRetVal );

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

    lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorPerformCalibration, &param );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_CLOSED, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor invalid ioctl input
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorCalibrationInvalidInputFuzz )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorPerformCalibration + testIotTSENSOR_INVALID_INPUT, 0 );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor trigger min threshold callback
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorTriggerMinThreshold )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t setLowThreshold = -999000;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_enable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        /* Read out current temperature */
        lRetVal = iot_tsensor_get_temp( xTsensorHandle, &setLowThreshold );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        /* Set min threshold to be 5 degrees C higher than current temperature.
         * Expect it to fire immediately */
        setLowThreshold += 5000;

        iot_tsensor_set_callback( xTsensorHandle, prvTsensorCallback, NULL );

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMinThreshold, &setLowThreshold );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

            lRetVal = xSemaphoreTake( xtestIotTsensorMinSemaphore, testIotTSENSOR_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test tsensor trigger max threshold callback
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorTriggerMaxThreshold )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t setHighThreshold = 999000;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_enable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        /* Read out current temperature */
        lRetVal = iot_tsensor_get_temp( xTsensorHandle, &setHighThreshold );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        /* Set max threshold to be 5 degrees C lower than current temperature.
         * Expect it to fire immediately */
        setHighThreshold -= 5000;

        iot_tsensor_set_callback( xTsensorHandle, prvTsensorCallback, NULL );

        lRetVal = iot_tsensor_ioctl( xTsensorHandle, eTsensorSetMaxThreshold, &setHighThreshold );

        if( lRetVal != IOT_TSENSOR_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

            lRetVal = xSemaphoreTake( xtestIotTsensorMaxSemaphore, testIotTSENSOR_DELAY );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
        }
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to print tsensor temp for assisted test.
 *
 */
TEST( TEST_IOT_TSENSOR, AFQP_IotTsensorTemp )
{
    IotTsensorHandle_t xTsensorHandle;
    int32_t lRetVal;
    int32_t temp;

    xTsensorHandle = iot_tsensor_open( uctestIotTsensorInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xTsensorHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_tsensor_enable( xTsensorHandle );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );

        lRetVal = iot_tsensor_get_temp( xTsensorHandle, &temp );
        TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
    }

    lRetVal = iot_tsensor_close( xTsensorHandle );
    TEST_ASSERT_EQUAL( IOT_TSENSOR_SUCCESS, lRetVal );
    /*Print temperature to console.*/
    TEST_ASSERT_LESS_THAN( TSENSOR_TEMP_ABSOLUTE_ZERO, temp );
}

/*-----------------------------------------------------------*/
