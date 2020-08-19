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

#ifndef _TEST_IOT_CONFIG_H_
#define _TEST_IOT_CONFIG_H_

/* GPIO includes */
#include "iot_gpio_config.h"

/* SPI includes */
#include "iot_spi.h"
#include "nrf_drv_spi.h"

/*------------------------FLASH-------------------------------*/
#define IOT_TEST_COMMON_IO_FLASH_SUPPORTED    0

/*------------------------GPIO-------------------------------*/
#define IOT_TEST_COMMON_IO_GPIO_SUPPORTED     0

/*------------------------UART-------------------------------*/
#define IOT_TEST_COMMON_IO_UART_SUPPORTED     0

/*------------------------I2C-------------------------------*/
#define IOT_TEST_COMMON_IO_I2C_SUPPORTED      0

/*------------------------SPI-------------------------------*/
#define IOT_TEST_COMMON_IO_SPI_SUPPORTED      0

/* Enables loop-back tests where a single SPI instance's MOSI --> MISO,
 * with no other connections. Without the looping connection, these tests
 * will fail.
 */
#ifndef IOT_TEST_COMMON_IO_SPI_LOOPBACK
    #define IOT_TEST_COMMON_IO_SPI_LOOPBACK    0
#endif

/* The number of supported SPI instances to be tested */
#define SPI_TEST_SET    1

static const uint8_t spiTestPort[ SPI_TEST_SET ] = { 0 };
static const uint32_t spiIotMode[ SPI_TEST_SET ] = { eSPIMode0 };
static const uint32_t spiIotSpitBitOrder[ SPI_TEST_SET ] = { eSPIMSBFirst };
static const uint32_t spiIotFrequency[ SPI_TEST_SET ] = { NRF_DRV_SPI_FREQ_125K };
static const uint32_t spiIotDummyValue[ SPI_TEST_SET ] = { 0 };

#endif /* ifndef _TEST_IOT_CONFIG_H_ */
