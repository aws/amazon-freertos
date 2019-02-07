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
 * @file aws_ble_gatt_server_demo.c
 * @brief Sample demo for a BLE GATT server
 */
#include "FreeRTOSConfig.h"
#include "aws_iot_demo.h"
#include "aws_ble_config.h"
#include "aws_ble_gatt_server_demo.h"
#include "aws_ble.h"
#include "task.h"
#include "semphr.h"



#define xServiceUUID_TYPE \
{\
    .uu.uu128 = gattDemoSVC_UUID, \
    .ucType   = eBTuuidType128 \
}
#define xCharCounterUUID_TYPE \
{\
    .uu.uu128 = gattDemoCHAR_COUNTER_UUID,\
    .ucType   = eBTuuidType128\
}
#define xCharControlUUID_TYPE \
{\
    .uu.uu128 = gattDemoCHAR_CONTROL_UUID,\
    .ucType   = eBTuuidType128\
}
#define xClientCharCfgUUID_TYPE \
{\
    .uu.uu16 = gattDemoCLIENT_CHAR_CFG_UUID,\
    .ucType  = eBTuuidType16\
}

static uint16_t usHandlesBuffer[egattDemoNbAttributes];


static const BLEAttribute_t pxAttributeTable[] = {
     {    
         .xServiceUUID =  xServiceUUID_TYPE
     },
    {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = xCharCounterUUID_TYPE,
              .xPermissions = ( bleconfigCHAR_READ_PERM ),
              .xProperties = ( eBTPropRead | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = xClientCharCfgUUID_TYPE,
             .xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM )
          }
     },
    {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic = 
         {
              .xUuid = xCharControlUUID_TYPE,
              .xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM  ),
              .xProperties = ( eBTPropRead | eBTPropWrite )
          }
     }
};

static const BLEService_t xGattDemoService = 
{
  .xNumberOfAttributes = egattDemoNbAttributes,
  .ucInstId = 0,
  .xType = eBTServiceTypePrimary,
  .pusHandlesBuffer = usHandlesBuffer,
  .pxBLEAttributes = (BLEAttribute_t *)pxAttributeTable
};

/**
 * @brief Task used to update the counter periodically.
 */
TaskHandle_t xCounterUpdateTask = NULL;

/**
 * @brief Counter value.
 */
uint32_t ulCounter = 0;

/**
 * @brief Interval in Milliseconds to update the counter.
 */
TickType_t xCounterUpdateInterval = pdMS_TO_TICKS( 1000 );

/**
 * @brief Should send the counter update as a notification.
 */
BaseType_t xNotifyCounterUpdate = pdFALSE;

/**
 * @brief BLE connection ID to send the notification.
 */
uint16_t usBLEConnectionID;


#define CHAR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ch_idx] )
#define CHAR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ch_idx].xCharacteristic.xUuid )

/**
 * @brief Callback to read request from a GATT client on the Counter value characteristic.
 * Returns the current counter value.
 * @param[in] pxAttribute Attribute structure for the characteristic
 * @param pxEventParam Event param for the read Request.
 */
void vReadCounter( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Callback to receive write request from a GATT client to set the counter status.
 *
 * Sets the event to start/stop/reset the counter update operation and sends back a response.
 *
 * @param pxAttribute
 * @param pxEventParam
 */
void vWriteCommand( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Callback to enable notification when GATT client writes a value to the Client Characteristic
 * Configuration descriptor.
 *
 * @param pxAttribute  Attribute structure for the Client Characteristic Configuration Descriptor
 * @param pxEventParam Write/Read event parametes
 */

void vEnableNotification( BLEAttributeEvent_t * pxEventParam );

/**
 * @brief Task used to update the counter value periodically
 *
 * Sends a notification of counter update to GATT client if it has enabled notification via Client Characteristic
 * Configuration Descriptor. Also receives events from GATT client and starts/stops/resets counter update.
 *
 * @param pvParams NULL
 */
static void vCounterUpdateTaskFunction( void * pvParams );

/**
 * @brief Callback for BLE connect/disconnect.
 *
 * Stops the Counter Update task on disconnection.
 *
 * @param[in] xStatus Status indicating result of connect/disconnect operation
 * @param[in] usConnId Connection Id for the connection
 * @param[in] bConnected true if connection, false if disconnection
 * @param[in] pxRemoteBdAddr Remote address of the BLE device which connected or disconnected
 */
static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t usConnId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr );
/* ---------------------------------------------------------------------------------------*/

static const BLEAttributeEventCallback_t pxCallBackArray[egattDemoNbAttributes] =
    {
  NULL,
  vReadCounter,
  vEnableNotification,
  vWriteCommand
};

/*-----------------------------------------------------------*/

void vCounterUpdateTaskFunction( void * pvParams )
{
    uint32_t xBitsToWait = EVENT_BIT( eGattDemoStart ) | EVENT_BIT( eGattDemoStop ) | EVENT_BIT( eGattDemoReset );
    BaseType_t xStarted = pdFALSE;
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;
    uint32_t ulSetBits = 0;

    xAttrData.xHandle = CHAR_HANDLE( &xGattDemoService, egattDemoCharCounter );
    xAttrData.xUuid = CHAR_UUID( &xGattDemoService, egattDemoCharCounter );
    xResp.pxAttrData = &xAttrData;
    xResp.xAttrDataOffset = 0;
    xResp.xEventStatus = eBTStatusSuccess;
    xResp.xRspErrorStatus = eBTRspErrorNone;

    ( void ) pvParams;

    while( 1 )
    {
        ( void ) xTaskNotifyWait( xBitsToWait, 0, &ulSetBits, xCounterUpdateInterval );

        if( ulSetBits & EVENT_BIT( eGattDemoReset ) )
        {
            ulCounter = 0;
        }
        else if( ulSetBits & EVENT_BIT( eGattDemoStart ) )
        {
            xStarted = pdTRUE;
        }
        else if( ulSetBits & EVENT_BIT( eGattDemoStop ) )
        {
            xStarted = pdFALSE;
        }

        if( xStarted == pdTRUE )
        {
            ulCounter++;

            if( xNotifyCounterUpdate == pdTRUE )
            {
                xAttrData.pucData = ( uint8_t * ) &ulCounter;
                xAttrData.xSize = sizeof( ulCounter );
                ( void ) BLE_SendIndication( &xResp, usBLEConnectionID, false );
            }
        }
    }

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

BaseType_t vGattDemoSvcInit( void )
{
    BaseType_t xRet = pdFALSE;
    BTStatus_t xStatus;
    BLEEventsCallbacks_t xCallback;

    /* Select the handle buffer. */
    xStatus = BLE_CreateService( (BLEService_t *)&xGattDemoService, (BLEAttributeEventCallback_t *)pxCallBackArray );
    if( xStatus == eBTStatusSuccess )
    {
        xRet = pdTRUE;
    }

    if( xRet == pdTRUE )
    {
        xCallback.pxConnectionCb = vConnectionCallback;

        if( BLE_RegisterEventCb( eBLEConnection, xCallback ) != eBTStatusSuccess )
        {
            xRet = pdFAIL;
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate(
            vCounterUpdateTaskFunction,
            "GattDemoCounterTask",
            configMINIMAL_STACK_SIZE * 4,
            NULL,
            tskIDLE_PRIORITY,
            &xCounterUpdateTask );
    }

    return xRet;
}

/*-----------------------------------------------------------*/

void vGattDemoSvcStart( void )
{
    ( void ) xTaskNotify( xCounterUpdateTask, EVENT_BIT( eGattDemoStart ), eSetBits );
}

/*-----------------------------------------------------------*/

void vGattDemoSvcStop( void )
{
    ( void ) xTaskNotify( xCounterUpdateTask, EVENT_BIT( eGattDemoStop ), eSetBits );
}

/*-----------------------------------------------------------*/

void vReadCounter( BLEAttributeEvent_t * pxEventParam )
{
    BLEReadEventParams_t * pxReadParam;
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;

    xResp.pxAttrData = &xAttrData;
    xResp.xRspErrorStatus = eBTRspErrorNone;
    xResp.xEventStatus = eBTStatusFail;
    xResp.xAttrDataOffset = 0;

    if( pxEventParam->xEventType == eBLERead )
    {
        pxReadParam = pxEventParam->pxParamRead;
        xResp.pxAttrData->xHandle = pxReadParam->usAttrHandle;
        xResp.pxAttrData->pucData = ( uint8_t * ) &ulCounter;
        xResp.pxAttrData->xSize = sizeof( ulCounter );
        xResp.xAttrDataOffset = 0;
        xResp.xEventStatus = eBTStatusSuccess;
        BLE_SendResponse( &xResp, pxReadParam->usConnId, pxReadParam->ulTransId );
    }
}

/*-----------------------------------------------------------*/

void vWriteCommand( BLEAttributeEvent_t * pxEventParam )
{
    BLEWriteEventParams_t * pxWriteParam;
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;
    uint8_t ucEvent;

    xResp.pxAttrData = &xAttrData;
    xResp.xRspErrorStatus = eBTRspErrorNone;
    xResp.xEventStatus = eBTStatusFail;
    xResp.xAttrDataOffset = 0;

    if( ( pxEventParam->xEventType == eBLEWrite ) || ( pxEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pxEventParam->pxParamWrite;
        xResp.pxAttrData->xHandle = pxWriteParam->usAttrHandle;


        if( pxWriteParam->xLength == 1 )
        {
            ucEvent = pxWriteParam->pucValue[ 0 ];
            xTaskNotify( xCounterUpdateTask, EVENT_BIT( ucEvent ), eSetBits );
            xResp.xEventStatus = eBTStatusSuccess;
        }

        if( pxEventParam->xEventType == eBLEWrite )
        {
            xResp.pxAttrData->pucData = pxWriteParam->pucValue;
            xResp.xAttrDataOffset = pxWriteParam->usOffset;
            xResp.pxAttrData->xSize = pxWriteParam->xLength;
            BLE_SendResponse( &xResp, pxWriteParam->usConnId, pxWriteParam->ulTransId );
        }
    }
}

/*-----------------------------------------------------------*/

void vEnableNotification( BLEAttributeEvent_t * pxEventParam )
{
    BLEWriteEventParams_t * pxWriteParam;
    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp;
    uint16_t ucCCFGValue;


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
            ucCCFGValue = ( pxWriteParam->pucValue[ 1 ] << 8 ) | pxWriteParam->pucValue[ 0 ];

            if( ucCCFGValue == ( uint16_t ) ENABLE_NOTIFICATION )
            {
                AwsIotLogInfo( "Enabled Notification for Read Characteristic\n" );
                xNotifyCounterUpdate = pdTRUE;
                usBLEConnectionID = pxWriteParam->usConnId;
            }
            else if( ucCCFGValue == 0 )
            {
                xNotifyCounterUpdate = pdFALSE;
            }

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
}

/*-----------------------------------------------------------*/

static void vConnectionCallback( BTStatus_t xStatus,
                                 uint16_t usConnId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr )
{
    if( ( xStatus == eBTStatusSuccess ) && ( bConnected == false ) )
    {
        if( usConnId == usBLEConnectionID )
        {
            AwsIotLogInfo( " Disconnected from BLE device. Stopping the counter update \n");
            vGattDemoSvcStop();
        }
    }
}
