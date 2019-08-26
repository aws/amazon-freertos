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
 * @file aws_ble_hal_common_gap.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "esp_bt.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_log.h"
#include "esp_system.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

BTProperties_t xProperties;
static BTCallbacks_t xBTCallbacks;

static const char * tag = "AFR";
uint16_t usGattConnHandle;


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

BTSecurityLevel_t prvConvertESPauthModeToSecurityLevel( struct ble_gap_conn_desc desc )
{
    BTSecurityLevel_t xSecurityLevel = eBTSecLevelNoSecurity;

    if( desc.sec_state.encrypted )
    {
        xSecurityLevel = eBTSecLevelUnauthenticatedPairing;

        if( desc.sec_state.authenticated )
        {
            if( ble_hs_cfg.sm_sc )
            {
                xSecurityLevel = eBTSecLevelSecureConnect;
            }
            else
            {
                xSecurityLevel = eBTSecLevelAuthenticatedPairing;
            }
        }
    }

    return xSecurityLevel;
}

int prvGAPeventHandler( struct ble_gap_event * event,
                        void * arg )
{
    uint16_t ccc_val = 0;
    uint16_t handle;
    struct ble_gap_conn_desc desc;
    int rc;
    bool connected = false;
    BTStatus_t xStatus = eBTStatusSuccess;
    BTSecurityLevel_t xSecurityLevel;
    BTBondState_t xBondedState = eBTbondStateNone;

    switch( event->type )
    {
        case BLE_GAP_EVENT_CONNECT:
            usGattConnHandle = event->connect.conn_handle;
            /* A new connection was established or a connection attempt failed. */
            MODLOG_DFLT( INFO, "connection %s; status=%d ",
                         event->connect.status == 0 ? "established" : "failed",
                         event->connect.status );

            if( event->connect.status == 0 )
            {
                rc = ble_gap_conn_find( event->connect.conn_handle, &desc );
                assert( rc == 0 );
            }

            MODLOG_DFLT( INFO, "\n" );

            if( event->connect.status == 0 )
            {
                connected = true;
            }

            if( xGattServerCb.pxConnectionCb != NULL )
            {
                xGattServerCb.pxConnectionCb( event->connect.conn_handle, ulGattServerIFhandle, connected, ( BTBdaddr_t * ) desc.peer_id_addr.val );
            }

            return 0;

        case BLE_GAP_EVENT_DISCONNECT:
            usGattConnHandle = 0xffff;
            MODLOG_DFLT( INFO, "disconnect; reason=%d ", event->disconnect.reason );
            MODLOG_DFLT( INFO, "\n" );

            if( xGattServerCb.pxConnectionCb != NULL )
            {
                xGattServerCb.pxConnectionCb( event->disconnect.conn.conn_handle, ulGattServerIFhandle, false, ( BTBdaddr_t * ) &event->disconnect.conn.peer_id_addr.val );
            }

            return 0;

        case BLE_GAP_EVENT_CONN_UPDATE:
            MODLOG_DFLT( INFO, "connection updated; status=%d ",
                         event->conn_update.status );
            return 0;

        case BLE_GAP_EVENT_CONN_UPDATE_REQ:

            if( xBTBleAdapterCallbacks.pxConnParameterUpdateCb != NULL )
            {
                ble_gap_conn_find( event->conn_update_req.conn_handle, &desc );
                xBTBleAdapterCallbacks.pxConnParameterUpdateCb( eBTStatusSuccess,
                                                                ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                                event->conn_update_req.peer_params->itvl_min,
                                                                event->conn_update_req.peer_params->itvl_max,
                                                                event->conn_update_req.peer_params->latency,
                                                                desc.conn_itvl,
                                                                event->conn_update_req.peer_params->supervision_timeout );
            }

            return 0;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            MODLOG_DFLT( INFO, "advertise complete; reason=%d",
                         event->adv_complete.reason );
            return 0;

        case BLE_GAP_EVENT_ENC_CHANGE:
            /* Encryption has been enabled or disabled for this connection. */
            MODLOG_DFLT( INFO, "encryption change event; status=%d ",
                         event->enc_change.status );
            rc = ble_gap_conn_find( event->enc_change.conn_handle, &desc );
            assert( rc == 0 );
            MODLOG_DFLT( INFO, "\n" );
            /* Check peer parameters and if SC only and authenticated is 0 then send SSP request event with consent, else*/

            if( ble_hs_cfg.sm_sc )
            {
                if( desc.sec_state.encrypted )
                {
                    if( !desc.sec_state.authenticated )
                    {
                        if( xBTCallbacks.pxSspRequestCb != NULL )
                        {
                            xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                         NULL,
                                                         0,
                                                         eBTsspVariantConsent,
                                                         0 );
                        }

                        if( xBTCallbacks.pxPairingStateChangedCb != NULL )
                        {
                            xSecurityLevel = prvConvertESPauthModeToSecurityLevel( desc );

                            if( xSecurityLevel )
                            {
                                xBTCallbacks.pxPairingStateChangedCb( eBTStatusFail, ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                                      eBTbondStateBonded,
                                                                      xSecurityLevel,
                                                                      0 );
                            }
                        }

                        return 0;
                    }
                }
            }

            if( desc.sec_state.bonded )
            {
                xBondedState = eBTbondStateBonded;

                if( xBTCallbacks.pxBondedCb != NULL )
                {
                    xBTCallbacks.pxBondedCb( xStatus,
                                             ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                             true );
                }
            }

            if( xBTCallbacks.pxPairingStateChangedCb != NULL )
            {
                xSecurityLevel = prvConvertESPauthModeToSecurityLevel( desc );

                if( xSecurityLevel )
                {
                    xBTCallbacks.pxPairingStateChangedCb( eBTStatusSuccess, ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                          xBondedState,
                                                          xSecurityLevel,
                                                          0 );
                }
            }

            return 0;

        case BLE_GAP_EVENT_NOTIFY_RX:
            MODLOG_DFLT( INFO, "notification rx event; attr_handle=%d indication=%d "
                               "len=%d",
                         event->notify_rx.attr_handle,
                         event->notify_rx.indication,
                         OS_MBUF_PKTLEN( event->notify_rx.om ) );

            return 0;

        case BLE_GAP_EVENT_NOTIFY_TX:

            if( ( event->notify_tx.status == BLE_HS_EDONE ) && ( xGattServerCb.pxIndicationSentCb != NULL ) )
            {
                xGattServerCb.pxIndicationSentCb( event->notify_tx.conn_handle, xStatus );
            }

            return 0;

        case BLE_GAP_EVENT_SUBSCRIBE:
            rc = ble_gap_conn_find( event->subscribe.conn_handle, &desc );
            MODLOG_DFLT( DEBUG, "subscribe event; conn_handle=%d attr_handle=%d "
                                "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                         event->subscribe.conn_handle,
                         event->subscribe.attr_handle,
                         event->subscribe.reason,
                         event->subscribe.prev_notify,
                         event->subscribe.cur_notify,
                         event->subscribe.prev_indicate,
                         event->subscribe.cur_indicate );

            if( event->subscribe.prev_notify != event->subscribe.cur_notify )
            {
                ccc_val = event->subscribe.cur_notify;

                if( ccc_val )
                {
                    ESP_LOGI( TAG, "Notifications enabled" );
                }
                else
                {
                    ESP_LOGI( TAG, "Notifications and indications disabled" );
                }
            }
            else if( event->subscribe.prev_indicate != event->subscribe.cur_indicate )
            {
                if( event->subscribe.cur_indicate )
                {
                    ccc_val = 2;
                    ESP_LOGI( TAG, "Indications enabled" );
                }
                else
                {
                    ccc_val = 0;
                    ESP_LOGI( TAG, "Notifications and indications disabled" );
                }
            }

            handle = prvGattFromDevHandle( event->subscribe.attr_handle + 1 );

            if( handle && ( xGattServerCb.pxRequestWriteCb != NULL ) )
            {
                xGattServerCb.pxRequestWriteCb( event->subscribe.conn_handle, 0, ( BTBdaddr_t * ) desc.peer_id_addr.val, handle, 0, sizeof( ccc_val ), 1, 0, ( uint8_t * ) &ccc_val );
            }

            return 0;

            break;

        case BLE_GAP_EVENT_MTU:
            MODLOG_DFLT( INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                         event->mtu.conn_handle,
                         event->mtu.channel_id,
                         event->mtu.value );

            if( xGattServerCb.pxMtuChangedCb != NULL )
            {
                xGattServerCb.pxMtuChangedCb( event->mtu.conn_handle, event->mtu.value );
            }

            return 0;

        case BLE_GAP_EVENT_REPEAT_PAIRING:

            /* We already have a bond with the peer, but it is attempting to
             * establish a new secure link.  This app sacrifices security for
             * convenience: just throw away the old bond and accept the new link.
             */
            /* Delete the old bond. */
            rc = ble_gap_conn_find( event->repeat_pairing.conn_handle, &desc );
            assert( rc == 0 );
            ble_store_util_delete_peer( &desc.peer_id_addr );

            /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
             * continue with the pairing operation.
             */
            return BLE_GAP_REPEAT_PAIRING_RETRY;

        case BLE_GAP_EVENT_PASSKEY_ACTION:
            ESP_LOGI( tag, "PASSKEY_ACTION_EVENT started %d\n", event->passkey.params.action );
            rc = ble_gap_conn_find( event->passkey.conn_handle, &desc );

            if( xBTCallbacks.pxSspRequestCb != NULL )
            {
                xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                             NULL,
                                             0,
                                             eBTsspVariantConsent,
                                             0 );
            }

            if( event->passkey.params.action == BLE_SM_IOACT_DISP )
            {
                if( xBTCallbacks.pxSspRequestCb != NULL )
                {
                    xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                 NULL,
                                                 0,
                                                 eBTsspVariantConsent,
                                                 0 );
                }
            }
            else if( event->passkey.params.action == BLE_SM_IOACT_INPUT )
            {
                if( xBTCallbacks.pxSspRequestCb != NULL )
                {
                    xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                 NULL,
                                                 0,
                                                 eBTsspVariantPasskeyEntry,
                                                 0 );
                }
            }
            else if( event->passkey.params.action == BLE_SM_IOACT_NUMCMP )
            {
                if( xBTCallbacks.pxSspRequestCb != NULL )
                {
                    xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) desc.peer_id_addr.val,
                                                 NULL,
                                                 0,
                                                 eBTsspVariantPasskeyConfirmation,
                                                 event->passkey.params.numcmp );
                }
            }

            return 0;
    }

    return 0;
}

/*-----------------------------------------------------------*/
static void bleprph_on_reset( int reason )
{
    MODLOG_DFLT( ERROR, "Resetting state; reason=%d\n", reason );
}

static void bleprph_on_sync( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    int rc;

    rc = ble_hs_util_ensure_addr( 0 );

    if( rc != 0 )
    {
        xStatus = eBTStatusFail;
    }

    /** If status is ok and callback is set, trigger the callback.
     *  If status is fail, no need to trig a callback as original call failed.
     **/
    if( ( xStatus == eBTStatusSuccess ) && ( xBTCallbacks.pxDeviceStateChangedCb != NULL ) )
    {
        xBTCallbacks.pxDeviceStateChangedCb( eBTstateOn );
    }
}


void ble_host_task( void * param )
{
    nimble_port_run();
}

void ble_store_config_init( void );

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    nimble_port_init();

    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    nimble_port_freertos_init( ble_host_task );
    ble_store_config_init();

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

    esp_bt_controller_mem_release( ESP_BT_MODE_BLE );
    esp_bt_controller_mem_release( ESP_BT_MODE_BTDM );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTEnable( uint8_t ucGuestMode )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /** If status is ok and callback is set, trigger the callback.
     *  If status is fail, no need to trig a callback as original call failed.
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

    if( esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED )
    {
        if( esp_bt_controller_disable() != ESP_OK )
        {
            xStatus = eBTStatusFail;
        }
    }

    /** If status is ok and callback is set, trigger the callback.
     *  If status is fail, no need to trig a callback as original call failed.
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
BTStatus_t prvGetBondableDeviceList( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    int usNbDevices;
    uint16_t usIndex;
    BTProperty_t xBondedDevices = { 0 };
    int xESPStatus;

    xESPStatus = ble_store_util_count( BLE_STORE_OBJ_TYPE_OUR_SEC, &usNbDevices );

    if( xESPStatus != 0 )
    {
        return eBTStatusFail;
    }

    if( usNbDevices == 0 )
    {
        xBondedDevices.xLen = 0;
        xBondedDevices.xType = eBTpropertyAdapterBondedDevices;
        xBondedDevices.pvVal = NULL;
        xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xBondedDevices );
        return eBTStatusSuccess;
    }

    ble_addr_t * pxESPDevlist = pvPortMalloc( sizeof( ble_addr_t ) * usNbDevices );
    xBondedDevices.pvVal = pvPortMalloc( sizeof( BTBdaddr_t ) * usNbDevices );

    if( !pxESPDevlist || !xBondedDevices.pvVal )
    {
        return eBTStatusFail;
    }

    xESPStatus = ble_store_util_bonded_peers( pxESPDevlist, &usNbDevices, sizeof( ble_addr_t ) * usNbDevices );

    if( xESPStatus == 0 )
    {
        for( usIndex = 0; usIndex < usNbDevices; usIndex++ )
        {
            memcpy( &( ( BTBdaddr_t * ) xBondedDevices.pvVal )[ usIndex ], pxESPDevlist[ usIndex ].val, sizeof( BTBdaddr_t ) );
        }
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    xBondedDevices.xLen = usNbDevices;
    xBondedDevices.xType = eBTpropertyAdapterBondedDevices;
    xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, &xBondedDevices );

    vPortFree( pxESPDevlist );
    vPortFree( xBondedDevices.pvVal );

    return xStatus;
}


BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType )
{
    const char * name = NULL;
    BTStatus_t xStatus = eBTStatusSuccess;
    BTProperty_t xReturnedProperty;

    if( xBTCallbacks.pxAdapterPropertiesCb != NULL )
    {
        xReturnedProperty.xType = xType;

        switch( xType )
        {
            case eBTpropertyAdapterBondedDevices:
                xStatus = prvGetBondableDeviceList();
                break;

            case eBTpropertyBdname:
                xStatus = eBTStatusUnsupported;
                name = ble_svc_gap_device_name();

                if( name )
                {
                    xReturnedProperty.xLen = strlen( name );
                    xReturnedProperty.xType = eBTpropertyBdname;
                    xReturnedProperty.pvVal = ( void * ) name;
                }
                else
                {
                    xStatus = eBTStatusFail;
                }

                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, &xReturnedProperty );
                break;

            case eBTpropertyBdaddr:
                xStatus = eBTStatusUnsupported;
                break;

            case eBTpropertyTypeOfDevice:
                xStatus = eBTStatusUnsupported;
                break;

            case eBTpropertyLocalMTUSize:
                xStatus = eBTStatusUnsupported;
                break;

            case eBTpropertyBondable:
                xReturnedProperty.xLen = sizeof( bool );
                xReturnedProperty.xType = eBTpropertyBondable;
                xReturnedProperty.pvVal = ( void * ) &xProperties.bBondable;

                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertyIO:
                xReturnedProperty.xLen = sizeof( BTIOtypes_t );
                xReturnedProperty.xType = eBTpropertyIO;
                xReturnedProperty.pvVal = ( void * ) &xProperties.xPropertyIO;

                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            case eBTpropertySecureConnectionOnly:
                xReturnedProperty.xLen = sizeof( bool );
                xReturnedProperty.xType = eBTpropertySecureConnectionOnly;
                xReturnedProperty.pvVal = ( void * ) &xProperties.bSecureConnectionOnly;

                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
                break;

            default:
                xStatus = eBTStatusUnsupported;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetDeviceProperty( const BTProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    int xESPStatus;
    char * pcName;

    switch( pxProperty->xType )
    {
        case eBTpropertyBdname:
            pcName = pvPortMalloc( pxProperty->xLen + 1 );

            if( pcName != NULL )
            {
                memcpy( pcName, pxProperty->pvVal, pxProperty->xLen );
                /* Add NULL termination for name string */
                pcName[ pxProperty->xLen ] = '\0';
                xESPStatus = ble_svc_gap_device_name_set( pcName );

                if( xESPStatus != 0 )
                {
                    xStatus = eBTStatusFail;
                }

                vPortFree( pcName );
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

        case eBTpropertyLocalMTUSize:
            xProperties.ulMtu = *( ( uint32_t * ) pxProperty->pvVal );
            xESPStatus = ble_att_set_preferred_mtu( xProperties.ulMtu );

            if( xESPStatus != 0 )
            {
                xStatus = eBTStatusFail;
            }

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertyBondable:

            xProperties.bBondable = *( ( bool * ) pxProperty->pvVal ); /* update flag */
            xStatus = prvToggleBondableFlag( xProperties.bBondable );

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertyIO:

            xProperties.xPropertyIO = *( ( BTIOtypes_t * ) pxProperty->pvVal );
            xStatus = prvSetIOs( xProperties.xPropertyIO );

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }

            break;

        case eBTpropertySecureConnectionOnly:
            xProperties.bSecureConnectionOnly = *( ( bool * ) pxProperty->pvVal ); /* update flag */
            xStatus = prvToggleSecureConnectionOnlyMode( xProperties.bSecureConnectionOnly );

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
    BTStatus_t xStatus = eBTStatusSuccess;
    int xESPStatus, usNbDevices;
    ble_addr_t ble_addr = { 0 };

    memcpy( &ble_addr.val, pxBdAddr->ucAddress, sizeof( pxBdAddr->ucAddress ) );

    xESPStatus = ble_store_util_delete_peer( &ble_addr );

    if( xESPStatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    xESPStatus = ble_store_util_count( BLE_STORE_OBJ_TYPE_OUR_SEC, &usNbDevices );

    if( xBTCallbacks.pxBondedCb != NULL )
    {
        xBTCallbacks.pxBondedCb( xStatus,
                                 ( BTBdaddr_t * ) pxBdAddr,
                                 false );
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
    int xESPStatus = eBTStatusSuccess;
    struct ble_sm_io pkey;
    BTStatus_t xStatus = eBTStatusSuccess;

    switch( xVariant )
    {
        case eBTsspVariantPasskeyConfirmation:
            pkey.action = BLE_SM_IOACT_NUMCMP;
            pkey.numcmp_accept = ucAccept;
            xESPStatus = ble_sm_inject_io( usGattConnHandle, &pkey );
            break;

        case eBTsspVariantPasskeyEntry:
            pkey.action = BLE_SM_IOACT_DISP;
            pkey.passkey = ulPasskey;
            xESPStatus = ble_sm_inject_io( usGattConnHandle, &pkey );
            break;

        case eBTsspVariantPasskeyNotification:
        case eBTsspVariantConsent:
            break;

        default:
            break;
    }

    if( xESPStatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

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
