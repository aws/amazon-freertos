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
 * @file iot_ble_hal_common_gap.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */
#include "iot_config.h"
#include <string.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_gatts_api.h"
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

BTProperties_t xProperties;
static BTCallbacks_t xBTCallbacks;

static char bleDeviceName[ IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH + 1 ] = { 0 };

static BTSecurityLevel_t prvConvertESPauthModeToSecurityLevel( esp_ble_auth_req_t xAuthMode );
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

/*-----------------------------------------------------------*/

char * prxESPGetBLEDeviceName( void )
{
    return bleDeviceName;
}

void prvGAPeventHandler( esp_gap_ble_cb_event_t event,
                         esp_ble_gap_cb_param_t * param )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTSecurityLevel_t xSecurityLevel;
    BTBondState_t xBondedState;

    switch( event )
    {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:

            if( xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL )
            {
                if( param->adv_data_raw_cmpl.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );
            }

            break;

        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:

            if( xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL )
            {
                if( param->adv_data_cmpl.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );
            }

            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:

            if( xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL )
            {
                if( param->scan_rsp_data_raw_cmpl.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );
            }

            break;

        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:

            if( xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL )
            {
                if( param->scan_rsp_data_cmpl.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );
            }

            break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:

            if( xBTBleAdapterCallbacks.pxAdvStatusCb != NULL )
            {
                if( param->adv_start_cmpl.status != ESP_OK )
                {
                    IotLogError( "Failed to start advertisement" );
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxAdvStatusCb( xStatus, 0, true );
            }

            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:

            if( xBTBleAdapterCallbacks.pxAdvStatusCb != NULL )
            {
                if( param->adv_stop_cmpl.status != ESP_OK )
                {
                    IotLogError( "Failed to stop advertisement" );
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxAdvStatusCb( xStatus, 0, false );
            }

            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:

            if( xBTBleAdapterCallbacks.pxConnParameterUpdateCb != NULL )
            {
                if( param->update_conn_params.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xBTBleAdapterCallbacks.pxConnParameterUpdateCb( xStatus,
                                                                ( BTBdaddr_t * ) &param->update_conn_params.bda,
                                                                param->update_conn_params.min_int,
                                                                param->update_conn_params.max_int,
                                                                param->update_conn_params.latency,
                                                                param->update_conn_params.conn_int,
                                                                param->update_conn_params.timeout );
            }

            break;

        case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:

            if( xBTCallbacks.pxPairingStateChangedCb != NULL )
            {
                if( param->remove_bond_dev_cmpl.status == ESP_OK )
                {
                    xBTCallbacks.pxPairingStateChangedCb( eBTStatusSuccess,
                                                          ( BTBdaddr_t * ) &param->remove_bond_dev_cmpl.bd_addr,
                                                          eBTbondStateNone,
                                                          eBTSecLevelNoSecurity,
                                                          0 );
                }
            }

            break;

        /*
         * case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT:
         * case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:*/
        case ESP_GAP_BLE_AUTH_CMPL_EVT:
            xBondedState = eBTbondStateNone;

            if( param->ble_security.auth_cmpl.success == true )
            {
                xStatus = eBTStatusSuccess;
                xSecurityLevel = prvConvertESPauthModeToSecurityLevel( param->ble_security.auth_cmpl.auth_mode );

                if( xProperties.bBondable == true )
                {
                    xBondedState = eBTbondStateBonded;
                }
            }
            else
            {
                xStatus = eBTStatusFail;
                xSecurityLevel = eBTSecLevelNoSecurity;
            }

            if( xBTCallbacks.pxPairingStateChangedCb != NULL )
            {
                xBTCallbacks.pxPairingStateChangedCb( xStatus,
                                                      ( BTBdaddr_t * ) &param->ble_security.auth_cmpl.bd_addr,
                                                      xBondedState,
                                                      xSecurityLevel,
                                                      0 );
            }

            break;

        case ESP_GAP_BLE_KEY_EVT:
            break;

        case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:

            if( xBTCallbacks.pxSspRequestCb != NULL )
            {
                xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) &param->ble_security.ble_req.bd_addr,
                                             NULL,
                                             0,
                                             eBTsspVariantPasskeyNotification,
                                             0 );
            }

            break;

        case ESP_GAP_BLE_PASSKEY_REQ_EVT:

            if( xBTCallbacks.pxSspRequestCb != NULL )
            {
                xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) &param->ble_security.ble_req.bd_addr,
                                             NULL,
                                             0,
                                             eBTsspVariantPasskeyEntry,
                                             0 );
            }

            break;

        case ESP_GAP_BLE_NC_REQ_EVT:

            if( xBTCallbacks.pxSspRequestCb != NULL )
            {
                xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) &param->ble_security.ble_req.bd_addr,
                                             NULL,
                                             0,
                                             eBTsspVariantPasskeyConfirmation,
                                             param->ble_security.key_notif.passkey );
            }

            break;

        case ESP_GAP_BLE_SEC_REQ_EVT:

            if( xBTCallbacks.pxSspRequestCb != NULL )
            {
                xBTCallbacks.pxSspRequestCb( ( BTBdaddr_t * ) &param->ble_security.ble_req.bd_addr,
                                             NULL,
                                             0,
                                             eBTsspVariantConsent,
                                             0 );
            }

            break;

        default:
            break;
    }
}

/*
 * BTAuthFailureReason_t prvESPFailPairingReasonsToBTAuthFailingReasons(uint8_t ucESPFailPairing)
 * {
 *  BTAuthFailureReason_t xFailAuth;
 *
 *  switch(ucESPFailPairing)
 *  {
 *      case BTA_DM_AUTH_SMP_PASSKEY_FAIL      :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_OOB_FAIL                :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_PAIR_AUTH_FAIL          :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_CONFIRM_VALUE_FAIL       :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_PAIR_NOT_SUPPORT        :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_ENC_KEY_SIZE            :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_INVALID_CMD             :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_UNKNOWN_ERR       :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_REPEATED_ATTEMPT       :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_INVALID_PARAMETERS      :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_INTERNAL_ERR          :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_UNKNOWN_IO     :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_INIT_FAIL      :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_CONFIRM_FAIL :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_BUSY     :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_ENC_FAIL      :xFailAuth =  break;
 *      case BTA_DM_AUTH_SMP_RSP_TIMEOUT :xFailAuth =  break;
 *      default:xFailAuth break;
 *  }
 * } */

BTStatus_t prvConvertESPStatusToBTStatus( esp_bt_status_t xESPStatus )
{
    BTStatus_t xStatus;

    switch( xESPStatus )
    {
        case ESP_BT_STATUS_SUCCESS:
            xStatus = eBTStatusSuccess;
            break;

        case ESP_BT_STATUS_FAIL:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_NOT_READY:
            xStatus = eBTStatusNotReady;
            break;

        case ESP_BT_STATUS_NOMEM:
            xStatus = eBTStatusNoMem;
            break;

        case ESP_BT_STATUS_BUSY:
            xStatus = eBTStatusBusy;
            break;

        case ESP_BT_STATUS_DONE:
            xStatus = eBTStatusDone;
            break;

        case ESP_BT_STATUS_UNSUPPORTED:
            xStatus = eBTStatusUnsupported;
            break;

        case ESP_BT_STATUS_PARM_INVALID:
            xStatus = eBTStatusParamInvalid;
            break;

        case ESP_BT_STATUS_UNHANDLED:
            xStatus = eBTStatusUnHandled;
            break;

        case ESP_BT_STATUS_AUTH_FAILURE:
            xStatus = eBTStatusAuthFailure;
            break;

        case ESP_BT_STATUS_RMT_DEV_DOWN:
            xStatus = eBTStatusRMTDevDown;
            break;

        case ESP_BT_STATUS_AUTH_REJECTED:
            xStatus = eBTStatusAuthRejected;
            break;

        case ESP_BT_STATUS_INVALID_STATIC_RAND_ADDR:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_PENDING:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_UNACCEPT_CONN_INTERVAL:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_PARAM_OUT_OF_RANGE:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_TIMEOUT:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_PEER_LE_DATA_LEN_UNSUPPORTED:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_CONTROL_LE_DATA_LEN_UNSUPPORTED:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_ERR_ILLEGAL_PARAMETER_FMT:
            xStatus = eBTStatusFail;
            break;

        case ESP_BT_STATUS_MEMORY_FULL:
            xStatus = eBTStatusFail;
            break;

        default:
            xStatus = eBTStatusFail;
            break;
    }

    return xStatus;
}

BTSecurityLevel_t prvConvertESPauthModeToSecurityLevel( esp_ble_auth_req_t xAuthMode )
{
    BTSecurityLevel_t xSecurityLevel = eBTSecLevelNoSecurity;

    if( ( ( xAuthMode & ESP_LE_AUTH_REQ_SC_ONLY ) != 0 ) && ( ( xAuthMode & ESP_LE_AUTH_REQ_MITM ) != 0 ) )
    {
        xSecurityLevel = eBTSecLevelSecureConnect;
    }
    else if( ( xAuthMode & ESP_LE_AUTH_REQ_MITM ) != 0 ) /* only legacy MITM has been used */
    {
        xSecurityLevel = eBTSecLevelAuthenticatedPairing;
    }
    else
    {
        xSecurityLevel = eBTSecLevelUnauthenticatedPairing;
    }

    return xSecurityLevel;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Initialize BLE */

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
    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTEnable( uint8_t ucGuestMode )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    esp_err_t xRet = ESP_OK;
    esp_bt_controller_config_t xBtCfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    xRet = esp_bt_controller_init( &xBtCfg );

    if( xRet == ESP_OK )
    {
        xRet = esp_bt_controller_enable( ESP_BT_MODE_BLE );
    }
    else
    {
        configPRINTF( ( "Failed to initialize bt controller, err = %d.\n", xRet ) );
    }

    if( xRet == ESP_OK )
    {
        xRet = esp_bluedroid_init();
    }
    else
    {
        configPRINTF( ( "Failed to initialize bluedroid stack, err = %d.\n", xRet ) );
    }

    if( xRet == ESP_OK )
    {
        xRet = esp_bluedroid_enable();
    }

    if( xRet != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

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
    esp_err_t xRet = ESP_OK;

    if( esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED )
    {
        xRet = esp_bluedroid_disable();
    }

    if( xRet == ESP_OK )
    {
        xRet = esp_bluedroid_deinit();
    }

    if( xRet == ESP_OK )
    {
        if( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED )
        {
            xRet = esp_bt_controller_disable();
        }
    }

    if( xRet == ESP_OK )
    {
        xRet = esp_bt_controller_deinit();
    }

    if( xRet != ESP_OK )
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
BTStatus_t prvGetBondableDeviceList( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    int usNbDevices;
    uint16_t usIndex;
    esp_ble_bond_dev_t * pxESPDevlist;
    BTProperty_t xBondedDevices;
    esp_err_t xESPStatus;

    usNbDevices = esp_ble_get_bond_device_num();
    pxESPDevlist = IotBle_Malloc( sizeof( esp_ble_bond_dev_t ) * usNbDevices );
    xBondedDevices.pvVal = IotBle_Malloc( sizeof( BTBdaddr_t ) * usNbDevices );

    if( pxESPDevlist != NULL )
    {
        xESPStatus = esp_ble_get_bond_device_list( &usNbDevices, pxESPDevlist );

        if( xESPStatus == ESP_OK )
        {
            for( usIndex = 0; usIndex < usNbDevices; usIndex++ )
            {
                memcpy( &( ( BTBdaddr_t * ) xBondedDevices.pvVal )[ usIndex ], &pxESPDevlist[ usIndex ].bd_addr, sizeof( BTBdaddr_t ) );
            }

            xBondedDevices.xLen = usNbDevices * sizeof( BTBdaddr_t );
            xBondedDevices.xType = eBTpropertyAdapterBondedDevices;

            xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xBondedDevices );
            IotBle_Free( pxESPDevlist );
            IotBle_Free( xBondedDevices.pvVal );
        }
        else
        {
            xStatus = eBTStatusFail;
        }
    }
    else
    {
        xBondedDevices.xLen = 0;
        xBondedDevices.xType = eBTpropertyAdapterBondedDevices;
        xBondedDevices.pvVal = NULL;
        xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xBondedDevices );
    }

    return xStatus;
}


BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType )
{
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
                xReturnedProperty.xLen = strlen( bleDeviceName );
                xReturnedProperty.xType = eBTpropertyBdname;
                xReturnedProperty.pvVal = ( void * ) bleDeviceName;

                xBTCallbacks.pxAdapterPropertiesCb( eBTStatusSuccess, 1, &xReturnedProperty );
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
    esp_err_t xESPstatus;

    switch( pxProperty->xType )
    {
        case eBTpropertyBdname:

            if( pxProperty->xLen <= IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH )
            {
                memcpy( bleDeviceName, pxProperty->pvVal, pxProperty->xLen );
                /* Add NULL termination for name string */
                bleDeviceName[ pxProperty->xLen ] = '\0';
                xESPstatus = esp_ble_gap_set_device_name( bleDeviceName );

                if( xESPstatus != ESP_OK )
                {
                    bleDeviceName[0] = '\0';
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

        case eBTpropertyLocalMTUSize:
            xProperties.ulMtu = *( ( uint32_t * ) pxProperty->pvVal );
            xESPstatus = esp_ble_gatt_set_local_mtu( xProperties.ulMtu );

            if( xESPstatus != ESP_OK )
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
    esp_err_t xESPstatus;

    xESPstatus = esp_ble_remove_bond_device( ( uint8_t * ) ( pxBdAddr->ucAddress ) );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
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
    esp_err_t xESPstatus = ESP_OK;

    switch( xVariant )
    {
        case eBTsspVariantPasskeyConfirmation:
            xESPstatus = esp_ble_confirm_reply( ( uint8_t * ) ( pxBdAddr->ucAddress ), ucAccept );
            break;

        case eBTsspVariantPasskeyEntry:
            xESPstatus = esp_ble_passkey_reply( ( uint8_t * ) ( pxBdAddr->ucAddress ), ucAccept, ulPasskey );
            break;

        case eBTsspVariantConsent:
            xESPstatus = esp_ble_gap_security_rsp( ( uint8_t * ) ( pxBdAddr->ucAddress ), ucAccept );
            break;

        case eBTsspVariantPasskeyNotification:
            break;

        default:
            break;
    }

    if( xESPstatus != ESP_OK )
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
