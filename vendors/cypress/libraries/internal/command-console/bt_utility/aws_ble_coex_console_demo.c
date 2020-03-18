/*
 * Amazon FreeRTOS V201910.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 /*
 * Copyright 2018, Cypress Semiconductor Corporation or a subsidiary of Cypress Semiconductor
 *  Corporation. All rights reserved. This software, including source code, documentation and  related
 * materials ("Software"), is owned by Cypress Semiconductor  Corporation or one of its
 *  subsidiaries ("Cypress") and is protected by and subject to worldwide patent protection
 * (United States and foreign), United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software ("EULA"). If no EULA applies, Cypress
 * hereby grants you a personal, nonexclusive, non-transferable license to  copy, modify, and
 * compile the Software source code solely for use in connection with Cypress's  integrated circuit
 * products. Any reproduction, modification, translation, compilation,  or representation of this
 * Software except as specified above is prohibited without the express written permission of
 * Cypress. Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO  WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING,  BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE. Cypress reserves the right to make changes to
 * the Software without notice. Cypress does not assume any liability arising out of the application
 * or use of the Software or any product or circuit  described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or failure of the
 * Cypress product may reasonably be expected to result  in significant property damage, injury
 * or death ("High Risk Product"). By including Cypress's product in a High Risk Product, the
 *  manufacturer of such system or application assumes  all risk of such use and in doing so agrees
 * to indemnify Cypress against all liability.
 */
 /** @file
 *
 *
 * During initialization the app registers with LE stack to receive various
 * notifications including bonding complete, connection status change and
 * peer write.  When device is successfully bonded, application saves
 * peer's Bluetooth Device address to the NVRAM.  Bonded device can also
 * write in to client configuration descriptor of the notification
 * characteristic.  That is also saved in the NVRAM.  When user pushes the
 * button, notification/indication is sent to the bonded and registered host.
 * User can also write to the characteristic configuration value. Number
 * of LED blinks indicates the value written
 *
 *
 * Features demonstrated
 *  - GATT database and Device configuration initialization
 *  - Registration with LE stack for various events
 *  - NVRAM write operation
 *  - Processing write requests from the client
 *  - Get the server(Rx) throughput.
 *
 * To demonstrate the app, work through the following steps.
 * 1. Plug the PSoC board into your computer
 * 2. Build (ninja -C ./build)and download the application(through the cypress programmer) 
 * 3. Pair with client.
 * 4. On the client side register for notifications
 * 6. Open the console and give the commands to see the throughput 
 *  a).ble_on 
 *  b).ble_coex_get_throughput
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_ble_gatt_server_demo.c
 * @brief Sample demo for a BLE GATT server
 */
#include "FreeRTOSConfig.h"
#include "iot_demo_logging.h"
#include "iot_ble_config.h"
#include "aws_ble_coex_console_demo.h"
#include "iot_ble.h"
#include "task.h"
#include "semphr.h"
#include "platform/iot_network.h"
#include "timers.h"

static const uint32_t millis_to_tick_ratio = (uint32_t)( 1000 / configTICK_RATE_HZ );
#define _get_current_time_ms (xTaskGetTickCount()*millis_to_tick_ratio)
static uint32_t rx_count;
uint32_t time_at_rx_start = 0;
uint32_t time_at_last_rx = 0;

static BaseType_t vCoexConsoleDemoSvcHook( void );

#define xServiceUUID_TYPE             \
    {                                 \
        .uu.uu128 = CoexConsoleDemoSVC_UUID, \
        .ucType = eBTuuidType128      \
    }
#define xCharCounterUUID_TYPE                  \
    {                                          \
        .uu.uu128 = CoexConsoleCHAR_UUID_MASK, \
        .ucType = eBTuuidType128               \
    }
#define xClientCharCfgUUID_TYPE                  \
    {                                            \
        .uu.uu16 = CoexConsoleDemoCLIENT_CHAR_CFG_UUID, \
        .ucType = eBTuuidType16                  \
    }

static uint16_t usHandlesBuffer[ eCoexConsoleDemoNbAttributes ];


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
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
            //.xProperties  = ( eBTPropRead | eBTPropNotify | eBTPropWriteNoResponse | eBTPropWrite | eBTPropSignedWrite)
            .xProperties  = ( eBTPropRead | eBTPropNotify | eBTPropWriteNoResponse )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = xClientCharCfgUUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
            //.xPermissions = ( eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm | eBTPermReadEncrypted | eBTPermWriteEncrypted |eBTPermRead | eBTPermWrite )
        }
    }
};

static const BTService_t xCoexConsoleDemoService =
{
    .xNumberOfAttributes = eCoexConsoleDemoNbAttributes,
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .pusHandlesBuffer    = usHandlesBuffer,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTable
};

/**
 * @brief Task used to update the counter periodically.
 */
//TaskHandle_t xCounterUpdateTask = NULL;

/**
 * @brief Counter value.
 */
//uint32_t ulCounter = 0;

/**
 * @brief Interval in Milliseconds to update the counter.
 */
//TickType_t xCounterUpdateInterval = pdMS_TO_TICKS( 1000 );

/**
 * @brief Should send the counter update as a notification.
 */
BaseType_t xNotifyUpdate = pdFALSE;

/**
 * @brief BLE connection ID to send the notification.
 */
uint16_t usBLEConnectionID;


#define CHAR_HANDLE( svc, ch_idx )    ( ( svc )->pusHandlesBuffer[ ch_idx ] )
#define CHAR_UUID( svc, ch_idx )      ( ( svc )->pxBLEAttributes[ ch_idx ].xCharacteristic.xUuid )

/**
 * @brief Callback to receive write request from a GATT client.
 *
 * Sets the event to start/stop/reset the counter update operation and sends without a response.
 *
 * @param pxAttribute
 * @param pEventParam
 */
void vDataWriteHandler( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback to enable notification when GATT client writes a value to the Client Characteristic
 * Configuration descriptor.
 *
 * @param pxAttribute  Attribute structure for the Client Characteristic Configuration Descriptor
 * @param pEventParam Write/Read event parametes
 */

void vEnableDataNotification( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Task used to update the counter value periodically
 *
 * Sends a notification of counter update to GATT client if it has enabled notification via Client Characteristic
 * Configuration Descriptor. Also receives events from GATT client and starts/stops/resets counter update.
 *
 * @param pvParams NULL
 */
//static void vCounterUpdateTaskFunction( void * pvParams );

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
static void _connectionCallback( BTStatus_t xStatus,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr );
/* ---------------------------------------------------------------------------------------*/

#if ( IOT_BLE_ADD_CUSTOM_SERVICES == 1 )
    void IotBle_AddCustomServicesCb( void )
    {
	vCoexConsoleDemoSvcHook();
    }
#endif

static const IotBleAttributeEventCallback_t pxCallBackArray[ eCoexConsoleDemoNbAttributes ] =
{
    NULL,
    vDataWriteHandler,
    vEnableDataNotification,
};

/*-----------------------------------------------------------*/
int vCoexConsoleDemoSvcInit( bool awsIotMqttMode,
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

static BaseType_t vCoexConsoleDemoSvcHook( void )
{
    BaseType_t xRet = pdFALSE;
    BTStatus_t xStatus;
    IotBleEventsCallbacks_t xCallback;

    /* Select the handle buffer. */
    xStatus = IotBle_CreateService( ( BTService_t * ) &xCoexConsoleDemoService, ( IotBleAttributeEventCallback_t * ) pxCallBackArray );

    if( xStatus == eBTStatusSuccess )
    {
        xRet = pdTRUE;
    }

    if( xRet == pdTRUE )
    {
        IotLogInfo( " vCoexConsoleDemoSvcHook  called ");
        xCallback.pConnectionCb = _connectionCallback;

        if( IotBle_RegisterEventCb( eBLEConnection, xCallback ) != eBTStatusSuccess )
        {
            xRet = pdFAIL;
        }
    }

    IotLogInfo( " IotBle_RegisterEventCb Status = %d ",xRet);

    return xRet;
}

/*-----------------------------------------------------------*/

void vDataWriteHandler( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;

    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    //IotLogInfo(" Data is being written ... ");

    if( pEventParam->xEventType == eBLEWriteNoResponse )
    {
    pxWriteParam = pEventParam->pParamWrite;
    usBLEConnectionID = pxWriteParam->connId;
        xResp.pAttrData->handle = pxWriteParam->attrHandle;

        xResp.pAttrData->pData = pxWriteParam->pValue;
        xResp.attrDataOffset = pxWriteParam->offset;
        xResp.pAttrData->size = pxWriteParam->length;

        if(rx_count == 0)
        {
        time_at_rx_start = _get_current_time_ms;
        }

        time_at_last_rx =  _get_current_time_ms;

        rx_count += pxWriteParam->length ; // total count of the received data.Check for the header info

        xResp.eventStatus = eBTStatusSuccess;

        //IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
        ( void ) IotBle_SendIndication( &xResp, usBLEConnectionID, false );
    }
}

void vCalculateServerThroughput( void )
{
    uint32_t rx_time;
    uint32_t server_throughput = 0;
    rx_time = (time_at_last_rx-time_at_rx_start)/1000; // time in seconds
    //IotLogInfo("total data written in bytes: %d bps", rx_count);

    if(rx_time != 0)
        {
        //IotLogInfo("total data written: %d bps", rx_count);
        //IotLogInfo("total time: %d sec", rx_time);
            // total data * 8 =>(bits)
            server_throughput = ((rx_count*8)/rx_time);
            IotLogInfo( "Server Throughput   : %d bps",server_throughput );
        }
}

void vEnableDataNotification( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp;
    uint16_t ucCCFGValue;


    xResp.pAttrData = &xAttrData;
    xResp.rspErrorStatus = eBTRspErrorNone;
    xResp.eventStatus = eBTStatusFail;
    xResp.attrDataOffset = 0;

    IotLogInfo(" vEnableDataNotification called ");

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pxWriteParam = pEventParam->pParamWrite;

        xResp.pAttrData->handle = pxWriteParam->attrHandle;

        if( pxWriteParam->length == 2 )
        {
            ucCCFGValue = ( pxWriteParam->pValue[ 1 ] << 8 ) | pxWriteParam->pValue[ 0 ];

            if( ucCCFGValue == ( uint16_t ) ENABLE_NOTIFICATION )
            {
                IotLogInfo( "Enabled Notification for Read Characteristic in side vEnableDataNotification " );
                xNotifyUpdate = pdTRUE;
                usBLEConnectionID = pxWriteParam->connId;
            }
            else if( ucCCFGValue == 0 )
            {
                xNotifyUpdate = pdFALSE;
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

/*-----------------------------------------------------------*/

static void _connectionCallback( BTStatus_t xStatus,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr )
{
    if( ( xStatus == eBTStatusSuccess ) && ( bConnected == false ) )
    {
        if( connId == usBLEConnectionID )
        {
            IotLogInfo( " Disconnected from BLE device." );
            rx_count = 0;
        }
    }
    else if (bConnected == true)
    {
        IotLogInfo("connected to BLE device.");
    }

}
