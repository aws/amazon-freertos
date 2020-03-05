/*
 * FreeRTOS Common IO V0.1.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "iot_test_common_io_internal.h"

#include "iot_i2c.h"
#include "iot_uart.h"

/* I2C */
extern uint8_t uctestIotI2CSlaveAddr;
extern uint8_t uctestIotI2CDeviceRegister;
extern uint8_t uctestIotI2CWriteVal;
extern uint8_t uctestIotI2CInstanceIdx;
extern uint8_t uctestIotI2CInstanceNum;

/* TMP006 sensor address. */
const uint8_t i2cTestSlaveAddr[ IOT_TEST_COMMON_IO_I2C_SUPPORTED ] = { 0x41 };

/*
 * A potential non-reserved register.
 * http://www.ti.com/ww/eu/sensampbook/tmp006.pdf
 */
const uint8_t i2cTestDeviceRegister[ IOT_TEST_COMMON_IO_I2C_SUPPORTED ] = { 0x01 };
const uint8_t i2cTestWriteVal[ IOT_TEST_COMMON_IO_I2C_SUPPORTED ] = { 0x01 };
const uint8_t i2cTestInstanceIdx[ IOT_TEST_COMMON_IO_I2C_SUPPORTED ] = { 0 };
const uint8_t i2cTestInstanceNum[ IOT_TEST_COMMON_IO_I2C_SUPPORTED ] = { 1 };

IotI2CHandle_t gIotI2cHandle[ 4 ] = { NULL, NULL, NULL, NULL };

void SET_TEST_IOT_I2C_CONFIG( int testSet )
{
    uctestIotI2CSlaveAddr = i2cTestSlaveAddr[ testSet ];
    uctestIotI2CDeviceRegister = i2cTestDeviceRegister[ testSet ];
    uctestIotI2CWriteVal = i2cTestWriteVal[ testSet ];
    uctestIotI2CInstanceIdx = i2cTestInstanceIdx[ testSet ];
    uctestIotI2CInstanceNum = i2cTestInstanceNum[ testSet ];
}

/* UART */
extern uint8_t uctestIotUartPort;
extern uint32_t ultestIotUartFlowControl;
extern uint32_t ultestIotUartParity;
extern uint32_t ultestIotUartWordLength;
extern uint32_t ultestIotUartStopBits;

void SET_TEST_IOT_UART_CONFIG( int testSet )
{
    uctestIotUartPort = 1;
    ultestIotUartFlowControl = 0;
    ultestIotUartParity = eUartParityNone;
    ultestIotUartWordLength = UART_LEN_8;
    ultestIotUartStopBits = eUartStopBitsOne;
}
