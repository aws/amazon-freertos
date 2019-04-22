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
 * @file aws_iot_mqtt_ble.c
 * @brief GATT service for transferring data over BLE
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

#include "iot_ble_config.h"
#include "iot_ble_data_transfer.h"
#include "task.h"

/**
 * @brief Maximum bytes which can be transferred at a time through the BLE connection.
 */
#define IOT_BLE_DATA_TRANSFER_LENGTH( mtu )    ( ( mtu ) - 3 )

/**
 *@brief Size of the buffer used to store outstanding bytes to be send out.
 */
#define IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE       ( 1024 )

/**
 * @brief Size of the buffer to hold the received message
 *
 */
#define IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE       ( 1024 )

/**
 * @brief The default timeout in milliseconds for sending a message to the proxy.
 */
#define IOT_BLE_DATA_TRANSFER_SEND_TIMEOUT_DEFAULT_MS    ( 2000 )


#define UUID128( value )                 \
{                                        \
    .uu.uu128 = value,                   \
    .ucType = eBTuuidType128             \
}

#define UUID16( value )                  \
{                                        \
    .uu.uu16 = value,                    \
    .ucType = eBTuuidType16              \
}

#define _SERVICE_ATTR_TABLE( INST_ID )                                                    \
{                                                                                         \
    {                                                                                     \
        .xServiceUUID = UUID128( IOT_BLE_DATA_TRANSFER_SERVICE_UUID( INST_ID ) )          \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = UUID128( IOT_BLE_DATA_TRANSFER_TX_CHAR_UUID( INST_ID ) ),     \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),                                   \
            .xProperties  = ( eBTPropRead | eBTPropNotify )                               \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbDescriptor,                                                \
        .xCharacteristicDescr =                                                           \
        {                                                                                 \
            .xUuid        = UUID16( IOT_BLE_DATA_TRANSFER_CCFG_UUID ),                    \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )          \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = UUID128( IOT_BLE_DATA_TRANSFER_RX_CHAR_UUID( INST_ID ) ),     \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),         \
            .xProperties  = ( eBTPropRead | eBTPropWrite )                                \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
       {                                                                                  \
            .xUuid        = UUID128(IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_UUID( INST_ID )), \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),                                   \
            .xProperties  = ( eBTPropRead | eBTPropNotify )                               \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbDescriptor,                                                \
        .xCharacteristicDescr =                                                           \
       {                                                                                  \
            .xUuid        = UUID16( IOT_BLE_DATA_TRANSFER_CCFG_UUID ),                    \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )          \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
       {                                                                                  \
            .xUuid        = UUID128(IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR_UUID( INST_ID )), \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),         \
            .xProperties  = ( eBTPropRead | eBTPropWrite )                                \
        }                                                                                 \
    }                                                                                     \
}


#define CHAR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ ch_idx ] )

#define CHAR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ ch_idx ].xCharacteristic.xUuid )

#define DESCR_HANDLE( svc, descr_idx )    ( ( svc )->pusHandlesBuffer[ descr_idx ] )

/*
 * @brief Gets a service instance given a GATT service.
 */
static IotBleDataTransferService_t * _getServiceFromHandle( uint16_t handle );


/**
 * @brief Reallocate a buffer; data is also copied from the old
 * buffer into the new buffer.
 *
 * @param[in] oldBuffer The current network receive buffer.
 * @param[in] oldBufferSize The size (in bytes) of oldBuffer.
 * @param[in] newBufferSize Requested size of the reallocated buffer.
 * copied into the new buffer.
 *
 * @return Pointer to a new, reallocated buffer; NULL if buffer reallocation failed.
 */
static uint8_t * _reallocBuffer( uint8_t * oldBuffer,
                                 size_t oldBufferSize,
                                 size_t newBufferSize );


/*
 * @brief Callback to register for events (read) on TX message characteristic.
 */
static void _TXMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback to register for events (write) on RX message characteristic.
 *
 */
static void _RXMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback to register for events (read) on TX large message characteristic.
 * Buffers a large message and sends the message in chunks of size MTU at a time as response
 * to the read request. Keeps the buffer until last message is read.
 */
static void _TXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback to register for events (write) on RX large message characteristic.
 * Copies the individual write packets into a buffer untill a packet less than BLE MTU size
 * is received. Sends the buffered message to the upper layer.
 */
static void _RXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback for Client Characteristic Configuration Descriptor events.
 */
static void _clientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam );


/*
 * @brief Resets the send and receive buffer for the given data transfer Service.
 * Any pending connection should be closed or the service should be disabled
 * prior to resetting the buffer.
 *
 * @param[in]  pService Pointer to the data transfer service.
 */
static void _resetBuffer( IotBleDataTransferService_t * pService );

/*
 * @brief Callback for BLE connect/disconnect.
 */
static void _connectionCallback( BTStatus_t status,
                                 uint16_t connId,
                                 bool connected,
                                 BTBdaddr_t * pRemoteBdAddr );

/*
 * @brief Callback to receive notification when the MTU of the BLE connection changes.
 */
static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t Mtu );

/*
 * @brief stdio.h conflict with posix library on some platform so using extern snprintf so not to include it.
 */
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );
/*------------------------------------------------------------------------------------*/


static const BTAttribute_t _pAttributeTable[ IOT_BLE_MAX_DATA_TRANSFER_SERVICES ][ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ] = 
{
    _SERVICE_ATTR_TABLE( IOT_BLE_MQTT_SERVICE_ID ),
    _SERVICE_ATTR_TABLE( IOT_BLE_WIFI_PROVISIONING_SERVICE_ID )
};

static IotBleDataTransferService_t _dataTransferServices[ IOT_BLE_MAX_DATA_TRANSFER_SERVICES] = { 0 };

static BTService_t _BLEServices[ IOT_BLE_MAX_DATA_TRANSFER_SERVICES ] = { 0 };

static uint16_t _handlesBuffer[ IOT_BLE_MAX_DATA_TRANSFER_SERVICES ][ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ] = { 0 };

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

/*-----------------------------------------------------------------------------------------------------*/

static const IotBleAttributeEventCallback_t _callbacks[ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ] =
{
    _TXMesgCharCallback,
    _clientCharCfgDescrCallback,
    _RXMesgCharCallback,
    _TXLargeMesgCharCallback,
    _clientCharCfgDescrCallback,
    _RXLargeMesgCharCallback
};

/**
 * @brief Holds the MTU Size for the BLE connection.
 */
static uint16_t bleMTU = IOT_BLE_PREFERRED_MTU_SIZE;

/**
 * @brief Holds the state of BLE connection.
 */
static bool bleConnected = false;

/**
 * @brief Holds the connection id for the BLE connection
 */
static uint16_t bleConnectionID;


/*-----------------------------------------------------------*/

static IotBleDataTransferService_t * _getServiceFromHandle( uint16_t handle )
{
    uint8_t id;
    IotBleDataTransferService_t * pService = NULL;

    for( id = 0; id < IOT_BLE_MAX_DATA_TRANSFER_SERVICES; id++ )
    {
        /* Check that the handle is included in the service. */
        if( ( handle > _handlesBuffer[ id ][ 0 ] ) &&
            ( handle <= _handlesBuffer[ id ][ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES - 1 ] ) )
        {
            pService = &_dataTransferServices[ id ];
            break;
        }
    }

    return pService;
}

/*-----------------------------------------------------------*/

static bool _sendNotification( IotBleDataTransferService_t * pService,
                               IotBleDataTransferAttributes_t characteristic,
                               uint8_t * pData,
                               size_t len )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    bool status = false;

    attrData.handle = CHAR_HANDLE( pService->pGattServicePtr, characteristic );
    attrData.uuid = CHAR_UUID( pService->pGattServicePtr, characteristic );
    attrData.pData = pData;
    attrData.size = len;
    resp.pAttrData = &attrData;
    resp.attrDataOffset = 0;
    resp.eventStatus = eBTStatusSuccess;
    resp.rspErrorStatus = eBTRspErrorNone;

    if( IotBle_SendIndication( &resp, bleConnectionID, false ) == eBTStatusSuccess )
    {
        status = true;
    }

    return status;
}

static uint8_t * _reallocBuffer( uint8_t * oldBuffer,
                                 size_t oldBufferSize,
                                 size_t newBufferSize )
{
    uint8_t * newBuffer = NULL;

    /* This function should not be called with a smaller new buffer size or a
     * copy offset greater than the old buffer size. */
    configASSERT( newBufferSize >= oldBufferSize );

    /* Allocate a larger receive buffer. */
    newBuffer = pvPortMalloc( newBufferSize );

    /* Copy data into the new buffer. */
    if( newBuffer != NULL )
    {
        ( void ) memcpy( newBuffer,
                         oldBuffer,
                         oldBufferSize );
    }

    /* Free the old buffer. */
    vPortFree( oldBuffer );

    return newBuffer;
}

/*-----------------------------------------------------------*/

void _resetBuffer( IotBleDataTransferService_t * pService )
{

    IotSemaphore_Wait( &pService->connection.sendLock );
    ( void ) xStreamBufferReset( pService->connection.sendBuffer );
    IotSemaphore_Post(&pService->connection.sendLock);
}

/*-----------------------------------------------------------*/

static void _TXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleReadEventParams_t * pReadParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( pEventParam->xEventType == eBLERead )
    {
        pReadParam = pEventParam->pParamRead;
        resp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
        resp.pAttrData->pData = NULL;
        resp.pAttrData->size = 0;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;

        IotBle_SendResponse( &resp, pReadParam->connId, pReadParam->transId );
    }
}

/*-----------------------------------------------------------*/

static void _TXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleReadEventParams_t * pReadParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleDataTransferService_t * pService;
    size_t xBytesToSend = 0;
    uint8_t * pData;
    BTStatus_t status;


    if( pEventParam->xEventType == eBLERead )
    {
        pReadParam = pEventParam->pParamRead;

        pService = _getServiceFromHandle( pReadParam->attrHandle );

        resp.pAttrData = &attrData;
        resp.rspErrorStatus = eBTRspErrorNone;
        resp.eventStatus = eBTStatusFail;
        resp.attrDataOffset = 0;
        resp.pAttrData->handle = pReadParam->attrHandle;
        xBytesToSend = IOT_BLE_DATA_TRANSFER_LENGTH( bleMTU );

        pData = pvPortMalloc( xBytesToSend );

        if( pData != NULL )
        {
            xBytesToSend = xStreamBufferReceive( pService->connection.sendBuffer, pData, xBytesToSend, ( TickType_t ) 0ULL );
            resp.pAttrData->pData = pData;
            resp.pAttrData->size = xBytesToSend;
            resp.attrDataOffset = 0;
            resp.eventStatus = eBTStatusSuccess;

            status = IotBle_SendResponse( &resp, pReadParam->connId, pReadParam->transId );

            if( status != eBTStatusSuccess )
            {
                configPRINTF(( "Failed to send large object chunk through ble connection.\r\n" ));
            }

            vPortFree( pData );
        }

        /* If this was the last chunk of a large message, release the send lock */
        if( ( resp.eventStatus == eBTStatusSuccess ) &&
            ( xBytesToSend < IOT_BLE_DATA_TRANSFER_LENGTH( bleMTU ) ) )
        {
            IotSemaphore_Post( &pService->connection.sendLock );
        }
    }
}

/*-----------------------------------------------------------*/
static bool _checkAndSizeReceiveBuffer( uint8_t ** pRecvBuffer,
                                        uint32_t * bufferLength,
                                        uint32_t * bufferOffset,
                                        uint32_t rxDataLength )
{
    bool result = true;

    /**
     * Create a new buffer if the buffer is empty.
     */
    if( *pRecvBuffer == NULL )
    {
        *pRecvBuffer = pvPortMalloc( IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE );

        if( *pRecvBuffer != NULL )
        {
            *bufferLength = IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        /**
         *  If current buffer can't hold the received data, resize the buffer by twicke the current size.
         */
        if( ( *bufferOffset + rxDataLength ) > *bufferLength )
        {
            *pRecvBuffer = _reallocBuffer( *pRecvBuffer,
                                           *bufferLength,
                                           ( 2 * *bufferLength ) );

            if( *pRecvBuffer != NULL )
            {
                *bufferLength = ( *bufferLength * 2 );
            }
            else
            {
                /* Free buffer if there is an error */
                vPortFree( *pRecvBuffer );
                result = false;
            }
        }
    }

    return result;
}


static void _RXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleDataTransferService_t * pService;
    BaseType_t result = true;
    uint8_t * pBufOffset;
    uint8_t ret;
    static uint8_t * recvBuffer = NULL;
    static uint32_t bufferLength = 0;
    static uint32_t bufferOffset = 0;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        pService = _getServiceFromHandle( pWriteParam->attrHandle );
        configASSERT( ( pService != NULL ) );

        resp.pAttrData->handle = pWriteParam->attrHandle;

        resp.pAttrData->uuid = CHAR_UUID( pService->pGattServicePtr, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR );

        if( IotSemaphore_TryWait( &pService->connection.recvLock ) == true )
        {
             if( ( !pWriteParam->isPrep ) && ( pService->connection.receiveCallback != NULL ) )
             {
           
                result = _checkAndSizeReceiveBuffer( &recvBuffer, &bufferLength, &bufferOffset, pWriteParam->length );

                if( result == true )
                {
                    /**
                     * Copy the received data into the buffer.
                     */
                    pBufOffset = ( uint8_t * ) recvBuffer + bufferOffset;
                    memcpy( pBufOffset, pWriteParam->pValue, pWriteParam->length );
                    bufferOffset += pWriteParam->length;

                    if( pWriteParam->length < IOT_BLE_DATA_TRANSFER_LENGTH( bleMTU ) )
                    {
                        pService->connection.recvBufferLen = bufferOffset;
                        pService->connection.pRecvBuffer = recvBuffer;

                        pService->connection.receiveCallback( pService, pService->connection.pContext );

                        vPortFree( recvBuffer );
                        pService->connection.pRecvBuffer = NULL;
                        recvBuffer = NULL;
                        bufferLength = 0;
                        bufferOffset = 0;
                    }

                    resp.eventStatus = eBTStatusSuccess;
                }
            }
            else
            {
                configPRINTF( ( "BLE data transfer receive failed, connection closed.\r\n" ) );
            }

            IotSemaphore_Post( &pService->connection.recvLock );
        }
        else
        {
            configPRINTF( ( "BLE data transfer receive callback failed, cannot acquire receive lock.\r\n" ) );
        }
        

        if( pEventParam->xEventType == eBLEWrite )
        {
            resp.attrDataOffset = 0;
            resp.pAttrData->pData = &ret;
            resp.pAttrData->size = 1;
            IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
        }
    }
}

/*-----------------------------------------------------------*/

static void _RXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleDataTransferService_t * pService;
    uint8_t ret = 0;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        pService = _getServiceFromHandle( pWriteParam->attrHandle );
        configASSERT( ( pService != NULL ) );
        resp.pAttrData->handle = pWriteParam->attrHandle;
        resp.pAttrData->uuid = CHAR_UUID( pService->pGattServicePtr, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR );

        if( IotSemaphore_TryWait( &pService->connection.recvLock ) == true )
        {
             if( ( !pWriteParam->isPrep ) && ( pService->connection.receiveCallback != NULL ) )
            {
                pService->connection.pRecvBuffer = pWriteParam->pValue;
                pService->connection.recvBufferLen = pWriteParam->length;

                pService->connection.receiveCallback( pService, pService->connection.pContext );
                pService->connection.pRecvBuffer = NULL;
                pService->connection.recvBufferLen = 0;

                resp.eventStatus = eBTStatusSuccess;
            }
            else
            {
                 configPRINTF( ( "BLE data transfer receive failed, connection closed.\r\n" ) );
                
            }

            IotSemaphore_Post( &pService->connection.recvLock );
        }
        else
        {
            configPRINTF( ( "BLE data transfer receive callback failed, cannot acquire receive lock.\r\n" ) );
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            resp.attrDataOffset = 0;
            resp.pAttrData->pData = &ret;
            resp.pAttrData->size = 1;
            IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
        }
    }
}

/*-----------------------------------------------------------*/

static void _clientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleDataTransferService_t * pService;
    uint16_t CCFGValue;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        resp.pAttrData->handle = pWriteParam->attrHandle;
        pService = _getServiceFromHandle( pWriteParam->attrHandle );

        if( pWriteParam->length == 2 )
        {
            pService->ccfgValue = ( pWriteParam->pValue[ 1 ] << 8 ) | pWriteParam->pValue[ 0 ];
            resp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            resp.pAttrData->pData = pWriteParam->pValue;
            resp.pAttrData->size = pWriteParam->length;
            resp.attrDataOffset = pWriteParam->offset;
            IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
        }
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pService = _getServiceFromHandle( pEventParam->pParamRead->attrHandle );
        resp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
        resp.pAttrData->pData  = ( uint8_t * ) &( pService->ccfgValue );
        resp.pAttrData->size   = 2;
        resp.attrDataOffset    = 0;
        resp.eventStatus      = eBTStatusSuccess;
        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/
static void _closeConnection( IotBleDataTransferService_t * pService )
{
    pService->connection.receiveCallback = NULL;
}

static void _connectionCallback( BTStatus_t status,
                                 uint16_t connId,
                                 bool connected,
                                 BTBdaddr_t * pRemoteBdAddr )
{
    uint8_t id;
    IotBleDataTransferService_t * pService;

    if( status == eBTStatusSuccess )
    {
        if( connected == true )
        {
            bleConnected = true;
            bleConnectionID = connId;
        }
        else
        {
            bleConnected = false;
            configPRINTF(( "BLE disconnected, closing all active BLE data transfers.\r\n" ));
            for( id = 0; id < IOT_BLE_MAX_DATA_TRANSFER_SERVICES; id++ )
            {
                _closeConnection( &_dataTransferServices[ id ] );

            }
            bleMTU = IOT_BLE_PREFERRED_MTU_SIZE;
        }
    }
}

/*-----------------------------------------------------------*/

static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t Mtu )
{
    /* Change the MTU size for the connection */
    if( bleMTU != Mtu )
    {
        configPRINTF( ( "Changing MTU size for BLE connection from %d to %d\n", bleMTU, Mtu ) );
        bleMTU = Mtu;
    }
}

/*-----------------------------------------------------------*/

bool IotBleDataTransferService_Init( IotBleDataTransferServiceID_t svcID )
{
    uint32_t svc;
    IotBleEventsCallbacks_t callback;
    BTStatus_t status = eBTStatusFail;
    bool ret = true;
    /* Flag to represent one time registeration of callbacks. */
    static bool registerCallbacks = true;


    if( registerCallbacks == true )
    {

        /* Register the connection callback. */
        callback.pConnectionCb = _connectionCallback;
        status = IotBle_RegisterEventCb(eBLEConnection, callback);
        if( status != eBTStatusSuccess )
        {
            configPRINTF(( "Failed to register connection callback, status = %d.r\n", status ));
            ret = false;
        }

        /* Register MTU changed callback. */
        if( ret == true )
        {
            callback.pMtuChangedCb = _MTUChangedCallback;
            status = IotBle_RegisterEventCb( eBLEMtuChanged, callback );
            if( status != eBTStatusSuccess )
            {
                configPRINTF(( "Failed to register MTU changed callback, status = %d.\r\n",status ));
                ret = false;
            }
        }

        registerCallbacks = false;
    }
    

    if( ret == true )
    {
        /* Initialize the GATT service */
            _dataTransferServices[svcID].pGattServicePtr = &_BLEServices[svc];
            _dataTransferServices[svcID].pGattServicePtr->pusHandlesBuffer = _handlesBuffer[svc];
            _dataTransferServices[svcID].pGattServicePtr->ucInstId = 0;
            _dataTransferServices[svcID].pGattServicePtr->xNumberOfAttributes = IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
            _dataTransferServices[svcID].pGattServicePtr->pxBLEAttributes = (BTAttribute_t *) &_pAttributeTable[svc];
            _dataTransferServices[svcID].connection.pRecvBuffer = NULL;
            _dataTransferServices[svcID].connection.sendTimeout = pdMS_TO_TICKS( IOT_BLE_DATA_TRANSFER_SEND_TIMEOUT_DEFAULT_MS );;
            
            ret = IotSemaphore_Create( &_dataTransferServices[ svcID ].connection.sendLock, 0, 1 );
            if( ret == false )
            {
                configPRINTF(( "Failed to create semaphore for data transfer service id: %d.\r\n", svcID ));
            }
            else
            {
                IotSemaphore_Post( &_dataTransferServices[ svcID ].connection.sendLock );
            }

            ret = IotSemaphore_Create( &_dataTransferServices[ svcID ].connection.recvLock, 0, 1 );
            if( ret == false )
            {
                configPRINTF(( "Failed to create semaphore for data transfer service id: %d.\r\n", svcID ));
            }
            else
            {
                IotSemaphore_Post( &_dataTransferServices[ svcID ].connection.recvLock );
            }


            if( ret == true )
            {

                 _dataTransferServices[ svcID ].connection.sendBuffer =
                 xStreamBufferCreate( IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE, IOT_BLE_PREFERRED_MTU_SIZE );

                if( _dataTransferServices[svcID].connection.sendBuffer == NULL )
                {
                     configPRINTF(( "Failed to create stream buffer for data transfer service id: %d.\r\n", svcID ));
                     ret = false;
                }
            }


            if( ret == true )
            {

                 status = IotBle_CreateService(_dataTransferServices[ svcID ].pGattServicePtr, (IotBleAttributeEventCallback_t *)_callbacks);
                 if (status != eBTStatusSuccess)
                 {
                      configPRINTF(("Failed to create data transfer BLE service id: %d\r\n.", svcID ));
                      ret = false;
                 }
                else
                {
                     _dataTransferServices[ svcID ].initialized = true;
                }
            }
    }

    return ret;
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkBle_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void ** pConnection )
{
    IotNetworkError_t ret = IOT_NETWORK_FAILURE;
    IotBleDataTransferService_t * pService;
    IotNetworkBleConnectionInfo_t *pInfo = ( IotNetworkBleConnectionInfo_t *) pConnectionInfo;

    ( void ) pCredentialInfo;

    if( pInfo->service < IOT_BLE_MAX_DATA_TRANSFER_SERVICES )
    {
        pService = &_dataTransferServices[ pInfo->service ];
        if( ( bleConnected == true ) && ( pService->initialized == true ) &&
            ( pService->connection.receiveCallback == NULL ) )
        {
            *pConnection = pService;
            ret = IOT_NETWORK_SUCCESS;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkBle_Close( void * pConnection )
{
    IotBleDataTransferService_t * pService = ( IotBleDataTransferService_t * ) pConnection;

    if( ( pService != NULL ) && ( pService->connection.receiveCallback != NULL ) )
    {
        _closeConnection( pService );
    }

    return IOT_NETWORK_SUCCESS;
}

IotNetworkError_t IotNetworkBle_Destroy( void * pConnection )
{
    IotBleDataTransferService_t * pService = ( IotBleDataTransferService_t * ) pConnection;

    if( ( pService != NULL ) && ( pService->connection.receiveCallback == NULL ) )
    {
        _resetBuffer( pService );
    }

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

size_t IotNetworkBle_Receive( void * pConnection,
                           uint8_t * pBuffer,
                           size_t bytesRequested )
{
    IotBleDataTransferService_t * pService = ( IotBleDataTransferService_t * ) pConnection;
    memcpy( pBuffer, pService->connection.pRecvBuffer, bytesRequested );
    return bytesRequested;
}


IotNetworkError_t IotNetworkBle_SetReceiveCallback( void * pConnection,
                                                 IotNetworkReceiveCallback_t receiveCallback,
                                                 void * pContext )
{
    IotBleDataTransferService_t * pService = ( IotBleDataTransferService_t * ) pConnection;
    pService->connection.receiveCallback = receiveCallback;
    pService->connection.pContext = pContext;
    return IOT_NETWORK_SUCCESS;
}
/*-----------------------------------------------------------*/

size_t IotNetworkBle_Send( void * pConnection,
                        const uint8_t * const pMessage,
                        size_t messageLength )
{
    IotBleDataTransferService_t * pService = ( ( IotBleDataTransferService_t * ) pConnection );
    size_t sendLength, remainingLength = messageLength;
    TickType_t timeRemaining = pdMS_TO_TICKS( pService->connection.sendTimeout );
    TimeOut_t timeout;
    uint8_t * pData;

    vTaskSetTimeOutState( &timeout );

    if( ( pService != NULL ) && ( pService->connection.receiveCallback != NULL ) )
    {
        if( messageLength < ( size_t ) IOT_BLE_DATA_TRANSFER_LENGTH( bleMTU ) )
        {
            if( _sendNotification( pService, IOT_BLE_DATA_TRANSFER_TX_CHAR, ( uint8_t * ) pMessage, messageLength ) == true )
            {
                remainingLength = 0;
            }
            else
            {
                configPRINTF( ( "Failed to send data over BLE, GATT notification failed.\r\n" ) );
            }
        }
        else
        {
            if( IotSemaphore_TimedWait( &pService->connection.sendLock, pService->connection.sendTimeout ) == true )
            {
                sendLength = ( size_t ) IOT_BLE_DATA_TRANSFER_LENGTH( bleMTU );
                pData = ( uint8_t * ) pMessage;

                if( _sendNotification( pService, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR, pData, sendLength ) == true )
                {
                    remainingLength = remainingLength - sendLength;
                    pData += sendLength;

                    while( remainingLength > 0 )
                    {
                        if( ( pService->connection.receiveCallback == NULL ) ||
                            ( xTaskCheckForTimeOut( &timeout, &timeRemaining ) == pdTRUE ) )
                        {
                            configPRINTF( ( "Failed to send data over BLE, connection timed out or closed.\r\n" ) );
                            xStreamBufferReset( pService->connection.sendBuffer );
                            IotSemaphore_Post( &pService->connection.sendLock );
                            break;
                        }

                        if( remainingLength < IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE )
                        {
                            sendLength = remainingLength;
                        }
                        else
                        {
                            sendLength = IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE;
                        }

                        sendLength = xStreamBufferSend( pService->connection.sendBuffer, pData, sendLength, timeRemaining );
                        remainingLength -= sendLength;
                        pData += sendLength;
                    }
                }
                else
                {
                    configPRINTF( ( "Failed to send data over BLE, GATT notification failed.\r\n" ) );
                }
            }
        }
    }
    else
    {
        configPRINTF( ( "Failed to send data over BLE, connection closed.\r\n" ) );
    }

    return( messageLength - remainingLength );
}
