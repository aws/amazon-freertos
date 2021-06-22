/*
 * FreeRTOS BLE V2.2.0
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

#include <stdbool.h>
#include <unity.h>

#include "portableDefs.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "mock_stream_buffer.h"
#include "mock_portable.h"
#include "mock_iot_ble_data_transfer.h"
#include "mock_iot_ble_mqtt_serialize.h"
#include "mock_core_mqtt_serializer.h"


#include "iot_ble.h"
#include "iot_ble_config_defaults.h"
#include "iot_ble_mqtt_transport.h"

#define SIZE_OF_ACK_PACKET          4U
#define SIZE_OF_SUB_ACK_PACKET      5U
#define SIZE_OF_PING_RESP_PACKET    2U

#define _NUM_PUBACK_PARAMS          ( 2 )
#define _NUM_CONNACK_PARAMS         ( 2 )
#define _NUM_SUBACK_PARAMS          ( 4 )
#define _NUM_PUBLISH_PARAMS         ( 6 )
#define _NUM_UNSUBACK_PARAMS        ( 3 )
#define _NUM_DISCONNECT_PARAMS      ( 1 )
#define _NUM_PINGRESP_PARAMS        ( 1 )

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/ble/include/iot_ble_mqtt_transport.h.
 */
struct NetworkContext
{
    BleTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

static NetworkContext_t context;

/**
 * @brief Ble Transport Parameters structure to store the data channel.
 */
static BleTransportParams_t xBleTransportParams = { 0 };
static MQTTFixedBuffer_t fixedBuffer;
static size_t bufferSize = 0;

/*-----------------------------------------------------------*/

uint8_t buffer[ 100 ];
void setUp( void )
{
    fixedBuffer.pBuffer = buffer;
    fixedBuffer.size = 100;
    context.pParams = &xBleTransportParams;
}

/* called before each testcase */
void tearDown( void )
{
    context.pParams->pChannel = NULL;
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}


/*******************************************************************************
 * IotBleMqttTransportInit
 ******************************************************************************/

/**
 *  @brief Init the streambuffer successfully
 *  @details Well formatted init, use any non null stream buffer handle to mock out completion
 */
void test_IotBleMqttTransportInit_success( void )
{
    bool ret = false;
    StreamBufferHandle_t testHandle = { 10 };
    uint8_t buffer[ 10 ] = { 0 };

    xStreamBufferGenericCreateStatic_IgnoreAndReturn( testHandle );
    ret = IotBleMqttTransportInit( buffer, 10, &context );

    TEST_ASSERT_TRUE( ret );
}

/**
 *  @brief TInit streambuffer unsuccessfully
 *  @details This should be tested in case the streambuffer initialization fails and it should return false
 */
void test_IotBleMqttTransportInit_fail( void )
{
    bool ret = false;
    uint8_t buffer[ 10 ] = { 0 };

    xStreamBufferGenericCreateStatic_IgnoreAndReturn( NULL );
    ret = IotBleMqttTransportInit( buffer, 10, &context );

    TEST_ASSERT_FALSE( ret );
}


/*******************************************************************************
 * IotBleMqttTransportSend (outgoing packets)
 ******************************************************************************/

/* Connect Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Connect Basic Test ----- */

/**
 * @brief Sends a connect packet with just client identifier
 * @details Well formatted with just the minimum requirements (client ID)
 */
MQTTBLEStatus_t basicConnectCallback( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                      uint8_t ** const pConnectPacket,
                                      size_t * const pPacketSize,
                                      int num_calls )
{
    TEST_ASSERT_TRUE( pConnectInfo->cleanSession );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pConnectInfo->pClientIdentifier, "Connectbasic", 12 ) );
    TEST_ASSERT_EQUAL_INT( 12U, pConnectInfo->clientIdentifierLength );
    TEST_ASSERT_EQUAL_INT( 10U, pConnectInfo->keepAliveSeconds );

    *pPacketSize = 10U;
    return MQTTBLESuccess;
}

/**
 *  IOT_BLE_MQTT_MSG_TYPE_CONNECT
 */
void test_IotBleMqttTransportSend_ConnectBasic( void )
{
    uint8_t MQTTPacket[] =
    {
        0x10, 0x18, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0x02,
        0x00, 0x0a, 0x00, 0x0c, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63,
        0x74, 0x62, 0x61, 0x73, 0x69, 0x63
    };
    size_t bytesSent = 0;
    size_t packetSize = 26U;

    IotBleMqtt_SerializeConnect_Stub( basicConnectCallback );
    IotBleDataTransfer_Send_IgnoreAndReturn( 10U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( bytesSent, packetSize );
}
/* ----- End Connect Basic Test ----- */


/* ----- Begin Connect Credential Test ----- */

/**
 * @brief Sends a connect packet with username and password
 * @details Well formatted with username and password
 */
MQTTBLEStatus_t credentialConnectCallback( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                           uint8_t ** const pConnectPacket,
                                           size_t * const pPacketSize,
                                           int num_calls )
{
    TEST_ASSERT_TRUE( pConnectInfo->cleanSession );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pConnectInfo->pClientIdentifier, "Connectcredentials", 18 ) );
    TEST_ASSERT_EQUAL_INT( 18U, pConnectInfo->clientIdentifierLength );
    TEST_ASSERT_EQUAL_INT( 10U, pConnectInfo->keepAliveSeconds );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pConnectInfo->pUserName, "abc", 3U ) );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pConnectInfo->pPassword, "012", 3U ) );
    TEST_ASSERT_EQUAL_INT( 3U, pConnectInfo->passwordLength );
    TEST_ASSERT_EQUAL_INT( 3U, pConnectInfo->userNameLength );

    *pPacketSize = 10U;
    return MQTTBLESuccess;
}

void test_IotBleMqttTransportSend_ConnectCredentials( void )
{
    /*IOT_BLE_MQTT_MSG_TYPE_CONNECT */
    uint8_t MQTTPacket[] =
    {
        0x10, 0x2a, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0xc2,
        0x00, 0x0a, 0x00, 0x12, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63,
        0x74, 0x63, 0x72, 0x65, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x61,
        0x6c, 0x73, 0x00, 0x03, 0x61, 0x62, 0x63, 0x00, 0x03, 0x30,
        0x31, 0x32
    };
    size_t bytesSent = 0;
    size_t packetSize = 42U;

    IotBleMqtt_SerializeConnect_Stub( credentialConnectCallback );
    IotBleDataTransfer_Send_IgnoreAndReturn( 10U );
    vPortFree_Ignore();
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( bytesSent, packetSize );
}
/* ----- End Connect Credential Test ----- */


/* ----- Begin Connect Credential and Will Test ----- */

/**
 * @brief Sends a connect packet with username and password and will
 * @details Well formatted with username and password and last will.  Will is not used at
 *          this time, so we can just call the credentials callback
 */
void test_IotBleMqttTransportSend_ConnectCredentialsAndWill( void )
{
    uint8_t MQTTPacket[] =
    {
        0x10, 0x34, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0xce,
        0x00, 0x0a, 0x00, 0x12, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63,
        0x74, 0x63, 0x72, 0x65, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x61,
        0x6c, 0x73, 0x00, 0x03, 0x68, 0x69, 0x70, 0x00, 0x03, 0x78,
        0x79, 0x80, 0x00, 0x03, 0x61, 0x62, 0x63, 0x00, 0x03, 0x30,
        0x31, 0x32
    };
    size_t bytesSent = 0;
    size_t packetSize = 52U;

    IotBleMqtt_SerializeConnect_Stub( credentialConnectCallback );
    IotBleDataTransfer_Send_IgnoreAndReturn( 10U );
    vPortFree_Ignore();
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( bytesSent, packetSize );
}
/* ----- End Connect Credential and Will Test ----- */


/* ----- Begin Connect Credential and Will Bad Test ----- */

/**
 * @brief Sends a connect packet with bad will topic
 * @details If the will topic is empty, but the flag is set, it should not send
 */
void test_IotBleMqttTransportSend_ConnectCredentialsAndWillBad( void )
{
    uint8_t MQTTPacket[] =
    {
        0x10, 0x33, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0xce,
        0x00, 0x0a, 0x00, 0x12, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63,
        0x74, 0x63, 0x72, 0x65, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x61,
        0x6c, 0x73, 0x00, 0x00, 0x68, 0x69, 0x70, 0x00, 0x00, 0x78,
        0x79, 0x80, 0x03, 0x61, 0x62, 0x63, 0x00, 0x03, 0x30, 0x31,
        0x32
    };
    size_t bytesSent = 0;
    size_t packetSize = 51U;

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Connect Credential and Will Bad Test ----- */


/* ----- Begin Connect Credentials Bad Username Test ----- */

/**
 * @brief Sends a connect packet with bad username
 * @details An empty username with the flag set should not send
 */
void test_IotBleMqttTransportSend_ConnectCredentialsBadUsername( void )
{
    uint8_t MQTTPacket[] =
    {
        0x10, 0x2a, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0xc2,
        0x00, 0x0a, 0x00, 0x12, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63,
        0x74, 0x63, 0x72, 0x65, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x61,
        0x6c, 0x73, 0x00, 0x00, 0x61, 0x62, 0x63, 0x00, 0x03, 0x30,
        0x31, 0x32
    };
    size_t bytesSent = 0;
    size_t packetSize = 42U;

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Connect Credentials Bad Username Test ----- */


/* ----- Begin Connect Credentials Bad Password Test ----- */

/**
 * @brief Sends a connect packet with bad password
 * @details An empty password with the flag set should not send
 */
void test_IotBleMqttTransportSend_ConnectCredentialsBadPassword( void )
{
    uint8_t MQTTPacket1[] =
    {
        0x10, 0x2a, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0xc2,
        0x00, 0x0a, 0x00, 0x12, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63,
        0x74, 0x63, 0x72, 0x65, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x61,
        0x6c, 0x73, 0x00, 0x03, 0x61, 0x62, 0x63, 0x00, 0x00, 0x30,
        0x31, 0x32
    };
    size_t bytesSent = 0;
    size_t packetSize = 42U;

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket1, packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Connect Credentials Bad Password Test ----- */


/* ----- Begin Connect Bad parameters Test ----- */

/**
 * @brief Tests for bad parameters in connect packets
 * @details Tests for bad protocol, will Qos, reserved bit in flag byte,
 *          no client id, in that order
 */
void test_IotBleMqttTransportSend_ConnectBadParameters( void )
{
    size_t bytesSent = 0;
    size_t packetSize = 12U;

    /* Test for bad protocol level IOT_BLE_MQTT_MSG_TYPE_CONNECT */
    uint8_t MQTTPacketBadProtocol[] =
    {
        0x10, 0x18, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x06, 0x01,
        0x00, 0x00
    };

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacketBadProtocol,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}

void test_IotBleMqttTransportSend_ConnectBadParameters2( void )
{
    size_t bytesSent = 0;
    size_t packetSize = 12U;
    /* Test for too high will Qos */
    uint8_t MQTTPacketWillQos[] =
    {
        0x10, 0x18, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0x18,
        0x00, 0x00
    };

    IotBleMqtt_SerializeConnect_IgnoreAndReturn( MQTTBLEBadParameter );

    packetSize = 12U;
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacketWillQos,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}

void test_IotBleMqttTransportSend_ConnectBadParametersr3( void )
{
    size_t bytesSent = 0;
    size_t packetSize = 12U;

    /* Test for bad connect flag byte */
    uint8_t MQTTPacketConnectFlag[] =
    {
        0x10, 0x18, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0x11,
        0x00, 0x00
    };

    packetSize = 12U;
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacketConnectFlag,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}

void test_IotBleMqttTransportSend_ConnectBadParametersr4( void )
{
    size_t bytesSent = 0;
    size_t packetSize = 12U;

    /* Test for no clientId */
    uint8_t MQTTPacketNoID[] =
    {
        0x10, 0x18, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0x02,
        0x00, 0x00, 0x00, 0x00
    };

    packetSize = 12U;
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacketNoID,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Connect Bad parameters Test ----- */


/* Publish Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Publish Basic Test ----- */

/**
 * @brief Sends a Publish packet -- IOT_BLE_MQTT_MSG_TYPE_PUBLISH --
 * @details Well formatted with Qos 0
 */
void test_IotBleMqttTransportSend_PublishBasic( void )
{
    size_t bytesSent = 0;

    context.pParams->publishInfo.topicNameLength = 20;
    char buffer[ context.pParams->publishInfo.topicNameLength ];

    uint8_t MQTTPacket[] =
    {
        0x30, 0x24, 0x00, 0x11, 0x69, 0x6f, 0x74, 0x64, 0x65, 0x6d,
        0x6f, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x2f, 0x61, 0x62, 0x63,
        0x32, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x4d, 0x51, 0x54,
        0x54, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00
    };
    size_t packetSize = 38U;

    IotBleMqtt_SerializePublish_ExpectAnyArgsAndReturn( MQTTBLESuccess );
    IotBleMqtt_SerializePublish_ReturnThruPtr_pPacketSize( &packetSize );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 38 );

    vPortFree_Ignore();
    pvPortMalloc_IgnoreAndReturn( buffer );

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 38U, bytesSent );
}
/* ----- End Publish Basic Test ----- */


/* ----- Begin Publish Bad Test ----- */

/**
 * @brief Sends a Publish packet with bad parameters
 * @details Mock out a non MQTTSuccess response from deserialize
 *          This should not call IotBleDataTransfer_Send
 */
void test_IotBleMqttTransportSend_PublishBadDeserialize( void )
{
    size_t bytesSent = 0;

    context.pParams->publishInfo.topicNameLength = 20;
    char buffer[ context.pParams->publishInfo.topicNameLength ];
    uint8_t MQTTPacket[] =
    {
        0x30, 0x24, 0x00, 0x11, 0x69, 0x6f, 0x74, 0x64, 0x65, 0x6d,
        0x6f, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x2f, 0x61, 0x62, 0x63,
        0x32, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x4d, 0x51, 0x54,
        0x54, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21
    };
    size_t packetSize = 38U;

    MQTT_DeserializePublish_IgnoreAndReturn( MQTTBadParameter );
    IotBleMqtt_SerializePublish_IgnoreAndReturn( MQTTBLEBadParameter );
    vPortFree_Ignore();
    pvPortMalloc_IgnoreAndReturn( buffer );

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Publish Bad Test ----- */

/* Outgoing PubAck Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Puback Basic Test ----- */

/**
 * @brief Sends a Puback packet
 * @details Well formatted with a valid packet
 */
void test_IotBleMqttTransportSend_Puback( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0x40, 0x02, 0x00, 0x01 };
    size_t packetSize = 4U;

    MQTT_DeserializeAck_IgnoreAndReturn( MQTTSuccess );
    IotBleMqtt_SerializePuback_ExpectAnyArgsAndReturn( MQTTBLESuccess );
    IotBleMqtt_SerializePuback_ReturnThruPtr_pPacketSize( &packetSize );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 4U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}
/* ----- End Puback Basic Test ----- */


/* ----- Begin Puback Bad Test ----- */

/**
 * @brief Sends a Puback packet
 * @details Mock out a non MQTTSuccess response from deserialize
 *          This should not call IotBleDataTransfer_Send
 */
void test_IotBleMqttTransportSend_PubackBadDeserialize( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0x40, 0x02, 0x00, 0x01 };
    size_t packetSize = 4U;

    MQTT_DeserializeAck_IgnoreAndReturn( MQTTBadParameter );
    /* IotBleMqtt_DeserializePuback_Stub( forgePacketIdentifierGood ); */
    IotBleMqtt_SerializePuback_IgnoreAndReturn( MQTTBLEBadParameter );

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Puback Bad Test ----- */

/* Outgoing Subscribe Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Subscribe Basic Test ----- */

/**
 * @brief Sends a subscribe packet with one topic with QoS 0
 * @details Well formatted subscribe with minimum requirements
 */
MQTTBLEStatus_t basicSubscribeCallback( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                        size_t subscriptionCount,
                                        uint8_t ** const pSubscribePacket,
                                        size_t * const pPacketSize,
                                        uint16_t * const pPacketIdentifier,
                                        int num_calls )
{
    TEST_ASSERT_EQUAL_INT( 1, subscriptionCount );
    TEST_ASSERT_EQUAL_INT( MQTTBLEQoS0, pSubscriptionList->qos );
    TEST_ASSERT_EQUAL_INT( 0x0e, pSubscriptionList->topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList->pTopicFilter, "basicSubscribe", 0x0e ) );

    *pPacketSize = 1U;
    return MQTTBLESuccess;
}

void test_IotBleMqttTransportSend_SubscribeBasic( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] =
    {
        0x82, 0x13, 0x00, 0x01, 0x00, 0x0e, 0x62, 0x61, 0x73, 0x69,
        0x63, 0x53, 0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65,
        0x0
    };
    size_t packetSize = 21;

    IotBleMqtt_SerializeSubscribe_Stub( basicSubscribeCallback );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 1U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}
/* ----- End Subscribe Basic Test ----- */


/* ----- Begin Subscribe Many Test ----- */

/**
 * @brief Sends a subscribe packet with two topics with QoS 0
 * @details Well formatted subscribe with multiple subscription requests
 */
MQTTBLEStatus_t multiSubscribeCallback( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                        size_t subscriptionCount,
                                        uint8_t ** const pSubscribePacket,
                                        size_t * const pPacketSize,
                                        uint16_t * const pPacketIdentifier,
                                        int num_calls )
{
    TEST_ASSERT_EQUAL_INT( 2, subscriptionCount );
    TEST_ASSERT_EQUAL_INT( MQTTBLEQoS0, pSubscriptionList[ 0 ].qos );
    TEST_ASSERT_EQUAL_INT( 0x0f, pSubscriptionList[ 0 ].topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList[ 0 ].pTopicFilter, "multiSubscribe0", 0x0f ) );

    TEST_ASSERT_EQUAL_INT( MQTTBLEQoS0, pSubscriptionList[ 1 ].qos );
    TEST_ASSERT_EQUAL_INT( 0x0f, pSubscriptionList[ 1 ].topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList[ 1 ].pTopicFilter, "multiSubscribe1", 0x0f ) );

    *pPacketSize = 1U;
    return MQTTBLESuccess;
}

void test_IotBleMqttTransportSend_SubscribeMulti( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] =
    {
        0x82, 0x26, 0x00, 0x3b, 0x00, 0x0f, 0x6d, 0x75, 0x6c, 0x74,
        0x69, 0x53, 0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65,
        0x30, 0x00, 0x00, 0x0f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x53,
        0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65, 0x31, 0x00
    };
    size_t packetSize = 40;

    IotBleMqtt_SerializeSubscribe_Stub( multiSubscribeCallback );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 1U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}

/* ----- End Subscribe Many Test ----- */

/* ----- Begin Subscribe Qos Test ----- */

/**
 * @brief Sends a subscribe packet with two topics with QoS 1
 * @details Well formatted subscribe with multiple subscription requests
 */
MQTTBLEStatus_t multiQosSubscribeCallback( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                           size_t subscriptionCount,
                                           uint8_t ** const pSubscribePacket,
                                           size_t * const pPacketSize,
                                           uint16_t * const pPacketIdentifier,
                                           int num_calls )
{
    TEST_ASSERT_EQUAL_INT( 2, subscriptionCount );
    TEST_ASSERT_EQUAL_INT( MQTTBLEQoS1, pSubscriptionList[ 0 ].qos );
    TEST_ASSERT_EQUAL_INT( 0x0f, pSubscriptionList[ 0 ].topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList[ 0 ].pTopicFilter, "multiSubscribe0", 0x0f ) );

    TEST_ASSERT_EQUAL_INT( MQTTBLEQoS1, pSubscriptionList[ 1 ].qos );
    TEST_ASSERT_EQUAL_INT( 0x0f, pSubscriptionList[ 1 ].topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList[ 1 ].pTopicFilter, "multiSubscribe1", 0x0f ) );
    *pPacketSize = 1U;
    return MQTTBLESuccess;
}

void test_IotBleMqttTransportSend_SubscribeMultiQos( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] =
    {
        0x82, 0x26, 0x00, 0x3b, 0x00, 0x0f, 0x6d, 0x75, 0x6c, 0x74,
        0x69, 0x53, 0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65,
        0x30, 0x01, 0x00, 0x0f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x53,
        0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65, 0x31, 0x01
    };
    size_t packetSize = 39;

    IotBleMqtt_SerializeSubscribe_Stub( multiQosSubscribeCallback );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 1U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}
/* ----- End Subscribe Qos Test ----- */


/* ----- Begin Subscribe Bad Test ----- */

/**
 * @brief Sends a badly formatted subscribe packet
 * @details Subscribe packet with no payload
 */
void test_IotBleMqttTransportSend_SubscribeBad( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0x82, 0x02, 0x00, 0x00 };
    size_t packetSize = 4;

    vPortFree_Ignore();

    /* No Payload */
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Subscribe Bad Test ----- */


/* Outgoing Unsubscribe Packets */
/*-----------------------------------------------------------*/
/* ----- Begin Basic Unsubscribe Test ----- */

/**
 * @brief Sends an unsubscribe packet with two topics
 * @details Well formatted unsubscribe with multiple subscription requests
 *          Subscribe and unsubscribe use the same code
 */
MQTTBLEStatus_t unsubscribeCallback( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                     size_t subscriptionCount,
                                     uint8_t ** const pSubscribePacket,
                                     size_t * const pPacketSize,
                                     uint16_t * const pPacketIdentifier,
                                     int num_calls )
{
    TEST_ASSERT_EQUAL_INT( 2, subscriptionCount );
    TEST_ASSERT_EQUAL_INT( 0x0f, pSubscriptionList[ 0 ].topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList[ 0 ].pTopicFilter, "multiSubscribe0", 0x0f ) );

    TEST_ASSERT_EQUAL_INT( 0x0f, pSubscriptionList[ 1 ].topicFilterLength );
    TEST_ASSERT_EQUAL_INT( 0, strncmp( pSubscriptionList[ 1 ].pTopicFilter, "multiSubscribe1", 0x0f ) );

    *pPacketSize = 1U;
    return MQTTBLESuccess;
}

void test_IotBleMqttTransportSend_Unsubscribe( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] =
    {
        0xa2, 0x24, 0x00, 0x3b, 0x00, 0x0f, 0x6d, 0x75, 0x6c, 0x74,
        0x69, 0x53, 0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65,
        0x30, 0x00, 0x0f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x53,
        0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65, 0x31
    };
    size_t packetSize = 37;

    IotBleMqtt_SerializeUnsubscribe_Stub( unsubscribeCallback );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 1U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}
/* ----- End Basic Unsubscribe Test ----- */


/* ----- Begin Bad Unsubscribe Test ----- */

/**
 * @brief Sends a badly formatted unsubscribe packet
 * @details Unsubscribe requires a payload, and this has none.
 *          Subscribe and unsubscribe use the same transport layer code
 */
void test_IotBleMqttTransportSend_UnsubscribeBad( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xa2, 0x02, 0x00, 0x00 };
    size_t packetSize = 4;

    vPortFree_Ignore();

    /* No Payload */
    bytesSent = ( size_t ) IotBleMqttTransportSend( &context, ( void * ) MQTTPacket, packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Bad Unsubscribe Test ----- */


/* Outgoing Pingreq Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Ping Request Simple Test ----- */

/**
 * @brief Sends a valid ping request packet
 * @details Well formatted
 */
void test_IotBleMqttTransportSend_PingReq( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xc0, 0x00 };
    size_t packetSize = 2U;

    IotBleMqtt_SerializePingreq_ExpectAnyArgsAndReturn( MQTTBLESuccess );
    IotBleMqtt_SerializePingreq_ReturnThruPtr_pPacketSize( &packetSize );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 2U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}
/* ----- End Ping Request Simple Test ----- */

/* ----- Begin Ping Request Bad Test ----- */

/**
 * @brief Sends a bad ping request packet
 * @details Mock out a non MQTTSuccess response from serialize,
 *          and make sure nothing sends
 *          This should not call IotBleDataTransferSend
 */
void test_IotBleMqttTransportSend_PingReqBadSerialize( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xc0, 0x00 };
    size_t packetSize = 2U;

    IotBleMqtt_SerializePingreq_IgnoreAndReturn( MQTTBLENoMemory );

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Ping Request Bad Test ----- */

/* Outgoing Disconnect Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Disconnect Simple Test ----- */

/**
 * @brief Sends a valid disconnect packet
 * @details Well formatted
 */
void test_IotBleMqttTransportSend_Disconnect( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xe0, 0x00 }; /* IOT_BLE_MQTT_MSG_TYPE_DISCONNECT */
    size_t packetSize = 2U;

    IotBleMqtt_SerializeDisconnect_ExpectAnyArgsAndReturn( MQTTBLESuccess );
    IotBleMqtt_SerializeDisconnect_ReturnThruPtr_pPacketSize( &packetSize );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 2U );
    vPortFree_Ignore();

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( packetSize, bytesSent );
}
/* ----- End Disconnect Simple Test ----- */

/* ----- Begin Disconnect Bad Test ----- */

/**
 * @brief Sends a valid disconnect packet
 * @details Mock out a non MQTTSuccess response from serialize,
 *          and make sure nothing sends
 *          This should not call IotBleDataTransfer_Send
 */
void test_IotBleMqttTransportSend_DisconnectBadSerialize( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xe0, 0x00 }; /* IOT_BLE_MQTT_MSG_TYPE_DISCONNECT */
    size_t packetSize = 2U;

    IotBleMqtt_SerializeDisconnect_IgnoreAndReturn( MQTTBLENoMemory );

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Disconnect Bad Test ----- */


/* Channel Failure */
/*-----------------------------------------------------------*/

/* ----- Begin Channel Failure Test ----- */

/**
 * @brief Attempts to send a packet but fails due to send failure
 * @details Mock out send to fail and make sure nothing is sent
 */
void test_IotBleMqttTransportSend_ChannelFails( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xc0, 0x00 }; /* IOT_BLE_MQTT_MSG_TYPE_PINGREQ */
    size_t packetSize = 2U;

    IotBleMqtt_SerializePingreq_ExpectAnyArgsAndReturn( MQTTBLESuccess );
    IotBleMqtt_SerializePingreq_ReturnThruPtr_pPacketSize( &packetSize );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 0U );
    vPortFree_Ignore();
    context.pParams->publishInfo.pending = false;

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}

/**
 * @brief Attempts to send a packet but fails due to zero paket size
 * @details Mock out send to fail and make sure nothing is sent
 */
void test_IotBleMqttTransportSend_ChannelFails_packetSizeZero( void )
{
    size_t bytesSent = 0;
    uint8_t MQTTPacket[] = { 0xc0, 0x00 }; /* IOT_BLE_MQTT_MSG_TYPE_PINGREQ */
    size_t packetSize = 2U;
    size_t ret_packetSize = 2U;

    IotBleMqtt_SerializePingreq_ExpectAnyArgsAndReturn( MQTTBLESuccess );
    IotBleMqtt_SerializePingreq_ReturnThruPtr_pPacketSize( &ret_packetSize );
    IotBleDataTransfer_Send_ExpectAnyArgsAndReturn( 0 );
    vPortFree_Ignore();
    context.pParams->publishInfo.pending = false;

    bytesSent = ( size_t ) IotBleMqttTransportSend( &context,
                                                    ( void * ) MQTTPacket,
                                                    packetSize );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Channel Failure Test ----- */

/*******************************************************************************
 * IotBleMqttTransport Accept
 ******************************************************************************/

/**
 * @brief Callback for IotBleDataTransfer_PeekReceiveBuffer
 *        Sets the packet in the transport layer to whatever we assign pBuffer to
 */
void receiveCallback( IotBleDataTransferChannel_t * pChannel,
                      const uint8_t ** pBuffer,
                      size_t * pBufferLength,
                      int num_calls )
{
    *pBuffer = fixedBuffer.pBuffer;
    *pBufferLength = bufferSize;
}

/**
 * @brief Callback for deserialize acks, sets packetID to 1U (valid number)
 */
MQTTBLEStatus_t forgePacketIdentifierGood( uint8_t * pPuback,
                                           size_t length,
                                           uint16_t * packetIdentifier,
                                           int num_calls )
{
    *packetIdentifier = 1U;
    return MQTTBLESuccess;
}

MQTTBLEStatus_t forgePacketIdentifierGood2( const uint8_t * pBuffer,
                                            size_t length,
                                            uint16_t * packetIdentifier,
                                            uint8_t * pStatusCode,
                                            int num_calls )

{
    *packetIdentifier = 1U;
    *pStatusCode = MQTTBLESuccess;
    return MQTTBLESuccess;
}

/**
 * @brief Callback for deserialize acks, sets packetID to 0U (invalid)
 */
MQTTBLEStatus_t forgePacketIdentifierBad( uint8_t * pBuffer,
                                          size_t length,
                                          uint16_t * packetIdentifier,
                                          int num_calls )
{
    *packetIdentifier = 0U;
    return MQTTBLESuccess;
}

/**
 * @brief Callback for deserialize acks, sets packetID to 0U (invalid)
 */
MQTTBLEStatus_t forgePacketSubPackIdentifierBad( const uint8_t * pBuffer,
                                                 size_t length,
                                                 uint16_t * packetIdentifier,
                                                 uint8_t * pStatusCode,
                                                 int num_calls )
{
    *packetIdentifier = 0U;
    return MQTTBLESuccess;
}


/* Incoming Connack Packets */
/*-----------------------------------------------------------*/

/* ----- Begin Connack Simple Test ----- */

/**
 * @brief Accept a Connack packet
 * @details Well formatted
 */
size_t checkConnackPacket( StreamBufferHandle_t xStreamBuffer,
                           const void * pvTxData,
                           size_t xDataLengthBytes,
                           TickType_t xTicksToWait,
                           int num_calls )
{
    uint8_t * bytes = ( uint8_t * ) pvTxData;

    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_TYPE_CONNACK, bytes[ 0 ] );
    TEST_ASSERT_EQUAL_INT( 2U, bytes[ 1 ] );
    TEST_ASSERT_EQUAL_INT( 0U, bytes[ 2 ] );
    TEST_ASSERT_EQUAL_INT( 0U, bytes[ 3 ] );

    return 0U;
}

void test_IotBleMqttTransportAccept_Connack( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x73, 0x02, 0x61, 0x77, 0x02 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_CONNACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeConnack_IgnoreAndReturn( MQTTBLESuccess );
    xStreamBufferSend_Stub( checkConnackPacket );
    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U );
    vPortFree_Ignore();

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, status );
}
/* ----- End Connack Simple Test ----- */


/* ----- Begin Connack Bad Test ----- */

/**
 * @brief Attempt to accept an invalid Connack packet
 * @details Mock out a non MQTTSuccess response from deserialize
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_ConnackBadDeserialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x73, 0x02, 0x61, 0x77, 0x02 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_CONNACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeConnack_IgnoreAndReturn( MQTTBLENoMemory );
    xStreamBufferSend_IgnoreAndReturn( 0 );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLENoMemory, status );
}
/* ----- Begin Connack Bad Test ----- */


/* Incoming Puback Packets */
/*-----------------------------------------------------------*/
/* ----- Begin Puback Simple Test ----- */

/**
 * @brief Accept a Puback packet
 * @details Well formatted
 */
size_t checkPubackPacket( StreamBufferHandle_t xStreamBuffer,
                          const void * pvTxData,
                          size_t xDataLengthBytes,
                          TickType_t xTicksToWait,
                          int num_calls )
{
    uint8_t * bytes = ( uint8_t * ) pvTxData;

    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_TYPE_PUBACK, bytes[ 0 ] );
    TEST_ASSERT_EQUAL_INT( 2U, bytes[ 1 ] );
    TEST_ASSERT_EQUAL_INT( 0U, bytes[ 2 ] );
    TEST_ASSERT_EQUAL_INT( 1U, bytes[ 3 ] );

    return 0U;
}

void test_IotBleMqttTransportAccept_Puback( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa1, 0x61, 0x77, 0x04 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePuback_Stub( forgePacketIdentifierGood );
    xStreamBufferSend_Stub( checkPubackPacket );
    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, status );
}
/* ----- End Puback Simple Test ----- */

/* ----- Begin Puback Bad ID Test ----- */

/**
 * @brief Attempt to accept a Puback packet
 * @details Puback has bad ID (0) and should not accept the packet
 */
void test_IotBleMqttTransportAccept_PubackBadId( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa1, 0x61, 0x77, 0x04 };

    /*Since packet id is 0 it fails */
    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( MQTT_PACKET_TYPE_PUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePuback_Stub( forgePacketIdentifierBad );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
}
/* ----- End Puback Bad Id Test ----- */


/* ----- Begin Puback Bad Deserialize Test ----- */

/**
 * @brief Attempt to accept a Puback packet
 * @details Mock out a non MQTTSuccess response from deserialize
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_PubackBadDeserialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa1, 0x61, 0x77, 0x04 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePuback_IgnoreAndReturn( MQTTBLENoMemory );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLENoMemory, status );
}
/* ----- End Puback Bad Deserialize Test ----- */


/* Incoming Publish Packets */
/*-----------------------------------------------------------*/
/* ----- Begin Publish Simple Test ----- */

/**
 * @brief Accept a Publish packet
 * @details Well formatted
 */
MQTTBLEStatus_t incomingPublishCallback( uint8_t * pBuffer,
                                         size_t length,
                                         MQTTBLEPublishInfo_t * publishInfo,
                                         uint16_t * packetIdentifier,
                                         int num_calls )
{
    publishInfo->qos = MQTTBLEQoS0;
    publishInfo->pTopicName = "test/123";
    publishInfo->topicNameLength = 8U;
    publishInfo->pPayload = "testing, testing";
    publishInfo->payloadLength = 16U;
    return MQTTBLESuccess;
}

MQTTStatus_t changePacketSize( const MQTTPublishInfo_t * pPublishInfo,
                               size_t * pRemainingLength,
                               size_t * pPacketSize,
                               int num_calls )
{
    *pPacketSize = 5U;
    return MQTTSuccess;
}

void test_IotBleMqttTransportAccept_Publish( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] =
    {
        0xa5, 0x61, 0x69, 0x18, 0x3b, 0x61, 0x6b, 0x51, 0x48, 0x65,
        0x6c, 0x6c, 0x6f, 0x20, 0x4d, 0x51, 0x54, 0x54, 0x20, 0x57,
        0x6f, 0x72, 0x6c, 0x64, 0x21, 0x61, 0x6e, 0x00, 0x61, 0x75,
        0x71, 0x69, 0x6f, 0x74, 0x64, 0x65, 0x6d, 0x6f, 0x2f, 0x74,
        0x65, 0x73, 0x74, 0x2f, 0x61, 0x62, 0x63, 0x31, 0x61, 0x77,
        0x03
    };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBLISH );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );

    IotBleMqtt_DeserializePublish_Stub( incomingPublishCallback );
    MQTT_GetPublishPacketSize_Stub( changePacketSize );
    MQTT_SerializePublishHeader_IgnoreAndReturn( MQTTSuccess );
    xStreamBufferSend_IgnoreAndReturn( 0U );
    xStreamBufferSend_IgnoreAndReturn( 0U );
    xStreamBufferSend_IgnoreAndReturn( 0U );

    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
}
/* ----- End Publish Simple Test ----- */


/* ----- Begin Publish Qos Test ----- */

/**
 * @brief Accept a Publish packet
 * @details Well formatted with Qos1
 */
MQTTBLEStatus_t incomingPublishQosCallback( uint8_t * pPacket,
                                            size_t length,
                                            MQTTBLEPublishInfo_t * publishInfo,
                                            uint16_t * packetIdentifier,
                                            int num_calls )
{
    publishInfo->qos = MQTTBLEQoS0;
    publishInfo->pTopicName = "test/123";
    publishInfo->topicNameLength = 8U;
    publishInfo->pPayload = "testing, testing";
    publishInfo->payloadLength = 16U;
    return MQTTBLESuccess;
}

MQTTBLEStatus_t incomingPublishQosCallback2( uint8_t * pPacket,
                                             size_t length,
                                             MQTTBLEPublishInfo_t * publishInfo,
                                             uint16_t * packetIdentifier,
                                             int num_calls )
{
    publishInfo->qos = MQTTBLEQoS1;
    publishInfo->pTopicName = "test/123";
    publishInfo->topicNameLength = 8U;
    publishInfo->pPayload = "testing, testing";
    publishInfo->payloadLength = 16U;
    return MQTTBLESuccess;
}

void test_IotBleMqttTransportAccept_PublishQos( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] =
    {
        0xa5, 0x61, 0x69, 0x18, 0x3b, 0x61, 0x6b, 0x51, 0x48, 0x65,
        0x6c, 0x6c, 0x6f, 0x20, 0x4d, 0x51, 0x54, 0x54, 0x20, 0x57,
        0x6f, 0x72, 0x6c, 0x64, 0x21, 0x61, 0x6e, 0x01, 0x61, 0x75,
        0x71, 0x69, 0x6f, 0x74, 0x64, 0x65, 0x6d, 0x6f, 0x2f, 0x74,
        0x65, 0x73, 0x74, 0x2f, 0x61, 0x62, 0x63, 0x31, 0x61, 0x77,
        0x03
    };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBLISH );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );

    IotBleMqtt_DeserializePublish_Stub( incomingPublishQosCallback );
    MQTT_GetPublishPacketSize_Stub( changePacketSize );
    /*MQTT_SerializePublishHeader_IgnoreAndReturn( MQTTBLESuccess ); */
    xStreamBufferSend_IgnoreAndReturn( 0U );
    xStreamBufferSend_IgnoreAndReturn( 0U );
    xStreamBufferSend_IgnoreAndReturn( 0U );
    xStreamBufferSend_IgnoreAndReturn( 0U );
    IotBleDataTransfer_Receive_IgnoreAndReturn( 0 );

/*    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U ); */

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, status );
}
/* ----- End Publish Qos Test ----- */


/* ----- Begin Publish Bad Serialize Test ----- */

/**
 * @brief Attempt to accept a Publish packet
 * @details Mock out a non MQTTSuccess response from serializePublishHeader
 *          and make sure nothing is sent to the buffer
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_PublishBadSerialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] =
    {
        0xa5, 0x61, 0x69, 0x18, 0x3b, 0x61, 0x6b, 0x51, 0x48, 0x65,
        0x6c, 0x6c, 0x6f, 0x20, 0x4d, 0x51, 0x54, 0x54, 0x20, 0x57,
        0x6f, 0x72, 0x6c, 0x64, 0x21, 0x61, 0x6e, 0x01, 0x61, 0x75,
        0x71, 0x69, 0x6f, 0x74, 0x64, 0x65, 0x6d, 0x6f, 0x2f, 0x74,
        0x65, 0x73, 0x74, 0x2f, 0x61, 0x62, 0x63, 0x31, 0x61, 0x77,
        0x03
    };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBLISH );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePublish_Stub( incomingPublishQosCallback2 );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLEBadParameter, status );
}

/* ----- End Publish Qos Test ----- */


/* ----- Begin Publish Bad Deserialize Test ----- */

/**
 * @brief Attempt to accept a Publish packet
 * @details Mock out a non MQTTSuccess response from deserializer
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_PublishBadDeserialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] =
    {
        0xa5, 0x61, 0x69, 0x18, 0x3b, 0x61, 0x6b, 0x51, 0x48, 0x65,
        0x6c, 0x6c, 0x6f, 0x20, 0x4d, 0x51, 0x54, 0x54, 0x20, 0x57,
        0x6f, 0x72, 0x6c, 0x64, 0x21, 0x61, 0x6e, 0x01, 0x61, 0x75,
        0x71, 0x69, 0x6f, 0x74, 0x64, 0x65, 0x6d, 0x6f, 0x2f, 0x74,
        0x65, 0x73, 0x74, 0x2f, 0x61, 0x62, 0x63, 0x31, 0x61, 0x77,
        0x03
    };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBLISH );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePublish_IgnoreAndReturn( MQTTBLEBadResponse );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLEBadResponse, status );
}
/* ----- End Publish Bad Deserialize Test ----- */

/* Incoming Suback Packets */
/*-----------------------------------------------------------*/
/* ----- Begin Suback Simple Test ----- */

/**
 * @brief Accepts a suback packet
 * @details Well Formatted
 */
size_t checkSubackPacket( StreamBufferHandle_t xStreamBuffer,
                          const void * pvTxData,
                          size_t xDataLengthBytes,
                          TickType_t xTicksToWait,
                          int num_calls )
{
    uint8_t * bytes = ( uint8_t * ) pvTxData;

    TEST_ASSERT_EQUAL_INT( IOT_BLE_MQTT_MSG_TYPE_SUBACK << 4, bytes[ 0 ] ); /* ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_SUBACK ) */
    TEST_ASSERT_EQUAL_INT( 3U, bytes[ 1 ] );                                /* SUB_ACK_REMAINING_LENGTH */
    TEST_ASSERT_EQUAL_INT( 0U, bytes[ 2 ] );
    TEST_ASSERT_EQUAL_INT( 1U, bytes[ 3 ] );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, bytes[ 4 ] ); /*  statusCode */

    return 0U;
}

void test_IotBleMqttTransportAccept_Suback( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x69, 0x01, 0x61, 0x77, 0x09 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_SUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeSuback_Stub( forgePacketIdentifierGood2 );
    xStreamBufferSend_Stub( checkSubackPacket );
    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, status );
}
/* ----- End Suback Simple Test ----- */

/* ----- Begin Suback Bad Deserialize Test ----- */

/**
 * @brief Tries to accepts a suback packet
 * @details Mock out a non MQTTSuccess response from Deserializer
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_SubackBadDeserialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x69, 0x01, 0x61, 0x77, 0x09 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( MQTT_PACKET_TYPE_SUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeSuback_IgnoreAndReturn( MQTTBLEBadParameter );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
}
/* ----- End Suback Bad Deserialize Test ----- */

/* ----- Begin Suback Bad Packet ID Test ----- */

/**
 * @brief Tries to accepts a suback packet
 * @details Packet ID is 0 and should be thrown out
 */
void test_IotBleMqttTransportAccept_SubackBadID( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x69, 0x00, 0x61, 0x77, 0x09 };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( MQTT_PACKET_TYPE_SUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeSuback_Stub( forgePacketSubPackIdentifierBad );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLEBadParameter, status );
}
/* ----- End Suback Bad Packet ID Test ----- */

/* Incoming Unsuback Packets */
/*-----------------------------------------------------------*/
/* ----- Begin Unsuback Simple Test ----- */

/**
 * @brief Accepts an Unsuback packet
 * @details Well formatted
 */
size_t checkUnsubackPacket( StreamBufferHandle_t xStreamBuffer,
                            const void * pvTxData,
                            size_t xDataLengthBytes,
                            TickType_t xTicksToWait,
                            int num_calls )
{
    uint8_t * bytes = ( uint8_t * ) pvTxData;

    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_TYPE_UNSUBACK, bytes[ 0 ] );
    TEST_ASSERT_EQUAL_INT( 2U, bytes[ 1 ] );
    TEST_ASSERT_EQUAL_INT( 0U, bytes[ 2 ] );
    TEST_ASSERT_EQUAL_INT( 1U, bytes[ 3 ] );

    return 0U;
}


void test_IotBleMqttTransportAccept_Unsuback( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x69, 0x01, 0x61, 0x77, 0x0a };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_UNSUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeUnsuback_Stub( forgePacketIdentifierGood );
    xStreamBufferSend_Stub( checkUnsubackPacket );
    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, status );
}
/* ----- End Unsuback Simple Test ----- */

/* ----- Begin Unsuback Bad Deserialize Test ----- */

/**
 * @brief Attempts to accept an Unsuback packet
 * @details Mock out a non MQTTSuccess response from Deserializer
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_UnsubackBadDeserialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x69, 0x01, 0x61, 0x77, 0x0a };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_UNSUBACK );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializeUnsuback_IgnoreAndReturn( MQTTBLEBadParameter );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
}
/* ----- End Unsuback Bad Deserialize Test ----- */


/* Incoming Pingresp Packets */
/*-----------------------------------------------------------*/
/* ----- Begin PingResp Simple Test ----- */

/**
 * @brief Accepts an PingResp packet
 * @details Well formatted
 */
size_t checkPingRespPacket( StreamBufferHandle_t xStreamBuffer,
                            const void * pvTxData,
                            size_t xDataLengthBytes,
                            TickType_t xTicksToWait,
                            int num_calls )
{
    uint8_t * bytes = ( uint8_t * ) pvTxData;

    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_TYPE_PINGRESP, bytes[ 0 ] );
    TEST_ASSERT_EQUAL_INT( 0U, bytes[ 1 ] );

    return 0U;
}

void test_IotBleMqttTransportAccept_PingResp( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x77, 0x0d };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PINGRESP );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePingresp_IgnoreAndReturn( MQTTBLESuccess );
    xStreamBufferSend_Stub( checkPingRespPacket );
    IotBleDataTransfer_Receive_IgnoreAndReturn( 0U );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLESuccess, status );
}
/* ----- End PingResp Simple Test ----- */

/* ----- Begin PingResp Bad Deserialize Test ----- */

/**
 * @brief Attempts to accept a PINGRESP packet
 * @details Mock out a non MQTTSuccess response from Deserializer
 *          This should not call xStreamBufferSend
 */
void test_IotBleMqttTransportAccept_PingRespBadDeserialize( void )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buf[] = { 0xa2, 0x61, 0x77, 0x0d };

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PINGRESP );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );
    IotBleMqtt_DeserializePingresp_IgnoreAndReturn( MQTTBLEBadParameter );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
}
/* ----- End PingResp Bad Deserialize Test ----- */


/*******************************************************************************
 * Other functions and tests
 ******************************************************************************/

/* ----- Begin Receive Test ----- */

/**
 * @brief Accepts data from the circular buffer
 * @details Well formatted
 */
void test_IotBleMqttTransportReceive( void )
{
    uint32_t bytesReceived = 0;

    xStreamBufferReceive_IgnoreAndReturn( 1U );

    bytesReceived = IotBleMqttTransportReceive( &context, NULL, 1U );
    TEST_ASSERT_EQUAL_INT( 1U, bytesReceived );
}
/* ----- End Receive Test ----- */

/* ----- Begin Cleanup Test ----- */

/**
 * @brief Deletes the streambuffer created by init
 * @details Well formatted
 */
void test_IotBleMqttTransportCleanup( void )
{
    vStreamBufferDelete_ExpectAnyArgs();
    IotBleMqttTransportCleanup( &context );
}
/* ----- End Cleanup Test ----- */

/* ----- Begin Bad Types Test ----- */

/**
 * @brief Bad types that should not be recognized
 */
void test_IotBleMqttTransportReceive_Bad( void )
{
    /*Test for PUBREC being denied */
    uint8_t buf[] = { 0x01 };
    uint32_t bytesSent = 0;
    MQTTBLEStatus_t status = MQTTBLESuccess;

    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( IOT_BLE_MQTT_MSG_TYPE_PUBREC );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLEBadParameter, status );

    /*Test for bad type being denied */
    fixedBuffer.pBuffer = buf;
    IotBleMqtt_GetPacketType_IgnoreAndReturn( 0XF0 );
    IotBleDataTransfer_PeekReceiveBuffer_Stub( receiveCallback );

    status = IotBleMqttTransportAcceptData( &context );
    TEST_ASSERT_EQUAL_INT( MQTTBLEBadParameter, status );

    /*Test for pubrec being denied */
    buf[ 0 ] = 0x50;
    bytesSent = IotBleMqttTransportSend( &context, buf, 1 );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );

    /*Test for connack being denied (server to client only) */
    buf[ 0 ] = 0x20;
    bytesSent = IotBleMqttTransportSend( &context, buf, 1 );
    TEST_ASSERT_EQUAL_INT( 0, bytesSent );
}
/* ----- End Bad Types Test ----- */
