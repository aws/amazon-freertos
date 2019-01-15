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

/**
* @file aws_hal_perf_cntr.h
* @brief File for the APIs of performance counter called by application layer.
*
* Performance counter uses a hardware peripheral timer to track time elapsed since
* the start of the counter. The interface implementation is MCU specific, and counter
* resolution is configurable via FreeRTOSConfig.h.
*
* To use the interfaces, application code needs to:
* - Initialize counter by calling aws_hal_perfcounter_open().
* - Get counter value for as many times as you want in whatever thread context
*   by calling aws_hal_perfcounter_get_value().
* - Time elapsed can be derived by ( counter value / counter frequency ),
*   where counter frequency can be acquired by calling aws_hal_perfcounter_get_frequency_hz().
* - Once completely done with performance measurement,
*   free resources by calling aws_hal_perfcounter_close().
*
* @warning It's not recommended to repurpose the hardware timer, nor have timer vector interrupt
*          priority lower than other peripheral vector interrupt priorities. Please refer to implementation
*          specific details.
*/

#ifndef _AWS_HAL_PERFCOUNTER_H_
#define _AWS_HAL_PERFCOUNTER_H_

/**
 * @brief Initialize a hardware timer which is to be used as performance counter.
 */
void aws_hal_perfcounter_open(void);

/**
 * @brief Deinitialize the hardware timer.
 */
void aws_hal_perfcounter_close( void);

/**
 * @brief Get current count from the performance counter.
 *
 * @return Total count since counter started.
 */
uint64_t aws_hal_perfcounter_get_value(void);

/**
 * @brief Get configured frequency of the performance counter.
 *
 * @return Frequency which the counter is configured to run at.
 */
uint32_t aws_hal_perfcounter_get_frequency_hz(void);

#endif /* _AWS_HAL_PERFCOUNTER_H_ */
