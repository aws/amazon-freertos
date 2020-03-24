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

#endif /* ifndef _TEST_IOT_CONFIG_H_ */
