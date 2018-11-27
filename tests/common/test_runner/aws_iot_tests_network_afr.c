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
 * @file aws_iot_mqtt_tests_network_afr.c
 * @brief Common network function implementations for the MQTT system tests.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Amazon FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_secure_sockets.h"
#include "aws_clientcredential.h"

/* Standard includes. */
#include <stdbool.h>
#include <string.h>

/* MQTT include. */
#include "aws_iot_mqtt.h"

/* Platform layer includes. */
#include "platform/aws_iot_network_afr.h"

/*-----------------------------------------------------------*/

/**
 * @brief Network interface setup function for the MQTT tests.
 *
 * Creates a global network connection to be used by the tests.
 * @return true if setup succeeded; false otherwise.
 *
 * @see #AwsIotTest_NetworkCleanup
 */
bool AwsIotTest_NetworkSetup( void );

/**
 * @brief Network interface cleanup function for the MQTT tests.
 *
 * @see #AwsIotTest_NetworkSetup
 */
void AwsIotTest_NetworkCleanup( void );

/**
 * @brief Network interface connect function for the MQTT tests.
 *
 * Creates a new network connection.
 * @param[out] pNewConnection The handle by which this new connection will be referenced.
 * @param[in] pMqttConnection The MQTT connection associated with the new network connection.
 *
 * @return true if a new network connection was successfully created; false otherwise.
 */
bool AwsIotTest_NetworkConnect( void ** const pNewConnection,
                                AwsIotMqttConnection_t * pMqttConnection );

/**
 * @brief Network interface close connection function for the MQTT tests.
 *
 * @param[in] pDisconnectContext The connection to close. Pass NULL to close
 * the global network connection created by #AwsIotTest_NetworkSetup.
 */
void AwsIotTest_NetworkClose( void * pDisconnectContext );

/**
 * @brief Network interface cleanup function for the MQTT tests.
 *
 * @param[in] pNetworkConnection The connection to destroy.
 */
void AwsIotTest_NetworkDestroy( void * pNetworkConnection );

/*-----------------------------------------------------------*/

/**
 * @brief The network connection shared among the tests.
 */
static AwsIotNetworkConnection_t _networkConnection = AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER;

/**
 * @brief The MQTT network interface shared among the tests.
 */
AwsIotMqttNetIf_t _AwsIotTestNetworkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;

/**
 * @brief The MQTT connection shared among the tests.
 */
AwsIotMqttConnection_t _AwsIotTestMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;

/*-----------------------------------------------------------*/

bool AwsIotTest_NetworkSetup( void )
{
    /* Establish the global network connection. */
    bool xStatus = AwsIotTest_NetworkConnect( &_networkConnection,
                                              &_AwsIotTestMqttConnection );

    /* Set the members of the connection interface. */
    if( xStatus == true )
    {
        _AwsIotTestNetworkInterface.pDisconnectContext = ( void * ) _networkConnection;
        _AwsIotTestNetworkInterface.pSendContext = ( void * ) _networkConnection;
        _AwsIotTestNetworkInterface.disconnect = AwsIotTest_NetworkClose;
        _AwsIotTestNetworkInterface.send = AwsIotNetwork_Send;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

void AwsIotTest_NetworkCleanup( void )
{
    /* Close the global network connection. */
    if( _networkConnection != NULL )
    {
        AwsIotTest_NetworkClose( NULL );
        AwsIotTest_NetworkDestroy( _networkConnection );
        _networkConnection = NULL;
    }

    /* Clear the network interface. */
    ( void ) memset( &_AwsIotTestNetworkInterface, 0x00, sizeof( AwsIotMqttNetIf_t ) );
}

/*-----------------------------------------------------------*/

bool AwsIotTest_NetworkConnect( void ** const pNewConnection,
                                AwsIotMqttConnection_t * pMqttConnection )
{
    bool xStatus = true;
    AwsIotNetworkConnection_t xNewConnection = AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER;
    AwsIotNetworkConnectParams_t connectParams = AWS_IOT_NETWORK_AFR_CONNECT_PARAMS_INITIALIZER;

    /* Establish network connection. */
    connectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
    connectParams.usPort = clientcredentialMQTT_BROKER_PORT;
    connectParams.flags = AWS_IOT_NETWORK_AFR_FLAG_SECURED |
                          AWS_IOT_NETWORK_AFR_FLAG_SNI;

    if( connectParams.usPort == securesocketsDEFAULT_TLS_DESTINATION_PORT )
    {
        connectParams.flags |= AWS_IOT_NETWORK_AFR_FLAG_ALPN;
    }

    xStatus = AwsIotNetwork_CreateConnection( &connectParams, &xNewConnection );

    /* Set MQTT receive callback. */
    if( xStatus == true )
    {
        xStatus = AwsIotNetwork_CreateMqttReceiveTask( xNewConnection,
                                                       pMqttConnection,
                                                       AWS_IOT_MQTT_RECEIVE_TASK_PRIORITY,
                                                       AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE );

        if( xStatus == false )
        {
            AwsIotNetwork_CloseConnection( xNewConnection );
            AwsIotNetwork_DestroyConnection( xNewConnection );
        }
        else
        {
            *pNewConnection = ( void * ) xNewConnection;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

void AwsIotTest_NetworkClose( void * pDisconnectContext )
{
    AwsIotNetworkConnection_t xConnection = ( AwsIotNetworkConnection_t ) pDisconnectContext;

    /* Close the provided network handle; if that is NULL, close the
     * global network handle. */
    if( ( xConnection != NULL ) &&
        ( xConnection != _networkConnection ) )
    {
        AwsIotNetwork_CloseConnection( xConnection );
    }
    else
    {
        AwsIotNetwork_CloseConnection( _networkConnection );
    }
}

/*-----------------------------------------------------------*/

void AwsIotTest_NetworkDestroy( void * pNetworkConnection )
{
    AwsIotNetworkConnection_t xConnection = ( AwsIotNetworkConnection_t ) pNetworkConnection;

    /* Destroy the provided network handle; if that is NULL, destroy the
     * global network handle. */
    if( ( xConnection != NULL ) &&
        ( xConnection != _networkConnection ) )
    {
        /* Wrap the network interface's destroy function. */
        AwsIotNetwork_DestroyConnection( ( AwsIotNetworkConnection_t ) pNetworkConnection );
    }
    else
    {
        if( _networkConnection != AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER )
        {
            AwsIotNetwork_DestroyConnection( ( AwsIotNetworkConnection_t ) _networkConnection );
            _networkConnection = AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER;
        }
    }
}

/*-----------------------------------------------------------*/
