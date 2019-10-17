/*
 * Amazon FreeRTOS V201908.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_iot_connection_task.h
 * @brief Header file containing the WIFI auto connect task initialize and destroy APIs.
 */

#ifndef _AWS_WIFI_CONNECT_TASK_H_
#define _AWS_WIFI_CONNECT_TASK_H_

#include "FreeRTOS.h"

/**
 * @brief Initialize the task which connects to provisioned WiFi networks.
 *
 * @return pdTRUE if task is initialized successfully.
 *
 */
BaseType_t xWiFiConnectTaskInitialize( void );

/**
 * @brief Destroys the task for connecting to provisioned WiFi networks its associated data structures.
 */
void vWiFiConnectTaskDestroy( void );


#endif /* _AWS_WIFI_CONNECT_TASK_H_ */
