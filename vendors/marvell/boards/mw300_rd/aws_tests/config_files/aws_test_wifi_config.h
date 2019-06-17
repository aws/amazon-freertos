/*
 * Amazon FreeRTOS V1.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_test_wifi_config.h
 * @brief Port-specific variables for Wi-Fi tests.
 */
#ifndef _AWS_TEST_WIFI_CONFIG_H_
#define _AWS_TEST_WIFI_CONFIG_H_

/**
 * @brief The task stack size used in all Wi-Fi multi-task tests.
 */
#define testwifiTASK_STACK_SIZE             ( configMINIMAL_STACK_SIZE * 8 )

/**
 * @brief The task priority used in all Wi-Fi mulit-task tests. 
 */
#define testwifiTASK_PRIORITY               ( tskIDLE_PRIORITY + 2)

/**
 * @brief The timeout for all tasks to finish in the Wi-Fi multi-task tests.
 */
#define testwifiMULTITASK_TEST_TIMEOUT      pdMS_TO_TICKS( 900000 )

/**
 * @brief The timeout to wait for tasks to synchronize with the other in the Wi-Fi mult-task tests.
 */
#define testwifiTASK_SYNC_TIMEOUT           pdMS_TO_TICKS( 30000 )

#define testwifiLOOPBACK_TEST_SOCKETS_RECEIVE_TIMEOUT    10000

#define testwifiLOOPBACK_TEST_SOCKETS_SEND_TIMEOUT       10000

#define testwifiCONNECTION_DELAY    pdMS_TO_TICKS( 2000 )

#endif /* _AWS_TEST_WIFI_CONFIG_H_ */
