/*
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * AMAZON PROPRIETARY/CONFIDENTIAL
 *
 * You may not use this file except in compliance with the terms and
 * conditions set forth in the accompanying LICENSE.TXT file.
 *
 * THESE MATERIALS ARE PROVIDED ON AN "AS IS" BASIS. AMAZON SPECIFICALLY
 * DISCLAIMS, WITH RESPECT TO THESE MATERIALS, ALL WARRANTIES, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
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
 * @returnNone
 */
void SET_TEST_IOT_UART_CONFIG(int testSet);

/**
 * Board specific Flash config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_FLASH_CONFIG(int testSet);

/**
 * Board specific Watchdog config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_WATCHDOG_CONFIG(int testSet);

/**
 * Board specific RTC config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_RTC_CONFIG(int testSet);

/**
 * Board specific GPIO config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_GPIO_CONFIG(int testSet);

/**
 * Board specific Timer config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_TIMER_CONFIG(int testSet);

/**
 * Board specific ADC config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_ADC_CONFIG(int testSet);

/**
 * Board specific Reset config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_RESET_CONFIG(int testSet);

/**
 * Board specific Performance Counter config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_PERFCOUNTER_CONFIG(int testSet);

/**
 * Board specific PWM config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_PWM_CONFIG(int testSet);

/**
 * Board specific I2C config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_I2C_CONFIG(int testSet);

/**
 * Board specific SPI config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_SPI_CONFIG(int testSet);

/**
 * Board specific Power config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_POWER_CONFIG(int testSet);

/**
 * Board specific SDIO config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_SDIO_CONFIG(int testSet);

/**
 * Board specific Temp Sensor config set
 *
 * @param: testSet: number of config set to be test
 * @returnNone
 */
void SET_TEST_IOT_TEMP_SENSOR_CONFIG(int testSet);
