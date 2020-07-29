/*
 * IoT Platform V1.1.0
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
 */


#ifndef IOT_BLE_MQTT_TRANSPORT_H
#define IOT_BLE_MQTT_TRANSPORT_H

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "iot_ble_mqtt_transport_config.h"
#include "mqtt_lightweight.h"
#include "iot_ble_mqtt_serialize.h"
#include "iot_ble_data_transfer.h"
#include "platform/iot_network.h"


/**
 * @brief Initiailzes the Circular buffer to store the received data
 */
bool IotBleMqttTransportInit( const NetworkContext_t * context );

/**
 * @brief Cleans up the Circular buffer
 */
void IotBleMqttTransportCleanup( void );

/**
 * @brief Function to accept data from the channel
 *
 * @param[in] pContext An opaque used by transport interface.
 */
void IotBleMqttTransportAcceptData( const NetworkContext_t * pContext );

/**
 * @brief Transport interface write function.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[in] buf A pointer to a buffer containing data to be sent out.
 * @param[in] bytesToWrite number of bytes to write from the buffer.
 * @return the number of bytes sent.
 */
int32_t IotBleMqttTransportSend( NetworkContext_t * pContext,
                                 void * buf,
                                 size_t bytesToWrite );

/**
 * @brief Transport interface read function.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[in] buf A pointer to a buffer where incoming data will be stored.
 * @param[in] bytesToRead number of bytes to read from the transport layer.
 * @return the number of bytes successfully read.
 */
int32_t IotBleMqttTransportReceive( NetworkContext_t * pContext,
                                    void * buf,
                                    size_t bytesToRead );


#endif /* ifndef IOT_BLE_MQTT_TRANSPORT_H */
