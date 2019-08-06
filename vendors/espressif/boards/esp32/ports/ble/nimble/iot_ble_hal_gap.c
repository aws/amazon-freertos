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
 * @file aws_ble_hal_gap.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */

#include <stddef.h>
#include <string.h>
#include "FreeRTOS.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"
#include "iot_ble_config.h"

BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;
static struct ble_gap_adv_params xAdv_params;
static bool xPrivacy;

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
                                       BTGattAdvertismentParams_t xAdvParams );
static BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t advParams );
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

uint8_t prvConvertPropertiesToESPIO( BTIOtypes_t xPropertyIO )
{
    uint8_t xIocap;

    switch( xPropertyIO )
    {
        case eBTIONone:
            xIocap = BLE_HS_IO_NO_INPUT_OUTPUT;
            break;

        case eBTIODisplayOnly:
            xIocap = BLE_HS_IO_DISPLAY_ONLY;
            break;

        case eBTIODisplayYesNo:
            xIocap = BLE_HS_IO_DISPLAY_YESNO;
            break;

        case eBTIOKeyboardOnly:
            xIocap = BLE_HS_IO_KEYBOARD_ONLY;
            break;

        case eBTIOKeyboardDisplay:
            xIocap = BLE_HS_IO_KEYBOARD_DISPLAY;
            break;

        default:
            xIocap = BLE_HS_IO_NO_INPUT_OUTPUT;
    }

    return xIocap;
}

BTStatus_t prvSetIOs( BTIOtypes_t xPropertyIO )
{
    uint8_t xIocap;
    BTStatus_t xStatus = eBTStatusSuccess;

    xIocap = prvConvertPropertiesToESPIO( xPropertyIO );
    ble_hs_cfg.sm_io_cap = xIocap;

    return xStatus;
}

BTStatus_t prvToggleBondableFlag( bool bEnable )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    ble_hs_cfg.sm_bonding = bEnable;
    return xStatus;
}

BTStatus_t prvToggleSecureConnectionOnlyMode( bool bEnable )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    ble_hs_cfg.sm_sc = bEnable;
    ble_hs_cfg.sm_mitm = 1;
    return xStatus;
}

BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;


    /* set default properties */
    xProperties.bBondable = true;
    xProperties.bSecureConnectionOnly = true;
    xProperties.xPropertyIO = eBTIODisplayYesNo;

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

    if( xBTBleAdapterCallbacks.pxRegisterBleAdapterCb != NULL )
    {
        xBTBleAdapterCallbacks.pxRegisterBleAdapterCb( eBTStatusSuccess, 0, pxAppUuid );
    }

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

    if( ble_gap_terminate( usConnId, BLE_ERR_CONN_TERM_LOCAL ) != 0 )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStartAdv( uint8_t ucAdapterIf )
{
    int xESPStatus;
    BTStatus_t xStatus = eBTStatusSuccess;
    uint8_t own_addr_type;

    /* Figure out address to use while advertising */
    xESPStatus = ble_hs_id_infer_auto( xPrivacy, &own_addr_type );

    if( xESPStatus != 0 )
    {
        xStatus = eBTStatusFail;
    }

    xESPStatus = ble_gap_adv_start( own_addr_type, NULL, BLE_HS_FOREVER,
                                    &xAdv_params, prvGAPeventHandler, NULL );

    if( xESPStatus != 0 )
    {
        xStatus = eBTStatusFail;
    }

    if( xBTBleAdapterCallbacks.pxAdvStatusCb != NULL )
    {
    	xBTBleAdapterCallbacks.pxAdvStatusCb( xStatus, ulGattServerIFhandle, true );
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStopAdv( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    int xESPStatus;

    xESPStatus = ble_gap_adv_stop();

    if( xESPStatus != 0 )
    {
        xStatus = eBTStatusFail;
    }

    if( xBTBleAdapterCallbacks.pxAdvStatusCb != NULL )
    {
    	xBTBleAdapterCallbacks.pxAdvStatusCb( xStatus, ulGattServerIFhandle, false );
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

/*-----------------------------------------------------------*/
BTStatus_t prvBTSetAdvData( uint8_t ucAdapterIf,
                            BTGattAdvertismentParams_t * pxParams,
                            uint16_t usManufacturerLen,
                            char * pcManufacturerData,
                            uint16_t usServiceDataLen,
                            char * pcServiceData,
                            BTUuid_t * pxServiceUuid,
                            size_t xNbServices )
{
    struct ble_hs_adv_fields fields;
    const char * name;
    int xESPStatus;
    ble_uuid16_t uuid16 = { 0 };
    ble_uuid32_t uuid32 = { 0 };
    ble_uuid128_t uuid128 = { 0 };

    BTStatus_t xStatus = eBTStatusSuccess;

    /**
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info).
     *     o Advertising tx power.
     *     o Device name.
     *     o 16-bit service UUIDs (alert notifications).
     */

    memset( &fields, 0, sizeof fields );

    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    if( pxParams->ulAppearance )
    {
        fields.appearance = pxParams->ulAppearance;
        fields.appearance_is_present = 1;
    }

    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assiging the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = pxParams->bIncludeTxPower;

    if( fields.tx_pwr_lvl_is_present )
    {
        fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    }

    if( pxParams->ucName.xType != BTGattAdvNameNone )
    {
        name = ble_svc_gap_device_name();
        fields.name = ( uint8_t * ) name;

        if( pxParams->ucName.xType == BTGattAdvNameShort )
        {
            fields.name_len = strlen( name );

            if( pxParams->ucName.ucShortNameLen < fields.name_len )
            {
                fields.name_len = pxParams->ucName.ucShortNameLen;
            }

            fields.name_is_complete = 0;
        }
        else
        {
            fields.name_len = strlen( name );
            fields.name_is_complete = 1;
        }
    }

    if( usManufacturerLen && pcManufacturerData )
    {
        fields.mfg_data = ( uint8_t * ) pcManufacturerData;
        fields.mfg_data_len = usManufacturerLen;
    }

    if( usServiceDataLen && pcServiceData )
    {
        fields.svc_data_uuid128 = (uint8_t *) pcServiceData;
        fields.svc_data_uuid128_len = usServiceDataLen;
    }

    if( pxServiceUuid != NULL )
    {
        if( pxServiceUuid->ucType == eBTuuidType16 )
        {
            uuid16.u.type = BLE_UUID_TYPE_16;
            uuid16.value = pxServiceUuid->uu.uu16;
            fields.uuids16 = &uuid16;
            fields.num_uuids16 = 1;
            fields.uuids16_is_complete = 1;
        }
        else if( pxServiceUuid->ucType == eBTuuidType32 )
        {
            uuid32.u.type = BLE_UUID_TYPE_32;
            uuid16.value = pxServiceUuid->uu.uu32;
            fields.uuids32 = &uuid32;
            fields.num_uuids32 = 1;
            fields.uuids32_is_complete = 1;
        }
        else if( pxServiceUuid->ucType == eBTuuidType128 )
        {
            uuid128.u.type = BLE_UUID_TYPE_128;
            memcpy( uuid128.value, pxServiceUuid->uu.uu128, sizeof( pxServiceUuid->uu.uu128 ) );
            fields.uuids128 = &uuid128;
            fields.num_uuids128 = 1;
            fields.uuids128_is_complete = 1;
        }
    }

    xAdv_params.itvl_min = ( pxParams->ulMinInterval * 1000 / BLE_HCI_ADV_ITVL );
    xAdv_params.itvl_max = ( pxParams->ulMaxInterval * 1000 / BLE_HCI_ADV_ITVL );

    if( pxParams->usAdvertisingEventProperties == BTAdvInd )
    {
        xAdv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
        xAdv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    }

    if( pxParams->usAdvertisingEventProperties == BTAdvDirectInd )
    {
        xAdv_params.conn_mode = BLE_GAP_CONN_MODE_DIR;
        /* fixme: set adv_params->high_duty_cycle accordingly */
    }

    if( pxParams->usAdvertisingEventProperties == BTAdvNonconnInd )
    {
        xAdv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
        xAdv_params.disc_mode = BLE_GAP_DISC_MODE_NON;
    }

    if( pxParams->xAddrType == BTAddrTypeResolvable )
    {
        xPrivacy = true;
    }

    if( pxParams->bSetScanRsp )
    {
        xESPStatus = ble_gap_adv_rsp_set_fields( &fields );
    }
    else
    {
        xESPStatus = ble_gap_adv_set_fields( &fields );
    }

    if( xESPStatus != 0 )
    {
        xStatus = eBTStatusFail;
    }

    if( xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL )
    {
        xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );
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
    BTStatus_t xStatus = eBTStatusSuccess;
    struct ble_gap_upd_params xParams =
    {
        .itvl_min            = BLE_GAP_INITIAL_CONN_ITVL_MIN,
        .itvl_max            = BLE_GAP_INITIAL_CONN_ITVL_MAX,
        .latency             = BLE_GAP_INITIAL_CONN_LATENCY,
        .supervision_timeout = BLE_GAP_INITIAL_SUPERVISION_TIMEOUT,
        .min_ce_len          = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len          = BLE_GAP_INITIAL_CONN_MAX_CE_LEN,
    };

    xParams.itvl_min = ulMinInterval;
    xParams.itvl_max = ulMaxInterval;
    xParams.latency = ulLatency;
    xParams.supervision_timeout = ulTimeout;

    if( ble_gap_update_params( usGattConnHandle, &xParams ) != 0 )
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
                                BTGattAdvertismentParams_t xAdvParams )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                BTGattAdvertismentParams_t advParams )
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
