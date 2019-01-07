/*
 * Amazon FreeRTOS V1.x.x
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*******************************************************************************
 * @file aws_test_hal_perf_cntr.c
 * @brief Functional Unit Test - Performance Counter
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "aws_hal_perf_cntr.h"


TEST_GROUP( Full_HAL_PERF_CNTR );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_HAL_PERF_CNTR )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_HAL_PERF_CNTR )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_HAL_PERF_CNTR )
{
    RUN_TEST_CASE( Full_HAL_PERF_CNTR, perf_counter_happy_path );
}

/*-----------------------------------------------------------*/

TEST( Full_HAL_PERF_CNTR, perf_counter_happy_path )
{
    uint64_t ullCounterValue1 = 0;
    uint64_t ullCounterValue2 = 0;
    uint32_t ulCounterFreq = 0;

    aws_hal_perfcounter_open();
    ullCounterValue1 = aws_hal_perfcounter_get_value();
    ulCounterFreq = aws_hal_perfcounter_get_frequency_hz();
    ullCounterValue2 = aws_hal_perfcounter_get_value();
    aws_hal_perfcounter_close();

    TEST_ASSERT_MESSAGE( ullCounterValue2 > ullCounterValue1, "Performance counter value should grow upwards.");
    TEST_ASSERT_MESSAGE( ulCounterFreq > 0, "Performance counter value should be at least larger than 0.")
}
