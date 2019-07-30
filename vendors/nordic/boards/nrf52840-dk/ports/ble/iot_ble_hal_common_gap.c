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
 * @file iot_ble_hal_common_gap.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */
#include "iot_config.h"
#include <string.h>
#include <stdlib.h>
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_config.h"
#include "iot_ble_hal_internals.h"
#include "iot_ble_gap_config.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "security_dispatcher.h"
#include "fds.h"
#include "nrf_ble_lesc.h"
#include "nrf_strerror.h"
#include "sdk_config.h"
/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_HAL" )
#include "iot_logging_setup.h"


uint32_t ulGAPEvtMngHandle;
bool bIsAdvertising = false;
static BTCallbacks_t xBTCallbacks;
BTProperties_t xProperties;
BTBdaddr_t xConnectionRemoteAddress;
static bool bIsConnected = false;
uint8_t pucBondedAddresses[ btADDRESS_LEN * IOT_BLE_MAX_BONDED_DEVICES ];

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks );
BTStatus_t prvBtManagerCleanup( void );
BTStatus_t prvBTEnable( uint8_t ucGuestMode );
BTStatus_t prvBTDisable();
BTStatus_t prvBTGetAllDeviceProperties();
BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType );
BTStatus_t prvBTSetDeviceProperty( const BTProperty_t * pxProperty );
BTStatus_t prvBTGetAllRemoteDeviceProperties( BTBdaddr_t * pxRemoteAddr );
BTStatus_t prvBTGetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         BTPropertyType_t type );
BTStatus_t prvBTSetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         const BTProperty_t * property );
BTStatus_t prvBTPair( const BTBdaddr_t * pxBdAddr,
                      BTTransport_t xTransport,
                      bool bCreateBond );
BTStatus_t prvBTCreateBondOutOfBand( const BTBdaddr_t * pxBdAddr,
                                     BTTransport_t xTransport,
                                     const BTOutOfBandData_t * pxOobData );
BTStatus_t prvBTCancelBond( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTRemoveBond( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr,
                                    bool * bConnectionState );
BTStatus_t prvBTPinReply( const BTBdaddr_t * pxBdAddr,
                          uint8_t ucAccept,
                          uint8_t ucPinLen,
                          BTPinCode_t * pxPinCode );
BTStatus_t prvBTSspReply( const BTBdaddr_t * pxBdAddr,
                          BTSspVariant_t xVariant,
                          uint8_t ucAccept,
                          uint32_t ulPasskey );
BTStatus_t prvBTReadEnergyInfo();
BTStatus_t prvBTDutModeConfigure( bool bEnable );
BTStatus_t prvBTDutModeSend( uint16_t usOpcode,
                             uint8_t * pucBuf,
                             size_t xLen );
BTStatus_t prvBTLeTestMode( uint16_t usOpcode,
                            uint8_t * pucBuf,
                            size_t xLen );
BTStatus_t prvBTConfigHCISnoopLog( bool bEnable );
BTStatus_t prvBTConfigClear();
BTStatus_t prvBTReadRssi( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTGetTxpower( const BTBdaddr_t * pxBdAddr,
                            BTTransport_t xTransport );
void prvPmEventHandler( const pm_evt_t * event );
static void prvBTSetSecurityReq( security_req_t xLevel,
                                 ble_gap_conn_sec_mode_t * pxPerm,
                                 bool bLesc );

const void * prvGetClassicAdapter();
const void * prvGetLeAdapter();

static BTInterface_t xBTinterface =
{
    .pxBtManagerInit                = prvBTManagerInit,
    .pxBtManagerCleanup             = prvBtManagerCleanup,
    .pxEnable                       = prvBTEnable,
    .pxDisable                      = prvBTDisable,
    .pxGetAllDeviceProperties       = prvBTGetAllDeviceProperties,
    .pxGetDeviceProperty            = prvBTGetDeviceProperty,
    .pxSetDeviceProperty            = prvBTSetDeviceProperty,
    .pxGetAllRemoteDeviceProperties = prvBTGetAllRemoteDeviceProperties,
    .pxGetRemoteDeviceProperty      = prvBTGetRemoteDeviceProperty,
    .pxSetRemoteDeviceProperty      = prvBTSetRemoteDeviceProperty,
    .pxPair                         = prvBTPair,
    .pxCreateBondOutOfBand          = prvBTCreateBondOutOfBand,
    .pxCancelBond                   = prvBTCancelBond,
    .pxRemoveBond                   = prvBTRemoveBond,
    .pxGetConnectionState           = prvBTGetConnectionState,
    .pxPinReply                     = prvBTPinReply,
    .pxSspReply                     = prvBTSspReply,
    .pxReadEnergyInfo               = prvBTReadEnergyInfo,
    .pxDutModeConfigure             = prvBTDutModeConfigure,
    .pxDutModeSend                  = prvBTDutModeSend,
    .pxLeTestMode                   = prvBTLeTestMode,
    .pxConfigHCISnoopLog            = prvBTConfigHCISnoopLog,
    .pxConfigClear                  = prvBTConfigClear,
    .pxReadRssi                     = prvBTReadRssi,
    .pxGetTxpower                   = prvBTGetTxpower,
    .pxGetClassicAdapter            = prvGetClassicAdapter,
    .pxGetLeAdapter                 = prvGetLeAdapter,
};

ble_gap_sec_params_t xSecurityParameters =
{
    .bond           = true,
    .mitm           = true,
    .lesc           = true,
    .keypress       = 0,
    .io_caps        = BLE_GAP_IO_CAPS_DISPLAY_YESNO,
    .oob            = 0,
    .min_key_size   = 7,
    .max_key_size   = 16,
    .kdist_own.enc  = 1,
    .kdist_own.id   = 1,
    .kdist_peer.enc = 1,
    .kdist_peer.id  = 1,
};

/*-----------------------------------------------------------*/


static ret_code_t prvDataLengthUpdate( uint16_t usConnHandle,
                                       uint16_t usDataLength )
{
    IotLogDebug( "Updating data length to %u on connection 0x%x.",
                 usDataLength, usConnHandle );

    ble_gap_data_length_params_t const dlp =
    {
        .max_rx_octets  = usDataLength,
        .max_tx_octets  = usDataLength,
        .max_rx_time_us = BLE_GAP_DATA_LENGTH_AUTO,
        .max_tx_time_us = BLE_GAP_DATA_LENGTH_AUTO,
    };

    ble_gap_data_length_limitation_t dll = { 0 };

    ret_code_t err_code = sd_ble_gap_data_length_update( usConnHandle, &dlp, &dll );

    if( err_code != NRF_SUCCESS )
    {
        IotLogDebug( "sd_ble_gap_data_length_update() (request) on connection 0x%x returned %s.",
                     usConnHandle, nrf_strerror_get( err_code ) );

        if( ( dll.tx_payload_limited_octets != 0 ) ||
            ( dll.rx_payload_limited_octets != 0 ) )
        {
            IotLogError( "The requested TX/RX packet length is too long by %u/%u octets.",
                         dll.tx_payload_limited_octets, dll.rx_payload_limited_octets );
        }

        if( dll.tx_rx_time_limited_us != 0 )
        {
            IotLogError( "The requested combination of TX and RX packet lengths "
                         "is too long by %u microseconds.",
                         dll.tx_rx_time_limited_us );
        }
    }

    return err_code;
}

/*-----------------------------------------------------------*/

ret_code_t xBTGattUpdateMtu( nrf_ble_gatt_t * pxGattHandler,
                             uint16_t usConnHandle )
{
    ret_code_t xErrCode = NRF_SUCCESS;

    if( pxGattHandler == NULL )
    {
        xErrCode = NRF_ERROR_NULL;
    }

    if( xErrCode == NRF_SUCCESS )
    {
        nrf_ble_gatt_link_t * xLink = &pxGattHandler->links[ usConnHandle ];
        xLink->att_mtu_desired = pxGattHandler->att_mtu_desired_periph;

        if( xLink->att_mtu_desired > xLink->att_mtu_effective )
        {
            IotLogDebug( "Requesting to update ATT MTU to %u bytes on connection 0x%x.",
                         xLink->att_mtu_desired, usConnHandle );

            xErrCode = sd_ble_gattc_exchange_mtu_request( usConnHandle, xLink->att_mtu_desired );

            if( xErrCode == NRF_SUCCESS )
            {
                xLink->att_mtu_exchange_requested = true;
            }
            else if( xErrCode == NRF_ERROR_BUSY )
            {
                xLink->att_mtu_exchange_pending = true;
                IotLogError( "sd_ble_gattc_exchange_mtu_request()"
                             " on connection 0x%x returned busy, will retry.", usConnHandle );
            }
            else
            {
                IotLogError( "sd_ble_gattc_exchange_mtu_request() returned %s.",
                             nrf_strerror_get( xErrCode ) );
            }
        }

        /* Send a data length update request if necessary. */
        if( xLink->data_length_desired > xLink->data_length_effective )
        {
            ( void ) prvDataLengthUpdate( usConnHandle, xLink->data_length_desired );
        }
    }

    return xErrCode;
}


ret_code_t prvResumeAdvertisement( uint8_t ucAdvMode )
{
    ret_code_t xErrCode;

    xAdvertisingHandle.adv_params.properties.type = ucAdvMode;
    xErrCode = sd_ble_gap_adv_set_configure( &xAdvertisingHandle.adv_handle, xAdvertisingHandle.p_adv_data, &xAdvertisingHandle.adv_params );

    if( xErrCode == NRF_SUCCESS )
    {
        xErrCode = sd_ble_gap_adv_start( xAdvertisingHandle.adv_handle, aws_ble_gap_configAPP_BLE_CONN_CFG_TAG );
    }

    return xErrCode;
}
/*-----------------------------------------------------------*/

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void prvGAPeventHandler( ble_evt_t const * p_ble_evt,
                         void * p_context )
{
    BTUuid_t xUuid;
    ret_code_t xErrCode = NRF_SUCCESS;
    uint16_t usLocalHandle = 0;
    BTBondState_t xBondedState;
    nrf_ble_gatt_t * xGattHandler = prvGetGattHandle();

    ble_gap_phys_t const xPhys =
    {
        .rx_phys = BLE_GAP_PHY_AUTO,
        .tx_phys = BLE_GAP_PHY_AUTO,
    };

    switch( p_ble_evt->header.evt_id )
    {
        case BLE_GAP_EVT_CONNECTED:
            usGattConnHandle = p_ble_evt->evt.gap_evt.conn_handle;
            bIsConnected = true;

            if( bIsAdvertising == true )
            {
                xErrCode = prvResumeAdvertisement( BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED );

                if( xErrCode != NRF_SUCCESS )
                {
                    IotLogError( "Could not switch to non connectable advertisement." );
                }
            }

            memcpy( &xConnectionRemoteAddress.ucAddress, p_ble_evt->evt.gap_evt.params.connected.peer_addr.addr, BLE_GAP_ADDR_LEN );

            if( ( xErrCode == NRF_SUCCESS ) && bGattInitialized )
            {
                xErrCode = nrf_ble_gatt_att_mtu_periph_set( xGattHandler, xProperties.ulMtu );
            }

            if( ( xErrCode == NRF_SUCCESS ) && bGattInitialized )
            {
                xErrCode = xBTGattUpdateMtu( xGattHandler, usGattConnHandle );
            }

            if( xGattServerCb.pxConnectionCb )
            {
                xGattServerCb.pxConnectionCb( p_ble_evt->evt.gap_evt.conn_handle,
                                              ulGattServerIFhandle,
                                              true,
                                              &xConnectionRemoteAddress );
            }

            break;

        case BLE_GAP_EVT_DISCONNECTED:

            bIsConnected = false;

            /* Switch advertisement to connectable. */
            if( bIsAdvertising == true )
            {
                xErrCode = prvStopAdv();

                if( xErrCode == NRF_SUCCESS )
                {
                    xErrCode = prvResumeAdvertisement( BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED );
                }

                if( xErrCode != NRF_SUCCESS )
                {
                    IotLogError( "Could not resume to connectable advertisement." );
                }
            }

            if( xGattServerCb.pxConnectionCb )
            {
                xGattServerCb.pxConnectionCb( p_ble_evt->evt.gap_evt.conn_handle, ulGattServerIFhandle, false, &xConnectionRemoteAddress );
            }

            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:

            if( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ )
            {
                usLocalHandle = prvGattFromSDHandle( p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle );

                if( ( usLocalHandle != UINT16_MAX ) && ( xGattServerCb.pxRequestReadCb != NULL ) )
                {
                    xGattServerCb.pxRequestReadCb( p_ble_evt->evt.gatts_evt.conn_handle,
                                                   BLE_GATTS_AUTHORIZE_TYPE_READ,
                                                   &xConnectionRemoteAddress,
                                                   usLocalHandle,
                                                   p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.offset );
                }
            }
            else if( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE )
            {
                usLocalHandle = prvGattFromSDHandle( p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle );

                if( usLocalHandle != UINT16_MAX )
                {
                    bool IsNeedRsp = false;
                    bool IsPrep = false;

                    /* TODO: Complete all cases */
                    switch( p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.op )
                    {
                        case BLE_GATTS_OP_WRITE_REQ:
                            IsNeedRsp = true;
                            break;

                        case BLE_GATTS_OP_WRITE_CMD:
                            break;

                        case BLE_GATTS_OP_SIGN_WRITE_CMD:
                            break;

                        case BLE_GATTS_OP_PREP_WRITE_REQ:
                            IsPrep = true;
                            break;

                        case BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL:
                            break;

                        case BLE_GATTS_OP_EXEC_WRITE_REQ_NOW:
                            break;

                        default:
                            xErrCode = NRF_ERROR_INVALID_STATE;
                            break;
                    }

                    if( ( xErrCode == NRF_SUCCESS ) && ( xGattServerCb.pxRequestWriteCb != NULL ) )
                    {
                        xGattServerCb.pxRequestWriteCb( p_ble_evt->evt.gatts_evt.conn_handle,
                                                        BLE_GATTS_AUTHORIZE_TYPE_WRITE,
                                                        &xConnectionRemoteAddress,
                                                        usLocalHandle,
                                                        p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.offset,
                                                        p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.len,
                                                        IsNeedRsp,
                                                        IsPrep,
                                                        ( uint8_t * ) p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.data );
                    }
                }
            }
            else
            {
                IotLogError( "Error : BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST Invalid." );
            }

            break;

        case BLE_GATTS_EVT_WRITE:

            usLocalHandle = prvGattFromSDHandle( p_ble_evt->evt.gatts_evt.params.write.handle );

            if( usLocalHandle != UINT16_MAX )
            {
                bool IsNeedRsp = false;
                bool IsPrep = false;

                /* TODO: Complete all cases */
                switch( p_ble_evt->evt.gatts_evt.params.write.op )
                {
                    case BLE_GATTS_OP_WRITE_REQ:
                        IsNeedRsp = true;
                        break;

                    case BLE_GATTS_OP_WRITE_CMD:
                        break;

                    case BLE_GATTS_OP_SIGN_WRITE_CMD:
                        break;

                    case BLE_GATTS_OP_PREP_WRITE_REQ:
                        IsPrep = true;
                        break;

                    case BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL:
                        break;

                    case BLE_GATTS_OP_EXEC_WRITE_REQ_NOW:
                        break;

                    default:
                        xErrCode = NRF_ERROR_INVALID_STATE;
                        break;
                }

                if( ( xErrCode == NRF_SUCCESS ) && ( xGattServerCb.pxRequestWriteCb != NULL ) )
                {
                    xGattServerCb.pxRequestWriteCb( p_ble_evt->evt.gatts_evt.conn_handle,
                                                    0,
                                                    &xConnectionRemoteAddress,
                                                    usLocalHandle,
                                                    p_ble_evt->evt.gatts_evt.params.write.offset,
                                                    p_ble_evt->evt.gatts_evt.params.write.len,
                                                    IsNeedRsp,
                                                    IsPrep,
                                                    ( uint8_t * ) p_ble_evt->evt.gatts_evt.params.write.data );
                }
            }

            break;

        case BLE_GATTS_EVT_HVC:

            xGattServerCb.pxIndicationSentCb( p_ble_evt->evt.gap_evt.conn_handle, xErrCode );
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:

            xGattServerCb.pxIndicationSentCb( p_ble_evt->evt.gap_evt.conn_handle, xErrCode );
            break;

        case BLE_GATTC_EVT_TIMEOUT:

            IotLogError( "BLE_GATTC_EVT_TIMEOUT." );

            /* Disconnect on GATT Client timeout event. */
            IotLogError( "GATT Client Timeout." );
            xErrCode = sd_ble_gap_disconnect( p_ble_evt->evt.gattc_evt.conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION );
            APP_ERROR_CHECK( xErrCode );
            break;

        case BLE_GATTS_EVT_TIMEOUT:

            IotLogError( "BLE_GATTS_EVT_TIMEOUT." );

            /* Disconnect on GATT Server timeout event. */
            IotLogError( "GATT Server Timeout." );
            xErrCode = sd_ble_gap_disconnect( p_ble_evt->evt.gatts_evt.conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION );
            APP_ERROR_CHECK( xErrCode );
            break;

        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:

            if( xGattServerCb.pxMtuChangedCb != NULL )
            {
                xGattServerCb.pxMtuChangedCb( p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gattc_evt.params.exchange_mtu_rsp.server_rx_mtu );
            }

            break;

        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:

            if( xGattServerCb.pxMtuChangedCb != NULL )
            {
                xGattServerCb.pxMtuChangedCb( p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gatts_evt.params.exchange_mtu_request.client_rx_mtu );
            }

            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:

            if( xBTBleAdapterCallbacks.pxConnParameterUpdateCb )
            {
                uint16_t usActualInterval = p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval;
                BTStatus_t xStatus = eBTStatusSuccess;

                if( ( usActualInterval > xLatestDesiredConnectionParams.ulMaxInterval ) || ( usActualInterval < xLatestDesiredConnectionParams.ulMinInterval ) )
                {
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxConnParameterUpdateCb( xStatus,
                                                                xLatestDesiredConnectionParams.pxBdAddr,
                                                                xLatestDesiredConnectionParams.ulMinInterval,
                                                                xLatestDesiredConnectionParams.ulMaxInterval,
                                                                xLatestDesiredConnectionParams.ulLatency,
                                                                usActualInterval,
                                                                xLatestDesiredConnectionParams.ulTimeout );
            }

            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:

            xErrCode = sd_ble_gap_phy_update( p_ble_evt->evt.gap_evt.conn_handle, &xPhys );
            APP_ERROR_CHECK( xErrCode );
            break;


        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:

            break;

        case BLE_GAP_EVT_CONN_SEC_UPDATE:

            if( xBTCallbacks.pxPairingStateChangedCb )
            {
                if( xSecurityParameters.bond == true )
                {
                    xBondedState = eBTbondStateNone;
                }
                else
                {
                    xBondedState = eBTbondStateBonded;
                }

                xBTCallbacks.pxPairingStateChangedCb( eBTStatusSuccess,
                                                      &xConnectionRemoteAddress,
                                                      xBondedState,
                                                      p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv,
                                                      0
                                                      );
            }

            break;

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:

            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            nrf_ble_lesc_request_handler();
            break;

        case BLE_GAP_EVT_PASSKEY_DISPLAY:

            if( xBTCallbacks.pxSspRequestCb )
            {
                uint32_t ulPasskey = strtoul(
                    p_ble_evt->evt.gap_evt.params.passkey_display.passkey,
                    NULL,
                    10 );

                if( p_ble_evt->evt.gap_evt.params.passkey_display.match_request == 1 )
                {
                    xBTCallbacks.pxSspRequestCb( &xConnectionRemoteAddress,
                                                 NULL,
                                                 0,
                                                 eBTsspVariantPasskeyConfirmation,
                                                 ulPasskey );
                }
                else
                {
                    xBTCallbacks.pxSspRequestCb( &xConnectionRemoteAddress,
                                                 NULL,
                                                 0,
                                                 eBTsspVariantPasskeyNotification,
                                                 ulPasskey );
                }
            }

            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:

            /* No system attributes have been stored. */
            xErrCode = sd_ble_gatts_sys_attr_set( usGattConnHandle, NULL, 0, 0 );
            APP_ERROR_CHECK( xErrCode );
            break;

        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            break;

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
           {
               uint8_t status = p_ble_evt->evt.gap_evt.params.auth_status.auth_status;

               if( status != 0x00 )
               {
                   uint8_t error_src = p_ble_evt->evt.gap_evt.params.auth_status.error_src;
                   NRF_LOG_DEBUG( 0, "AUTH STATUS - status %d 0x%x\n\rERROR SOURCE - error %d 0x%x\n\r",
                                  status,
                                  status,
                                  error_src,
                                  error_src );

                   if( xBTCallbacks.pxBondedCb && ( status == 0x85 ) ) /* NOTE: Decode other errors */
                   {
                       xBTCallbacks.pxBondedCb( eBTStatusAuthRejected, &xConnectionRemoteAddress, false );
                   }
               }
               else
               {
                   if( xBTCallbacks.pxBondedCb )
                   {
                       xBTCallbacks.pxBondedCb( eBTStatusSuccess,
                                                &xConnectionRemoteAddress,
                                                true );
                   }
               }
           }

            IotLogDebug( "BLE_GAP_EVT_AUTH_STATUS." );

            break;

        default:
            IotLogError( "UNKNOWN BLE EVENT %d.", p_ble_evt->header.evt_id );

            break;
    }
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    memset( &xProperties, 0, sizeof( xProperties ) );
    xProperties.xDeviceType = eBTdeviceDevtypeBle;
    /* Set the device name from the iot_ble_config.h. We store it without a trailing zero. */
    xProperties.usDeviceNameLength = sizeof( IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME ) - 1;
    xProperties.puDeviceName = ( uint8_t * ) IotBle_Malloc( xProperties.usDeviceNameLength );
    xProperties.ulMtu = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;
    xProperties.bOnlySecure = true;
    xProperties.bBondable = true;

    if( xProperties.puDeviceName != NULL )
    {
        memcpy( xProperties.puDeviceName, IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME, xProperties.usDeviceNameLength );
    }
    else
    {
        xStatus = eBTStatusNoMem;
    }

    if( pxCallbacks != NULL )
    {
        xBTCallbacks = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBtManagerCleanup()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    IotBle_Free( xProperties.puDeviceName );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTEnable( uint8_t ucGuestMode )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /** If status is ok and callback is set, trigger the callback.
     *  If status is fail, not need to trig a callback as original call failed.
     **/
    if( ( xStatus == eBTStatusSuccess ) && ( xBTCallbacks.pxDeviceStateChangedCb != NULL ) )
    {
        xBTCallbacks.pxDeviceStateChangedCb( eBTstateOn );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDisable()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    uint32_t err_code = 0;

    err_code = nrf_sdh_disable_request();

    if( err_code != NRF_SUCCESS )
    {
        xStatus = eBTStatusFail;
    }

    /** If status is ok and callback is set, trigger the callback.
     *  If status is fail, not need to trig a callback as original call failed.
     **/
    if( ( xStatus == eBTStatusSuccess ) && ( xBTCallbacks.pxDeviceStateChangedCb != NULL ) )
    {
        xBTCallbacks.pxDeviceStateChangedCb( eBTstateOff );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetAllDeviceProperties()
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTProperty_t xReturnedProperty;

    ret_code_t xNRFstatus = NRF_SUCCESS;

    if( xBTCallbacks.pxAdapterPropertiesCb != NULL )
    {
        xReturnedProperty.xType = xType;

        switch( xType )
        {
            case eBTpropertyBdname:
                xReturnedProperty.pvVal = xProperties.puDeviceName;
                xReturnedProperty.xLen = xProperties.usDeviceNameLength;

                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyBdaddr:
                /* TODO: find out where shoud we store all the information provided to user */
                /* and who is responsible to free the memory */
                xNRFstatus = sd_ble_gap_addr_get( &xProperties.xDeviceAddress );

                if( xNRFstatus == NRF_SUCCESS )
                {
                    memcpy( ( ( BTBdaddr_t * ) xReturnedProperty.pvVal )->ucAddress, xProperties.xDeviceAddress.addr, BLE_GAP_ADDR_LEN );
                    xReturnedProperty.xLen = BLE_GAP_ADDR_LEN;
                }
                else
                {
                    xStatus = eBTStatusFail;
                }

                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyTypeOfDevice:
                xReturnedProperty.pvVal = &xProperties.xDeviceType;
                xReturnedProperty.xLen = sizeof( xProperties.xDeviceType );
                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyLocalMTUSize:
                xReturnedProperty.pvVal = &xProperties.ulMtu;
                xReturnedProperty.xLen = sizeof( xProperties.ulMtu );
                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyBondable:
                xReturnedProperty.pvVal = &xProperties.bBondable;
                xReturnedProperty.xLen = sizeof( bool );
                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyIO:

                switch( xSecurityParameters.io_caps )
                {
                    case BLE_GAP_IO_CAPS_DISPLAY_ONLY:
                        xProperties.xIOTypes = eBTIODisplayOnly;
                        break;

                    case BLE_GAP_IO_CAPS_DISPLAY_YESNO:
                        xProperties.xIOTypes = eBTIODisplayYesNo;
                        break;

                    case BLE_GAP_IO_CAPS_KEYBOARD_ONLY:
                        xProperties.xIOTypes = eBTIOKeyboardOnly;
                        break;

                    case BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY:
                        xProperties.xIOTypes = eBTIOKeyboardDisplay;
                        break;

                    default:
                        xProperties.xIOTypes = eBTIONone;
                        break;
                }

                xReturnedProperty.pvVal = &xProperties.xIOTypes;
                xReturnedProperty.xLen = sizeof( xProperties.xIOTypes );
                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyAdapterBondedDevices:
               {
                   int peer_count = 0;
                   pm_peer_data_bonding_t bonding_data;
                   ret_code_t xErrCode = NRF_SUCCESS;
                   pm_peer_id_t current_peer_id = pm_next_peer_id_get( PM_PEER_ID_INVALID );

                   while( current_peer_id != PM_PEER_ID_INVALID )
                   {
                       xErrCode = pm_peer_data_bonding_load( current_peer_id, &bonding_data );
                       /* TODO: Resolve real addresses */
                       memcpy( &( pucBondedAddresses[ peer_count * btADDRESS_LEN ] ), bonding_data.peer_ble_id.id_addr_info.addr, btADDRESS_LEN );
                       peer_count += 1;

                       if( peer_count >= IOT_BLE_MAX_BONDED_DEVICES )
                       {
                           break;
                       }

                       current_peer_id = pm_next_peer_id_get( current_peer_id );
                   }

                   xReturnedProperty.pvVal = pucBondedAddresses;
                   xReturnedProperty.xLen = peer_count;
                   xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
               }

               break;

            case eBTpropertySecureConnectionOnly:
                xReturnedProperty.pvVal = &xProperties.bOnlySecure;
                xReturnedProperty.xLen = 1;
                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );

            default:
                xStatus = eBTStatusUnsupported;
                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusFail, 0, NULL );
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvBTSetSecurityReq( security_req_t xLevel,
                                 ble_gap_conn_sec_mode_t * pxPerm,
                                 bool bLesc )
{
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS( pxPerm );

    switch( xLevel )
    {
        case SEC_NO_ACCESS:
            BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS( pxPerm );
            break;

        case SEC_OPEN:
            BLE_GAP_CONN_SEC_MODE_SET_OPEN( pxPerm );
            break;

        case SEC_JUST_WORKS:
            BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM( pxPerm );
            break;

        case SEC_MITM:

            if( bLesc )
            {
                BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM( pxPerm );
            }
            else
            {
                BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM( pxPerm );
            }

            break;

        case SEC_SIGNED:
            BLE_GAP_CONN_SEC_MODE_SET_SIGNED_NO_MITM( pxPerm );
            break;

        case SEC_SIGNED_MITM:
            BLE_GAP_CONN_SEC_MODE_SET_SIGNED_WITH_MITM( pxPerm );
            break;
    }
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetDeviceProperty( const BTProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ret_code_t xNRFstatus = NRF_SUCCESS;
    nrf_ble_gatt_t * xGattHandler = prvGetGattHandle();

    switch( pxProperty->xType )
    {
        case eBTpropertyBdname:

            xProperties.usDeviceNameLength = pxProperty->xLen;
            IotBle_Free( xProperties.puDeviceName );
            xProperties.puDeviceName = ( uint8_t * ) IotBle_Malloc( xProperties.usDeviceNameLength );

            if( xProperties.puDeviceName != NULL )
            {
                memcpy( xProperties.puDeviceName, ( pxProperty->pvVal ), xProperties.usDeviceNameLength );

                ble_gap_conn_sec_mode_t xSecurityMode;
                security_req_t xLevel = SEC_NO_ACCESS;
                bool bLesc;

                #if ( bleconfigREQUIRE_SIGNED && bleConfigENCRYPTION_REQUIRED )
                    /* Mode 2 */
                    #if bleconfigREQUIRE_MITM
                        xLevel = SEC_SIGNED_MITM; /* Level 2 */
                    #else
                        xLevel = SEC_SIGNED;      /* Level 1 */
                    #endif
                #else
                    /* Mode 1 */
                    #if ( bleconfigREQUIRE_MITM && IOT_BLE_ENABLE_SECURE_CONNECTION ) /* Level 4 */
                        xLevel = SEC_MITM;
                        bLesc = true;
                    #elif bleconfigREQUIRE_MITM  /* Level 3 */
                        xLevel = SEC_MITM;
                        bLesc = false;
                    #elif ( bleConfigENCRYPTION_REQUIRED && ( !bleconfigREQUIRE_MITM ) )  /* Level 2 */
                        xLevel = SEC_JUST_WORKS;
                    #else
                        xLevel = SEC_OPEN; /* Level 1 */
                    #endif
                #endif /* if ( bleconfigREQUIRE_SIGNED && bleConfigENCRYPTION_REQUIRED ) */

                prvBTSetSecurityReq( xLevel, &xSecurityMode, bLesc );
                xNRFstatus = sd_ble_gap_device_name_set( &xSecurityMode, xProperties.puDeviceName, xProperties.usDeviceNameLength );

                if( xNRFstatus != NRF_SUCCESS )
                {
                    xStatus = eBTStatusFail;
                }
            }
            else
            {
                xStatus = eBTStatusNoMem;
            }

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertyBdaddr:

            xStatus = eBTStatusUnsupported;
            break;

        case eBTpropertyTypeOfDevice:

            xStatus = eBTStatusUnsupported;
            break;

        case eBTpropertyLocalMTUSize: /* TODO: Check if we should save gatt handler from connection even and update its MTU */
            xProperties.ulMtu = *( ( uint32_t * ) pxProperty->pvVal );

            if( bGattInitialized )
            {
                xNRFstatus = nrf_ble_gatt_att_mtu_periph_set( xGattHandler, xProperties.ulMtu );

                if( ( xNRFstatus == NRF_SUCCESS ) && bIsConnected )
                {
                    xNRFstatus = xBTGattUpdateMtu( xGattHandler, usGattConnHandle );
                }
            }

            xStatus = BTNRFError( xNRFstatus );

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertyBondable:
            xProperties.bBondable = *( ( bool * ) pxProperty->pvVal );

            if( xProperties.bBondable == true )
            {
                xSecurityParameters.bond = true;
                xSecurityParameters.kdist_own.enc = 1;
                xSecurityParameters.kdist_own.id = 1;
                xSecurityParameters.kdist_peer.enc = 1;
                xSecurityParameters.kdist_peer.id = 1;
            }
            else
            {
                xSecurityParameters.bond = false;
                xSecurityParameters.kdist_own.enc = 0;
                xSecurityParameters.kdist_own.id = 0;
                xSecurityParameters.kdist_peer.enc = 0;
                xSecurityParameters.kdist_peer.id = 0;
            }

            xNRFstatus = pm_sec_params_set( &xSecurityParameters );

            if( xNRFstatus != NRF_SUCCESS )
            {
                xStatus = eBTStatusFail;
            }

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertyIO:

            switch( *( ( BTIOtypes_t * ) pxProperty->pvVal ) )
            {
                case eBTIODisplayOnly:
                    xSecurityParameters.io_caps = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
                    break;

                case eBTIODisplayYesNo:
                    xSecurityParameters.io_caps = BLE_GAP_IO_CAPS_DISPLAY_YESNO;
                    break;

                case eBTIOKeyboardOnly:
                    xSecurityParameters.io_caps = BLE_GAP_IO_CAPS_KEYBOARD_ONLY;
                    break;

                case eBTIOKeyboardDisplay:
                    xSecurityParameters.io_caps = BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY;
                    break;

                default:
                    xSecurityParameters.io_caps = BLE_GAP_IO_CAPS_NONE;
                    xSecurityParameters.mitm = false;
                    break;
            }

            xNRFstatus = pm_sec_params_set( &xSecurityParameters );

            if( xNRFstatus != NRF_SUCCESS )
            {
                xStatus = eBTStatusFail;
            }

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertySecureConnectionOnly:

            if( *( ( bool * ) pxProperty->pvVal ) )
            {
                if( xSecurityParameters.io_caps == BLE_GAP_IO_CAPS_NONE )
                {
                    xStatus = eBTStatusUnsupported;
                }
                else
                {
                    xProperties.bOnlySecure = true;
                }
            }
            else
            {
                xProperties.bOnlySecure = false;
            }

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        default:
            xStatus = eBTStatusUnsupported;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetAllRemoteDeviceProperties( BTBdaddr_t * pxRemoteAddr )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         BTPropertyType_t type )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         const BTProperty_t * property )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTPair( const BTBdaddr_t * pxBdAddr,
                      BTTransport_t xTransport,
                      bool bCreateBond )
{
    ( void * ) xTransport; /* We use BLE anyway */
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTCreateBondOutOfBand( const BTBdaddr_t * pxBdAddr,
                                     BTTransport_t xTransport,
                                     const BTOutOfBandData_t * pxOobData )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTCancelBond( const BTBdaddr_t * pxBdAddr )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTRemoveBond( const BTBdaddr_t * pxBdAddr )
{
    /*#define DELETE_ALL_BONDS */
    int peer_count = 0;
    ret_code_t xErrCode = NRF_SUCCESS;
    BTStatus_t xStatus = eBTStatusSuccess;

    #ifdef DELETE_ALL_BONDS
        xErrCode = pm_peers_delete();
    #else
        pm_peer_data_bonding_t bonding_data;
        pm_peer_id_t current_peer_id = pm_next_peer_id_get( PM_PEER_ID_INVALID );

        while( current_peer_id != PM_PEER_ID_INVALID )
        {
            xErrCode = pm_peer_data_bonding_load( current_peer_id, &bonding_data );
            /* TODO: Resolve real addresses */
            /* if (!memcmp(pxBdAddr->ucAddress, bonding_data.peer_ble_id.id_addr_info.addr, btADDRESS_LEN )) { */
            break;
            /*  }; */
            peer_count += 1;

            if( peer_count >= IOT_BLE_MAX_BONDED_DEVICES )
            {
                xErrCode = NRF_ERROR_NOT_FOUND;
                break;
            }

            current_peer_id = pm_next_peer_id_get( current_peer_id );
        }

        if( current_peer_id == PM_PEER_ID_INVALID )
        {
            xErrCode = NRF_ERROR_NOT_FOUND;
        }

        if( xErrCode == NRF_SUCCESS )
        {
            xErrCode = pm_peer_delete( current_peer_id );
        }
    #endif /* ifdef DELETE_ALL_BONDS */
    xStatus = BTNRFError( xErrCode );

    if( ( xBTCallbacks.pxBondedCb != NULL ) )
    {
        xBTCallbacks.pxBondedCb( xStatus, ( BTBdaddr_t * ) pxBdAddr, false );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr,
                                    bool * bConnectionState )
{
    return 0;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTPinReply( const BTBdaddr_t * pxBdAddr,
                          uint8_t ucAccept,
                          uint8_t ucPinLen,
                          BTPinCode_t * pxPinCode )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSspReply( const BTBdaddr_t * pxBdAddr,
                          BTSspVariant_t xVariant,
                          uint8_t ucAccept,
                          uint32_t ulPasskey )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ret_code_t xErrCode = NRF_SUCCESS;
    uint8_t passKey[ 6 ];

    switch( xVariant )
    {
        case eBTsspVariantPasskeyConfirmation:

            if( ucAccept )
            {
                xErrCode = sd_ble_gap_auth_key_reply( usGattConnHandle, BLE_GAP_AUTH_KEY_TYPE_PASSKEY, NULL );
            }
            else
            {
                xErrCode = sd_ble_gap_auth_key_reply( usGattConnHandle, BLE_GAP_AUTH_KEY_TYPE_NONE, NULL );
            }

            break;

        case eBTsspVariantPasskeyEntry:
            xStatus = eBTStatusUnsupported;
            break;

        case eBTsspVariantPasskeyNotification:
            xStatus = eBTStatusUnsupported;
            break;

        default:
            break;
    }

    if( xErrCode != NRF_SUCCESS )
    {
        BT_NRF_PRINT_ERROR( pm_conn_secure, xErrCode );
    }

    if( xErrCode != NRF_SUCCESS )
    {
        xStatus = eBTStatusFail;
    }

/*    ret_code_t xErrCode = pm_conn_secure(usGattConnHandle, false); */

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTReadEnergyInfo()
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDutModeConfigure( bool bEnable )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDutModeSend( uint16_t usOpcode,
                             uint8_t * pucBuf,
                             size_t xLen )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTLeTestMode( uint16_t usOpcode,
                            uint8_t * pucBuf,
                            size_t xLen )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTConfigHCISnoopLog( bool bEnable )
{
    return eBTStatusUnsupported;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTConfigClear()
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTReadRssi( const BTBdaddr_t * pxBdAddr )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetTxpower( const BTBdaddr_t * pxBdAddr,
                            BTTransport_t xTransport )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

const void * prvGetClassicAdapter()
{
    return NULL;
}

/*-----------------------------------------------------------*/

const BTInterface_t * BTGetBluetoothInterface()
{
    return &xBTinterface;
}

void prvPmEventHandler( const pm_evt_t * event )
{
    pm_handler_on_pm_evt( event );
    pm_handler_flash_clean( event );
    pm_conn_sec_config_t conn_sec_config;

    ret_code_t err_code;

    switch( event->evt_id )
    {
        case PM_EVT_BONDED_PEER_CONNECTED:

            if( xBTCallbacks.pxBondedCb )
            {
                xBTCallbacks.pxBondedCb( eBTStatusSuccess,
                                         &xConnectionRemoteAddress,
                                         true );
            }

            break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
            /* Repairing attempt between already bonded devices */
            conn_sec_config.allow_repairing = true;
            pm_conn_sec_config_reply( event->conn_handle, &conn_sec_config );
            break;

        case PM_EVT_CONN_SEC_START:
/* Consider to rework in case of AWS_BLE API further changes for different security connection establishing variants: */
/*            configPRINTF((( "PM_EVT_CONN_SEC_START\n" ); */
/* 2. eBTsspVariantPasskeyEntry for MITM with Display+Yes/No or Display/Keyboard IOCaps */
/* 3. eBTsspVariantPasskeyNotification for MITM with Display+Yes/No or Display/Keyboard IOCaps */
/* 4. eBTsspVariantConsent in case of NO MITM (Just Works) without IOCaps (eBTIONone) */
/*          configPRINTF( "PM_EVT_CONN_SEC_START\n" ); */
           {
               BTSspVariant_t xPairingVariant = eBTsspVariantConsent;

/* This is an informational event; no action is needed for the procedure to proceed. */
/* So, just responce with consent */
               switch( event->params.conn_sec_start.procedure )
               {
                   case PM_CONN_SEC_PROCEDURE_ENCRYPTION:
                       xPairingVariant = eBTsspVariantConsent;
                       break;

PM_CONN_SEC_PROCEDURE_BONDING:
                       xPairingVariant = eBTsspVariantConsent;
                       break;

PM_CONN_SEC_PROCEDURE_PAIRING:
                       xPairingVariant = eBTsspVariantConsent;
                       break;
               }

               if( xBTCallbacks.pxSspRequestCb )
               {
                   xBTCallbacks.pxSspRequestCb( &xConnectionRemoteAddress,
                                                NULL,
                                                0,
                                                xPairingVariant,
                                                0 );
               }
           }
           break;

        case PM_EVT_CONN_SEC_PARAMS_REQ:

            if( xProperties.bOnlySecure )
            {
                if( event->params.conn_sec_params_req.p_peer_params &&
                    ( event->params.conn_sec_params_req.p_peer_params->mitm == 0 ) ) /* The peer tries to connect in "Just Works" mode */
                {
                    sd_ble_gap_sec_params_reply( event->conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL );

                    if( xBTCallbacks.pxPairingStateChangedCb )
                    {
                        xBTCallbacks.pxPairingStateChangedCb( eBTStatusFail,
                                                              &xConnectionRemoteAddress,
                                                              eBTbondStateNone,
                                                              eBTSecLevelNoSecurity,
                                                              eBTauthFailInsuffSecurity );
                    }
                }
            }

/* Consider to rework in case of AWS_BLE API further changes for different security connection establishing variants: */
/* 1. eBTsspVariantPasskeyConfirmation for MITM with Display+Yes/No or Display/Keyboard IOCaps */
/* 2. eBTsspVariantPasskeyEntry for MITM with Display+Yes/No or Display/Keyboard IOCaps */
/* 3. eBTsspVariantPasskeyNotification for MITM with Display+Yes/No or Display/Keyboard IOCaps */
/* 4. eBTsspVariantConsent in case of NO MITM (Just Works) without IOCaps (eBTIONone) */
/*            if (xBTCallbacks.pxSspRequestCb) { */
/*                xBTCallbacks.pxSspRequestCb(NULL, */
/*                                            NULL, */
/*                                            0, */
/*                                            eBTsspVariantConsent, */
/*                                            0); */
/*            } */
            break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:

/* This is an event to inform user application when data was changed after successed write command/request */
/*            if( xBTCallbacks.pxBondedCb ) */
/*            { */
/*                if( event->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING ) */

/*                {
 * /* NOTE: Responce does not depend on condition */
/*                    if( event->params.peer_data_update_succeeded.action == PM_PEER_DATA_OP_UPDATE ) */
/*                    { */
/*                        xBTCallbacks.pxBondedCb( eBTStatusSuccess, */
/*                                                 &xConnectionRemoteAddress, */
/*                                                 true ); */
/*                    } */
/*                    else */
/*                    { */
/*                        xBTCallbacks.pxBondedCb( eBTStatusSuccess, */
/*                                                 &xConnectionRemoteAddress, */
/*                                                 true ); */
/*                    } */
/*                } */
/*            } */
            break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
            break;

        case PM_EVT_CONN_SEC_FAILED:
            IotLogError( "Failed to secure connection. Disconnecting." );
            {
                uint8_t error_src = event->params.conn_sec_failed.error_src;
                uint16_t error_code = event->params.conn_sec_failed.error;
                uint16_t process = event->params.conn_sec_failed.procedure;
                uint8_t hci_status_code = BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION;

                NRF_LOG_DEBUG( 0, "PM_EVT_CONN_SEC_FAILED src: 0x%x; err: 0x%x; proc: 0x%x\n\r",
                               error_src,
                               error_code,
                               process );

                if( error_code == PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING )
                {
                    hci_status_code = BLE_HCI_STATUS_CODE_PIN_OR_KEY_MISSING;
                }

                err_code = sd_ble_gap_disconnect( event->conn_handle, hci_status_code );

                if( err_code != NRF_ERROR_INVALID_STATE )
                {
                    APP_ERROR_CHECK( err_code );
                }
            }

            usGattConnHandle = BLE_CONN_HANDLE_INVALID;
            break;

        case PM_EVT_STORAGE_FULL:
            /* Run garbage collection on the flash. */
            err_code = fds_gc();

            if( err_code == FDS_ERR_NO_SPACE_IN_QUEUES )
            {
                /* Retry. */
            }
            else
            {
                APP_ERROR_CHECK( err_code );
            }

            break;

        case PM_EVT_PEER_DELETE_SUCCEEDED:
            break;

        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
            break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
            /* This can happen when the local DB has changed. */
            break;

        case PM_EVT_SERVICE_CHANGED_IND_SENT:
            break;

        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            /* Assert. */
            APP_ERROR_CHECK( event->params.peer_data_update_failed.error );
            break;

        case PM_EVT_PEER_DELETE_FAILED:
            /* Assert. */
            APP_ERROR_CHECK( event->params.peer_delete_failed.error );
            break;

        case PM_EVT_PEERS_DELETE_FAILED:
            /* Assert. */
            APP_ERROR_CHECK( event->params.peers_delete_failed_evt.error );
            break;

        case PM_EVT_ERROR_UNEXPECTED:
            /* Assert. */
            APP_ERROR_CHECK( event->params.error_unexpected.error );
            break;

        default:
            IotLogError( "PM_UNKNOWN, %d.", event->evt_id );
            break;
    }
}
