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

/**
 * @file aws_iot_test_access_mqtt.h
 * @brief Declares the functions that provide access to the internal functions
 * and variables of the MQTT library.
 */

#ifndef _AWS_IOT_TEST_ACCESS_MQTT_H_
#define _AWS_IOT_TEST_ACCESS_MQTT_H_

/*--------------------------- aws_iot_mqtt_api.c ---------------------------*/

/**
 * @brief Test access function for #_createMqttConnection.
 *
 * @see #_createMqttConnection.
 */
_mqttConnection_t * AwsIotTestMqtt_createMqttConnection( bool awsIotMqttMode,
                                                         const AwsIotMqttNetIf_t * const pNetworkInterface,
                                                         uint16_t keepAliveSeconds );

/**
 * @brief Test access function for #_destroyMqttConnection.
 *
 * @see #_destroyMqttConnection.
 */
void AwsIotTestMqtt_destroyMqttConnection( _mqttConnection_t * const pMqttConnection );

/*------------------------- aws_iot_mqtt_serialize.c ------------------------*/

/*
 * Macros for reading the high and low byte of a 2-byte unsigned int.
 */
#define _UINT16_HIGH_BYTE( x )    ( ( uint8_t ) ( x >> 8 ) )            /**< @brief Get high byte. */
#define _UINT16_LOW_BYTE( x )     ( ( uint8_t ) ( x & 0x00ff ) )        /**< @brief Get low byte. */

/**
 * @brief Macro for decoding a 2-byte unsigned int from a sequence of bytes.
 *
 * @param[in] ptr A uint8_t* that points to the high byte.
 */
#define _UINT16_DECODE( ptr )                               \
    ( uint16_t ) ( ( ( ( uint16_t ) ( *( ptr ) ) ) << 8 ) | \
                   ( ( uint16_t ) ( *( ptr + 1 ) ) ) )

/**
 * @brief Test access function for #_decodeRemainingLength.
 *
 * @see #_decodeRemainingLength.
 */
AwsIotMqttError_t AwsIotTestMqtt_decodeRemainingLength( const uint8_t * pSource,
                                                        const uint8_t ** const pEnd,
                                                        size_t * const pLength );

/*----------------------- aws_iot_mqtt_subscription.c -----------------------*/

/* Internal data structures of aws_iot_mqtt_subscription.c, redefined for the tests. */
typedef struct _topicMatchParams
{
    const char * pTopicName;
    uint16_t topicNameLength;
    bool exactMatchOnly;
} _topicMatchParams_t;
typedef struct _packetMatchParams
{
    uint16_t packetIdentifier;
    long order;
} _packetMatchParams_t;

/**
 * @brief Test access function for #_topicMatch.
 *
 * @see #_topicMatch.
 */
bool AwsIotTestMqtt_topicMatch( const IotLink_t * pSubscriptionLink,
                                void * pMatch );

/**
 * @brief Test access function for #_packetMatch.
 *
 * @see #_packetMatch.
 */
bool AwsIotTestMqtt_packetMatch( const IotLink_t * pSubscriptionLink,
                                 void * pMatch );

#endif /* ifndef _AWS_IOT_TEST_ACCESS_MQTT_H_ */
