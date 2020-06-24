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
 * @file   iot_spi_config.h
 * @brief  Additional settings for SPI instances
 */

#ifndef _AWS_COMMON_IO_SPI_CONFIG_H_
#define _AWS_COMMON_IO_SPI_CONFIG_H_

#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

#define IOT_COMMON_IO_SPI_1_MOSI_PIN    NRF_GPIO_PIN_MAP( 1, 13 )
#define IOT_COMMON_IO_SPI_1_MISO_PIN    NRF_GPIO_PIN_MAP( 1, 14 )
#define IOT_COMMON_IO_SPI_1_SCLK_PIN    NRF_GPIO_PIN_MAP( 1, 15 )

#define IOT_COMMON_IO_SPI_2_MOSI_PIN    NRF_DRV_SPI_PIN_NOT_USED
#define IOT_COMMON_IO_SPI_2_MISO_PIN    NRF_DRV_SPI_PIN_NOT_USED
#define IOT_COMMON_IO_SPI_2_SCLK_PIN    NRF_DRV_SPI_PIN_NOT_USED

#define IOT_COMMON_IO_SPI_3_MOSI_PIN    NRF_DRV_SPI_PIN_NOT_USED
#define IOT_COMMON_IO_SPI_3_MISO_PIN    NRF_DRV_SPI_PIN_NOT_USED
#define IOT_COMMON_IO_SPI_3_SCLK_PIN    NRF_DRV_SPI_PIN_NOT_USED

#endif /* ifndef _AWS_COMMON_IO_SPI_CONFIG_H_ */
