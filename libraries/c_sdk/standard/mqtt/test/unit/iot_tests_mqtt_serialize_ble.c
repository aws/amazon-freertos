/*
 * FreeRTOS MQTT V2.2.0
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
 * @file iot_tests_mqtt_serialize_ble.c
 * @brief Tests for serialize/deserialize functions for MQTT BLE proxy
 */

#include "iot_config.h"

#include "FreeRTOS.h"

#include <string.h>
#include "iot_ble_config.h"
#include "iot_serializer.h"
#include "private/iot_mqtt_internal.h"
#include "iot_ble_mqtt_serialize.h"
#include "aws_clientcredential.h"

/* Test framework includes. */
#include "unity_fixture.h"


#define TEST_TOPIC                "/topic1"
#define TEST_TOPIC_LENGTH         11

#define TEST_TOPIC2               "/topic2"
#define TEST_TOPIC2_LENGTH        12

#define TEST_DATA                 "HelloWorld"
#define TEST_DATA_LENGTH          15

#define TEST_CLIENTID             "clientID"
#define TEST_USER_NAME            "user"

#define TEST_QOS0                 0
#define TEST_QOS1                 1
#define TEST_RETRY_MS             0
#define TEST_RETRY_LIMIT          0
#define TEST_PACKET_IDENTIFIER    5

#define TEST_MESG_LEN             512


static bool serializeInit;

TEST_GROUP( MQTT_Unit_BLE_Serialize );

/*-----------------------------------------------------------*/

TEST_SETUP( MQTT_Unit_BLE_Serialize )
{
    serializeInit = IotBleMqtt_InitSerialize();
    TEST_ASSERT_EQUAL( true, serializeInit );
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( MQTT_Unit_BLE_Serialize )
{
    if( serializeInit == true )
    {
        IotBleMqtt_CleanupSerialize();
    }
}


TEST_GROUP_RUNNER( MQTT_Unit_BLE_Serialize )
{
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeCONNECT );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBLISH );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBLISH_QOS0 );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBACK );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeDISCONNECT );

    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, DeserializeCONNACK );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, DeserializePUBLISH );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, DeserializePUBLISH_QOS0 );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, DeserializeSUBACK );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, DeserializePUBACK );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, DeserializeUNSUBACK );

    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeCONNECT_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBLISH_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBACK_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeDISCONNECT_MallocFail );
}

TEST( MQTT_Unit_BLE_Serialize, SerializeCONNECT )
{
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;


    connectInfo.clientIdentifierLength = strlen( TEST_CLIENTID );
    connectInfo.pClientIdentifier = TEST_CLIENTID;
    connectInfo.pUserName = TEST_USER_NAME;
    connectInfo.userNameLength = strlen( TEST_USER_NAME );
    connectInfo.pPassword = NULL;
    connectInfo.cleanSession = true;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeConnect( &connectInfo, ( uint8_t ** ) &pMesg, &bufLen );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

        /*Verify its in proper encoded format */
        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_CONNECT, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_CLIENT_ID, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, TEST_CLIENTID, xValue.u.value.u.string.length ) );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_BROKER_EP, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, clientcredentialMQTT_BROKER_ENDPOINT, xValue.u.value.u.string.length ) );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_CLEAN_SESSION, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_BOOL, xValue.type );
        TEST_ASSERT_EQUAL( true, xValue.u.value.u.booleanValue );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_CLIENT_ID, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, TEST_CLIENTID, xValue.u.value.u.string.length ) );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializePUBLISH )
{
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier;

    publishInfo.qos = TEST_QOS1;
    publishInfo.pPayload = ( uint8_t * ) TEST_DATA;
    publishInfo.payloadLength = TEST_DATA_LENGTH;
    publishInfo.pTopicName = TEST_TOPIC;
    publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
    publishInfo.retain = false;
    publishInfo.retryMs = TEST_RETRY_MS;
    publishInfo.retryLimit = TEST_RETRY_LIMIT;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializePublish( &publishInfo, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier, NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
        TEST_ASSERT_NOT_EQUAL( 0, packetIdentifier );

        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_PUBLISH, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_TOPIC, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, TEST_TOPIC, xValue.u.value.u.string.length ) );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_PAYLOAD, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_BYTE_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, TEST_DATA, xValue.u.value.u.string.length ) );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_QOS, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( TEST_QOS1, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MESSAGE_ID, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( packetIdentifier, xValue.u.value.u.signedInt );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializePUBLISH_QOS0 )
{
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier = 0;

    publishInfo.qos = TEST_QOS0;
    publishInfo.pPayload = ( uint8_t * ) TEST_DATA;
    publishInfo.payloadLength = TEST_DATA_LENGTH;
    publishInfo.pTopicName = TEST_TOPIC;
    publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
    publishInfo.retain = false;
    publishInfo.retryMs = TEST_RETRY_MS;
    publishInfo.retryLimit = TEST_RETRY_LIMIT;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializePublish( &publishInfo, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier, NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
        TEST_ASSERT_EQUAL( 0, packetIdentifier );

        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_PUBLISH, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_TOPIC, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, TEST_TOPIC, xValue.u.value.u.string.length ) );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_PAYLOAD, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_BYTE_STRING, xValue.type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) xValue.u.value.u.string.pString, TEST_DATA, xValue.u.value.u.string.length ) );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_QOS, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( TEST_QOS0, xValue.u.value.u.signedInt );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE )
{
    IotMqttSubscription_t subscriptionList[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER, IOT_MQTT_SUBSCRIPTION_INITIALIZER };
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier;

    subscriptionList[ 0 ].qos = TEST_QOS1;
    subscriptionList[ 0 ].pTopicFilter = TEST_TOPIC;
    subscriptionList[ 0 ].topicFilterLength = TEST_TOPIC_LENGTH;
    subscriptionList[ 1 ].qos = TEST_QOS1;
    subscriptionList[ 1 ].pTopicFilter = TEST_TOPIC2;
    subscriptionList[ 1 ].topicFilterLength = TEST_TOPIC2_LENGTH;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeSubscribe( subscriptionList, 2, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
        TEST_ASSERT_NOT_EQUAL( 0, packetIdentifier );

        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_SUBSCRIBE, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MESSAGE_ID, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( packetIdentifier, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_TOPIC_LIST, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_ARRAY, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xValue );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_QOS_LIST, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_ARRAY, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xValue );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE )
{
    IotMqttSubscription_t subscriptionList[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER, IOT_MQTT_SUBSCRIPTION_INITIALIZER };
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier;

    subscriptionList[ 0 ].qos = TEST_QOS1;
    subscriptionList[ 0 ].pTopicFilter = TEST_TOPIC;
    subscriptionList[ 0 ].topicFilterLength = TEST_TOPIC_LENGTH;
    subscriptionList[ 1 ].qos = TEST_QOS1;
    subscriptionList[ 1 ].pTopicFilter = TEST_TOPIC2;
    subscriptionList[ 1 ].topicFilterLength = TEST_TOPIC2_LENGTH;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeUnsubscribe( subscriptionList, 2, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
        TEST_ASSERT_NOT_EQUAL( 0, packetIdentifier );

        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_UNSUBSCRIBE, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MESSAGE_ID, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( packetIdentifier, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_TOPIC_LIST, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_ARRAY, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xValue );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}


TEST( MQTT_Unit_BLE_Serialize, SerializePUBACK )
{
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializePuback( TEST_PACKET_IDENTIFIER, ( uint8_t ** ) &pMesg, &bufLen );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_PUBACK, xValue.u.value.u.signedInt );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MESSAGE_ID, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, xValue.u.value.u.signedInt );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializeDISCONNECT )
{
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeDisconnect( ( uint8_t ** ) &pMesg, &bufLen );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

        xResult = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( const uint8_t * ) pMesg, bufLen );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, xDecoderObj.type );

        xValue.u.value.u.signedInt = 0;
        xResult = IOT_BLE_MESG_DECODER.find( &xDecoderObj, IOT_BLE_MQTT_MSG_TYPE, &xValue );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, xResult );
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, xValue.type );
        TEST_ASSERT_EQUAL( IOT_BLE_MQTT_MSG_TYPE_DISCONNECT, xValue.u.value.u.signedInt );

        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

static void prvCreateCONNACKPacket( uint8_t * pBuffer,
                                    size_t * pLength,
                                    int64_t respCode,
                                    uint16_t numParams )
{
    IotSerializerEncoderObject_t xEncoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t xConnAackMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t xData = { 0 };
    IotSerializerError_t xError;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pLength );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    xError = IOT_BLE_MESG_ENCODER.openContainer(
        &xEncoderObj,
        &xConnAackMap,
        ( numParams + 1 ) );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_CONNACK;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnAackMap, IOT_BLE_MQTT_MSG_TYPE, xData );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    if( respCode != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = respCode;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnAackMap, IOT_BLE_MQTT_STATUS, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xConnAackMap );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    *pLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );

    TEST_ASSERT_NOT_EQUAL( 0, *pLength );

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
}

TEST( MQTT_Unit_BLE_Serialize, DeserializeCONNACK )
{
    IotMqttError_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    _mqttPacket_t connack;

    connack.remainingLength = TEST_MESG_LEN;
    connack.pRemainingData = buffer;

    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_CONNECTED, 1 );

    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_CONNECTING, 1 );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_CONNECTION_REFUSED, 1 );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_SERVER_REFUSED, status );

    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_DISCONNECTED, 1 );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_SERVER_REFUSED, status );

    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_PROTOCOL_ERROR, 1 );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_SERVER_REFUSED, status );

    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_UNKNOWN, 1 );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_SERVER_REFUSED, status );

    /** Empty message **/
    memset( connack.pRemainingData, 0x00, TEST_MESG_LEN );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** Tampered message **/
    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, IOT_BLE_MQTT_STATUS_CONNECTED, 1 );
    connack.pRemainingData[ 0 ] = 0x00; /*Not valid cbor */
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** No response code */
    prvCreateCONNACKPacket( connack.pRemainingData, &connack.remainingLength, -1, 0 );
    status = IotBleMqtt_DeserializeConnack( &connack );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );
}

static void prvCreatePUBLISHPacket( uint8_t * pBuffer,
                                    size_t * pLength,
                                    const char * pTopic,
                                    size_t topicLen,
                                    const char * pPayload,
                                    size_t payloadLen,
                                    int16_t qos,
                                    int32_t packetId,
                                    uint16_t numParams )
{
    IotSerializerEncoderObject_t xEncoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t xPubMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t xData = { 0 };
    IotSerializerError_t xError;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pLength );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    xError = IOT_BLE_MESG_ENCODER.openContainer(
        &xEncoderObj,
        &xPubMap,
        ( numParams + 1 ) );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );


    xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_PUBLISH;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubMap, IOT_BLE_MQTT_MSG_TYPE, xData );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    if( pTopic != NULL )
    {
        xData.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
        xData.value.u.string.pString = ( uint8_t * ) pTopic;
        xData.value.u.string.length = topicLen;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubMap, IOT_BLE_MQTT_TOPIC, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    if( pPayload != NULL )
    {
        xData.type = IOT_SERIALIZER_SCALAR_BYTE_STRING;
        xData.value.u.string.pString = ( uint8_t * ) pPayload;
        xData.value.u.string.length = payloadLen;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubMap, IOT_BLE_MQTT_PAYLOAD, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    if( qos != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = qos;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubMap, IOT_BLE_MQTT_QOS, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    if( packetId != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = packetId;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubMap, IOT_BLE_MQTT_MESSAGE_ID, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xPubMap );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    *pLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    TEST_ASSERT_NOT_EQUAL( 0, *pLength );

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
}

TEST( MQTT_Unit_BLE_Serialize, DeserializePUBLISH )
{
    IotMqttError_t status;
    uint8_t buffer[ TEST_MESG_LEN ];

    _mqttOperation_t output = { 0 };

    _mqttPacket_t packet =
    {
        .pRemainingData     = buffer,
        .remainingLength    = TEST_MESG_LEN,
        .u.pIncomingPublish = &output
    };

    prvCreatePUBLISHPacket( packet.pRemainingData,
                            &packet.remainingLength,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            4 );

    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
    TEST_ASSERT_EQUAL_INT( TEST_TOPIC_LENGTH, output.u.publish.publishInfo.topicNameLength );
    TEST_ASSERT_EQUAL( 0, strncmp( output.u.publish.publishInfo.pTopicName, TEST_TOPIC, TEST_TOPIC_LENGTH ) );
    TEST_ASSERT_EQUAL_INT( TEST_DATA_LENGTH, output.u.publish.publishInfo.payloadLength );
    TEST_ASSERT_EQUAL( 0, strncmp( output.u.publish.publishInfo.pPayload, TEST_DATA, TEST_DATA_LENGTH ) );
    TEST_ASSERT_EQUAL( TEST_QOS1, output.u.publish.publishInfo.qos );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packet.packetIdentifier );
    TEST_ASSERT_EQUAL( false, output.u.publish.publishInfo.retain );

    /** Malformed message **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    packet.remainingLength = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( packet.pRemainingData,
                            &packet.remainingLength,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            4 );
    packet.pRemainingData[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** NO QOS **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    packet.remainingLength = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( packet.pRemainingData,
                            &packet.remainingLength,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            -1,
                            TEST_PACKET_IDENTIFIER,
                            3 );
    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** NO Topic **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    packet.remainingLength = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( packet.pRemainingData,
                            &packet.remainingLength,
                            NULL,
                            0,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            3 );
    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** NO Packet id **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    packet.remainingLength = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( packet.pRemainingData,
                            &packet.remainingLength,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            -1,
                            3 );
    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** NO Payload **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    packet.remainingLength = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( packet.pRemainingData,
                            &packet.remainingLength,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            NULL,
                            0,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            3 );
    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );
}

TEST( MQTT_Unit_BLE_Serialize, DeserializePUBLISH_QOS0 )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };

    _mqttOperation_t output = { 0 };

    _mqttPacket_t packet =
    {
        .pRemainingData     = buffer,
        .remainingLength    = TEST_MESG_LEN,
        .u.pIncomingPublish = &output
    };

    prvCreatePUBLISHPacket(
        packet.pRemainingData,
        &packet.remainingLength,
        TEST_TOPIC,
        TEST_TOPIC_LENGTH,
        TEST_DATA,
        TEST_DATA_LENGTH,
        TEST_QOS0,
        -1,
        3 );

    status = IotBleMqtt_DeserializePublish( &packet );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
    TEST_ASSERT_EQUAL_INT( TEST_TOPIC_LENGTH, output.u.publish.publishInfo.topicNameLength );
    TEST_ASSERT_EQUAL( 0, strncmp( output.u.publish.publishInfo.pTopicName, TEST_TOPIC, TEST_TOPIC_LENGTH ) );
    TEST_ASSERT_EQUAL_INT( TEST_DATA_LENGTH, output.u.publish.publishInfo.payloadLength );
    TEST_ASSERT_EQUAL( 0, strncmp( output.u.publish.publishInfo.pPayload, TEST_DATA, TEST_DATA_LENGTH ) );
    TEST_ASSERT_EQUAL( TEST_QOS0, output.u.publish.publishInfo.qos );
    TEST_ASSERT_EQUAL( false, output.u.publish.publishInfo.retain );
}

static void prvCreateSUBACKPacket( uint8_t * pBuffer,
                                   size_t * pLength,
                                   int32_t status,
                                   int32_t packetId,
                                   uint16_t numParams )
{
    IotSerializerEncoderObject_t xEncoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t xSubackMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t xData = { 0 };
    IotSerializerError_t xError;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pLength );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    xError = IOT_BLE_MESG_ENCODER.openContainer(
        &xEncoderObj,
        &xSubackMap,
        ( numParams + 1 ) );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );


    xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_SUBACK;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubackMap, IOT_BLE_MQTT_MSG_TYPE, xData );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    if( status != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = status;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubackMap, IOT_BLE_MQTT_STATUS, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    if( packetId != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = packetId;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubackMap, IOT_BLE_MQTT_MESSAGE_ID, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xSubackMap );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    *pLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    TEST_ASSERT_NOT_EQUAL( 0, *pLength );

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
}

TEST( MQTT_Unit_BLE_Serialize, DeserializeSUBACK )
{
    IotMqttError_t status;
    uint8_t buffer[ TEST_MESG_LEN ];
    _mqttPacket_t suback;

    suback.pRemainingData = buffer;
    suback.remainingLength = TEST_MESG_LEN;

    prvCreateSUBACKPacket( suback.pRemainingData, &suback.remainingLength, TEST_QOS1, TEST_PACKET_IDENTIFIER, 2 );

    status = IotBleMqtt_DeserializeSuback( &suback );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, suback.packetIdentifier );

    /** Malformed message **/
    suback.remainingLength = TEST_MESG_LEN;
    prvCreateSUBACKPacket( suback.pRemainingData, &suback.remainingLength, TEST_QOS1, TEST_PACKET_IDENTIFIER, 2 );
    suback.pRemainingData[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializeSuback( &suback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );


    /** No Status **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    suback.remainingLength = TEST_MESG_LEN;
    prvCreateSUBACKPacket( suback.pRemainingData, &suback.remainingLength, -1, TEST_PACKET_IDENTIFIER, 1 );
    suback.pRemainingData[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializeSuback( &suback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** No packet id **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    suback.remainingLength = TEST_MESG_LEN;
    prvCreateSUBACKPacket( suback.pRemainingData, &suback.remainingLength, TEST_QOS1, -1, 1 );
    suback.pRemainingData[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializeSuback( &suback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );
}

static void prvCreatePUBACKPacket( uint8_t * pBuffer,
                                   size_t * pLength,
                                   int32_t packetId,
                                   uint16_t numParams )
{
    IotSerializerEncoderObject_t xEncoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t xPubackMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t xData = { 0 };
    IotSerializerError_t xError;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pLength );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    xError = IOT_BLE_MESG_ENCODER.openContainer(
        &xEncoderObj,
        &xPubackMap,
        ( numParams + 1 ) );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );


    xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_PUBACK;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubackMap, IOT_BLE_MQTT_MSG_TYPE, xData );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    if( packetId != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = packetId;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubackMap, IOT_BLE_MQTT_MESSAGE_ID, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xPubackMap );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    *pLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    TEST_ASSERT_NOT_EQUAL( 0, *pLength );

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
}


TEST( MQTT_Unit_BLE_Serialize, DeserializePUBACK )
{
    IotMqttError_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    _mqttPacket_t puback;

    puback.pRemainingData = buffer;
    puback.remainingLength = TEST_MESG_LEN;

    prvCreatePUBACKPacket( puback.pRemainingData, &puback.remainingLength, TEST_PACKET_IDENTIFIER, 1 );
    status = IotBleMqtt_DeserializePuback( &puback );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, puback.packetIdentifier );

    /** Malformed message */
    memset( buffer, 0x00, TEST_MESG_LEN );
    puback.remainingLength = TEST_MESG_LEN;
    prvCreatePUBACKPacket( puback.pRemainingData, &puback.remainingLength, TEST_PACKET_IDENTIFIER, 1 );
    puback.pRemainingData[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializePuback( &puback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** No message id */
    memset( buffer, 0x00, TEST_MESG_LEN );
    puback.remainingLength = TEST_MESG_LEN;
    prvCreatePUBACKPacket( puback.pRemainingData, &puback.remainingLength, -1, 0 );
    status = IotBleMqtt_DeserializePuback( &puback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );
}


static void prvCreateUNSUBACKPacket( uint8_t * pBuffer,
                                     size_t * pLength,
                                     int32_t packetId,
                                     uint16_t numParams )
{
    IotSerializerEncoderObject_t xEncoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t xUnsubackMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t xData = { 0 };
    IotSerializerError_t xError;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pLength );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    xError = IOT_BLE_MESG_ENCODER.openContainer(
        &xEncoderObj,
        &xUnsubackMap,
        ( numParams + 1 ) );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );


    xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_UNSUBACK;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xUnsubackMap, IOT_BLE_MQTT_MSG_TYPE, xData );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    if( packetId != -1 )
    {
        xData.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.u.signedInt = packetId;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xUnsubackMap, IOT_BLE_MQTT_MESSAGE_ID, xData );
        TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xUnsubackMap );
    TEST_ASSERT_EQUAL( xError, IOT_SERIALIZER_SUCCESS );

    *pLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    TEST_ASSERT_NOT_EQUAL( 0, *pLength );

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
}

TEST( MQTT_Unit_BLE_Serialize, DeserializeUNSUBACK )
{
    IotMqttError_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    _mqttPacket_t puback;

    puback.pRemainingData = buffer;
    puback.remainingLength = TEST_MESG_LEN;

    prvCreateUNSUBACKPacket( puback.pRemainingData, &puback.remainingLength, TEST_PACKET_IDENTIFIER, 1 );
    status = IotBleMqtt_DeserializePuback( &puback );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, puback.packetIdentifier );

    /** Malformed message */
    memset( buffer, 0x00, TEST_MESG_LEN );
    puback.remainingLength = TEST_MESG_LEN;
    prvCreatePUBACKPacket( puback.pRemainingData, &puback.remainingLength, TEST_PACKET_IDENTIFIER, 1 );
    puback.pRemainingData[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializePuback( &puback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );

    /** No message id */
    memset( buffer, 0x00, TEST_MESG_LEN );
    puback.remainingLength = TEST_MESG_LEN;
    prvCreatePUBACKPacket( puback.pRemainingData, &puback.remainingLength, -1, 0 );
    status = IotBleMqtt_DeserializePuback( &puback );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_RESPONSE, status );
}

TEST( MQTT_Unit_BLE_Serialize, SerializeCONNECT_MallocFail )
{
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;

    connectInfo.clientIdentifierLength = strlen( TEST_CLIENTID );
    connectInfo.pClientIdentifier = TEST_CLIENTID;
    connectInfo.pUserName = TEST_USER_NAME;
    connectInfo.userNameLength = strlen( TEST_USER_NAME );
    connectInfo.pPassword = NULL;
    connectInfo.cleanSession = true;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 0 );
        status = IotBleMqtt_SerializeConnect( &connectInfo, ( uint8_t ** ) &pMesg, &bufLen );

        if( status != IOT_MQTT_SUCCESS )
        {
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializePUBLISH_MallocFail )
{
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    uint16_t packetIdentifier;

    publishInfo.qos = TEST_QOS1;
    publishInfo.pPayload = ( uint8_t * ) TEST_DATA;
    publishInfo.payloadLength = TEST_DATA_LENGTH;
    publishInfo.pTopicName = TEST_TOPIC;
    publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
    publishInfo.retain = false;
    publishInfo.retryMs = TEST_RETRY_MS;
    publishInfo.retryLimit = TEST_RETRY_LIMIT;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 1 );
        status = IotBleMqtt_SerializePublish( &publishInfo, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier, NULL );

        if( status != IOT_MQTT_SUCCESS )
        {
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}


TEST( MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE_MallocFail )
{
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    uint16_t packetIdentifier;

    subscription.qos = TEST_QOS1;
    subscription.pTopicFilter = TEST_TOPIC;
    subscription.topicFilterLength = TEST_TOPIC_LENGTH;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 1 );
        status = IotBleMqtt_SerializeSubscribe( &subscription, 1, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier );

        if( status != IOT_MQTT_SUCCESS )
        {
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializePUBACK_MallocFail )
{
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 0 );
        status = IotBleMqtt_SerializePuback( TEST_PACKET_IDENTIFIER, ( uint8_t ** ) &pMesg, &bufLen );

        if( status != IOT_MQTT_SUCCESS )
        {
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE_MallocFail )
{
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;
    uint16_t packetIdentifier;

    subscription.qos = TEST_QOS1;
    subscription.pTopicFilter = TEST_TOPIC;
    subscription.topicFilterLength = TEST_TOPIC_LENGTH;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 1 );
        status = IotBleMqtt_SerializeUnsubscribe( &subscription, 1, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier );

        if( status != IOT_MQTT_SUCCESS )
        {
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( MQTT_Unit_BLE_Serialize, SerializeDISCONNECT_MallocFail )
{
    volatile IotMqttError_t status;
    char * pMesg;
    size_t bufLen;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 0 );
        status = IotBleMqtt_SerializeDisconnect( ( uint8_t ** ) &pMesg, &bufLen );

        if( status != IOT_MQTT_SUCCESS )
        {
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}
