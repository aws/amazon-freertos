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
 * @file aws_iot_data_transfer.c
 * @brief Implementation of BLE data transfer channel APIS using GATT service.
 */

/* Build using a config header, if provided. */
#include "iot_config.h"
#include "iot_ble_config.h"

#include "iot_ble.h"
#include "iot_ble_data_transfer.h"
#include "platform/iot_threads.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_TXRX" )
#include "iot_logging_setup.h"

#define _SERVICE_UUID( SERVICE_ID )         { 0x00, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _CONTROL_CHAR_UUID( SERVICE_ID )    { 0x01, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _TX_CHAR_UUID( SERVICE_ID )         { 0x02, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _RX_CHAR_UUID( SERVICE_ID )         { 0x03, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _TX_LARGE_UUID( SERVICE_ID )        { 0x04, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _RX_LARGE_UUID( SERVICE_ID )        { 0x05, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }
#define _CCFG_UUID    ( 0x2902 )

#define _UUID128( value )        \
    {                            \
        .uu.uu128 = value,       \
        .ucType = eBTuuidType128 \
    }

#define _UUID16( value )        \
    {                           \
        .uu.uu16 = value,       \
        .ucType = eBTuuidType16 \
    }

#define _ATTRIBUTE_TABLE_INITIALIZER( identifier )                                    \
    {                                                                                 \
        {                                                                             \
            .xServiceUUID = _UUID128( _SERVICE_UUID( identifier ) )                   \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbCharacteristic,                                    \
            .xCharacteristic =                                                        \
            {                                                                         \
                .xUuid        = _UUID128( _CONTROL_CHAR_UUID( identifier ) ),         \
                .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ), \
                .xProperties  = ( eBTPropRead | eBTPropWrite )                        \
            }                                                                         \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbCharacteristic,                                    \
            .xCharacteristic =                                                        \
            {                                                                         \
                .xUuid        = _UUID128( _TX_CHAR_UUID( identifier ) ),              \
                .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),                           \
                .xProperties  = ( eBTPropRead | eBTPropNotify )                       \
            }                                                                         \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbDescriptor,                                        \
            .xCharacteristicDescr =                                                   \
            {                                                                         \
                .xUuid        = _UUID16( _CCFG_UUID ),                                \
                .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )  \
            }                                                                         \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbCharacteristic,                                    \
            .xCharacteristic =                                                        \
            {                                                                         \
                .xUuid        = _UUID128( _RX_CHAR_UUID( identifier ) ),              \
                .xPermissions = ( IOT_BLE_CHAR_WRITE_PERM ),                          \
                .xProperties  = ( eBTPropWrite )                                      \
            }                                                                         \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbCharacteristic,                                    \
            .xCharacteristic =                                                        \
            {                                                                         \
                .xUuid        = _UUID128( _TX_LARGE_UUID( identifier ) ),             \
                .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),                           \
                .xProperties  = ( eBTPropRead | eBTPropNotify )                       \
            }                                                                         \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbDescriptor,                                        \
            .xCharacteristicDescr =                                                   \
            {                                                                         \
                .xUuid        = _UUID16( _CCFG_UUID ),                                \
                .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )  \
            }                                                                         \
        },                                                                            \
        {                                                                             \
            .xAttributeType = eBTDbCharacteristic,                                    \
            .xCharacteristic =                                                        \
            {                                                                         \
                .xUuid        = _UUID128( _RX_LARGE_UUID( identifier ) ),             \
                .xPermissions = ( IOT_BLE_CHAR_WRITE_PERM ),                          \
                .xProperties  = ( eBTPropWrite )                                      \
            }                                                                         \
        }                                                                             \
    }

#define _SERVICE_INITIALIZER( id )                   { .identifier = id }

#define CHAR_HANDLE( svc, ch_idx )                   ( ( svc )->pusHandlesBuffer[ ch_idx ] )

#define CHAR_UUID( svc, ch_idx )                     ( ( svc )->pxBLEAttributes[ ch_idx ].xCharacteristic.xUuid )

#define DESCR_HANDLE( svc, descr_idx )               ( ( svc )->pusHandlesBuffer[ descr_idx ] )


#define _CONTAINER( type, pChannel, channelName )    ( ( type * ) ( void * ) ( ( ( uint8_t * ) ( pChannel ) ) - offsetof( type, channelName ) ) )

/**
 * @brief Maximum bytes which can be transferred at a time through the BLE connection.
 */
#define _TRANSMIT_LENGTH( mtu )                      ( ( mtu ) - 3 )

/**
 * @brief Number of data transfer services is determined by number of rows of attribute table.
 */
#define _NUM_DATA_TRANSFER_SERVICES    ( sizeof( _attributeTable ) / sizeof( _attributeTable[ 0 ] ) )

/*-------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Attribute table index for the Data transfer Gatt service attributes.
 */
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
 * @brief Structure used to represent a data channel buffer.
 */
typedef struct IotBleDataChannelBuffer
{
    uint8_t * pBuffer;
    size_t head;
    size_t tail;
    size_t bufferLength;
} IotBleDataChannelBuffer_t;

/**
 * @brief Structure used to represent a data transfer channel.
 */
struct IotBleDataTransferChannel
{
    IotBleDataChannelBuffer_t lotBuffer;          /**< Points to a large object buffer. */
    IotBleDataChannelBuffer_t * pReceiveBuffer;   /**< Points to the buffer where data is received. */

    IotBleDataChannelBuffer_t sendBuffer;         /**< Buffer used to send data. */
    IotSemaphore_t sendComplete;                  /**< Lock to protect access to the send buffer. */

    IotBleDataTransferChannelCallback_t callback; /**< Callback invoked on various events on the channel. */
    void * pContext;                              /**< Callback context. */

    uint32_t timeout;                             /**< Timeout value in milliseconds for the sending/receiving data. */

    bool isUsed;                                  /**< Flag to indicate if the channel is used. */
    bool isOpen;                                  /**< Flag to indicate if the channel is ready to send/receive data. */
};


/**
 * @ingroup ble_datatypes_structs
 * @brief MQTT BLE Service structure.
 */
typedef struct IotBleDataTransferService
{
    uint16_t handles[ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ]; /**< Array to store the attribute handles. */
    uint16_t CCFGValue;                                       /**< Client characteristic configuration values for all the descriptors. */
    uint8_t identifier;                                       /**< Uniquely identifies a data transfer service. */
    BTService_t gattService;                                  /**< Internal gatt Service structure. */
    IotBleDataTransferChannel_t channel;                      /**< Channel used ot send or receive data. */
    bool isReady;
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


static bool _resizeChannelBuffer( IotBleDataChannelBuffer_t * pChannelBuffer,
                                  size_t initialLength,
                                  size_t requiredLength );


static void _deleteChannelBuffer( IotBleDataChannelBuffer_t * pChannelBuffer );


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
 * Copies the individual write packets into a buffer until a packet less than BLE MTU size
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

static const BTAttribute_t _attributeTable[][ IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES ] =
{
    #if ( IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE == 1 )
        #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
            _ATTRIBUTE_TABLE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING ),
        #endif
        #if ( IOT_BLE_ENABLE_MQTT == 1 )
            _ATTRIBUTE_TABLE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT )
        #endif
    #endif
};

static IotBleDataTransferService_t _services[] =
{
    #if ( IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE == 1 )
        #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
            _SERVICE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING ),
        #endif
        #if ( IOT_BLE_ENABLE_MQTT == 1 )
            _SERVICE_INITIALIZER( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT )
        #endif
    #endif
};

static const uint16_t _numDataTransferServices = _NUM_DATA_TRANSFER_SERVICES;


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

    for( id = 0; id < _numDataTransferServices; id++ )
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
    IotBleDataTransferService_t * pService = _CONTAINER( IotBleDataTransferService_t, pChannel, channel );
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
    attrData.uuid = CHAR_UUID( &pService->gattService, attribute );
    attrData.pData = pData;
    attrData.size = len;

    if( IotBle_SendIndication( &response, bleConnectionID, false ) != eBTStatusSuccess )
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
    IotBle_Assert( newBufferSize >= oldBufferSize );

    /* Allocate a larger receive buffer. */
    newBuffer = IotBle_Malloc( newBufferSize );

    /* Copy data into the new buffer. */
    if( newBuffer != NULL )
    {
        ( void ) memcpy( newBuffer,
                         oldBuffer,
                         oldBufferSize );
    }

    /* Free the old buffer. */
    IotBle_Free( oldBuffer );

    return newBuffer;
}

/*-----------------------------------------------------------*/

static bool _resizeChannelBuffer( IotBleDataChannelBuffer_t * pChannelBuffer,
                                  size_t initialLength,
                                  size_t requiredLength )
{
    bool result = true;

    /**
     * Create a new buffer if the buffer is empty.
     */
    if( pChannelBuffer->pBuffer == NULL )
    {
        size_t resultingLength = requiredLength > initialLength ? requiredLength : initialLength;
        pChannelBuffer->pBuffer = IotBle_Malloc( resultingLength );

        if( pChannelBuffer->pBuffer != NULL )
        {
            pChannelBuffer->bufferLength = resultingLength;
            pChannelBuffer->head = pChannelBuffer->tail = 0;
        }
        else
        {
            IotLogError( "Failed to allocate a buffer of size %d", resultingLength );
            result = false;
        }
    }
    else
    {
        /**
         *  If current buffer can't hold the received data, resize the buffer by twice the current size.
         */
        if( ( pChannelBuffer->head + requiredLength ) > pChannelBuffer->bufferLength )
        {
            pChannelBuffer->pBuffer = _reallocBuffer( pChannelBuffer->pBuffer,
                                                      pChannelBuffer->bufferLength,
                                                      ( 2 * pChannelBuffer->bufferLength ) );

            if( pChannelBuffer->pBuffer != NULL )
            {
                pChannelBuffer->bufferLength = ( pChannelBuffer->bufferLength * 2 );
            }
            else
            {
                IotLogError( "Failed to re-allocate a buffer of size %d.\r\n", ( 2 * pChannelBuffer->bufferLength ) );
                result = false;
            }
        }
    }

    return result;
}


static void _deleteChannelBuffer( IotBleDataChannelBuffer_t * pChannelBuffer )
{
    if( pChannelBuffer->pBuffer != NULL )
    {
        IotBle_Free( pChannelBuffer->pBuffer );
        pChannelBuffer->pBuffer = NULL;
        pChannelBuffer->head = pChannelBuffer->tail = 0;
        pChannelBuffer->bufferLength = 0;
    }
}

/*-----------------------------------------------------------*/

static void _ControlCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleDataTransferService_t * pService;
    IotBleDataTransferChannelEvent_t channelEvent;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( pEventParam->xEventType == eBLERead )
    {
        pService = _getServiceFromHandle( pEventParam->pParamRead->attrHandle );

        if( pService != NULL )
        {
            resp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
            resp.pAttrData->pData = ( uint8_t * ) &pService->isReady;
            resp.pAttrData->size = 1;
            resp.attrDataOffset = 0;
            resp.eventStatus = eBTStatusSuccess;
        }

        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
    else if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pService = _getServiceFromHandle( pEventParam->pParamWrite->attrHandle );

        if( pService != NULL )
        {
            pService->isReady = ( *( ( uint8_t * ) pEventParam->pParamWrite->pValue ) == 1 );

            if( pService->channel.callback != NULL )
            {
                pService->channel.isOpen = pService->isReady;
                channelEvent = ( pService->isReady == true ) ? IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED : IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED;
                pService->channel.callback( channelEvent,
                                            &pService->channel,
                                            pService->channel.pContext );
            }

            resp.pAttrData->handle = pEventParam->pParamWrite->attrHandle;
            resp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            IotBle_SendResponse( &resp, pEventParam->pParamWrite->connId, pEventParam->pParamWrite->transId );
        }
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
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .pAttrData      = &attrData,
        .eventStatus    = eBTStatusSuccess,
        .rspErrorStatus = eBTRspErrorNone,
    };

    IotBleDataTransferService_t * pService;
    size_t length;
    BTStatus_t status;

    if( pEventParam->xEventType == eBLERead )
    {
        resp.pAttrData->handle = pEventParam->pParamRead->attrHandle;

        pService = _getServiceFromHandle( pEventParam->pParamRead->attrHandle );

        if( pService && ( pService->channel.isOpen == true ) )
        {
            length = ( pService->channel.sendBuffer.head - pService->channel.sendBuffer.tail );

            if( length > transmitLength )
            {
                length = transmitLength;
            }

            attrData.pData = ( pService->channel.sendBuffer.pBuffer + pService->channel.sendBuffer.tail );
            attrData.size = length;

            status = IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );

            if( status == eBTStatusSuccess )
            {
                pService->channel.sendBuffer.tail += length;

                if( length < transmitLength )
                {
                    pService->channel.sendBuffer.head = pService->channel.sendBuffer.tail = 0;
                    IotSemaphore_Post( &pService->channel.sendComplete );

                    if( pService->channel.callback != NULL )
                    {
                        pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT,
                                                    &pService->channel,
                                                    pService->channel.pContext );
                    }
                }
            }
            else
            {
                IotLogError( "Failed to send large object chunk through ble connection" );
            }
        }
        else
        {
            /** Send an empty response, if the device closed the channel in between.
             *  Its upto the application to handle the partial data buffer sent.
             **/
            ( void ) IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
        }
    }
}

/*-----------------------------------------------------------------------------------------------------------------*/
static void _RXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .pAttrData      = &attrData,
        .rspErrorStatus = eBTRspErrorNone,
        .eventStatus    = eBTStatusFail,
        .attrDataOffset = 0
    };
    IotBleDataTransferService_t * pService;
    bool status = false;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pService = _getServiceFromHandle( pEventParam->pParamWrite->attrHandle );

        if( ( pService != NULL ) &&
            ( pService->channel.isOpen ) )
        {
            status = _resizeChannelBuffer( &pService->channel.lotBuffer, IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE, pEventParam->pParamWrite->length );

            if( status == true )
            {
                /* Copy the received data into the buffer. */
                memcpy( ( pService->channel.lotBuffer.pBuffer + pService->channel.lotBuffer.head ),
                        pEventParam->pParamWrite->pValue,
                        pEventParam->pParamWrite->length );

                pService->channel.lotBuffer.head += pEventParam->pParamWrite->length;

                if( pEventParam->pParamWrite->length < transmitLength )
                {
                    /* All chunks for large object transfer received. */
                    pService->channel.pReceiveBuffer = &pService->channel.lotBuffer;

                    if( pService->channel.callback != NULL )
                    {
                        pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED,
                                                    &pService->channel,
                                                    pService->channel.pContext );
                    }
                }

                resp.eventStatus = eBTStatusSuccess;
            }
            else
            {
                IotLogError( "RX failed, unable to allocate buffer to read data" );
            }
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
        .pAttrData      = &attrData,
        .rspErrorStatus = eBTRspErrorNone,
        .eventStatus    = eBTStatusFail,
        .attrDataOffset = 0
    };
    IotBleDataTransferService_t * pService;
    bool status = false;
    IotBleDataChannelBuffer_t recvBuffer = { 0 };

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pService = _getServiceFromHandle( pEventParam->pParamWrite->attrHandle );

        if( ( pService != NULL ) &&
            ( pService->channel.isOpen == true ) )
        {
            recvBuffer.pBuffer = ( uint8_t * ) pEventParam->pParamWrite->pValue;
            recvBuffer.head = pEventParam->pParamWrite->length;
            recvBuffer.tail = 0;
            pService->channel.pReceiveBuffer = &recvBuffer;

            if( pService->channel.callback != NULL )
            {
                pService->channel.callback( IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED,
                                            &pService->channel,
                                            pService->channel.pContext );
            }

            resp.eventStatus = eBTStatusSuccess;
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
        resp.pAttrData->pData = ( uint8_t * ) &( pService->CCFGValue );
        resp.pAttrData->size = 2;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
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

    if( status == eBTStatusSuccess )
    {
        if( connected == true )
        {
            bleConnectionID = connId;
        }
        else
        {
            for( index = 0; index < _numDataTransferServices; index++ )
            {
                IotBleDataTransfer_Close( &_services[ index ].channel );
                _services[ index ].isReady = false;
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
        IotLogInfo( "MTU changed, changing transmit length for data transfer service from %d to %d\n", transmitLength, length );
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
    status = IotBle_RegisterEventCb( eBLEConnection, callback );

    if( status != eBTStatusSuccess )
    {
        IotLogError( "Failed to register connection callback, status = %d", status );
        ret = false;
    }

    /* Register MTU changed callback. */
    if( ret == true )
    {
        callback.pMtuChangedCb = _MTUChangedCallback;
        status = IotBle_RegisterEventCb( eBLEMtuChanged, callback );

        if( status != eBTStatusSuccess )
        {
            IotLogError( "Failed to register MTU changed callback, status = %d", status );
            ret = false;
        }
    }

    return ret;
}


bool _unregisterCallbacks()
{
    IotBleEventsCallbacks_t callback = { 0 };
    BTStatus_t status;
    bool retVal = true;

    callback.pConnectionCb = _connectionCallback;
    status = IotBle_UnRegisterEventCb( eBLEConnection, callback );

    if( status == eBTStatusSuccess )
    {
        callback.pMtuChangedCb = _MTUChangedCallback;
        status = IotBle_UnRegisterEventCb( eBLEMtuChanged, callback );
    }

    if( status != eBTStatusSuccess )
    {
        retVal = false;
    }

    return retVal;
}

static bool _initializeService( IotBleDataTransferService_t * pService,
                                const BTAttribute_t * pAttributeTable )
{
    BTStatus_t status;
    bool ret = true;

    memset( pService->handles, 0x00, sizeof( pService->handles ) );
    memset( &pService->gattService, 0x00, sizeof( BTService_t ) );

    pService->gattService.pusHandlesBuffer = pService->handles;
    pService->gattService.xNumberOfAttributes = IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
    pService->gattService.pxBLEAttributes = ( BTAttribute_t * ) pAttributeTable;

    status = IotBle_CreateService( &pService->gattService, ( IotBleAttributeEventCallback_t * ) _callbacks );

    if( status != eBTStatusSuccess )
    {
        IotLogError( "Failed to create GATT service for service id %d, error = %d.", pService->identifier, status );
        ret = false;
    }

    return ret;
}

static bool _initializeChannel( IotBleDataTransferChannel_t * pChannel )
{
    bool ret;

    memset( pChannel, 0x00, sizeof( IotBleDataTransferChannel_t ) );

    pChannel->timeout = IOT_BLE_DATA_TRANSFER_TIMEOUT_MS;
    ret = IotSemaphore_Create( &pChannel->sendComplete, 0, 1 );

    if( ret == true )
    {
        IotSemaphore_Post( &pChannel->sendComplete );
    }
    else
    {
        IotLogError( "Failed to create semaphore for send buffer." );
    }

    return ret;
}

static void _cleanupChannel( IotBleDataTransferChannel_t * pChannel )
{
    IotBleDataTransfer_Close( pChannel );
    IotBleDataTransfer_Reset( pChannel );
    IotSemaphore_Destroy( &pChannel->sendComplete );
}

static bool _cleanupService( IotBleDataTransferService_t * pService )
{
    BTStatus_t status;
    bool ret = true;

    pService->isReady = false;
    status = IotBle_DeleteService( &pService->gattService );

    if( status != eBTStatusSuccess )
    {
        IotLogError( "Failed to delete data transfer GATT service, error = %d.", status );
        ret = false;
    }

    return ret;
}


bool IotBleDataTransfer_Init( void )
{
    uint8_t index;
    bool ret;

    ret = _registerCallbacks();

    /* Initialize all data transfer services */
    if( ret == true )
    {
        for( index = 0; index < _numDataTransferServices; index++ )
        {
            ret = _initializeChannel( &_services[ index ].channel );

            if( ret == false )
            {
                IotLogError( "Failed to initialize channel for service id: %d.", _services[ index ].identifier );
                break;
            }
            else
            {
                ret = _initializeService( &_services[ index ], _attributeTable[ index ] );

                if( ret == false )
                {
                    IotLogError( "Failed to initialize channel for service id: %d.", _services[ index ].identifier );
                    break;
                }
            }
        }
    }

    return ret;
}

bool IotBleDataTransfer_Cleanup( void )
{
    bool status = true;
    size_t index;

    /* Unregister callbacks to not receive more events. */
    status = _unregisterCallbacks();

    /* Close any open channel and delete the services. */
    if( status == true )
    {
        for( index = 0; index < _numDataTransferServices; index++ )
        {
            _cleanupChannel( &_services[ index ].channel );
            status = _cleanupService( &_services[ index ] );

            if( status == false )
            {
                break;
            }
        }
    }

    return status;
}

/*-----------------------------------------------------------*/


IotBleDataTransferChannel_t * IotBleDataTransfer_Open( uint8_t serviceIdentifier )
{
    uint8_t index;
    IotBleDataTransferChannel_t * pChannel = NULL;

    for( index = 0; index < _numDataTransferServices; index++ )
    {
        if( _services[ index ].identifier == serviceIdentifier )
        {
            if( _services[ index ].channel.isUsed == false )
            {
                pChannel = &_services[ index ].channel;
                pChannel->isUsed = true;

                if( _services[ index ].isReady == true )
                {
                    pChannel->isOpen = true;
                }
            }
        }
    }

    return pChannel;
}


bool IotBleDataTransfer_SetCallback( IotBleDataTransferChannel_t * pChannel,
                                     const IotBleDataTransferChannelCallback_t callback,
                                     void * pContext )
{
    bool ret = false;

    if( ( pChannel != NULL ) &&
        ( pChannel->isUsed == true ) &&
        ( callback != NULL ) &&
        ( pChannel->callback == NULL ) )
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
    if( pChannel->isOpen == true )
    {
        pChannel->isOpen = false;

        /* Nobody writes/reads from send buffer after timeout value. */
        ( void ) IotSemaphore_TimedWait( &pChannel->sendComplete, pChannel->timeout );
        _deleteChannelBuffer( &pChannel->sendBuffer );
        IotSemaphore_Post( &pChannel->sendComplete );
        _deleteChannelBuffer( &pChannel->lotBuffer );
        pChannel->pReceiveBuffer = NULL;

        if( pChannel->callback != NULL )
        {
            pChannel->callback( IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED, pChannel, pChannel->pContext );
        }
    }
}

void IotBleDataTransfer_Reset( IotBleDataTransferChannel_t * pChannel )
{
    pChannel->isUsed = false;
    pChannel->callback = NULL;
    pChannel->pContext = NULL;
}

/*-----------------------------------------------------------*/
size_t IotBleDataTransfer_Receive( IotBleDataTransferChannel_t * pChannel,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested )
{
    size_t bytesReturned = pChannel->pReceiveBuffer->head - pChannel->pReceiveBuffer->tail;

    if( bytesReturned > bytesRequested )
    {
        bytesReturned = bytesRequested;
    }

    if( pBuffer != NULL )
    {
        memcpy( pBuffer, ( pChannel->pReceiveBuffer->pBuffer + pChannel->pReceiveBuffer->tail ), bytesReturned );
    }

    pChannel->pReceiveBuffer->tail += bytesReturned;

    if( pChannel->pReceiveBuffer->tail == pChannel->pReceiveBuffer->head )
    {
        pChannel->pReceiveBuffer->head = pChannel->pReceiveBuffer->tail = 0;
    }

    return bytesReturned;
}

/*----------------------------------------------------------------------------------------------------------------------------*/

void IotBleDataTransfer_PeekReceiveBuffer( IotBleDataTransferChannel_t * pChannel,
                                           const uint8_t ** pBuffer,
                                           size_t * pBufferLength )
{
    if( pChannel->pReceiveBuffer != NULL )
    {
        *pBuffer = ( pChannel->pReceiveBuffer->pBuffer + pChannel->pReceiveBuffer->tail );
        *pBufferLength = ( pChannel->pReceiveBuffer->head - pChannel->pReceiveBuffer->tail );
    }
    else
    {
        *pBuffer = NULL;
        *pBufferLength = 0;
    }
}

/*----------------------------------------------------------------------------------------------------------------------------*/

size_t IotBleDataTransfer_Send( IotBleDataTransferChannel_t * pChannel,
                                const uint8_t * const pMessage,
                                size_t messageLength )
{
    size_t remainingLength = messageLength;

    if( pChannel && pChannel->isOpen )
    {
        if( messageLength < transmitLength )
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
                IotLogError( "TX failed, GATT notification failed." );
            }
        }
        else
        {
            /*
             * Data transfer service does not support concurrent large object transfer so it waits for previous
             * transfer to be complete or the configured timeout is reached.
             */
            if( IotSemaphore_TimedWait( &pChannel->sendComplete, pChannel->timeout ) == true )
            {
                if( _send( pChannel, true, ( uint8_t * ) pMessage, transmitLength ) == true )
                {
                    remainingLength -= transmitLength;

                    if( remainingLength > 0 )
                    {
                        if( _resizeChannelBuffer( &pChannel->sendBuffer, IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE, remainingLength ) == true )
                        {
                            memcpy( pChannel->sendBuffer.pBuffer, ( pMessage + transmitLength ), remainingLength );
                            pChannel->sendBuffer.head += remainingLength;
                            remainingLength = 0;
                        }
                        else
                        {
                            IotLogError( "TX Failed, Failed to allocate send buffer." );
                            IotSemaphore_Post( &pChannel->sendComplete );
                        }
                    }
                }
                else
                {
                    IotLogError( "TX Failed, GATT notification failed." );
                    IotSemaphore_Post( &pChannel->sendComplete );
                }
            }
            else
            {
                IotLogError( "TX Failed, channel timed out." );
            }
        }
    }
    else
    {
        IotLogError( "TX Failed, channel closed." );
    }

    return( messageLength - remainingLength );
}
