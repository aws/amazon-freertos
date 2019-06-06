/*
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
 */

/* This file contains configuration settings for the demos. */

#ifndef _IOT_TEST_CONFIG_H_
#define _IOT_TEST_CONFIG_H_

/* Standard include. */
#include <stdbool.h>

#define AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER {NULL}

/* Platform thread priority and stack size. */
#define IOT_THREAD_DEFAULT_STACK_SIZE    ( 4 * configMINIMAL_STACK_SIZE )
#define IOT_THREAD_DEFAULT_PRIORITY      ( tskIDLE_PRIORITY )

/* Provide additional serializer initialization functions. */
extern bool IotBleMqtt_InitSerialize( void );
extern void IotBleMqtt_CleanupSerialize( void );
#define _IotMqtt_InitSerializeAdditional IotBleMqtt_InitSerialize
#define _IotMqtt_CleanupSerializeAdditional IotBleMqtt_CleanupSerialize

/* Network type configuration for this board. */
#define DEFAULT_NETWORK AWSIOT_NETWORK_TYPE_BLE
#define BLE_SUPPORTED 1
#define WIFI_SUPPORTED 0

#define IOT_TEST_NETWORK_HEADER  "platform/iot_network_ble.h" 
/* Include the common configuration file for FreeRTOS. */
#include "iot_config_common.h"

#endif /* ifndef _IOT_TEST_CONFIG_H_ */
