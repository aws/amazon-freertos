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
 * @file iot_test_access_mqtt.h
 * @brief Declares the functions that provide access to the internal functions
 * and variables of the MQTT library.
 */

#ifndef IOT_TEST_ACCESS_MQTT_H_
#define IOT_TEST_ACCESS_MQTT_H_

/*--------------------------- iot_mqtt_api.c ---------------------------*/

/**
 * @brief Test access function for #_createMqttConnection.
 *
 * @see #_createMqttConnection.
 */
_mqttConnection_t * IotTestMqtt_createMqttConnection( bool awsIotMqttMode,
                                                      const IotMqttNetworkInfo_t * pNetworkInfo,
                                                      uint16_t keepAliveSeconds );

/*------------------------- iot_mqtt_serialize.c ------------------------*/

/*
 * Macros for reading the high and low byte of a 2-byte unsigned int.
 */
#define UINT16_HIGH_BYTE( x )    ( ( uint8_t ) ( x >> 8 ) )            /**< @brief Get high byte. */
#define UINT16_LOW_BYTE( x )     ( ( uint8_t ) ( x & 0x00ff ) )        /**< @brief Get low byte. */

/**
 * @brief Macro for decoding a 2-byte unsigned int from a sequence of bytes.
 *
 * @param[in] ptr A uint8_t* that points to the high byte.
 */
#define UINT16_DECODE( ptr )                                \
    ( uint16_t ) ( ( ( ( uint16_t ) ( *( ptr ) ) ) << 8 ) | \
                   ( ( uint16_t ) ( *( ptr + 1 ) ) ) )

/*----------------------- iot_mqtt_subscription.c -----------------------*/

/**
 * @brief Test access function for #_topicMatch.
 *
 * @see #_topicMatch.
 */
bool IotTestMqtt_topicMatch( _mqttSubscription_t * pSubscription,
                             void * pMatch );

/**
 * @brief Test access function for #_packetMatch.
 *
 * @see #_packetMatch.
 */
bool IotTestMqtt_packetMatch( _mqttSubscription_t * pSubscription,
                              void * pMatch );

/**
 * @brief Test access function for #_IotMqtt_getFreeIndexFromContextConnectionArray.
 *
 * @see #_IotMqtt_getFreeIndexFromContextConnectionArray.
 */
int8_t IotTestMqtt_getFreeIndexFromContextConnectionArray( void );

#endif /* ifndef IOT_TEST_ACCESS_MQTT_H_ */
