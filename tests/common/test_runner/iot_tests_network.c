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
 * @file iot_tests_network.c
 * @brief Common network function implementations for the tests.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

#include "platform/iot_network_afr.h"
#include "private/iot_error.h"
#include "iot_ble_mqtt.h"


IotMqttNetworkInfo_t IotNetworkInfo;
IotMqttConnection_t IotMqttConnection;
void * pNetworkConnection;
uint16_t _IotTestNetworkType = AWSIOT_NETWORK_TYPE_WIFI;


BaseType_t xNetworkCreated = pdFALSE;

#if (WIFI_SUPPORTED == 1)
static BaseType_t _createConnectionWIFI( void  )
{
    _IOT_FUNCTION_ENTRY( BaseType_t, pdTRUE);

    IotNetworkError_t xStatus = IOT_NETWORK_SUCCESS;
    static IotNetworkConnectionAfr_t xConnection;


    pNetworkConnection = &xConnection;
    IotNetworkServerInfoAfr_t xServerInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t xCredentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

    /* Disable ALPN if not using port 443. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        xCredentials.pAlpnProtos = NULL;
    }

    /* Establish a TCP connection to the MQTT server. */
    xStatus =  IotNetworkAfr.create(&xServerInfo,&xCredentials, pNetworkConnection);

    if( xStatus != IOT_NETWORK_SUCCESS )
    {
        _IOT_SET_AND_GOTO_CLEANUP( pdFALSE );
    }
    else
    {
        xNetworkCreated = pdTRUE;
    }

    IotNetworkInfo.createNetworkConnection = false;
    IotNetworkInfo.pNetworkConnection = pNetworkConnection;
    IotNetworkInfo.pNetworkInterface = &IotNetworkAfr;

    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != pdTRUE )
    {
        if( xNetworkCreated == pdTRUE )
        {
        	IotNetworkInfo.pNetworkInterface->close(pNetworkConnection);
        	IotNetworkInfo.pNetworkInterface->destroy(pNetworkConnection);
        }
    }

    _IOT_FUNCTION_CLEANUP_END();
 }
#endif

#if (BLE_SUPPORTED == 1)
static BaseType_t _createConnectionBLE( void  )
{
    _IOT_FUNCTION_ENTRY( BaseType_t, pdTRUE);

    IotNetworkError_t xStatus = IOT_NETWORK_SUCCESS;
    static IotBleMqttConnection_t * bleConnection = IOT_BLE_MQTT_CONNECTION_INITIALIZER;

    pNetworkConnection = &bleConnection;

    /* Establish a TCP connection to the MQTT server. */
    xStatus =  IotNetworkBle.create(NULL,NULL, pNetworkConnection);

    if( xStatus != IOT_NETWORK_SUCCESS )
    {
        _IOT_SET_AND_GOTO_CLEANUP( pdFALSE );
    }
    else
    {
        xNetworkCreated = pdTRUE;
    }

    IotNetworkInfo.createNetworkConnection = false;
    IotNetworkInfo.pNetworkCredentialInfo = NULL;
    IotNetworkInfo.pNetworkConnection = pNetworkConnection;
    IotNetworkInfo.pNetworkInterface = &IotNetworkBle;
    IotNetworkInfo.pMqttSerializer = (IotMqttSerializer_t *) &IotBleMqttSerializer;

    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != pdTRUE )
    {
        if( xNetworkCreated == pdTRUE )
        {
        	IotNetworkInfo.pNetworkInterface->close(pNetworkConnection);
        	IotNetworkInfo.pNetworkInterface->destroy(pNetworkConnection);
        }
    }

    _IOT_FUNCTION_CLEANUP_END();
}

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


#endif

void IotTestNetwork_SelectNetworkType(uint16_t networkType)
{
    switch(networkType)
    {
        #if (BLE_SUPPORTED == 1) 
        case AWSIOT_NETWORK_TYPE_BLE:
            _BLEEnable();
        break;
        #endif
        default:break;
    }
    _IotTestNetworkType = networkType;
}

void IotTestNetwork_OverrideSerializer ( IotMqttSerializer_t * pSerializer )
{
    IotNetworkInfo.pMqttSerializer = pSerializer;
}

IotMqttSerializer_t * IotTestNetwork_GetSerializer(void)
{
    return (IotMqttSerializer_t *)IotNetworkInfo.pMqttSerializer;
}


BaseType_t IotTestNetwork_Connect( void )
{
    size_t xTriesLeft = 50;
    BaseType_t xRet = pdFALSE;
    BaseType_t xNetworkCreated = pdFALSE;
    TickType_t xRetryDelay = pdMS_TO_TICKS( IOT_TEST_NETWORK_RETRY_DELAY_MS );

    while ( xTriesLeft > 0 )
    {
        switch(_IotTestNetworkType)
        {
          #if (BLE_SUPPORTED == 1) 
          case AWSIOT_NETWORK_TYPE_BLE:
              xNetworkCreated = _createConnectionBLE( );
              break;
          #endif

          #if (WIFI_SUPPORTED == 1)  
          case AWSIOT_NETWORK_TYPE_WIFI:
              xNetworkCreated = _createConnectionWIFI( );
              break;
          #endif
          default:break;
        }
    	

        if( xNetworkCreated == pdTRUE )
        {
            xRet = pdTRUE;
            break;
        }
        else
        {
            xTriesLeft--;

            if( xTriesLeft > 0 )
            {
            	configPRINTF(("Failed to connect to network, %d retries left\n", xTriesLeft));
                vTaskDelay( xRetryDelay );
            }else
            {
            	configPRINTF(("Failed to connect to network, no more retry\n"));
            }
        }
    }

    return xRet;
}

void IotTestNetwork_Cleanup(void)
{
    if( xNetworkCreated == pdTRUE )
    {
    	IotNetworkInfo.pNetworkInterface->close(pNetworkConnection);
    	IotNetworkInfo.pNetworkInterface->destroy(pNetworkConnection);
        xNetworkCreated = pdFALSE;
    }
}


void IotTestNetwork_Close(void)
{
    if( xNetworkCreated == pdTRUE )
    {
        IotNetworkInfo.pNetworkInterface->close(pNetworkConnection);
    }
 }

 void IotTestNetwork_Destroy(void)
{
    if( xNetworkCreated == pdTRUE )
    {
        IotNetworkInfo.pNetworkInterface->destroy(pNetworkConnection);
    }
 }
