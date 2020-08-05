/*
 * $ Copyright Cypress Semiconductor $
 */

/**
 * @file bt_hal_manager_adapter_ble.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */

#include <stddef.h>
#include <string.h>
#include "FreeRTOS.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "bt_hal_internal.h"

// #include "aws_ble_event_manager.h"

#include "wiced_bt_ble.h"
#include "wiced_bt_cfg.h"
#include "wiced_bt_dynamic_gattdb.h"

#include "platform_peripheral.h"

extern uint16_t wiced_bt_get_conn_id_from_peer_addr(const uint8_t* pucBdAddr);
extern uint8_t* wiced_bt_get_peer_addr_from_conn_id(const uint16_t usConnId);

extern BOOLEAN wiced_bt_l2cap_update_ble_conn_params (BD_ADDR rem_bda, uint16_t min_int, uint16_t max_int, uint16_t latency, uint16_t timeout);
extern const void * prvBTGetGattServerInterface();

uint8_t BTM_BleSetStaticRandomAddr(BD_ADDR rs_addr);

extern wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern platform_bt_dev_property_interface_t *pxPlatformDevPropInterface;

BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;
uint8_t ucAdvMode;
BOOLEAN bMultAdvDataSet[3];
BOOLEAN bMultiAdvParamsUpdated[3];
BTGattAdvertismentParams_t gxAdvParams[3];

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
                                BTTransport_t ulTransport );
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
                                       BTGattAdvertismentParams_t* xAdvParams );
static BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t* advParams );
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
static BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks );

extern WICEDPropCache_t xWICEDPropCache;

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
    .ppvGetGattClientInterface         = NULL,
    .ppvGetGattServerInterface         = prvBTGetGattServerInterface,
};

/*-----------------------------------------------------------*/

BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

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
    static uint8_t ucAdapterIf = 1;

    if( ucAdapterIf > 3)
    {
        if ( xBTBleAdapterCallbacks.pxRegisterBleAdapterCb )
            xBTBleAdapterCallbacks.pxRegisterBleAdapterCb( eBTStatusFail, 0xFF, pxAppUuid );
    }
    else
    {
        if ( xBTBleAdapterCallbacks.pxRegisterBleAdapterCb )
            xBTBleAdapterCallbacks.pxRegisterBleAdapterCb( eBTStatusSuccess, ucAdapterIf++, pxAppUuid );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterBleApp( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    wiced_bt_gatt_db_init(NULL, 0);
    wiced_bt_gatt_db_reset();

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
                         BTTransport_t ulTransport )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTDisconnect( uint8_t ucAdapterIf,
                            const BTBdaddr_t * pxBdAddr,
                            uint16_t usConnId )
{
    wiced_bt_gatt_status_t xWICEDStatus = WICED_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;

    xWICEDStatus = wiced_bt_gatt_disconnect(usConnId);
    xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStartAdv( uint8_t ucAdapterIf )
{
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;

    if(BTAdvNonconnInd == ucAdvMode)
    {
        ucAdvMode = BTM_BLE_ADVERT_NONCONN_LOW;
    }
    else if(BTAdvDirectInd == ucAdvMode)
    {
        ucAdvMode = BTM_BLE_ADVERT_DIRECTED_LOW;
    }
    else
    {
        ucAdvMode = BTM_BLE_ADVERT_UNDIRECTED_LOW;
    }

    xWICEDStatus = wiced_bt_start_advertisements(ucAdvMode, 0, NULL);
    xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);
    xWICEDPropCache.ucAdapterIfCache = ucAdapterIf;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStopAdv( uint8_t ucAdapterIf )
{
	wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;

    xWICEDStatus = wiced_bt_start_advertisements(BTM_BLE_ADVERT_OFF, 0, NULL);
    xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);

    return xStatus;
}


/*-----------------------------------------------------------*/
static void prvReadRssiCb( wiced_bt_dev_rssi_result_t *p_rssi )
{
    if ( ( p_rssi->status == WICED_BT_SUCCESS ) && ( p_rssi->hci_status == HCI_SUCCESS ) )
    {
        if(xBTBleAdapterCallbacks.pxReadRemoteRssiCb)
        {
            xBTBleAdapterCallbacks.pxReadRemoteRssiCb(0, (BTBdaddr_t *)p_rssi->rem_bda, p_rssi->rssi, eBTStatusSuccess);
        }
    }
    else
    {
        if(xBTBleAdapterCallbacks.pxReadRemoteRssiCb)
        {
            xBTBleAdapterCallbacks.pxReadRemoteRssiCb(0, (BTBdaddr_t *)p_rssi->rem_bda, p_rssi->rssi, eBTStatusFail);
        }
    }
}

BTStatus_t prvBTReadRemoteRssi( uint8_t ucAdapterIf,
                                const BTBdaddr_t * pxBdAddr )
{
    wiced_result_t xWICEDStatus;
    wiced_bt_device_address_t xBdAddr;

    memcpy(xBdAddr, pxBdAddr->ucAddress, btADDRESS_LEN);

    xWICEDStatus = wiced_bt_dev_read_rssi( xBdAddr, BT_TRANSPORT_LE, (wiced_bt_dev_cmpl_cback_t *)prvReadRssiCb);

    return prvConvertWicedErrorToAfr(xWICEDStatus);
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
    uint8_t ucDevType = 0xFF;

    ucDevType = wiced_bt_dev_get_remote_type((uint8_t *)pxBdAddr->ucAddress);

    switch (ucDevType)
    {
        case BT_DEVICE_TYPE_BREDR:
            ucDevType = BTTransportBredr;
        break;

        case BT_DEVICE_TYPE_BLE:
            ucDevType = BTTransportLe;
        break;

        case BT_DEVICE_TYPE_DUMO:
            ucDevType = BTTransportAuto;
        break;

        default:
            ucDevType = 0xFF;
        break;

    }

    return ucDevType;
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
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;
    uint32_t uiAdvParamLen = 0, uiAdvPayloadLen = 0;
    #define BTM_BLE_AD_DATA_LEN 31

    uint8_t i = 0, ucAdvElemCnt = 1; // 1 for BTM_BLE_ADVERT_TYPE_FLAG
    wiced_bt_cfg_ble_advert_settings_t *p_adv_cfg = (wiced_bt_cfg_ble_advert_settings_t *) &wiced_bt_cfg_settings.ble_advert_cfg;
    wiced_bt_ble_advert_elem_t *pxAdvElem = NULL;
    uint8_t ble_advertisement_flag_value = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    uint16_t usAppearance = pxParams->ulAppearance;
    uint8_t *pcServiceDataCache = NULL, *pcManufacturerDataCache = NULL;

    // find the number of adv elements
    ( BTGattAdvNameNone != pxParams->ucName.xType ) ? ucAdvElemCnt++ : 0;
    ( pxParams->bIncludeTxPower ) ? ucAdvElemCnt++ : 0;
    ( pxParams->ulAppearance ) ? ucAdvElemCnt++ : 0;
    ( pxServiceUuid ) ? ( ucAdvElemCnt += xNbServices ) : 0;
    ( pcServiceData && usServiceDataLen ) ? ucAdvElemCnt++ : 0;
    ( pcManufacturerData && usManufacturerLen) ? ucAdvElemCnt++ : 0;

    pxAdvElem = (wiced_bt_ble_advert_elem_t*) pvPortMalloc( sizeof(wiced_bt_ble_advert_elem_t) * ucAdvElemCnt );
    if ( NULL == pxAdvElem )
    {
        return eBTStatusNoMem;
    }

    if ( BTGattAdvNameNone != pxParams->ucName.xType )
    {
        if(BTGattAdvNameShort == pxParams->ucName.xType)
        {
            pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_NAME_SHORT;
            pxAdvElem->len = pxParams->ucName.ucShortNameLen;
        }
        else
        {
            pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
            pxAdvElem->len = strlen((const char*)xWICEDPropCache.pucDevName);
        }

        pxAdvElem->p_data = xWICEDPropCache.pucDevName;
        pxAdvElem++;
        uiAdvParamLen += pxAdvElem->len;
    }

    if ( pxParams->bIncludeTxPower )
    {
        pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_TX_POWER;
        pxAdvElem->len = 1;
        pxAdvElem->p_data = &pxParams->ucTxPower;
        pxAdvElem++;
        uiAdvParamLen += pxAdvElem->len;
    }

    if ( pxParams->ulAppearance )
    {
        pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_APPEARANCE;
        pxAdvElem->len = 2;
        pxAdvElem->p_data = (uint8_t *) &usAppearance;
        pxAdvElem++;
        uiAdvParamLen += pxAdvElem->len;
    }

    if ( pxServiceUuid )
    {
        for ( i = 0; i < xNbServices; i++ )
        {
            if ( eBTuuidType16 == pxServiceUuid->ucType )
            {
                pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_16SRV_COMPLETE;
                pxAdvElem->len = 2;
                pxAdvElem->p_data = (uint8_t *) &pxServiceUuid->uu.uu16;
                uiAdvParamLen += pxAdvElem->len;
            }
            else if ( eBTuuidType32 == pxServiceUuid->ucType )
            {
                pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_32SRV_COMPLETE;
                pxAdvElem->len = 4;
                pxAdvElem->p_data = (uint8_t *) &pxServiceUuid->uu.uu32;
                uiAdvParamLen += pxAdvElem->len;
            }
            else if ( eBTuuidType128 == pxServiceUuid->ucType )
            {
                pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_128SRV_COMPLETE;
                pxAdvElem->len = 16;
                pxAdvElem->p_data = (uint8_t *) &pxServiceUuid->uu.uu128;
                uiAdvParamLen += pxAdvElem->len;
            }
            else
            {
                vPortFree( pxAdvElem );
                return eBTStatusParamInvalid;
            }

            pxAdvElem++;
        }
    }

    if ( ( 0 != usServiceDataLen ) && ( NULL != pcServiceData ) )
    {
        pcServiceDataCache = ( uint8_t *) pvPortMalloc(usServiceDataLen);
        if(NULL != pcServiceDataCache ) {
            memcpy( pcServiceDataCache, pcServiceData, usServiceDataLen );
            pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_SERVICE_DATA;
            pxAdvElem->len = usServiceDataLen;
            pxAdvElem->p_data = pcServiceDataCache;
            pxAdvElem++;
            uiAdvParamLen += pxAdvElem->len;
        }
    }

    if ( ( 0 != usManufacturerLen ) && ( NULL != pcManufacturerData ) )
    {
        pcManufacturerDataCache = ( uint8_t *) pvPortMalloc(usManufacturerLen);
        if(NULL != pcManufacturerDataCache ) {
            memcpy( pcManufacturerDataCache, pcManufacturerData, usManufacturerLen );
            pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_MANUFACTURER;
            pxAdvElem->len = usManufacturerLen;
            pxAdvElem->p_data = pcManufacturerDataCache;
            pxAdvElem++;
            uiAdvParamLen += pxAdvElem->len;
        }
    }

    pxAdvElem->advert_type = BTM_BLE_ADVERT_TYPE_FLAG;
    pxAdvElem->len = 1;
    pxAdvElem->p_data = &ble_advertisement_flag_value;
    pxAdvElem++;
    uiAdvParamLen += pxAdvElem->len;

    pxAdvElem -= ucAdvElemCnt;

    //Check if the length of elements exceeds the max. supported by ADV
    for(int i=0; i<ucAdvElemCnt; i++)
    {
      uiAdvPayloadLen += pxAdvElem[i].len+2;
    }

    if(uiAdvPayloadLen > (BTM_BLE_AD_DATA_LEN))
    {
      xStatus = eBTStatusFail;
    }
    else
    {
      if ( pxParams->bSetScanRsp )
        {
          xWICEDStatus = wiced_bt_ble_set_raw_scan_response_data( ucAdvElemCnt, pxAdvElem );
        }
      else
        {
            if(pxParams->ulMinInterval && pxParams->ulMaxInterval)
            {
                p_adv_cfg->low_duty_min_interval = pxParams->ulMinInterval;
                p_adv_cfg->low_duty_max_interval = pxParams->ulMaxInterval;
            }

            p_adv_cfg->low_duty_duration = pxParams->usTimeout;
            p_adv_cfg->channel_map = pxParams->ucChannelMap;

            xWICEDStatus = wiced_bt_ble_set_raw_advertisement_data( ucAdvElemCnt, pxAdvElem );
        }

        if ( xWICEDStatus != WICED_SUCCESS )
        {
            xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);
        }
        else if ( !pxParams->bSetScanRsp )
        {
            ucAdvMode = pxParams->usAdvertisingEventProperties;
            BD_ADDR bdAddress;

            memset(bdAddress, 0, sizeof(bdAddress));
            if(pxParams->xAddrType == BTAddrTypePublic)
            {
                /* If address value is all zero, then the BTM_BleSetStaticRandomAddr API configures the address as public */
                BTM_BleSetStaticRandomAddr(bdAddress);
            }
            else if(pxParams->xAddrType == BTAddrTypeRandom || pxParams->xAddrType == BTAddrTypeStaticRandom)
            {
                if(pxPlatformDevPropInterface != NULL && pxPlatformDevPropInterface->get_bd_address != NULL)
                {
                    if(WICED_SUCCESS == pxPlatformDevPropInterface->get_bd_address(&bdAddress[0], sizeof(bdAddress), PLATFORM_BD_ADDRESS_STATIC_RANDOM))
                    {
                        BTM_BleSetStaticRandomAddr(bdAddress);
                    }
                }
            }
        }
    }

    vPortFree( pxAdvElem );

    if(pcServiceDataCache)
      vPortFree( pcServiceDataCache );

    if(pcManufacturerDataCache)
      vPortFree( pcManufacturerDataCache );

    if ( xBTBleAdapterCallbacks.pxSetAdvDataCb )
        xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );

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

    if(!wiced_bt_l2cap_update_ble_conn_params( (uint8_t*) pxBdAddr, ulMinInterval, ulMaxInterval, ulLatency, ulTimeout ))
    {
        xStatus = eBTStatusFail;
    }

    if ( xBTBleAdapterCallbacks.pxConnParameterUpdateCb )
        xBTBleAdapterCallbacks.pxConnParameterUpdateCb( xStatus, pxBdAddr, ulMinInterval, ulMaxInterval,
                ulLatency, WICED_BT_CFG_DEFAULT_CONN_MIN_INTERVAL, ulTimeout );

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
                                BTGattAdvertismentParams_t *xAdvParams )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_result_t xWICEDStatus;
    wiced_bt_ble_multi_adv_params_t xMultiAdvparams;

    memset( &xMultiAdvparams, 0, sizeof(wiced_bt_ble_multi_adv_params_t));

    gxAdvParams[ucAdapterIf].ulMinInterval  = xMultiAdvparams.adv_int_min  = xAdvParams->ulMinInterval;
    gxAdvParams[ucAdapterIf].ulMaxInterval  = xMultiAdvparams.adv_int_max  = xAdvParams->ulMaxInterval;
    gxAdvParams[ucAdapterIf].ucChannelMap  = xMultiAdvparams.channel_map  = xAdvParams->ucChannelMap;
    gxAdvParams[ucAdapterIf].ucTxPower = xMultiAdvparams.adv_tx_power = xAdvParams->ucTxPower;

    if(BTAdvNonconnInd == xAdvParams->usAdvertisingEventProperties)
    {
        xMultiAdvparams.adv_type = MULTI_ADVERT_NONCONNECTABLE_EVENT;
    }
    else if(BTAdvDirectInd == xAdvParams->usAdvertisingEventProperties)
    {
        xMultiAdvparams.adv_type = MULTI_ADVERT_CONNECTABLE_DIRECT_EVENT;
    }
    else
    {
        xMultiAdvparams.adv_type = MULTI_ADVERT_CONNECTABLE_UNDIRECT_EVENT;
    }

    xMultiAdvparams.adv_filter_policy = 0;
    memset(xMultiAdvparams.peer_bd_addr, 0, 6);
    xMultiAdvparams.peer_addr_type = BLE_ADDR_PUBLIC;
    memset(xMultiAdvparams.own_bd_addr, 0, 6);
    xMultiAdvparams.own_addr_type = BLE_ADDR_PUBLIC;

    xWICEDStatus = wiced_set_multi_advertisement_params( ucAdapterIf, &xMultiAdvparams );
    if(WICED_BT_SUCCESS != xWICEDStatus)
    {
        return prvConvertWicedErrorToAfr(xWICEDStatus);
    }
    else
    {
        bMultiAdvParamsUpdated[ucAdapterIf] = true;
    }

    if(bMultAdvDataSet[ucAdapterIf])
    {
        xWICEDStatus = wiced_start_multi_advertisements( 1, ucAdapterIf );
        if(WICED_BT_SUCCESS != xWICEDStatus)
        {
            xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);
        }
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                BTGattAdvertismentParams_t *advParams )
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
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;
    #define BTM_BLE_AD_DATA_LEN 31

    uint8_t i = 0;
    uint8_t ble_advertisement_flag_value = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    uint8_t mult_adv_data[31];
    uint8_t index = 0;

    memset(mult_adv_data, 0, 31);

    if ( bIncludeName )
    {
        mult_adv_data[index++] = strlen((const char*)xWICEDPropCache.pucDevName)+1;
        mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
        memcpy(&mult_adv_data[index], xWICEDPropCache.pucDevName, strlen((const char*)xWICEDPropCache.pucDevName));
        index += strlen((const char*)xWICEDPropCache.pucDevName);
    }

    if ( bInclTxpower )
    {
        mult_adv_data[index++] = sizeof(uint8_t) + 1;
        mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_TX_POWER;
        mult_adv_data[index++] = gxAdvParams[ucAdapterIf].ucTxPower;
    }

    if ( ulAppearance )
    {
        mult_adv_data[index++] = sizeof(uint16_t) + 1;
        mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_APPEARANCE;
        mult_adv_data[index++] = (ulAppearance & 0xFF);
        mult_adv_data[index++] = (ulAppearance & 0xFF00) >> 8;
    }

    if ( pxServiceUuid )
    {
        for ( i = 0; i < xNbServices; i++ )
        {
            if ( eBTuuidType16 == pxServiceUuid->ucType )
            {
                mult_adv_data[index++] = 3;
                mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_16SRV_COMPLETE;
                mult_adv_data[index++] = (pxServiceUuid->uu.uu16 & 0xFF);
                mult_adv_data[index++] = (pxServiceUuid->uu.uu16 & 0xFF00) >> 8;
            }
            else if ( eBTuuidType32 == pxServiceUuid->ucType )
            {
                mult_adv_data[index++] = 5;
                mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_32SRV_COMPLETE;
                mult_adv_data[index++] = (pxServiceUuid->uu.uu32 & 0xFF);
                mult_adv_data[index++] = (pxServiceUuid->uu.uu32 & 0xFF00) >> 8;
                mult_adv_data[index++] = (pxServiceUuid->uu.uu32 & 0xFF0000) >> 16;
                mult_adv_data[index++] = (pxServiceUuid->uu.uu32 & 0xFF000000) >> 24;
            }
            else if ( eBTuuidType128 == pxServiceUuid->ucType )
            {
                mult_adv_data[index++] = 16+1;
                mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_128SRV_COMPLETE;
                memcpy(&mult_adv_data[index], pxServiceUuid->uu.uu128, 16);
                index += 16;
            }
            else
            {
                return eBTStatusParamInvalid;
            }
        }
    }

    if ( ( 0 != xServiceDataLen ) && ( NULL != pcServiceData ) )
    {
        mult_adv_data[index++] = xServiceDataLen+1;
        mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_SERVICE_DATA;

        for(int i=0; i<xServiceDataLen ; i++)
        {
            mult_adv_data[index+i] = pcServiceData[i];
        }

        index += xServiceDataLen;
    }

    if ( ( 0 != xManufacturerLen ) && ( NULL != pcManufacturerData ) )
    {
        mult_adv_data[index++] = xManufacturerLen+1;
        mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_MANUFACTURER;

        for(int i=0; i<xManufacturerLen ; i++)
        {
            mult_adv_data[index+i] = pcManufacturerData[i];
        }

        index += xManufacturerLen;
    }

    mult_adv_data[index++] = sizeof(uint8_t) + 1;
    mult_adv_data[index++] = BTM_BLE_ADVERT_TYPE_FLAG;
    mult_adv_data[index++] = ble_advertisement_flag_value;

    //Check if the length of elements exceeds the max. supported by ADV
    if(index > (BTM_BLE_AD_DATA_LEN))
    {
      xStatus = eBTStatusFail;
    }
    else
    {
      if ( bSetScanRsp )
          xWICEDStatus = wiced_set_multi_advertisement_scan_response_data( mult_adv_data, index, ucAdapterIf );
      else
          xWICEDStatus = wiced_set_multi_advertisement_data( mult_adv_data, index, ucAdapterIf );

        if ( xWICEDStatus != WICED_SUCCESS )
        {
            xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);
        }
        else
        {
            bMultAdvDataSet[ucAdapterIf] = true;

            /* Start advertisements after setting up Adv data.
             * This is an Amazon specific request to make out HAL work with their preexisting code */
            if ( !bSetScanRsp && bMultiAdvParamsUpdated[ucAdapterIf] )
            {
                /* Call prvBTStartAdv instead of wiced_bt_start_advertisements to trigger ADV callback */
                xStatus = prvBTMultiAdvEnable(ucAdapterIf, &gxAdvParams[ucAdapterIf]);
            }
        }
    }

    if ( xBTBleAdapterCallbacks.pxSetAdvDataCb )
        xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvDisable( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_result_t xWICEDStatus;

    xWICEDStatus = wiced_start_multi_advertisements( 0, ucAdapterIf );
    xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);

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
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_bt_ble_phy_preferences_t xphyPreferences;
    wiced_result_t xWICEDStatus;

    xphyPreferences.tx_phys = ucTxPhy;
    xphyPreferences.rx_phys = ucRxPhy;
    xphyPreferences.phy_opts = usPhyOptions;
    memcpy(xphyPreferences.remote_bd_addr, wiced_bt_get_peer_addr_from_conn_id(usConnId), sizeof(wiced_bt_device_address_t));

    xWICEDStatus = wiced_bt_ble_set_phy(&xphyPreferences);
    xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);

    return xStatus;
}

/*-----------------------------------------------------------*/
static BTReadClientPhyCallback_t xcbCache = NULL;

static void wiced_bt_ble_read_phy_complete_cb (wiced_bt_ble_phy_update_t *p_phy_result)
{
    BTStatus_t xStatus = eBTStatusFail;

    if(p_phy_result->status == WICED_SUCCESS)
    {
        xStatus = eBTStatusSuccess;
    }

    if(xcbCache)
    {
        xcbCache(wiced_bt_get_conn_id_from_peer_addr(p_phy_result->bd_address) ,p_phy_result->tx_phy, p_phy_result->rx_phy, xStatus);
    }
}

BTStatus_t prvBTReadPhy( uint16_t usConnId,
                         BTReadClientPhyCallback_t xCb )
{
    wiced_bt_dev_status_t xWICEDStatus;

    xcbCache =xCb;

    xWICEDStatus = wiced_bt_ble_read_phy( wiced_bt_get_peer_addr_from_conn_id(usConnId), wiced_bt_ble_read_phy_complete_cb);

    return prvConvertWicedErrorToAfr(xWICEDStatus);
}

/*-----------------------------------------------------------*/

const void * prvGetLeAdapter()
{
    return &xBTLeAdapter;
}
