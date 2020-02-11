/*
 * FreeRTOS Common IO V0.1.1
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
 * IOT On-Target Unit Test
 * @File: test_iot_internal.h
 * @Brief: File for IOT HAL test board specific configuration setup
 ******************************************************************************/

#include "test_iot_config.h"

#pragma once

/**
 * Board specific UART config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_UART_CONFIG( int testSet );

/**
 * Board specific Flash config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_FLASH_CONFIG( int testSet );

/**
 * Board specific Watchdog config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_WATCHDOG_CONFIG( int testSet );

/**
 * Board specific RTC config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_RTC_CONFIG( int testSet );

/**
 * Board specific GPIO config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_GPIO_CONFIG( int testSet );

/**
 * Board specific Timer config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_TIMER_CONFIG( int testSet );

/**
 * Board specific ADC config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_ADC_CONFIG( int testSet );

/**
 * Board specific Reset config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_RESET_CONFIG( int testSet );

/**
 * Board specific Performance Counter config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_PERFCOUNTER_CONFIG( int testSet );

/**
 * Board specific PWM config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_PWM_CONFIG( int testSet );

/**
 * Board specific I2C config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_I2C_CONFIG( int testSet );

/**
 * Board specific SPI config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_SPI_CONFIG( int testSet );

/**
 * Board specific Power config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_POWER_CONFIG( int testSet );

/**
 * Board specific SDIO config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_SDIO_CONFIG( int testSet );

/**
 * Board specific Temp Sensor config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_TEMP_SENSOR_CONFIG( int testSet );

/**
 * Board specific I2S config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_I2S_CONFIG( int testSet );
