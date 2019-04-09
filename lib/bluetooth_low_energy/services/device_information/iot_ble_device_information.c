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
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

#include <string.h>

#include "iot_ble_config.h"
#include "FreeRTOS.h"
#include "iot_ble_device_information.h"
#include "semphr.h"
#include "aws_clientcredential.h"
#include "task.h"

#define INT64_WIDTH                 ( 20 )
#define MAX_INTEGER_BUFFER_WIDTH    ( INT64_WIDTH + 1 )

/*------------------------------------------------------------------------------------*/
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );
static IotBleDeviceInfoService_t _service =
{
    .pBLEService = NULL,
    .BLEConnId   = 65535,
    .BLEMtu      = 0,
    .CCFGVal     = { 0 }
};

#define IOT_BLE_DEVICE_INFO_VERSION_UUID_TYPE         \
    {                                                 \
        .uu.uu128 = IOT_BLE_DEVICE_INFO_VERSION_UUID, \
        .ucType = eBTuuidType128                      \
    }
#define IOT_BLE_DEVICE_INFO_BROKER_ENDPOINT_UUID_TYPE         \
    {                                                         \
        .uu.uu128 = IOT_BLE_DEVICE_INFO_BROKER_ENDPOINT_UUID, \
        .ucType = eBTuuidType128                              \
    }
#define IOT_BLE_DEVICE_INFO_CLIENT_CHAR_CFG_UUID_TYPE        \
    {                                                        \
        .uu.uu16 = IOT_BLE_DEVICE_INFO_CLIENT_CHAR_CFG_UUID, \
        .ucType = eBTuuidType16                              \
    }
#define IOT_BLE_DEVICE_INFO_CHAR_MTU_UUID_TYPE         \
    {                                                  \
        .uu.uu128 = IOT_BLE_DEVICE_INFO_CHAR_MTU_UUID, \
        .ucType = eBTuuidType128                       \
    }

/**
 * @brief UUID for Device Information Service.
 *
 * This UUID is used in advertisement for the companion apps to discover and connect to the device.
 */
#define xDeviceInfoSvcUUID                            \
    {                                                 \
        .uu.uu128 = IOT_BLE_DEVICE_INFO_SERVICE_UUID, \
        .ucType = eBTuuidType128                      \
    }


typedef enum
{
    _ATTR_SERVICE,
    _ATTR_CHAR_END_POINT,
    _ATTR_CHAR_VERSION,
    _ATTR_CHAR_MTU,
    _ATTR_CHAR_DESCR_MTU,
    _ATTR_NUMBER
} _attr_t;

static uint16_t _handlesBuffer[ _ATTR_NUMBER ];

static const BTAttribute_t _pAttributeTable[] =
{
    {
        .xServiceUUID = xDeviceInfoSvcUUID
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = IOT_BLE_DEVICE_INFO_VERSION_UUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = IOT_BLE_DEVICE_INFO_BROKER_ENDPOINT_UUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = IOT_BLE_DEVICE_INFO_CHAR_MTU_UUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead | eBTPropNotify )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = IOT_BLE_DEVICE_INFO_CLIENT_CHAR_CFG_UUID_TYPE,
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
        }
    }
};

static const BTService_t _deviceInformationService =
{
    .xNumberOfAttributes = _ATTR_NUMBER,
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .pusHandlesBuffer    = _handlesBuffer,
    .pxBLEAttributes     = ( BTAttribute_t * ) _pAttributeTable
};


/**
 * @brief Client Characteristic Configuration descriptor callback for MTU characteristic
 *
 * Enables notification to send MTU changed information to the GATT client
 *
 * @param[in] pEventParam Write/Read request param to the attribute
 */
static void _deviceInfoCCFGCallback( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback invoked when GATT client reads MTU characteristic
 *
 * Returns the MTU value in JSON payload as response.
 *
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
static void _deviceInfoMTUCharCallback( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback invoked when GATT client reads Broker Endpoint Characteristic
 *
 * Returns the IOT Broker Endpoint provisioned for the device in a JSON payload as response.
 *
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
static void _deviceInfoBrokerEndpointCharCallback( IotBleAttributeEvent_t * pEventParam );


/**
 * @brief Callback invoked when GATT client reads the encoding Characteristic
 * Returns the version for the device in a JSON payload as response.
 *
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
static void _deviceInfoVersionCharCallback( IotBleAttributeEvent_t * pEventParam );

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
static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu );

/**
 * @brief Callback invoked on a BLE connect and disconnect event
 *
 * Stores the connection ID of the BLE client for sending notifications.
 *
 * @param status BLE status of the operation
 * @param connId Connection ID for the connection
 * @param bConnected Is Connected or disconnected
 * @param pxRemoteBdAddr Remote address of the client
 */
static void _connectionCallback( BTStatus_t status,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr );


static const IotBleAttributeEventCallback_t pxCallBackArray[ _ATTR_NUMBER ] =
{
    NULL,
    _deviceInfoVersionCharCallback,
    _deviceInfoBrokerEndpointCharCallback,
    _deviceInfoMTUCharCallback,
    _deviceInfoCCFGCallback
};


/*-----------------------------------------------------------*/

bool IotBleDeviceInfo_Init( void )
{
    BTStatus_t status = eBTStatusFail;
    bool error = false;
    IotBleEventsCallbacks_t callback;

    /* Select the handle buffer. */
    status = IotBle_CreateService( ( BTService_t * ) &_deviceInformationService, ( IotBleAttributeEventCallback_t * ) pxCallBackArray );

    if( status == eBTStatusSuccess )
    {
        callback.pMtuChangedCb = _MTUChangedCallback;
        status = IotBle_RegisterEventCb( eBLEMtuChanged, callback );
    }

    if( status == eBTStatusSuccess )
    {
        callback.pConnectionCb = _connectionCallback;
        status = IotBle_RegisterEventCb( eBLEConnection, callback );
    }

    if( status == eBTStatusSuccess )
    {
        error = true;
    }

    return error;
}

/*-----------------------------------------------------------*/

void _deviceInfoCCFGCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;

    resp.pAttrData = &attrData;
    resp.rspErrorStatus = eBTRspErrorNone;
    resp.eventStatus = eBTStatusFail;
    resp.attrDataOffset = 0;

    if( ( pEventParam->xEventType == eBLEWrite ) || ( pEventParam->xEventType == eBLEWriteNoResponse ) )
    {
        pWriteParam = pEventParam->pParamWrite;
        attrData.handle = pWriteParam->attrHandle;

        if( pWriteParam->length == 2 )
        {
            _service.CCFGVal[ IOT_BLE_DEVICE_INFO_MTU_CHAR_DESCR ] = ( pWriteParam->pValue[ 1 ] << 8 ) | pWriteParam->pValue[ 0 ];
            resp.eventStatus = eBTStatusSuccess;
        }

        if( pEventParam->xEventType == eBLEWrite )
        {
            attrData.pData = pWriteParam->pValue;
            attrData.size = pWriteParam->length;
            resp.attrDataOffset = pWriteParam->offset;
            IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
        }
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        attrData.handle = pEventParam->pParamRead->attrHandle;
        attrData.pData = ( uint8_t * ) &_service.CCFGVal[ IOT_BLE_DEVICE_INFO_MTU_CHAR_DESCR ];
        attrData.size = 2;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;

        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

void _deviceInfoBrokerEndpointCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;



    if( pEventParam->xEventType == eBLERead )
    {
        attrData.handle = pEventParam->pParamRead->attrHandle;
        attrData.pData = ( uint8_t * ) clientcredentialMQTT_BROKER_ENDPOINT;
        attrData.size = strlen( clientcredentialMQTT_BROKER_ENDPOINT );

        resp.pAttrData = &attrData;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
        resp.rspErrorStatus = eBTRspErrorNone;

        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

void _deviceInfoVersionCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;


    if( pEventParam->xEventType == eBLERead )
    {
        attrData.handle = pEventParam->pParamRead->attrHandle;
        attrData.pData = ( uint8_t * ) tskKERNEL_VERSION_NUMBER;
        attrData.size = strlen( tskKERNEL_VERSION_NUMBER );

        resp.pAttrData = &attrData;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
        resp.rspErrorStatus = eBTRspErrorNone;


        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

void _deviceInfoMTUCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp;
    char pMessage[ MAX_INTEGER_BUFFER_WIDTH ] = { 0 };
    size_t pMessageLen;

    if( pEventParam->xEventType == eBLERead )
    {
        pMessageLen = snprintf( pMessage, MAX_INTEGER_BUFFER_WIDTH, "%d", _service.BLEMtu );

        attrData.handle = pEventParam->pParamRead->attrHandle;
        attrData.pData = ( uint8_t * ) pMessage;
        attrData.size = pMessageLen;

        resp.pAttrData = &attrData;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
        resp.rspErrorStatus = eBTRspErrorNone;
        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/

static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp = { 0 };
    char pMessage[ MAX_INTEGER_BUFFER_WIDTH ] = { 0 };
    size_t pMessageLen;


    if( usMtu != _service.BLEMtu )
    {
        _service.BLEMtu = usMtu;
        pMessageLen = snprintf( pMessage, MAX_INTEGER_BUFFER_WIDTH, "%d", _service.BLEMtu );

        attrData.handle = _deviceInformationService.pusHandlesBuffer[ _ATTR_CHAR_MTU ];
        attrData.uuid = _deviceInformationService.pxBLEAttributes[ _ATTR_CHAR_MTU ].xCharacteristic.xUuid;
        attrData.pData = ( uint8_t * ) pMessage;
        attrData.size = pMessageLen;

        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
        resp.pAttrData = &attrData;
        resp.rspErrorStatus = eBTRspErrorNone;

        ( void ) IotBle_SendIndication( &resp, _service.BLEConnId, false );
    }
}

static void _connectionCallback( BTStatus_t status,
                                 uint16_t connId,
                                 bool bConnected,
                                 BTBdaddr_t * pxRemoteBdAddr )
{
    if( status == eBTStatusSuccess )
    {
        if( bConnected == true )
        {
            _service.BLEConnId = connId;
        }
    }
}
