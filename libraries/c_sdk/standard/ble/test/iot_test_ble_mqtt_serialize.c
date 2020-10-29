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
#include <string.h>
#include "iot_config.h"

#include "FreeRTOS.h"


#include "iot_ble_config.h"
#include "iot_serializer.h"
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
#define TEST_PACKET_IDENTIFIER    5

#define TEST_MESG_LEN             512

TEST_GROUP( BLE_Unit_MQTT_Serialize );

/*-----------------------------------------------------------*/

TEST_SETUP( BLE_Unit_MQTT_Serialize )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( BLE_Unit_MQTT_Serialize )
{
}


TEST_GROUP_RUNNER( BLE_Unit_MQTT_Serialize )
{
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeCONNECT );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializePUBLISH );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializePUBLISH_QOS0 );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeSUBSCRIBE );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeUNSUBSCRIBE );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializePUBACK );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeDISCONNECT );

    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, DeserializeCONNACK );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, DeserializePUBLISH );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, DeserializePUBLISH_QOS0 );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, DeserializeSUBACK );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, DeserializePUBACK );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, DeserializeUNSUBACK );

    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeCONNECT_MallocFail );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializePUBLISH_MallocFail );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeSUBSCRIBE_MallocFail );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeUNSUBSCRIBE_MallocFail );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializePUBACK_MallocFail );
    RUN_TEST_CASE( BLE_Unit_MQTT_Serialize, SerializeDISCONNECT_MallocFail );
}

TEST( BLE_Unit_MQTT_Serialize, SerializeCONNECT )
{
    MQTTBLEConnectInfo_t connectInfo = { 0 };
    volatile MQTTBLEStatus_t status;
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
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
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

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializePUBLISH )
{
    MQTTBLEPublishInfo_t publishInfo = { 0 };
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier = 2;

    publishInfo.qos = TEST_QOS1;
    publishInfo.pPayload = ( uint8_t * ) TEST_DATA;
    publishInfo.payloadLength = TEST_DATA_LENGTH;
    publishInfo.pTopicName = TEST_TOPIC;
    publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
    publishInfo.retain = false;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializePublish( &publishInfo, ( uint8_t ** ) &pMesg, &bufLen, packetIdentifier );
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

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

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializePUBLISH_QOS0 )
{
    MQTTBLEPublishInfo_t publishInfo = { 0 };
    volatile MQTTBLEStatus_t status;
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

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializePublish( &publishInfo, ( uint8_t ** ) &pMesg, &bufLen, packetIdentifier );
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

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

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializeSUBSCRIBE )
{
    MQTTBLESubscribeInfo_t subscriptionList[ 2 ] = { 0 };
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier = 1;

    subscriptionList[ 0 ].qos = TEST_QOS1;
    subscriptionList[ 0 ].pTopicFilter = TEST_TOPIC;
    subscriptionList[ 0 ].topicFilterLength = TEST_TOPIC_LENGTH;
    subscriptionList[ 1 ].qos = TEST_QOS1;
    subscriptionList[ 1 ].pTopicFilter = TEST_TOPIC2;
    subscriptionList[ 1 ].topicFilterLength = TEST_TOPIC2_LENGTH;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeSubscribe( subscriptionList, 2, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier );
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

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

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializeUNSUBSCRIBE )
{
    MQTTBLESubscribeInfo_t subscriptionList[ 2 ] = { 0 };
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;
    uint16_t packetIdentifier = 1;

    subscriptionList[ 0 ].qos = TEST_QOS1;
    subscriptionList[ 0 ].pTopicFilter = TEST_TOPIC;
    subscriptionList[ 0 ].topicFilterLength = TEST_TOPIC_LENGTH;
    subscriptionList[ 1 ].qos = TEST_QOS1;
    subscriptionList[ 1 ].pTopicFilter = TEST_TOPIC2;
    subscriptionList[ 1 ].topicFilterLength = TEST_TOPIC2_LENGTH;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeUnsubscribe( subscriptionList, 2, ( uint8_t ** ) &pMesg, &bufLen, &packetIdentifier );
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
        TEST_ASSERT_NOT_NULL( pMesg );
        TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

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

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}


TEST( BLE_Unit_MQTT_Serialize, SerializePUBACK )
{
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializePuback( TEST_PACKET_IDENTIFIER, ( uint8_t ** ) &pMesg, &bufLen );
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
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

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializeDISCONNECT )
{
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;
    IotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    IotSerializerError_t xResult;

    if( TEST_PROTECT() )
    {
        status = IotBleMqtt_SerializeDisconnect( ( uint8_t ** ) &pMesg, &bufLen );
        TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
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

    if( status == MQTTBLESuccess )
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

TEST( BLE_Unit_MQTT_Serialize, DeserializeCONNACK )
{
    MQTTBLEStatus_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    size_t length = TEST_MESG_LEN;

    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_CONNECTED, 1 );

    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );

    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_CONNECTING, 1 );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );

    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_CONNECTION_REFUSED, 1 );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEServerRefused, status );

    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_DISCONNECTED, 1 );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEServerRefused, status );

    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_PROTOCOL_ERROR, 1 );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEServerRefused, status );

    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_UNKNOWN, 1 );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEServerRefused, status );

    /** Empty message **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** Tampered message **/
    prvCreateCONNACKPacket( buffer, &length, IOT_BLE_MQTT_STATUS_CONNECTED, 1 );
    buffer[ 0 ] = 0x00; /*Not valid cbor */
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** No response code */
    prvCreateCONNACKPacket( buffer, &length, -1, 0 );
    status = IotBleMqtt_DeserializeConnack( buffer, length );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );
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

TEST( BLE_Unit_MQTT_Serialize, DeserializePUBLISH )
{
    MQTTBLEStatus_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    size_t length = TEST_MESG_LEN;
    MQTTBLEPublishInfo_t publishInfo = { 0 };
    uint16_t packetIdentifier;


    prvCreatePUBLISHPacket( buffer,
                            &length,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            4 );

    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
    TEST_ASSERT_EQUAL_INT( TEST_TOPIC_LENGTH, publishInfo.topicNameLength );
    TEST_ASSERT_EQUAL( 0, strncmp( publishInfo.pTopicName, TEST_TOPIC, TEST_TOPIC_LENGTH ) );
    TEST_ASSERT_EQUAL_INT( TEST_DATA_LENGTH, publishInfo.payloadLength );
    TEST_ASSERT_EQUAL( 0, strncmp( publishInfo.pPayload, TEST_DATA, TEST_DATA_LENGTH ) );
    TEST_ASSERT_EQUAL( TEST_QOS1, publishInfo.qos );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packetIdentifier );
    TEST_ASSERT_EQUAL( false, publishInfo.retain );

    /** Malformed message **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( buffer,
                            &length,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            4 );
    buffer[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** NO QOS **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( buffer,
                            &length,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            -1,
                            TEST_PACKET_IDENTIFIER,
                            3 );
    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** NO Topic **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( buffer,
                            &length,
                            NULL,
                            0,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            3 );
    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** NO Packet id **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( buffer,
                            &length,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            TEST_DATA,
                            TEST_DATA_LENGTH,
                            TEST_QOS1,
                            -1,
                            3 );
    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** NO Payload **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBLISHPacket( buffer,
                            &length,
                            TEST_TOPIC,
                            TEST_TOPIC_LENGTH,
                            NULL,
                            0,
                            TEST_QOS1,
                            TEST_PACKET_IDENTIFIER,
                            3 );
    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );
}

TEST( BLE_Unit_MQTT_Serialize, DeserializePUBLISH_QOS0 )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    size_t length = TEST_MESG_LEN;
    MQTTBLEPublishInfo_t publishInfo = { 0 };
    uint16_t packetIdentifier;


    prvCreatePUBLISHPacket(
        buffer,
        &length,
        TEST_TOPIC,
        TEST_TOPIC_LENGTH,
        TEST_DATA,
        TEST_DATA_LENGTH,
        TEST_QOS0,
        -1,
        3 );

    status = IotBleMqtt_DeserializePublish( buffer, length, &publishInfo, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
    TEST_ASSERT_EQUAL_INT( TEST_TOPIC_LENGTH, publishInfo.topicNameLength );
    TEST_ASSERT_EQUAL( 0, strncmp( publishInfo.pTopicName, TEST_TOPIC, TEST_TOPIC_LENGTH ) );
    TEST_ASSERT_EQUAL_INT( TEST_DATA_LENGTH, publishInfo.payloadLength );
    TEST_ASSERT_EQUAL( 0, strncmp( publishInfo.pPayload, TEST_DATA, TEST_DATA_LENGTH ) );
    TEST_ASSERT_EQUAL( TEST_QOS0, publishInfo.qos );
    TEST_ASSERT_EQUAL( false, publishInfo.retain );
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

TEST( BLE_Unit_MQTT_Serialize, DeserializeSUBACK )
{
    MQTTBLEStatus_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    size_t length = TEST_MESG_LEN;
    uint16_t packetIdentifier;
    uint8_t statusCode;

    prvCreateSUBACKPacket( buffer, &length, TEST_QOS1, TEST_PACKET_IDENTIFIER, 2 );

    status = IotBleMqtt_DeserializeSuback( buffer, length, &packetIdentifier, &statusCode );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packetIdentifier );
    TEST_ASSERT_EQUAL( TEST_QOS1, statusCode );

    /** Malformed message **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreateSUBACKPacket( buffer, &length, TEST_QOS1, TEST_PACKET_IDENTIFIER, 2 );
    buffer[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializeSuback( buffer, length, &packetIdentifier, &statusCode );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );


    /** No Status **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreateSUBACKPacket( buffer, &length, -1, TEST_PACKET_IDENTIFIER, 1 );
    status = IotBleMqtt_DeserializeSuback( buffer, length, &packetIdentifier, &statusCode );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** No packet id **/
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreateSUBACKPacket( buffer, &length, TEST_QOS1, -1, 1 );
    status = IotBleMqtt_DeserializeSuback( buffer, length, &packetIdentifier, &statusCode );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );
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


TEST( BLE_Unit_MQTT_Serialize, DeserializePUBACK )
{
    MQTTBLEStatus_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    size_t length = TEST_MESG_LEN;
    uint16_t packetIdentifier;

    prvCreatePUBACKPacket( buffer, &length, TEST_PACKET_IDENTIFIER, 1 );
    status = IotBleMqtt_DeserializePuback( buffer, length, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packetIdentifier );

    /** Malformed message */
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBACKPacket( buffer, &length, TEST_PACKET_IDENTIFIER, 1 );
    buffer[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializePuback( buffer, length, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** No message id */
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreatePUBACKPacket( buffer, &length, -1, 0 );
    status = IotBleMqtt_DeserializePuback( buffer, length, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );
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

TEST( BLE_Unit_MQTT_Serialize, DeserializeUNSUBACK )
{
    volatile MQTTBLEStatus_t status;
    uint8_t buffer[ TEST_MESG_LEN ] = { 0 };
    size_t length = TEST_MESG_LEN;
    uint16_t packetIdentifier;

    prvCreateUNSUBACKPacket( buffer, &length, TEST_PACKET_IDENTIFIER, 1 );
    status = IotBleMqtt_DeserializeUnsuback( buffer, length, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLESuccess, status );
    TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packetIdentifier );

    /** Malformed message */
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreateUNSUBACKPacket( buffer, &length, TEST_PACKET_IDENTIFIER, 1 );
    buffer[ 0 ] = 0x00;
    status = IotBleMqtt_DeserializeUnsuback( buffer, length, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );

    /** No message id */
    memset( buffer, 0x00, TEST_MESG_LEN );
    length = TEST_MESG_LEN;
    prvCreateUNSUBACKPacket( buffer, &length, -1, 0 );
    status = IotBleMqtt_DeserializeUnsuback( buffer, length, &packetIdentifier );
    TEST_ASSERT_EQUAL( MQTTBLEBadResponse, status );
}

TEST( BLE_Unit_MQTT_Serialize, SerializeCONNECT_MallocFail )
{
    MQTTBLEConnectInfo_t connectInfo = { 0 };
    volatile MQTTBLEStatus_t status;
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

        if( status != MQTTBLESuccess )
        {
            TEST_ASSERT_EQUAL( MQTTBLENoMemory, status );
        }
    }

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializePUBLISH_MallocFail )
{
    MQTTBLEPublishInfo_t publishInfo = { 0 };
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;
    uint16_t packetIdentifier = 1;

    publishInfo.qos = TEST_QOS1;
    publishInfo.pPayload = ( uint8_t * ) TEST_DATA;
    publishInfo.payloadLength = TEST_DATA_LENGTH;
    publishInfo.pTopicName = TEST_TOPIC;
    publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
    publishInfo.retain = false;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 1 );
        status = IotBleMqtt_SerializePublish( &publishInfo, ( uint8_t ** ) &pMesg, &bufLen, packetIdentifier );

        if( status != MQTTBLESuccess )
        {
            TEST_ASSERT_EQUAL( MQTTBLENoMemory, status );
        }
    }

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}


TEST( BLE_Unit_MQTT_Serialize, SerializeSUBSCRIBE_MallocFail )
{
    MQTTBLESubscribeInfo_t subscription = { 0 };
    volatile MQTTBLEStatus_t status;
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

        if( status != MQTTBLESuccess )
        {
            TEST_ASSERT_EQUAL( MQTTBLENoMemory, status );
        }
    }

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializePUBACK_MallocFail )
{
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 0 );
        status = IotBleMqtt_SerializePuback( TEST_PACKET_IDENTIFIER, ( uint8_t ** ) &pMesg, &bufLen );

        if( status != MQTTBLESuccess )
        {
            TEST_ASSERT_EQUAL( MQTTBLENoMemory, status );
        }
    }

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializeUNSUBSCRIBE_MallocFail )
{
    MQTTBLESubscribeInfo_t subscription = { 0 };
    volatile MQTTBLEStatus_t status;
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

        if( status != MQTTBLESuccess )
        {
            TEST_ASSERT_EQUAL( MQTTBLENoMemory, status );
        }
    }

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}

TEST( BLE_Unit_MQTT_Serialize, SerializeDISCONNECT_MallocFail )
{
    volatile MQTTBLEStatus_t status;
    char * pMesg;
    size_t bufLen;

    if( TEST_PROTECT() )
    {
        UnityMalloc_MakeMallocFailAfterCount( 0 );
        status = IotBleMqtt_SerializeDisconnect( ( uint8_t ** ) &pMesg, &bufLen );

        if( status != MQTTBLESuccess )
        {
            TEST_ASSERT_EQUAL( MQTTBLENoMemory, status );
        }
    }

    if( status == MQTTBLESuccess )
    {
        IotBleMqtt_FreePacket( ( uint8_t * ) pMesg );
    }
}
