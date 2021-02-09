/*
 * FreeRTOS V202012.00
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
 * @file iot_ble_gatt_server_demo.c
 * @brief Sample demo for a BLE GATT server
 */
#include "FreeRTOSConfig.h"
#include "iot_demo_logging.h"
#include "iot_ble_config.h"
#include "iot_ble_gatt_server_demo.h"
#include "iot_ble.h"
#include "task.h"
#include "semphr.h"
#include "platform/iot_network.h"

#define xServiceUUID_TYPE             \
    {                                 \
        .uu.uu128 = gattDemoSVC_UUID, \
        .ucType = eBTuuidType128      \
    }
#define xCharCounterUUID_TYPE                  \
    {                                          \
        .uu.uu128 = gattDemoCHAR_COUNTER_UUID, \
        .ucType = eBTuuidType128               \
    }
#define xCharControlUUID_TYPE                  \
    {                                          \
        .uu.uu128 = gattDemoCHAR_CONTROL_UUID, \
        .ucType = eBTuuidType128               \
    }
#define xClientCharCfgUUID_TYPE                  \
    {                                            \
        .uu.uu16 = gattDemoCLIENT_CHAR_CFG_UUID, \
        .ucType = eBTuuidType16                  \
    }

static uint16_t usHandlesBuffer[ egattDemoNbAttributes ];


static const BTAttribute_t pxAttributeTable[] =
{
    {
        .xServiceUUID = xServiceUUID_TYPE
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = xCharCounterUUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead | eBTPropNotify )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = xClientCharCfgUUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = xCharControlUUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    }
};

static const BTService_t xGattDemoService =
{
    .xNumberOfAttributes = egattDemoNbAttributes,
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .pusHandlesBuffer    = usHandlesBuffer,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTable
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


#define CHAR_HANDLE( svc, ch_idx )    ( ( svc )->pusHandlesBuffer[ ch_idx ] )
#define CHAR_UUID( svc, ch_idx )      ( ( svc )->pxBLEAttributes[ ch_idx ].xCharacteristic.xUuid )


/**
 * @brief Callback to read request from a GATT client on the Counter value characteristic.
 * Returns the current counter value.
 * @param[in] pxAttribute Attribute structure for the characteristic
 * @param pEventParam Event param for the read Request.
 */
static void prvReadCounter( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback to receive write request from a GATT client to set the counter status.
 *
 * Sets the event to start/stop/reset the counter update operation and sends back a response.
 *
 * @param pxAttribute
 * @param pEventParam
 */
static void prvWriteCommand( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback to enable notification when GATT client writes a value to the Client Characteristic
 * Configuration descriptor.
 *
 * @param pxAttribute  Attribute structure for the Client Characteristic Configuration Descriptor
 * @param pEventParam Write/Read event parametes
 */

static void prvEnableNotification( IotBleAttributeEvent_t * pEventParam );


/**
 * @brief Task used to update the counter value periodically
 *
 * Sends a notification of counter update to GATT client if it has enabled notification via Client Characteristic
 * Configuration Descriptor. Also receives events from GATT client and starts/stops/resets counter update.
 *
 * @param pvParams NULL
 */
static void prvCounterUpdateTaskFunction( void * pvParams );

/**
 * @brief Hook for an application's own custom services.
 *
 * @return pdTRUE on success creation of the custom services, pdFALSE otherwise.
 */
static BaseType_t prvGattDemoSvcHook( void );

/**
 * @brief Callback for BLE connect/disconnect.
 *
 * Stops the Counter Update task on disconnection.
 *
 * @param[in] xStatus Status indicating result of connect/disconnect operation
 * @param[in] connId Connection Id for the connection
 * @param[in] bConnected true if connection, false if disconnection
 * @param[in] pxRemoteBdAddr Remote address of the BLE device which connected or disconnected
 */
static void prvConnectionCallback( BTStatus_t xStatus,
                                   uint16_t connId,
                                   bool bConnected,
                                   BTBdaddr_t * pxRemoteBdAddr );


/**
 * @brief Stops the GATT demo service.
 */
static void prvGattDemoSvcStop( void );


/* ---------------------------------------------------------------------------------------*/

void IotBle_AddCustomServicesCb( void )
{
    prvGattDemoSvcHook();
}


static const IotBleAttributeEventCallback_t pxCallBackArray[ egattDemoNbAttributes ] =
{
    NULL,
    prvReadCounter,
    prvEnableNotification,
    prvWriteCommand
};

/*-----------------------------------------------------------*/

int vGattDemoSvcInit( bool iotMqttMode,
                      const char * pIdentifier,
                      void * pNetworkServerInfo,
                      void * pNetworkCredentialInfo,
                      const IotNetworkInterface_t * pNetworkInterface )
{
    int status = EXIT_SUCCESS;


    while( 1 )
    {
        vTaskDelay( 1000 );
    }

    return status;
}

/*-----------------------------------------------------------*/


static void prvCounterUpdateTaskFunction( void * pvParams )
{
    uint32_t xBitsToWait = EVENT_BIT( eGattDemoStart ) | EVENT_BIT( eGattDemoStop ) | EVENT_BIT( eGattDemoReset );
    BaseType_t xStarted = pdFALSE;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    uint32_t ulSetBits = 0;

    xAttrData.handle = CHAR_HANDLE( &xGattDemoService, egattDemoCharCounter );
    xAttrData.uuid = CHAR_UUID( &xGattDemoService, egattDemoCharCounter );
    xResp.pAttrData = &xAttrData;
    xResp.attrDataOffset = 0;
    xResp.eventStatus = eBTStatusSuccess;
    xResp.rspErrorStatus = eBTRspErrorNone;

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
                xAttrData.pData = ( uint8_t * ) &ulCounter;
                xAttrData.size = sizeof( ulCounter );
                ( void ) IotBle_SendIndication( &xResp, usBLEConnectionID, false );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvGattDemoSvcHook( void )
{
    BaseType_t xRet = pdFALSE;
    BTStatus_t xStatus;
    IotBleEventsCallbacks_t xCallback;

    /* Select the handle buffer. */
    xStatus = IotBle_CreateService( ( BTService_t * ) &xGattDemoService, ( IotBleAttributeEventCallback_t * ) pxCallBackArray );

    if( xStatus == eBTStatusSuccess )
    {
        xRet = pdTRUE;
    }

    if( xRet == pdTRUE )
    {
        xCallback.pConnectionCb = prvConnectionCallback;

        if( IotBle_RegisterEventCb( eBLEConnection, xCallback ) != eBTStatusSuccess )
        {
            xRet = pdFAIL;
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate(
            prvCounterUpdateTaskFunction,
            "GattDemoCounterTask",
            configMINIMAL_STACK_SIZE * 4,
            NULL,
            tskIDLE_PRIORITY,
            &xCounterUpdateTask );
    }

    return xRet;
}

/*-----------------------------------------------------------*/

static void prvConnectionCallback( BTStatus_t xStatus,
                                   uint16_t connId,
                                   bool bConnected,
                                   BTBdaddr_t * pxRemoteBdAddr )
{
    if( ( xStatus == eBTStatusSuccess ) && ( bConnected == false ) )
    {
        if( connId == usBLEConnectionID )
        {
            IotLogInfo( " Disconnected from BLE device. Stopping the counter update \n" );
            prvGattDemoSvcStop();
        }
    }
}

/*-----------------------------------------------------------*/

static void prvGattDemoSvcStop( void )
{
    ( void ) xTaskNotify( xCounterUpdateTask, EVENT_BIT( eGattDemoStop ), eSetBits );
}

/*-----------------------------------------------------------*/

static void prvReadCounter( IotBleAttributeEvent_t * pEventParam )
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
        xResp.pAttrData->handle = pxReadParam->attrHandle;
        xResp.pAttrData->pData = ( uint8_t * ) &ulCounter;
        xResp.pAttrData->size = sizeof( ulCounter );
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        IotBle_SendResponse( &xResp, pxReadParam->connId, pxReadParam->transId );
    }
}

/*-----------------------------------------------------------*/

static void prvWriteCommand( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    uint8_t ucEvent;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;
        xResp.pAttrData->handle = pxWriteParam->attrHandle;

        if( pxWriteParam->length == 1 )
        {
            ucEvent = pxWriteParam->pValue[ 0 ];
            xTaskNotify( xCounterUpdateTask, EVENT_BIT( ucEvent ), eSetBits );
            xResp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            xResp.pAttrData->pData = pxWriteParam->pValue;
            xResp.attrDataOffset = pxWriteParam->offset;
            xResp.pAttrData->size = pxWriteParam->length;
            IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        }
    }
}

/*-----------------------------------------------------------*/

static void prvEnableNotification( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    uint16_t ucCCFGValue;


    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;

        xResp.pAttrData->handle = pxWriteParam->attrHandle;

        if( pxWriteParam->length == 2 )
        {
            ucCCFGValue = ( pxWriteParam->pValue[ 1 ] << 8 ) | pxWriteParam->pValue[ 0 ];

            if( ucCCFGValue == ( uint16_t ) ENABLE_NOTIFICATION )
            {
                IotLogInfo( "Enabled Notification for Read Characteristic\n" );
                xNotifyCounterUpdate = pdTRUE;
                usBLEConnectionID = pxWriteParam->connId;
            }
            else if( ucCCFGValue == 0 )
            {
                xNotifyCounterUpdate = pdFALSE;
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
}
