/*
 * FreeRTOS MQTT SHIM V1.1.1
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
 * @file iot_mqtt_managed_api.c
 * @brief Implements all the public facing API using MQTT v4 beta_2 library for
 * sending the packets.
 */
/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* MQTT v4_beta 2 lightweight library includes. */
#include "mqtt_lightweight.h"

/* Atomic operations. */
#include "iot_atomic.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_managedDisconnect( IotMqttConnection_t mqttConnection )
{
    int8_t contextIndex = -1;
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    /* Initializing MQTT Status. */
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;

    IotMqtt_Assert( mqttConnection != NULL );

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );

    if( contextIndex >= 0 )
    {
        IotMutex_Lock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Calling MQTT v4 beta_2 API for sending the DISCONNECT packet on the network. */
        managedMqttStatus = MQTT_Disconnect( &( connToContext[ contextIndex ].context ) );

        IotMutex_Unlock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Converting the status code. */
        status = convertReturnCode( managedMqttStatus );
    }

    return status;
}

/*-----------------------------------------------------------*/
