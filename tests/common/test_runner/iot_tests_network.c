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
BaseType_t xNetworkCreated = pdFALSE;

static BaseType_t _createConnection( void * pNetworkConnection, const IotMqttSerializer_t const * pSerializer, const IotNetworkInterface_t const * networkInterface  )
{
    _IOT_FUNCTION_ENTRY( BaseType_t, pdTRUE);

    IotNetworkError_t xStatus = IOT_NETWORK_SUCCESS;
    static IotNetworkConnectionAfr_t xConnection;
    void * pConnectionInfo = NULL;
    void * pCredentialInfo = NULL;

   #if WIFI_SUPPORTED
    IotNetworkServerInfoAfr_t xServerInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t xCredentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

    /* Disable ALPN if not using port 443. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        xCredentials.pAlpnProtos = NULL;
    }

    pConnectionInfo = &xServerInfo;
    pCredentialInfo =  &xCredentials;
    #endif

    /* Establish a TCP connection to the MQTT server. */
    xStatus =  networkInterface->create(pConnectionInfo,pCredentialInfo, pNetworkConnection);

    if( xStatus != IOT_NETWORK_SUCCESS )
    {
        _IOT_SET_AND_GOTO_CLEANUP( pdFALSE );
    }
    else
    {
        xNetworkCreated = pdTRUE;
    }

    IotNetworkInfo.createNetworkConnection = false;
    IotNetworkInfo.pNetworkConnection = &xConnection;
    IotNetworkInfo.pNetworkInterface = networkInterface;
    IotNetworkInfo.pMqttSerializer = (IotMqttSerializer_t *) pSerializer;

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

BaseType_t IotTestNetwork_Connect( void * pNetworkConnection, const IotMqttSerializer_t const * pSerializer, const IotNetworkInterface_t const * networkInterface )
{
    size_t xTriesLeft = 5;
    BaseType_t xRet = pdFALSE;
    BaseType_t xNetworkCreated = pdFALSE;
    TickType_t xRetryDelay = pdMS_TO_TICKS( IOT_TEST_NETWORK_RETRY_DELAY_MS * 1000 );

    while ( xTriesLeft > 0 )
    {
    	xNetworkCreated = _createConnection( pNetworkConnection, pSerializer, networkInterface );

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

void IotTestNetwork_Cleanup(void * pNetworkConnection)
{
    if( xNetworkCreated == pdTRUE )
    {
    	IotNetworkInfo.pNetworkInterface->close(pNetworkConnection);
    	IotNetworkInfo.pNetworkInterface->destroy(pNetworkConnection);
        xNetworkCreated = pdFALSE;
    }
}


void IotTestNetwork_Close(void * pNetworkConnection)
{
    if( xNetworkCreated == pdTRUE )
    {
        IotNetworkInfo.pNetworkInterface->close(pNetworkConnection);
    }
 }

 void IotTestNetwork_Destroy(void * pNetworkConnection)
{
    if( xNetworkCreated == pdTRUE )
    {
        IotNetworkInfo.pNetworkInterface->destroy(pNetworkConnection);
    }
 }
