/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_iot_mqtt_demo_network.h
 * @brief Containis functions for handling network connections for MQTT demo
 */
#ifndef AWS_IOT_DEMO_NETWORK_H_
#define AWS_IOT_DEMO_NETWORK_H_



#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include "aws_iot_mqtt.h"

/**
 * @brief Handle to an IoT network connection used by the demo.
 */
typedef void* AwsIotDemoNetworkConnection_t;

/**
 *  Callback Invoked when the underlying transport is disconnected.
 *
 * @param[in] Handle to the IoT Network connection
 */
typedef void ( *NetworkDisconnectedCallback_t ) ( AwsIotDemoNetworkConnection_t );
/**
 * @brief Creates an IoT network connection for demo
 *
 * Function creates a  IoT network connection which can be then used by demo applications with MQTT library APIs.
 *
 * @param[in] pxNetworkInterface Network Interface for the connection
 * @param[in] pxMqttConnection Handle to the MQTT connection
 * @param[in] xDisconnectCallback Callback invoked when the underlying network is disconnected.
 * @param[out] pxIoTNetworkConnection Handle to the Network Connection
 * @param[in] ulConnRetryIntervalSeconds Connection retry Interval in seconds
 * @param[in] ulConnRetryLimit Connection Retry limit
 */
void AwsIotDemo_CreateNetworkConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        NetworkDisconnectedCallback_t xDisconnectCallback,
        AwsIotDemoNetworkConnection_t* pxIoTNetworkConnection,
        uint32_t ulConnRetryIntervalSeconds,
        uint32_t ulConnRetryLimit );

/**
 * @brief Get the type of transport associated with the network connection
 * @param xIoTNetworkConnection Handle to the IoT Network Connection
 * @return The type of network connection
 */
uint32_t AwsIotDemo_GetNetworkType( AwsIotDemoNetworkConnection_t xIoTNetworkConnection );

/**
 * @brief Deletes an Iot Network Connection.
 *
 * @param xIotNetworkConnection
 */
void AwsIotDemo_DeleteNetworkConnection( AwsIotDemoNetworkConnection_t xIoTNetworkConnection );

#endif /* AWS_IOT_DEMO_NETWORK_H_ */
