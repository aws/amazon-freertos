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
 * @file iot_test_reset.c
 * @brief Functional Unit Test - TIMER
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_reset.h"

#include "FreeRTOS.h"
#include "task.h"


/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_RESET );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_RESET )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_RESET )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_RESET )
{
    RUN_TEST_CASE( TEST_IOT_RESET, AFQP_IotResetShutdown );
    RUN_TEST_CASE( TEST_IOT_RESET, AFQP_IotResetRebootZero );
    RUN_TEST_CASE( TEST_IOT_RESET, AFQP_IotResetRebootNonZero );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_reset_shutdown
 *
 */
TEST( TEST_IOT_RESET, AFQP_IotResetShutdown )
{
    int32_t lRetVal;

    /* Shutdown the device.
     * Only return on error */
    lRetVal = iot_reset_shutdown();

    /* If shutdown is supported then
     * execution shall not reach here */
    if( lRetVal != IOT_RESET_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_NOT_EQUAL( NULL, NULL );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_reset_reboot with
 * Zero coldBootFlag
 *
 */
TEST( TEST_IOT_RESET, AFQP_IotResetRebootZero )
{
    uint8_t coldBootFlag = 0;

    /* Reboot the device.  No returning */
    iot_reset_reboot( coldBootFlag );

    /* Execution does not reach here */
    TEST_ASSERT_NOT_EQUAL( NULL, NULL );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_reset_reboot with
 * Non-Zero coldBootFlag
 *
 */
TEST( TEST_IOT_RESET, AFQP_IotResetRebootNonZero )
{
    uint8_t coldBootFlag = -1;

    /* Reboot the device.  No returning */
    iot_reset_reboot( coldBootFlag );

    /* Execution does not reach here */
    TEST_ASSERT_NOT_EQUAL( NULL, NULL );
}
