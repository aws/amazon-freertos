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
 * @file iot_test_access_mqtt_api.c
 * @brief Provides access to the internal functions and variables of
 * iot_mqtt_api.c
 *
 * This file should only be included at the bottom of iot_mqtt_api.c and never
 * compiled by itself.
 */

_mqttConnection_t * IotTestMqtt_createMqttConnection( bool awsIotMqttMode,
                                                      const IotMqttNetworkInfo_t * pNetworkInfo,
                                                      uint16_t keepAliveSeconds );

/*-----------------------------------------------------------*/

_mqttConnection_t * IotTestMqtt_createMqttConnection( bool awsIotMqttMode,
                                                      const IotMqttNetworkInfo_t * pNetworkInfo,
                                                      uint16_t keepAliveSeconds )
{
    return _createMqttConnection( awsIotMqttMode, pNetworkInfo, keepAliveSeconds );
}

/*-----------------------------------------------------------*/
