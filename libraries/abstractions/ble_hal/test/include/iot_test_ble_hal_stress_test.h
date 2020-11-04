/*
 * FreeRTOS BLE HAL V5.1.0
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
 * @file iot_test_ble_hal_stress_test.h
 * @brief
 */

#ifndef _IOT_TEST_BLE_HAL_STRESS_TEST_H_
#define _IOT_TEST_BLE_HAL_STRESS_TEST_H_

#include "iot_test_ble_hal_common.h"


/*The stress test cases are related.
 * e.g. If only one stress test case needs to be run,
 * the parameters of outside cases should be set to 1 and inside and parallel ones should be set to 0.*/
#define STRESS_TIER_0                        0
#define STRESS_TIER_1                        1
#define STRESS_TIER_2                        0
#define STRESS_TIER_3                        0

#if STRESS_TIER_0
    #define STRESS_TEST_INIT_REPETITION      1
    #define STRESS_TEST_ENABLE_REPETITION    1
    #define STRESS_TEST_MODULE_REPETITION    1
#elif STRESS_TIER_1
    #define STRESS_TEST_INIT_REPETITION      1
    #define STRESS_TEST_ENABLE_REPETITION    1
    #define STRESS_TEST_MODULE_REPETITION    10
#elif STRESS_TIER_2
    #define STRESS_TEST_INIT_REPETITION      2
    #define STRESS_TEST_ENABLE_REPETITION    10
    #define STRESS_TEST_MODULE_REPETITION    100
#elif STRESS_TIER_3
    #define STRESS_TEST_INIT_REPETITION      3
    #define STRESS_TEST_ENABLE_REPETITION    100
    #define STRESS_TEST_MODULE_REPETITION    1000
#endif /* if STRESS_TIER_0 */

void prvRestartService( BTService_t * xRefSrvc );

#endif /* _IOT_TEST_BLE_HAL_STRESS_TEST_H_ */
