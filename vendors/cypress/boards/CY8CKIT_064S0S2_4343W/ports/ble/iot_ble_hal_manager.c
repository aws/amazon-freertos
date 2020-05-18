/*
 * $ Copyright Cypress Semiconductor $
 */

/**
 * @file bt_manager.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */

#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager_adapter_classic.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "bt_hal_internal.h"

#include "wiced_bt_dev.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_stack.h"

#include "platform_bt_nvram.h"

#include "wiced_rtos.h"

#include "platform_peripheral.h"


#define MAX_NUMBER_OF_BONDED_DEVIECS    5
#define STACK_INITIALIZATION_TIMEOUT    20000
#define ACE_BT_ADDRESS_ENTRY_LOCATION   13

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

#define BTM_SEC_MODE_SC             6

extern wiced_bool_t btsnd_hcic_change_name (uint8_t* name);
extern wiced_result_t BTM_ReadLocalDeviceNameFromController (wiced_bt_dev_cmpl_cback_t *p_rln_cmpl_cback);
extern UINT8 *BTM_ReadDeviceClass (void);
extern wiced_result_t BTM_SetDeviceClass (DEV_CLASS dev_class);

wiced_bool_t BTM_IsAclConnectionUp (wiced_bt_device_address_t remote_bda, wiced_bt_transport_t transport);
extern const wiced_bt_cfg_buf_pool_t wiced_bt_cfg_buf_pools[];
extern wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern void BTM_SetPairableMode (BOOLEAN allow_pairing, BOOLEAN connect_only_paired);
extern void BTM_SetSecurityMode (UINT8 sec_mode);

extern uint16_t wiced_bt_get_conn_id_from_peer_addr(const uint8_t* pucBdAddr);
int aceDeviceInfoDsHal_getEntry(const uint32_t entry, char* data, const uint32_t bufSize);

wiced_worker_thread_t btStack_worker_thread = {0};

extern BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;

WICEDPropCache_t xWICEDPropCache;
extern platform_bt_nvram_interface_t nvram_interface;
platform_bt_nvram_interface_t *pxWicedNvramInterface = &nvram_interface;
platform_bt_dev_property_interface_t *pxPlatformDevPropInterface = NULL;

uint32_t ulGAPEvtMngHandle;
static BTCallbacks_t xBTCallbacks;
static wiced_semaphore_t stack_init_semaphore;

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
static BTStatus_t prvBTSendSlaveSecurityRequest( const BTBdaddr_t * pxBdAddr,
                                                 BTSecurityLevel_t xSecurityLevel,
                                                 bool bBonding );
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
BTStatus_t CleanupGattServer(void);

uint32_t prvBTGetLastError( void );
BTStatus_t prvBTGetStackFeaturesSupport( uint32_t * pulFeatureMask );

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
    .pxSendSlaveSecurityRequest     = prvBTSendSlaveSecurityRequest,
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
    .pxGetLastError                 = prvBTGetLastError,
    // .pxGetStackFeaturesSupport      = prvBTGetStackFeaturesSupport
};

// extern BTClassicInterface_t xBTClassicAdapter;

/*-----------------------------------------------------------*/
BTStatus_t prvConvertWicedErrorToAfr (wiced_result_t xWicedErrorCode)
{
    BTStatus_t xAfrStatus;

    switch(xWicedErrorCode)
    {
        case WICED_SUCCESS:
            xAfrStatus = eBTStatusSuccess;
        break;

        case WICED_PENDING :
            xAfrStatus = eBTStatusNotReady;
        break;

        case WICED_BADARG :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_BADOPTION :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_UNSUPPORTED :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_OUT_OF_HEAP_SPACE :
            xAfrStatus = eBTStatusNoMem;
        break;

        case WICED_NOTUP :
            xAfrStatus = eBTStatusNotReady;
        break;

        case WICED_CONNECTION_LOST :
            xAfrStatus = eBTStatusRMTDevDown;
        break;

        case WICED_TIMEOUT :
        case WICED_PARTIAL_RESULTS :
        case WICED_ERROR :
        case WICED_UNFINISHED :
        case WICED_NOT_FOUND :
        case WICED_PACKET_BUFFER_CORRUPT :
        case WICED_CONNECTION_RESET :
        case WICED_CONNECTION_CLOSED :
        case WICED_NOT_CONNECTED :
        case WICED_ADDRESS_IN_USE :
        case WICED_NETWORK_INTERFACE_ERROR :
        case WICED_ALREADY_CONNECTED :
        case WICED_INVALID_INTERFACE :
        case WICED_SOCKET_CREATE_FAIL :
        case WICED_INVALID_SOCKET :
        case WICED_CORRUPT_PACKET_BUFFER :
        case WICED_UNKNOWN_NETWORK_STACK_ERROR :
        case WICED_ALREADY_INITIALIZED :
        default:
            xAfrStatus = eBTStatusFail;
        break;
    }

    return xAfrStatus;
}

uint32_t prvConvertCodToWICED(wiced_bt_dev_class_t codAfr)
{
    uint32_t cod = 0;

    for (int i = 0; i < 3; i++)
    {
        cod |= codAfr[i] << i * 8;
    }

    return cod;
}

uint32_t prvConvertIoCapToWICED(BTIOtypes_t IoCapWICED)
{
    uint32_t ioCap = 0;

    switch (IoCapWICED)
    {
        case eBTIONone:
            ioCap =  BTM_IO_CAPABILITIES_NONE;
            break;

        case eBTIODisplayOnly:
            ioCap =  BTM_IO_CAPABILITIES_DISPLAY_ONLY;
            break;

        case eBTIODisplayYesNo:
            ioCap = BTM_IO_CAPABILITIES_DISPLAY_AND_YES_NO_INPUT;
            break;

        case eBTIOKeyboardOnly:
            ioCap =  BTM_IO_CAPABILITIES_KEYBOARD_ONLY;
            break;

        case eBTIOKeyboardDisplay:
            ioCap =  BTM_IO_CAPABILITIES_BLE_DISPLAY_AND_KEYBOARD_INPUT;
            break;

        default:
            //@TODO:Is this a good default ??
            ioCap =  BTM_IO_CAPABILITIES_DISPLAY_AND_YES_NO_INPUT;
            break;
    }

    return ioCap;
}

/* bt/ble link management callback */
static wiced_result_t prvWicedBTManagementCb( wiced_bt_management_evt_t event,
                                              wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_result_t result = WICED_BT_SUCCESS;
    bool bStart;
    BTStatus_t xStatus;
    static uint32_t bSecurityReqReceived = false;

    switch ( event )
    {
        /* Bluetooth  stack enabled */
        case BTM_ENABLED_EVT:

            /* Trigger On event only when the callback status is success. Timeout and shutdown events are ignored for now */
            if(p_event_data->enabled.status == WICED_SUCCESS)
            {
                //create a worker thread to send cb to app thread
                if(!btStack_worker_thread._thread._thread)
                    wiced_rtos_create_worker_thread(&btStack_worker_thread, 0, 1024*2, 5);

                //Set default IO cap.
                xWICEDPropCache.xIoCap = eBTIODisplayYesNo;
                xWICEDPropCache.bBondable = TRUE;
                xWICEDPropCache.bSecureConnectionOnly = true;
                BTM_SetSecurityMode(BTM_SEC_MODE_SC);

                /** If status is ok and callback is set, trigger the callback.
                 *  If status is fail, not need to trig a callback as original call failed.
                 **/
                if( xBTCallbacks.pxDeviceStateChangedCb != NULL )
                {
                    xBTCallbacks.pxDeviceStateChangedCb( eBTstateOn );
                }

                 /* Let the prvBTManagerInit know that the radio is initialized successfully.
                   Call this only after the pxDeviceStateChangedCb is made */
                if(((uint32_t *)&stack_init_semaphore) != NULL)
                {
                    wiced_rtos_set_semaphore(&stack_init_semaphore);
                }
            }
            break;

        case BTM_DISABLED_EVT:
			if( xBTCallbacks.pxDeviceStateChangedCb != NULL )
            {
                xBTCallbacks.pxDeviceStateChangedCb( eBTstateOff );
            }
            break;

        case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT:
            p_event_data->pairing_io_capabilities_ble_request.local_io_cap      = prvConvertIoCapToWICED( xWICEDPropCache.xIoCap );
            p_event_data->pairing_io_capabilities_ble_request.oob_data          = BTM_OOB_NONE;
            p_event_data->pairing_io_capabilities_ble_request.max_key_size      = 0x10;
            p_event_data->pairing_io_capabilities_ble_request.init_keys         = BTM_LE_KEY_PENC|BTM_LE_KEY_PID|BTM_LE_KEY_PCSRK|BTM_LE_KEY_LENC;
            p_event_data->pairing_io_capabilities_ble_request.resp_keys         = BTM_LE_KEY_PENC|BTM_LE_KEY_PID|BTM_LE_KEY_PCSRK|BTM_LE_KEY_LENC;

            if(xWICEDPropCache.bBondable && \
                pxWicedNvramInterface != NULL)
            {
                p_event_data->pairing_io_capabilities_ble_request.auth_req = BTM_LE_AUTH_REQ_SC_MITM_BOND;
            }
            else
            {
                p_event_data->pairing_io_capabilities_ble_request.auth_req = BTM_LE_AUTH_REQ_SC_MITM;
            }

            //reset the SC flag if not requested by app
            p_event_data->pairing_io_capabilities_ble_request.auth_req &= ~((xWICEDPropCache.bSecureConnectionOnly) ? 0 : BTM_LE_AUTH_REQ_SC_ONLY);

            break;

        case BTM_PAIRING_IO_CAPABILITIES_BR_EDR_REQUEST_EVT:
            /* Use the default security for BR/EDR*/
            p_event_data->pairing_io_capabilities_br_edr_request.local_io_cap = BTM_IO_CAPABILITIES_NONE;
            p_event_data->pairing_io_capabilities_br_edr_request.auth_req = BTM_AUTH_SINGLE_PROFILE_GENERAL_BONDING_NO;
            break;

        case BTM_PAIRING_COMPLETE_EVT:

            //reset the flag for next connction
            bSecurityReqReceived = false;

            xStatus = (0 == p_event_data->pairing_complete.pairing_complete_info.ble.status) ? eBTStatusSuccess : eBTStatusFail;

            // printf("ble.status : %d, ble.sec_level : %d, reason : %d\n", p_event_data->pairing_complete.pairing_complete_info.ble.status, p_event_data->pairing_complete.pairing_complete_info.ble.sec_level, p_event_data->pairing_complete.pairing_complete_info.ble.reason);

            if ( xBTCallbacks.pxPairingStateChangedCb )
                xBTCallbacks.pxPairingStateChangedCb( xStatus, (BTBdaddr_t *) p_event_data->pairing_complete.bd_addr, eBTbondStateBonded, p_event_data->pairing_complete.pairing_complete_info.ble.sec_level, p_event_data->pairing_complete.pairing_complete_info.ble.reason );
            break;

        case BTM_PIN_REQUEST_EVT:
            if (xBTCallbacks.pxPinRequestCb)
                xBTCallbacks.pxPinRequestCb((BTBdaddr_t *) p_event_data->pin_request.bd_addr,
                        (BTBdname_t*) p_event_data->pin_request.bd_name,
                        prvConvertCodToWICED( wiced_bt_cfg_settings.device_class ), 0);
            break;

        case BTM_USER_CONFIRMATION_REQUEST_EVT:
            if ( xBTCallbacks.pxSspRequestCb )
                xBTCallbacks.pxSspRequestCb( (BTBdaddr_t *) p_event_data->user_confirmation_request.bd_addr, &xWICEDPropCache.xBtName,
                        prvConvertCodToWICED( wiced_bt_cfg_settings.device_class ), eBTsspVariantPasskeyConfirmation, p_event_data->user_confirmation_request.numeric_value );
            break;

        case BTM_PASSKEY_REQUEST_EVT:
            if ( xBTCallbacks.pxSspRequestCb )
                xBTCallbacks.pxSspRequestCb( (BTBdaddr_t *) p_event_data->user_passkey_request.bd_addr, &xWICEDPropCache.xBtName,
                        prvConvertCodToWICED( wiced_bt_cfg_settings.device_class ), eBTsspVariantPasskeyEntry, 0);
            break;

        case BTM_PASSKEY_NOTIFICATION_EVT:
            if ( xBTCallbacks.pxSspRequestCb )
                xBTCallbacks.pxSspRequestCb( (BTBdaddr_t *) p_event_data->user_passkey_notification.bd_addr, &xWICEDPropCache.xBtName,
                        prvConvertCodToWICED( wiced_bt_cfg_settings.device_class ), eBTsspVariantPasskeyNotification, p_event_data->user_passkey_notification.passkey);
            break;

        case BTM_SECURITY_REQUEST_EVT:
            bSecurityReqReceived = true;
            if (xBTCallbacks.pxSspRequestCb)
                xBTCallbacks.pxSspRequestCb((BTBdaddr_t *)p_event_data->user_confirmation_request.bd_addr, &xWICEDPropCache.xBtName,
                                            prvConvertCodToWICED(wiced_bt_cfg_settings.device_class), eBTsspVariantConsent, 0);
            break;

        case BTM_ENCRYPTION_STATUS_EVT:
            if (!bSecurityReqReceived)
            {
                if (xBTCallbacks.pxPairingStateChangedCb)
                {
                    xBTCallbacks.pxPairingStateChangedCb(eBTStatusSuccess, (BTBdaddr_t *)p_event_data->encryption_status.bd_addr, eBTbondStateBonded, eBTSecLevelSecureConnect, eBTauthSuccess);
                }
            }
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
            if(pxWicedNvramInterface != NULL && pxWicedNvramInterface->save_bonded_device_key != NULL)
            {
                pxWicedNvramInterface->save_bonded_device_key(
                        p_event_data->paired_device_link_keys_request.bd_addr,
                        (uint8_t *) &p_event_data->paired_device_link_keys_request.key_data,
                        sizeof(wiced_bt_device_sec_keys_t));
            }
            else
            {
                result = WICED_BT_UNSUPPORTED;
            }
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
            if(pxWicedNvramInterface != NULL && pxWicedNvramInterface->load_bonded_device_keys != NULL)
            {
                result = pxWicedNvramInterface->load_bonded_device_keys( p_event_data->paired_device_link_keys_request.bd_addr, \
                        (uint8_t *)&p_event_data->paired_device_link_keys_request.key_data, sizeof(wiced_bt_device_sec_keys_t));
            }
            else
            {
                result = WICED_BT_KEY_MISSING;
            }
            break;

        case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
            if(pxWicedNvramInterface != NULL && pxWicedNvramInterface->load_local_identity_keys != NULL)
            {
                result = pxWicedNvramInterface->load_local_identity_keys(&p_event_data->local_identity_keys_request);
            }
            else
            {
                result = WICED_BT_KEY_MISSING;
            }
            break;

        case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
            if(pxWicedNvramInterface != NULL && pxWicedNvramInterface->save_local_identity_keys != NULL)
            {
                result = pxWicedNvramInterface->save_local_identity_keys(&p_event_data->local_identity_keys_update);
            }
            else
            {
                result = WICED_BT_UNSUPPORTED;
            }
            break;

        case BTM_BLE_ADVERT_STATE_CHANGED_EVT:

            bStart = (BTM_BLE_ADVERT_OFF == p_event_data->ble_advert_state_changed) ? 0 : 1;
            if ( xBTBleAdapterCallbacks.pxAdvStatusCb )
                xBTBleAdapterCallbacks.pxAdvStatusCb( eBTStatusSuccess, 1, bStart);
            break;

        case BTM_BLE_PHY_UPDATE_EVT:
            if(xBTBleAdapterCallbacks.pxPhyUpdatedCb)
            {
                xBTBleAdapterCallbacks.pxPhyUpdatedCb( wiced_bt_get_conn_id_from_peer_addr(p_event_data->ble_phy_update_event.bd_address), p_event_data->ble_phy_update_event.tx_phy, p_event_data->ble_phy_update_event.rx_phy, p_event_data->ble_phy_update_event.status);
            }
            break;

        default:
            break;
    }

    return result;
}

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_result_t result;

    if( pxCallbacks != NULL )
    {
        xBTCallbacks = *pxCallbacks;
    }
    else
    {
        return eBTStatusParamInvalid;
    }

    //provide buffer for xBtName
    xWICEDPropCache.xBtName.ucName = xWICEDPropCache.pucDevName;


    result = wiced_bt_stack_init( prvWicedBTManagementCb, &wiced_bt_cfg_settings, wiced_bt_cfg_buf_pools );
    if ( WICED_SUCCESS != result )
    {
        return prvConvertWicedErrorToAfr(result);
    }

    /* Amazon requested to wait in this API and return only when radio is turned on */
    result = wiced_rtos_init_semaphore(&stack_init_semaphore);

    if(result == WICED_SUCCESS)
    {
         /* 10s timeout for stack initialization */
        result = wiced_rtos_get_semaphore(&stack_init_semaphore, STACK_INITIALIZATION_TIMEOUT);

        if(result != WICED_SUCCESS)
        {
            xStatus = prvConvertWicedErrorToAfr(result);
        }
        wiced_rtos_deinit_semaphore(&stack_init_semaphore);
    }
    else
    {
        xStatus = prvConvertWicedErrorToAfr(result);
    }

    if(pxWicedNvramInterface != NULL && pxWicedNvramInterface->init != NULL)
    {
        pxWicedNvramInterface->init();
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBtManagerCleanup()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTEnable( uint8_t ucGuestMode )
{

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDisable()
{
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_result_t result;

    CleanupGattServer();

    result = wiced_bt_stack_deinit();
    xStatus = prvConvertWicedErrorToAfr(result);

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
void prvLocalNameCb(void *p1)
{
    BTProperty_t xReturnedProperty;

    xReturnedProperty.xLen = strlen(p1);
    xReturnedProperty.xType = eBTpropertyBdname;
    xReturnedProperty.pvVal = p1;

    /* update the cached name */
    memcpy(xWICEDPropCache.pucDevName, p1, xReturnedProperty.xLen);

    if (xBTCallbacks.pxAdapterPropertiesCb)
        xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
}

BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTProperty_t xReturnedProperty;
    wiced_bt_device_address_t bda;
    wiced_bt_device_address_t paired_device_list[MAX_NUMBER_OF_BONDED_DEVIECS];
    wiced_bt_dev_bonded_device_info_t ram_bonded_device_list[MAX_NUMBER_OF_BONDED_DEVIECS];
    uint16_t usDevNum = 0, idx;

    if( xBTCallbacks.pxAdapterPropertiesCb != NULL )
    {
        xReturnedProperty.xType = xType;

        switch( xType )
        {
            case eBTpropertyBdname:
                BTM_ReadLocalDeviceNameFromController(prvLocalNameCb);
                break;

            case eBTpropertyBdaddr:
                wiced_bt_dev_read_local_addr(bda);
		xReturnedProperty.xLen = sizeof(wiced_bt_device_address_t);
				xReturnedProperty.xType = eBTpropertyBdaddr;
				xReturnedProperty.pvVal = bda;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
                break;

            case eBTpropertyTypeOfDevice:
                xReturnedProperty.xLen = 3;
                xReturnedProperty.xType = eBTpropertyTypeOfDevice;
                xReturnedProperty.pvVal = BTM_ReadDeviceClass();

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
                break;

    		case eBTpropertyAdapterBondedDevices:
    		    if(pxWicedNvramInterface != NULL && pxWicedNvramInterface->get_bonded_devices != NULL)
    		        pxWicedNvramInterface->get_bonded_devices( paired_device_list, &usDevNum );
                else
                {
                    //FIXME!!! get the list from NVRAM only
                    wiced_bt_dev_get_bonded_devices( ram_bonded_device_list, &usDevNum );

                    for(idx=0;idx<usDevNum;idx++)
                    {
                        memcpy(&paired_device_list[idx], ram_bonded_device_list[idx].bd_addr, 6);
                    }
                }

                xReturnedProperty.xLen = usDevNum * sizeof(wiced_bt_device_address_t);
                xReturnedProperty.xType = xType;
                xReturnedProperty.pvVal = paired_device_list;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
			break;

            case eBTpropertyRemoteFriendlyName:
            case eBTpropertyRemoteRssi:
            case eBTpropertyRemoteVersionInfo:
                // how do we know which remote are we talking about???
                xStatus = eBTStatusUnsupported;
                break;

            case eBTpropertyLocalMTUSize:
                xReturnedProperty.xLen = 1;
                xReturnedProperty.xType = eBTpropertyLocalMTUSize;
                xReturnedProperty.pvVal = (void*)&wiced_bt_cfg_settings.gatt_cfg.max_mtu_size;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
                break;

            case eBTpropertyBondable:
                xReturnedProperty.xLen = sizeof(bool);
                xReturnedProperty.xType = eBTpropertyBondable;
                xReturnedProperty.pvVal = &xWICEDPropCache.bBondable;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
                break;

            case eBTpropertyIO:
                xReturnedProperty.xLen = sizeof(BTIOtypes_t);
                xReturnedProperty.xType = eBTpropertyIO;
                xReturnedProperty.pvVal = &xWICEDPropCache.xIoCap;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
                break;

            case eBTpropertySecureConnectionOnly:
                xReturnedProperty.xLen = sizeof(bool);
                xReturnedProperty.xType = eBTpropertySecureConnectionOnly;
                xReturnedProperty.pvVal = &xWICEDPropCache.bSecureConnectionOnly;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
		break;

            case eBTpropertyConnectable:
                xReturnedProperty.xLen = sizeof(bool);
                xReturnedProperty.xType = eBTpropertyConnectable;
                xReturnedProperty.pvVal = &xWICEDPropCache.bConnectable;

                if (xBTCallbacks.pxAdapterPropertiesCb)
                    xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
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

                if( !btsnd_hcic_change_name( (uint8_t *)pcName ) )
                {
                    xStatus = eBTStatusFail;
                }
                else
                {
                    /*@TODO: We should read the name always from controller instead of caching !! */
                    memset(xWICEDPropCache.pucDevName, 0, sizeof(xWICEDPropCache.pucDevName));
                    memcpy(xWICEDPropCache.pucDevName, pcName, strlen(pcName));
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

		wiced_bt_set_local_bdaddr(pxProperty->pvVal);

		if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }
            break;

        case eBTpropertyTypeOfDevice:
            BTM_SetDeviceClass(pxProperty->pvVal);
            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }
            break;

        case eBTpropertyLocalMTUSize:
            wiced_bt_cfg_settings.gatt_cfg.max_mtu_size = *(uint16_t *)pxProperty->pvVal;

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }
            break;

        case eBTpropertyBondable:
            xWICEDPropCache.bBondable = *(bool *)pxProperty->pvVal;
            BTM_SetPairableMode(xWICEDPropCache.bBondable, false);

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ))
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }
            break;

        case eBTpropertyIO:
            xWICEDPropCache.xIoCap = (*(BTIOtypes_t *)pxProperty->pvVal);

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }
            break;

        case eBTpropertyRemoteFriendlyName:
            // how do we know which remote are we talking about???
            xStatus = eBTStatusUnsupported;
            break;

        case eBTpropertySecureConnectionOnly:
            xWICEDPropCache.bSecureConnectionOnly = *(bool*)pxProperty->pvVal;

            if( ( xBTCallbacks.pxAdapterPropertiesCb != NULL ) && ( xStatus == eBTStatusSuccess ) )
            {
                xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );
            }
            break;

        case eBTpropertyConnectable:
            xWICEDPropCache.bConnectable = (*(bool *)pxProperty->pvVal) ? BTM_CONNECTABLE : BTM_NON_CONNECTABLE;

            wiced_bt_dev_set_connectability(xWICEDPropCache.bConnectable,  BTM_DEFAULT_CONN_WINDOW,  BTM_DEFAULT_CONN_INTERVAL );

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

static BTStatus_t prvBTSendSlaveSecurityRequest( const BTBdaddr_t * pxBdAddr,
                                                 BTSecurityLevel_t xSecurityLevel,
                                                 bool bBonding )
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
    BTStatus_t xStatus = eBTStatusFail;
    wiced_bt_device_address_t xAddress;
    wiced_result_t xWicedStatus;

    memcpy(xAddress, pxBdAddr, sizeof(wiced_bt_device_address_t));
    xWicedStatus = wiced_bt_dev_delete_bonded_device(xAddress);

    if(xWicedStatus == WICED_SUCCESS)
    {
        if (pxWicedNvramInterface!= NULL && pxWicedNvramInterface->delete_bonded_device != NULL)
        {
            if (WICED_SUCCESS == pxWicedNvramInterface->delete_bonded_device(xAddress))
            {
                xStatus = eBTStatusSuccess;
            }
        }
        else
        {
            xStatus = eBTStatusSuccess;
        }
    }
    else
    {
        xStatus = prvConvertWicedErrorToAfr(xWicedStatus);
    }
    if ( xBTCallbacks.pxPairingStateChangedCb )
    {
        xBTCallbacks.pxPairingStateChangedCb( eBTStatusSuccess, (BTBdaddr_t *)pxBdAddr, eBTbondStateNone, eBTSecLevelSecureConnect, eBTauthSuccess);
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr,
                                    bool * bConnectionState )
{
    if( BTM_IsAclConnectionUp ( (uint8_t*) pxBdAddr->ucAddress, BT_TRANSPORT_BR_EDR ) )
    {
        *bConnectionState = true;
    }
    else if( BTM_IsAclConnectionUp ( (uint8_t*) pxBdAddr->ucAddress, BT_TRANSPORT_BR_EDR ) )
    {
        *bConnectionState = true;
    }
    else
    {
        *bConnectionState = false;
    }

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTPinReply( const BTBdaddr_t * pxBdAddr,
                          uint8_t ucAccept,
                          uint8_t ucPinLen,
                          BTPinCode_t * pxPinCode )
{
    uint8_t ucStatus = ( 1 == ucAccept ) ? WICED_BT_SUCCESS : WICED_BT_ERROR;
    wiced_bt_dev_pin_code_reply( (uint8_t*) pxBdAddr->ucAddress, ucStatus, ucPinLen, (uint8_t*) pxPinCode );

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSspReply( const BTBdaddr_t * pxBdAddr,
                          BTSspVariant_t xVariant,
                          uint8_t ucAccept,
                          uint32_t ulPasskey )
{
    uint8_t ucStatus = ( 1 == ucAccept ) ? WICED_BT_SUCCESS : WICED_BT_ERROR;

    switch ( xVariant )
    {
        case eBTsspVariantPasskeyConfirmation:
            wiced_bt_dev_confirm_req_reply(ucStatus, (uint8_t *) ( pxBdAddr->ucAddress ));
            break;
        case eBTsspVariantPasskeyEntry:
            wiced_bt_dev_pass_key_req_reply( ucStatus, (uint8_t *) ( pxBdAddr->ucAddress ), ulPasskey );
            break;
        case eBTsspVariantConsent:
            wiced_bt_ble_security_grant( (uint8_t *) ( pxBdAddr->ucAddress ), ucStatus );
            break;
        case eBTsspVariantPasskeyNotification:
            break;

        default:
            break;
    }

    return eBTStatusSuccess;
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
/* RSSI report handler */
static void prvRssiCb( void *pxRssi )
{
    BTStatus_t xStatus = eBTStatusFail;
    wiced_bt_dev_rssi_result_t *pxRssiRsult = (wiced_bt_dev_rssi_result_t*) pxRssi;

    if ( ( pxRssiRsult->status == WICED_BT_SUCCESS ) && ( pxRssiRsult->hci_status == HCI_SUCCESS ) )
    {
        xStatus = eBTStatusSuccess;
    }
    else
    {
        xStatus = prvConvertWicedErrorToAfr(pxRssiRsult->status);
    }

    if ( xBTCallbacks.pxReadRssiCb )
        xBTCallbacks.pxReadRssiCb( (BTBdaddr_t *) pxRssiRsult->rem_bda, pxRssiRsult->rssi, xStatus );
}

BTStatus_t prvBTReadRssi( const BTBdaddr_t * pxBdAddr )
{
    wiced_result_t xWicedStatus;

    xWicedStatus = wiced_bt_dev_read_rssi((uint8_t *) ( pxBdAddr->ucAddress ), BT_TRANSPORT_BR_EDR, prvRssiCb);

    return prvConvertWicedErrorToAfr(xWicedStatus);
}

/*-----------------------------------------------------------*/
/* TX Power report handler */
static void prvTxPowerCb( wiced_bt_tx_power_result_t *pxTxPower )
{
    BTStatus_t xStatus = eBTStatusFail;

    //@TODO: change the implementation to support classic as well (i.e, cache this when prvBTGetTxpower is invoked and give to the callback here)
    BTTransport_t xTransport = BTTransportLe;

    if ( ( pxTxPower->status == WICED_BT_SUCCESS ) && ( pxTxPower->hci_status == HCI_SUCCESS ) )
    {
        xStatus = eBTStatusSuccess;
    }
    else
    {
        xStatus = prvConvertWicedErrorToAfr(pxTxPower->status);
    }

    if ( xBTCallbacks.pxTxPowerCb )
        xBTCallbacks.pxTxPowerCb( (BTBdaddr_t *) pxTxPower->rem_bda, xTransport, pxTxPower->tx_power, xStatus );
}

BTStatus_t prvBTGetTxpower( const BTBdaddr_t * pxBdAddr,
                            BTTransport_t xTransport )
{
    wiced_bt_transport_t xWicedTransport = 0;

    if ( BTTransportBredr == xTransport )
    {
        xWicedTransport = BT_TRANSPORT_BR_EDR;
    }
    else if ( BTTransportLe == xTransport )
    {
        xWicedTransport = BT_TRANSPORT_LE;
    }

    if ( xTransport )
    {
        wiced_bt_dev_read_tx_power( (uint8_t *) ( pxBdAddr->ucAddress ), xWicedTransport, (wiced_bt_dev_cmpl_cback_t *) prvTxPowerCb );
        return eBTStatusSuccess;
    }
    else
    {
        return eBTStatusFail;
    }
}

/*-----------------------------------------------------------*/

const void * prvGetClassicAdapter()
{
    // return &xBTClassicAdapter;
    return NULL;
}

/*-----------------------------------------------------------*/

const BTInterface_t * BTGetBluetoothInterface()
{
    return &xBTinterface;
}

/*
 * Function     platform_bt_dev_property_interface_register
 *
 * Platform Bluetooth device property interaface registration API
 *
 * param:   platform_bt_dev_property_interface_t* : Pointer to platform device property interface APIs
 * return:  Always returns WICED_TRUE. Platform can pass a NULL Pointer to un-register the device property interface
 */
wiced_result_t platform_bt_dev_property_interface_register(platform_bt_dev_property_interface_t *p_bt_dev_property)
{
    pxPlatformDevPropInterface = p_bt_dev_property;
    return WICED_SUCCESS;
}

/*
 * Function     platform_bt_nvram_interface_register
 *
 * Platform NVRAM read/modify/write interaface registration API
 *
 * param:   platform_bt_nvram_interface_t* : Pointer to platform NVRAM interface APIs
 * return:  Always returns WICED_TRUE. Platform can pass a NULL Pointer to un-register the NVRAM interface
 */
wiced_result_t platform_bt_nvram_interface_register(platform_bt_nvram_interface_t *pxPlatformNvRam)
{
    pxWicedNvramInterface = pxPlatformNvRam;
    return WICED_SUCCESS;
}

uint32_t prvBTGetLastError( void )
{
    return 0;
}

BTStatus_t prvBTGetStackFeaturesSupport( uint32_t * pulFeatureMask )
{
    return eBTStatusUnsupported;
}
