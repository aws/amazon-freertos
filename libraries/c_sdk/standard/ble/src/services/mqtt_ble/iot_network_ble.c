/*
 * FreeRTOS BLE V2.2.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_network_ble.c
 * @brief Implementation of network interface for BLE. This implementation is provided as a SHIM
 * layer of backwards compatibility for the V4 MQTT library.
 */

#include <stdbool.h>
#include <stdint.h>


/* Config files needs to be defined first. */
#include "iot_config.h"
#include "iot_ble_config.h"

#include "FreeRTOS.h"

#include "platform/iot_threads.h"
#include "platform/iot_network_ble.h"
#include "iot_ble_data_transfer.h"
#include "iot_ble_mqtt_transport.h"


#define IOT_BLE_MAX_NETWORK_CONNECTIONS    ( 1 )

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/c_sdk/standard/ble/include/iot_ble_mqtt_transport.h.
 */
struct NetworkContext
{
    BleTransportParams_t * pParams;
};

/**
 * @brief Structure holds the context associated with a ble connection.
 */
typedef struct IotBleNetworkConnection
{
    BaseType_t xIsUsed;                                      /* Boolean flag to indicate if the context is being used. */
    NetworkContext_t xContext;                               /* Network Context structure to hold BleTransportParams_t pointer */
    IotSemaphore_t xChannelSem;                              /* Semaphore used to block on the channel. */
    IotNetworkReceiveCallback_t pCallback;                   /* Callback registered by user to get notified of receipt of new data. */
    void * pUserContext;                                     /* User context associated with the callback registered. */
    uint8_t buffer[ IOT_BLE_NETWORK_INTERFACE_BUFFER_SIZE ]; /* Buffer used internally by BLE transport to queue the data. */
    BleTransportParams_t xBleTransportParams;                /* Ble Transport Parameters structure to hold the ble transport channel */
} IotBleNetworkConnection_t;

/*-----------------------------------------------------------*/

/**
 * @brief An implementation of #IotNetworkInterface_t::create using
 * transport interface for BLE.
 */
static IotNetworkError_t IotNetworkBle_Create( void * pConnectionInfo,
                                               void * pCredentialInfo,
                                               void ** pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::setReceiveCallback using
 * transport interface for BLE.
 */
static IotNetworkError_t IotNetworkBle_SetReceiveCallback( void * pConnection,
                                                           IotNetworkReceiveCallback_t receiveCallback,
                                                           void * pContext );

/**
 * @brief An implementation of #IotNetworkInterface_t::send using
 * transport interface for BLE.
 */
static size_t IotNetworkBle_Send( void * pConnection,
                                  const uint8_t * pMessage,
                                  size_t messageLength );

/**
 * @brief An implementation of #IotNetworkInterface_t::receive using
 * transport interface for BLE.
 */
static size_t IotNetworkBle_Receive( void * pConnection,
                                     uint8_t * pBuffer,
                                     size_t bytesRequested );

/**
 * @brief An implementation of #IotNetworkInterface_t::close fusing
 * transport interface for BLE.
 */
static IotNetworkError_t IotNetworkBle_Close( void * pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::destroy using
 * transport interface for BLE.
 */
static IotNetworkError_t IotNetworkBle_Destroy( void * pConnection );


/**
 * @brief Static array of all network connections over BLE.
 */
static IotBleNetworkConnection_t xNetworkConnections[ IOT_BLE_MAX_NETWORK_CONNECTIONS ] = { 0 };

static void _callback( IotBleDataTransferChannelEvent_t event,
                       IotBleDataTransferChannel_t * pChannel,
                       void * pContext )
{
    IotBleNetworkConnection_t * pBleConnection = ( IotBleNetworkConnection_t * ) pContext;

    switch( event )
    {
        case IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED:
            IotSemaphore_Post( &pBleConnection->xChannelSem );
            break;

        case IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED:
            ( void ) IotBleMqttTransportAcceptData( &pBleConnection->xContext );

            if( pBleConnection->pCallback != NULL )
            {
                pBleConnection->pCallback( pBleConnection, pBleConnection->pUserContext );
            }

            break;

        default:
            break;
    }
}

/**
 * @brief An #IotNetworkInterface_t that uses the functions in this file.
 */
const IotNetworkInterface_t IotNetworkBle =
{
    .create             = IotNetworkBle_Create,
    .setReceiveCallback = IotNetworkBle_SetReceiveCallback,
    .send               = IotNetworkBle_Send,
    .receive            = IotNetworkBle_Receive,
    .close              = IotNetworkBle_Close,
    .destroy            = IotNetworkBle_Destroy
};

IotNetworkError_t IotNetworkBle_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void ** pConnection )
{
    IotNetworkError_t status = IOT_NETWORK_SUCCESS;
    IotBleNetworkConnection_t * pBleConnection = NULL;
    NetworkContext_t * pContext = NULL;
    BleTransportParams_t * pBleTransportParams = NULL;
    size_t xIndex = 0;
    bool semCreated = false;

    /* Unused parameters */
    ( void ) pConnectionInfo;
    ( void ) pCredentialInfo;

    for( ; xIndex < IOT_BLE_MAX_NETWORK_CONNECTIONS; xIndex++ )
    {
        if( xNetworkConnections[ xIndex ].xIsUsed == pdFALSE )
        {
            pBleConnection = &xNetworkConnections[ xIndex ];
            pBleConnection->xIsUsed = pdTRUE;
        }
    }

    if( !pBleConnection )
    {
        configPRINTF( ( "No free BLE connection descriptors available.\r\n" ) );
        status = IOT_NETWORK_NO_MEMORY;
    }

    if( status == IOT_NETWORK_SUCCESS )
    {
        pContext = &pBleConnection->xContext;
        pContext->pParams = &pBleConnection->xBleTransportParams;
        pBleTransportParams = pContext->pParams;

        pBleTransportParams->pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );

        if( !pBleTransportParams->pChannel )
        {
            configPRINTF( ( "Failed to open BLE data transfer channel.\r\n" ) );
            status = IOT_NETWORK_SYSTEM_ERROR;
        }
    }

    if( status == IOT_NETWORK_SUCCESS )
    {
        /* Must initialize the channel, context must contain the buffer and buf size at this point. */
        if( IotBleMqttTransportInit( pBleConnection->buffer, IOT_BLE_NETWORK_INTERFACE_BUFFER_SIZE, pContext ) != true )
        {
            configPRINTF( ( "Failed to initialize transport interface for BLE.\r\n" ) );
            status = IOT_NETWORK_FAILURE;
        }
    }

    if( status == IOT_NETWORK_SUCCESS )
    {
        semCreated = IotSemaphore_Create( &pBleConnection->xChannelSem, 0, 1 );

        if( !semCreated )
        {
            configPRINTF( ( "Failed to create BLE data transfer channel semaphore.\r\n" ) );
            status = IOT_NETWORK_NO_MEMORY;
        }
    }

    if( status == IOT_NETWORK_SUCCESS )
    {
        IotBleDataTransfer_SetCallback( pBleTransportParams->pChannel, _callback, pBleConnection );

        if( IotSemaphore_TimedWait( &pBleConnection->xChannelSem,
                                    IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS ) != true )
        {
            configPRINTF( ( "Failed to create BLE network connection after %d milliseconds.", IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS ) );
            status = IOT_NETWORK_FAILURE;
        }
        else
        {
            ( *pConnection ) = pBleConnection;
        }
    }

    if( status != IOT_NETWORK_SUCCESS )
    {
        if( ( pContext != NULL ) && ( pBleTransportParams->pChannel != NULL ) )
        {
            IotBleMqttTransportCleanup( pContext );
            IotBleDataTransfer_Reset( pBleTransportParams->pChannel );
        }

        if( semCreated )
        {
            IotSemaphore_Destroy( &pBleConnection->xChannelSem );
        }

        if( pBleConnection )
        {
            memset( pBleConnection, 0x00, sizeof( IotBleNetworkConnection_t ) );
        }
    }

    return status;
}

IotNetworkError_t IotNetworkBle_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext )
{
    IotBleNetworkConnection_t * pBleConnection = ( IotBleNetworkConnection_t * ) pConnection;
    IotNetworkError_t status;

    if( pBleConnection )
    {
        pBleConnection->pCallback = receiveCallback;
        pBleConnection->pUserContext = pContext;
        status = IOT_NETWORK_SUCCESS;
    }
    else
    {
        status = IOT_NETWORK_FAILURE;
    }

    return status;
}

size_t IotNetworkBle_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength )
{
    IotBleNetworkConnection_t * pBleConnection = ( IotBleNetworkConnection_t * ) pConnection;

    return ( size_t ) IotBleMqttTransportSend( &pBleConnection->xContext, ( uint8_t * ) pMessage, messageLength );
}

size_t IotNetworkBle_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested )
{
    IotBleNetworkConnection_t * pBleConnection = ( IotBleNetworkConnection_t * ) pConnection;

    return ( size_t ) IotBleMqttTransportReceive( &pBleConnection->xContext, pBuffer, bytesRequested );
}

IotNetworkError_t IotNetworkBle_Close( void * pConnection )
{
    IotBleNetworkConnection_t * pBleConnection = ( IotBleNetworkConnection_t * ) pConnection;

    IotBleDataTransfer_Close( pBleConnection->xBleTransportParams.pChannel );
    IotBleMqttTransportCleanup( &( pBleConnection->xContext ) );
    IotBleDataTransfer_Reset( pBleConnection->xBleTransportParams.pChannel );
    IotSemaphore_Destroy( &pBleConnection->xChannelSem );
    memset( pBleConnection, 0x00, sizeof( IotBleNetworkConnection_t ) );
    return IOT_NETWORK_SUCCESS;
}


IotNetworkError_t IotNetworkBle_Destroy( void * pConnection )
{
    return IOT_NETWORK_SUCCESS;
}
