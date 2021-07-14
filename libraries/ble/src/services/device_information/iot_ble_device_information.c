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
 * @file aws_device_information.c
 * @brief BLE GATT service to expose FreeRTOS device information.
 */

#include <string.h>

/* The config header is always included first. */
#include "iot_config.h"
#include "iot_ble_config.h"
#include "iot_ble_device_information.h"
#include "iot_ble.h"


#define INT64_WIDTH                                 ( 20 )
#define MAX_INTEGER_BUFFER_WIDTH                    ( INT64_WIDTH + 1 )

/**
 * @brief GATT Service, characteristic and descriptor UUIDs used by the Device information service.
 *
 */
#define IOT_BLE_DEVICE_INFO_VERSION_UUID            { 0x01, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief Firmware version. */
#define IOT_BLE_DEVICE_INFO_CLOUD_ENDPOINT_UUID     { 0x02, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief Broker endpoint. */
#define IOT_BLE_DEVICE_INFO_CHAR_MTU_UUID           { 0x03, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief MTU size. */
#define IOT_BLE_DEVICE_INFO_PLATFORM_NAME_UUID      { 0x04, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief Device platform name. */
#define IOT_BLE_DEVICE_INFO_DEVICE_ID_UUID          { 0x05, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief Device unique identifier. */
#define IOT_BLE_DEVICE_INFO_CLIENT_CHAR_CFG_UUID    0x2902                                                  /**< @brief Client configuration. */

/*-------------------------------------------------------------------------------------*/

/**
 * @brief Structure used for Device Information Service
 */
typedef struct IotBleDeviceInfoService
{
    BTService_t * pBLEService; /**< A pointer to the GATT service for Device Information. */
    uint16_t CCFGVal;          /**< The configuration descriptor. */
    uint16_t BLEConnId;        /**< The connection ID. */
    uint16_t BLEMtu;           /**< The MTU size. */
} IotBleDeviceInfoService_t;


/*------------------------------------------------------------------------------------*/
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );

static IotBleDeviceInfoService_t _service =
{
    .pBLEService = NULL,
    .BLEConnId   = 65535,
    .BLEMtu      = IOT_BLE_PREFERRED_MTU_SIZE,
    .CCFGVal     = 0
};

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

typedef enum
{
    _ATTR_SERVICE,
    _ATTR_CHAR_VERSION,
    _ATTR_CHAR_CLOUD_END_POINT,
    _ATTR_CHAR_MTU,
    _ATTR_CHAR_DESCR_MTU,
    _ATTR_CHAR_PLATFROM_NAME,
    _ATTR_CHAR_DEVICE_ID,
    _ATTR_NUMBER
} _attr_t;

static uint16_t _handlesBuffer[ _ATTR_NUMBER ];

static const BTAttribute_t _pAttributeTable[] =
{
    {
        .xServiceUUID = _UUID128( IOT_BLE_DEVICE_INFO_SERVICE_UUID )
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = _UUID128( IOT_BLE_DEVICE_INFO_VERSION_UUID ),
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = _UUID128( IOT_BLE_DEVICE_INFO_CLOUD_ENDPOINT_UUID ),
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = _UUID128( IOT_BLE_DEVICE_INFO_CHAR_MTU_UUID ),
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead | eBTPropNotify )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = _UUID16( IOT_BLE_DEVICE_INFO_CLIENT_CHAR_CFG_UUID ),
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = _UUID128( IOT_BLE_DEVICE_INFO_PLATFORM_NAME_UUID ),
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = _UUID128( IOT_BLE_DEVICE_INFO_DEVICE_ID_UUID ),
            .xPermissions = ( IOT_BLE_CHAR_READ_PERM ),
            .xProperties  = ( eBTPropRead )
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
static void _deviceInfoCloudEndpointCharCallback( IotBleAttributeEvent_t * pEventParam );


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
 * @brief Callback invoked when GATT client reads Broker Endpoint Characteristic
 *
 * Returns the IOT Broker Endpoint provisioned for the device in a JSON payload as response.
 *
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
static void _deviceInfoPlatformNameCharCallback( IotBleAttributeEvent_t * pEventParam );

/**
 * @brief Callback used to retrieve the unique device identifier through device information service.
 * Returns a SHA-256 of the device certificate which is used to uniquely identify the device.
 *
 * @param[in] pEventParam Write/Read request param to the attribute
 *
 */
static void _deviceInfoDeviceIDCharCallback( IotBleAttributeEvent_t * pEventParam );

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

/**
 * @bried Sends reply for a device information read request from peer device.
 */
static void _sendDeviceInfoReply( IotBleAttributeEvent_t * pEventParam,
                                  const char * pData,
                                  size_t length );


static const IotBleAttributeEventCallback_t pxCallBackArray[ _ATTR_NUMBER ] =
{
    NULL,
    _deviceInfoVersionCharCallback,
    _deviceInfoCloudEndpointCharCallback,
    _deviceInfoMTUCharCallback,
    _deviceInfoCCFGCallback,
    _deviceInfoPlatformNameCharCallback,
    _deviceInfoDeviceIDCharCallback
};

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
            _service.CCFGVal = ( pWriteParam->pValue[ 1 ] << 8 ) | pWriteParam->pValue[ 0 ];
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
        attrData.pData = ( uint8_t * ) &_service.CCFGVal;
        attrData.size = 2;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;

        IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
    }
}

/*-----------------------------------------------------------*/


static void _sendDeviceInfoReply( IotBleAttributeEvent_t * pEventParam,
                                  const char * pData,
                                  size_t length )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .eventStatus    = eBTStatusSuccess,
        .attrDataOffset = 0,
        .pAttrData      = &attrData,
        .rspErrorStatus = eBTRspErrorNone
    };

    if( pEventParam->pParamRead->offset <= length )
    {
        attrData.pData = ( ( uint8_t * ) pData ) + pEventParam->pParamRead->offset;
        attrData.size = length - pEventParam->pParamRead->offset;
    }

    attrData.handle = pEventParam->pParamRead->attrHandle;
    IotBle_SendResponse( &resp, pEventParam->pParamRead->connId, pEventParam->pParamRead->transId );
}

/*-----------------------------------------------------------*/
static void _deviceInfoCloudEndpointCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    const char * pCloudEndpoint = IOT_CLOUD_ENDPOINT;
    size_t length = strlen( pCloudEndpoint );

    if( pEventParam->xEventType == eBLERead )
    {
        _sendDeviceInfoReply( pEventParam, pCloudEndpoint, length );
    }
}

/*-----------------------------------------------------------*/

void _deviceInfoVersionCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    const char * pDeviceVersion = IOT_SDK_VERSION;
    size_t length = strlen( pDeviceVersion );

    if( pEventParam->xEventType == eBLERead )
    {
        _sendDeviceInfoReply( pEventParam, pDeviceVersion, length );
    }
}

/*-----------------------------------------------------------*/

void _deviceInfoMTUCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    char mtuStr[ MAX_INTEGER_BUFFER_WIDTH ] = { 0 };
    size_t length;

    if( pEventParam->xEventType == eBLERead )
    {
        length = snprintf( mtuStr, MAX_INTEGER_BUFFER_WIDTH, "%d", _service.BLEMtu );
        _sendDeviceInfoReply( pEventParam, mtuStr, length );
    }
}

/*-----------------------------------------------------------*/

static void _deviceInfoPlatformNameCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    const char * pPlatformName = IOT_PLATFORM_NAME;
    size_t length = strlen( pPlatformName );

    if( pEventParam->xEventType == eBLERead )
    {
        _sendDeviceInfoReply( pEventParam, pPlatformName, length );
    }
}

/*-----------------------------------------------------------*/

static void _deviceInfoDeviceIDCharCallback( IotBleAttributeEvent_t * pEventParam )
{
    const char * pDeviceIdentifier = IOT_DEVICE_IDENTIFIER;
    size_t length = strlen( pDeviceIdentifier );

    if( pEventParam->xEventType == eBLERead )
    {
        _sendDeviceInfoReply( pEventParam, pDeviceIdentifier, length );
    }
}

/*-----------------------------------------------------------*/

static void _MTUChangedCallback( uint16_t connId,
                                 uint16_t usMtu )
{
    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp = { 0 };
    char mtuStr[ MAX_INTEGER_BUFFER_WIDTH ] = { 0 };
    size_t length;


    if( usMtu != _service.BLEMtu )
    {
        _service.BLEMtu = usMtu;
        length = snprintf( mtuStr, MAX_INTEGER_BUFFER_WIDTH, "%d", _service.BLEMtu );

        attrData.handle = _deviceInformationService.pusHandlesBuffer[ _ATTR_CHAR_MTU ];
        attrData.uuid = _deviceInformationService.pxBLEAttributes[ _ATTR_CHAR_MTU ].xCharacteristic.xUuid;
        attrData.pData = ( uint8_t * ) mtuStr;
        attrData.size = length;

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

bool IotBleDeviceInfo_Cleanup( void )
{
    BTStatus_t status = eBTStatusFail;
    IotBleEventsCallbacks_t callback;
    bool ret = true;

    callback.pConnectionCb = _connectionCallback;
    status = IotBle_UnRegisterEventCb( eBLEConnection, callback );

    if( status == eBTStatusSuccess )
    {
        callback.pMtuChangedCb = _MTUChangedCallback;
        status = IotBle_UnRegisterEventCb( eBLEMtuChanged, callback );
    }

    if( status == eBTStatusSuccess )
    {
        status = IotBle_DeleteService( ( BTService_t * ) &_deviceInformationService );
    }

    if( status != eBTStatusSuccess )
    {
        ret = false;
    }

    return ret;
}
