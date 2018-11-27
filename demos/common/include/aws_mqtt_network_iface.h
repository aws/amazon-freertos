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
 * @file aws_mqtt_network_iface.h
 * @brief The APIS used to setup teardown network connections for MQTT.
 */

#ifndef AWS_MQTT_NETWORK_IFACE_H_
#define AWS_MQTT_NETWORK_IFACE_H_

/* MQTT include. */
#include "aws_iot_mqtt.h"

/**
 * @brief Creates the Network interface for MQTT connection.
 *
 * The function creates underlying network interface used for MQTT connections
 *
 * @return The network interface used for MQTT connection or NULL if it failed to create one.
 */
AwsIotMqttNetIf_t* AwsIotMqtt_CreateNetIface( void );


/**
 * @brief Opens a network connection with the underlying network
 *
 * The underlying connection can be secure TLS over TCP/IP connection such as in WIFI or
 * a BLE GATT connection.
 * @param[in] pxNetIf The network interface for which the connection is created.
 * @param[in] pxMqttConnection pointer to the MQTT connection to which the network connection is attached.
 * @param[out] pConnection  Handle of the opened connection
 * @return true if the connection succeeded, false if otherwise
 */
bool AwsIotMqtt_OpenConnection( AwsIotMqttNetIf_t* pxNetIf,
		AwsIotMqttConnection_t* pxMqttConnection,
		size_t xRetryLimit,
		size_t xRetryIntervalMS,
		void** pConnection );


/**
 * @brief Closes the network connection.
 * @param[in] pConnection Handle of the connection to be closed
 */
void AwsIotMqtt_CloseConnection( void* pConnection );

/**
 * @brief Frees up the resource used by a closed network connection.
 *
 * @param[in] pxNetIf The interface for which network connection is to be freed.
 * @param[in] pConnection Handle of the closed connection
 */
void AwsIotMqtt_DestroyConnection( AwsIotMqttNetIf_t* pxNetIf, void* pConnection );


/**
 * @brief Frees the network interface.
 *
 * @param[in] pxNetIf The network interface to be freed.
 */
void AwsIotMqtt_DestroyNetworkIface( AwsIotMqttNetIf_t* pxNetIf );

#endif /* AWS_MQTT_NETWORK_IFACE_H_ */
