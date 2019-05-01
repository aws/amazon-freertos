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
 * @file iot_ble_network.c
 * @brief Implementation of Network Interface for BLE using Data transfer service.
 */

#include  <stdbool.h>

#include "FreeRTOS.h"
#include "iot_ble_config.h"
#include "platform/iot_network_ble.h"
#include "iot_ble_data_transfer.h"


static void _callback( IotBleDataTransferChannelStatus_t event, IotBleDataTransferChannel_t *pChannel, void *pContext )
{
    IotBleNetworkContext_t *pNetworkContext = ( IotBleNetworkContext_t * ) pContext;
    switch( event )
    {
        case IOT_BLE_DATA_TRANSFER_CHANNEL_READY:
            IotSemaphore_Post( &pNetworkContext->lock );
            break;
        case IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_COMPLETE:
            pNetworkContext->pCallback( pNetworkContext, pNetworkContext->pContext );
            break;
        default:
            break;
    }

}

static IotBleNetworkContext_t _context = { 0 };

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
    IotNetworkError_t status = IOT_NETWORK_FAILURE;
    IotBleDataTransferChannel_t *pChannel = NULL;

    /* Unused parameters */
    ( void ) pConnectionInfo;
    ( void ) pCredentialInfo;

    pChannel =  IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );
    if( pChannel != NULL )
    {
        _context.pChannel = pChannel;
        if( IotSemaphore_Create( &_context.lock, 0, 1 ) == true )
        {
            IotBleDataTransfer_SetCallback( pChannel, _callback, &_context );
            if( pChannel->isReady == false )
            {
                IotSemaphore_TimedWait( &_context.lock, 5000 );
            }
            if( pChannel->isReady == true )
            {
                ( *pConnection ) = &_context;
                status = IOT_NETWORK_SUCCESS;

            }
            else
            {
                configPRINTF(( "Failed to create connection, ble data transfer channel not ready.\r\n" ));
            }
        }
        else
        {
            configPRINTF(( "Failed to create connection, lock create failed.\r\n" ));
        }
        
    }
    
    return status;
}

IotNetworkError_t IotNetworkBle_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext )
{
    IotBleNetworkContext_t* pNetworkContext = ( IotBleNetworkContext_t* ) pConnection;
    IotNetworkError_t status = IOT_NETWORK_FAILURE;
    if( pNetworkContext != NULL )
    {
        pNetworkContext->pCallback = receiveCallback;
        pNetworkContext->pContext = pContext;
        status = IOT_NETWORK_SUCCESS;
    }
    return status;
}

size_t IotNetworkBle_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength )
{
   IotBleNetworkContext_t* pNetworkContext = ( IotBleNetworkContext_t* ) pConnection;
   return IotBleDataTransfer_Send( ( IotBleDataTransferChannel_t * ) ( pNetworkContext->pChannel ), pMessage, messageLength ); 
}

size_t IotNetworkBle_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested )
{
    IotBleNetworkContext_t* pNetworkContext = ( IotBleNetworkContext_t* ) pConnection;
    return IotBleDataTransfer_Receive(  ( IotBleDataTransferChannel_t * ) ( pNetworkContext->pChannel ), pBuffer, bytesRequested ); 
}

IotNetworkError_t IotNetworkBle_Close( void * pConnection )
{
    IotBleNetworkContext_t* pNetworkContext = ( IotBleNetworkContext_t* ) pConnection;
    IotBleDataTransfer_Close( ( IotBleDataTransferChannel_t * ) ( pNetworkContext->pChannel ) ); 
    return IOT_NETWORK_SUCCESS;
}


IotNetworkError_t IotNetworkBle_Destroy( void * pConnection )
{
    IotBleNetworkContext_t* pNetworkContext = ( IotBleNetworkContext_t* ) pConnection;
    IotBleDataTransfer_Reset(( IotBleDataTransferChannel_t * ) ( pNetworkContext->pChannel ) );
    IotSemaphore_Destroy( &pNetworkContext->lock );
    memset( pNetworkContext, 0x00, sizeof( IotBleNetworkContext_t ) );
    return IOT_NETWORK_SUCCESS;
}
