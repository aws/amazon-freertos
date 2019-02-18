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


#define mqttBLECHAR_CONTROL_UUID_TYPE \
{ \
    .uu.uu128 = mqttBLECHAR_CONTROL_UUID,\
    .ucType   = eBTuuidType128 \
}
#define mqttBLECHAR_TX_MESG_UUID_TYPE \
{  \
    .uu.uu128 = mqttBLECHAR_TX_MESG_UUID,\
    .ucType  = eBTuuidType128\
}

#define mqttBLECHAR_RX_MESG_UUID_TYPE \
{\
    .uu.uu128 = mqttBLECHAR_RX_MESG_UUID,\
    .ucType  = eBTuuidType128\
}

#define mqttBLECHAR_TX_LARGE_MESG_UUID_TYPE \
{\
    .uu.uu128 = mqttBLECHAR_TX_LARGE_MESG_UUID,\
    .ucType  = eBTuuidType128\
}

#define mqttBLECHAR_RX_LARGE_MESG_UUID_TYPE \
{\
    .uu.uu128 = mqttBLECHAR_RX_LARGE_MESG_UUID,\
    .ucType  = eBTuuidType128\
}
#define mqttBLECCFG_UUID_TYPE \
{\
    .uu.uu16 = mqttBLECCFG_UUID,\
    .ucType  = eBTuuidType16\
}
/**
 * @brief UUID for Device Information Service.
 *
 * This UUID is used in advertisement for the companion apps to discover and connect to the device.
 */
#define mqttBLESERVICE_UUID_TYPE \
{ \
    .uu.uu128 = mqttBLESERVICE_UUID,\
    .ucType   = eBTuuidType128\
}

#define IS_VALID_SERIALIZER_RET( ret, pxSerializerBuf )                                \
    (  ( ret == AWS_IOT_SERIALIZER_SUCCESS ) ||                                        \
          (  ( !pxSerializerBuf ) && ( ret == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )

static uint16_t pusHandlesBuffer[mqttBLEMAX_SVC_INSTANCES][eMQTTBLE_NUM_ATTRIBUTES];

#define CHAR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ch_idx] )
#define CHAR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ch_idx].xCharacteristic.xUuid )
#define DESCR_HANDLE( svc, descr_idx )    ( ( svc )->pusHandlesBuffer[descr_idx] )


/*-----------------------------------------------------------------------------------------------------*/
static MqttBLEService_t xMqttBLEServices[ mqttBLEMAX_SVC_INSTANCES ] = { 0 };
static BTService_t xBLEServices[ mqttBLEMAX_SVC_INSTANCES ] = { 0 };

static const BTAttribute_t pxAttributeTable[] = {
     {    
          .xServiceUUID = mqttBLESERVICE_UUID_TYPE
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = mqttBLECHAR_CONTROL_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = mqttBLECHAR_TX_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = (  eBTPropRead | eBTPropNotify  )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = mqttBLECCFG_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = mqttBLECHAR_RX_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = mqttBLECHAR_TX_LARGE_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = ( eBTPropRead | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = mqttBLECCFG_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = mqttBLECHAR_RX_LARGE_MESG_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     }
};


/**
 * Variable stores the MTU Size for the BLE connection.
 */
static uint16_t usBLEConnMTU;

/*------------------------------------------------------------------------------------------------------*/

/*
 * @brief Gets an MQTT proxy service instance given a GATT service.
 */
static MqttBLEService_t * prxGetServiceInstance( uint16_t usHandle );


/**
 * @brief Reallocate a buffer; data is also copied from the old
 * buffer into the new buffer.
 *
 * @param[in] pucOldBuffer The current network receive buffer.
 * @param[in] xOldBufferSize The size (in bytes) of pucOldBuffer.
 * @param[in] xNewBufferSize Requested size of the reallocated buffer.
 * copied into the new buffer.
 *
 * @return Pointer to a new, reallocated buffer; NULL if buffer reallocation failed.
 */
static uint8_t * prvReallocBuffer( uint8_t * pucOldBuffer,
                                          size_t xOldBufferSize,
                                          size_t xNewBufferSize );


/*
 * @brief Callback to register for events (read) on TX message characteristic.
 */
static void vTXMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback to register for events (write) on RX message characteristic.
 *
 */
static void vRXMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback to register for events (read) on TX large message characteristic.
 * Buffers a large message and sends the message in chunks of size MTU at a time as response
 * to the read request. Keeps the buffer until last message is read.
 */
static void vTXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback to register for events (write) on RX large message characteristic.
 * Copies the individual write packets into a buffer untill a packet less than BLE MTU size
 * is received. Sends the buffered message to the MQTT layer.
 */
static void vRXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback for Client Characteristic Configuration Descriptor events.
 */
void vClientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam );


static BaseType_t prxGetMqttStateFromMessage( uint8_t* pucMessage, size_t xMessageLen, BaseType_t *pxState );

static BaseType_t prxSerializeMqttControlMessage(  BaseType_t xState, uint8_t* pucBuffer, size_t* pxLength );

/*
 * @brief This is the callback to the control characteristic. It is used to toggle ( turn on/off)
 * MQTT proxy service by the BLE IOS/Android app.
 */
static void vToggleMQTTService( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Resets the send and receive buffer for the given MQTT Service.
 * Any pending MQTT connection should be closed or the service should be disabled
 * prior to resetting the buffer.
 *
 * @param[in]  pxService Pointer to the MQTT service.
 */
static void prvResetBuffer( MqttBLEService_t* pxService );

/*
 * @brief Callback for BLE connect/disconnect. Toggles the Proxy state to off on a BLE disconnect.
 */
static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr );

/*
 * @brief Callback to receive notification when the MTU of the BLE connection changes.
 */
static void vMTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu );

/*
 * @brief stdio.h conflict with posix library on some platform so using extern snprintf so not to include it.
 */
extern int snprintf( char *, size_t, const char *, ... );
/*------------------------------------------------------------------------------------*/

static const IotBleAttributeEventCallback_t pxCallBackArray[eMQTTBLE_NUM_ATTRIBUTES] =
        {
  NULL,
  vToggleMQTTService,
  vTXMesgCharCallback,
  vClientCharCfgDescrCallback,
  vRXMesgCharCallback,
  vTXLargeMesgCharCallback,
  vClientCharCfgDescrCallback,
  vRXLargeMesgCharCallback
};

/*-----------------------------------------------------------*/

static MqttBLEService_t * prxGetServiceInstance( uint16_t usHandle )
{
    uint8_t ucId;
    MqttBLEService_t * pxMQTTSvc = NULL;

    for( ucId = 0; ucId < mqttBLEMAX_SVC_INSTANCES; ucId++ )
    {
        /* Check that the handle is included in the service. */
        if(( usHandle > pusHandlesBuffer[ucId][0] )&&
          (usHandle <= pusHandlesBuffer[ucId][eMQTTBLE_NUM_ATTRIBUTES - 1]))
        {
            pxMQTTSvc = &xMqttBLEServices[ ucId ];
            break;
        }
    }

    return pxMQTTSvc;
}

/*-----------------------------------------------------------*/

static BaseType_t prxSendNotification( MqttBLEService_t * pxMQTTService,
                                MQTTBLEAttributes_t xCharacteristic,
                                uint8_t * pucData,
                                size_t xLen )
{
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    BaseType_t xStatus = pdFALSE;

    xAttrData.handle = CHAR_HANDLE( pxMQTTService->pxServicePtr, xCharacteristic );
    xAttrData.uuid = CHAR_UUID( pxMQTTService->pxServicePtr, xCharacteristic );
    xAttrData.pData = pucData;
    xAttrData.size = xLen;
    xResp.pAttrData = &xAttrData;
    xResp.attrDataOffset = 0;
    xResp.eventStatus = eBTStatusSuccess;
    xResp.rspErrorStatus = eBTRspErrorNone;

    if( IotBle_SendIndication( &xResp, pxMQTTService->usBLEConnId, false ) == eBTStatusSuccess )
    {
        xStatus = pdTRUE;
    }

    return xStatus;
}

static uint8_t * prvReallocBuffer( uint8_t * pucOldBuffer,
                                          size_t xOldBufferSize,
                                          size_t xNewBufferSize )
{
    uint8_t * pucNewBuffer = NULL;

    /* This function should not be called with a smaller new buffer size or a
     * copy offset greater than the old buffer size. */
    configASSERT( xNewBufferSize >= xOldBufferSize );

    /* Allocate a larger receive buffer. */
    pucNewBuffer = pvPortMalloc( xNewBufferSize );

    /* Copy data into the new buffer. */
    if( pucNewBuffer != NULL )
    {
        ( void ) memcpy( pucNewBuffer,
                         pucOldBuffer,
                         xOldBufferSize );
    }

    /* Free the old buffer. */
    vPortFree( pucOldBuffer );

    return pucNewBuffer;
}

/*-----------------------------------------------------------*/

void prvResetBuffer( MqttBLEService_t* pxService )
{

    if( !( pxService->bIsEnabled ) || ( pxService->xConnection.pxMqttConnection == NULL ) )
    {
    	/*
    	 * Acquire send lock by blocking for a maximum wait time of send timeout.
    	 * This will ensure no more sender tasks enqueue the stream buffer.
    	 * Resets the stream buffer and release the lock.
    	 */
    	( void ) xSemaphoreTake( pxService->xConnection.xSendLock, pxService->xConnection.xSendTimeout  );

    	( void ) xStreamBufferReset( pxService->xConnection.xSendBuffer );

    	( void ) xSemaphoreGive( pxService->xConnection.xSendLock );


    	/*
    	 * Wait for completion of any receive callback.
    	 * Releases any pending receive buffer, Set the buffer to NULL,
    	 * reset the offset and length of the buffer to zero.
    	 */
    	( void ) xSemaphoreTake( pxService->xConnection.xRecvLock, portMAX_DELAY );

    	if( pxService->xConnection.pRecvBuffer != NULL )
    	{
    		vPortFree( pxService->xConnection.pRecvBuffer );
    		pxService->xConnection.pRecvBuffer = NULL;
    	}

    	pxService->xConnection.xRecvOffset = 0;
    	pxService->xConnection.xRecvBufferLen = 0;

    	( void ) xSemaphoreGive( pxService->xConnection.xRecvLock );
    }


}



static BaseType_t prxGetMqttStateFromMessage( uint8_t* pucMessage, size_t xMessageLen, BaseType_t *pxState )
{
    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xRet = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t xResult = pdTRUE;

    xRet = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pucMessage, xMessageLen );

    if( ( xRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        configPRINTF(( "Failed to initialize the decoder, error = %d, object type = %d\n", xRet, xDecoderObj.type ));
        xResult = pdFALSE;
    }

    if( xResult == pdTRUE )
    {
        xRet = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEPARAM_STATE, &xValue );
        if( ( xRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get state parameter, error = %d, value type = %d\n", xRet, xValue.type ));
            xResult = pdFALSE;
        }
        else
        {
            *pxState  = ( BaseType_t ) xValue.value.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    return xResult;
}

static BaseType_t prxSerializeMqttControlMessage(  BaseType_t xState, uint8_t* pucBuffer, size_t* pxLength )
{
    AwsIotSerializerEncoderObject_t xContainer = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t xStateMesgMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t xValue = { 0 };
    AwsIotSerializerError_t xRet = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t xResult = pdFALSE;
    size_t xLength = *pxLength;

    xRet = IOT_BLE_MESG_ENCODER.init( &xContainer, pucBuffer, xLength );
    if( xRet == AWS_IOT_SERIALIZER_SUCCESS )
    {
        xRet = IOT_BLE_MESG_ENCODER.openContainer( &xContainer, &xStateMesgMap, mqttBLE_NUM_CONTROL_MESG_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( xRet, pucBuffer ) )
    {
        xValue.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xValue.value.signedInt = xState;
        xRet = IOT_BLE_MESG_ENCODER.appendKeyValue( &xStateMesgMap, mqttBLEPARAM_STATE, xValue );
    }

    if( IS_VALID_SERIALIZER_RET( xRet, pucBuffer ) )
    {
        xRet = IOT_BLE_MESG_ENCODER.closeContainer( &xContainer, &xStateMesgMap );
    }

    if( IS_VALID_SERIALIZER_RET( xRet, pucBuffer ) )
    {
        if( pucBuffer == NULL )
        {
            *pxLength = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xContainer );
        }
        else
        {
            *pxLength = IOT_BLE_MESG_ENCODER.getEncodedSize( &xContainer, pucBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &xContainer );
        xResult = pdTRUE;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void vToggleMQTTService( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    MqttBLEService_t * pxService;
    BaseType_t xProxyEnable;
    BaseType_t xResult;
    uint8_t *pucMesg = NULL;
    size_t xMesgLen = 0;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;
        xResp.pAttrData->handle = pxWriteParam->attrHandle;
        pxService = prxGetServiceInstance( pxWriteParam->attrHandle );
        configASSERT( ( pxService != NULL ) );

        if( !pxWriteParam->isPrep )
        {
            xResult = prxGetMqttStateFromMessage( pxWriteParam->pValue, pxWriteParam->length, &xProxyEnable );

            if( xResult == pdTRUE )
            {
                if( xProxyEnable == pdTRUE )
                {
                    pxService->bIsEnabled = true;
                }
                else
                {
                    pxService->bIsEnabled = false;
                    prvResetBuffer( pxService );
                }

                xResp.eventStatus = eBTStatusSuccess;
            }
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            xResp.pAttrData->pData = pxWriteParam->pValue;
            xResp.attrDataOffset = pxWriteParam->offset;
            xResp.pAttrData->size = pxWriteParam->length;
            IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        }
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pxService = prxGetServiceInstance( pEventParam->pParamRead->attrHandle );
        configASSERT( ( pxService != NULL ) );

        xResult = prxSerializeMqttControlMessage( ( BaseType_t ) pxService->bIsEnabled, NULL, &xMesgLen );
        if( xResult == pdTRUE )
        {
            pucMesg = pvPortMalloc( xMesgLen );
            if( pucMesg != NULL )
            {
                xResult = prxSerializeMqttControlMessage( ( BaseType_t ) pxService->bIsEnabled, pucMesg, &xMesgLen );
            }
            else
            {
                configPRINTF(( "Failed to allocate memory to send state message\n" ));
                xResult = pdFALSE;
            }
        }
        if( xResult == pdTRUE )
        {


            xResp.pAttrData->handle =pEventParam->pParamRead->attrHandle;
            xResp.eventStatus = eBTStatusSuccess;
            xResp.pAttrData->pData = ( uint8_t * ) pucMesg;
            xResp.attrDataOffset = 0;
            xResp.pAttrData->size = xMesgLen;
            IotBle_SendResponse( &xResp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
        }
    }
}
/*-----------------------------------------------------------*/

void vTXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleReadEventParams_t * pxReadParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( pEventParam->xEventType == eBLERead )
    {
        pxReadParam = pEventParam->pParamRead;
        xResp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
        xResp.pAttrData->pData = NULL;
        xResp.pAttrData->size = 0;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;

        IotBle_SendResponse( &xResp, pxReadParam->connId, pxReadParam->transId );
    }
}

/*-----------------------------------------------------------*/

void vTXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleReadEventParams_t * pxReadParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    MqttBLEService_t * pxService;
    size_t xBytesToSend = 0;
    uint8_t * pucData;


    if( pEventParam->xEventType == eBLERead )
    {

        pxReadParam = pEventParam->pParamRead;
        
        pxService = prxGetServiceInstance( pxReadParam->attrHandle );
    configASSERT( ( pxService != NULL ) );


    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;
        xResp.pAttrData->handle = pxReadParam->attrHandle;
        xBytesToSend = mqttBLETRANSFER_LEN( usBLEConnMTU );

        pucData = pvPortMalloc( xBytesToSend );
        if( pucData != NULL )
        {
            xBytesToSend = xStreamBufferReceive( pxService->xConnection.xSendBuffer, pucData, xBytesToSend, ( TickType_t ) 0ULL );
            xResp.pAttrData->pData = pucData;
            xResp.pAttrData->size = xBytesToSend;
            xResp.attrDataOffset = 0;
            xResp.eventStatus = eBTStatusSuccess;
        }

        IotBle_SendResponse( &xResp, pxReadParam->connId, pxReadParam->transId );

        if( pucData != NULL )
        {
            vPortFree( pucData );
        }

        /* If this was the last chunk of a large message, release the send lock */
        if( ( xResp.eventStatus == eBTStatusSuccess ) &&
            ( xBytesToSend < mqttBLETRANSFER_LEN( usBLEConnMTU ) ) )
        {
            ( void ) xSemaphoreGive( pxService->xConnection.xSendLock );
        }
    }
}

/*-----------------------------------------------------------*/

void vRXLargeMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    MqttBLEService_t * pxService;
    BaseType_t xResult = pdTRUE;
    uint8_t *pucBufOffset;
    uint8_t ucRet;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;
        pxService = prxGetServiceInstance( pxWriteParam->attrHandle );
        configASSERT( ( pxService != NULL ) );

        xResp.pAttrData->handle = pxWriteParam->attrHandle;
        
        xResp.pAttrData->uuid = CHAR_UUID( pxService->pxServicePtr, eMQTTBLE_CHAR_RX_LARGE );

        if( ( !pxWriteParam->isPrep ) &&
        		( pxService->xConnection.pxMqttConnection != NULL ) &&
				( pxService->bIsEnabled ) )
        {
        	if( xSemaphoreTake( pxService->xConnection.xRecvLock, 0 ) == pdTRUE )
        	{
        		/**
        		 * Create a new buffer if the buffer is empty.
        		 */
        		if( pxService->xConnection.pRecvBuffer == NULL )
        		{
        			pxService->xConnection.pRecvBuffer = pvPortMalloc( mqttBLERX_BUFFER_SIZE );
        			if( pxService->xConnection.pRecvBuffer != NULL )
        			{
        				pxService->xConnection.xRecvBufferLen = mqttBLERX_BUFFER_SIZE;

        			}
        			else
        			{
        				xResult = pdFALSE;
					}
        		}
        		else
        		{
        			/**
        			 *  If current buffer can't hold the received data, resize the buffer by twicke the current size.
        			 */
        			if( ( pxService->xConnection.xRecvOffset + pxWriteParam->length ) > pxService->xConnection.xRecvBufferLen )
        			{
        				pxService->xConnection.pRecvBuffer = prvReallocBuffer( pxService->xConnection.pRecvBuffer,
        						pxService->xConnection.xRecvBufferLen,
								( 2 * pxService->xConnection.xRecvBufferLen ) );

        				if( pxService->xConnection.pRecvBuffer != NULL )
        				{
        					pxService->xConnection.xRecvBufferLen = ( pxService->xConnection.xRecvBufferLen * 2 );

        				}
        				else
        				{
        					xResult = pdFALSE;
        				}

        			}
        		}

        		if( xResult == pdTRUE )
        		{
        			/**
        			 * Copy the received data into the buffer.
        			 */
        			pucBufOffset = ( uint8_t* ) pxService->xConnection.pRecvBuffer + pxService->xConnection.xRecvOffset;
        			memcpy( pucBufOffset, pxWriteParam->pValue, pxWriteParam->length );
        			pxService->xConnection.xRecvOffset += pxWriteParam->length;

        			if( pxWriteParam->length < mqttBLETRANSFER_LEN( usBLEConnMTU ) )
        			{

        				( void ) AwsIotMqtt_ReceiveCallback( pxService->xConnection.pxMqttConnection,
        						                pxService->xConnection.pRecvBuffer,
        										0,
												pxService->xConnection.xRecvOffset,
        										vPortFree );

        				pxService->xConnection.pRecvBuffer = NULL;
        				pxService->xConnection.xRecvBufferLen = 0;
        				pxService->xConnection.xRecvOffset = 0;
        			}

        			xResp.eventStatus = eBTStatusSuccess;
        		}

        		( void ) xSemaphoreGive( pxService->xConnection.xRecvLock );
        	}
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            xResp.attrDataOffset = 0;
            xResp.pAttrData->pData = &ucRet;
            xResp.pAttrData->size = 1;
            IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        }
    }
}

/*-----------------------------------------------------------*/

void vRXMesgCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    MqttBLEService_t * pxService;
    uint8_t ucRet = 0;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;
        pxService = prxGetServiceInstance( pxWriteParam->attrHandle );
        configASSERT( ( pxService != NULL ) );
        xResp.pAttrData->handle = pxWriteParam->attrHandle;
        xResp.pAttrData->uuid = CHAR_UUID( pxService->pxServicePtr, eMQTTBLE_CHAR_RX_LARGE );

        if( ( !pxWriteParam->isPrep ) &&
               		( pxService->xConnection.pxMqttConnection != NULL ) &&
       				( pxService->bIsEnabled ) )
        {
        		( void ) AwsIotMqtt_ReceiveCallback( pxService->xConnection.pxMqttConnection,
        				( const void * ) pxWriteParam->pValue,
						0,
						pxWriteParam->length,
						NULL );
        		xResp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            xResp.attrDataOffset = 0;
            xResp.pAttrData->pData = &ucRet;
            xResp.pAttrData->size = 1;
            IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        }
    }
}

/*-----------------------------------------------------------*/

void vClientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    MqttBLEService_t * pxMQTTService;
    uint16_t usCCFGValue;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;
        xResp.pAttrData->handle = pxWriteParam->attrHandle;
        pxMQTTService = prxGetServiceInstance( pxWriteParam->attrHandle );
        configASSERT( ( pxMQTTService != NULL ) );

        if( pxWriteParam->length == 2 )
        {
            usCCFGValue = ( pxWriteParam->pValue[ 1 ] << 8 ) | pxWriteParam->pValue[ 0 ];

            if( pEventParam->pParamWrite->attrHandle == pxMQTTService->pxServicePtr->pusHandlesBuffer[eMQTTBLE_CHAR_DESCR_TX_MESG])
            {
                 pxMQTTService->usDescrVal[ eMQTTBLETXMessageDescr ] = usCCFGValue;
            }else 
            {
                 pxMQTTService->usDescrVal[ eMQTTBLETXLargeMessageDescr ] = usCCFGValue;
            }
            
            xResp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            xResp.pAttrData->pData = pxWriteParam->pValue;
            xResp.pAttrData->size = pxWriteParam->length;
            xResp.attrDataOffset = pxWriteParam->offset;
            IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        }
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pxMQTTService = prxGetServiceInstance( pEventParam->pParamRead->attrHandle );
        configASSERT( ( pxMQTTService != NULL ) );

        xResp.pAttrData->handle = pEventParam->pParamRead->attrHandle;
        xResp.eventStatus = eBTStatusSuccess;
        if( pEventParam->pParamWrite->attrHandle == pxMQTTService->pxServicePtr->pusHandlesBuffer[eMQTTBLE_CHAR_DESCR_TX_MESG])
        {
            xResp.pAttrData->pData = ( uint8_t * ) &pxMQTTService->usDescrVal[ eMQTTBLETXMessageDescr ];
        }else 
        {
            xResp.pAttrData->pData = ( uint8_t * ) &pxMQTTService->usDescrVal[ eMQTTBLETXLargeMessageDescr ];
        }

        xResp.pAttrData->size = 2;
        xResp.attrDataOffset = 0;
        IotBle_SendResponse( &xResp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr )
{
    uint8_t ucId;
    MqttBLEService_t *pxService;
    
    if( xStatus == eBTStatusSuccess )
    {
        for( ucId = 0; ucId < mqttBLEMAX_SVC_INSTANCES; ucId++ )
        {
            pxService = &xMqttBLEServices[ ucId ];
            if( bConnected == true )
            {
                pxService->usBLEConnId = connId;
            }
            else
            {
                configPRINTF( ( "Disconnect received for MQTT service instance %d\n", ucId ) );
                pxService->bIsEnabled = false;
                pxService->xConnection.pxMqttConnection = NULL;
                prvResetBuffer( pxService );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void vMTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu )
{
	/* Change the MTU size for the connection */
	if( usBLEConnMTU != usMtu )
	{
		configPRINTF( ( "Changing MTU size for BLE connection from %d to %d\n", usBLEConnMTU, usMtu ) );
		usBLEConnMTU = usMtu;
	}

}

/*-----------------------------------------------------------*/

BaseType_t AwsIotMqttBLE_Init( void )
{
    uint8_t ucId;
    IotBleEventsCallbacks_t xCallback;
    MqttBLEService_t *pxService;
    BTStatus_t status = eBTStatusFail;
    BaseType_t error = pdFAIL;

    usBLEConnMTU = mqttBLEDEFAULT_MTU_SIZE;

    for( ucId = 0; ucId < mqttBLEMAX_SVC_INSTANCES; ucId++ )
    {
        pxService = &xMqttBLEServices[ ucId ];

        /* Initialize service */
        pxService->bIsInit = false;
        pxService->bIsEnabled = false;
        pxService->pxServicePtr = &xBLEServices[ucId];
        pxService->pxServicePtr->pusHandlesBuffer = pusHandlesBuffer[ucId];
        pxService->pxServicePtr->ucInstId = ucId;
        pxService->pxServicePtr->xNumberOfAttributes = eMQTTBLE_NUM_ATTRIBUTES;
        pxService->pxServicePtr->pxBLEAttributes = (BTAttribute_t *)pxAttributeTable;

        status = IotBle_CreateService( pxService->pxServicePtr, (IotBleAttributeEventCallback_t *)pxCallBackArray );

    	if( status == eBTStatusSuccess )
        {
            pxService->bIsInit = true;
            xCallback.pConnectionCb = vConnectionCallback;

            status = IotBle_RegisterEventCb( eBLEConnection, xCallback );
        }

    	if( status == eBTStatusSuccess )
        {
            xCallback.pMtuChangedCb = vMTUChangedCallback;
            status = IotBle_RegisterEventCb( eBLEMtuChanged, xCallback );
        }

    	if( status == eBTStatusSuccess )
        {
            pxService->xConnection.xSendTimeout = pdMS_TO_TICKS( mqttBLEDEFAULT_SEND_TIMEOUT_MS );
            pxService->xConnection.xSendLock = xSemaphoreCreateBinary();
            if( pxService->xConnection.xSendLock != NULL )
            {
                ( void ) xSemaphoreGive( pxService->xConnection.xSendLock );
            }
            else
            {
            	status = eBTStatusFail;
            }
        }

    	if( status == eBTStatusSuccess )
        {
        	pxService->xConnection.xRecvLock = xSemaphoreCreateBinary();
        	if( pxService->xConnection.xRecvLock != NULL )
        	{
        		( void ) xSemaphoreGive( pxService->xConnection.xRecvLock );
        	}
        	else
        	{
            	status = eBTStatusFail;
        	}

        }

        if( status == eBTStatusSuccess )
        {
        	pxService->xConnection.xSendBuffer = xStreamBufferCreate( mqttBLETX_BUFFER_SIZE, mqttBLEDEFAULT_MTU_SIZE );

            if(pxService->xConnection.xSendBuffer == NULL )
            {
            	status = eBTStatusFail;
            }
        }

        if( status != eBTStatusSuccess )
        {
            if( pxService->xConnection.xSendLock != NULL )
            {
                vSemaphoreDelete( pxService->xConnection.xSendLock );
            }

            if( pxService->xConnection.xRecvLock != NULL )
            {
            	vSemaphoreDelete( pxService->xConnection.xRecvLock );
            }

            if( pxService->xConnection.xSendBuffer != NULL )
            {
                vStreamBufferDelete( pxService->xConnection.xSendBuffer );
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

BaseType_t AwsIotMqttBLE_CreateConnection( AwsIotMqttConnection_t* pxMqttConnection, AwsIotMqttBLEConnection_t* pxConnection )
{
    BaseType_t xRet = pdFALSE;
    MqttBLEService_t* pxService;
    int lX;

    for( lX = 0; lX < mqttBLEMAX_SVC_INSTANCES; lX++ )
    {
    	pxService = &xMqttBLEServices[ lX ];
        if( ( pxService->bIsEnabled ) && ( pxService->xConnection.pxMqttConnection == NULL ) )
        {
        	pxService->xConnection.pxMqttConnection = pxMqttConnection;
        	*pxConnection =  ( AwsIotMqttBLEConnection_t ) pxService;
        	xRet = pdTRUE;
        }
    }

    return xRet;
}

/*-----------------------------------------------------------*/

void AwsIotMqttBLE_CloseConnection( AwsIotMqttBLEConnection_t xConnection )
{
    MqttBLEService_t * pxService = ( MqttBLEService_t * ) xConnection;
    if( ( pxService != NULL ) && ( pxService->xConnection.pxMqttConnection != NULL ) )
    {
    	pxService->xConnection.pxMqttConnection = NULL;
    }
}

void AwsIotMqttBLE_DestroyConnection( AwsIotMqttBLEConnection_t xConnection )
{
    MqttBLEService_t* pxService = ( MqttBLEService_t * ) xConnection;
    if( ( pxService != NULL ) && ( pxService->xConnection.pxMqttConnection == NULL ) )
    {
        prvResetBuffer( pxService );
    }
}

/*-----------------------------------------------------------*/

BaseType_t AwsIotMqttBLE_SetSendTimeout( AwsIotMqttBLEConnection_t xConnection, uint16_t usTimeoutMS )
{
    BaseType_t xRet = pdFALSE;
    MqttBLEService_t * pxService = ( MqttBLEService_t * ) xConnection;

    if( pxService != NULL )
    {
        pxService->xConnection.xSendTimeout = pdMS_TO_TICKS( usTimeoutMS );
        xRet = pdTRUE;
    }
    return xRet;
}

/*-----------------------------------------------------------*/

size_t AwsIotMqttBLE_Send( void* pvConnection, const void * const pvMessage, size_t xMessageLength )
{
    MqttBLEService_t * pxService = ( MqttBLEService_t * ) pvConnection;
    size_t xSendLen, xRemainingLen = xMessageLength;
    TickType_t xRemainingTime = pxService->xConnection.xSendTimeout;
    TimeOut_t xTimeout;
    uint8_t * pucData;

    vTaskSetTimeOutState( &xTimeout );

    if( ( pxService != NULL ) && ( pxService->bIsEnabled ) && ( pxService->xConnection.pxMqttConnection != NULL ) )
    {
        if( xMessageLength < ( size_t ) mqttBLETRANSFER_LEN( usBLEConnMTU ) )
        {
            if( prxSendNotification( pxService, eMQTTBLE_CHAR_TX_MESG, ( uint8_t *) pvMessage, xMessageLength ) == pdTRUE )
            {
                xRemainingLen = 0;
            }else
            {
                 configPRINTF( ( "Failed to send notify\n") );
            }
        }
        else
        {
            if( xSemaphoreTake( pxService->xConnection.xSendLock, xRemainingTime ) == pdPASS )
            {
                xSendLen = ( size_t ) mqttBLETRANSFER_LEN( usBLEConnMTU );
                pucData = ( uint8_t *) pvMessage;

                if( prxSendNotification( pxService, eMQTTBLE_CHAR_TX_LARGE_MESG, pucData, xSendLen ) == pdTRUE )
                {
                    xRemainingLen = xRemainingLen - xSendLen;
                    pucData += xSendLen;

                    while( xRemainingLen > 0 )
                    {
                        if( !( pxService->bIsEnabled ) ||
                        		( pxService->xConnection.pxMqttConnection == NULL ) ||
                        		( xTaskCheckForTimeOut( &xTimeout, &xRemainingTime ) == pdTRUE ) )
                        {
                            xStreamBufferReset( pxService->xConnection.xSendBuffer );
                            xSemaphoreGive( pxService->xConnection.xSendLock );
                            break;
                        }

                        if( xRemainingLen < mqttBLETX_BUFFER_SIZE )
                        {
                            xSendLen = xRemainingLen;
                        }
                        else
                        {
                            xSendLen = mqttBLETX_BUFFER_SIZE;
                        }

                        xSendLen = xStreamBufferSend( pxService->xConnection.xSendBuffer, pucData, xSendLen, xRemainingTime );
                        xRemainingLen -= xSendLen;
                        pucData += xSendLen;

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
        configPRINTF( ( "Failed to send notification, mqtt proxy state:%d \n", pxService->bIsEnabled ) );
    }

    return( xMessageLength - xRemainingLen );
}
