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
#include "aws_ble_event_manager.h"
#include "aws_ble_hal_common.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble_hal_internals.h"

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/




uint8_t bdaddr[] = { 0x12, 0x34, 0x00, 0xE1, 0x80, 0x02 };

LocalName_t xLocalName;
// RR
//uint8_t xAuthReq = NO_BONDING;
//uint8_t xIocap = IO_CAP_NO_INPUT_NO_OUTPUT;
uint8_t xAuthReq;
uint8_t xIocap;
// RR

uint16_t service_handle = 0;
uint16_t dev_name_char_handle = 0;
uint16_t appearance_char_handle = 0;
uint16_t connection_handle = 0;

// RR - dafault settings
BTProperties_t xProperties =
{
		.xPropertyIO = eBTIODisplayYesNo,
		.ulMtu = 220,
		.bSecureConnectionOnly = true,
		.bBondable = true
};
// RR
uint32_t ulGAPEvtMngHandle;
BTCallbacks_t xBTCallbacks;

BTStatus_t prvBTManagerInit(const BTCallbacks_t * pxCallbacks);
BTStatus_t prvBTEnable(uint8_t ucGuestMode);
BTStatus_t prvBTDisable();
BTStatus_t prvBTGetAllDeviceProperties();
BTStatus_t prvBTGetDeviceProperty(BTPropertyType_t xType);
BTStatus_t prvBTSetDeviceProperty(const BTProperty_t *pxProperty);
BTStatus_t prvBTGetAllRemoteDeviceProperties(BTBdaddr_t *pxRemoteAddr);
BTStatus_t prvBTGetRemoteDeviceProperty(BTBdaddr_t *pxRemoteAddr, BTPropertyType_t type);
BTStatus_t prvBTSetRemoteDeviceProperty(BTBdaddr_t *pxRemoteAddr, const BTProperty_t *property);
BTStatus_t prvBTPair(const BTBdaddr_t * pxBdAddr, BTTransport_t xTransport, bool bCreateBond);
BTStatus_t prvBTCreateBondOutOfBand(const BTBdaddr_t * pxBdAddr, BTTransport_t xTransport, const BTOutOfBandData_t * pxOobData);
BTStatus_t prvBTCancelBond(const BTBdaddr_t *pxBdAddr);
BTStatus_t prvBTRemoveBond(const BTBdaddr_t *pxBdAddr);
BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr , bool * bConnectionState );
BTStatus_t prvBTPinReply(const BTBdaddr_t * pxBdAddr, uint8_t ucAccept, uint8_t ucPinLen, BTPinCode_t * pxPinCode);
BTStatus_t prvBTSspReply(const BTBdaddr_t * pxBdAddr, BTSspVariant_t xVariant, uint8_t ucAccept, uint32_t ulPasskey);
BTStatus_t prvBTReadEnergyInfo();
BTStatus_t prvBTDutModeConfigure(bool bEnable);
BTStatus_t prvBTDutModeSend(uint16_t usOpcode, uint8_t * pucBuf, size_t xLen);
BTStatus_t prvBTLeTestMode(uint16_t usOpcode, uint8_t * pucBuf, size_t xLen);
BTStatus_t prvBTConfigHCISnoopLog(bool bEnable);
BTStatus_t prvBTConfigClear();
BTStatus_t prvBTReadRssi(const BTBdaddr_t *pxBdAddr);
BTStatus_t prvBTGetTxpower(const BTBdaddr_t * pxBdAddr, BTTransport_t xTransport);
const void * prvGetClassicAdapter();
const void * prvGetLeAdapter();

static BTInterface_t xBTinterface = {
		.pxBtManagerInit = prvBTManagerInit,
		.pxEnable = prvBTEnable,
		.pxDisable = prvBTDisable,
		.pxGetAllDeviceProperties = prvBTGetAllDeviceProperties,
		.pxGetDeviceProperty = prvBTGetDeviceProperty,
		.pxSetDeviceProperty = prvBTSetDeviceProperty,
		.pxGetAllRemoteDeviceProperties = prvBTGetAllRemoteDeviceProperties,
		.pxGetRemoteDeviceProperty = prvBTGetRemoteDeviceProperty,
		.pxSetRemoteDeviceProperty = prvBTSetRemoteDeviceProperty,
		.pxPair = prvBTPair,
		.pxCreateBondOutOfBand = prvBTCreateBondOutOfBand,
		.pxCancelBond = prvBTCancelBond,
		.pxRemoveBond = prvBTRemoveBond,
		.pxGetConnectionState = prvBTGetConnectionState,
		.pxPinReply = prvBTPinReply,
		.pxSspReply = prvBTSspReply,
		.pxReadEnergyInfo = prvBTReadEnergyInfo,
		.pxDutModeConfigure = prvBTDutModeConfigure,
		.pxDutModeSend = prvBTDutModeSend,
		.pxLeTestMode = prvBTLeTestMode,
		.pxConfigHCISnoopLog = prvBTConfigHCISnoopLog,
		.pxConfigClear = prvBTConfigClear,
		.pxReadRssi = prvBTReadRssi,
		.pxGetTxpower = prvBTGetTxpower,
		.pxGetClassicAdapter = prvGetClassicAdapter,
		.pxGetLeAdapter = prvGetLeAdapter,
};

//static void prvGAPeventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
//	void * ppvEventParams = NULL;
//	bool bFoundEvent;
//	esp_err_t xESPStatus;
//	BTStatus_t xStatus = eBTStatusSuccess;
//	BTSecurityLevel_t xSecurityLevel;
//
//	vEVTMNGRgetEventParameters(ulGAPEvtMngHandle, event, &ppvEventParams, &bFoundEvent);
//
//	configPRINTF(( "GAP EVent %d\n", event));
//
//	switch (event) {
//	case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
//		if (xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL) {
//			if (param->adv_data_cmpl.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//
//			xBTBleAdapterCallbacks.pxSetAdvDataCb(xStatus);
//		}
//		break;
//	case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
//		if (xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL) {
//			if (param->scan_rsp_data_cmpl.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xBTBleAdapterCallbacks.pxSetAdvDataCb(xStatus);
//		}
//		break;
//	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
//		if (xBTBleAdapterCallbacks.pxAdvStartCb != NULL) {
//			if (param->adv_start_cmpl.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xBTBleAdapterCallbacks.pxAdvStartCb(xStatus, ulGattServerIFhandle);
//		}
//		break;
//	case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
//		if (param->adv_stop_cmpl.status != ESP_OK) {
//			configPRINTF(("Failed to stop advertisement\n"));
//		}
//		break;
//	case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
//		if (xBTBleAdapterCallbacks.pxConnParameterUpdateRequestCb != NULL) {
//			if (param->update_conn_params.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xBTBleAdapterCallbacks.pxConnParameterUpdateRequestCb(xStatus, (BTBdaddr_t *) &param->update_conn_params.bda, param->update_conn_params.min_int, param->update_conn_params.max_int, param->update_conn_params.latency, param->update_conn_params.conn_int, param->update_conn_params.timeout);
//		}
//		break;
//
//	case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:
//		if (param->remove_bond_dev_cmpl.status != ESP_OK) {
//			xStatus = eBTStatusFail;
//		}
//
//		xBTCallbacks.pxBondedCb(xStatus, (BTBdaddr_t *) &param->remove_bond_dev_cmpl.bd_addr,
//		false);
//
//		break;
//		/*
//		 case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT:
//		 case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:*/
//	case ESP_GAP_BLE_AUTH_CMPL_EVT:
//		if (param->ble_security.auth_cmpl.success == true) {
//			xStatus = eBTStatusSuccess;
//			xSecurityLevel = eBTSecLevelUnauthenticatedPairing;
//			if (xProperties.bBondable == true) {
//				xBTCallbacks.pxBondedCb(xStatus, (BTBdaddr_t *) &param->remove_bond_dev_cmpl.bd_addr,
//				true);
//			}
//		} else {
//			xStatus = eBTStatusFail;
//			xSecurityLevel = eBTSecLevelNoSecurity;
//		}
//
//		xBTCallbacks.pxPairingStateChangedCb(xStatus, (BTBdaddr_t *) &param->ble_security.auth_cmpl.bd_addr, xSecurityLevel, 0);
//
//		break;
//	case ESP_GAP_BLE_KEY_EVT:
//		break;
//	case ESP_GAP_BLE_SEC_REQ_EVT:
//		xBTCallbacks.pxSspRequestCb((BTBdaddr_t *) &param->ble_security.ble_req.bd_addr,
//		NULL, 0, eBTsspVariantConsent, 0);
//		break;
//	default:
//		break;
//	}
//
//	if (ppvEventParams != NULL) {
//		(void) vPortFree(ppvEventParams);
//	}

//}

// RR 11/21
void aci_gap_numeric_comparison_value_event(uint16_t Connection_Handle, uint32_t Numeric_Value) {
    // here I am replying directly with the aci_gap_numeric_comparison_value_confirm_yesno with a fixed 0x01 without checking on the Numeric Value
	xBTCallbacks.pxSspRequestCb(&btadd, NULL, 0, eBTsspVariantPasskeyConfirmation, Numeric_Value);
}
// end RR 11/21

// RR 11/21
void aci_gap_pairing_complete_event(uint16_t Connection_Handle, uint8_t Status, uint8_t Reason){

	//HB need to implement this
	//xBTCallbacks.pxPairingStateChangedCb(Status, )
	// if you want, we can trigger a callback and in the call back check the bonded device(s) through the stack api:
	// tBleStatus aci_gap_get_bonded_devices(uint8_t *Num_of_Addresses, Bonded_Device_Entry_t Bonded_Device_Entry[])
}
// end RR 11/21

// RR 11/21
void hci_encryption_change_event(uint8_t Status, uint16_t Connection_Handle, uint8_t Encryption_Enabled){

	// nothing to here.
}
// end RR 11/21

// RR 11/21
void aci_gap_slave_security_initiated_event(void){

	// nothing to here.
}
// end RR 11/21

BTStatus_t prvBTManagerInit(const BTCallbacks_t * pxCallbacks) {
	BTStatus_t xStatus = eBTStatusSuccess;

	if (pxCallbacks != NULL) {
		xBTCallbacks = *pxCallbacks;
	} else {
		xStatus = eBTStatusFail;
	}

	return xStatus;
}


BTStatus_t prvBTEnable(uint8_t ucGuestMode) {

	BlueNRG_RST();
	
	HAL_Delay(100);

	for (;;) {

		/* Set the device public address */
		if (aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr))
			break;

		/* Set the TX power -2 dBm */
		if (aci_hal_set_tx_power_level(1, 4))
			break;

		/* GATT Init */
		if (aci_gatt_init())
			break;

		/* GAP Init */
		if (aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle))
			break;

		/* Update device name */
		if (aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, xLocalName.local_name_size, (uint8_t*) xLocalName.local_name))
			break;


		if(xBTCallbacks.pxDeviceStateChangedCb != NULL)
		{
			xBTCallbacks.pxDeviceStateChangedCb( eBTstateOn );
		}


		return eBTStatusSuccess;

	}

	return eBTStatusFail;

}

BTStatus_t prvBTDisable() {
	BlueNRG_Power_Down();
	return eBTStatusSuccess;
}

BTStatus_t prvBTGetAllDeviceProperties() {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTGetDeviceProperty(BTPropertyType_t xType) {
	BTStatus_t xStatus = eBTStatusSuccess;
	BTProperty_t xReturnedProperty;

	if (xBTCallbacks.pxAdapterPropertiesCb != NULL) {
		xReturnedProperty.xType = xType;

		switch (xType) {
		case eBTpropertySecureConnectionOnly:
        	xReturnedProperty.xLen = sizeof(bool);
        	xReturnedProperty.xType = eBTpropertySecureConnectionOnly;
        	xReturnedProperty.pvVal = (void *)&xProperties.bSecureConnectionOnly;
        	xBTCallbacks.pxAdapterPropertiesCb(eBTStatusSuccess, 1, &xReturnedProperty);
			break;
		case eBTpropertyBdname:
			xReturnedProperty.pvVal = pvPortMalloc(xLocalName.local_name_size + 1);
			memcpy(xReturnedProperty.pvVal, xLocalName.local_name, xLocalName.local_name_size);
			xReturnedProperty.xLen = xLocalName.local_name_size;
			xStatus = eBTStatusSuccess;
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, &xReturnedProperty);
			break;
		case eBTpropertyBdaddr:
			xStatus = eBTStatusUnsupported;
			break;
		case eBTpropertyTypeOfDevice:
			xStatus = eBTStatusUnsupported;
			break;
		case eBTpropertyLocalMTUSize:
			xReturnedProperty.pvVal = (void*) xProperties.ulMtu;
			xReturnedProperty.xLen = 1;
			xStatus = eBTStatusSuccess;
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, &xReturnedProperty);
			break;
		case eBTpropertyBondable:
			switch (xAuthReq) {
				case BONDING:
					xReturnedProperty.pvVal = (void*) true;
					break;
				case NO_BONDING:
				default:
					xReturnedProperty.pvVal = (void*) false;
					break;
			}
			xReturnedProperty.xLen = 1;
			xStatus = eBTStatusSuccess;
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, &xReturnedProperty);
			break;
		case eBTpropertyIO:
			switch (xIocap) {
			case IO_CAP_DISPLAY_ONLY:
				xReturnedProperty.pvVal = (void*) eBTIODisplayOnly;
				break;
			case IO_CAP_DISPLAY_YES_NO:
				xReturnedProperty.pvVal = (void*) eBTIODisplayYesNo;
				break;
			case IO_CAP_KEYBOARD_ONLY:
				xReturnedProperty.pvVal = (void*) eBTIOKeyboardOnly;
				break;
			case IO_CAP_KEYBOARD_DISPLAY:
				xReturnedProperty.pvVal = (void*) eBTIOKeyboardDisplay;
				break;
			default:
				xReturnedProperty.pvVal = (void*) eBTIONone;
				break;
			}
			xReturnedProperty.xLen = 1;
			xStatus = eBTStatusSuccess;
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, &xReturnedProperty);
			break;
		default:
			xStatus = eBTStatusUnsupported;
		}
	}

	return xStatus;
}

BTStatus_t prvBTSetDeviceProperty(const BTProperty_t *pxProperty) {
	BTStatus_t xStatus = eBTStatusSuccess;

	uint8_t ret = BLE_STATUS_SUCCESS;

	switch (pxProperty->xType) {
	case eBTpropertyBdname:
		xLocalName.local_name = pvPortMalloc(pxProperty->xLen + 2);
		if (xLocalName.local_name != NULL) {
			memcpy(&xLocalName.local_name[1], pxProperty->pvVal, pxProperty->xLen);
			xLocalName.local_name[0] = AD_TYPE_COMPLETE_LOCAL_NAME;
			xLocalName.local_name[pxProperty->xLen + 1] = '\0';
			xLocalName.local_name_size = pxProperty->xLen + 1;
			ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, xLocalName.local_name_size, (uint8_t*) xLocalName.local_name);
			if (ret != BLE_STATUS_SUCCESS) {
				xStatus = eBTStatusFail;
			}
		} else {
			xStatus = eBTStatusNoMem;
		}
		if ((xBTCallbacks.pxAdapterPropertiesCb != NULL) && (xStatus == eBTStatusSuccess)) {
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, (BTProperty_t *) pxProperty);
		}
		break;

	case eBTpropertyBdaddr:
		xStatus = eBTStatusUnsupported;
		break;

	case eBTpropertyTypeOfDevice:
		xStatus = eBTStatusUnsupported;
		break;

	case eBTpropertyBondable:
		xProperties.bBondable = *((bool *) pxProperty->pvVal);
		if (*((bool *) pxProperty->pvVal) == true) {
			xAuthReq = BONDING;
		}

		ret = aci_gap_set_authentication_requirement(xAuthReq, MITM_PROTECTION_REQUIRED, SC_IS_SUPPORTED, KEYPRESS_IS_NOT_SUPPORTED, 7, 16, USE_FIXED_PIN_FOR_PAIRING, 123456, 0x00);
		if (ret != BLE_STATUS_SUCCESS) {
			xStatus = eBTStatusFail;
		}
		if ((xBTCallbacks.pxAdapterPropertiesCb != NULL) && (xStatus == eBTStatusSuccess)) {
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, (BTProperty_t *) pxProperty);
		}
		break;

	case eBTpropertyLocalMTUSize:
//		xProperties.ulMtu = *((uint32_t *) pxProperty->pvVal);
//		ret = hci_le_set_data_length(connection_handle, xProperties.ulMtu, ((xProperties.ulMtu) + 14) * 8);
//		if (ret != BLE_STATUS_SUCCESS) {
//			xStatus = eBTStatusFail;
//		}
//
//		if ((xBTCallbacks.pxAdapterPropertiesCb != NULL) && (xStatus == eBTStatusSuccess)) {
//			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, (BTProperty_t *) pxProperty);
//		}
//		break;

// RR
		xProperties.ulMtu = *((uint32_t *) pxProperty->pvVal);
		if (xProperties.ulMtu!=220) {
			xProperties.ulMtu = 220; // 220 is the default MTU size - hardcoded in the stack
			configPRINT_STRING("prvBTSetDeviceProperty / eBTpropertyLocalMTUSize: 220 bytes is the default MTU size - hard-coded in the stack\r\n");
			xStatus = eBTStatusSuccess;
		}

		if ((xBTCallbacks.pxAdapterPropertiesCb != NULL) && (xStatus == eBTStatusSuccess)) {
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, (BTProperty_t *) pxProperty);
		}
		break;
// RR

	case eBTpropertyIO:
		xProperties.xPropertyIO = *((BTIOtypes_t *) pxProperty->pvVal);
		switch (*((BTIOtypes_t *) pxProperty->pvVal)) {
		case eBTIODisplayOnly:
			xIocap = IO_CAP_DISPLAY_ONLY;
			break;
		case eBTIODisplayYesNo:
			xIocap = IO_CAP_DISPLAY_YES_NO;
			break;
		case eBTIOKeyboardOnly:
			xIocap = IO_CAP_KEYBOARD_ONLY;
			break;
		case eBTIOKeyboardDisplay:
			xIocap = IO_CAP_KEYBOARD_DISPLAY;
			break;
		default:
			// RR
			if (xProperties.bSecureConnectionOnly == true)  {
				configPRINT_STRING(("prvBTSetDeviceProperty / eBTpropertyIO: WARNING!!! eBTpropertySecureConnectionOnly is true and setting eBTpropertyIO property as eBTIONone will result in pairing failure!\r\n"));
			}
			// RR
			xIocap = IO_CAP_NO_INPUT_NO_OUTPUT;
			break;
		}
		xProperties.xPropertyIO = xIocap;
		ret = aci_gap_set_io_capability(xIocap);
		if (ret != BLE_STATUS_SUCCESS) {
			xStatus = eBTStatusFail;
		}

		if ((xBTCallbacks.pxAdapterPropertiesCb != NULL) && (xStatus == eBTStatusSuccess)) {
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, (BTProperty_t *) pxProperty);
		}
		break;
	case eBTpropertySecureConnectionOnly:
		xProperties.bSecureConnectionOnly = *(( bool *) pxProperty->pvVal); /* update flag */
// RR
		if (xProperties.bSecureConnectionOnly){
			if ((xProperties.xPropertyIO != eBTIODisplayYesNo) && (xProperties.xPropertyIO != eBTIOKeyboardDisplay)) {
				configPRINT_STRING(("prvBTSetDeviceProperty / bSecureConnectionOnly: Input/output property are incompatible with secure connection only Mode\r\n"));
				xStatus = eBTStatusFail;
			} else{
				ret = aci_gap_set_authentication_requirement(xProperties.bBondable, MITM_PROTECTION_REQUIRED, SC_IS_MANDATORY, KEYPRESS_IS_NOT_SUPPORTED, 7, 16, USE_FIXED_PIN_FOR_PAIRING, 123456, 0x00);
				if (ret != BLE_STATUS_SUCCESS) {
					xStatus = eBTStatusFail;
				}
			}
		}
		else{
			ret = aci_gap_set_authentication_requirement(xProperties.bBondable, MITM_PROTECTION_REQUIRED, SC_IS_SUPPORTED, KEYPRESS_IS_NOT_SUPPORTED, 7, 16, USE_FIXED_PIN_FOR_PAIRING, 123456, 0x00);
			if (ret != BLE_STATUS_SUCCESS) {
				xStatus = eBTStatusFail;
			}
		}
// RR
		xStatus = prvToggleSecureConnectionOnlyMode(xProperties.bSecureConnectionOnly);
		if ((xBTCallbacks.pxAdapterPropertiesCb != NULL) && (xStatus == eBTStatusSuccess)) {
			xBTCallbacks.pxAdapterPropertiesCb(xStatus, 1, (BTProperty_t *) pxProperty);
		}
		break;
	default:
		xStatus = eBTStatusUnsupported;
		break;
	}

	return xStatus;

}

BTStatus_t prvBTGetAllRemoteDeviceProperties(BTBdaddr_t *pxRemoteAddr) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTGetRemoteDeviceProperty(BTBdaddr_t *pxRemoteAddr, BTPropertyType_t type) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTSetRemoteDeviceProperty(BTBdaddr_t *pxRemoteAddr, const BTProperty_t *property) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTPair(const BTBdaddr_t * pxBdAddr, BTTransport_t xTransport, bool bCreateBond) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTCreateBondOutOfBand(const BTBdaddr_t * pxBdAddr, BTTransport_t xTransport, const BTOutOfBandData_t * pxOobData) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTCancelBond(const BTBdaddr_t *pxBdAddr) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTRemoveBond(const BTBdaddr_t *pxBdAddr) {
	uint8_t ret = BLE_STATUS_SUCCESS;
	ret = aci_gap_remove_bonded_device(0x00, (uint8_t *) pxBdAddr->ucAddress); // 0x00 Public - 0x01 Random Static
	return ret == BLE_STATUS_SUCCESS ? eBTStatusSuccess : eBTStatusFail;
}

BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr , bool * bConnectionState ) {
	return 0;
}

BTStatus_t prvBTPinReply(const BTBdaddr_t * pxBdAddr, uint8_t ucAccept, uint8_t ucPinLen, BTPinCode_t * pxPinCode) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTSspReply(const BTBdaddr_t * pxBdAddr, BTSspVariant_t xVariant, uint8_t ucAccept, uint32_t ulPasskey) {
	BTStatus_t xStatus = eBTStatusSuccess;
	uint8_t xBleStatus = BLE_STATUS_SUCCESS;

	if(ucAccept == true)
	{
		xBleStatus = aci_gap_numeric_comparison_value_confirm_yesno(connection_handle, 0x01);
	}else
	{
		xBleStatus = aci_gap_numeric_comparison_value_confirm_yesno(connection_handle, 0x00);
	}

	return (xBleStatus == BLE_STATUS_SUCCESS) ? eBTStatusSuccess : eBTStatusFail;
}

BTStatus_t prvBTReadEnergyInfo() {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTDutModeConfigure(bool bEnable) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTDutModeSend(uint16_t usOpcode, uint8_t * pucBuf, size_t xLen) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTLeTestMode(uint16_t usOpcode, uint8_t * pucBuf, size_t xLen) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTConfigHCISnoopLog(bool bEnable) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTConfigClear() {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTReadRssi(const BTBdaddr_t *pxBdAddr) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTGetTxpower(const BTBdaddr_t * pxBdAddr, BTTransport_t xTransport) {
	return eBTStatusUnsupported;
}

const void * prvGetClassicAdapter() {
	return NULL;
}

const BTInterface_t * BTGetBluetoothInterface() {
	return &xBTinterface;
}
