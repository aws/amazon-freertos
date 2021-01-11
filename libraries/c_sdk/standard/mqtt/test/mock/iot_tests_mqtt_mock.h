/*
 * FreeRTOS MQTT V2.3.1
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
 * @file iot_tests_mqtt_mock.h
 * @brief Declares functions to mock MQTT network responses.
 *
 * The function in this file are not thread safe; only one MQTT operation
 * should be mocked at any time.
 */

#ifndef IOT_TESTS_MQTT_MOCK_H_
#define IOT_TESTS_MQTT_MOCK_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>

/* MQTT types include. */
#include "types/iot_mqtt_types.h"

/* Define an assertion function to use. */
#ifndef IotTest_Assert
    #include <assert.h>
    #define IotTest_Assert    assert
#endif

/**
 * @brief Initialize an MQTT connection to mock.
 *
 * @param[out] pMqttConnection Set to an MQTT connection handle on success.
 *
 * @return `true` if all initialization succeeded; `false` otherwise.
 */
bool IotTest_MqttMockInit( IotMqttConnection_t * pMqttConnection );

/**
 * @brief Clean up the MQTT connection mocking.
 */
void IotTest_MqttMockCleanup( void );

#endif /* ifndef IOT_TESTS_MQTT_MOCK_H_ */
