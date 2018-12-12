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
 * @file aws_iot_tests_network.c
 * @brief Common network function implementations for the tests.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <string.h>

/* MQTT include. */
#include "aws_iot_mqtt.h"

/* Platform layer includes. */
#include "platform/aws_iot_network.h"

/*-----------------------------------------------------------*/

/**
 * @brief Network interface setup function for the tests.
 *
 * Creates a global network connection to be used by the tests.
 * @return true if setup succeeded; false otherwise.
 *
 * @see #AwsIotTest_NetworkCleanup
 */
bool AwsIotTest_NetworkSetup( void );

/**
 * @brief Network interface cleanup function for the tests.
 *
 * @see #AwsIotTest_NetworkSetup
 */
void AwsIotTest_NetworkCleanup( void );

/**
 * @brief Network interface connect function for the tests.
 *
 * Creates a new network connection for use with MQTT.
 *
 * @param[out] pNewConnection The handle by which this new connection will be referenced.
 * @param[in] pMqttConnection The MQTT connection associated with the new network connection.
 *
 * @return true if a new network connection was successfully created; false otherwise.
 */
bool AwsIotTest_NetworkConnect( void ** const pNewConnection,
                                AwsIotMqttConnection_t * pMqttConnection );

/**
 * @brief Network interface close connection function for the tests.
 *
 * @param[in] pDisconnectContext The connection to close. Pass NULL to close
 * the global network connection created by #AwsIotTest_NetworkSetup.
 */
void AwsIotTest_NetworkClose( void * pDisconnectContext );

/**
 * @brief Network interface cleanup function for the tests.
 *
 * @param[in] pNetworkConnection The connection to destroy.
 */
void AwsIotTest_NetworkDestroy( void * pNetworkConnection );

/*-----------------------------------------------------------*/

/**
 * @brief The network connection shared among the tests.
 */
static AwsIotNetworkConnection_t _networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;

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
    /* Initialize the network library. */
    if( AwsIotNetwork_Init() != AWS_IOT_NETWORK_SUCCESS )
    {
        return false;
    }

    if( AwsIotTest_NetworkConnect( &_networkConnection,
                                   &_AwsIotTestMqttConnection ) == false )
    {
        AwsIotNetwork_Cleanup();

        return false;
    }

    /* Set the members of the network interface. */
    _AwsIotTestNetworkInterface.pDisconnectContext = NULL;
    _AwsIotTestNetworkInterface.disconnect = AwsIotTest_NetworkClose;
    _AwsIotTestNetworkInterface.pSendContext = ( void * ) _networkConnection;
    _AwsIotTestNetworkInterface.send = AwsIotNetwork_Send;

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotTest_NetworkCleanup( void )
{
    /* Close the TCP connection to the server. */
    if( _networkConnection != AWS_IOT_NETWORK_CONNECTION_INITIALIZER )
    {
        AwsIotTest_NetworkClose( NULL );
        AwsIotTest_NetworkDestroy( _networkConnection );
        _networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
    }

    /* Clean up the network library. */
    AwsIotNetwork_Cleanup();

    /* Clear the network interface. */
    ( void ) memset( &_AwsIotTestNetworkInterface, 0x00, sizeof( AwsIotMqttNetIf_t ) );
}

/*-----------------------------------------------------------*/

bool AwsIotTest_NetworkConnect( void ** const pNewConnection,
                                AwsIotMqttConnection_t * pMqttConnection )
{
    AwsIotNetworkConnection_t newConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
    AwsIotNetworkTlsInfo_t * pTlsInfo = NULL;

    /* Set up TLS if the endpoint is secured. These tests should always use ALPN. */
    #if AWS_IOT_TEST_SECURED_CONNECTION == 1
        AwsIotNetworkTlsInfo_t tlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
        pTlsInfo = &tlsInfo;

        /* Set credentials for secured connection. Lengths are optional on some
         * platforms and only set if needed. */
        tlsInfo.pRootCa = AWS_IOT_TEST_ROOT_CA;
        tlsInfo.pClientCert = AWS_IOT_TEST_CLIENT_CERT;
        tlsInfo.pPrivateKey = AWS_IOT_TEST_PRIVATE_KEY;

        #ifdef AWS_IOT_TEST_ROOT_CA_LENGTH
            tlsInfo.rootCaLength = AWS_IOT_TEST_ROOT_CA_LENGTH;
        #endif

        #ifdef AWS_IOT_TEST_CLIENT_CERT_LENGTH
            tlsInfo.clientCertLength = AWS_IOT_TEST_CLIENT_CERT_LENGTH;
        #endif

        #ifdef AWS_IOT_TEST_PRIVATE_KEY_LENGTH
            tlsInfo.privateKeyLength = AWS_IOT_TEST_PRIVATE_KEY_LENGTH;
        #endif
    #endif /* if AWS_IOT_TEST_SECURED_CONNECTION == 0 */

    /* Open a connection to the test server. */
    if( AwsIotNetwork_CreateConnection( &newConnection,
                                        AWS_IOT_TEST_SERVER,
                                        AWS_IOT_TEST_PORT,
                                        pTlsInfo ) != AWS_IOT_NETWORK_SUCCESS )
    {
        return false;
    }

    /* Set the MQTT receive callback. */
    if( AwsIotNetwork_SetMqttReceiveCallback( newConnection,
                                              pMqttConnection,
                                              AwsIotMqtt_ReceiveCallback ) != AWS_IOT_NETWORK_SUCCESS )
    {
        AwsIotNetwork_CloseConnection( newConnection );
        AwsIotNetwork_DestroyConnection( newConnection );
        *pNewConnection = NULL;

        return false;
    }

    /* Set the output parameter. */
    *pNewConnection = ( void * ) newConnection;

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotTest_NetworkClose( void * pDisconnectContext )
{
    AwsIotNetworkConnection_t networkConnection = ( AwsIotNetworkConnection_t ) pDisconnectContext;

    /* Close the provided network handle; if that is uninitialized, close the
     * global network handle. */
    if( ( networkConnection != AWS_IOT_NETWORK_CONNECTION_INITIALIZER ) &&
        ( networkConnection != _networkConnection ) )
    {
        AwsIotNetwork_CloseConnection( networkConnection );
    }
    else if( _networkConnection != AWS_IOT_NETWORK_CONNECTION_INITIALIZER )
    {
        AwsIotNetwork_CloseConnection( _networkConnection );
    }
}

/*-----------------------------------------------------------*/

void AwsIotTest_NetworkDestroy( void * pNetworkConnection )
{
    AwsIotNetworkConnection_t networkConnection = ( AwsIotNetworkConnection_t ) pNetworkConnection;

    if( ( networkConnection != NULL ) &&
        ( networkConnection != _networkConnection ) )
    {
        /* Wrap the network interface's destroy function. */
        AwsIotNetwork_DestroyConnection( ( AwsIotNetworkConnection_t ) pNetworkConnection );
    }
    else
    {
        if( _networkConnection != AWS_IOT_NETWORK_CONNECTION_INITIALIZER )
        {
            AwsIotNetwork_DestroyConnection( ( AwsIotNetworkConnection_t ) _networkConnection );
            _networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
        }
    }
}

/*-----------------------------------------------------------*/
