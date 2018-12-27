/*
 * Amazon FreeRTOS MQTT AFQP V1.1.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_iot_tests_mqtt_serialize_ble.c
 * @brief Tests for serialize/deserialize functions for MQTT BLE proxy
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include <string.h>
#include "private/aws_iot_mqtt_serialize_ble.h"
#include "aws_clientcredential.h"
#include "jsmn.h"

/* Test framework includes. */
#include "unity_fixture.h"


#define TEST_TOPIC                  "/topic/test"
#define TEST_TOPIC_LENGTH           11

#define TEST_TOPIC_ENCODED          "L3RvcGljL3Rlc3Q="
#define TEST_TOPIC_ENCODED_LENGTH   16

#define TEST_TOPIC2                  "/topic/test2"
#define TEST_TOPIC2_LENGTH           12

#define TEST_TOPIC2_ENCODED          "L3RvcGljL3Rlc3Qy"
#define TEST_TOPIC2_ENCODED_LENGTH   16

#define TEST_DATA                   "HelloWorld-Test"
#define TEST_DATA_LENGTH            15

#define TEST_DATA_ENCODED           "SGVsbG9Xb3JsZC1UZXN0"
#define TEST_DATA_ENCODED_LENGTH    20

#define TEST_CLIENTID               "clientID-test"
#define TEST_USER_NAME              "user-test"

#define TEST_QOS0                   0
#define TEST_QOS1                   1
#define TEST_RETRY_MS               0
#define TEST_RETRY_LIMIT            0
#define TEST_PACKET_IDENTIFIER      5

#define TEST_MALFORMED_MESG \
"{"\
"\"type\":"

#define TEST_CONNACK_MESG \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLESTATUS )":%d" \
"}"

#define TEST_CONNACK_MESG_LEN     ( sizeof( TEST_CONNACK_MESG ) )

#define TEST_CONNACK_MESG_NO_RESP_CODE \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d" \
"}"

#define TEST_PUBLISH_MESG \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLETOPIC )":"JSON_STR( TEST_TOPIC_ENCODED )"," \
JSON_STR( mqttBLEQOS )":"JSON_STR( TEST_QOS1 )"," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
JSON_STR( mqttBLEPAYLOAD )":"JSON_STR( TEST_DATA_ENCODED ) \
"}"

#define TEST_PUBLISH_MESG_LEN ( sizeof( TEST_PUBLISH_MESG ) )

#define TEST_PUBLISH_MESG_QOS0 \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLETOPIC )":"JSON_STR( TEST_TOPIC_ENCODED )"," \
JSON_STR( mqttBLEQOS )":"JSON_STR( TEST_QOS0 )"," \
JSON_STR( mqttBLEPAYLOAD )":"JSON_STR( TEST_DATA_ENCODED ) \
"}"

#define TEST_PUBLISH_MESG_NO_TOPIC \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLEQOS )":"JSON_STR( TEST_QOS1 )"," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
JSON_STR( mqttBLEPAYLOAD )":"JSON_STR( TEST_DATA_ENCODED ) \
"}"

#define TEST_PUBLISH_MESG_NO_QOS \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLETOPIC )":"JSON_STR( TEST_TOPIC_ENCODED )"," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
JSON_STR( mqttBLEPAYLOAD )":"JSON_STR( TEST_DATA_ENCODED ) \
"}"

#define TEST_PUBLISH_MESG_NO_DATA \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLETOPIC )":"JSON_STR( TEST_TOPIC_ENCODED )"," \
JSON_STR( mqttBLEQOS )":"JSON_STR( TEST_QOS1 )"," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
"}"

#define TEST_PUBLISH_MESG_NO_MSG_ID \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLETOPIC )":"JSON_STR( TEST_TOPIC_ENCODED )"," \
JSON_STR( mqttBLEQOS )":"JSON_STR( TEST_QOS1 )"," \
JSON_STR( mqttBLEPAYLOAD )":"JSON_STR( TEST_DATA_ENCODED ) \
"}"

#define TEST_SUBACK_MESG \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLESTATUS )":%d," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
"}"
#define TEST_SUBACK_MESG_LEN  ( sizeof( TEST_SUBACK_MESG ) )

#define TEST_SUBACK_MESG_NO_STATUS \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
"}"

#define TEST_SUBACK_MESG_NO_MSG_ID \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLESTATUS )":%d," \
"}"

#define TEST_PUBUNSUBACK_MESG \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
JSON_STR( mqttBLEMESSAGE_ID )":"JSON_STR( TEST_PACKET_IDENTIFIER )"," \
"}"
#define TEST_PUBUNSUBACK_MESG_LEN  ( sizeof( TEST_PUBUNSUBACK_MESG ) )

#define TEST_PUBUNSUBACK_MESG_NO_MSG_ID \
"{" \
JSON_STR( mqttBLEMSG_TYPE )":%d," \
"}"

/* Declaration of snprintf. The header stdio.h cannot be included because it
 * causes conflicts on some platforms. */
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );

static bool serializeInit;
static jsmn_parser jsmnParser;

uint16_t prvGetTokenIndex( const char *pMesg, jsmntok_t *tokens, int16_t numTokens, const char* pKey, size_t keyLen )
{
	uint16_t index;
	jsmntok_t* pTok;

	for( index = 0; index < numTokens; index++ )
	{
		pTok = &tokens[index];
		if( ( pTok->type == JSMN_STRING ) &&
				( 0 == strncmp( ( pMesg + pTok->start ),  pKey, keyLen ))) {

			break;
		}
	}

	return index;
}

TEST_GROUP( MQTT_Unit_BLE_Serialize );

/*-----------------------------------------------------------*/

TEST_SETUP( MQTT_Unit_BLE_Serialize )
{
	serializeInit = AwsIotMqttBLE_InitSerialize();
	TEST_ASSERT_EQUAL( true, serializeInit  );

	jsmn_init( &jsmnParser );
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( MQTT_Unit_BLE_Serialize )
{
	if( serializeInit == true )
	{
		AwsIotMqttBLE_CleanupSerialize();
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
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBLISH_TopicEncodingFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBLISH_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE_TopicEncodingFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE_TopicEncodingFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializePUBACK_MallocFail );
    RUN_TEST_CASE( MQTT_Unit_BLE_Serialize, SerializeDISCONNECT_MallocFail );


}

TEST(MQTT_Unit_BLE_Serialize, SerializeCONNECT )
{
	AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index;

	connectInfo.clientIdentifierLength = strlen( TEST_CLIENTID );
	connectInfo.pClientIdentifier = TEST_CLIENTID;
	connectInfo.pUserName = TEST_USER_NAME;
	connectInfo.userNameLength = strlen( TEST_USER_NAME );
	connectInfo.pPassword = NULL;
	connectInfo.cleanSession = true;

	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializeConnect( &connectInfo, NULL, ( uint8_t**) &pMesg, &bufLen );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

		/*Verify its in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_CONNECT ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLECLIENT_ID, strlen( mqttBLECLIENT_ID ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( TEST_CLIENTID ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEBROKER_EP, strlen( mqttBLEBROKER_EP ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), clientcredentialMQTT_BROKER_ENDPOINT, tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEBROKER_PORT, strlen( mqttBLEBROKER_PORT ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( clientcredentialMQTT_BROKER_PORT ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLECLEAN_SESSION, strlen( mqttBLECLEAN_SESSION ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), "true", tokens[index+1].size ));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}

}

TEST(MQTT_Unit_BLE_Serialize, SerializePUBLISH )
{
	AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index, packetIdentifier;

	publishInfo.QoS = TEST_QOS1;
	publishInfo.pPayload = ( uint8_t* ) TEST_DATA;
	publishInfo.payloadLength = TEST_DATA_LENGTH;
	publishInfo.pTopicName = TEST_TOPIC;
	publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
	publishInfo.retain = false;
	publishInfo.retryMs = TEST_RETRY_MS;
	publishInfo.retryLimit = TEST_RETRY_LIMIT;

	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializePublish( &publishInfo, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
		TEST_ASSERT_NOT_EQUAL( 0, packetIdentifier );

		/*Verify serialized message is in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_PUBLISH ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLETOPIC, strlen( mqttBLETOPIC ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ),  TEST_TOPIC_ENCODED, TEST_TOPIC_ENCODED_LENGTH ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEPAYLOAD, strlen( mqttBLEPAYLOAD ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), TEST_DATA_ENCODED , TEST_DATA_ENCODED_LENGTH ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEQOS, strlen( mqttBLEQOS ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( TEST_QOS1 ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMESSAGE_ID, strlen( mqttBLEMESSAGE_ID ) );
		TEST_ASSERT_LESS_THAN( numTokens-1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( packetIdentifier, atoi( ( pMesg + tokens[index+1].start )));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializePUBLISH_QOS0 )
{
	AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index, packetIdentifier = 0;

	publishInfo.QoS = TEST_QOS0;
	publishInfo.pPayload = ( uint8_t* ) TEST_DATA;
	publishInfo.payloadLength = TEST_DATA_LENGTH;
	publishInfo.pTopicName = TEST_TOPIC;
	publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
	publishInfo.retain = false;
	publishInfo.retryMs = TEST_RETRY_MS;
	publishInfo.retryLimit = TEST_RETRY_LIMIT;

	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializePublish( &publishInfo, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
		TEST_ASSERT_EQUAL( 0, packetIdentifier );

		/*Verify serialized message is in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_PUBLISH ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLETOPIC, strlen( mqttBLETOPIC ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ),  TEST_TOPIC_ENCODED, TEST_TOPIC_ENCODED_LENGTH ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEPAYLOAD, strlen( mqttBLEPAYLOAD ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), TEST_DATA_ENCODED , TEST_DATA_ENCODED_LENGTH ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEQOS, strlen( mqttBLEQOS ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( TEST_QOS1 ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMESSAGE_ID, strlen( mqttBLEMESSAGE_ID ) );
		TEST_ASSERT_LESS_THAN( numTokens-1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( packetIdentifier, atoi( ( pMesg + tokens[index+1].start )));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE )
{
	AwsIotMqttSubscription_t subscriptionList[2] = { AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER,  AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER };
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index, packetIdentifier;

	subscriptionList[0].QoS = TEST_QOS1;
	subscriptionList[0].pTopicFilter = TEST_TOPIC;
	subscriptionList[0].topicFilterLength = TEST_TOPIC_LENGTH;
	subscriptionList[1].QoS = TEST_QOS1;
	subscriptionList[1].pTopicFilter = TEST_TOPIC2;
	subscriptionList[1].topicFilterLength = TEST_TOPIC2_LENGTH;

	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializeSubscribe( subscriptionList, 2, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
		TEST_ASSERT_NOT_EQUAL( 0, packetIdentifier );

		/*Verify serialized message is in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_SUBSCRIBE ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLETOPIC_LIST, strlen( mqttBLETOPIC_LIST ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_ARRAY, tokens[ index + 1 ].type );
		TEST_ASSERT_EQUAL( 2, tokens[ index + 1 ].size );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[ index + 2 ].type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[ index + 2 ].start ), TEST_TOPIC_ENCODED, TEST_TOPIC_ENCODED_LENGTH ));
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[ index + 3 ].type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[ index + 3 ].start ), TEST_TOPIC2_ENCODED, TEST_TOPIC2_ENCODED_LENGTH ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEQOS_LIST, strlen( mqttBLEQOS_LIST ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_ARRAY, tokens[ index + 1 ].type );
		TEST_ASSERT_EQUAL( 2, tokens[ index + 1 ].size );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[ index + 2 ].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+2].start ), JSON_STR( TEST_QOS1 ), tokens[index+2].size ));
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[ index + 3 ].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+3].start ), JSON_STR( TEST_QOS1 ), tokens[index+3].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMESSAGE_ID, strlen( mqttBLEMESSAGE_ID ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( packetIdentifier, atoi( ( pMesg + tokens[index+1].start )));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE )
{
	AwsIotMqttSubscription_t subscriptionList[2] = { AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER, AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER };
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index, packetIdentifier;

	subscriptionList[0].QoS = TEST_QOS1;
	subscriptionList[0].pTopicFilter = TEST_TOPIC;
	subscriptionList[0].topicFilterLength = TEST_TOPIC_LENGTH;
	subscriptionList[1].QoS = TEST_QOS1;
	subscriptionList[1].pTopicFilter = TEST_TOPIC2;
	subscriptionList[1].topicFilterLength = TEST_TOPIC2_LENGTH;

	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializeUnsubscribe( subscriptionList, 2, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );
		TEST_ASSERT_NOT_EQUAL( 0, packetIdentifier );

		/*Verify serialized message is in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_UNSUBSCRIBE ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLETOPIC_LIST, strlen( mqttBLETOPIC_LIST ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_ARRAY, tokens[ index + 1 ].type );
		TEST_ASSERT_EQUAL( 2, tokens[ index + 1 ].size );
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[ index + 2 ].type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[ index + 2 ].start ), TEST_TOPIC_ENCODED, TEST_TOPIC_ENCODED_LENGTH ));
		TEST_ASSERT_EQUAL( JSMN_STRING, tokens[ index + 3 ].type );
        TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[ index + 3 ].start ), TEST_TOPIC2_ENCODED, TEST_TOPIC2_ENCODED_LENGTH ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMESSAGE_ID, strlen( mqttBLEMESSAGE_ID ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( packetIdentifier, atoi( ( pMesg + tokens[index+1].start )));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}


TEST(MQTT_Unit_BLE_Serialize, SerializePUBACK )
{
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index;


	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializePuback( TEST_PACKET_IDENTIFIER, ( uint8_t**) &pMesg, &bufLen );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

		/*Verify serialized message is in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_PUBACK ), tokens[index+1].size ));

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMESSAGE_ID, strlen( mqttBLEMESSAGE_ID ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, atoi( ( pMesg + tokens[index+1].start )));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeDISCONNECT )
{
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	int16_t numTokens;
	uint16_t index;


	if( TEST_PROTECT() )
	{
		status = AwsIotMqttBLE_SerializeDisconnect( ( uint8_t**) &pMesg, &bufLen );
		TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
		TEST_ASSERT_NOT_NULL( pMesg );
		TEST_ASSERT_NOT_EQUAL( 0UL, bufLen );

		/*Verify serialized message is in JSON format */
		numTokens = ( int16_t ) jsmn_parse( &jsmnParser, pMesg, bufLen, tokens, mqttBLEMAX_MESG_TOKENS );
		TEST_ASSERT_GREATER_THAN( 0, numTokens  );

		index = prvGetTokenIndex( pMesg, tokens, numTokens, mqttBLEMSG_TYPE, strlen( mqttBLEMSG_TYPE ) );
		TEST_ASSERT_LESS_THAN( numTokens -1, index );
		TEST_ASSERT_EQUAL( JSMN_PRIMITIVE, tokens[index+1].type );
		TEST_ASSERT_EQUAL( 0, strncmp( ( pMesg + tokens[index+1].start ), JSON_STR( mqttBLEMSG_TYPE_DISCONNECT ), tokens[index+1].size ));
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, DeserializeCONNACK )
{
	AwsIotMqttError_t status;
	char connack[ TEST_CONNACK_MESG_LEN ];
	size_t connackLen, bytesProcessed;

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusConnected );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusConnecting );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusConnectionRefused );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SERVER_REFUSED, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusConnectionError );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SERVER_REFUSED, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusDisconnected );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SERVER_REFUSED, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusProtocolError );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SERVER_REFUSED, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG, mqttBLEMSG_TYPE_CONNACK, eMQTTBLEStatusUnknown );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SERVER_REFUSED, status );
	TEST_ASSERT_EQUAL( TEST_CONNACK_MESG_LEN, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_MALFORMED_MESG );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	connackLen = snprintf( connack, TEST_CONNACK_MESG_LEN, TEST_CONNACK_MESG_NO_RESP_CODE, mqttBLEMSG_TYPE_CONNACK );
	status =  AwsIotMqttBLE_DeserializeConnack( ( uint8_t* ) connack, TEST_CONNACK_MESG_LEN, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );
}

TEST(MQTT_Unit_BLE_Serialize, DeserializePUBLISH )
{
	AwsIotMqttError_t status;
	char publish[ TEST_PUBLISH_MESG_LEN ];
	size_t publishLen, bytesProcessed;
	AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
	uint16_t packetIdentifier;

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_PUBLISH_MESG, mqttBLEMSG_TYPE_PUBLISH );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_PUBLISH_MESG_LEN, bytesProcessed );
	TEST_ASSERT_EQUAL_INT( TEST_TOPIC_LENGTH, publishInfo.topicNameLength );
	TEST_ASSERT_EQUAL(0, strncmp( publishInfo.pTopicName, TEST_TOPIC, TEST_TOPIC_LENGTH ) );
	TEST_ASSERT_EQUAL_INT( TEST_DATA_LENGTH, publishInfo.payloadLength );
	TEST_ASSERT_EQUAL(0, strncmp( publishInfo.pPayload, TEST_DATA, TEST_DATA_LENGTH ) );
	TEST_ASSERT_EQUAL(TEST_QOS1, publishInfo.QoS );
	TEST_ASSERT_EQUAL(TEST_PACKET_IDENTIFIER, packetIdentifier );
	TEST_ASSERT_EQUAL( false, publishInfo.retain );

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_MALFORMED_MESG );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_PUBLISH_MESG_NO_QOS, mqttBLEMSG_TYPE_PUBLISH );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_PUBLISH_MESG_NO_TOPIC, mqttBLEMSG_TYPE_PUBLISH );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_PUBLISH_MESG_NO_MSG_ID, mqttBLEMSG_TYPE_PUBLISH );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_PUBLISH_MESG_NO_DATA, mqttBLEMSG_TYPE_PUBLISH );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );
}

TEST(MQTT_Unit_BLE_Serialize, DeserializePUBLISH_QOS0 )
{
	AwsIotMqttError_t status;
	char publish[ TEST_PUBLISH_MESG_LEN ];
	size_t publishLen, bytesProcessed;
	AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
	uint16_t packetIdentifier = 0;

	publishLen = snprintf( publish, TEST_PUBLISH_MESG_LEN, TEST_PUBLISH_MESG_QOS0, mqttBLEMSG_TYPE_PUBLISH );
	status =  AwsIotMqttBLE_DeserializePublish( ( uint8_t* ) publish, TEST_PUBLISH_MESG_LEN, &publishInfo, &packetIdentifier,  &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_PUBLISH_MESG_LEN, bytesProcessed );
	TEST_ASSERT_EQUAL_INT( TEST_TOPIC_LENGTH, publishInfo.topicNameLength );
	TEST_ASSERT_EQUAL(0, strncmp( publishInfo.pTopicName, TEST_TOPIC, TEST_TOPIC_LENGTH ) );
	TEST_ASSERT_EQUAL_INT( TEST_DATA_LENGTH, publishInfo.payloadLength );
	TEST_ASSERT_EQUAL(0, strncmp( publishInfo.pPayload, TEST_DATA, TEST_DATA_LENGTH ) );
	TEST_ASSERT_EQUAL(TEST_QOS0, publishInfo.QoS );
	TEST_ASSERT_EQUAL(0, packetIdentifier );
	TEST_ASSERT_EQUAL( false, publishInfo.retain );

}

TEST(MQTT_Unit_BLE_Serialize, DeserializeSUBACK )
{
	AwsIotMqttError_t status;
	char suback[ TEST_SUBACK_MESG_LEN ];
	size_t subackLen, bytesProcessed;
	AwsIotMqttConnection_t mqttConnection = { 0 };
	uint16_t packetIdentifier;

	subackLen = snprintf( suback, TEST_SUBACK_MESG_LEN, TEST_SUBACK_MESG, mqttBLEMSG_TYPE_SUBACK, TEST_QOS1 );
	status =  AwsIotMqttBLE_DeserializeSuback( mqttConnection, ( uint8_t* ) suback, TEST_SUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_SUBACK_MESG_LEN, bytesProcessed );
	TEST_ASSERT_EQUAL(TEST_PACKET_IDENTIFIER, packetIdentifier );

	subackLen = snprintf( suback, TEST_SUBACK_MESG_LEN, TEST_MALFORMED_MESG );
	status =  AwsIotMqttBLE_DeserializeSuback( mqttConnection, ( uint8_t* ) suback, TEST_SUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	subackLen = snprintf( suback, TEST_SUBACK_MESG_LEN, TEST_SUBACK_MESG_NO_STATUS, mqttBLEMSG_TYPE_SUBACK );
	status =  AwsIotMqttBLE_DeserializeSuback( mqttConnection, ( uint8_t* ) suback, TEST_SUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	subackLen = snprintf( suback, TEST_SUBACK_MESG_LEN, TEST_SUBACK_MESG_NO_MSG_ID, mqttBLEMSG_TYPE_SUBACK, TEST_QOS1 );
	status =  AwsIotMqttBLE_DeserializeSuback( mqttConnection, ( uint8_t* ) suback, TEST_SUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );
}

TEST(MQTT_Unit_BLE_Serialize, DeserializePUBACK )
{
	AwsIotMqttError_t status;
	char puback[ TEST_PUBUNSUBACK_MESG_LEN ];
	size_t pubackLen, bytesProcessed;
	uint16_t packetIdentifier;

	pubackLen = snprintf( puback, TEST_PUBUNSUBACK_MESG_LEN, TEST_PUBUNSUBACK_MESG, mqttBLEMSG_TYPE_PUBACK );
	status =  AwsIotMqttBLE_DeserializePuback( ( uint8_t* ) puback, TEST_PUBUNSUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_PUBUNSUBACK_MESG_LEN, bytesProcessed );
	TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packetIdentifier );

	pubackLen = snprintf( puback, TEST_PUBUNSUBACK_MESG_LEN, TEST_MALFORMED_MESG );
	status =  AwsIotMqttBLE_DeserializePuback( ( uint8_t* ) puback, TEST_PUBUNSUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	pubackLen = snprintf( puback, TEST_PUBUNSUBACK_MESG_LEN, TEST_PUBUNSUBACK_MESG_NO_MSG_ID, mqttBLEMSG_TYPE_PUBACK );
	status =  AwsIotMqttBLE_DeserializePuback( ( uint8_t* ) puback, TEST_PUBUNSUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );
}

TEST(MQTT_Unit_BLE_Serialize, DeserializeUNSUBACK )
{
	AwsIotMqttError_t status;
	char puback[ TEST_PUBUNSUBACK_MESG_LEN ];
	size_t pubackLen, bytesProcessed;
	uint16_t packetIdentifier;

	pubackLen = snprintf( puback, TEST_PUBUNSUBACK_MESG_LEN, TEST_PUBUNSUBACK_MESG, mqttBLEMSG_TYPE_UNSUBACK );
	status =  AwsIotMqttBLE_DeserializeUnsuback( ( uint8_t* ) puback, TEST_PUBUNSUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
	TEST_ASSERT_EQUAL( TEST_PUBUNSUBACK_MESG_LEN, bytesProcessed );
	TEST_ASSERT_EQUAL( TEST_PACKET_IDENTIFIER, packetIdentifier );

	pubackLen = snprintf( puback, TEST_PUBUNSUBACK_MESG_LEN, TEST_MALFORMED_MESG );
	status =  AwsIotMqttBLE_DeserializeUnsuback( ( uint8_t* ) puback, TEST_PUBUNSUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );

	pubackLen = snprintf( puback, TEST_PUBUNSUBACK_MESG_LEN, TEST_PUBUNSUBACK_MESG_NO_MSG_ID, mqttBLEMSG_TYPE_UNSUBACK );
	status =  AwsIotMqttBLE_DeserializeUnsuback( ( uint8_t* ) puback, TEST_PUBUNSUBACK_MESG_LEN, &packetIdentifier, &bytesProcessed );
	TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_RESPONSE, status );
	TEST_ASSERT_EQUAL( 0, bytesProcessed );
}

TEST(MQTT_Unit_BLE_Serialize, SerializeCONNECT_MallocFail )
{
	AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
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
		status = AwsIotMqttBLE_SerializeConnect( &connectInfo, NULL, ( uint8_t**) &pMesg, &bufLen );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializePUBLISH_TopicEncodingFail )
{
	AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	uint16_t packetIdentifier;

	publishInfo.QoS = TEST_QOS1;
	publishInfo.pPayload = ( uint8_t* ) TEST_DATA;
	publishInfo.payloadLength = TEST_DATA_LENGTH;
	publishInfo.pTopicName = TEST_TOPIC;
	publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
	publishInfo.retain = false;
	publishInfo.retryMs = TEST_RETRY_MS;
	publishInfo.retryLimit = TEST_RETRY_LIMIT;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 0 );
		status = AwsIotMqttBLE_SerializePublish( &publishInfo, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializePUBLISH_MallocFail )
{
	AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	uint16_t packetIdentifier;

	publishInfo.QoS = TEST_QOS1;
	publishInfo.pPayload = ( uint8_t* ) TEST_DATA;
	publishInfo.payloadLength = TEST_DATA_LENGTH;
	publishInfo.pTopicName = TEST_TOPIC;
	publishInfo.topicNameLength = TEST_TOPIC_LENGTH;
	publishInfo.retain = false;
	publishInfo.retryMs = TEST_RETRY_MS;
	publishInfo.retryLimit = TEST_RETRY_LIMIT;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 1 );
		status = AwsIotMqttBLE_SerializePublish( &publishInfo, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE_TopicEncodingFail )
{
	AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	uint16_t packetIdentifier;

	subscription.QoS = TEST_QOS1;
	subscription.pTopicFilter = TEST_TOPIC;
	subscription.topicFilterLength = TEST_TOPIC_LENGTH;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 0 );
		status = AwsIotMqttBLE_SerializeSubscribe( &subscription, 1, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeSUBSCRIBE_MallocFail )
{
	AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	uint16_t packetIdentifier;

	subscription.QoS = TEST_QOS1;
	subscription.pTopicFilter = TEST_TOPIC;
	subscription.topicFilterLength = TEST_TOPIC_LENGTH;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 1 );
		status = AwsIotMqttBLE_SerializeSubscribe( &subscription, 1, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializePUBACK_MallocFail )
{
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 0 );
		status = AwsIotMqttBLE_SerializePuback( TEST_PACKET_IDENTIFIER, ( uint8_t**) &pMesg, &bufLen );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE_TopicEncodingFail )
{
	AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	uint16_t packetIdentifier;

	subscription.QoS = TEST_QOS1;
	subscription.pTopicFilter = TEST_TOPIC;
	subscription.topicFilterLength = TEST_TOPIC_LENGTH;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 0 );
		status = AwsIotMqttBLE_SerializeUnsubscribe( &subscription, 1, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}
	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}


TEST(MQTT_Unit_BLE_Serialize, SerializeUNSUBSCRIBE_MallocFail )
{
	AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;
	uint16_t packetIdentifier;

	subscription.QoS = TEST_QOS1;
	subscription.pTopicFilter = TEST_TOPIC;
	subscription.topicFilterLength = TEST_TOPIC_LENGTH;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 1 );
		status = AwsIotMqttBLE_SerializeUnsubscribe( &subscription, 1, ( uint8_t**) &pMesg, &bufLen, &packetIdentifier );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}
	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}

TEST(MQTT_Unit_BLE_Serialize, SerializeDISCONNECT_MallocFail )
{
	volatile AwsIotMqttError_t status;
	char *pMesg;
	size_t bufLen;

	if( TEST_PROTECT() )
	{
		UnityMalloc_MakeMallocFailAfterCount( 0 );
		status = AwsIotMqttBLE_SerializeDisconnect( ( uint8_t**) &pMesg, &bufLen );
		if( status != AWS_IOT_MQTT_SUCCESS )
		{
			TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
		}
	}

	if( status == AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqttBLE_FreePacket( ( uint8_t* )pMesg );
	}
}
