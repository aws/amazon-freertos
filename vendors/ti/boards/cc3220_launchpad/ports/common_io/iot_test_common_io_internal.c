/*******************************************************************************
 * IOT On-Target Unit Test Internal - LPC54018
 *
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *******************************************************************************
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
