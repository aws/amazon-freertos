/*
 * Amazon FreeRTOS
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_test_ble_end_to_end.c
 * @brief Tests for ble.
 */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* C standard library includes. */
#include <stddef.h>
#include <string.h>
#include "iot_ble.h"
#include "iot_ble_mqtt.h"
#include "iot_ble_config.h"
#include "platform/iot_network_ble.h"
#include "platform/iot_clock.h"
#include "platform/iot_network_afr.h"
/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @brief The maximum length of an MQTT client identifier.
 *
 * When @ref IOT_TEST_MQTT_CLIENT_IDENTIFIER is defined, this value must
 * accommodate the length of @ref IOT_TEST_MQTT_CLIENT_IDENTIFIER plus 4
 * to accommodate the Last Will and Testament test. Otherwise, this value is
 * set to 24, which is the longest client identifier length an MQTT server is
 * obligated to accept plus a NULL terminator.
 */
#ifdef IOT_TEST_MQTT_CLIENT_IDENTIFIER
    #define _CLIENT_IDENTIFIER_MAX_LENGTH    ( sizeof( IOT_TEST_MQTT_CLIENT_IDENTIFIER ) + 4 )
#else
    #define _CLIENT_IDENTIFIER_MAX_LENGTH    ( 24 )
#endif

extern char _pClientIdentifier[];

IotMqttNetworkInfo_t _IotNetworkInfo;
IotMqttConnection_t _IotMqttConnection;
IotBleMqttConnection_t _mqttBLEConnection = IOT_BLE_MQTT_CONNECTION_INITIALIZER;


extern void IotTestMqtt_SubscribePublishWait( IotMqttQos_t qos, IotMqttNetworkInfo_t * _IotNetworkInfo );
extern void IotTestMqtt_TearDown(void);

static void _BLEConnectionCallback( BTStatus_t status,
                             uint16_t connId,
                             bool connected,
                             BTBdaddr_t * pBa )
{
    if( connected == true )
    {
        IotBle_StopAdv();
    }
    else
    {
        ( void ) IotBle_StartAdv( NULL );

    }
}
static BaseType_t _BLEEnable( void )
{
    IotBleEventsCallbacks_t xEventCb;
    BaseType_t xRet = pdTRUE;
    static bool bInitBLE = false;
    BTStatus_t xStatus;

    if( bInitBLE == false )
    {
        xStatus = IotBle_Init();

        if( xStatus == eBTStatusSuccess )
        {
            bInitBLE = true;
        }
    }
    else
    {
        xStatus = IotBle_On();
    }
    /* Register BLE Connection callback */
    if( xStatus == eBTStatusSuccess )
    {
        xEventCb.pConnectionCb = _BLEConnectionCallback;
        if( IotBle_RegisterEventCb( eBLEConnection, xEventCb ) != eBTStatusSuccess )
        {
        	xStatus = eBTStatusFail;
        }
    }

    if( xStatus != eBTStatusSuccess)
    {
    	xRet = pdFALSE;
    }

    return xRet;
}

static BaseType_t _BLEDisable( void )
{
    bool ret = true;
    IotBleEventsCallbacks_t xEventCb;

    xEventCb.pConnectionCb = _BLEConnectionCallback;
    if( IotBle_UnRegisterEventCb( eBLEConnection, xEventCb ) != eBTStatusSuccess )
    {
    	ret = false;
    }

    if( ret == true )
    {
        if( IotBle_StopAdv() != eBTStatusSuccess )
        {
        	ret = false;
        }
    }

    if( ret == true )
    {
        if( IotBle_Off() != eBTStatusSuccess )
        {
        	ret = false;
        }
    }

	return ret;
}


static BaseType_t _removeBLEConnection()
{
	IotNetworkBle_Close(&_mqttBLEConnection);
	IotNetworkBle_Destroy(&_mqttBLEConnection);
	return _BLEDisable();
}


static BaseType_t _createBLEConnection()
{
    BaseType_t status = pdFALSE;
    size_t triesLeft = ( 200 );
    TickType_t retryDelay = pdMS_TO_TICKS( 1000 );
    IotMqttSerializer_t serializer = IOT_MQTT_SERIALIZER_INITIALIZER;

    /* Initialize common components. */
    if( IotCommon_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize common components." );
    }

    /* Initialize the MQTT library. */
    if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT library." );
    }

    _BLEEnable();


    while ( triesLeft > 0 )
    {
		if( IotNetworkBle_Create( &_IotMqttConnection, NULL,  &_mqttBLEConnection ) == pdTRUE )
		{
			IOT_MQTT_BLE_INIT_SERIALIZER( &serializer );

		    _IotNetworkInfo.createNetworkConnection = false;
		    _IotNetworkInfo.pNetworkConnection = &_mqttBLEConnection;
		    _IotNetworkInfo.pNetworkInterface = IOT_NETWORK_INTERFACE_BLE;
		    _IotNetworkInfo.pMqttSerializer = &serializer;

			status = pdTRUE;
			break;
		}else
		{
			triesLeft--;
			vTaskDelay(retryDelay);
		}
    }

    IotNetworkBle_SetReceiveCallback(&_mqttBLEConnection, NULL, &_IotMqttConnection);
    return status;
}
/*-----------------------------------------------------------*/
TEST_GROUP( Full_BLE_END_TO_END );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_END_TO_END )
{
    /* Generate a new, unique client identifier based on the time if no client
     * identifier is defined. Otherwise, copy the provided client identifier. */
    ( void ) snprintf( _pClientIdentifier,
                       _CLIENT_IDENTIFIER_MAX_LENGTH,
                       "iot%llu",
                       ( long long unsigned int ) IotClock_GetTimeMs() );
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_END_TO_END )
{
    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_END_TO_END )
{
    _createBLEConnection();

    RUN_TEST_CASE( Full_BLE_END_TO_END, IotTest_SubscribePublishWait );

	_removeBLEConnection();
}

TEST( Full_BLE_END_TO_END, IotTest_SubscribePublishWait )
{
	IotTestMqtt_SubscribePublishWait(IOT_MQTT_QOS_1, &_IotNetworkInfo);
}
