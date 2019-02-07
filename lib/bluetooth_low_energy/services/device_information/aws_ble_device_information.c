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


#include <string.h>

#include "aws_ble_config.h"
#include "FreeRTOS.h"
#include "aws_ble_device_information.h"
#include "semphr.h"
#include "aws_json_utils.h"
#include "aws_clientcredential.h"
#include "task.h"

/*------------------------------------------------------------------------------------*/
extern int snprintf( char *, size_t, const char *, ... );
static DeviceInfoService_t xService =
{
    .pxBLEService = NULL,
    .usBLEConnId  = 65535,
    .usBLEMtu     =     0,
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
    .uu.uu128 = bleconfigDEVICE_INFO_SERVICE_UUID,\
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

static const BLEAttribute_t pxAttributeTable[] = {
     {    
         .xServiceUUID =  xDeviceInfoSvcUUID
     },
    {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = deviceInfoCHAR_VERSION_UUID_TYPE,
              .xPermissions = ( bleconfigCHAR_READ_PERM ),
              .xProperties = ( eBTPropRead )
          }
     },
     {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = deviceInfoBROKER_ENDPOINT_UUID_TYPE,
              .xPermissions = ( bleconfigCHAR_READ_PERM ),
              .xProperties = ( eBTPropRead )
          }
     },
{
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = deviceInfoCHAR_MTU_UUID_TYPE,
              .xPermissions = ( bleconfigCHAR_READ_PERM ),
              .xProperties = ( eBTPropRead | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = deviceInfoCLIENT_CHAR_CFG_UUID_TYPE,
             .xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM )
          }
     }
};

static const BLEService_t xDeviceInformationService = 
{
  .xNumberOfAttributes = bledeviceinfoATTR_NUMBER,
  .ucInstId = 0,
  .xType = eBTServiceTypePrimary,
  .pusHandlesBuffer = usHandlesBuffer,
  .pxBLEAttributes = (BLEAttribute_t *)pxAttributeTable
};


/**
 * @brief Client Characteristic Configuration descriptor callback for MTU characteristic
 *
 * Enables notification to send MTU changed information to the GATT client
 *
 * @param[in] pxAttribute Client Characteristic Configuration descriptor attribute
 * @param[in] pxEventParam Write/Read request param to the attribute
 */
void vDeviceInfoCCFGCallback( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Callback invoked when GATT client reads MTU characteristic
 *
 * Returns the MTU value in JSON payload as response.
 *
 * @param[in] pxAttribute MTU characteristic attribute
 * @param[in] pxEventParam Write/Read request param to the attribute
 *
 */
void vDeviceInfoMTUCharCallback( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Callback invoked when GATT client reads Broker Endpoint Characteristic
 *
 * Returns the IOT Broker Endpoint provisioned for the device in a JSON payload as response.
 *
 * @param[in] pxAttribute Broker Endpoint characteristic attribute
 * @param[in] pxEventParam Write/Read request param to the attribute
 *
 */
void vDeviceInfoBrokerEndpointCharCallback( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Callback invoked when GATT client reads the device version Characteristic
 *
 * Returns the version for the device in a JSON payload as response.
 *
 * @param[in] pxAttribute Device version characteristic attribute
 * @param[in] pxEventParam Write/Read request param to the attribute
 *
 */
void vDeviceInfoVersionCharCallback( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Callback invoked MTU for the BLE connection changes.
 *
 * Updates the value for MTU characteristic. Sends a notification to GATT client if notification is enabled
 * via Client Characteristic Configuration descriptoe
 *
 * @param usConnId[in] Connection Identifier
 * @param usMtu[in] New MTU for the connection
 *
 */
static void vMTUChangedCallback( uint16_t usConnId,
                                 uint16_t usMtu );

/**
 * @brief Callback invoked on a BLE connect and disconnect event
 *
 * Stores the connection ID of the BLE client for sending notifications.
 *
 * @param xStatus BLE status of the operation
 * @param usConnId Connection ID for the connection
 * @param bConnected Is Connected or disconnected
 * @param pxRemoteBdAddr Remote address of the client
 */
static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t usConnId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr );


static const BLEAttributeEventCallback_t pxCallBackArray[bledeviceinfoATTR_NUMBER] =
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
    BTStatus_t xStatus;
    BaseType_t xResult = pdFAIL;
    BLEEventsCallbacks_t xCallback;

    /* Select the handle buffer. */
    xStatus = BLE_CreateService( (BLEService_t *)&xDeviceInformationService, (BLEAttributeEventCallback_t *)pxCallBackArray );
        if( xStatus == eBTStatusSuccess )
        {
        xResult = pdPASS;
        }

        if( xResult == pdPASS )
        {
            xCallback.pxMtuChangedCb = vMTUChangedCallback;

            if( BLE_RegisterEventCb( eBLEMtuChanged, xCallback ) != eBTStatusSuccess )
            {
                xResult = pdFALSE;
            }
        }

        if( xResult == pdPASS )
        {
        	xCallback.pxConnectionCb = vConnectionCallback;

        	if( BLE_RegisterEventCb( eBLEConnection, xCallback ) != eBTStatusSuccess )
        	{
        		xResult = pdFAIL;
        	}
        }

    return xResult;
}

/*-----------------------------------------------------------*/

void vDeviceInfoCCFGCallback( BLEAttributeEvent_t * pxEventParam )
{
    BLEWriteEventParams_t * pxWriteParam;
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;

    xResp.pxAttrData = &xAttrData;
    xResp.xRspErrorStatus = eBTRspErrorNone;
    xResp.xEventStatus = eBTStatusFail;
    xResp.xAttrDataOffset = 0;


    if( ( pxEventParam->xEventType == eBLEWrite ) || ( pxEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pxEventParam->pxParamWrite;
        xResp.pxAttrData->xHandle = pxWriteParam->usAttrHandle;
        if( pxWriteParam->xLength == 2 )
        {
            xService.usCCFGVal[ eDeviceInfoMtuCharDescr ] = ( pxWriteParam->pucValue[ 1 ] << 8 ) | pxWriteParam->pucValue[ 0 ];
            xResp.xEventStatus = eBTStatusSuccess;
        }

        if( pxEventParam->xEventType == eBLEWrite )
        {
            xResp.pxAttrData->pucData = pxWriteParam->pucValue;
            xResp.pxAttrData->xSize = pxWriteParam->xLength;
            xResp.xAttrDataOffset = pxWriteParam->usOffset;
            BLE_SendResponse( &xResp, pxWriteParam->usConnId, pxWriteParam->ulTransId );
        }
    }
    else if( pxEventParam->xEventType == eBLERead )
    {
        xResp.xEventStatus = eBTStatusSuccess;
        xResp.pxAttrData->xHandle = pxEventParam->pxParamRead->usAttrHandle;
        xResp.pxAttrData->pucData = ( uint8_t * ) &xService.usCCFGVal[ eDeviceInfoMtuCharDescr ];
        xResp.pxAttrData->xSize = 2;
        xResp.xAttrDataOffset = 0;
        BLE_SendResponse( &xResp, pxEventParam->pxParamRead->usConnId, pxEventParam->pxParamRead->ulTransId );
    }
}

/*-----------------------------------------------------------*/

void vDeviceInfoBrokerEndpointCharCallback( BLEAttributeEvent_t * pxEventParam )
{
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;
    uint32_t ulMsgLen = deviceInfoBROKERENDPOINT_MSG_LEN + sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) + 1;
    uint8_t * pucData;
    uint32_t ulSendLen;

    xResp.pxAttrData = &xAttrData;
    xResp.xRspErrorStatus = eBTRspErrorNone;
    xResp.xEventStatus = eBTStatusFail;
    xResp.xAttrDataOffset = 0;

    if( pxEventParam->xEventType == eBLERead )
    {
        pucData = pvPortMalloc( ulMsgLen );

        if( pucData != NULL )
        {
            ulSendLen = snprintf( ( char * ) pucData, ulMsgLen, deviceInfoBROKERENDPOINT_MSG_FORMAT,
                                  strlen( clientcredentialMQTT_BROKER_ENDPOINT ),
                                  clientcredentialMQTT_BROKER_ENDPOINT );

            {
                xResp.pxAttrData->xHandle = pxEventParam->pxParamRead->usAttrHandle;
                xResp.xEventStatus = eBTStatusSuccess;
                xResp.pxAttrData->pucData = pucData;
                xResp.pxAttrData->xSize = ulSendLen;
                xResp.xAttrDataOffset = 0;
                BLE_SendResponse( &xResp, pxEventParam->pxParamRead->usConnId, pxEventParam->pxParamRead->ulTransId );
            }

            vPortFree( pucData );
        }
        else
        {
            configPRINTF( ( "Cannot allocate memory for sending Broker endpoint response \n" ) );
        }
    }
}

/*-----------------------------------------------------------*/

void vDeviceInfoVersionCharCallback( BLEAttributeEvent_t * pxEventParam )
{
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;
    uint32_t ulMsgLen = deviceInfoVERSION_MSG_LEN + strlen( tskKERNEL_VERSION_NUMBER ) + 1;
    uint8_t * pucData;
    uint32_t ulSendLen;

    xResp.pxAttrData = &xAttrData;
    xResp.xRspErrorStatus = eBTRspErrorNone;
    xResp.xEventStatus = eBTStatusFail;
    xResp.xAttrDataOffset = 0;

    if( pxEventParam->xEventType == eBLERead )
    {
        pucData = pvPortMalloc( ulMsgLen );

        if( pucData != NULL )
        {
            ulSendLen = snprintf( ( char * ) pucData, ulMsgLen, deviceInfoVERSION_MSG_FORMAT,
                                  strlen( tskKERNEL_VERSION_NUMBER ),
                                  tskKERNEL_VERSION_NUMBER );

            {
                xResp.pxAttrData->xHandle = pxEventParam->pxParamRead->usAttrHandle;
                xResp.xEventStatus = eBTStatusSuccess;
                xResp.pxAttrData->pucData = pucData;
                xResp.pxAttrData->xSize = ulSendLen;
                xResp.xAttrDataOffset = 0;
                BLE_SendResponse( &xResp, pxEventParam->pxParamRead->usConnId, pxEventParam->pxParamRead->ulTransId );
            }

            vPortFree( pucData );
        }
        else
        {
            configPRINTF( ( "Cannot allocate memory for sending Broker endpoint response \n" ) );
        }
    }
}


/*-----------------------------------------------------------*/

void vDeviceInfoMTUCharCallback( BLEAttributeEvent_t * pxEventParam )
{
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;
    char cMTUMsg[ deviceInfoMTU_MSG_LEN + 1 ] = { 0 };
    uint32_t ulSendLen;

    ulSendLen = snprintf( cMTUMsg, deviceInfoMTU_MSG_LEN, deviceInfoMTU_MSG_FORMAT, xService.usBLEMtu );
    xResp.pxAttrData = &xAttrData;
    xResp.xRspErrorStatus = eBTRspErrorNone;
    xResp.xEventStatus = eBTStatusFail;
    xResp.xAttrDataOffset = 0;

    if( pxEventParam->xEventType == eBLERead )
    {
        xResp.xEventStatus = eBTStatusSuccess;
        xResp.pxAttrData->xHandle = pxEventParam->pxParamRead->usAttrHandle;
        xResp.pxAttrData->pucData = ( uint8_t * ) cMTUMsg;
        xResp.pxAttrData->xSize = ulSendLen;
        xResp.xAttrDataOffset = 0;
        BLE_SendResponse( &xResp, pxEventParam->pxParamRead->usConnId, pxEventParam->pxParamRead->ulTransId );
    }
}

/*-----------------------------------------------------------*/

static void vMTUChangedCallback( uint16_t usConnId,
                                 uint16_t usMtu )
{
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp = { 0 };
    char cMTUMsg[ deviceInfoMTU_MSG_LEN + 1 ] = { 0 };
    uint32_t ulSendLen;

    if( usMtu != xService.usBLEMtu )
    {
        xService.usBLEMtu = usMtu;
        ulSendLen = snprintf( cMTUMsg, deviceInfoMTU_MSG_LEN, deviceInfoMTU_MSG_FORMAT, usMtu );

        xAttrData.xHandle = xDeviceInformationService.pusHandlesBuffer[bledeviceinfoATTR_CHAR_MTU];
        xAttrData.xUuid = xDeviceInformationService.pxBLEAttributes[bledeviceinfoATTR_CHAR_MTU].xCharacteristic.xUuid;
        xAttrData.pucData = ( uint8_t * ) cMTUMsg;
        xAttrData.xSize = ulSendLen;

        xResp.xAttrDataOffset = 0;
        xResp.xEventStatus = eBTStatusSuccess;
        xResp.pxAttrData = &xAttrData;
        xResp.xRspErrorStatus = eBTRspErrorNone;

        ( void ) BLE_SendIndication( &xResp, xService.usBLEConnId, false );
    }
}

static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t usConnId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr )
{
	if( xStatus == eBTStatusSuccess )
    {
    	if( bConnected == true )
    	{
    		xService.usBLEConnId = usConnId;
    	}
    }
}
