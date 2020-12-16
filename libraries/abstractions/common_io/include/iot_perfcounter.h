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
 * @file iot_perfcounter.h
 *
 * @breif HAL APIs for Performance Counter
 *
 *******************************************************************************
 */

/**
 * @file    iot_perfcounter.h
 * @brief   This file contains all the Performance counters HAL API definitions
 */

#ifndef _IOT_PERFCOUNTER_H
#define _IOT_PERFCOUNTER_H

/**
 * @defgroup iot_perfcounter PerfCounter HAL APIs
 * @{
 * @brief   The perf counter is generally used to measure the cycles (usually processor clock cycles )
 *          taken between 2 places in code. perfcounters can be implemented using cycle counters if
 *          the processor supports, or any generic timer that is granular enough to measure the time
 *          between 2 points in code.
 */

/**
 * @brief   iot_perfcounter_open is used to Initialize the performance counter.
 *
 */
void iot_perfcounter_open( void );

/**
 * @brief   iot_perfcounter_get_value is used to get the current performance counter
 *          value.
 *
 * @return  returns performance counter value as uint64 value.
 */
uint64_t iot_perfcounter_get_value( void );

/**
 * @brief   iot_perfcounter_get_frequency is used to get the current frequency
 *          performance counters are running at. This can be used to determine the
 *          time delta between two perfcounter values returned by valling iot_perfcounter_get_value()
 *
 * @return  returns the frequency of the performance counter as a uint32 value.
 *          This can be used to deterime the period between perfcounter
 *          increments.
 */
uint32_t iot_perfcounter_get_frequency( void );


/**
 * @brief   iot_perfcounter_close is used to de-initialize the perfcounter.
 *          It may reset the counter value in perfcounter.
 *
 */
void iot_perfcounter_close( void );

/**
 * @}
 */

#endif /* ifndef _IOT_PERFCOUNTER_H */
