/*
 * FreeRTOS V202203.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @brief Header file contains APIs for managing the task which provisions WiFi networks onto
 * device and auto reconnects to provisioned networks in case of a WiFi access point disconnect.
 */

#ifndef _AWS_WIFI_CONNECT_TASK_H_
#define _AWS_WIFI_CONNECT_TASK_H_

#include "FreeRTOS.h"

/**
 * @brief Initializes WiFi provisioning and creates task which manages WiFi connection running
 * the WiFi provision loop or auto connecting to provisioned networks in case of WiFi disconnection.
 *
 * @return pdTRUE If WiFi provisioning and the task is initialized successfully.
 */
BaseType_t xWiFiConnectTaskInitialize( void );

/**
 * @brief Destroys the task for managing WiFi connection as well as disable WiFi provisioning and associated
 * data structures.
 */
void vWiFiConnectTaskDestroy( void );


#endif /* _AWS_WIFI_CONNECT_TASK_H_ */
