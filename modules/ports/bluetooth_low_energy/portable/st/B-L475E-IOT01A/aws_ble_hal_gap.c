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

#include "aws_ble_hal_common.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble_event_manager.h"
#include "aws_ble_hal_internals.h"
#include "aws_ble_event_manager.h"

typedef struct {

	BTAdvProperties_t usAdvertisingEventProperties;
	bool bIncludeTxPower;				/*!< Advertising data include TX power */
	bool bIncludeName;					/*!< Advertising data include device name or not */
	bool bSetScanRsp;					/*!< Set this advertising data as scan response or not*/
	uint32_t ulAppearance;				/*!< External appearance of device */
	uint32_t ulMinInterval; 			/*!< Advertising data show advertising min interval */
	uint32_t ulMaxInterval; 			/*!< Advertising data show advertising max interval */
	uint8_t ucChannelMap;
	uint8_t ucPrimaryAdvertisingPhy; 	/* 5.0 Specific interface */
	uint8_t ucSecondaryAdvertisingPhy; 	/* 5.0 Specific interface */
	BTAddrType_t xAddrType;

	uint16_t manufacturer_len; 			/*!< Manufacturer data length */
	uint8_t *p_manufacturer_data; 		/*!< Manufacturer data point */
	uint16_t service_data_len;			/*!< Service data length */
	uint8_t *p_service_data;			/*!< Service data point */
	uint16_t service_uuid_len; 			/*!< Service uuid length */
	uint8_t *p_service_uuid; 			/*!< Pointer to the service uuid array */
	uint8_t service_uuid_type;

} BlueNRG_ble_adv_data_t;

BlueNRG_ble_adv_data_t xBle_adv_data =
{
		.usAdvertisingEventProperties = BTAdvInd,
		.ulMinInterval = 1000,
		.ulMaxInterval = 1200,
		.bSetScanRsp = false,
		.bIncludeTxPower = false
};

extern LocalName_t xLocalName;

UUID_t BlueNRG_UUID;

// RR
extern BTProperties_t xProperties;
// RR

extern uint16_t service_handle;
extern uint16_t dev_name_char_handle;
extern uint16_t appearance_char_handle;
extern uint16_t connection_handle;

void prvCopytoBlueNRGUUID(UUID_t * pxBlueNRGuuid, BTUuid_t * pxUuid);


BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;

static BTStatus_t prvBTBleAdapterInit(const BTBleAdapterCallbacks_t * pxCallbacks);
static BTStatus_t prvBTRegisterBleApp(BTUuid_t * pxAppUuid);
static BTStatus_t prvBTUnregisterBleApp(uint8_t ucAdapterIf);
static BTStatus_t prvBTGetBleAdapterProperty(BTBlePropertyType_t xType);
static BTStatus_t prvBTSetBleAdapterProperty(const BTBleProperty_t * pxProperty);
static BTStatus_t prvBTGetallBleRemoteDeviceProperties(BTBdaddr_t * pxRremoteAddr);
static BTStatus_t prvBTGetBleRemoteDeviceProperty(BTBdaddr_t * pxRemoteAddr, BTBleProperty_t xType);
static BTStatus_t prvBTSetBleRemoteDeviceProperty(BTBdaddr_t * pxRemoteAddr, const BTBleProperty_t * pxProperty);
static BTStatus_t prvBTScan( bool bStart);
static BTStatus_t prvBTConnect(uint8_t ucAdapterIf, const BTBdaddr_t * pxBdAddr, bool bIsDirect, BTTransport_t ulTransport);
static BTStatus_t prvBTDisconnect(uint8_t ucAdapterIf, const BTBdaddr_t * pxBdAddr, uint16_t usConnId);
static BTStatus_t prvBTStartAdv(uint8_t ucAdapterIf);
static BTStatus_t prvBTStopAdv(uint8_t ucAdapterIf);
static BTStatus_t prvBTReadRemoteRssi(uint8_t ucAdapterIf, const BTBdaddr_t * pxBdAddr);
static BTStatus_t prvBTScanFilterParamSetup(BTGattFiltParamSetup_t xFiltParam);
static BTStatus_t prvBTScanFilterAddRemove(uint8_t ucAdapterIf, uint32_t ulAction, uint32_t ulFiltType, uint32_t ulFiltIndex, uint32_t ulCompanyId, uint32_t ulCompanyIdMask, const BTUuid_t * pxUuid, const BTUuid_t * pxUuidMask, const BTBdaddr_t * pxBdAddr, char cAddrType, size_t xDataLen, char * pcData, size_t xMaskLen, char * pcMask);
static BTStatus_t prvBTScanFilterEnable(uint8_t ucAdapterIf, bool bEnable);
static BTTransport_t prvBTGetDeviceType(const BTBdaddr_t * pxBdAddr);
static BTStatus_t prvBTSetAdvData(uint8_t ucAdapterIf, BTGattAdvertismentParams_t * pxParams, uint16_t usManufacturerLen, char * pcManufacturerData, uint16_t usServiceDataLen, char * pcServiceData, BTUuid_t * pxServiceUuid, size_t xNbServices);
static BTStatus_t prvBTSetAdvRawData(uint8_t ucAdapterIf, uint8_t * pucData, uint8_t ucLen);
static BTStatus_t prvBTConnParameterUpdateRequest(const BTBdaddr_t * pxBdAddr, uint32_t ulMinInterval, uint32_t ulMaxInterval, uint32_t ulLatency, uint32_t ulTimeout);
static BTStatus_t prvBTSetScanParameters(uint8_t ucAdapterIf, uint32_t ulScanInterval, uint32_t ulScanWindow);
static BTStatus_t prvBTMultiAdvEnable(uint8_t ucAdapterIf, BTGattAdvertismentParams_t xAdvParams);
static BTStatus_t prvBTMultiAdvUpdate(uint8_t ucAdapterIf, BTGattAdvertismentParams_t advParams);
static BTStatus_t prvBTMultiAdvSetInstData(uint8_t ucAdapterIf, bool bSetScanRsp, bool bIncludeName, bool bInclTxpower, uint32_t ulAppearance, size_t xManufacturerLen, char * pcManufacturerData, size_t xServiceDataLen, char * pcServiceData, BTUuid_t * pxServiceUuid, size_t xNbServices);
static BTStatus_t prvBTMultiAdvDisable(uint8_t ucAdapterIf);
static BTStatus_t prvBTBatchscanCfgStorage(uint8_t ucAdapterIf, uint32_t ulBatchScanFullMax, uint32_t ulBatchScanTruncMax, uint32_t ulBatchScanNotifyThreshold);
static BTStatus_t prvBTBatchscanEndBatchScan(uint8_t ucAdapterIf, uint32_t ulScanMode, uint32_t ulScanInterval, uint32_t ulScanWindow, uint32_t ulAddrType, uint32_t ulDiscardRule);
static BTStatus_t prvBTBatchscanDisBatchScan(uint8_t ucAdapterIf);
static BTStatus_t prvBTBatchscanReadReports(uint8_t ucAdapterIf, uint32_t ulScanMode);
static BTStatus_t prvBTSetPreferredPhy(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, uint16_t usPhyOptions);
static BTStatus_t prvBTReadPhy(uint16_t usConnId, BTReadClientPhyCallback_t xCb);
static const void * prvBTGetGattClientInterface(void);
static BTStatus_t prvBTBleAdapterInit(const BTBleAdapterCallbacks_t * pxCallbacks);

BTBleAdapter_t xBTLeAdapter = {
		.pxBleAdapterInit = prvBTBleAdapterInit,
		.pxRegisterBleApp = prvBTRegisterBleApp,
		.pxUnregisterBleApp = prvBTUnregisterBleApp,
		.pxGetBleAdapterProperty = prvBTGetBleAdapterProperty,
		.pxSetBleAdapterProperty = prvBTSetBleAdapterProperty,
		.pxGetallBleRemoteDeviceProperties = prvBTGetallBleRemoteDeviceProperties,
		.pxGetBleRemoteDeviceProperty =	prvBTGetBleRemoteDeviceProperty,
		.pxSetBleRemoteDeviceProperty = prvBTSetBleRemoteDeviceProperty,
		.pxScan = prvBTScan,
		.pxConnect = prvBTConnect,
		.pxDisconnect = prvBTDisconnect,
		.pxStartAdv = prvBTStartAdv,
		.pxStopAdv = prvBTStopAdv,
		.pxReadRemoteRssi = prvBTReadRemoteRssi,
		.pxScanFilterParamSetup = prvBTScanFilterParamSetup,
		.pxScanFilterAddRemove = prvBTScanFilterAddRemove,
		.pxScanFilterEnable = prvBTScanFilterEnable,
		.pxGetDeviceType = prvBTGetDeviceType,
		.pxSetAdvData = prvBTSetAdvData,
		.pxSetAdvRawData = prvBTSetAdvRawData,
		.pxConnParameterUpdateRequest = prvBTConnParameterUpdateRequest,
		.pxSetScanParameters = prvBTSetScanParameters,
		.pxMultiAdvEnable = prvBTMultiAdvEnable,
		.pxMultiAdvUpdate = prvBTMultiAdvUpdate,
		.pxMultiAdvSetInstData = prvBTMultiAdvSetInstData,
		.pxMultiAdvDisable = prvBTMultiAdvDisable,
		.pxBatchscanCfgStorage = prvBTBatchscanCfgStorage,
		.pxBatchscanEndBatchScan = prvBTBatchscanEndBatchScan,
		.pxBatchscanDisBatchScan = prvBTBatchscanDisBatchScan,
		.pxBatchscanReadReports = prvBTBatchscanReadReports,
		.pxSetPreferredPhy = prvBTSetPreferredPhy,
		.pxReadPhy = prvBTReadPhy,
		.ppvGetGattClientInterface = prvBTGetGattClientInterface,
		.ppvGetGattServerInterface = prvBTGetGattServerInterface,
};

void prvCopytoBlueNRGUUID(UUID_t * pxBlueNRGuuid, BTUuid_t * pxUuid) {
	switch (pxUuid->ucType) {
	case eBTuuidType16:
		pxBlueNRGuuid->UUID_16 = pxUuid->uu.uu16;
		break;
	case eBTuuidType128:
	default:
		Osal_MemCpy(&pxBlueNRGuuid->UUID_128, pxUuid->uu.uu128, 16);
		break;
	}
}

BTStatus_t prvToggleSecureConnectionOnlyMode(bool bEnable) {

	BTStatus_t xStatus = eBTStatusSuccess;
//
//	esp_err_t xESPstatus;
//
//	if (bEnable == true) {
//		xAuthOption = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE;
//		xESPstatus = esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &xAuthOption, sizeof(uint8_t));
//
//		aci_gap_set_authentication_requirement()
//		/* Check the IO are compatible with the desired security level */
//		if ((xProperties.xPropertyIO != eBTIODisplayYesNo) && (xProperties.xPropertyIO != eBTIOKeyboardDisplay)) {
//			configPRINTF(("BLE: Input/output property are incompatible with secure connection only Mode\n"));
//		}
//	} else {
//		xAuthOption = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
//		xESPstatus = esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &xAuthOption, sizeof(uint8_t));
//	}
//
//	if (xESPstatus != ESP_OK) {
//		xStatus = eBTStatusFail;
//	}

	return xStatus;
}

BTStatus_t prvBTBleAdapterInit(const BTBleAdapterCallbacks_t * pxCallbacks) {
	BTStatus_t xStatus = eBTStatusSuccess;

	// RR
	xProperties.bBondable = true;
	xProperties.bSecureConnectionOnly = true;
	xProperties.xPropertyIO = eBTIODisplayYesNo;
	// RR

	if (pxCallbacks != NULL) {
		xBTBleAdapterCallbacks = *pxCallbacks;
	} else {
		xStatus = eBTStatusFail;
	}

	return xStatus;
}

BTStatus_t prvBTRegisterBleApp(BTUuid_t * pxAppUuid) {
	BTStatus_t xStatus = eBTStatusSuccess;

	xBTBleAdapterCallbacks.pxRegisterBleAdapterCb(eBTStatusSuccess, 0, pxAppUuid);

	return xStatus;
}

BTStatus_t prvBTUnregisterBleApp(uint8_t ucAdapterIf) {
	BTStatus_t xStatus = eBTStatusSuccess;

	return xStatus;
}

BTStatus_t prvBTGetBleAdapterProperty(BTBlePropertyType_t xType) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTSetBleAdapterProperty(const BTBleProperty_t * pxProperty) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTGetallBleRemoteDeviceProperties(BTBdaddr_t * pxRremoteAddr) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTGetBleRemoteDeviceProperty(BTBdaddr_t * pxRemoteAddr, BTBleProperty_t xType) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTSetBleRemoteDeviceProperty(BTBdaddr_t * pxRemoteAddr, const BTBleProperty_t * pxProperty) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTScan( bool bStart) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTConnect(uint8_t ucAdapterIf, const BTBdaddr_t * pxBdAddr, bool bIsDirect, BTTransport_t ulTransport) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTDisconnect(uint8_t ucAdapterIf, const BTBdaddr_t * pxBdAddr, uint16_t usConnId) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

char HardCode[] = {0x02, 0x01, 0x06, 0x06, 0x09, 0x45, 0x53, 0x50, 0x33, 0x32, 0x10, 0x00, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A, 0x02, 0x0A, 0xFE, 0x00}; // AWS
BTStatus_t prvBTStartAdv(uint8_t ucAdapterIf) {
	tBleStatus ret = BLE_STATUS_SUCCESS;
	uint8_t adv_type;
	uint8_t own_address_type;
	BTStatus_t xStatus = eBTStatusSuccess;

	// Select advertising property
	switch (xBle_adv_data.usAdvertisingEventProperties) {
	case BTAdvInd:
		adv_type = ADV_IND;
		break;
	case BTAdvDirectInd:
		adv_type = ADV_DIRECT_IND;
		break;
	case BTAdvNonconnInd:
		adv_type = ADV_NONCONN_IND;
		break;
	default:
		adv_type = ADV_IND;
		break;
	}

	// Select own address type
	switch (xBle_adv_data.xAddrType) {
	case BTAddrTypePublic:
		own_address_type = PUBLIC_ADDR;
		break;
	case BTAddrTypeRandom:
		own_address_type = RANDOM_ADDR;
		break;
	case BTAddrTypeStaticRandom:
		own_address_type = STATIC_RANDOM_ADDR;
		break;
	case BTAddrTypeResolvable:
		own_address_type = RESOLVABLE_PRIVATE_ADDR;
		break;
	default:
		own_address_type = STATIC_RANDOM_ADDR;
		break;
	}

	
	// Start advertising
	ret += aci_gap_set_discoverable(adv_type,(xBle_adv_data.ulMinInterval * 1000) / 625, (xBle_adv_data.ulMaxInterval * 1000) / 625, own_address_type, NO_WHITE_LIST_USE, 0, NULL, 0, NULL, 0, 0);

	// Remove TX power TODO: must be romeve
	ret += aci_gap_delete_ad_type(AD_TYPE_TX_POWER_LEVEL);

	uint8_t maxNumAdvByte = 0;
	uint8_t advData[28];
	uint8_t advFreeBytes = 28;

	uint8_t t[2] = {0,0};

	if (xBle_adv_data.service_uuid_len != 0) {
		if (xBle_adv_data.service_uuid_type==eBTuuidType16){
			t[0] = xBle_adv_data.service_uuid_len + 1;
			t[1] = AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST;
		}
		if (xBle_adv_data.service_uuid_type==eBTuuidType128){
			t[0] = xBle_adv_data.service_uuid_len + 1;
			t[1] = AD_TYPE_128_BIT_SERV_UUID_CMPLT_LIST;
		}
		memcpy(advData + maxNumAdvByte, t, 2);
		maxNumAdvByte += 2;
		advFreeBytes -= 2;
		memcpy(advData + maxNumAdvByte, xBle_adv_data.p_service_uuid, xBle_adv_data.service_uuid_len);
		maxNumAdvByte += xBle_adv_data.service_uuid_len;
		advFreeBytes -= xBle_adv_data.service_uuid_len;
	}

	if ((xBle_adv_data.manufacturer_len != 0) && ((xBle_adv_data.manufacturer_len+2) <= advFreeBytes)) {
		uint8_t t[2] = { xBle_adv_data.manufacturer_len + 1, AD_TYPE_MANUFACTURER_SPECIFIC_DATA };
		memcpy(advData + maxNumAdvByte, t, 2);
		maxNumAdvByte += 2;
		advFreeBytes -= 2;
		memcpy(advData + maxNumAdvByte, xBle_adv_data.p_manufacturer_data, xBle_adv_data.manufacturer_len);
		maxNumAdvByte += xBle_adv_data.manufacturer_len;
		advFreeBytes -= xBle_adv_data.manufacturer_len;
	}

	uint8_t advTxPower[3] = { 0x02, AD_TYPE_TX_POWER_LEVEL, 0x00 };

	// Add Tx Power
	if ((xBle_adv_data.bIncludeTxPower) && advFreeBytes >= 3) { // There is space for Tx power
		memcpy(advData + maxNumAdvByte, advTxPower, 3);
		maxNumAdvByte += 3;
		advFreeBytes -= 3;
	}

	if (xBle_adv_data.bIncludeName && (advFreeBytes >= (xLocalName.local_name_size+2))) {
		uint8_t t[2] = { xLocalName.local_name_size + 1, AD_TYPE_COMPLETE_LOCAL_NAME };
		memcpy(advData + maxNumAdvByte, t, 2);
		maxNumAdvByte += 2;
		advFreeBytes -= 2;
		memcpy(advData + maxNumAdvByte, xLocalName.local_name, xLocalName.local_name_size);
		maxNumAdvByte += xLocalName.local_name_size;
		advFreeBytes -= xLocalName.local_name_size;
	}

	if ((xBle_adv_data.service_data_len  != 0) && (advFreeBytes >= (xBle_adv_data.service_data_len+2))) {
			uint8_t t[2] = { xBle_adv_data.service_data_len + 1, AD_TYPE_SERVICE_DATA };
			memcpy(advData + maxNumAdvByte, t, 2);
			maxNumAdvByte += 2;
			advFreeBytes -= 2;
			memcpy(advData + maxNumAdvByte, xBle_adv_data.p_service_data, xBle_adv_data.service_data_len);
			maxNumAdvByte += xBle_adv_data.service_data_len;
			advFreeBytes -= xBle_adv_data.service_data_len;
		}

	ret += aci_gap_update_adv_data(maxNumAdvByte, advData);

	// Set scan response - if scan response is required, it will replicate exactly the ADV_IND packet except the AD Type Flags, otherwise it'll be null
	if (xBle_adv_data.bSetScanRsp)
		ret += hci_le_set_scan_response_data(maxNumAdvByte, advData);
	else
		ret += hci_le_set_scan_response_data(0, NULL);
		
	
	if (ret != BLE_STATUS_SUCCESS) {
		xStatus = eBTStatusFail;
	}

	if (xStatus == eBTStatusSuccess) {
		xBTBleAdapterCallbacks.pxAdvStartCb(xStatus, ulGattServerIFhandle);
	}

	return xStatus;
}

BTStatus_t prvBTStopAdv(uint8_t ucAdapterIf) {
	tBleStatus ret = BLE_STATUS_SUCCESS;
	// Stop Advertising if not connected and application wants to stop it
	if (connection_handle == 0)
		ret = aci_gap_set_non_discoverable();
	return ret == BLE_STATUS_SUCCESS ? eBTStatusSuccess : eBTStatusFail;

}

BTStatus_t prvBTReadRemoteRssi(uint8_t ucAdapterIf, const BTBdaddr_t * pxBdAddr) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTScanFilterParamSetup(BTGattFiltParamSetup_t xFiltParam) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTScanFilterAddRemove(uint8_t ucAdapterIf, uint32_t ulAction, uint32_t ulFiltType, uint32_t ulFiltIndex, uint32_t ulCompanyId, uint32_t ulCompanyIdMask, const BTUuid_t * pxUuid, const BTUuid_t * pxUuidMask, const BTBdaddr_t * pxBdAddr, char cAddrType, size_t xDataLen, char * pcData, size_t xMaskLen, char * pcMask) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTScanFilterEnable(uint8_t ucAdapterIf,bool bEnable) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTTransport_t prvBTGetDeviceType(const BTBdaddr_t * pxBdAddr) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTSetAdvData(uint8_t ucAdapterIf, BTGattAdvertismentParams_t * pxParams, uint16_t usManufacturerLen, char * pcManufacturerData, uint16_t usServiceDataLen, char * pcServiceData, BTUuid_t * pxServiceUuid, size_t xNbServices) {

	xBle_adv_data.ulAppearance = pxParams->ulAppearance;
	xBle_adv_data.bIncludeName = pxParams->bIncludeName;
	xBle_adv_data.ulMaxInterval = pxParams->ulMaxInterval;
	xBle_adv_data.ulMinInterval = pxParams->ulMinInterval;
	xBle_adv_data.bSetScanRsp = pxParams->bSetScanRsp;
	xBle_adv_data.usAdvertisingEventProperties = pxParams->usAdvertisingEventProperties;
	xBle_adv_data.bIncludeTxPower = pxParams->bIncludeTxPower;
	xBle_adv_data.xAddrType = pxParams->xAddrType;
	xBle_adv_data.p_manufacturer_data = (uint8_t *) pcManufacturerData;
	xBle_adv_data.manufacturer_len = usManufacturerLen;
	xBle_adv_data.p_service_data = (uint8_t *) pcServiceData;
	xBle_adv_data.service_data_len = usServiceDataLen;

	prvCopytoBlueNRGUUID(&BlueNRG_UUID, pxServiceUuid);

	if (pxServiceUuid->ucType == eBTuuidType16) {
		xBle_adv_data.p_service_uuid = (uint8_t*)(&BlueNRG_UUID.UUID_16);
		xBle_adv_data.service_uuid_len = 2;
	} else if (pxServiceUuid->ucType == eBTuuidType128){
		xBle_adv_data.p_service_uuid = BlueNRG_UUID.UUID_128;
		xBle_adv_data.service_uuid_len = 16;
	}
	xBle_adv_data.service_uuid_type = pxServiceUuid->ucType;

	xBTBleAdapterCallbacks.pxSetAdvDataCb(eBTStatusSuccess);

	return eBTStatusSuccess;
}

BTStatus_t prvBTSetAdvRawData(uint8_t ucAdapterIf, uint8_t * pucData, uint8_t ucLen) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTConnParameterUpdateRequest(const BTBdaddr_t * pxBdAddr, uint32_t ulMinInterval, uint32_t ulMaxInterval, uint32_t ulLatency, uint32_t ulTimeout) {
	tBleStatus ret = BLE_STATUS_SUCCESS;
	ret = aci_l2cap_connection_parameter_update_req(connection_handle, ulMinInterval * 1.25, ulMaxInterval * 1.25, ulMaxInterval, ulTimeout * 10);
	return ret == BLE_STATUS_SUCCESS ? eBTStatusSuccess : eBTStatusFail;
}

BTStatus_t prvBTSetScanParameters(uint8_t ucAdapterIf, uint32_t ulScanInterval, uint32_t ulScanWindow) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTMultiAdvEnable(uint8_t ucAdapterIf, BTGattAdvertismentParams_t xAdvParams) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTMultiAdvUpdate(uint8_t ucAdapterIf, BTGattAdvertismentParams_t advParams) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTMultiAdvSetInstData(uint8_t ucAdapterIf, bool bSetScanRsp, bool bIncludeName, bool bInclTxpower, uint32_t ulAppearance, size_t xManufacturerLen, char * pcManufacturerData, size_t xServiceDataLen, char * pcServiceData, BTUuid_t * pxServiceUuid, size_t xNbServices) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTMultiAdvDisable(uint8_t ucAdapterIf) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTBatchscanCfgStorage(uint8_t ucAdapterIf, uint32_t ulBatchScanFullMax, uint32_t ulBatchScanTruncMax, uint32_t ulBatchScanNotifyThreshold) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTBatchscanEndBatchScan(uint8_t ucAdapterIf, uint32_t ulScanMode, uint32_t ulScanInterval, uint32_t ulScanWindow, uint32_t ulAddrType, uint32_t ulDiscardRule) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTBatchscanDisBatchScan(uint8_t ucAdapterIf) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTBatchscanReadReports(uint8_t ucAdapterIf, uint32_t ulScanMode) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTSetPreferredPhy(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, uint16_t usPhyOptions) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

BTStatus_t prvBTReadPhy(uint16_t usConnId, BTReadClientPhyCallback_t xCb) {
	BTStatus_t xStatus = eBTStatusUnsupported;

	return xStatus;
}

const void * prvBTGetGattClientInterface() {
	return NULL;
}

const void * prvGetLeAdapter() {
	return &xBTLeAdapter;
}

