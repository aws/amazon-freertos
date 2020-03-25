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
#include "iot_uart.h"


/*------------------------UART-------------------------------*/

/* UART is supported. */
#define IOT_TEST_COMMON_IO_UART_SUPPORTED    1

/* How many UART ports are being test. */
#define UART_TEST_SET                        1

/* Instance ID 3 corresponds to UART4. */
extern const uint8_t uartTestPort[ UART_TEST_SET ];
/* Following configurations are not being used for now. */
extern const uint32_t uartIotUartFlowControl[ UART_TEST_SET ];
extern const uint32_t uartIotUartParity[ UART_TEST_SET ];
extern const uint32_t uartIotUartWordLength[ UART_TEST_SET ];
extern const uint32_t uartIotUartStopBits[ UART_TEST_SET ];

/* How many I2C ports are being test. */
#define I2C_TEST_SET                        1

#define IOT_TEST_COMMON_IO_I2C_SUPPORTED                     1

#if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED == 1 )
    #define IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP    1
    #define IOT_TEST_COMMON_IO_I2C_SUPPORTED_CANCEL          0
#endif

extern uint8_t uctestIotI2CSlaveAddr;         /* Address of Slave I2C (7-bit address) connected to the bus */
extern uint8_t uctestIotI2CDeviceRegister;    /* Slave I2C register address used in read/write tests */
extern uint8_t uctestIotI2CWriteVal;          /* Write value that will be used in the register write test */
extern uint8_t uctestIotI2CInstanceIdx;       /* I2C instance used in the test */
extern uint8_t uctestIotI2CInstanceNum;       /* The total number of I2C instances on the device */
extern uint8_t ucAssistedTestIotI2CSlaveAddr; /* The slave address to be set for the assisted test. */

#define IOT_TEST_COMMON_IO_SPI_SUPPORTED    0

#endif /* ifndef _TEST_IOT_CONFIG_H_ */
