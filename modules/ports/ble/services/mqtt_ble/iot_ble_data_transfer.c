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

#define _SERVICE_UUID( SERVICE_ID )         { 0x00, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _CONTROL_CHAR_UUID( SERVICE_ID )    { 0x01, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _TX_CHAR_UUID( SERVICE_ID )         { 0x02, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _RX_CHAR_UUID( SERVICE_ID )         { 0x03, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _TX_LARGE_UUID( SERVICE_ID)         { 0x04, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _RX_LARGE_UUID( SERVICE_ID )        { 0x05, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _CCFG_UUID                          ( 0x2902 )

#define _UUID128( value )                \
{                                        \
    .uu.uu128 = value,                   \
    .ucType = eBTuuidType128             \
}

#define _UUID16( value )                 \
{                                        \
    .uu.uu16 = value,                    \
    .ucType = eBTuuidType16              \
}

#define _ATTRIBUTE_TABLE_INITIALIZER( identifier )                                        \
{                                                                                         \
    {                                                                                     \
        .xServiceUUID = _UUID128( _SERVICE_UUID( identifier ) )                           \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = _UUID128( _CONTROL_CHAR_UUID( identifier ) ),                 \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),         \
            .xProperties  = ( eBTPropRead | eBTPropWrite )                                \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = _UUID128( _TX_CHAR_UUID( identifier ) ),                      \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),                                   \
            .xProperties  = ( eBTPropRead | eBTPropNotify )                               \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbDescriptor,                                                \
        .xCharacteristicDescr =                                                           \
        {                                                                                 \
            .xUuid        = _UUID16( _CCFG_UUID ),                                        \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )          \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = _UUID128( _RX_CHAR_UUID( identifier ) ),                      \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),         \
            .xProperties  = ( eBTPropRead | eBTPropWrite )                                \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = _UUID128( _TX_LARGE_UUID( identifier ) ),                     \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),                                   \
            .xProperties  = ( eBTPropRead | eBTPropNotify )                               \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbDescriptor,                                                \
        .xCharacteristicDescr =                                                           \
        {                                                                                 \
            .xUuid        = _UUID16( _CCFG_UUID ),                                        \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )          \
        }                                                                                 \
    },                                                                                    \
    {                                                                                     \
        .xAttributeType = eBTDbCharacteristic,                                            \
        .xCharacteristic =                                                                \
        {                                                                                 \
            .xUuid        = _UUID128(_RX_LARGE_UUID( identifier )),                       \
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),         \
            .xProperties  = ( eBTPropRead | eBTPropWrite )                                \
        }                                                                                 \
    }                                                                                     \
}

#define _SERVICE_INITIALIZER( id )         { .identifier = id }

#define CHAR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ ch_idx ] )

#define CHAR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ ch_idx ].xCharacteristic.xUuid )

#define DESCR_HANDLE( svc, descr_idx )    ( ( svc )->pusHandlesBuffer[ descr_idx ] )


#define _CONTAINER( type, pChannel, channelName )  ( ( type * ) ( void * ) ( ( ( uint8_t * ) ( pChannel ) ) - offsetof( type, channelName ) ) )

/**
 * @brief Maximum bytes which can be transferred at a time through the BLE connection.
 */
#define _TRANSMIT_LENGTH( mtu )    ( ( mtu ) - 3 )

/*-------------------------------------------------------------------------------------------------------------------------*/

typedef enum IotBleDataTransferAttributes
{
    IOT_BLE_DATA_TRANSFER_SERVICE = 0,
    IOT_BLE_DATA_TRANSFER_CONTROL_CHAR,
    IOT_BLE_DATA_TRANSFER_TX_CHAR,             /*!< IOT_BLE_DATA_TRANSFER_TX_CHAR Characteristic to send message. */
    IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR,       /*!< IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR Characteristic descriptor. */
    IOT_BLE_DATA_TRANSFER_RX_CHAR,             /*!< IOT_BLE_DATA_TRANSFER_RX_CHAR Characteristic to receive a message.*/
    IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR,       /*!< IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR Characteristic to send a large message (> BLE MTU Size). */
    IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_DESCR, /*!< IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_DESCR Characteristic descriptor. */
    IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR,       /*!< IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR Characteristic to receive large message (> BLE MTU Size). */
    IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES       /*!< IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES Max number of attributes for ble data transfer service. */

} IotBleDataTransferAttributes_t;



/**
 * @ingroup ble_datatypes_structs
 * @brief MQTT BLE Service structure.
 */
typedef struct IotBleDataTransferService
{
    uint16_t  handles[ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ]; /**< Array to store the attribute handles. */
    uint16_t  CCFGValue;                           /**< Client characteristic configuration values for all the descriptors. */
    uint8_t   identifier;                         /**< Uniquely identifies a data transfer service. */
    BTService_t                 gattService;      /**< Internal gatt Service structure. */
    IotBleDataTransferChannel_t channel;          /**< Channel used ot send or receive data. */    

} IotBleDataTransferService_t;


/*-------------------------------------------------------------------------------------------------------------------------*/

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



static bool _send( IotBleDataTransferChannel_t * pChannel,
                   bool isLOT,
                   uint8_t * pData,
                   size_t len );


/*
 * @brief Callback to register for events (read) on TX message characteristic.
 */
static void _ControlCharCallback( IotBleAttributeEvent_t * pEventParam );

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

/*----------------------------------------------------------------------------------------------------------*/


static const BTAttribute_t _attributeTable[ IOT_BLE_NUM_DATA_TRANSFER_SERVICES ][ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ] =
{
#if( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
    _ATTRIBUTE_TABLE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING ),
#endif
    _ATTRIBUTE_TABLE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT )
};

static IotBleDataTransferService_t _services[ IOT_BLE_NUM_DATA_TRANSFER_SERVICES ] =
{
#if( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
    _SERVICE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING ),
#endif
    _SERVICE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT )
};

static const IotBleAttributeEventCallback_t _callbacks[ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ] =
{
    NULL,
    _ControlCharCallback,
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
static size_t transmitLength = _TRANSMIT_LENGTH( IOT_BLE_PREFERRED_MTU_SIZE );

/**
 * @brief Holds the connection id for the BLE connection
 */
static uint16_t bleConnectionID;


/*-----------------------------------------------------------*/

static IotBleDataTransferService_t * _getServiceFromHandle( uint16_t handle )
{
    uint8_t id;
    IotBleDataTransferService_t * pService = NULL;

    for( id = 0; id < IOT_BLE_NUM_DATA_TRANSFER_SERVICES; id++ )
    {
        /* Check that the handle is included in the service. */
        if( ( handle > _services[ id ].handles[ 0 ] ) &&
            ( handle <= _services[ id ].handles[ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES - 1 ] ) )
        {
            pService = &_services[ id ];
            break;
        }
    }

    return pService;
}

/*-----------------------------------------------------------*/

static bool _send( IotBleDataTransferChannel_t * pChannel,
                   bool isLOT,
                   uint8_t * pData,
                   size_t len )
{
    IotBleDataTransferService_t *pService = _CONTAINER( IotBleDataTransferService_t, pChannel, channel );
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t response =
    {
        .pAttrData      = &attrData,
        .attrDataOffset = 0,
        .eventStatus    = eBTStatusSuccess,
        .rspErrorStatus = eBTRspErrorNone,
    };
    IotBleDataTransferAttributes_t attribute; 
    bool status = true;

    attribute = ( isLOT == true ) ? IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR : IOT_BLE_DATA_TRANSFER_TX_CHAR;
    attrData.handle = CHAR_HANDLE( &pService->gattService, attribute );
    attrData.uuid   = CHAR_UUID( &pService->gattService, attribute );
    attrData.pData  = pData;
    attrData.size   = len;
    

    if(  IotBle_SendIndication( &response, bleConnectionID, false ) != eBTStatusSuccess )
    {
        status = false;
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

static void _ControlCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleDataTransferService_t* pService;
    uint8_t value;
    IotBleDataTransferChannelEvent_t channelEvent;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if (pEventParam->xEventType == eBLERead)
    {
        pService = _getServiceFromHandle(pEventParam->pParamRead->attrHandle);
        if (pService != NULL)
        {
            resp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
            resp.pAttrData->pData = ( uint8_t * ) pService->channel.isOpen;
            resp.pAttrData->size = 1;
            resp.attrDataOffset = 0;
            resp.eventStatus = eBTStatusSuccess;
        }

        IotBle_SendResponse(&resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId);
    }
    else if (pEventParam->xEventType == eBLEWrite || pEventParam->xEventType == eBLEWriteNoResponse)
    {
        pService = _getServiceFromHandle(pEventParam->pParamWrite->attrHandle);
        if (pService != NULL)
        {     
            pService->channel.isOpen = ( *( ( uint8_t * ) pEventParam->pParamWrite->pValue ) == 1 );   
            if( pService->channel.callback != NULL )
            {
                channelEvent =  ( pService->channel.isOpen == true ) ? IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED : IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED;
                pService->channel.callback( channelEvent,
                                            &pService->channel,
                                            pService->channel.pContext );
            }
            resp.pAttrData->handle = pEventParam->pParamWrite->attrHandle;
            resp.eventStatus = eBTStatusSuccess;
        }

        IotBle_SendResponse(&resp, pEventParam->pParamWrite->connId, pEventParam->pParamWrite->transId);
    }
}

/*--------------------------------------------------------------------------------*/

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
    size_t length;
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

        pData = pvPortMalloc( transmitLength );

        if( pData != NULL )
        {
            length = xStreamBufferReceive( pService->channel.sendBuffer, pData, transmitLength, ( TickType_t ) 0ULL );
            resp.pAttrData->pData = pData;
            resp.pAttrData->size = length;
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
            ( length < transmitLength ) )
        {
            IotSemaphore_Post( &pService->channel.sendLock );
            if( pService->channel.callback  != NULL )
            {
                pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT, &pService->channel, pService->channel.pContext );
            }
        }
    }
}

/*-----------------------------------------------------------*/
static bool _checkAndSizeReceiveBuffer( uint8_t ** pReceiveBuffer,
                                        size_t * pBufferLength,
                                        size_t   bufferOffset,
                                        size_t   rxDataLength )
{
    bool result = true;

    /**
     * Create a new buffer if the buffer is empty.
     */
    if( *pReceiveBuffer == NULL )
    {
        *pReceiveBuffer = pvPortMalloc( IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE );
        if( *pReceiveBuffer != NULL )
        {
            *pBufferLength = IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE;
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
        if( ( bufferOffset + rxDataLength ) > *pBufferLength )
        {
            *pReceiveBuffer = _reallocBuffer( *pReceiveBuffer,
                                              *pBufferLength,
                                              ( 2 * *pBufferLength ) );

            if( *pReceiveBuffer != NULL )
            {
                *pBufferLength = ( *pBufferLength * 2 );
            }
            else
            {
                result = false;
            }
        }
    }

    return result;
}


static void _RXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .pAttrData        = &attrData,
        .rspErrorStatus   = eBTRspErrorNone,
        .eventStatus      = eBTStatusFail,
        .attrDataOffset   = 0
    };
    IotBleDataTransferService_t * pService;
    bool status  = false;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pService = _getServiceFromHandle( pEventParam->pParamWrite->attrHandle );
        if( ( pService != NULL ) && ( pService->channel.isOpen ) )
        {
            IotMutex_Lock( &pService->channel.receiveLock );
            if( pService->channel.pendingRead  == false )
            {
                if( ( pService->channel.head == 0 ) &&  ( pService->channel.tail == 0 ) )
                {
                    if (pService->channel.callback != NULL)
                    {
                        pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_START,
                                                    &pService->channel,
                                                    pService->channel.pContext );
                    }
                }

                status = _checkAndSizeReceiveBuffer( &pService->channel.pReceiveBuffer,
                                                     &pService->channel.length,
                                                     pService->channel.head,
                                                     pEventParam->pParamWrite->length );
                if( status == true )
                {
                    /* Copy the received data into the buffer. */
                    memcpy( ( pService->channel.pReceiveBuffer + pService->channel.head ), 
                            pEventParam->pParamWrite->pValue,
                            pEventParam->pParamWrite->length );
                
                    pService->channel.head += pEventParam->pParamWrite->length;

                    if( pEventParam->pParamWrite->length < transmitLength )
                    {
                        /* All chunks for large object transfer received. set channel to pending read */
                        pService->channel.pendingRead = true;
                        if (pService->channel.callback != NULL)
                        {
                            pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_COMPLETE,
                                                        &pService->channel,
                                                        pService->channel.pContext );
                        }
                    }                    
                    resp.eventStatus = eBTStatusSuccess;
                }
                else
                {
                    if( pService->channel.pReceiveBuffer != NULL )
                    {
                        vPortFree( pService->channel.pReceiveBuffer );
                        pService->channel.pReceiveBuffer = NULL;
                        pService->channel.head = pService->channel.tail = 0;
                        pService->channel.length = 0;
                    }

                    configPRINTF(( "RX failed, unable to allocate buffer to read data.\r\n" ));
                }
            }
            else
            {
                configPRINTF(( "RX failed, pending data needs to be read.\r\n" ));
            }

            IotMutex_Unlock( &pService->channel.receiveLock );
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            resp.pAttrData->handle = pEventParam->pParamWrite->attrHandle;
            resp.pAttrData->pData = ( uint8_t * ) &status;
            resp.pAttrData->size = 1;
            IotBle_SendResponse( &resp, pEventParam->pParamWrite->connId, pEventParam->pParamWrite->transId );
        }
    }
}

/*-----------------------------------------------------------*/

static void _RXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .pAttrData = &attrData,
        .rspErrorStatus = eBTRspErrorNone,
        .eventStatus = eBTStatusFail,
        .attrDataOffset = 0
    };
    IotBleDataTransferService_t * pService;
    bool status = false;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pService = _getServiceFromHandle( pEventParam->pParamWrite->attrHandle );

        if( ( pService != NULL ) && ( pService->channel.isOpen == true ) )
        {
            IotMutex_Lock( &pService->channel.receiveLock );

            if( pService->channel.pendingRead == false )
            {
                status = _checkAndSizeReceiveBuffer( &pService->channel.pReceiveBuffer,
                                                     &pService->channel.length,
                                                     pService->channel.head,
                                                     pEventParam->pParamWrite->length );

                if( status == true )
                {           
                    /* Copy the received data into the buffer. */
                    memcpy( ( pService->channel.pReceiveBuffer + pService->channel.head ), 
                            pEventParam->pParamWrite->pValue,
                            pEventParam->pParamWrite->length );
                
                    pService->channel.head += pEventParam->pParamWrite->length;
                    pService->channel.pendingRead = true;
                    if (pService->channel.callback != NULL)
                    {
                        pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_COMPLETE,
                                                    &pService->channel,
                                                    pService->channel.pContext );
                    }
                    resp.eventStatus = eBTStatusSuccess;
                }
                else
                {
                    if( pService->channel.pReceiveBuffer != NULL )
                    {
                        vPortFree( pService->channel.pReceiveBuffer );
                        pService->channel.pReceiveBuffer = NULL;
                        pService->channel.head = pService->channel.tail = 0;
                        pService->channel.length = 0;
                    }

                    configPRINTF(( "RX failed, unable to allocate buffer to read data.\r\n" ));
                }
            }
            
            IotMutex_Unlock( &pService->channel.receiveLock );
        }
        
        if( pEventParam->xEventType == eBLEWrite )
        {
            resp.pAttrData->handle = pEventParam->pParamWrite->attrHandle;
            resp.pAttrData->pData = ( uint8_t * ) &status;
            resp.pAttrData->size = 1;
            IotBle_SendResponse( &resp, pEventParam->pParamWrite->connId, pEventParam->pParamWrite->transId );
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
            pService->CCFGValue = ( pWriteParam->pValue[ 1 ] << 8 ) | pWriteParam->pValue[ 0 ];
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
        resp.pAttrData->pData  = ( uint8_t * ) &( pService->CCFGValue );
        resp.pAttrData->size   = 2;
        resp.attrDataOffset    = 0;
        resp.eventStatus      = eBTStatusSuccess;
        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

static void _connectionCallback( BTStatus_t status,
                                 uint16_t connId,
                                 bool connected,
                                 BTBdaddr_t * pRemoteBdAddr )
{
    uint8_t index;
    IotBleDataTransferService_t * pService;

    if( status == eBTStatusSuccess )
    {
        if( connected == true )
        {
            bleConnectionID = connId;
        }
        else
        {
            configPRINTF(( "BLE disconnected, closing all active BLE data transfer sessions.\r\n" ));
            for( index = 0; index < IOT_BLE_NUM_DATA_TRANSFER_SERVICES; index++ )
            {
                IotBleDataTransfer_Close( &_services[index].channel );
            }
            transmitLength = _TRANSMIT_LENGTH( IOT_BLE_PREFERRED_MTU_SIZE );
        }
    }
}

/*-----------------------------------------------------------*/

static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t mtu )
{
    size_t length = _TRANSMIT_LENGTH( mtu );
    /* Change the MTU size for the connection */
    if( transmitLength != length )
    {
        configPRINTF( ( "MTU changed, changing transmit length for data transfer service from %d to %d\n", transmitLength, length ) );
        transmitLength = length;
    }
}

/*-----------------------------------------------------------*/


bool _registerCallbacks()
{
    IotBleEventsCallbacks_t callback = { 0 };
    bool ret = true;
    BTStatus_t status;

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

    return ret;
}

static bool _initializeService( IotBleDataTransferService_t* pService, const BTAttribute_t* pAttributeTable )
{
    BTStatus_t status;
    bool ret = true;

    memset( pService->handles, 0x00,  sizeof( pService->handles ) );
    memset( &pService->gattService, 0x00, sizeof( BTService_t ) );

    pService->gattService.pusHandlesBuffer     = pService->handles;
    pService->gattService.xNumberOfAttributes  = IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
    pService->gattService.pxBLEAttributes      = ( BTAttribute_t * ) pAttributeTable;

    status = IotBle_CreateService(&pService->gattService, (IotBleAttributeEventCallback_t *)_callbacks);
    if (status != eBTStatusSuccess)
    {
        configPRINTF(("Failed to create GATT service for service id %d, error = %d\r\n.", pService->identifier, status));
        ret = false;
    }

    return ret;

}

static bool _initializeChannel( IotBleDataTransferChannel_t* pChannel )
{
    bool ret;

    memset( pChannel, 0x00, sizeof( IotBleDataTransferChannel_t ) );

    pChannel->timeout = IOT_BLE_DATA_TRANSFER_TIMEOUT_MS;
    ret = IotSemaphore_Create( &pChannel->sendLock, 0, 1 );
    if( ret == true )
    {
        IotSemaphore_Post( &pChannel->sendLock);
    }
    else
    {
        configPRINTF(( "Failed to create semaphore for send buffer.\r\n" ));
    }

    if (ret == true)
    {
        /* Create a recursive lock for receive. */
        ret = IotMutex_Create( &pChannel->receiveLock, true );
        if (ret == false)
        {
            configPRINTF(( "Failed to create mutex for receive buffer.\r\n" ));
        }
    }

    if (ret == true)
    {

        pChannel->sendBuffer = xStreamBufferCreate( IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE, IOT_BLE_PREFERRED_MTU_SIZE );
        if (pChannel->sendBuffer == NULL)
        {
            configPRINTF(( "Failed to create send buffer. \r\n" ));
            ret = false;
        }
    }

    return ret;
}


bool IotBleDataTransfer_Init( void )
{
    
    static bool callbacksRegistered = false;
    IotBleDataTransferService_t *pService = NULL;
    uint8_t index;
    bool ret;

    ret = _registerCallbacks();

    /* Initialize all data transfer services */
    if( ret == true )
    {
        for (index = 0; index < IOT_BLE_NUM_DATA_TRANSFER_SERVICES; index++)
        {
            ret = _initializeChannel( &_services[index].channel );
            if (ret == false)
            {
                configPRINTF(("Failed to initialize channel for service id: %d\r\n.", _services[index].identifier));
                break;
            }
            else
            {
                ret = _initializeService( &_services[index], _attributeTable[ index ] );
                if (ret == false)
                {
                    configPRINTF(( "Failed to initialize channel for service id: %d\r\n.", _services[index].identifier ));
                    break;
                }
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/


IotBleDataTransferChannel_t * IotBleDataTransfer_Open( uint8_t serviceIdentifier )
{
    uint8_t index;
    IotBleDataTransferChannel_t * pChannel = NULL;

    for( index = 0; index < IOT_BLE_NUM_DATA_TRANSFER_SERVICES; index++ )
    {
        if( _services[index].identifier == serviceIdentifier )
        {
            if( _services[index].channel.isUsed == false )
            {  
                pChannel = &_services[ index ].channel;
                pChannel->isUsed = true;
            }
        }
    }

    return pChannel;
}


bool IotBleDataTransfer_SetCallback( IotBleDataTransferChannel_t* pChannel, const IotBleDataTransferChannelCallback_t callback, void *pContext )
{
    bool ret = false;
    if( pChannel != NULL &&
        pChannel->isUsed == true &&
        callback != NULL &&
        pChannel->callback == NULL  )
    {
        pChannel->callback = callback;
        pChannel->pContext = pContext;

        if( pChannel->isOpen == true )
        {
            pChannel->callback( IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED, pChannel, pContext );
        }

        ret = true;
    }

    return ret;
}


/*-----------------------------------------------------------*/

void IotBleDataTransfer_Close( IotBleDataTransferChannel_t * pChannel )
{
    pChannel->isOpen = false;
    /* Assume nobody writes/reads from buffer after timeout. */
    ( void ) IotSemaphore_TimedWait( &pChannel->sendLock, pChannel->timeout );
    xStreamBufferReset( pChannel->sendBuffer );
    IotSemaphore_Post( &pChannel->sendLock );

    IotMutex_Lock( &pChannel->receiveLock );
    if( pChannel->pReceiveBuffer != NULL )
    {
        vPortFree( pChannel->pReceiveBuffer );
        pChannel->head = pChannel->tail = 0;
        pChannel->length = 0;
        pChannel->pReceiveBuffer = NULL;
        pChannel->pendingRead = false;
    }
    IotMutex_Unlock( &pChannel->receiveLock );

    if( pChannel->callback != NULL )
    {
        pChannel->callback( IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED, pChannel, pChannel->pContext );
    }
}

void IotBleDataTransfer_Reset( IotBleDataTransferChannel_t * pChannel )
{
    pChannel->isUsed = false;
    pChannel->callback = NULL;
    pChannel->pContext = NULL;
}

/*-----------------------------------------------------------*/
size_t IotBleDataTransfer_Receive( IotBleDataTransferChannel_t * pChannel, uint8_t * pBuffer, size_t bytesRequested )
{
    size_t bytesReturned = 0;
    
    IotMutex_Lock( &pChannel->receiveLock );
    
    bytesReturned = ( pChannel->head - pChannel->tail );
    if( bytesReturned > bytesRequested )
    {
        bytesReturned = bytesRequested;
    }

    if( pBuffer != NULL )
    {
        memcpy( pBuffer, pChannel->pReceiveBuffer, bytesReturned );
    }

    pChannel->tail += bytesReturned;
    if( ( pChannel->tail == pChannel->head ) && pChannel->pendingRead == true )
    {
        pChannel->head = pChannel->tail = 0;
        pChannel->pendingRead = false;
    }
    
    IotMutex_Unlock( &pChannel->receiveLock );

    
    return bytesReturned;
}

/*-----------------------------------------------------------*/

size_t IotBleDataTransfer_Send( IotBleDataTransferChannel_t * pChannel, const uint8_t * const pMessage, size_t messageLength )
{
    size_t sendLength, remainingLength = messageLength;
    TickType_t timeRemaining = pdMS_TO_TICKS( pChannel->timeout );
    TimeOut_t timeout;
    uint8_t * pData;

    vTaskSetTimeOutState( &timeout );

    if( pChannel->isOpen )
    {
        if( messageLength < transmitLength  )
        {
            if( _send( pChannel, false, ( uint8_t * ) pMessage, messageLength ) == true )
            {
                if( pChannel->callback != NULL )
                {
                    pChannel->callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT, pChannel, pChannel->pContext );
                }
                remainingLength = 0;
            }
            else
            {
                configPRINTF( ( "TX failed, GATT notification failed.\r\n" ) );
            }
        }
        else
        {
            if( IotSemaphore_TimedWait( &pChannel->sendLock, pChannel->timeout ) == true )
            {
                sendLength =  transmitLength;
                pData = ( uint8_t * ) pMessage;

                if( _send( pChannel, true, pData, sendLength ) == true )
                {
                    remainingLength = remainingLength - sendLength;
                    pData += sendLength;

                    while( remainingLength > 0 )
                    {
                        if( xTaskCheckForTimeOut( &timeout, &timeRemaining ) == pdTRUE )
                        {
                            configPRINTF( ( "TX Failed, channel timed out.\r\n" ) );
                            xStreamBufferReset( pChannel->sendBuffer );
                            IotSemaphore_Post( &pChannel->sendLock );
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

                        sendLength = xStreamBufferSend( pChannel->sendBuffer, pData, sendLength, timeRemaining );
                        remainingLength -= sendLength;
                        pData += sendLength;
                    }
                }
                else
                {
                    configPRINTF( ( "TX Failed, GATT notification failed.\r\n" ) );
                }
            }
        }
    }
    else
    {
        configPRINTF( ( "TX Failed, channel closed.\r\n" ) );
    }

    return( messageLength - remainingLength );
}
