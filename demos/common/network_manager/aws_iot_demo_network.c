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
 * @brief File can handle multiple networks and has function to create and destroy network connections for mqtt demos.
 */
#include "aws_iot_demo.h"
#include "aws_iot_network_manager.h"
#include "aws_iot_demo_network.h"
#include "platform/aws_iot_clock.h"
#include "FreeRTOS_POSIX/time.h"

#if WIFI_ENABLED
#include "aws_clientcredential.h"
#include "platform/aws_iot_network.h"
#endif

#if BLE_ENABLED
#include "aws_iot_mqtt_ble.h"
#endif

typedef struct NetworkConnection
{
    uint32_t ulType;
    SubscriptionHandle_t xDisconnectCbSubscription;
    NetworkDisconnectedCallback_t xDisconnectCallback;
    void *pvConnection;
} NetworkConnection_t;

/**
 * @brief Callback wrapper which pass a network disconnect event to the demo application.
 * @param[in] ulNetworkType Type of network to register the callback.
 * @param[in] xNetworkState State of the network passed in.
 * @param[in] pvContext Pointer to the application context.
 */
static void prvNetworkSubscriptionWrapper ( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext );

#if BLE_ENABLED
/**
 * @brief Creates a network connection over BLE to transfer MQTT messages.
 * @param[in] pxNetworkInterface Network Interface for the connection
 * @param[in] pxMqttConnection Handle to the MQTT
 * @param[out] ppvConnection Pointer to a connection
 * @return true if the connection was created successfully
 */
static bool prbCreateBLEConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvCpnnection );
#endif

#if WIFI_ENABLED
/**
 * @brief Creates a secure socket Connection
 * @param[in] pxNetworkInterface Network Interface for the connection.
 * @param[in] pxMqttConnection Handle to the connection
 * @param[out] ppvConnection Pointer to the Connection
 * @return true if the connection was created successfully
 */
static bool prbCreateSecureSocketConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void **ppvConnection );
#endif

static uint32_t prxCreateNetworkConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvConnection );
/**
 * @brief Used to initialize the network interface.
 */
static AwsIotMqttNetIf_t xDefaultNetworkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;

static uint32_t prxCreateNetworkConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvConnection )
{
    uint32_t ulConnectedNetworks = AwsIotNetworkManager_GetConnectedNetworks();
    if( ulConnectedNetworks == AWSIOT_NETWORK_TYPE_NONE )
    {
        ulConnectedNetworks = AwsIotNetworkManager_WaitForNetworkConnection();
    }
    ( *pxNetworkInterface ) = xDefaultNetworkInterface;

#if BLE_ENABLED
    if( ( ulConnectedNetworks & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
    {
        if( prbCreateBLEConnection( pxNetworkInterface, pxMqttConnection, ppvConnection ) == true )
        {
            return AWSIOT_NETWORK_TYPE_BLE;
        }
    }
#endif
#if WIFI_ENABLED
    if( ( ulConnectedNetworks & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
    {
        if( prbCreateSecureSocketConnection( pxNetworkInterface, pxMqttConnection, ppvConnection ) == true )
        {
            return AWSIOT_NETWORK_TYPE_WIFI;
        }
    }
#endif
    return AWSIOT_NETWORK_TYPE_NONE;
}

#if WIFI_ENABLED
static bool prbCreateSecureSocketConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void **ppvConnection )
{
    AwsIotNetworkError_t xStatus = AWS_IOT_NETWORK_SUCCESS;
    AwsIotNetworkConnection_t xConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
    AwsIotNetworkTlsInfo_t xTlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
    const char* pcHost = clientcredentialMQTT_BROKER_ENDPOINT;

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
    		pcHost,
			clientcredentialMQTT_BROKER_PORT,
			&xTlsInfo );

    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        /* Create the task that processes incoming MQTT data. */
        xStatus = AwsIotNetwork_SetMqttReceiveCallback(
        		xConnection,
				pxMqttConnection,
				AwsIotMqtt_ReceiveCallback );
    }

    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        pxNetworkInterface->pDisconnectContext = ( void * ) xConnection;
        pxNetworkInterface->pSendContext = ( void * ) xConnection;
        pxNetworkInterface->disconnect = AwsIotNetwork_CloseConnection;
        pxNetworkInterface->send = AwsIotNetwork_Send;

        *ppvConnection = ( void* ) xConnection;
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
static bool prbCreateBLEConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvConnection )
{
    bool xStatus = false;
    AwsIotMqttBLEConnection_t xBLEConnection = AWS_IOT_MQTT_BLE_CONNECTION_INITIALIZER;

    if( AwsIotMqttBLE_CreateConnection( pxMqttConnection, &xBLEConnection ) == pdTRUE )
    {
        AWS_IOT_MQTT_BLE_INIT_SERIALIZER( pxNetworkInterface );
        pxNetworkInterface->send          = AwsIotMqttBLE_Send;
        pxNetworkInterface->pSendContext  = ( void * ) xBLEConnection;
        pxNetworkInterface->pDisconnectContext = ( void * ) xBLEConnection;
        pxNetworkInterface->disconnect = AwsIotMqttBLE_CloseConnection;

        *ppvConnection = ( void* ) xBLEConnection;
        xStatus = true;
    }
    return xStatus;
}
#endif


static void prvNetworkSubscriptionWrapper( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext )
{
    NetworkConnection_t* pxConn = ( NetworkConnection_t* ) pvContext;

    if( ( xNetworkState == eNetworkStateDisabled ) && ( pxConn != NULL ) )
    {
       pxConn->xDisconnectCallback( ( AwsIotDemoNetworkConnection_t ) pxConn->pvConnection );
    }
}

void AwsIotDemo_CreateNetworkConnection(
		AwsIotMqttNetIf_t* pxNetworkInterface,
		AwsIotMqttConnection_t* pxMqttConnection,
		NetworkDisconnectedCallback_t xDisconnectCallback,
		AwsIotDemoNetworkConnection_t* pxIotNetworkConnection,
		uint32_t ulConnIntervalSec,
		uint32_t ulConnRetryLimit)
{
    size_t xRetryCount = 0;
    struct timespec xDelay =
    {
        .tv_sec = ulConnIntervalSec,
        .tv_nsec = 0
    };
    uint32_t ulConnectedNetwork = AWSIOT_NETWORK_TYPE_NONE;
    void *pvConnection = NULL;
    NetworkConnection_t* pxNetwork = NULL;

    while ( xRetryCount < ulConnRetryLimit )
    {
        ulConnectedNetwork = prxCreateNetworkConnection( pxNetworkInterface, pxMqttConnection, &pvConnection );
        if( ulConnectedNetwork != AWSIOT_NETWORK_TYPE_NONE )
        {
            pxNetwork = pvPortMalloc( sizeof( NetworkConnection_t ) );
            if( pxNetwork != NULL )
            {
                pxNetwork->pvConnection = pvConnection;
                pxNetwork->ulType = ulConnectedNetwork;
                if( xDisconnectCallback != NULL )
                {
                    AwsIotNetworkManager_SubscribeForStateChange(
                            ulConnectedNetwork,
                            prvNetworkSubscriptionWrapper,
                            pxNetwork,
                            &( pxNetwork->xDisconnectCbSubscription ) );
                    pxNetwork->xDisconnectCallback = xDisconnectCallback;
                }
                else
                {
                    pxNetwork->xDisconnectCallback = NULL;
                    pxNetwork->xDisconnectCbSubscription = NULL;
                }
                break;
            }
            else
            {
                AwsIotLogInfo(( "Failed to allocate memory for a new connection" ));
                break;
            }
        }
        else if ( ( ++xRetryCount ) < ulConnRetryLimit )
        {
            ( void ) clock_nanosleep( CLOCK_REALTIME, 0, &xDelay, NULL );
        }
    }

    *pxIotNetworkConnection = ( AwsIotDemoNetworkConnection_t ) pxNetwork;

}

uint32_t AwsIotDemo_GetNetworkType( AwsIotDemoNetworkConnection_t xNetworkConnection )
{
    NetworkConnection_t* pxConn = ( NetworkConnection_t* ) xNetworkConnection;
    if( pxConn != NULL )
    {
        return pxConn->ulType;
    }

    return AWSIOT_NETWORK_TYPE_NONE;
}

void AwsIotDemo_DeleteNetworkConnection( AwsIotDemoNetworkConnection_t xNetworkConnection )
{
    NetworkConnection_t* pxNetwork = ( NetworkConnection_t* ) xNetworkConnection;

    if( pxNetwork != NULL )
    {

#if BLE_ENABLED
        if( pxNetwork->ulType == AWSIOT_NETWORK_TYPE_BLE )
        {
            AwsIotMqttBLE_CloseConnection( ( AwsIotMqttBLEConnection_t ) pxNetwork->pvConnection );
            AwsIotMqttBLE_DestroyConnection( ( AwsIotMqttBLEConnection_t ) pxNetwork->pvConnection );
        }
#endif
#if WIFI_ENABLED
        if( pxNetwork->ulType  == AWSIOT_NETWORK_TYPE_WIFI )
        {
            AwsIotNetwork_CloseConnection( ( AwsIotDemoNetworkConnection_t ) pxNetwork->pvConnection );
            AwsIotNetwork_DestroyConnection( ( AwsIotDemoNetworkConnection_t ) pxNetwork->pvConnection );
        }
#endif
        if( pxNetwork->xDisconnectCbSubscription != AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER )
        {
            AwsIotNetworkManager_RemoveSubscription( pxNetwork->xDisconnectCbSubscription );
        }
        vPortFree( pxNetwork );
    }
}
