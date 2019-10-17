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
 * @file aws_iot_mqtt_demo_network.h
 * @brief Contains network creation and teardown functions for handling different types of network connections
 * for MQTT demos.
 */
#ifndef AWS_IOT_DEMO_NETWORK_H_
#define AWS_IOT_DEMO_NETWORK_H_



/* The config header is always included first. */
#include "iot_config.h"

#include "FreeRTOS.h"
#include "types/iot_mqtt_types.h"


typedef struct MqttConnectionContext
{
    void * pvNetworkConnection;
    uint32_t ulNetworkType;
    IotMqttNetworkInfo_t xNetworkInfo;
    IotNetworkError_t ( * xDisconnectCallback )( void * );
    IotMqttConnection_t xMqttConnection;
} MqttConnectionContext_t;


/**
 * @brief Creates an IoT network connection for demo.
 *
 * Function creates a  IoT network connection which can be then used by demo applications with MQTT library APIs.
 * IOT network connection is an abstraction over an BLE or WIFI connection.
 *
 * @param pxNetworkConnection Pointer to the network connection context passed from user.
 * @param ulPreferredNetworks OR separated flags indicating preferred network types.
 * @return pdTRUE if network is created successfully.
 */
BaseType_t xMqttDemoCreateNetworkConnection( MqttConnectionContext_t * pxNetworkContext,
                                             uint32_t ulNetworkTypes );

/**
 * @brief Deletes a Network Connection.
 *
 * @param pxNetworkConnection pointer to the network connection context.
 */
void vMqttDemoDeleteNetworkConnection( MqttConnectionContext_t * pxNetworkConnection );

#endif /* AWS_IOT_DEMO_NETWORK_H_ */
