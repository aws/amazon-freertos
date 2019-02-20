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
 * @brief GATT service for transferring MQTT packets over BLE
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include "iot_ble_config.h"
#include "iot_ble_mqtt.h"
#include "task.h"

#include "aws_iot_serializer.h"


#define IOT_BLE_MQTT_CHAR_CONTROL_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_MQTT_CHAR_CONTROL_UUID,\
    .ucType   = eBTuuidType128 \
}
#define IOT_BLE_MQTT_CHAR_TX_MESG_UUID_TYPE \
{  \
    .uu.uu128 = IOT_BLE_MQTT_CHAR_TX_MESG_UUID,\
    .ucType  = eBTuuidType128\
}

#define IOT_BLE_MQTT_CHAR_RX_MESG_UUID_TYPE \
{\
    .uu.uu128 = IOT_BLE_MQTT_CHAR_RX_MESG_UUID,\
    .ucType  = eBTuuidType128\
}

#define IOT_BLE_MQTT_CHAR_TX_LARGE_MESG_UUID_TYPE \
{\
    .uu.uu128 = IOT_BLE_MQTT_CHAR_TX_LARGE_MESG_UUID,\
    .ucType  = eBTuuidType128\
}

#define IOT_BLE_MQTT_CHAR_RX_LARGE_MESG_UUID_TYPE \
{\
    .uu.uu128 = IOT_BLE_MQTT_CHAR_RX_LARGE_MESG_UUID,\
    .ucType  = eBTuuidType128\
}
#define IOT_BLE_MQTT_CCFG_UUID_TYPE \
{\
    .uu.uu16 = IOT_BLE_MQTT_CCFG_UUID,\
    .ucType  = eBTuuidType16\
}
/**
 * @brief UUID for Device Information Service.
 *
 * This UUID is used in advertisement for the companion apps to discover and connect to the device.
 */
#define IOT_BLE_MQTT_SERVICE_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_MQTT_SERVICE_UUID,\
    .ucType   = eBTuuidType128\
}

#define IS_VALID_SERIALIZER_RET( ret, pxSerializerBuf )                                \
    (  ( ret == AWS_IOT_SERIALIZER_SUCCESS ) ||                                        \
          (  ( !pxSerializerBuf ) && ( ret == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )

static uint16_t _handlesBuffer[IOT_BLE_MQTT_MAX_SVC_INSTANCES][IOT_BLE_MQTT_NUMBER_ATTRIBUTES];

#define CHAR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ch_idx] )
#define CHAR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ch_idx].xCharacteristic.xUuid )
#define DESCR_HANDLE( svc, descr_idx )    ( ( svc )->pusHandlesBuffer[descr_idx] )


/*-----------------------------------------------------------------------------------------------------*/
static IotBleMqttService_t _mqttBLEServices[ IOT_BLE_MQTT_MAX_SVC_INSTANCES ] = { 0 };
static BTService_t _BLEServices[ IOT_BLE_MQTT_MAX_SVC_INSTANCES ] = { 0 };

static const BTAttribute_t _pAttributeTable[] = {
     {    
          .xServiceUUID = IOT_BLE_MQTT_SERVICE_UUID_TYPE
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = IOT_BLE_MQTT_CHAR_CONTROL_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = IOT_BLE_MQTT_CHAR_TX_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = (  eBTPropRead | eBTPropNotify  )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = IOT_BLE_MQTT_CCFG_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = IOT_BLE_MQTT_CHAR_RX_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = IOT_BLE_MQTT_CHAR_TX_LARGE_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = ( eBTPropRead | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = IOT_BLE_MQTT_CCFG_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = IOT_BLE_MQTT_CHAR_RX_LARGE_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     }
};


/**
 * Variable stores the MTU Size for the BLE connection.
 */
static uint16_t BLEConnMTU;

/*------------------------------------------------------------------------------------------------------*/

/*
 * @brief Gets an MQTT proxy service instance given a GATT service.
 */
static IotBleMqttService_t * _getServiceInstance( uint16_t handle );


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
 * is received. Sends the buffered message to the MQTT layer.
 */
static void _RXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback for Client Characteristic Configuration Descriptor events.
 */
void _clientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam );


static BaseType_t _getMqttStateFromMessage( uint8_t* pMessage, size_t messageLen, BaseType_t *pState );

static BaseType_t _serializeMqttControlMessage(  BaseType_t state, uint8_t* pBuffer, size_t* pLength );

/*
 * @brief This is the callback to the control characteristic. It is used to toggle ( turn on/off)
 * MQTT proxy service by the BLE IOS/Android app.
 */
static void _toggleMQTTService( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Resets the send and receive buffer for the given MQTT Service.
 * Any pending MQTT connection should be closed or the service should be disabled
 * prior to resetting the buffer.
 *
 * @param[in]  pService Pointer to the MQTT service.
 */
static void _resetBuffer( IotBleMqttService_t* pService );

/*
 * @brief Callback for BLE connect/disconnect. Toggles the Proxy state to off on a BLE disconnect.
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
extern int snprintf( char *, size_t, const char *, ... );
/*------------------------------------------------------------------------------------*/

static const IotBleAttributeEventCallback_t pCallBackArray[IOT_BLE_MQTT_NUMBER_ATTRIBUTES] =
        {
  NULL,
  _toggleMQTTService,
  _TXMesgCharCallback,
  _clientCharCfgDescrCallback,
  _RXMesgCharCallback,
  _TXLargeMesgCharCallback,
  _clientCharCfgDescrCallback,
  _RXLargeMesgCharCallback
};

/*-----------------------------------------------------------*/

static IotBleMqttService_t * _getServiceInstance( uint16_t handle )
{
    uint8_t id;
    IotBleMqttService_t * pMQTTSvc = NULL;

    for( id = 0; id < IOT_BLE_MQTT_MAX_SVC_INSTANCES; id++ )
    {
        /* Check that the handle is included in the service. */
        if(( handle > _handlesBuffer[id][0] )&&
          (handle <= _handlesBuffer[id][IOT_BLE_MQTT_NUMBER_ATTRIBUTES - 1]))
        {
            pMQTTSvc = &_mqttBLEServices[ id ];
            break;
        }
    }

    return pMQTTSvc;
}

/*-----------------------------------------------------------*/

static BaseType_t _sendNotification( IotBleMqttService_t * pMQTTService,
                                IotBleMqttAttributes_t characteristic,
                                uint8_t * pData,
                                size_t len )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    BaseType_t status = pdFALSE;

    attrData.handle = CHAR_HANDLE( pMQTTService->pServicePtr, characteristic );
    attrData.uuid = CHAR_UUID( pMQTTService->pServicePtr, characteristic );
    attrData.pData = pData;
    attrData.size = len;
    resp.pAttrData = &attrData;
    resp.attrDataOffset = 0;
    resp.eventStatus = eBTStatusSuccess;
    resp.rspErrorStatus = eBTRspErrorNone;

    if( IotBle_SendIndication( &resp, pMQTTService->BLEConnId, false ) == eBTStatusSuccess )
    {
        status = pdTRUE;
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

void _resetBuffer( IotBleMqttService_t* pService )
{

    if( !( pService->isEnabled ) || ( pService->connection.pMqttConnection == NULL ) )
    {
    	/*
    	 * Acquire send lock by blocking for a maximum wait time of send timeout.
    	 * This will ensure no more sender tasks enqueue the stream buffer.
    	 * Resets the stream buffer and release the lock.
    	 */
    	( void ) xSemaphoreTake( pService->connection.sendLock, pService->connection.sendTimeout  );

    	( void ) xStreamBufferReset( pService->connection.sendBuffer );

    	( void ) xSemaphoreGive( pService->connection.sendLock );


    	/*
    	 * Wait for completion of any receive callback.
    	 * Releases any pending receive buffer, Set the buffer to NULL,
    	 * reset the offset and length of the buffer to zero.
    	 */
    	( void ) xSemaphoreTake( pService->connection.recvLock, portMAX_DELAY );

    	if( pService->connection.pRecvBuffer != NULL )
    	{
    		vPortFree( pService->connection.pRecvBuffer );
    		pService->connection.pRecvBuffer = NULL;
    	}

    	pService->connection.recvOffset = 0;
    	pService->connection.recvBufferLen = 0;

    	( void ) xSemaphoreGive( pService->connection.recvLock );
    }


}



static BaseType_t _getMqttStateFromMessage( uint8_t* pMessage, size_t messageLen, BaseType_t *pState )
{
    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t result = pdTRUE;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pMessage, messageLen );

    if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        configPRINTF(( "Failed to initialize the decoder, error = %d, object type = %d\n", ret, decoderObj.type ));
        result = pdFALSE;
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_PARAM_STATE, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get state parameter, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            *pState  = ( BaseType_t ) value.value.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

static BaseType_t _serializeMqttControlMessage(  BaseType_t state, uint8_t* pBuffer, size_t* pLength )
{
    AwsIotSerializerEncoderObject_t container = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t stateMesgMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t result = pdFALSE;
    size_t length = *pLength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );
    if( ret == AWS_IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &stateMesgMap, IOT_BLE_MQTT_NUM_CONTROL_MESG_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.signedInt = state;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &stateMesgMap, IOT_BLE_MQTT_PARAM_STATE, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        ret = IOT_BLE_MESG_ENCODER.closeContainer( &container, &stateMesgMap );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pLength = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &container );
        }
        else
        {
            *pLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &container, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &container );
        result = pdTRUE;
    }

    return result;
}

/*-----------------------------------------------------------*/

void _toggleMQTTService( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleMqttService_t * pService;
    BaseType_t proxyEnable;
    BaseType_t result;
    uint8_t *pMesg = NULL;
    size_t mesgLen = 0;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        resp.pAttrData->handle = pWriteParam->attrHandle;
        pService = _getServiceInstance( pWriteParam->attrHandle );
        configASSERT( ( pService != NULL ) );

        if( !pWriteParam->isPrep )
        {
            result = _getMqttStateFromMessage( pWriteParam->pValue, pWriteParam->length, &proxyEnable );

            if( result == pdTRUE )
            {
                if( proxyEnable == pdTRUE )
                {
                    pService->isEnabled = true;
                }
                else
                {
                    pService->isEnabled = false;
                    _resetBuffer( pService );
                }

                resp.eventStatus = eBTStatusSuccess;
            }
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            resp.pAttrData->pData = pWriteParam->pValue;
            resp.attrDataOffset = pWriteParam->offset;
            resp.pAttrData->size = pWriteParam->length;
            IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
        }
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pService = _getServiceInstance( pEventParam->pParamRead->attrHandle );
        configASSERT( ( pService != NULL ) );

        result = _serializeMqttControlMessage( ( BaseType_t ) pService->isEnabled, NULL, &mesgLen );
        if( result == pdTRUE )
        {
            pMesg = pvPortMalloc( mesgLen );
            if( pMesg != NULL )
            {
                result = _serializeMqttControlMessage( ( BaseType_t ) pService->isEnabled, pMesg, &mesgLen );
            }
            else
            {
                configPRINTF(( "Failed to allocate memory to send state message\n" ));
                result = pdFALSE;
            }
        }
        if( result == pdTRUE )
        {


            resp.pAttrData->handle =pEventParam->pParamRead->attrHandle;
            resp.eventStatus = eBTStatusSuccess;
            resp.pAttrData->pData = ( uint8_t * ) pMesg;
            resp.attrDataOffset = 0;
            resp.pAttrData->size = mesgLen;
            IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
        }
    }
}
/*-----------------------------------------------------------*/

void _TXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
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

void _TXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleReadEventParams_t * pReadParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleMqttService_t * pService;
    size_t xBytesToSend = 0;
    uint8_t * pData;


    if( pEventParam->xEventType == eBLERead )
    {

        pReadParam = pEventParam->pParamRead;
        
        pService = _getServiceInstance( pReadParam->attrHandle );
    configASSERT( ( pService != NULL ) );


    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;
        resp.pAttrData->handle = pReadParam->attrHandle;
        xBytesToSend = IOT_BLE_MQTT_TRANSFER_LEN( BLEConnMTU );

        pData = pvPortMalloc( xBytesToSend );
        if( pData != NULL )
        {
            xBytesToSend = xStreamBufferReceive( pService->connection.sendBuffer, pData, xBytesToSend, ( TickType_t ) 0ULL );
            resp.pAttrData->pData = pData;
            resp.pAttrData->size = xBytesToSend;
            resp.attrDataOffset = 0;
            resp.eventStatus = eBTStatusSuccess;
        }

        IotBle_SendResponse( &resp, pReadParam->connId, pReadParam->transId );

        if( pData != NULL )
        {
            vPortFree( pData );
        }

        /* If this was the last chunk of a large message, release the send lock */
        if( ( resp.eventStatus == eBTStatusSuccess ) &&
            ( xBytesToSend < IOT_BLE_MQTT_TRANSFER_LEN( BLEConnMTU ) ) )
        {
            ( void ) xSemaphoreGive( pService->connection.sendLock );
        }
    }
}

/*-----------------------------------------------------------*/

void _RXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleMqttService_t * pService;
    BaseType_t result = pdTRUE;
    uint8_t *pBufOffset;
    uint8_t ret;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        pService = _getServiceInstance( pWriteParam->attrHandle );
        configASSERT( ( pService != NULL ) );

        resp.pAttrData->handle = pWriteParam->attrHandle;
        
        resp.pAttrData->uuid = CHAR_UUID( pService->pServicePtr, IOT_BLE_MQTT_CHAR_RX_LARGE );

        if( ( !pWriteParam->isPrep ) &&
        		( pService->connection.pMqttConnection != NULL ) &&
				( pService->isEnabled ) )
        {
        	if( xSemaphoreTake( pService->connection.recvLock, 0 ) == pdTRUE )
        	{
        		/**
        		 * Create a new buffer if the buffer is empty.
        		 */
        		if( pService->connection.pRecvBuffer == NULL )
        		{
        			pService->connection.pRecvBuffer = pvPortMalloc( IOT_BLE_MQTT_RX_BUFFER_SIZE );
        			if( pService->connection.pRecvBuffer != NULL )
        			{
        				pService->connection.recvBufferLen = IOT_BLE_MQTT_RX_BUFFER_SIZE;

        			}
        			else
        			{
        				result = pdFALSE;
					}
        		}
        		else
        		{
        			/**
        			 *  If current buffer can't hold the received data, resize the buffer by twicke the current size.
        			 */
        			if( ( pService->connection.recvOffset + pWriteParam->length ) > pService->connection.recvBufferLen )
        			{
        				pService->connection.pRecvBuffer = _reallocBuffer( pService->connection.pRecvBuffer,
        						pService->connection.recvBufferLen,
								( 2 * pService->connection.recvBufferLen ) );

        				if( pService->connection.pRecvBuffer != NULL )
        				{
        					pService->connection.recvBufferLen = ( pService->connection.recvBufferLen * 2 );

        				}
        				else
        				{
        					result = pdFALSE;
        				}

        			}
        		}

        		if( result == pdTRUE )
        		{
        			/**
        			 * Copy the received data into the buffer.
        			 */
        			pBufOffset = ( uint8_t* ) pService->connection.pRecvBuffer + pService->connection.recvOffset;
        			memcpy( pBufOffset, pWriteParam->pValue, pWriteParam->length );
        			pService->connection.recvOffset += pWriteParam->length;

        			if( pWriteParam->length < IOT_BLE_MQTT_TRANSFER_LEN( BLEConnMTU ) )
        			{

        				( void ) AwsIotMqtt_ReceiveCallback( pService->connection.pMqttConnection,
        						                pService->connection.pRecvBuffer,
        										0,
												pService->connection.recvOffset,
        										vPortFree );

        				pService->connection.pRecvBuffer = NULL;
        				pService->connection.recvBufferLen = 0;
        				pService->connection.recvOffset = 0;
        			}

        			resp.eventStatus = eBTStatusSuccess;
        		}

        		( void ) xSemaphoreGive( pService->connection.recvLock );
        	}
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

void _RXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleMqttService_t * pService;
    uint8_t ret = 0;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        pService = _getServiceInstance( pWriteParam->attrHandle );
        configASSERT( ( pService != NULL ) );
        resp.pAttrData->handle = pWriteParam->attrHandle;
        resp.pAttrData->uuid = CHAR_UUID( pService->pServicePtr, IOT_BLE_MQTT_CHAR_RX_LARGE );

        if( ( !pWriteParam->isPrep ) &&
               		( pService->connection.pMqttConnection != NULL ) &&
       				( pService->isEnabled ) )
        {
        		( void ) AwsIotMqtt_ReceiveCallback( pService->connection.pMqttConnection,
        				( const void * ) pWriteParam->pValue,
						0,
						pWriteParam->length,
						NULL );
        		resp.eventStatus = eBTStatusSuccess;
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

void _clientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    IotBleMqttService_t * pMQTTService;
    uint16_t CCFGValue;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        resp.pAttrData->handle = pWriteParam->attrHandle;
        pMQTTService = _getServiceInstance( pWriteParam->attrHandle );
        configASSERT( ( pMQTTService != NULL ) );

        if( pWriteParam->length == 2 )
        {
            CCFGValue = ( pWriteParam->pValue[ 1 ] << 8 ) | pWriteParam->pValue[ 0 ];

            if( pEventParam->pParamWrite->attrHandle == pMQTTService->pServicePtr->pusHandlesBuffer[IOT_BLE_MQTT_CHAR_DESCR_TX_MESG])
            {
                 pMQTTService->descrVal[ IOT_BLE_MQTT_TX_MSG_DESCR ] = CCFGValue;
            }else 
            {
                 pMQTTService->descrVal[ IOT_BLE_MQTT_TX_LARGE_MSG_DESCR ] = CCFGValue;
            }
            
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
        pMQTTService = _getServiceInstance( pEventParam->pParamRead->attrHandle );
        configASSERT( ( pMQTTService != NULL ) );

        resp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
        resp.eventStatus = eBTStatusSuccess;
        if( pEventParam->pParamWrite->attrHandle == pMQTTService->pServicePtr->pusHandlesBuffer[IOT_BLE_MQTT_CHAR_DESCR_TX_MESG])
        {
            resp.pAttrData->pData = ( uint8_t * ) &pMQTTService->descrVal[ IOT_BLE_MQTT_TX_MSG_DESCR ];
        }else 
        {
            resp.pAttrData->pData = ( uint8_t * ) &pMQTTService->descrVal[ IOT_BLE_MQTT_TX_LARGE_MSG_DESCR ];
        }

        resp.pAttrData->size = 2;
        resp.attrDataOffset = 0;
        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

static void _connectionCallback( BTStatus_t status,
                                 uint16_t connId,
                                 bool connected,
                                 BTBdaddr_t * pRemoteBdAddr )
{
    uint8_t id;
    IotBleMqttService_t *pService;
    
    if( status == eBTStatusSuccess )
    {
        for( id = 0; id < IOT_BLE_MQTT_MAX_SVC_INSTANCES; id++ )
        {
            pService = &_mqttBLEServices[ id ];
            if( connected == true )
            {
                pService->BLEConnId = connId;
            }
            else
            {
                configPRINTF( ( "Disconnect received for MQTT service instance %d\n", id ) );
                pService->isEnabled = false;
                pService->connection.pMqttConnection = NULL;
                _resetBuffer( pService );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t Mtu )
{
	/* Change the MTU size for the connection */
	if( BLEConnMTU != Mtu )
	{
		configPRINTF( ( "Changing MTU size for BLE connection from %d to %d\n", BLEConnMTU, Mtu ) );
		BLEConnMTU = Mtu;
	}

}

/*-----------------------------------------------------------*/

BaseType_t IotBleMqtt_Init( void )
{
    uint8_t id;
    IotBleEventsCallbacks_t callback;
    IotBleMqttService_t *pService;
    BTStatus_t status = eBTStatusFail;
    BaseType_t error = pdFAIL;

    BLEConnMTU = IOT_BLE_MQTT_DEFAULT_MTU_SIZE;

    for( id = 0; id < IOT_BLE_MQTT_MAX_SVC_INSTANCES; id++ )
    {
        pService = &_mqttBLEServices[ id ];

        /* Initialize service */
        pService->isInit = false;
        pService->isEnabled = false;
        pService->pServicePtr = &_BLEServices[id];
        pService->pServicePtr->pusHandlesBuffer = _handlesBuffer[id];
        pService->pServicePtr->ucInstId = id;
        pService->pServicePtr->xNumberOfAttributes = IOT_BLE_MQTT_NUMBER_ATTRIBUTES;
        pService->pServicePtr->pxBLEAttributes = (BTAttribute_t *)_pAttributeTable;

        status = IotBle_CreateService( pService->pServicePtr, (IotBleAttributeEventCallback_t *)pCallBackArray );

    	if( status == eBTStatusSuccess )
        {
            pService->isInit = true;
            callback.pConnectionCb = _connectionCallback;

            status = IotBle_RegisterEventCb( eBLEConnection, callback );
        }

    	if( status == eBTStatusSuccess )
        {
            callback.pMtuChangedCb = _MTUChangedCallback;
            status = IotBle_RegisterEventCb( eBLEMtuChanged, callback );
        }

    	if( status == eBTStatusSuccess )
        {
            pService->connection.sendTimeout = pdMS_TO_TICKS( IOT_BLE_MQTT_DEFAULT_SEND_TIMEOUT_MS );
            pService->connection.sendLock = xSemaphoreCreateBinary();
            if( pService->connection.sendLock != NULL )
            {
                ( void ) xSemaphoreGive( pService->connection.sendLock );
            }
            else
            {
            	status = eBTStatusFail;
            }
        }

    	if( status == eBTStatusSuccess )
        {
        	pService->connection.recvLock = xSemaphoreCreateBinary();
        	if( pService->connection.recvLock != NULL )
        	{
        		( void ) xSemaphoreGive( pService->connection.recvLock );
        	}
        	else
        	{
            	status = eBTStatusFail;
        	}

        }

        if( status == eBTStatusSuccess )
        {
        	pService->connection.sendBuffer = xStreamBufferCreate( IOT_BLE_MQTT_TX_BUFFER_SIZE, IOT_BLE_MQTT_DEFAULT_MTU_SIZE );

            if(pService->connection.sendBuffer == NULL )
            {
            	status = eBTStatusFail;
            }
        }

        if( status != eBTStatusSuccess )
        {
            if( pService->connection.sendLock != NULL )
            {
                vSemaphoreDelete( pService->connection.sendLock );
            }

            if( pService->connection.recvLock != NULL )
            {
            	vSemaphoreDelete( pService->connection.recvLock );
            }

            if( pService->connection.sendBuffer != NULL )
            {
                vStreamBufferDelete( pService->connection.sendBuffer );
            }

            break;
        }
    }

    if(status == eBTStatusSuccess)
    {
    	error = pdPASS;
    }
    return error;
}

/*-----------------------------------------------------------*/

BaseType_t IotBleMqtt_CreateConnection( AwsIotMqttConnection_t* pMqttConnection, IotBleMqttConnection_t* pConnection )
{
    BaseType_t ret = pdFALSE;
    IotBleMqttService_t* pService;
    int lX;

    for( lX = 0; lX < IOT_BLE_MQTT_MAX_SVC_INSTANCES; lX++ )
    {
    	pService = &_mqttBLEServices[ lX ];
        if( ( pService->isEnabled ) && ( pService->connection.pMqttConnection == NULL ) )
        {
        	pService->connection.pMqttConnection = pMqttConnection;
        	*pConnection =  ( IotBleMqttConnection_t ) pService;
        	ret = pdTRUE;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

void IotBleMqtt_CloseConnection( IotBleMqttConnection_t xConnection )
{
    IotBleMqttService_t * pService = ( IotBleMqttService_t * ) xConnection;
    if( ( pService != NULL ) && ( pService->connection.pMqttConnection != NULL ) )
    {
    	pService->connection.pMqttConnection = NULL;
    }
}

void IotBleMqtt_DestroyConnection( IotBleMqttConnection_t xConnection )
{
    IotBleMqttService_t* pService = ( IotBleMqttService_t * ) xConnection;
    if( ( pService != NULL ) && ( pService->connection.pMqttConnection == NULL ) )
    {
        _resetBuffer( pService );
    }
}

/*-----------------------------------------------------------*/

BaseType_t IotBleMqtt_SetSendTimeout( IotBleMqttConnection_t xConnection, uint16_t usTimeoutMS )
{
    BaseType_t ret = pdFALSE;
    IotBleMqttService_t * pService = ( IotBleMqttService_t * ) xConnection;

    if( pService != NULL )
    {
        pService->connection.sendTimeout = pdMS_TO_TICKS( usTimeoutMS );
        ret = pdTRUE;
    }
    return ret;
}

/*-----------------------------------------------------------*/

size_t IotBleMqtt_Send( void* pvConnection, const void * const pvMessage, size_t messageLength )
{
    IotBleMqttService_t * pService = ( IotBleMqttService_t * ) pvConnection;
    size_t sendLen, remainingLen = messageLength;
    TickType_t remainingTime = pService->connection.sendTimeout;
    TimeOut_t timeout;
    uint8_t * pData;

    vTaskSetTimeOutState( &timeout );

    if( ( pService != NULL ) && ( pService->isEnabled ) && ( pService->connection.pMqttConnection != NULL ) )
    {
        if( messageLength < ( size_t ) IOT_BLE_MQTT_TRANSFER_LEN( BLEConnMTU ) )
        {
            if( _sendNotification( pService, IOT_BLE_MQTT_CHAR_TX_MESG, ( uint8_t *) pvMessage, messageLength ) == pdTRUE )
            {
                remainingLen = 0;
            }else
            {
                 configPRINTF( ( "Failed to send notify\n") );
            }
        }
        else
        {
            if( xSemaphoreTake( pService->connection.sendLock, remainingTime ) == pdPASS )
            {
                sendLen = ( size_t ) IOT_BLE_MQTT_TRANSFER_LEN( BLEConnMTU );
                pData = ( uint8_t *) pvMessage;

                if( _sendNotification( pService, IOT_BLE_MQTT_CHAR_TX_LARGE_MESG, pData, sendLen ) == pdTRUE )
                {
                    remainingLen = remainingLen - sendLen;
                    pData += sendLen;

                    while( remainingLen > 0 )
                    {
                        if( !( pService->isEnabled ) ||
                        		( pService->connection.pMqttConnection == NULL ) ||
                        		( xTaskCheckForTimeOut( &timeout, &remainingTime ) == pdTRUE ) )
                        {
                            xStreamBufferReset( pService->connection.sendBuffer );
                            xSemaphoreGive( pService->connection.sendLock );
                            break;
                        }

                        if( remainingLen < IOT_BLE_MQTT_TX_BUFFER_SIZE )
                        {
                            sendLen = remainingLen;
                        }
                        else
                        {
                            sendLen = IOT_BLE_MQTT_TX_BUFFER_SIZE;
                        }

                        sendLen = xStreamBufferSend( pService->connection.sendBuffer, pData, sendLen, remainingTime );
                        remainingLen -= sendLen;
                        pData += sendLen;

                    }
                }else
                {
                      configPRINTF( ( "Failed to send notify\n") );
                }
            }
        }
    }
    else
    {
        configPRINTF( ( "Failed to send notification, mqtt proxy state:%d \n", pService->isEnabled ) );
    }

    return( messageLength - remainingLen );
}
