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
#include "platform/iot_network_afr.h"
/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"


typedef struct MqttConnectionContext
{
    void *pNetworkConnection;
    uint32_t networkType;
    IotMqttNetIf_t networkInterface;
    IotNetworkError_t ( * pDisconnectCallback ) ( void *) ;
    IotMqttConnection_t mqttConnection;
} MqttConnectionContext_t;
static MqttConnectionContext_t   networkContext;


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

static BaseType_t _createBLEConnection(MqttConnectionContext_t   * pNetworkContext)
{
    BaseType_t status = pdFALSE;
    IotBleMqttConnection_t BLEConnection = IOT_BLE_MQTT_CONNECTION_INITIALIZER;
    IotMqttNetIf_t* pNetworkIface = &( pNetworkContext->networkInterface );
    IotMqttNetIf_t defaultNetworkInterface = IOT_MQTT_NETIF_INITIALIZER;
    size_t triesLeft = ( 60 );
    TickType_t retryDelay = pdMS_TO_TICKS( 1000 );

    _BLEEnable();

    while ( triesLeft > 0 )
    {
		if( IotBleMqtt_CreateConnection( &pNetworkContext->mqttConnection, &BLEConnection ) == pdTRUE )
		{
			( *pNetworkIface ) = defaultNetworkInterface;
			IOT_MQTT_BLE_INIT_SERIALIZER( pNetworkIface );
			pNetworkIface->send          = IotBleMqtt_Send;
			pNetworkIface->pSendContext  = ( void * ) BLEConnection;
			pNetworkIface->pDisconnectContext = pNetworkContext;
			pNetworkIface->disconnect = pNetworkContext->pDisconnectCallback;

			pNetworkContext->pNetworkConnection = ( void* ) BLEConnection;
			status = pdTRUE;
			break;
		}else
		{
			triesLeft--;
			vTaskDelay(retryDelay);
		}
    }
    return status;
}

static BaseType_t _destroyBLEConnection(MqttConnectionContext_t   * pNetworkContext)
{
	return _BLEDisable();
}

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_END_TO_END )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_END_TO_END )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_END_TO_END )
{
	(void)_createBLEConnection(&networkContext);
	RUN_TEST_GROUP( Full_MQTT_Agent_Stress_Tests );
	(void)_destroyBLEConnection(&networkContext);
}
