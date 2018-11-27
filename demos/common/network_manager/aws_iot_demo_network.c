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
#include "platform/aws_iot_network_afr.h"
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

static void prvNetworkSubscriptionWrapper ( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext );

#if BLE_ENABLED
static bool prbCreateMqttBLEConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvCpnnection );
#endif

#if WIFI_ENABLED
static bool prbCreateSecureSocketConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void **ppvConnection );
#endif

static uint32_t prxCreateNetworkConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvConnection );

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
        if( prbCreateMqttBLEConnection( pxNetworkInterface, pxMqttConnection, ppvConnection ) == true )
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
    bool xStatus = false;
    static AwsIotDemoNetworkConnection_t xWifiConnection = AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER;
    AwsIotNetworkConnectParams_t xConnectParams = AWS_IOT_NETWORK_AFR_CONNECT_PARAMS_INITIALIZER;

    xConnectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
    xConnectParams.usPort = clientcredentialMQTT_BROKER_PORT;
    xConnectParams.flags = AWS_IOT_NETWORK_AFR_FLAG_SECURED;

    /* If connecting over port 443, set ALPN flag. */
    if( xConnectParams.usPort == securesocketsDEFAULT_TLS_DESTINATION_PORT )
    {
        xConnectParams.flags |= AWS_IOT_NETWORK_AFR_FLAG_ALPN;
    }

    /* Override the default root CA certificates by setting pcServerCertificate
     * and xServerCertificateSize. SNI may need to be disabled if not using the
     * default certificates. */
    xConnectParams.pcServerCertificate = NULL;
    xConnectParams.xServerCertificateSize = 0;
    xConnectParams.flags |= AWS_IOT_NETWORK_AFR_FLAG_SNI;

    /* Establish a TCP connection to the MQTT server. */
    xStatus =  AwsIotNetwork_CreateConnection( &xConnectParams,
                                        &xWifiConnection );

    if( xStatus == true )
    {
        /* Create the task that processes incoming MQTT data. */
        xStatus = AwsIotNetwork_CreateMqttReceiveTask( xWifiConnection,
                                                 pxMqttConnection,
                                                 AWS_IOT_MQTT_RECEIVE_TASK_PRIORITY,
                                                 AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE );
    }

    if( xStatus == true )
    {
        pxNetworkInterface->pDisconnectContext = ( void * ) xWifiConnection;
        pxNetworkInterface->pSendContext = ( void * ) xWifiConnection;
        pxNetworkInterface->disconnect = AwsIotNetwork_CloseConnection;
        pxNetworkInterface->send = AwsIotNetwork_Send;

        *ppvConnection = ( void* ) xWifiConnection;
    }
    else
    {
        if( xWifiConnection != AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER )
        {
            AwsIotNetwork_CloseConnection( xWifiConnection );
            AwsIotNetwork_DestroyConnection( xWifiConnection );
        }

    }

    return xStatus;
}
#endif

#if BLE_ENABLED
static bool prbCreateMqttBLEConnection(
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        void** ppvConnection )
{
    bool xStatus = false;
    static AwsIotMqttBLEConnection_t xBLEConnection = AWS_IOT_MQTT_BLE_CONNECTION_INITIALIZER;

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

void ulCreateNetworkConnectionWithRetry(
        uint32_t ulConnIntervalSec,
        uint32_t ulConnRetryLimit,
        AwsIotMqttNetIf_t* pxNetworkInterface,
        AwsIotMqttConnection_t* pxMqttConnection,
        NetworkDisconnectedCallback_t xDisconnectCallback,
        AwsIotDemoNetworkConnection_t* pxIotNetworkConnection )
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

uint32_t ulGetConnectedNetworkType( AwsIotDemoNetworkConnection_t xNetworkConnection )
{
    NetworkConnection_t* pxConn = ( NetworkConnection_t* ) xNetworkConnection;
    if( pxConn != NULL )
    {
        return pxConn->ulType;
    }

    return AWSIOT_NETWORK_TYPE_NONE;
}

void vDeleteNetworkConnection( AwsIotDemoNetworkConnection_t xNetworkConnection )
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
