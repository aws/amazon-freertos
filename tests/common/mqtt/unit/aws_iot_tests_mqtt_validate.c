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
 * @file aws_iot_tests_mqtt_validate.c
 * @brief Tests for the functions in aws_iot_mqtt_validate.c
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief Length of the subscription array used in
 * #TEST_MQTT_Unit_Validate_ValidateSubscriptionList_.
 */
#define _SUBSCRIPTION_COUNT    ( _AWS_IOT_MQTT_SERVER_MAX_TOPIC_FILTERS_PER_SUBSCRIBE )

/**
 * @brief A non-NULL function pointer.
 */
#define _FUNCTION_POINTER      ( ( void * ) 0x1 )

/*-----------------------------------------------------------*/

/**
 * @brief Test group for MQTT validate tests.
 */
TEST_GROUP( MQTT_Unit_Validate );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for MQTT validate tests.
 */
TEST_SETUP( MQTT_Unit_Validate )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT validate tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_Validate )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT validate tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_Validate )
{
    RUN_TEST_CASE( MQTT_Unit_Validate, ValidateNetIf );
    RUN_TEST_CASE( MQTT_Unit_Validate, ValidateConnectInfo );
    RUN_TEST_CASE( MQTT_Unit_Validate, ValidatePublish );
    RUN_TEST_CASE( MQTT_Unit_Validate, ValidateReference );
    RUN_TEST_CASE( MQTT_Unit_Validate, ValidateSubscriptionList );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test validation of an #AwsIotMqttNetIf_t.
 */
TEST( MQTT_Unit_Validate, ValidateNetIf )
{
    bool validateStatus = false;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;

    /* NULL parameter. */
    validateStatus = AwsIotMqttInternal_ValidateNetIf( NULL );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Uninitialized parameter. */
    validateStatus = AwsIotMqttInternal_ValidateNetIf( &networkInterface );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Uninitialized disconnect function is allowed. */
    networkInterface.send = _FUNCTION_POINTER;
    validateStatus = AwsIotMqttInternal_ValidateNetIf( &networkInterface );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* Check serializer override function pointers. */
    #if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        /* No freePacket function with serializer. */
        networkInterface.serialize.disconnect = _FUNCTION_POINTER;
        validateStatus = AwsIotMqttInternal_ValidateNetIf( &networkInterface );
        TEST_ASSERT_EQUAL_INT( false, validateStatus );
        networkInterface.serialize.disconnect = NULL;

        /* No freePacket function with deserializer. */
        networkInterface.deserialize.pingresp = _FUNCTION_POINTER;
        validateStatus = AwsIotMqttInternal_ValidateNetIf( &networkInterface );
        TEST_ASSERT_EQUAL_INT( false, validateStatus );

        /* freePacket function pointer set. */
        networkInterface.freePacket = _FUNCTION_POINTER;
        validateStatus = AwsIotMqttInternal_ValidateNetIf( &networkInterface );
        TEST_ASSERT_EQUAL_INT( true, validateStatus );
    #endif /* if AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1 */
}

/*-----------------------------------------------------------*/

/**
 * @brief Test validation of an #AwsIotMqttConnectInfo_t.
 */
TEST( MQTT_Unit_Validate, ValidateConnectInfo )
{
    bool validateStatus = false;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;

    /* NULL parameter. */
    validateStatus = AwsIotMqttInternal_ValidateConnect( NULL );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Uninitialized parameter. */
    validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Zero-length client identifier with clean session. */
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = "";
    connectInfo.clientIdentifierLength = 0;
    validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Client identifier longer than 23 characters. */
    connectInfo.pClientIdentifier = "longlongclientidentifier";
    connectInfo.clientIdentifierLength = 24;
    validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* AWS IoT MQTT service limit tests. */
    #if AWS_IOT_MQTT_MODE == 1
        /* Client identifier too long. */
        connectInfo.clientIdentifierLength = _AWS_IOT_MQTT_SERVER_MAX_CLIENTID + 1;
        validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
        TEST_ASSERT_EQUAL_INT( false, validateStatus );
        connectInfo.clientIdentifierLength = 24;

        /* Keep-alive disabled. */
        connectInfo.keepAliveSeconds = 0;
        validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
        TEST_ASSERT_EQUAL_INT( true, validateStatus );

        /* Keep-alive too small. */
        connectInfo.keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE - 1;
        validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
        TEST_ASSERT_EQUAL_INT( true, validateStatus );

        /* Keep-alive too large. */
        connectInfo.keepAliveSeconds = _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE + 1;
        validateStatus = AwsIotMqttInternal_ValidateConnect( &connectInfo );
        TEST_ASSERT_EQUAL_INT( true, validateStatus );
    #endif /* if AWS_IOT_MQTT_MODE == 1 */
}

/*-----------------------------------------------------------*/

/**
 * @brief Test validation of an #AwsIotMqttPublishInfo_t.
 */
TEST( MQTT_Unit_Validate, ValidatePublish )
{
    bool validateStatus = false;
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* NULL parameter. */
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, NULL );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Zero-length topic name. */
    publishInfo.pTopicName = "";
    publishInfo.topicNameLength = 0;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    publishInfo.pTopicName = "/test";
    publishInfo.topicNameLength = 5;

    /* Zero-length/NULL payload. */
    publishInfo.pPayload = NULL;
    publishInfo.payloadLength = 0;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* NULL payload only allowed with length 0. */
    publishInfo.pPayload = NULL;
    publishInfo.payloadLength = 1;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    publishInfo.payloadLength = 0;

    /* Negative QoS or QoS > 2. */
    publishInfo.QoS = -1;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    publishInfo.QoS = 3;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    publishInfo.QoS = 0;

    /* Negative retry limit. */
    publishInfo.retryLimit = -1;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Positive retry limit with no period. */
    publishInfo.retryLimit = 1;
    publishInfo.retryMs = 0;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Positive retry limit with positive period. */
    publishInfo.retryLimit = 1;
    publishInfo.retryMs = 1;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* Retry limit 0. */
    publishInfo.retryLimit = 0;
    validateStatus = AwsIotMqttInternal_ValidatePublish( false, &publishInfo );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* AWS IoT MQTT service limit tests. */

    /* QoS 2. */
    publishInfo.QoS = 2;
    validateStatus = AwsIotMqttInternal_ValidatePublish( true, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    publishInfo.QoS = 0;

    /* Retained message. */
    publishInfo.retain = true;
    validateStatus = AwsIotMqttInternal_ValidatePublish( true, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    publishInfo.retain = false;

    /* Topic name too long. */
    publishInfo.topicNameLength = _AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH + 1;
    validateStatus = AwsIotMqttInternal_ValidatePublish( true, &publishInfo );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test validation of an #AwsIotMqttReference_t.
 */
TEST( MQTT_Unit_Validate, ValidateReference )
{
    bool validateStatus = false;
    _mqttOperation_t reference = { 0 };

    /* NULL parameter. */
    validateStatus = AwsIotMqttInternal_ValidateReference( NULL );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Non-waitable reference. */
    reference.flags = 0;
    validateStatus = AwsIotMqttInternal_ValidateReference( &reference );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Waitable (valid) reference. */
    reference.flags = AWS_IOT_MQTT_FLAG_WAITABLE;
    validateStatus = AwsIotMqttInternal_ValidateReference( &reference );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test validation of a list of #AwsIotMqttSubscription_t.
 */
TEST( MQTT_Unit_Validate, ValidateSubscriptionList )
{
    size_t i = 0;
    bool validateStatus = false;
    AwsIotMqttSubscription_t pSubscriptions[ _SUBSCRIPTION_COUNT ] = { AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* NULL parameter. */
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, NULL, 1 );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Zero parameter. */
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, 0 );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Uninitialized subscriptions. */
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* Initialize all subscriptions to valid values. */
    for( i = 0; i < _SUBSCRIPTION_COUNT; i++ )
    {
        pSubscriptions[ i ].pTopicFilter = "/test";
        pSubscriptions[ i ].topicFilterLength = 5;
        pSubscriptions[ i ].callback.function = _FUNCTION_POINTER;
    }

    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* One subscription with invalid QoS. */
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].QoS = -1;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].QoS = 3;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    /* QoS is not validated for UNSUBSCRIBE. */
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_UNSUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].QoS = 0;

    /* One subscription with no callback. */
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].callback.function = NULL;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    /* Callback is not validated for UNSUBSCRIBE. */
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_UNSUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].callback.function = _FUNCTION_POINTER;

    /* Valid subscription filters. */
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "#";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 1;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "/#";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "/+/";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 3;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "+/";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "/+";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "+/+/+/+";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 7;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( true, validateStatus );

    /* Invalid subscription filters. */
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "#/";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "#/#";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 3;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "a#";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "a+";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].pTopicFilter = "+a";
    pSubscriptions[ _SUBSCRIPTION_COUNT - 1 ].topicFilterLength = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE, false, pSubscriptions, _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* AWS IoT MQTT service limit tests. */

    /* Too many subscriptions. */
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE,
                                                                  true,
                                                                  pSubscriptions,
                                                                  _AWS_IOT_MQTT_SERVER_MAX_TOPIC_FILTERS_PER_SUBSCRIBE + 1 );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );

    /* QoS 2. */
    pSubscriptions[ 0 ].QoS = 2;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE,
                                                                  true,
                                                                  pSubscriptions,
                                                                  _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
    pSubscriptions[ 0 ].QoS = 0;

    /* Topic filter too long. */
    pSubscriptions[ 0 ].topicFilterLength = _AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH + 1;
    validateStatus = AwsIotMqttInternal_ValidateSubscriptionList( AWS_IOT_MQTT_SUBSCRIBE,
                                                                  true,
                                                                  pSubscriptions,
                                                                  _SUBSCRIPTION_COUNT );
    TEST_ASSERT_EQUAL_INT( false, validateStatus );
}

/*-----------------------------------------------------------*/
