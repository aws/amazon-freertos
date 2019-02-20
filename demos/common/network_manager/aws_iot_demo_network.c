/*
 * Amazon FreeRTOS
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
 * @file aws_iot_demo_network.c
 * @brief Contains implementation for network creation and teardown functions for handling different types of network connections
 */
#include "aws_iot_demo.h"
#include "aws_iot_network_manager.h"
#include "aws_iot_demo_network.h"

#if WIFI_ENABLED
#include "platform/aws_iot_network.h"
#include "aws_clientcredential.h"
#endif

#if BLE_ENABLED
#include "iot_ble_mqtt.h"
#endif


#if BLE_ENABLED
/**
 * @brief Creates a network connection over BLE transport type to transfer MQTT messages.
 * @return true if the connection was created successfully
 */
static BaseType_t prxCreateBLEConnection( MqttConnectionContext_t *pxNetworkContext );
#endif

#if WIFI_ENABLED
/**
 * @brief Creates a secure socket Connection over TCP/IP transport type.
 *
 * @param[in] pxNetworkContext Network context for the connection.
 * @return pdTRUE if the connection is created successfully
 */
static BaseType_t prxCreateSecureSocketConnection( MqttConnectionContext_t *pxNetworkContext );
#endif

/**
 * @brief Creates a network connection to one of the preferred networks
 * @param pxNetworkContext Pointer to the user supplied network context.
 * @param ulPreferredNetworks OR separated flag of preferred neworks.
 * @return
 */
static uint32_t prxCreateNetworkConnection( MqttConnectionContext_t *pxNetworkContext, uint32_t ulNetworkTypes );

/**
 * @brief Used to initialize the network interface.
 */
static AwsIotMqttNetIf_t xDefaultNetworkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;

static uint32_t prxCreateNetworkConnection( MqttConnectionContext_t *pxNetworkContext, uint32_t ulNetworkTypes )
{
    uint32_t ulConnectedNetworks =
            ( AwsIotNetworkManager_GetConnectedNetworks() & ulNetworkTypes );

#if BLE_ENABLED
    if( ( ulConnectedNetworks & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
    {
        if( prxCreateBLEConnection( pxNetworkContext ) == pdTRUE )
        {
            return AWSIOT_NETWORK_TYPE_BLE;
        }
    }
#endif

#if WIFI_ENABLED
    if( ( ulConnectedNetworks & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
    {
        if( prxCreateSecureSocketConnection( pxNetworkContext ) == pdTRUE )
        {
            return AWSIOT_NETWORK_TYPE_WIFI;
        }
    }
#endif

    return AWSIOT_NETWORK_TYPE_NONE;
}

#if WIFI_ENABLED
static BaseType_t prxCreateSecureSocketConnection( MqttConnectionContext_t *pxNetworkContext )
{
    AwsIotNetworkError_t xStatus = AWS_IOT_NETWORK_SUCCESS;

    AwsIotNetworkConnection_t xConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
    AwsIotNetworkTlsInfo_t xTlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
    AwsIotMqttNetIf_t* pxNetworkIface = &( pxNetworkContext->xNetworkInterface );

    /* Connection Uses default root certificates.
     * Override the default root CA certificates by setting pcServerCertificate
     * and xServerCertificateSize. SNI may need to be disabled if not using the
     * default certificates. */
    xTlsInfo.disableSni = false;
    xTlsInfo.pClientCert = clientcredentialCLIENT_CERTIFICATE_PEM;
    xTlsInfo.clientCertLength = ( size_t ) clientcredentialCLIENT_CERTIFICATE_LENGTH;
    xTlsInfo.pPrivateKey = clientcredentialCLIENT_PRIVATE_KEY_PEM;
    xTlsInfo.privateKeyLength = ( size_t ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

    /* Disable ALPN if not using port 443. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        xTlsInfo.pAlpnProtos = NULL;
    }

    /* Establish a TCP connection to the MQTT server. */
    xStatus =  AwsIotNetwork_CreateConnection( &xConnection,
            clientcredentialMQTT_BROKER_ENDPOINT,
            clientcredentialMQTT_BROKER_PORT,
            &xTlsInfo );

    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        /* Create the task that processes incoming MQTT data. */
        xStatus = AwsIotNetwork_SetMqttReceiveCallback(
                xConnection,
                &pxNetworkContext->xMqttConnection,
                AwsIotMqtt_ReceiveCallback );
    }

    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        ( *pxNetworkIface ) = xDefaultNetworkInterface;
        pxNetworkIface->pSendContext = ( void * ) xConnection;
        pxNetworkIface->send = AwsIotNetwork_Send;
        pxNetworkIface->pDisconnectContext = pxNetworkContext;
        pxNetworkIface->disconnect = pxNetworkContext->xDisconnectCallback;

        pxNetworkContext->pvNetworkConnection = ( void* ) xConnection;
    }
    else
    {
        if( xConnection != AWS_IOT_NETWORK_CONNECTION_INITIALIZER )
        {
            AwsIotNetwork_CloseConnection( xConnection );
            AwsIotNetwork_DestroyConnection( xConnection );
        }

    }

    return ( xStatus == AWS_IOT_NETWORK_SUCCESS );
}
#endif

#if BLE_ENABLED
static BaseType_t prxCreateBLEConnection( MqttConnectionContext_t *pxNetworkContext )
{
    BaseType_t xStatus = pdFALSE;
    IotBleMqttConnection_t xBLEConnection = IOT_BLE_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttNetIf_t* pxNetworkIface = &( pxNetworkContext->xNetworkInterface );

    if( IotBleMqtt_CreateConnection( &pxNetworkContext->xMqttConnection, &xBLEConnection ) == pdTRUE )
    {
        ( *pxNetworkIface ) = xDefaultNetworkInterface;
        AWS_IOT_MQTT_BLE_INIT_SERIALIZER( pxNetworkIface );
        pxNetworkIface->send          = IotBleMqtt_Send;
        pxNetworkIface->pSendContext  = ( void * ) xBLEConnection;
        pxNetworkIface->pDisconnectContext = pxNetworkContext;
        pxNetworkIface->disconnect = pxNetworkContext->xDisconnectCallback;

        pxNetworkContext->pvNetworkConnection = ( void* ) xBLEConnection;
        xStatus = pdTRUE;
    }
    return xStatus;
}
#endif


BaseType_t xMqttDemoCreateNetworkConnection(
        MqttConnectionContext_t* pxConnection,
        uint32_t ulNetworkTypes,
        uint32_t ulConnRetryIntervalSeconds,
        uint32_t ulConnRetryLimit )
{
    size_t xTriesLeft = ( ulConnRetryLimit + 1 );
    BaseType_t xRet = pdFALSE;
    TickType_t xRetryDelay = pdMS_TO_TICKS( ulConnRetryIntervalSeconds * 1000 );

    while ( xTriesLeft > 0 )
    {
        pxConnection->ulNetworkType = prxCreateNetworkConnection( pxConnection, ulNetworkTypes );

        if( pxConnection->ulNetworkType != AWSIOT_NETWORK_TYPE_NONE )
        {
            xRet = pdTRUE;
            break;
        }
        else
        {
            xTriesLeft--;
            if( xTriesLeft > 0 )
            {
                vTaskDelay( xRetryDelay );
            }
        }
    }

    return xRet;
}

void vMqttDemoDeleteNetworkConnection( MqttConnectionContext_t* pxNetworkContext )
{
    BaseType_t xDeleted = pdFALSE;

    if( pxNetworkContext != NULL )
    {

#if BLE_ENABLED
        if( pxNetworkContext->ulNetworkType == AWSIOT_NETWORK_TYPE_BLE )
        {
            IotBleMqtt_CloseConnection( ( IotBleMqttConnection_t ) pxNetworkContext->pvNetworkConnection );
            IotBleMqtt_DestroyConnection( ( IotBleMqttConnection_t ) pxNetworkContext->pvNetworkConnection );
            xDeleted = pdTRUE;
        }
#endif
#if WIFI_ENABLED
        if(  pxNetworkContext->ulNetworkType  == AWSIOT_NETWORK_TYPE_WIFI )
        {
            AwsIotNetwork_CloseConnection( ( AwsIotNetworkConnection_t ) pxNetworkContext->pvNetworkConnection );
            AwsIotNetwork_DestroyConnection( ( AwsIotNetworkConnection_t ) pxNetworkContext->pvNetworkConnection );
            xDeleted = pdTRUE;
        }
#endif

        if( xDeleted )
        {
            pxNetworkContext->pvNetworkConnection = NULL;
            pxNetworkContext->ulNetworkType = AWSIOT_NETWORK_TYPE_NONE;
        }

    }
}
