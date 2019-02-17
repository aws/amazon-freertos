/*
 * Amazon Kernel
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
 * http://aws.amazon.com/Kernel
 * http://www.Kernel.org
 */

/**
 * @file aws_device_information.c
 * @brief BLE GATT service to expose Amazon FreeRTOS device information.
 */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include <string.h>

#include "iot_ble_config.h"
#include "FreeRTOS.h"
#include "iot_ble_device_information.h"
#include "semphr.h"
#include "aws_clientcredential.h"
#include "task.h"

#define INT64_WIDTH                         ( 20 )
#define MAX_INTEGER_BUFFER_WIDTH            ( INT64_WIDTH + 1 )

/*------------------------------------------------------------------------------------*/
extern int snprintf( char *, size_t, const char *, ... );
static DeviceInfoService_t xService =
{
    .pxBLEService = NULL,
    .usBLEConnId  = 65535,
    .usBLEMtu     = 0,
    .usCCFGVal    = { 0 }
};

#define deviceInfoCHAR_VERSION_UUID_TYPE \
{ \
    .uu.uu128 = deviceInfoCHAR_VERSION_UUID,\
    .ucType   = eBTuuidType128 \
}
#define deviceInfoBROKER_ENDPOINT_UUID_TYPE \
{  \
    .uu.uu128 = deviceInfoBROKER_ENDPOINT_UUID,\
    .ucType  = eBTuuidType128\
}
#define deviceInfoCLIENT_CHAR_CFG_UUID_TYPE \
{\
    .uu.uu16 = deviceInfoCLIENT_CHAR_CFG_UUID,\
    .ucType  = eBTuuidType16\
}
#define deviceInfoCHAR_MTU_UUID_TYPE \
{\
    .uu.uu128 = deviceInfoCHAR_MTU_UUID,\
    .ucType  = eBTuuidType128\
}
/**
 * @brief UUID for Device Information Service.
 *
 * This UUID is used in advertisement for the companion apps to discover and connect to the device.
 */
#define xDeviceInfoSvcUUID \
{ \
    .uu.uu128 = IOT_BLE_DEVICE_INFO_SERVICE_UUID,\
    .ucType   = eBTuuidType128\
}


typedef enum {
  bledeviceinfoATTR_SERVICE,
  bledeviceinfoATTR_CHAR_END_POINT,
  bledeviceinfoATTR_CHAR_VERSION,
  bledeviceinfoATTR_CHAR_MTU,
  bledeviceinfoATTR_CHAR_DESCR_MTU,
  bledeviceinfoATTR_NUMBER
} bledeviceinfoAttr_t;

static uint16_t usHandlesBuffer[bledeviceinfoATTR_NUMBER];

static const BTAttribute_t pxAttributeTable[] = {
     {    
         .xServiceUUID =  xDeviceInfoSvcUUID
     },
    {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = deviceInfoCHAR_VERSION_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = ( eBTPropRead )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = deviceInfoBROKER_ENDPOINT_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = ( eBTPropRead )
          }
     },
{
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = deviceInfoCHAR_MTU_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
              .xProperties = ( eBTPropRead | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = deviceInfoCLIENT_CHAR_CFG_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
          }
     }
};

static const BTService_t xDeviceInformationService = 
{
  .xNumberOfAttributes = bledeviceinfoATTR_NUMBER,
  .ucInstId = 0,
  .xType = eBTServiceTypePrimary,
  .pusHandlesBuffer = usHandlesBuffer,
  .pxBLEAttributes = (BTAttribute_t *)pxAttributeTable
};


/**
 * @brief Client Characteristic Configuration descriptor callback for MTU characteristic
 *
 * Enables notification to send MTU changed information to the GATT client
 *
 * @param[in] pxAttribute Client Characteristic Configuration descriptor attribute
 * @param[in] pEventParam Write/Read request param to the attribute
 */
void vDeviceInfoCCFGCallback( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback invoked when GATT client reads MTU characteristic
 *
 * Returns the MTU value in JSON payload as response.
 *
 * @param[in] pxAttribute MTU characteristic attribute
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
void vDeviceInfoMTUCharCallback( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback invoked when GATT client reads Broker Endpoint Characteristic
 *
 * Returns the IOT Broker Endpoint provisioned for the device in a JSON payload as response.
 *
 * @param[in] pxAttribute Broker Endpoint characteristic attribute
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
void vDeviceInfoBrokerEndpointCharCallback( IotBleAttributeEvent_t * pEventParam );


/**
 * @brief Callback invoked when GATT client reads the encoding Characteristic
 * Returns the version for the device in a JSON payload as response.
 *
 * @param[in] pxAttribute Device version characteristic attribute
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
void vDeviceInfoVersionCharCallback( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback invoked MTU for the BLE connection changes.
 *
 * Updates the value for MTU characteristic. Sends a notification to GATT client if notification is enabled
 * via Client Characteristic Configuration descriptoe
 *
 * @param connId[in] Connection Identifier
 * @param usMtu[in] New MTU for the connection
 *
 */
static void vMTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu );

/**
 * @brief Callback invoked on a BLE connect and disconnect event
 *
 * Stores the connection ID of the BLE client for sending notifications.
 *
 * @param xStatus BLE status of the operation
 * @param connId Connection ID for the connection
 * @param bConnected Is Connected or disconnected
 * @param pxRemoteBdAddr Remote address of the client
 */
static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr );


static const IotBleAttributeEventCallback_t pxCallBackArray[bledeviceinfoATTR_NUMBER] =
    {
  NULL,
  vDeviceInfoVersionCharCallback,
  vDeviceInfoBrokerEndpointCharCallback,
  vDeviceInfoMTUCharCallback,
  vDeviceInfoCCFGCallback
};


/*-----------------------------------------------------------*/

BaseType_t AFRDeviceInfoSvc_Init( void )
{
    BTStatus_t xStatus = eBTStatusFail;
    BaseType_t error = pdFAIL;
    IotBleEventsCallbacks_t xCallback;

    /* Select the handle buffer. */
    xStatus = IotBle_CreateService( (BTService_t *)&xDeviceInformationService, (IotBleAttributeEventCallback_t *)pxCallBackArray );

	if( xStatus == eBTStatusSuccess )
	{
		xCallback.pMtuChangedCb = vMTUChangedCallback;
		xStatus = IotBle_RegisterEventCb( eBLEMtuChanged, xCallback );
	}

	if( xStatus == eBTStatusSuccess )
	{
		xCallback.pConnectionCb = vConnectionCallback;
		xStatus = IotBle_RegisterEventCb( eBLEConnection, xCallback );
	}

	if( xStatus == eBTStatusSuccess )
	{
		error = pdPASS;
	}
    return error;
}

/*-----------------------------------------------------------*/

void vDeviceInfoCCFGCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;


    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;
        xAttrData.handle = pxWriteParam->attrHandle;

        if( pxWriteParam->length == 2 )
        {
            xService.usCCFGVal[ eDeviceInfoMtuCharDescr ] = ( pxWriteParam->pValue[ 1 ] << 8 ) | pxWriteParam->pValue[ 0 ];
            xResp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            xAttrData.pData = pxWriteParam->pValue;
            xAttrData.size = pxWriteParam->length;
            xResp.attrDataOffset = pxWriteParam->offset;
            IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        }
    }
    else if( pEventParam->xEventType == eBLERead )
    {

        xAttrData.handle = pEventParam->pParamRead->attrHandle;
        xAttrData.pData = ( uint8_t * ) &xService.usCCFGVal[ eDeviceInfoMtuCharDescr ];
        xAttrData.size = 2;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;

        IotBle_SendResponse( &xResp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

void vDeviceInfoBrokerEndpointCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;



    if( pEventParam->xEventType == eBLERead )
    {
        xAttrData.handle = pEventParam->pParamRead->attrHandle;
        xAttrData.pData = ( uint8_t *) clientcredentialMQTT_BROKER_ENDPOINT;
        xAttrData.size = strlen(  clientcredentialMQTT_BROKER_ENDPOINT );

        xResp.pAttrData = &xAttrData;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        xResp.rspErrorStatus = eBTRspErrorNone;

        IotBle_SendResponse( &xResp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

void vDeviceInfoVersionCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;


    if( pEventParam->xEventType == eBLERead )
    {

        xAttrData.handle = pEventParam->pParamRead->attrHandle;
        xAttrData.pData = ( uint8_t* ) tskKERNEL_VERSION_NUMBER;
        xAttrData.size = strlen( tskKERNEL_VERSION_NUMBER );

        xResp.pAttrData = &xAttrData;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        xResp.rspErrorStatus = eBTRspErrorNone;


        IotBle_SendResponse( &xResp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

void vDeviceInfoMTUCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    char cMessage[ MAX_INTEGER_BUFFER_WIDTH  ] = { 0 };
    size_t xMessageLen;

    if( pEventParam->xEventType == eBLERead )
    {
        xMessageLen = snprintf( cMessage, MAX_INTEGER_BUFFER_WIDTH, "%d", xService.usBLEMtu );

        xAttrData.handle = pEventParam->pParamRead->attrHandle;
        xAttrData.pData = ( uint8_t* ) cMessage;
        xAttrData.size = xMessageLen;

        xResp.pAttrData = &xAttrData;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        xResp.rspErrorStatus = eBTRspErrorNone;
        IotBle_SendResponse( &xResp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

static void vMTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu )
{
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp = { 0 };
    char cMessage[ MAX_INTEGER_BUFFER_WIDTH  ] = { 0 };
    size_t xMessageLen;


    if( usMtu != xService.usBLEMtu )
    {
        xService.usBLEMtu = usMtu;
        xMessageLen = snprintf( cMessage, MAX_INTEGER_BUFFER_WIDTH, "%d", xService.usBLEMtu );
        
        xAttrData.handle = xDeviceInformationService.pusHandlesBuffer[bledeviceinfoATTR_CHAR_MTU];
        xAttrData.uuid = xDeviceInformationService.pxBLEAttributes[bledeviceinfoATTR_CHAR_MTU].xCharacteristic.xUuid;
        xAttrData.pData = ( uint8_t* ) cMessage;
        xAttrData.size = xMessageLen;

        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        xResp.pAttrData = &xAttrData;
        xResp.rspErrorStatus = eBTRspErrorNone;

        ( void ) IotBle_SendIndication( &xResp, xService.usBLEConnId, false );
    }
}

static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr )
{
	if( xStatus == eBTStatusSuccess )
    {
    	if( bConnected == true )
    	{
    		xService.usBLEConnId = connId;
    	}
    }
}
