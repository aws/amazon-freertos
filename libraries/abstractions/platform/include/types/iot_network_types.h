/*
 * FreeRTOS Common V1.2.0
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

/**
 * @file iot_network_types.h
 * @brief Header file contains the network types shared by both low level networking drivers
 * and upper level applications.
 */

#ifndef IOT_NETWORK_TYPES_H_
#define IOT_NETWORK_TYPES_H_

/**
 * @brief Network types supported by FreeRTOS.
 */
#define AWSIOT_NETWORK_TYPE_NONE      0x00000000
#define AWSIOT_NETWORK_TYPE_WIFI      0x00000001
#define AWSIOT_NETWORK_TYPE_BLE       0x00000002
#define AWSIOT_NETWORK_TYPE_ETH       0x00000004
#define AWSIOT_NETWORK_TYPE_ALL       ( AWSIOT_NETWORK_TYPE_WIFI | AWSIOT_NETWORK_TYPE_BLE | AWSIOT_NETWORK_TYPE_ETH )
#define AWSIOT_NETWORK_TYPE_TCP_IP    ( AWSIOT_NETWORK_TYPE_WIFI | AWSIOT_NETWORK_TYPE_ETH )

/**
 * @brief Enum types representing states for different networks.
 */
typedef enum AwsIotNetworkState
{
    eNetworkStateDisabled = 0, /*!< eNetworkStateDisabled State of Network is OFF or disabled. */
    eNetworkStateEnabled = 1,  /*!< eNetworkStateEnabled State of the network is enabled but not yet serving traffic. */
    eNetworkStateConnected = 2 /*!< eNetworkStateConnected State of the network is connected and ready for serving traffic. */
} AwsIotNetworkState_t;

#endif /* IOT_NETWORK_TYPES_H_ */
