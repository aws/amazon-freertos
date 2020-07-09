/*
 * FreeRTOS
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
 * @file   iot_gpio_config.h
 * @brief  Additional settings for GPIO, such as CommonIO-to-Board pin mapping.
 */

#ifndef _AWS_COMMON_IO_GPIO_CONFIG_H_
#define _AWS_COMMON_IO_GPIO_CONFIG_H_

#include "nrf_gpio.h"

/*
 * The pin map is an int array with IOT_COMMON_IO_GPIO_NUMBER_OF_PINS of entries,
 * where the ith entry will be the ith Common IO pin and will map board-specific pin
 * stored at map[i]
 *
 * Example for nrf52840-dk:
 *
 * #define IOT_COMMON_IO_GPIO_NUMBER_OF_PINS  2
 * #define IOT_COMMON_IO_GPIO_PIN_MAP \
 * {                                  \
 *    NRF_GPIO_PIN_MAP(1, 7),         \
 *    NRF_GPIO_PIN_MAP(1, 8)          \
 * }
 *
 */

#define IOT_COMMON_IO_GPIO_NUMBER_OF_PINS    0
#define IOT_COMMON_IO_GPIO_PIN_MAP           {}

#endif /* ifndef _AWS_COMMON_IO_GPIO_CONFIG_H_ */
