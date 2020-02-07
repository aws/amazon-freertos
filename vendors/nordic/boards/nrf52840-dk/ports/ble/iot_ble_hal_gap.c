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

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "ble_conn_params.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_strerror.h"
#include "peer_manager.h"
#include "nrf_ble_lesc.h"
#include "iot_ble_hal_internals.h"
#include "iot_ble_gap_config.h"
#include "aws_clientcredential.h"
#include "nrf_log.h"
/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL                         IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL                         IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME                              ( "BLE_HAL" )
#include "iot_logging_setup.h"

#define iot_ble_hal_gapADVERTISING_BUFFER_SIZE        31
#define iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE    2

BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;
uint16_t usConnHandle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

#define BLE_ADVERTISING_DEF_NO_STATIC( _name )                  \
    ble_advertising_t _name;                                    \
    NRF_SDH_BLE_OBSERVER( _name ## _ble_obs,                    \
                          BLE_ADV_BLE_OBSERVER_PRIO,            \
                          ble_advertising_on_ble_evt, &_name ); \
    NRF_SDH_SOC_OBSERVER( _name ## _soc_obs,                    \
                          BLE_ADV_SOC_OBSERVER_PRIO,            \
                          ble_advertising_on_sys_evt, &_name )
BLE_ADVERTISING_DEF_NO_STATIC( xAdvertisingHandle ); /**< Advertising module instance. */
bool prvAdvRestart = false;

NRF_SDH_BLE_OBSERVER( m_ble_observer, aws_ble_gap_configAPP_BLE_OBSERVER_PRIO, prvGAPeventHandler, NULL );

static bool prvAdvertisingInitialized = false;
uint8_t ucBaseUUIDindex = 0;
BTUuid_t xAppUuid =
{
    .ucType  = eBTuuidType16,
    .uu.uu16 = ( uint16_t ) 0x00
};

BTConnectionParams_t xLatestDesiredConnectionParams =
{
    .pxBdAddr      = NULL,
    .ulMinInterval = 0,
    .ulMaxInterval = 0,
    .ulLatency     = 0,
    .ulTimeout     = 0
};

uint32_t prvRamStart = 0;

/* Cache for the advertising data */
ble_advdata_t prvAdvData;
ble_advdata_t prvScanResponseData;
uint8_t prvAdvBinData[ 2 ][ iot_ble_hal_gapADVERTISING_BUFFER_SIZE ];
uint8_t prvSrBinData[ 2 ][ iot_ble_hal_gapADVERTISING_BUFFER_SIZE ];

uint8_t prvCurrentAdvBuf = 0;

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
                                       BTGattAdvertismentParams_t * xAdvParams );
static BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t * advParams );
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

/** @brief Function for initializing the Connection Parameters module. */
static ret_code_t prvConnectionParamsInit( void );

/** @brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void prvOnConnParamsEvt( ble_conn_params_evt_t * pxEvt );

/** @brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void prvConnParamsErrorHandler( uint32_t ulError );

/** @brief BLE advertising event handler type. */
static void prvOnAdvEvt( ble_adv_evt_t const adv_evt );

/** @brief BLE advertising error handler type. */
static void prvOnAdvErr( uint32_t nrf_error );

/** @brief Initialization of the Peer Manager module. */
static ret_code_t prvPeerManagerInit( void );

/** @breef Frees all memory occupied with ble_advdata_t structure */
static void prvBTFreeAdvData( ble_advdata_t * xAdvData );

/** @breef Save parameters of the latest connection */
static void prvSetLatestConnParams( const BTBdaddr_t * pxBdAddr,
                                    uint32_t ulMinInterval,
                                    uint32_t ulMaxInterval,
                                    uint32_t ulLatency,
                                    uint32_t ulTimeout );

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


BTStatus_t prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks )
{
    ret_code_t xErrCode = NRF_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;


    memset( &prvAdvData, 0, sizeof( prvAdvData ) );
    memset( &prvScanResponseData, 0, sizeof( prvScanResponseData ) );
    memset( &xConnectionRemoteAddress, 0, sizeof( xConnectionRemoteAddress ) );

    /* Configure the BLE stack using the default settings.*/
    /* Fetch the start address of the application RAM.*/
    xErrCode = nrf_sdh_ble_default_cfg_set( aws_ble_gap_configAPP_BLE_CONN_CFG_TAG, &prvRamStart );
    BT_NRF_PRINT_ERROR( nrf_sdh_ble_default_cfg_set, xErrCode );

    if( xErrCode == NRF_SUCCESS )
    {
        /* Enable BLE stack.*/
        xErrCode = nrf_sdh_ble_enable( &prvRamStart );
        printf( "The RAM must start at %x, it's size must be %x\n", prvRamStart, 0x20040000 - prvRamStart );
        BT_NRF_PRINT_ERROR( nrf_sdh_ble_enable, xErrCode );
    }

    if( xErrCode == NRF_SUCCESS )
    {
        xErrCode = prvConnectionParamsInit();
        BT_NRF_PRINT_ERROR( prvConnectionParamsInit, xErrCode );
    }

    if( xErrCode == NRF_SUCCESS )
    {
        prvPeerManagerInit();
        BT_NRF_PRINT_ERROR( prvConnectionParamsInit, xErrCode );
    }

    xStatus = BTNRFError( xErrCode );

    /* TODO: Add initial security */
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

ret_code_t prvPeerManagerInit( void )
{
    ret_code_t xErrCode = NRF_SUCCESS;


    if( xErrCode == NRF_SUCCESS )
    {
        xErrCode = pm_init();
        BT_NRF_PRINT_ERROR( pm_init, xErrCode );
    }

    if( xErrCode == NRF_SUCCESS )
    {
        xErrCode = pm_sec_params_set( &xSecurityParameters );
        BT_NRF_PRINT_ERROR( pm_sec_params_set, xErrCode );
    }

    #ifdef TEST1
        if( xErrCode == NRF_SUCCESS )
        {
            xErrCode = pm_peers_delete();
            BT_NRF_PRINT_ERROR( pm_sec_params_set, xErrCode );
        }
    #endif

    if( xErrCode == NRF_SUCCESS )
    {
        xErrCode = pm_register( prvPmEventHandler );
        BT_NRF_PRINT_ERROR( pm_register, xErrCode );
    }

    return xErrCode;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTRegisterBleApp( BTUuid_t * pxAppUuid )
{
    ret_code_t xErrCode = NRF_SUCCESS;

    if( ucBaseUUIDindex != 0 )
    {
        /* We allow only one app to be registered */
        xErrCode = NRF_ERROR_INVALID_PARAM;
    }

    if( xErrCode == NRF_SUCCESS )
    {
        /* TODO: Think, do we even need that? */
        xErrCode = sd_ble_uuid_vs_add( ( ble_uuid128_t * ) pxAppUuid->uu.uu128, &ucBaseUUIDindex );
    }

    BT_NRF_PRINT_ERROR( sd_ble_uuid_vs_add, xErrCode );

    if( xErrCode == NRF_SUCCESS )
    {
        xAppUuid.ucType = pxAppUuid->ucType;
        memcpy( &xAppUuid.uu.uu128, &( pxAppUuid->uu.uu128 ), bt128BIT_UUID_LEN );

        if( xBTBleAdapterCallbacks.pxRegisterBleAdapterCb )
        {
            xBTBleAdapterCallbacks.pxRegisterBleAdapterCb( eBTStatusSuccess, 0, pxAppUuid );
        }
    }

    return BTNRFError( xErrCode );
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterBleApp( uint8_t ucAdapterIf )
{
    ret_code_t xErrCode = NRF_SUCCESS;

    if( ucBaseUUIDindex == 0 )
    {
        /* No app is registered */
        xErrCode == NRF_ERROR_INVALID_PARAM;
    }

    xErrCode = ble_conn_params_stop();
    prvBTFreeAdvData( &prvAdvData );
    prvBTFreeAdvData( &prvScanResponseData );

    /* We cannot remove the VS UUID because the SoftDevice can only remove the latest vendor-specific UUID */
    if( xErrCode == NRF_SUCCESS )
    {
        /* xErrCode = sd_ble_uuid_vs_remove( &ucBaseUUIDindex ); */
        ucBaseUUIDindex = 0;
    }

    BT_NRF_PRINT_ERROR( sd_ble_uuid_vs_remove, xErrCode );

    return BTNRFError( xErrCode );
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
    ret_code_t xErrCode = sd_ble_gap_disconnect( usConnHandle,
                                                 BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION );

    xLatestDesiredConnectionParams.pxBdAddr = NULL;
    BT_NRF_PRINT_ERROR( sd_ble_gap_disconnect, xErrCode );
    return BTNRFError( xErrCode );
}

/*-----------------------------------------------------------*/

ret_code_t prvStartAdv()
{
    ret_code_t xStatus;

    if( prvAdvRestart )
    {
        xStatus = prvResumeAdvertisement( BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED );
    }
    else
    {
        prvAdvRestart = true;
        xStatus = ble_advertising_start( &xAdvertisingHandle, BLE_ADV_MODE_FAST );
    }

    if( NRF_SUCCESS == xStatus )
    {
        bIsAdvertising = true;
    }

    return xStatus;
}

ret_code_t prvStopAdv()
{
    return sd_ble_gap_adv_stop( xAdvertisingHandle.adv_handle );
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStartAdv( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus;
    ret_code_t xErrCode = prvStartAdv();

    BT_NRF_PRINT_ERROR( ble_advertising_start, xErrCode );

    xStatus = BTNRFError( xErrCode );

    if( xBTBleAdapterCallbacks.pxAdvStatusCb )
    {
        xBTBleAdapterCallbacks.pxAdvStatusCb( xStatus, 0, true );
    }

    if( xStatus != eBTStatusSuccess )
    {
        IotLogError( "Failed to start advertisement." );
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTStopAdv( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus;
    ret_code_t xErrCode;

    bIsAdvertising = false;
    xErrCode = prvStopAdv();

    /* User can inadvertentely try to stop advertising when it is already stopped,
     * for example when connection is established and SD stopped advertising itself.
     * In this case we are going to get NRF_ERROR_INVALID_STATE, but this is fine */
    if( xErrCode == NRF_ERROR_INVALID_STATE )
    {
        xErrCode = NRF_SUCCESS;
        NRF_LOG_WARNING( "Attempt to stop an already stopped advertising" );
    }

    BT_NRF_PRINT_ERROR( sd_ble_gap_adv_stop, xErrCode );
    xStatus = BTNRFError( xErrCode );

    if( xBTBleAdapterCallbacks.pxAdvStatusCb )
    {
        xBTBleAdapterCallbacks.pxAdvStatusCb( xStatus, 0, false );
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

ret_code_t prvAFRUUIDtoNordic( BTUuid_t * pxAFRUuid,
                               ble_uuid_t * pxNordicUUID )
{
    ret_code_t xStatus = NRF_SUCCESS;
    ble_uuid128_t xNordicUUID128;

    switch( pxAFRUuid->ucType )
    {
        case eBTuuidType16:
            pxNordicUUID->type = BLE_UUID_TYPE_BLE;
            pxNordicUUID->uuid = pxAFRUuid->uu.uu16;
            break;

        case eBTuuidType128:

            for( int i = 0; i < 16; ++i )
            {
                xNordicUUID128.uuid128[ i ] = pxAFRUuid->uu.uu128[ i ];
            }

            xStatus = sd_ble_uuid_vs_add( &xNordicUUID128, &pxNordicUUID->type );
            pxNordicUUID->uuid = *( ( uint16_t * ) &( xNordicUUID128.uuid128[ 12 ] ) );
            break;

        default:
            xStatus = NRF_ERROR_NOT_SUPPORTED;
    }

    return xStatus;
}

void prvBTFreeAdvData( ble_advdata_t * xAdvData )
{
    IotBle_Free( xAdvData->p_tx_power_level );
    IotBle_Free( xAdvData->p_slave_conn_int );

    IotBle_Free( xAdvData->uuids_complete.p_uuids );
    IotBle_Free( xAdvData->uuids_more_available.p_uuids );
    IotBle_Free( xAdvData->uuids_solicited.p_uuids );

    if( xAdvData->p_manuf_specific_data )
    {
        IotBle_Free( xAdvData->p_manuf_specific_data->data.p_data );
    }

    IotBle_Free( xAdvData->p_manuf_specific_data );

    if( xAdvData->p_service_data_array )
    {
        IotBle_Free( xAdvData->p_service_data_array->data.p_data );
    }

    IotBle_Free( xAdvData->p_service_data_array );
    IotBle_Free( xAdvData->p_tk_value );
    IotBle_Free( xAdvData->p_sec_mgr_oob_flags );
    IotBle_Free( xAdvData->p_lesc_data );
    memset( xAdvData, 0, sizeof( ble_gap_adv_data_t ) );
}

ret_code_t prvBTAdvDataConvert( ble_advdata_t * xAdvData,
                                BTGattAdvertismentParams_t * pxParams,
                                uint16_t usManufacturerLen,
                                char * pcManufacturerData,
                                uint16_t usServiceDataLen,
                                char * pcServiceData,
                                BTUuid_t * pxServiceUuid,
                                size_t xNbServices )
{
    prvBTFreeAdvData( xAdvData );
    ret_code_t xErrCode = NRF_SUCCESS;

    uint16_t companyId = 0;

    ble_advdata_uuid_list_t xCompleteUUIDS;

    memset( &xCompleteUUIDS, 0, sizeof( xCompleteUUIDS ) );

    if( pxParams->ulAppearance )
    {
        xAdvData->include_appearance = pxParams->ulAppearance;
    }

    xAdvData->flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    if( ( pxParams->ucName.xType == BTGattAdvNameComplete ) || ( IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE >= sizeof( IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME ) - 1 ) )
    {
        xAdvData->name_type = BLE_ADVDATA_FULL_NAME;
    }
    else if( pxParams->ucName.xType == BTGattAdvNameShort )
    {
        xAdvData->name_type = BLE_ADVDATA_SHORT_NAME;
        xAdvData->short_name_len = IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE;
    }
    else
    {
        xAdvData->name_type = BLE_ADVDATA_NO_NAME;
    }

    if( pxParams->bIncludeTxPower )
    {
        xAdvData->p_tx_power_level = IotBle_Malloc( sizeof( int8_t ) );

        if( xAdvData->p_tx_power_level == NULL )
        {
            xErrCode = NRF_ERROR_NO_MEM;
        }
        else
        {
            *xAdvData->p_tx_power_level = 0; /* TODO: Find out where to get real tx_power */
        }
    }
    else
    {
        xAdvData->p_tx_power_level = NULL;
    }

    if( xErrCode == NRF_SUCCESS )
    {
        xAdvData->include_ble_device_addr = false; /* TODO: Find out where to get whether set address or not */

        /* TODO: Set address type */
        /* TODO: Find out what to do with the channel map */
        if( xNbServices != 0 )
        {
            xCompleteUUIDS.uuid_cnt = xNbServices;
            xCompleteUUIDS.p_uuids = IotBle_Malloc( sizeof( ble_uuid_t ) * xNbServices );

            if( xCompleteUUIDS.p_uuids == NULL )
            {
                xErrCode = NRF_ERROR_NO_MEM;
            }

            for( uint8_t ucCService = 0; ucCService < xNbServices; ucCService++ )
            {
                if( ( pxServiceUuid[ ucCService ].ucType == eBTuuidType16 ) ||
                    ( pxServiceUuid[ ucCService ].ucType == eBTuuidType128 ) )
                {
                    if( xErrCode != NRF_SUCCESS )
                    {
                        break;
                    }

                    /* We've already checeked the correctness of UUID type before, so we can ignore the returned value */
                    prvAFRUUIDtoNordic( &pxServiceUuid[ ucCService ], &xCompleteUUIDS.p_uuids[ ucCService ] );
                }
                else
                {
                    xErrCode = NRF_ERROR_NOT_SUPPORTED;
                }
            }
        }
    }

    if( xErrCode == NRF_SUCCESS )
    {
        /* The first two UUID go to the initial advertisement packet */
        xAdvData->uuids_complete = xCompleteUUIDS;
        xAdvData->p_slave_conn_int->max_conn_interval = pxParams->ulMaxInterval;
        xAdvData->p_slave_conn_int->min_conn_interval = pxParams->ulMinInterval;

        /* Set manufacturer specific data only when its length is atleast equal to iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE */
        if( ( pcManufacturerData != NULL ) && ( usManufacturerLen >= iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE ) )
        {
            xAdvData->p_manuf_specific_data = IotBle_Malloc( sizeof( ble_advdata_manuf_data_t ) );

            if( xAdvData->p_manuf_specific_data == NULL )
            {
                xErrCode = NRF_ERROR_NO_MEM;
            }
            else
            {
                /* extract company_identifier from input manufacturer data
                 * because nordic sdk has separate field for company_identifier.
                 *
                 * company identifier is two bytes in size stored in little endian form.
+                * Exctract the bytes and store into a 16bit variable.
+                */
                for( uint8_t i = 0; i < iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE; i++ )
                {
                    companyId = companyId | ( ( uint16_t ) pcManufacturerData[ i ] << ( i * 8 ) );
                }

                xAdvData->p_manuf_specific_data->company_identifier = companyId;

                /* allocate memory for rest of the data in pcManufacturerData i.e. (usManufacturerLen - first 2 octets) */
                xAdvData->p_manuf_specific_data->data.p_data = IotBle_Malloc( usManufacturerLen - iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE );

                if( xAdvData->p_manuf_specific_data->data.p_data == NULL )
                {
                    xErrCode = NRF_ERROR_NO_MEM;
                }
                else
                {
                    /* copy data left after extracting company_identifier (first 2 octets) */
                    memcpy( xAdvData->p_manuf_specific_data->data.p_data, pcManufacturerData + iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE, usManufacturerLen );
                    xAdvData->p_manuf_specific_data->data.size = usManufacturerLen - iot_ble_hal_gapADVERTISING_COMPANY_ID_SIZE;
                }
            }
        }
    }

    if( xErrCode == NRF_SUCCESS )
    {
        if( ( pcServiceData != NULL ) && ( usServiceDataLen != 0 ) )
        {
            xAdvData->p_service_data_array = IotBle_Malloc( sizeof( ble_advdata_service_data_t ) );

            if( xAdvData->p_service_data_array == NULL )
            {
                xErrCode = NRF_ERROR_NO_MEM;
            }
            else
            {
                xAdvData->p_service_data_array->data.p_data = IotBle_Malloc( usServiceDataLen );

                if( xAdvData->p_service_data_array->data.p_data )
                {
                    xErrCode = NRF_ERROR_NO_MEM;
                }
                else
                {
                    memcpy( xAdvData->p_service_data_array->data.p_data, pcServiceData, usServiceDataLen );
                    xAdvData->p_service_data_array->data.size = usServiceDataLen;
                }
            }
        }
    }

    return xErrCode;
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
    ret_code_t xErrCode = NRF_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;

    ble_adv_modes_config_t xAdvConfig;

    ble_advdata_t * xSettingData = NULL;
    ble_gap_adv_data_t xNewData;

    memset( &xNewData, 0, sizeof( xNewData ) );
    memset( &xAdvConfig, 0, sizeof( xAdvConfig ) );

    xErrCode = prvBTAdvDataConvert( pxParams->bSetScanRsp ? &prvScanResponseData : &prvAdvData,
                                    pxParams,
                                    usManufacturerLen,
                                    pcManufacturerData,
                                    usServiceDataLen,
                                    pcServiceData,
                                    pxServiceUuid,
                                    xNbServices );
    prvScanResponseData.flags = 0;

    xErrCode = sd_ble_gap_appearance_set( pxParams->ulAppearance );
    BT_NRF_PRINT_ERROR( sd_ble_gap_appearance_set, xErrCode );

    if( xErrCode == NRF_SUCCESS )
    {
        xAdvConfig.ble_adv_fast_enabled = true;
        xAdvConfig.ble_adv_fast_interval = IOT_BLE_ADVERTISING_INTERVAL;

        /* If it is an advertisement data and advertising timeout is provided, set the timeout. Else set the default timeout by vendor. */
        if( ( pxParams->bSetScanRsp == false ) && ( pxParams->usTimeout != 0 ) )
        {
            xAdvConfig.ble_adv_fast_timeout = pxParams->usTimeout;
        }
        else
        {
            xAdvConfig.ble_adv_fast_timeout = aws_ble_gap_configADV_DURATION;
        }

        xAdvConfig.ble_adv_primary_phy = pxParams->ucPrimaryAdvertisingPhy; /* TODO: Check which values can these variable get */
        xAdvConfig.ble_adv_secondary_phy = pxParams->ucSecondaryAdvertisingPhy;
        xAdvConfig.ble_adv_on_disconnect_disabled = true;
    }

    if( !xAdvertisingHandle.initialized )
    {
        ble_advertising_init_t init;
        memset( &init, 0, sizeof( init ) );
        memcpy( &init.advdata, &prvAdvData, sizeof( prvAdvData ) );
        memcpy( &init.srdata, &prvScanResponseData, sizeof( prvScanResponseData ) );
        memcpy( &init.config, &xAdvConfig, sizeof( xAdvConfig ) );
        init.config = xAdvConfig;
        xErrCode = ble_advertising_init( &xAdvertisingHandle, &init );
        BT_NRF_PRINT_ERROR( ble_advertising_init, xErrCode );

        if( xErrCode == NRF_SUCCESS )
        {
            ble_advertising_conn_cfg_tag_set( &xAdvertisingHandle, aws_ble_gap_configAPP_BLE_CONN_CFG_TAG );
        }
    }
    else
    {
        if( xErrCode == NRF_SUCCESS )
        {
            /* It seems we need to provide new buffers for the ble_advertising_advdata_update */
            prvCurrentAdvBuf = 1 - prvCurrentAdvBuf;
            xNewData.adv_data.p_data = prvAdvBinData[ prvCurrentAdvBuf ];
            xNewData.scan_rsp_data.p_data = prvSrBinData[ prvCurrentAdvBuf ];
            xNewData.adv_data.len = iot_ble_hal_gapADVERTISING_BUFFER_SIZE;
            xNewData.scan_rsp_data.len = iot_ble_hal_gapADVERTISING_BUFFER_SIZE;

            if( ( xNewData.adv_data.p_data == NULL ) || ( xNewData.scan_rsp_data.p_data == NULL ) )
            {
                xErrCode = NRF_ERROR_NO_MEM;
            }
        }

        if( xErrCode == NRF_SUCCESS )
        {
            xErrCode = ble_advdata_encode( &prvAdvData, xNewData.adv_data.p_data, &xNewData.adv_data.len );
            BT_NRF_PRINT_ERROR( ble_advdata_encode_ad, xErrCode );
        }

        if( xErrCode == NRF_SUCCESS )
        {
            xErrCode = ble_advdata_encode( &prvScanResponseData, xNewData.scan_rsp_data.p_data, &xNewData.scan_rsp_data.len );
            BT_NRF_PRINT_ERROR( ble_advdata_encode_sr, xErrCode );
        }

        if( xErrCode == NRF_SUCCESS )
        {
            xErrCode = ble_advertising_advdata_update( &xAdvertisingHandle, &xNewData, true );
            BT_NRF_PRINT_ERROR( ble_advertising_advdata_update, xErrCode );
        }

        if( xErrCode == NRF_SUCCESS )
        {
            ble_advertising_modes_config_set( &xAdvertisingHandle, &xAdvConfig );
        }
    }

    xStatus = BTNRFError( xErrCode );

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

BTStatus_t prvBTSetScanParameters( uint8_t ucAdapterIf,
                                   uint32_t ulScanInterval,
                                   uint32_t ulScanWindow )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvEnable( uint8_t ucAdapterIf,
                                BTGattAdvertismentParams_t * xAdvParams )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                BTGattAdvertismentParams_t * advParams )
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

ret_code_t prvConnectionParamsInit( void )
{
    ret_code_t xErrCode;
    ble_conn_params_init_t cp_init;
    ble_gap_conn_params_t gap_params_init;

    memset( &cp_init, 0, sizeof( cp_init ) );

    gap_params_init.conn_sup_timeout = aws_ble_gap_configCONN_SUP_TIMEOUT;
    gap_params_init.max_conn_interval = aws_ble_gap_configMAX_CONN_INTERVAL;
    gap_params_init.min_conn_interval = aws_ble_gap_configMIN_CONN_INTERVAL;
    gap_params_init.slave_latency = aws_ble_gap_configSLAVE_LATENCY;

    cp_init.p_conn_params = &gap_params_init;
    cp_init.first_conn_params_update_delay = aws_ble_gap_configFIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = aws_ble_gap_configNEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = aws_ble_gap_configMAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = prvOnConnParamsEvt;
    cp_init.error_handler = prvConnParamsErrorHandler;

    xErrCode = ble_conn_params_init( &cp_init );
    BT_NRF_PRINT_ERROR( ble_conn_params_init, xErrCode );
    return xErrCode;
}

/*-----------------------------------------------------------*/

static void prvSetLatestConnParams( const BTBdaddr_t * pxBdAddr,
                                    uint32_t ulMinInterval,
                                    uint32_t ulMaxInterval,
                                    uint32_t ulLatency,
                                    uint32_t ulTimeout )
{
    xLatestDesiredConnectionParams.pxBdAddr = pxBdAddr;
    xLatestDesiredConnectionParams.ulLatency = ulLatency;
    xLatestDesiredConnectionParams.ulMaxInterval = ulMaxInterval;
    xLatestDesiredConnectionParams.ulMinInterval = ulMinInterval;
    xLatestDesiredConnectionParams.ulTimeout = ulTimeout;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                            uint32_t ulMinInterval,
                                            uint32_t ulMaxInterval,
                                            uint32_t ulLatency,
                                            uint32_t ulTimeout )
{
    ble_gap_conn_params_t xConnParams;
    BTStatus_t xStatus;

    prvSetLatestConnParams( pxBdAddr, ulMinInterval, ulMaxInterval, ulLatency, ulTimeout );

    memset( &xConnParams, 0, sizeof( xConnParams ) );
    xConnParams.min_conn_interval = ulMinInterval;
    xConnParams.max_conn_interval = ulMaxInterval;
    xConnParams.slave_latency = ulLatency;
    xConnParams.conn_sup_timeout = ulTimeout;

    ret_code_t xErrCode = ble_conn_params_change_conn_params( usGattConnHandle, &xConnParams );
    xStatus = BTNRFError( xErrCode );

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvOnConnParamsEvt( ble_conn_params_evt_t * pxEvt )
{
    ret_code_t xErrCode;

    if( pxEvt->evt_type == BLE_CONN_PARAMS_EVT_FAILED )
    {
        xErrCode = sd_ble_gap_disconnect( usConnHandle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE );
        BT_NRF_PRINT_ERROR( sd_ble_gap_disconnect, xErrCode );
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void prvConnParamsErrorHandler( uint32_t ulError )
{
    APP_ERROR_HANDLER( ulError );
}


void prvOnAdvEvt( ble_adv_evt_t const adv_evt )
{
}

void prvOnAdvErr( uint32_t nrf_error )
{
}
