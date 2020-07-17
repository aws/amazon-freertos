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

/* I2C includes */
#include "iot_i2c.h"

/* UART includes */
#include "stm32l4xx_hal.h"

/* SPI includes */
#include "iot_spi.h"

#include "iot_sdio.h"

/*------------------------UART-------------------------------*/

/* UART is supported. */
#define IOT_TEST_COMMON_IO_UART_SUPPORTED                    1

/* How many UART ports are being test. */
#define UART_TEST_SET                                        1
#define IOT_TEST_COMMON_IO_UART_SUPPORTED_CANCEL             1

/* Instance ID 3 corresponds to UART4. */
const uint8_t uartTestPort[ UART_TEST_SET ] = { 3 };
/* Following configurations are not being used for now. */
const uint32_t uartIotUartFlowControl[ UART_TEST_SET ] = { UART_HWCONTROL_NONE };
const uint32_t uartIotUartParity[ UART_TEST_SET ] = { UART_PARITY_NONE };
const uint32_t uartIotUartWordLength[ UART_TEST_SET ] = { UART_WORDLENGTH_8B };
const uint32_t uartIotUartStopBits[ UART_TEST_SET ] = { UART_STOPBITS_1 };

/*------------------------I2C-------------------------------*/

/* I2C is supported. */
#define IOT_TEST_COMMON_IO_I2C_SUPPORTED                     1

#if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED == 1 )
    #define IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP    1

/* TODO: ST I2C driver does support cancel: HAL_I2C_Master_Abort_IT
 * However, after write_async or read_async, calling HAL_I2C_Master_Abort_IT immediately will leave the bus in a bad state,
 * so any operation afterwards will return timeout or error.*/
    #define IOT_TEST_COMMON_IO_I2C_SUPPORTED_CANCEL          0
#endif

#define I2C_TEST_SET                                         1

/* 0xD4 is the slave address for on-board sensor. */
const uint8_t i2cTestSlaveAddr[ I2C_TEST_SET ] = { 0xD4 };
const uint8_t i2cTestDeviceRegister[ I2C_TEST_SET ] = { 0x73 };
const uint8_t i2cTestWriteVal[ I2C_TEST_SET ] = { 0b01101010 };
const uint8_t i2cTestInstanceIdx[ I2C_TEST_SET ] = { 1 };
const uint8_t i2cTestInstanceNum[ I2C_TEST_SET ] = { 3 };

/* Not used by tests in this code base. */
IotI2CHandle_t gIotI2cHandle[ 4 ] = { NULL, NULL, NULL, NULL };

/*------------------------SPI-------------------------------*/
#define IOT_TEST_COMMON_IO_SPI_SUPPORTED                     1

#define SPI_TEST_SET                                         1

const uint8_t spiTestPort[ SPI_TEST_SET ] = { 1 };
const uint32_t spiIotMode[ SPI_TEST_SET ] = { eSPIMode0 };
const uint32_t spiIotSpitBitOrder[ SPI_TEST_SET ] = { eSPIMSBFirst };
const uint32_t spiIotFrequency[ SPI_TEST_SET ] = { 500000U };
const uint32_t spiIotDummyValue[ SPI_TEST_SET ] = {0 };

#endif /* ifndef _TEST_IOT_CONFIG_H_ */
