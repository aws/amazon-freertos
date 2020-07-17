/*
* FreeRTOS
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
 * @file iot_ble_hal_gap.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */
#include "iot_config.h"
#include <stddef.h>
#include <string.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"
#include "iot_ble_config.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_HAL" )
#include "iot_logging_setup.h"

BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;
static esp_ble_adv_params_t xAdv_params;

static BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks );
static BTStatus_t prvBTRegisterBleApp( BTUuid_t * pxAppUuid );
static BTStatus_t prvBTUnregisterBleApp( uint8_t ucAdapterIf );
static BTStatus_t prvBTGetBleAdapterProperty( BTBlePropertyType_t xType );
static BTStatus_t prvBTSetBleAdapterProperty( const BTBleProperty_t * pxProperty );
static BTStatus_t prvBTGetallBleRemoteDeviceProperties( BTBdaddr_t * pxRremoteAddr );
static BTStatus_t prvBTGetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                   BTBleProperty_t xType );
static BTStatus_t prvBTSetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                   const BTBleProperty_t * pxProperty );
static BTStatus_t prvBTScan( bool bStart );
static BTStatus_t prvBTConnect( uint8_t ucAdapterIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                uint32_t ulTransport );
static BTStatus_t prvBTDisconnect( uint8_t ucAdapterIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );
static BTStatus_t prvBTStartAdv( uint8_t ucAdapterIf );
static BTStatus_t prvBTStopAdv( uint8_t ucAdapterIf );
static BTStatus_t prvBTReadRemoteRssi( uint8_t ucAdapterIf,
                                       const BTBdaddr_t * pxBdAddr );
static BTStatus_t prvBTScanFilterParamSetup( BTGattFiltParamSetup_t xFiltParam );
static BTStatus_t prvBTScanFilterAddRemove( uint8_t ucAdapterIf,
                                            uint32_t ulAction,
                                            uint32_t ulFiltType,
                                            uint32_t ulFiltIndex,
                                            uint32_t ulCompanyId,
                                            uint32_t ulCompanyIdMask,
                                            const BTUuid_t * pxUuid,
                                            const BTUuid_t * pxUuidMask,
                                            const BTBdaddr_t * pxBdAddr,
                                            char cAddrType,
                                            size_t xDataLen,
                                            char * pcData,
                                            size_t xMaskLen,
                                            char * pcMask );
static BTStatus_t prvBTScanFilterEnable( uint8_t ucAdapterIf,
                                         bool bEnable );
static BTTransport_t prvBTGetDeviceType( const BTBdaddr_t * pxBdAddr );
static BTStatus_t prvBTSetAdvData( uint8_t ucAdapterIf,
                                   BTGattAdvertismentParams_t * pxParams,
                                   uint16_t usManufacturerLen,
                                   char * pcManufacturerData,
                                   uint16_t usServiceDataLen,
                                   char * pcServiceData,
                                   BTUuid_t * pxServiceUuid,
                                   size_t xNbServices );
static BTStatus_t prvBTSetAdvRawData( uint8_t ucAdapterIf,
                                      uint8_t * pucData,
                                      uint8_t ucLen );
static BTStatus_t prvBTConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                                   uint32_t ulMinInterval,
                                                   uint32_t ulMaxInterval,
                                                   uint32_t ulLatency,
                                                   uint32_t ulTimeout );
static BTStatus_t prvBTSetScanParameters( uint8_t ucAdapterIf,
                                          uint32_t ulScanInterval,
                                          uint32_t ulScanWindow );
static BTStatus_t prvBTMultiAdvEnable( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t * pxAdvParams );
static BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t * pxAdvParams );
static BTStatus_t prvBTMultiAdvSetInstData( uint8_t ucAdapterIf,
                                            bool bSetScanRsp,
                                            bool bIncludeName,
                                            bool bInclTxpower,
                                            uint32_t ulAppearance,
                                            size_t xManufacturerLen,
                                            char * pcManufacturerData,
                                            size_t xServiceDataLen,
                                            char * pcServiceData,
                                            BTUuid_t * pxServiceUuid,
                                            size_t xNbServices );
static BTStatus_t prvBTMultiAdvDisable( uint8_t ucAdapterIf );
static BTStatus_t prvBTBatchscanCfgStorage( uint8_t ucAdapterIf,
                                            uint32_t ulBatchScanFullMax,
                                            uint32_t ulBatchScanTruncMax,
                                            uint32_t ulBatchScanNotifyThreshold );
static BTStatus_t prvBTBatchscanEndBatchScan( uint8_t ucAdapterIf,
                                              uint32_t ulScanMode,
                                              uint32_t ulScanInterval,
                                              uint32_t ulScanWindow,
                                              uint32_t ulAddrType,
                                              uint32_t ulDiscardRule );
static BTStatus_t prvBTBatchscanDisBatchScan( uint8_t ucAdapterIf );
static BTStatus_t prvBTBatchscanReadReports( uint8_t ucAdapterIf,
                                             uint32_t ulScanMode );
static BTStatus_t prvBTSetPreferredPhy( uint16_t usConnId,
                                        uint8_t ucTxPhy,
                                        uint8_t ucRxPhy,
                                        uint16_t usPhyOptions );
static BTStatus_t prvBTReadPhy( uint16_t usConnId,
                                BTReadClientPhyCallback_t xCb );
static const void * prvBTGetGattClientInterface( void );
static BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks );

BTBleAdapter_t xBTLeAdapter =
{
    .pxBleAdapterInit                  = prvBTBleAdapterInit,
    .pxRegisterBleApp                  = prvBTRegisterBleApp,
    .pxUnregisterBleApp                = prvBTUnregisterBleApp,
    .pxGetBleAdapterProperty           = prvBTGetBleAdapterProperty,
    .pxSetBleAdapterProperty           = prvBTSetBleAdapterProperty,
    .pxGetallBleRemoteDeviceProperties = prvBTGetallBleRemoteDeviceProperties,
    .pxGetBleRemoteDeviceProperty      = prvBTGetBleRemoteDeviceProperty,
    .pxSetBleRemoteDeviceProperty      = prvBTSetBleRemoteDeviceProperty,
    .pxScan                            = prvBTScan,
    .pxConnect                         = prvBTConnect,
    .pxDisconnect                      = prvBTDisconnect,
    .pxStartAdv                        = prvBTStartAdv,
    .pxStopAdv                         = prvBTStopAdv,
    .pxReadRemoteRssi                  = prvBTReadRemoteRssi,
    .pxScanFilterParamSetup            = prvBTScanFilterParamSetup,
    .pxScanFilterAddRemove             = prvBTScanFilterAddRemove,
    .pxScanFilterEnable                = prvBTScanFilterEnable,
    .pxGetDeviceType                   = prvBTGetDeviceType,
    .pxSetAdvData                      = prvBTSetAdvData,
    .pxSetAdvRawData                   = prvBTSetAdvRawData,
    .pxConnParameterUpdateRequest      = prvBTConnParameterUpdateRequest,
    .pxSetScanParameters               = prvBTSetScanParameters,
    .pxMultiAdvEnable                  = prvBTMultiAdvEnable,
    .pxMultiAdvUpdate                  = prvBTMultiAdvUpdate,
    .pxMultiAdvSetInstData             = prvBTMultiAdvSetInstData,
    .pxMultiAdvDisable                 = prvBTMultiAdvDisable,
    .pxBatchscanCfgStorage             = prvBTBatchscanCfgStorage,
    .pxBatchscanEndBatchScan           = prvBTBatchscanEndBatchScan,
    .pxBatchscanDisBatchScan           = prvBTBatchscanDisBatchScan,
    .pxBatchscanReadReports            = prvBTBatchscanReadReports,
    .pxSetPreferredPhy                 = prvBTSetPreferredPhy,
    .pxReadPhy                         = prvBTReadPhy,
    .ppvGetGattClientInterface         = prvBTGetGattClientInterface,
    .ppvGetGattServerInterface         = prvBTGetGattServerInterface,
};

/*-----------------------------------------------------------*/
esp_ble_auth_req_t prvConvertPropertiesToESPAuth( bool bBondableFlag )
{
    esp_ble_auth_req_t xAuth = ESP_LE_AUTH_NO_BOND;

    /* Bondable is an optional flag. */
    if( bBondableFlag == true )
    {
        xAuth |= ESP_LE_AUTH_BOND;
    }

    /* The other flags are mandatory since only v4.2 stack is supported */
    xAuth |= ( ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY );

    return xAuth;
}

esp_ble_io_cap_t prvConvertPropertiesToESPIO( BTIOtypes_t xPropertyIO )
{
    esp_ble_io_cap_t xIocap;

    switch( xPropertyIO )
    {
        case eBTIONone:
            xIocap = ESP_IO_CAP_NONE;
            break;

        case eBTIODisplayOnly:
            xIocap = ESP_IO_CAP_OUT;
            break;

        case eBTIODisplayYesNo:
            xIocap = ESP_IO_CAP_IO;
            break;

        case eBTIOKeyboardOnly:
            xIocap = ESP_IO_CAP_IN;
            break;

        case eBTIOKeyboardDisplay:
            xIocap = ESP_IO_CAP_KBDISP;
            break;

        default:
            xIocap = ESP_IO_CAP_NONE;
    }

    return xIocap;
}

BTStatus_t prvSetIOs( BTIOtypes_t xPropertyIO )
{
    esp_ble_io_cap_t xIocap;
    esp_err_t xESPstatus;
    BTStatus_t xStatus = eBTStatusSuccess;

    xIocap = prvConvertPropertiesToESPIO( xPropertyIO );
    xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_IOCAP_MODE, &xIocap, sizeof( uint8_t ) );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

BTStatus_t prvToggleBondableFlag( bool bEnable )
{
    esp_ble_auth_req_t xAuthReq;
    esp_err_t xESPstatus;
    BTStatus_t xStatus = eBTStatusSuccess;

    xAuthReq = prvConvertPropertiesToESPAuth( bEnable );
    xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_AUTHEN_REQ_MODE, &xAuthReq, sizeof( uint8_t ) );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

BTStatus_t prvToggleSecureConnectionOnlyMode( bool bEnable )
{
    uint8_t xAuthOption;
    esp_err_t xESPstatus;
    BTStatus_t xStatus = eBTStatusSuccess;

    if( bEnable == true )
    {
        xAuthOption = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE;
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &xAuthOption, sizeof( uint8_t ) );

        /* Check the IO are compatible with the desired security level */
        if( ( xProperties.xPropertyIO != eBTIODisplayYesNo ) && ( xProperties.xPropertyIO != eBTIOKeyboardDisplay ) )
        {
            IotLogError( "BLE: Input/output property are incompatible with secure connection only Mode." );
        }
    }
    else
    {
        xAuthOption = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &xAuthOption, sizeof( uint8_t ) );
    }

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    esp_err_t xESPstatus = ESP_OK;
    esp_ble_io_cap_t xIocap;
    uint8_t xKeySize = 16;
    uint8_t xInitKey = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t xRspKey = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t xAuthOption;
    esp_ble_auth_req_t xAuthReq;

    /* set default properties */
    xProperties.bBondable = true;
    xProperties.bSecureConnectionOnly = true;
    xProperties.xPropertyIO = eBTIODisplayYesNo;

    if( xESPstatus == ESP_OK )
    {
        xESPstatus = esp_ble_gap_register_callback( prvGAPeventHandler );
    }

    /* Default initialization for ESP32 */
    if( xESPstatus == ESP_OK )
    {
        xAuthReq = prvConvertPropertiesToESPAuth( xProperties.bBondable );
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_AUTHEN_REQ_MODE, &xAuthReq, sizeof( uint8_t ) );

        if( xESPstatus == ESP_OK )
        {
            xAuthOption = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE;
            xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &xAuthOption, sizeof( uint8_t ) );
        }
    }

    if( xESPstatus == ESP_OK )
    {
        xIocap = prvConvertPropertiesToESPIO( xProperties.xPropertyIO );
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_IOCAP_MODE, &xIocap, sizeof( uint8_t ) );
    }

    if( xESPstatus == ESP_OK )
    {
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_MAX_KEY_SIZE, &xKeySize, sizeof( uint8_t ) );
    }

    if( xESPstatus == ESP_OK )
    {
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_MIN_KEY_SIZE, &xKeySize, sizeof( uint8_t ) );
    }

    if( xESPstatus == ESP_OK )
    {
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_SET_INIT_KEY, &xInitKey, sizeof( uint8_t ) );
    }

    if( xESPstatus == ESP_OK )
    {
        xESPstatus = esp_ble_gap_set_security_param( ESP_BLE_SM_SET_RSP_KEY, &xRspKey, sizeof( uint8_t ) );
    }

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    /*
     * Now set all the properties.
     */
    /* Set display IO property */
    if( xStatus == eBTStatusSuccess )
    {
        xStatus = prvSetIOs( xProperties.xPropertyIO );
    }

    /* Set bondable property */
    if( xStatus == eBTStatusSuccess )
    {
        xStatus = prvToggleBondableFlag( xProperties.bBondable );
    }

    /* Set connection only property */
    if( xStatus == eBTStatusSuccess )
    {
        xStatus = prvToggleSecureConnectionOnlyMode( xProperties.bSecureConnectionOnly );
    }

    if( pxCallbacks != NULL )
    {
        xBTBleAdapterCallbacks = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTRegisterBleApp( BTUuid_t * pxAppUuid )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xBTBleAdapterCallbacks.pxRegisterBleAdapterCb( eBTStatusSuccess, 0, pxAppUuid );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterBleApp( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetBleAdapterProperty( BTBlePropertyType_t xType )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetBleAdapterProperty( const BTBleProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTGetallBleRemoteDeviceProperties( BTBdaddr_t * pxRremoteAddr )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTGetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                            BTBleProperty_t xType )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTSetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                            const BTBleProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTScan( bool bStart )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTConnect( uint8_t ucAdapterIf,
                         const BTBdaddr_t * pxBdAddr,
                         bool bIsDirect,
                         uint32_t ulTransport )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTDisconnect( uint8_t ucAdapterIf,
                            const BTBdaddr_t * pxBdAddr,
                            uint16_t usConnId )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    esp_bd_addr_t xAddr = { 0 };

    memcpy( xAddr, pxBdAddr->ucAddress, ESP_BD_ADDR_LEN );

    if( esp_ble_gap_disconnect( xAddr ) != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStartAdv( uint8_t ucAdapterIf )
{
    esp_err_t xESPStatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;


    xESPStatus = esp_ble_gap_start_advertising( &xAdv_params );

    if( xESPStatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStopAdv( uint8_t ucAdapterIf )
{
    esp_err_t xESPStatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;


    xESPStatus = esp_ble_gap_stop_advertising();

    if( xESPStatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTReadRemoteRssi( uint8_t ucAdapterIf,
                                const BTBdaddr_t * pxBdAddr )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTScanFilterParamSetup( BTGattFiltParamSetup_t xFiltParam )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTScanFilterAddRemove( uint8_t ucAdapterIf,
                                     uint32_t ulAction,
                                     uint32_t ulFiltType,
                                     uint32_t ulFiltIndex,
                                     uint32_t ulCompanyId,
                                     uint32_t ulCompanyIdMask,
                                     const BTUuid_t * pxUuid,
                                     const BTUuid_t * pxUuidMask,
                                     const BTBdaddr_t * pxBdAddr,
                                     char cAddrType,
                                     size_t xDataLen,
                                     char * pcData,
                                     size_t xMaskLen,
                                     char * pcMask )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTScanFilterEnable( uint8_t ucAdapterIf,
                                  bool bEnable )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTTransport_t prvBTGetDeviceType( const BTBdaddr_t * pxBdAddr )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}

/* According to  https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/bluetooth/controller_vhci.html#_CPPv417esp_power_level_t */
int prvConvertPowerLevelToDb( esp_power_level_t powerLevel )
{
    int dbm;

    switch( powerLevel )
    {
        case ESP_PWR_LVL_N12:
            dbm = -12;
            break;

        case ESP_PWR_LVL_N9:
            dbm = -9;
            break;

        case ESP_PWR_LVL_N6:
            dbm = -6;
            break;

        case ESP_PWR_LVL_N3:
            dbm = -3;
            break;

        case ESP_PWR_LVL_N0:
            dbm = 0;
            break;

        case ESP_PWR_LVL_P3:
            dbm = 3;
            break;

        case ESP_PWR_LVL_P6:
            dbm = 6;
            break;

        case ESP_PWR_LVL_P9:
            dbm = 9;
            break;

        default:
            dbm = 0;
            break;
    }

    return dbm;
}

/**
 * @Brief fill up the pucAdvMsg array. Return eBTStatusSuccess in case of success.
 */
BTStatus_t prvAddToAdvertisementMessage( uint8_t * pucAdvMsg,
                                         uint8_t * pucIndex,
                                         esp_ble_adv_data_type xDType,
                                         uint8_t * pucData,
                                         const uint8_t ucDataLength )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( ( *pucIndex ) + ucDataLength + 1 < ESP_BLE_ADV_DATA_LEN_MAX )
    {
        pucAdvMsg[ ( *pucIndex )++ ] = ucDataLength + 1;
        pucAdvMsg[ ( *pucIndex )++ ] = xDType;
        memcpy( &pucAdvMsg[ *pucIndex ], pucData, ucDataLength );
        ( *pucIndex ) += ucDataLength;
    }
    else
    {
        IotLogError( "Advertising data can't fit in advertisement message." );
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

BTStatus_t prvBTSetAdvData( uint8_t ucAdapterIf,
                            BTGattAdvertismentParams_t * pxParams,
                            uint16_t usManufacturerLen,
                            char * pcManufacturerData,
                            uint16_t usServiceDataLen,
                            char * pcServiceData,
                            BTUuid_t * pxServiceUuid,
                            size_t xNbServices )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    uint8_t ucSlaveConnectInterval[ 4 ];
    uint8_t ucIndex, ucMessageIndex = 0;
    uint8_t ucMessageRaw[ ESP_BLE_ADV_DATA_LEN_MAX ];
    uint8_t ucFlags;
    uint8_t ucTxPower;
    esp_err_t xESPErr = ESP_OK;
    char *pDeviceName;
    size_t deviceNameLength;


    pDeviceName = prxESPGetBLEDeviceName();
    deviceNameLength = strlen( pDeviceName );

    if( ( pxParams->ucName.xType == BTGattAdvNameComplete ) || ( IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE >= deviceNameLength ) )
    {
        prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_NAME_CMPL, ( uint8_t * ) pDeviceName, deviceNameLength );
    }
    else if( pxParams->ucName.xType == BTGattAdvNameShort )
    {
        prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_NAME_SHORT, ( uint8_t * ) pDeviceName, IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE );
    }

    if( ( pxParams->bIncludeTxPower == true ) && ( xStatus == eBTStatusSuccess ) )
    {
        ucTxPower = prvConvertPowerLevelToDb( esp_ble_tx_power_get( ESP_BLE_PWR_TYPE_ADV ) );
        xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_TX_PWR, &ucTxPower, 1 );
    }

    if( ( xStatus == eBTStatusSuccess ) && ( pxParams->ulAppearance != 0 ) )
    {
        xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_APPEARANCE, ( uint8_t * ) &pxParams->ulAppearance, 4 );
    }

    if( ( xStatus == eBTStatusSuccess ) && ( pxParams->ulMinInterval != 0 ) && ( pxParams->ulMaxInterval != 0 ) )
    {
        ucSlaveConnectInterval[ 0 ] = ( pxParams->ulMinInterval ) & 0xFF;
        ucSlaveConnectInterval[ 1 ] = ( pxParams->ulMinInterval >> 8 ) & 0xFF;
        ucSlaveConnectInterval[ 2 ] = ( pxParams->ulMaxInterval ) & 0xFF;
        ucSlaveConnectInterval[ 3 ] = ( pxParams->ulMaxInterval >> 8 ) & 0xFF;
        xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_INT_RANGE, ucSlaveConnectInterval, 4 );
    }

    if( ( xStatus == eBTStatusSuccess ) && ( pxParams->bSetScanRsp == false ) )
    {
        ucFlags = ( ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT );
        xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_FLAG, &ucFlags, 1 );
    }

    if( ( pcManufacturerData != NULL ) && ( xStatus == eBTStatusSuccess ) )
    {
        xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE, ( uint8_t * ) pcManufacturerData, usManufacturerLen );
    }

    if( ( pcServiceData != NULL ) && ( xStatus == eBTStatusSuccess ) )
    {
        xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_SERVICE_DATA, ( uint8_t * ) pcServiceData, usServiceDataLen );
    }

    if( ( xNbServices != 0 ) && ( xStatus == eBTStatusSuccess ) )
    {
        for( ucIndex = 0; ucIndex < xNbServices; ucIndex++ )
        {
            switch( pxServiceUuid[ ucIndex ].ucType )
            {
                case eBTuuidType16:
                    xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_16SRV_PART, ( uint8_t * ) &pxServiceUuid[ ucIndex ].uu.uu16, 2 );
                    break;

                case eBTuuidType32:
                    xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_32SRV_PART, ( uint8_t * ) &pxServiceUuid[ ucIndex ].uu.uu32, 4 );
                    break;

                case eBTuuidType128:
                    xStatus = prvAddToAdvertisementMessage( ucMessageRaw, &ucMessageIndex, ESP_BLE_AD_TYPE_128SRV_PART, pxServiceUuid[ ucIndex ].uu.uu128, bt128BIT_UUID_LEN );
                    break;
            }

            if( xStatus != eBTStatusSuccess )
            {
                break;
            }
        }
    }

    if( xStatus == eBTStatusSuccess )
    {
        xAdv_params.channel_map = ADV_CHNL_ALL;
        xAdv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

        if( pxParams->usMinAdvInterval != 0 )
        {
            xAdv_params.adv_int_min = pxParams->usMinAdvInterval;
        }
        else
        {
            xAdv_params.adv_int_min = IOT_BLE_ADVERTISING_INTERVAL;
        }

        if( pxParams->usMaxAdvInterval != 0 )
        {
            xAdv_params.adv_int_max = pxParams->usMaxAdvInterval;
        }
        else
        {
            xAdv_params.adv_int_max = ( 2 * IOT_BLE_ADVERTISING_INTERVAL );
        }

        if( pxParams->usAdvertisingEventProperties == BTAdvInd )
        {
            xAdv_params.adv_type = ADV_TYPE_IND;
        }

        if( pxParams->usAdvertisingEventProperties == BTAdvNonconnInd )
        {
            xAdv_params.adv_type = ADV_TYPE_NONCONN_IND;
        }

        xAdv_params.own_addr_type = pxParams->xAddrType;

        if( pxParams->bSetScanRsp == true )
        {
            xESPErr = esp_ble_gap_config_scan_rsp_data_raw( ucMessageRaw, ucMessageIndex );

            if( xESPErr != ESP_OK )
            {
                IotLogError( "Failed to configure scan response." );
                xStatus = eBTStatusFail;
            }
        }
        else
        {
            xESPErr = esp_ble_gap_config_adv_data_raw( ucMessageRaw, ucMessageIndex );

            if( xESPErr != ESP_OK )
            {
                IotLogError( "Failed to configure Advertisement message." );
                xStatus = eBTStatusFail;
            }
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetAdvRawData( uint8_t ucAdapterIf,
                               uint8_t * pucData,
                               uint8_t ucLen )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                            uint32_t ulMinInterval,
                                            uint32_t ulMaxInterval,
                                            uint32_t ulLatency,
                                            uint32_t ulTimeout )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;
    esp_ble_conn_update_params_t xParams;

    xParams.latency = ulLatency;
    xParams.max_int = ulMaxInterval;
    xParams.min_int = ulMinInterval;
    xParams.timeout = ulTimeout;
    memcpy( xParams.bda, pxBdAddr->ucAddress, btADDRESS_LEN );

    xESPstatus = esp_ble_gap_update_conn_params( &xParams );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTSetScanParameters( uint8_t ucAdapterIf,
                                   uint32_t ulScanInterval,
                                   uint32_t ulScanWindow )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvEnable( uint8_t ucAdapterIf,
                                BTGattAdvertismentParams_t * pxAdvParams )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                BTGattAdvertismentParams_t * pxAdvParams )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvSetInstData( uint8_t ucAdapterIf,
                                     bool bSetScanRsp,
                                     bool bIncludeName,
                                     bool bInclTxpower,
                                     uint32_t ulAppearance,
                                     size_t xManufacturerLen,
                                     char * pcManufacturerData,
                                     size_t xServiceDataLen,
                                     char * pcServiceData,
                                     BTUuid_t * pxServiceUuid,
                                     size_t xNbServices )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvDisable( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTBatchscanCfgStorage( uint8_t ucAdapterIf,
                                     uint32_t ulBatchScanFullMax,
                                     uint32_t ulBatchScanTruncMax,
                                     uint32_t ulBatchScanNotifyThreshold )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTBatchscanEndBatchScan( uint8_t ucAdapterIf,
                                       uint32_t ulScanMode,
                                       uint32_t ulScanInterval,
                                       uint32_t ulScanWindow,
                                       uint32_t ulAddrType,
                                       uint32_t ulDiscardRule )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTBatchscanDisBatchScan( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTBatchscanReadReports( uint8_t ucAdapterIf,
                                      uint32_t ulScanMode )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetPreferredPhy( uint16_t usConnId,
                                 uint8_t ucTxPhy,
                                 uint8_t ucRxPhy,
                                 uint16_t usPhyOptions )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTReadPhy( uint16_t usConnId,
                         BTReadClientPhyCallback_t xCb )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}

/*-----------------------------------------------------------*/

const void * prvBTGetGattClientInterface()
{
    return NULL;
}

/*-----------------------------------------------------------*/

const void * prvGetLeAdapter()
{
    return &xBTLeAdapter;
}
