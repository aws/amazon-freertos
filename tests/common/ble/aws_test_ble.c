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
 * @file aws_test_ble.c
 * @brief Tests for ble.
 */

/* C standard library includes. */
#include <stddef.h>
#include <string.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "aws_doubly_linked_list.h"
#include "semphr.h"
#include "aws_ble.h"
/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

enum
{
	bletestsCHARA,
	bletestsCHARB,
	bletestsCHARC,
	bletestsCHARD,
	bletestsCHARE,
	bletestsCHARF,
	bletestsNB_CHARACTERISTICS
};


enum
{
	bletestsCHAR_DESCRA,
	bletestsCHAR_DESCRB,
	bletestsCHAR_DESCRC,
	bletestsCHAR_DESCRD,
	bletestsCCCD_E,
	bletestsCCCD_F,
	bletestsNB_CHARACTERISTICS_DESCRIPTORS
};


typedef enum{
	eBLEHALEventServerRegisteredCb = 0,
	eBLEHALEventEnableDisableCb = 1,
	eBLEHALEventCharAddedCb = 2,
	eBLEHALEventSeviceAddedCb = 3,
	eBLEHALEventSeviceStartedCb = 4,
	eBLEHALEventSeviceStoppedCb = 5,
	eBLEHALEventSeviceDeletedCb = 6,
	eBLEHALEventCharDescrAddedCb = 7,
	eBLEHALEventIncludedServiceAdded = 8,
	eBLEHALEventRegisterBleAdapterCb = 9,
	eBLEHALEventAdapterPropertiesCb = 10,
	eBLEHALEventRegisterUnregisterGattServerCb = 11,
	eBLEHALEventPropertyCb = 12,
	eBLEHALEventSetAdvCb = 13,
	eBLEHALEventStartAdvCb = 14,
	eBLEHALEventConnectionCb = 15,
	eBLEHALEventConnectionUpdateCb = 16,
	eBLEHALEventReadAttrCb = 17,
	eBLEHALEventWriteAttrCb = 18,
	eBLEHALEventIndicateCb = 19,
	eBLEHALEventConfimCb = 20,
	eBLEHALEventSSPrequestCb = 21,
	eBLEHALEventPairingStateChangedCb = 22,
	eBLEHALEventRequestExecWriteCb = 23,
	eBLEHALEventBondedCb = 24,
	eBLENbHALEvents,
}BLEHALEventsInternals_t;

#define bletestsDEFAULT_CHAR_VALUE "hello"
#define bletestsSTRINGYFIED_UUID_SIZE 36  /* like "8a7f1168-48af-4efb-83b5-e679f9320002" */
#define bletestsFULL_PERMISSIONS ( eBTPermRead | eBTPermWrite )
#define bletestsNB_INCLUDEDSERVICES   1
#define bletestsCCCD 0x2902
#define bletestsAPP_UUID                 { 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsSERVER_UUID              { 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsFREERTOS_SVC_UUID        { 0x5A, 0xDB, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }


#define bletestsFREERTOS_SVC_A_UUID      { 0x00, 0x00, 0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31 }
#define  bletestsFREERTOS_CHAR_DUMMY_UUID { 0x01, 0x00, 0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31 }

#define bletestsFREERTOS_SVC_B_UUID      { 1, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_CHAR_A_UUID      { 2, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_CHAR_B_UUID      { 3, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_CHAR_C_UUID      { 4, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_CHAR_D_UUID      { 5, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_CHAR_E_UUID      { 6, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_CHAR_F_UUID      { 7, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }

#define bletestsFREERTOS_DESCR_A_UUID     { 8, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_DESCR_B_UUID     { 9, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_DESCR_C_UUID     { 10, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_DESCR_D_UUID     { 11, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }

#define bletestsDEVICE_NAME             "BT"
#define bletestsMAX_PROPERTY_SIZE       30

#define bletestsMTU_SIZE1 200
#define bletestsMAX_ADVERTISEMENT_INTERVAL 64

#define bletestsMIN_CONNECTION_INTERVAL   32
#define bletestsMAX_CONNECTION_INTERVAL   64

#define QUEUE_LENGTH    20
#define ITEM_SIZE       sizeof( void *)

/* Creating a waiting queue, were all events that are not immediatly expected are pushed too */
typedef struct{
	Link_t xNextQueueItem;
	void * pxNextQueuedItem;
} xWaitingEvents_t;
static Link_t xWaitingEventQueue;
/* Tests are waiting event on callback queue, if an unexpected event comes, it is added to the waiting queue to be processed later. */
static QueueHandle_t xCallbackQueue;
static BTInterface_t* pxBTInterface;
static BTState_t xBLEState = eBTstateOff;
static StaticEventGroup_t xWaitOperationComplete;
static BTBleAdapter_t * pxBTLeAdapterInterface;
static BTUuid_t xServerUUID = {
		.ucType = eBTuuidType128,
		.uu.uu128 = bletestsSERVER_UUID
};
static BTUuid_t xAppUUID = {
		.ucType = eBTuuidType128,
		.uu.uu128 = bletestsAPP_UUID
};
static uint8_t ucBLEAdapterIf;
static uint8_t ucBLEServerIf;
static BTBdaddr_t xAddressConnectedDevice;
static BTGattServerInterface_t * pxGattServerInterface;
static uint16_t usBLEConnId;

static BTStatus_t xCbStatus;
static uint16_t usCbSrvcHandle;
static uint16_t usCbHandle;
static BTGattSrvcId_t xCbSrvcId;
static BTUuid_t xCbUuid;
static uint32_t  ulCbNumProperties;
static BTProperty_t xCbProperties;
static uint8_t ucCbPropertyBuffer[bletestsMAX_PROPERTY_SIZE];

static BTBdaddr_t xCbBda;
static BLEConnectionParam_t xCbConnectionParam;
static uint32_t usCbConnInterval;


typedef struct{
	 BLEHALEventsInternals_t xEvent;
	 uint16_t usConnId;
	 uint32_t ulTransId;
	 BTBdaddr_t xBda;
	 uint16_t usAttrHandle;
	 uint16_t usOffset;
} BLETESTreadAttrCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	uint16_t usConnId;
	uint32_t ulTransId;
	BTBdaddr_t xBda;
	uint16_t usAttrHandle;
	uint16_t usOffset;
	size_t xLength;
	bool bNeedRsp;
	bool bIsPrep;
	uint8_t ucValue[bletestsSTRINGYFIED_UUID_SIZE];
} BLETESTwriteAttrCallback_t;


typedef struct{
	BLEHALEventsInternals_t xEvent;
	uint16_t usConnId;
	BTStatus_t xStatus;
} BLETESTindicateCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	BTStatus_t xStatus;
	uint16_t usAttrHandle;
} BLETESTconfirmCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	BTBdaddr_t xRemoteBdAddr;
	BTBdname_t xRemoteBdName;
	uint32_t ulCod;
	BTSspVariant_t xPairingVariant;
	uint32_t ulPassKey;
} BLETESTsspRequestCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	BTStatus_t xStatus;
	BTBdaddr_t xRemoteBdAddr;
	BTSecurityLevel_t xSecurityLevel;
	BTAuthFailureReason_t xReason;
} BLETESTPairingStateChangedCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	uint16_t usConnId;
    uint32_t ulTransId;
    BTBdaddr_t xBda;
    bool bExecWrite;
} BLETESTRequestExecWriteCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	uint16_t usConnId;
	uint8_t ucServerIf;
	BTBdaddr_t pxBda;
	bool bConnected;
} BLETESTConnectionCallback_t;

typedef struct{
	BLEHALEventsInternals_t xEvent;
	BTStatus_t xStatus;
	BTBdaddr_t xRemoteBdAddr;
	bool bIsBonded;
}BLETESTBondedCallback_t;

BLEService_t xSrvcA ={
		.xAttributeData.xUuid.ucType = eBTuuidType128,
		.xAttributeData.xUuid.uu.uu128 = bletestsFREERTOS_SVC_A_UUID,
		.pxCharacteristics = NULL,
		.xNbCharacteristics = 1, /* one fake characteristic */
		.xNbDescriptors = 0,
		.pxDescriptors = NULL,
		.pxIncludedServices = NULL,
		.xNbIncludedServices = 0,
		.xServiceType = eBTServiceTypePrimary,
		.ucInstId = 0
};


BLECharacteristic_t       xCharacteristicts[bletestsNB_CHARACTERISTICS];
BLECharacteristicDescr_t  xDescriptors[bletestsNB_CHARACTERISTICS_DESCRIPTORS];
uint8_t ucResponseBufferChar[bletestsNB_CHARACTERISTICS][bletestsSTRINGYFIED_UUID_SIZE];
uint8_t ucResponseBufferDescr[bletestsNB_CHARACTERISTICS_DESCRIPTORS][bletestsSTRINGYFIED_UUID_SIZE];
BLEIncludedService_t xIncludedServiceA;


/* Dummy for service A */
BLECharacteristic_t xDummyCharacteristic;

BLEService_t xSrvcB ={
		.xAttributeData.xUuid.ucType = eBTuuidType128,
		.xAttributeData.xUuid.uu.uu128 = bletestsFREERTOS_SVC_B_UUID,
		.pxCharacteristics = xCharacteristicts,
		.xNbCharacteristics = bletestsNB_CHARACTERISTICS,
		.pxDescriptors = xDescriptors,
		.xNbDescriptors = bletestsNB_CHARACTERISTICS_DESCRIPTORS,
		.pxIncludedServices = &xIncludedServiceA,
		.xNbIncludedServices = bletestsNB_INCLUDEDSERVICES,
		.xServiceType = eBTServiceTypePrimary,
		.ucInstId = 0
};

BTGattAdvertismentParams_t xAdvertisementConfigA =
{
		.usAdvertisingEventProperties = BTAdvInd,
		.bIncludeTxPower = true,
		.bIncludeName = true,
		.bSetScanRsp = false,
		.ulAppearance = 0,
		.ulMinInterval = bletestsMAX_ADVERTISEMENT_INTERVAL/2,
		.ulMaxInterval = bletestsMAX_ADVERTISEMENT_INTERVAL,
		.ucChannelMap = 0,
		.ucPrimaryAdvertisingPhy = 0,
		.ucSecondaryAdvertisingPhy = 0,
		.xAddrType = BTAddrTypePublic,
};

BTGattAdvertismentParams_t xAdvertisementConfigB =
{
		.usAdvertisingEventProperties = BTAdvInd,
		.bIncludeTxPower = true,
		.bIncludeName = true,
		.bSetScanRsp = true,
		.ulAppearance = 0,
		.ulMinInterval = bletestsMAX_ADVERTISEMENT_INTERVAL/2,
		.ulMaxInterval = bletestsMAX_ADVERTISEMENT_INTERVAL,
		.ucChannelMap = 0,
		.ucPrimaryAdvertisingPhy = 0,
		.ucSecondaryAdvertisingPhy = 0,
		.xAddrType = BTAddrTypePublic,
};

BLEConnectionParam_t xConnectionParamA =
{
		.ulMinInterval = bletestsMIN_CONNECTION_INTERVAL,
		.ulMaxInterval = bletestsMAX_CONNECTION_INTERVAL,
		.ulLatency = 4,
		.ulTimeout = 400
};

const TickType_t bletestWAIT_MODE1_LEVEL2_QUERY = pdMS_TO_TICKS( 10000 ); /* Wait 10s max */
const TickType_t BLE_TESTS_WAIT = pdMS_TO_TICKS( 60000 ); /* Wait 60s max */

static void prvSetGetProperty( BTProperty_t * pxProperty, bool bIsSet );
static void prvStartAdvertisement(void);
static BTStatus_t prvWaitEventFromQueue( BLEHALEventsInternals_t xEventName, void * pxMessage, size_t xMessageLength, TickType_t xTestWai);
static void prvWriteCheckAndResponse(BLEAttributeData_t * xAttributeData,
							bool bNeedRsp,
							bool IsPrep,
							uint16_t usOffset);
static void prvReadCheckAndResponse(BLEAttributeData_t * xAttributeData);
static void prvGroupInit( );
static void prvDeviceStateChangedCb( BTState_t xState );
static void prvGroupFree();
static void prvRegisterBleAdapterCb( BTStatus_t xStatus,
							 uint8_t ucAdapterIf,
							 BTUuid_t * pxAppUuid );
static void prvBTRegisterServerCb( BTStatus_t xStatus,
                                             uint8_t ucServerIf,
                                             BTUuid_t * pxAppUuid );
static void prvBTUnregisterServerCb( BTStatus_t xStatus,
                                             uint8_t ucServerIf );
static void prvServiceAddedCb( BTStatus_t xStatus,
                                           uint8_t ucServerIf,
                                           BTGattSrvcId_t * pxSrvcId,
                                           uint16_t usServiceHandle );
static void prvIncludedServiceAddedCb( BTStatus_t xStatus,
										uint8_t ucServerIf,
										uint16_t usServiceHandle,
										uint16_t usInclSrvcHandle );
static void prvServiceStartedCb ( BTStatus_t xStatus,
								  uint8_t ucServerIf,
								  uint16_t usServiceHandle );
void prvServiceStoppedCb( BTStatus_t xStatus,
						  uint8_t ucServerIf,
						  uint16_t usServiceHandle );
void prvServiceDeletedCb( BTStatus_t xStatus,
						  uint8_t ucServerIf,
						  uint16_t usServiceHandle );
void prvCharacteristicAddedCb( BTStatus_t xStatus,
        uint8_t ucServerIf,
        BTUuid_t * pxUuid,
        uint16_t usServiceHandle,
        uint16_t usCharHandle );
void prvCharacteristicDescrAddedCb( BTStatus_t xStatus,
        uint8_t ucServerIf,
        BTUuid_t * pxUuid,
        uint16_t usServiceHandle,
        uint16_t usCharHandle );
void prvAdapterPropertiesCb( BTStatus_t xStatus,
        uint32_t ulNumProperties,
        BTProperty_t * pxProperties );
void prvSetAdvDataCb( BTStatus_t xStatus );
void prvAdvStartCb( BTStatus_t xStatus,
                    uint32_t ulServerIf );
void prvConnectionCb ( uint16_t usConnId,
						 uint8_t ucServerIf,
						 bool bConnected,
						 BTBdaddr_t * pxBda );
void prvConnParameterUpdateCb (BTStatus_t xStatus,
								  const BTBdaddr_t * pxBdAddr,
								  uint32_t ulMinInterval,
								  uint32_t ulMaxInterval,
								  uint32_t ulLatency,
								  uint32_t usConnInterval,
								  uint32_t ulTimeout );
void prvRequestReadCb( uint16_t usConnId,
					  uint32_t ulTransId,
					  BTBdaddr_t * pxBda,
					  uint16_t usAttrHandle,
					  uint16_t usOffset );
void prvRequestWriteCb( uint16_t usConnId,
					    uint32_t ulTransId,
					    BTBdaddr_t * pxBda,
					    uint16_t usAttrHandle,
					    uint16_t usOffset,
					    size_t xLength,
					    bool bNeedRsp,
 					    bool bIsPrep,
 					    uint8_t * pucValue );
void prvIndicationSentCb( uint16_t usConnId,
                          BTStatus_t xStatus );
void prvResponseConfirmationCb( BTStatus_t xStatus,
                                uint16_t usHandle );
void prvSspRequestCb( BTBdaddr_t * pxRemoteBdAddr,
                             BTBdname_t * pxRemoteBdName,
                             uint32_t ulCod,
                             BTSspVariant_t xPairingVariant,
                             uint32_t ulPassKey );
void prvPairingStateChangedCb( BTStatus_t xStatus,
							   BTBdaddr_t * pxRemoteBdAddr,
                                                           BTBondState_t xState,
							   BTSecurityLevel_t xSecurityLevel,
							   BTAuthFailureReason_t xReason );
void prvRequestExecWriteCb( uint16_t usConnId,
                            uint32_t ulTransId,
                            BTBdaddr_t * pxBda,
                            bool bExecWrite );
void prvBondedCb( BTStatus_t xStatus,
        BTBdaddr_t * pxRemoteBdAddr,
        bool bIsBonded );

static BTCallbacks_t xBTManagerCb =
{
	    .pxDeviceStateChangedCb = prvDeviceStateChangedCb,
	    .pxAdapterPropertiesCb = prvAdapterPropertiesCb,
	    .pxRemoteDevicePropertiesCb = NULL,
	    .pxSspRequestCb = prvSspRequestCb,
	    .pxPairingStateChangedCb = prvPairingStateChangedCb,
	    .pxBondedCb = prvBondedCb,
	    .pxDutModeRecvCb = NULL,
	    .pxleTestModeCb = NULL,
	    .pxEnergyInfoCb = NULL,
	    .pxReadRssiCb = NULL,
	    .pxTxPowerCb = NULL,
		.pxSlaveSecurityRequestCb = NULL,
};

static BTBleAdapterCallbacks_t xBTBleAdapterCb =
{
	.pxRegisterBleAdapterCb = prvRegisterBleAdapterCb,
	.pxScanResultCb = NULL,
	.pxBleAdapterPropertiesCb = NULL,
	.pxBleRemoteDevicePropertiesCb = NULL,
	.pxOpenCb = NULL,
	.pxCloseCb = NULL,
	.pxReadRemoteRssiCb = NULL,
	.pxAdvStartCb = prvAdvStartCb,
	.pxSetAdvDataCb = prvSetAdvDataCb,
	.pxConnParameterUpdateCb = prvConnParameterUpdateCb,
	.pxScanFilterCfgCb = NULL,
	.pxScanFilterParamCb = NULL,
	.pxScanFilterStatusCb = NULL,
	.pxMultiAdvEnableCb = NULL,
	.pxMultiAdvUpdateCb = NULL,
	.pxMultiAdvDataCb = NULL,
	.pxMultiAdvDisableCb = NULL,
	.pxCongestionCb = NULL,
	.pxBatchscanCfgStorageCb = NULL,
	.pxBatchscanEnbDisableCb = NULL,
	.pxBatchscanReportsCb = NULL,
	.pxBatchscanThresholdCb = NULL,
	.pxTrackAdvEventCb = NULL,
	.pxScanParameterSetupCompletedCb = NULL,
	.pxPhyUpdatedCb = NULL,
};


static BTGattServerCallbacks_t xBTGattServerCb =
{
	.pxRegisterServerCb       = prvBTRegisterServerCb,
	.pxUnregisterServerCb     = prvBTUnregisterServerCb,
	.pxConnectionCb           = prvConnectionCb,
	.pxServiceAddedCb         = prvServiceAddedCb,
	.pxIncludedServiceAddedCb = prvIncludedServiceAddedCb,
	.pxCharacteristicAddedCb  = prvCharacteristicAddedCb,
	.pxSetValCallbackCb     = NULL,
	.pxDescriptorAddedCb      = prvCharacteristicDescrAddedCb,
	.pxServiceStartedCb       = prvServiceStartedCb,
	.pxServiceStoppedCb       = prvServiceStoppedCb,
	.pxServiceDeletedCb       = prvServiceDeletedCb,
	.pxRequestReadCb     = prvRequestReadCb,
	.pxRequestWriteCb     = prvRequestWriteCb,
	.pxRequestExecWriteCb     = prvRequestExecWriteCb,
	.pxResponseConfirmationCb     = prvResponseConfirmationCb,
	.pxIndicationSentCb     = prvIndicationSentCb ,
	.pxCongestionCb     = NULL,
	.pxMtuChangedCb 		  = NULL
};
/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE )
{
	/* Initializations that need to be done once for all the tests. */
	prvGroupInit();


    RUN_TEST_CASE( Full_BLE, BLE_Initialize_common_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GATT );

    RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_CreateServices );
    RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_CreateCharacteristics );
    RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_Descriptors );
//RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_IncludedService );
    RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_StartService );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetProperties );//@TOTO, incomplete
	RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveAllBonds );

	RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetAvertisementData );//@TOTO, incomplete
	RUN_TEST_CASE( Full_BLE, BLE_Advertising_StartAdvertisement );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_SimpleConnection );
	RUN_TEST_CASE( Full_BLE, BLE_Connection_UpdateConnectionParamReq );

//RUN_TEST_CASE( Full_BLE, BLE_Connection_ChangeMTUsize );
	RUN_TEST_CASE( Full_BLE, BLE_Property_WriteCharacteristic );
	RUN_TEST_CASE( Full_BLE, BLE_Property_WriteDescriptor );
	RUN_TEST_CASE( Full_BLE, BLE_Property_ReadCharacteristic);
	RUN_TEST_CASE( Full_BLE, BLE_Property_ReadDescriptor );
	RUN_TEST_CASE( Full_BLE, BLE_Property_WriteNoResponse );
    RUN_TEST_CASE( Full_BLE, BLE_Property_Enable_Indication_Notification );
	RUN_TEST_CASE( Full_BLE, BLE_Property_Notification );
	RUN_TEST_CASE( Full_BLE, BLE_Property_Indication );
    RUN_TEST_CASE( Full_BLE, BLE_Property_Disable_Indication_Notification );
        
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level4 );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteDescr );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteChar );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Disconnect );
	RUN_TEST_CASE( Full_BLE, BLE_Connection_BondedReconnectAndPair );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Disconnect );

	RUN_TEST_CASE( Full_BLE, BLE_Connection_CheckBonding);
	RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveBonding );
	RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level2 );

    RUN_TEST_CASE( Full_BLE, BLE_DeInitialize );
    prvGroupFree();
}

void prvRemoveBond(BTBdaddr_t * pxDeviceAddress)
{
	BTStatus_t xStatus;
	BLETESTBondedCallback_t  xBondedEvent;

	xStatus = pxBTInterface->pxRemoveBond(pxDeviceAddress);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xStatus = prvWaitEventFromQueue(eBLEHALEventBondedCb, (void *)&xBondedEvent, sizeof(BLETESTBondedCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xBondedEvent.xStatus);
	TEST_ASSERT_EQUAL(false, xBondedEvent.bIsBonded);
	TEST_ASSERT_EQUAL(0, memcmp(&xBondedEvent.xRemoteBdAddr, pxDeviceAddress, sizeof(BTBdaddr_t) ));

}

TEST( Full_BLE, BLE_Connection_RemoveAllBonds )
{
	BTProperty_t pxProperty;
	uint16_t usIndex;

	/* Set the name */
	pxProperty.xType = eBTpropertyAdapterBondedDevices;

	/* Get bonded devices */
	prvSetGetProperty(&pxProperty, false);

	for(usIndex = 0; usIndex < pxProperty.xLen; usIndex++)
	{
		prvRemoveBond(&((BTBdaddr_t *)pxProperty.pvVal)[usIndex]);
	}

	/* Get bonded devices. */
	prvSetGetProperty(&pxProperty, false);
	/* Check none are left. */
	TEST_ASSERT_EQUAL(0, pxProperty.xLen);
}

bool prvGetCheckDeviceBonded(BTBdaddr_t * pxDeviceAddress)
{
	BTProperty_t pxProperty;
	uint16_t usIndex;
	bool bFoundRemoteDevice = false;

	pxProperty.xType = eBTpropertyAdapterBondedDevices;

	/* Set the name */
	prvSetGetProperty(&pxProperty, false);

	for(usIndex = 0; usIndex < pxProperty.xLen; usIndex++)
	{
		if( 0 == memcmp(&((BTBdaddr_t *)pxProperty.pvVal)[usIndex], pxDeviceAddress, sizeof(BTBdaddr_t) ))
		{
			bFoundRemoteDevice = true;
			break;
		}
	}

	return bFoundRemoteDevice;
}

void prvWaitConnection(bool bConnected)
{
	BLETESTConnectionCallback_t xConnectionEvent;
	BTStatus_t xStatus;
	xStatus = prvWaitEventFromQueue(eBLEHALEventConnectionCb, (void *)&xConnectionEvent, sizeof(BLETESTConnectionCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(bConnected, xConnectionEvent.bConnected);
	TEST_ASSERT_EQUAL(ucBLEServerIf, xConnectionEvent.ucServerIf);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);

	/* Stop advertisement. */
	xStatus = pxBTLeAdapterInterface->pxStopAdv(ucBLEAdapterIf);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
}
TEST( Full_BLE, BLE_Connection_Mode1Level2 )
{
	BTStatus_t xStatus;
	BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
	BLETESTsspRequestCallback_t xSSPrequestEvent;

	prvStartAdvertisement();
	prvWaitConnection(true);

	xStatus = prvWaitEventFromQueue(eBLEHALEventSSPrequestCb, (void *)&xSSPrequestEvent, sizeof(BLETESTsspRequestCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(0, memcmp(&xSSPrequestEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(eBTsspVariantConsent , xSSPrequestEvent.xPairingVariant);

	xStatus = pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xStatus = prvWaitEventFromQueue(eBLEHALEventPairingStateChangedCb, (void *)&xPairingStateChangedEvent, sizeof(BLETESTPairingStateChangedCallback_t), bletestWAIT_MODE1_LEVEL2_QUERY);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);/* Pairing should never come since it is secure connection only */
	TEST_ASSERT_EQUAL(eBTStatusFail, xPairingStateChangedEvent.xStatus);/* Pairing should never come since it is secure connection only */
	/* @TODO add correct flag */
	/* TEST_ASSERT_EQUAL(eBTauthFailInsuffSecurity, xPairingStateChangedEvent.xReason); */ /* Pairing should never come since it is secure connection only */
}

TEST( Full_BLE, BLE_Connection_RemoveBonding )
{
	bool bFoundRemoteDevice;

	prvRemoveBond(&xAddressConnectedDevice);

	bFoundRemoteDevice =  prvGetCheckDeviceBonded(&xAddressConnectedDevice);
	TEST_ASSERT_EQUAL(false, bFoundRemoteDevice);
}


TEST( Full_BLE, BLE_Connection_CheckBonding )
{
	bool bFoundRemoteDevice;

	bFoundRemoteDevice =  prvGetCheckDeviceBonded(&xAddressConnectedDevice);
	TEST_ASSERT_EQUAL(true, bFoundRemoteDevice);
}



TEST( Full_BLE, BLE_Connection_BondedReconnectAndPair )
{
	BTStatus_t xStatus;
	BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
	BLETESTBondedCallback_t  xBondedEvent;

	prvStartAdvertisement();
	prvWaitConnection(true);

	xStatus = prvWaitEventFromQueue(eBLEHALEventPairingStateChangedCb, (void *)&xPairingStateChangedEvent, sizeof(BLETESTPairingStateChangedCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xPairingStateChangedEvent.xStatus);
	TEST_ASSERT_EQUAL(0, memcmp(&xPairingStateChangedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel);

	xStatus = prvWaitEventFromQueue(eBLEHALEventBondedCb, (void *)&xBondedEvent, sizeof(BLETESTBondedCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xBondedEvent.xStatus);
	TEST_ASSERT_EQUAL(true, xBondedEvent.bIsBonded);
	TEST_ASSERT_EQUAL(0, memcmp(&xBondedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));

	prvWriteCheckAndResponse(&xCharacteristicts[bletestsCHARB].xAttributeData,
							 true,
							 false,
							 0);
}


TEST( Full_BLE, BLE_Connection_Disconnect )
{
	prvWaitConnection(false);
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteDescr )
{
	prvWriteCheckAndResponse(&xDescriptors[bletestsCHAR_DESCRB].xAttributeData,
							 true,
							 false,
							 0);
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteChar )
{
	prvWriteCheckAndResponse(&xCharacteristicts[bletestsCHARB].xAttributeData,
							 true,
							 false,
							 0);
}

TEST( Full_BLE, BLE_Connection_Mode1Level4 )
{
	BTStatus_t xStatus;
	BLETESTsspRequestCallback_t xSSPrequestEvent;
	BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
	BLETESTBondedCallback_t  xBondedEvent;

	/* Wait secure connection. Secure connection is triggered by writting to bletestsCHARB. */
	xStatus = prvWaitEventFromQueue(eBLEHALEventSSPrequestCb, (void *)&xSSPrequestEvent, sizeof(BLETESTsspRequestCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(0, memcmp(&xSSPrequestEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(eBTsspVariantConsent , xSSPrequestEvent.xPairingVariant);

	xStatus = pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xStatus = prvWaitEventFromQueue(eBLEHALEventSSPrequestCb, (void *)&xSSPrequestEvent, sizeof(BLETESTsspRequestCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(0, memcmp(&xSSPrequestEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(eBTsspVariantPasskeyConfirmation , xSSPrequestEvent.xPairingVariant);

	xStatus = pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantPasskeyConfirmation, true, 0 );
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xStatus = prvWaitEventFromQueue(eBLEHALEventPairingStateChangedCb, (void *)&xPairingStateChangedEvent, sizeof(BLETESTPairingStateChangedCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xPairingStateChangedEvent.xStatus);
	TEST_ASSERT_EQUAL(0, memcmp(&xPairingStateChangedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel);

	xStatus = prvWaitEventFromQueue(eBLEHALEventBondedCb, (void *)&xBondedEvent, sizeof(BLETESTBondedCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xBondedEvent.xStatus);
	TEST_ASSERT_EQUAL(true, xBondedEvent.bIsBonded);
	TEST_ASSERT_EQUAL(0, memcmp(&xBondedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));

	prvWriteCheckAndResponse(&xCharacteristicts[bletestsCHARB].xAttributeData,
							 true,
							 false,
							 0);
}


void prvWriteCheckAndResponse(BLEAttributeData_t * xAttributeData,
							bool bNeedRsp,
							bool IsPrep,
							uint16_t usOffset)
{
	BLETESTwriteAttrCallback_t xWriteEvent;
	BLETESTconfirmCallback_t xConfirmEvent;
	BTGattResponse_t xGattResponse;
	BTStatus_t xStatus;

	/* Wait write event on char A*/
	xStatus = prvWaitEventFromQueue(eBLEHALEventWriteAttrCb, (void *)&xWriteEvent, sizeof(BLETESTwriteAttrCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(IsPrep, xWriteEvent.bIsPrep);
	TEST_ASSERT_EQUAL(bNeedRsp, xWriteEvent.bNeedRsp);
	TEST_ASSERT_EQUAL(xAttributeData->xHandle, xWriteEvent.usAttrHandle);
	TEST_ASSERT_EQUAL(usBLEConnId, xWriteEvent.usConnId);
	TEST_ASSERT_EQUAL(0, memcmp(&xWriteEvent.xBda, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(usOffset, xWriteEvent.usOffset);
	TEST_ASSERT_EQUAL(bletestsSTRINGYFIED_UUID_SIZE, xWriteEvent.xLength);

	if(bNeedRsp == true)
	{
		xGattResponse.usHandle = xWriteEvent.usAttrHandle;
		xGattResponse.xAttrValue.usHandle = xWriteEvent.usAttrHandle;
		xGattResponse.xAttrValue.usOffset = xWriteEvent.usOffset;
		xGattResponse.xAttrValue.xLen = xWriteEvent.xLength;
		xAttributeData->xSize = xWriteEvent.xLength;
		memcpy(xAttributeData->pucData, xWriteEvent.ucValue, xWriteEvent.xLength);
		xGattResponse.xAttrValue.pucValue = xAttributeData->pucData;
		xStatus = pxGattServerInterface->pxSendResponse(xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse);
		TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

		xStatus = prvWaitEventFromQueue(eBLEHALEventConfimCb, (void *)&xConfirmEvent, sizeof(BLETESTconfirmCallback_t), BLE_TESTS_WAIT);
		TEST_ASSERT_EQUAL(eBTStatusSuccess,xConfirmEvent.xStatus);
		TEST_ASSERT_EQUAL(xAttributeData->xHandle, xConfirmEvent.usAttrHandle);
	}
}

void prvSendNotification(BLEAttributeData_t * xAttributeData, bool bConfirm)
{
	BTStatus_t xStatus;

	xStatus = pxGattServerInterface->pxSendIndication( ucBLEServerIf,
											xAttributeData->xHandle,
											usBLEConnId,
											xAttributeData->xSize,
										    xAttributeData->pucData,
										    bConfirm);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
}

TEST( Full_BLE, BLE_Property_Enable_Indication_Notification )
{
    BTStatus_t xStatus;
    BLETESTwriteAttrCallback_t xWriteEvent;

    /* Wait for the CCCD write events */
    /* TODO: Test the actual values of events */
    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( xDescriptors[ bletestsCCCD_E ].xAttributeData.xHandle, xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( xDescriptors[ bletestsCCCD_F ].xAttributeData.xHandle, xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
}

TEST( Full_BLE, BLE_Property_Disable_Indication_Notification )
{
    BTStatus_t xStatus;
    BLETESTwriteAttrCallback_t xWriteEvent;

    /* Wait for the CCCD write events */
    /* TODO: Test the actual values of events */
    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( xDescriptors[ bletestsCCCD_E ].xAttributeData.xHandle, xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( xDescriptors[ bletestsCCCD_F ].xAttributeData.xHandle, xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
}


TEST( Full_BLE, BLE_Property_Indication )
{
	BTStatus_t xStatus;
	BLETESTindicateCallback_t xIndicateEvent;

	memcpy(xCharacteristicts[bletestsCHARF].xAttributeData.pucData, bletestsDEFAULT_CHAR_VALUE, sizeof(bletestsDEFAULT_CHAR_VALUE) - 1);
	xCharacteristicts[bletestsCHARF].xAttributeData.xSize = sizeof(bletestsDEFAULT_CHAR_VALUE) - 1;

	prvSendNotification(&xCharacteristicts[bletestsCHARF].xAttributeData, true);
	xStatus = prvWaitEventFromQueue(eBLEHALEventIndicateCb, (void *) &xIndicateEvent, sizeof(BLETESTindicateCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(usBLEConnId, xIndicateEvent.usConnId);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xIndicateEvent.xStatus);

}

TEST( Full_BLE, BLE_Property_Notification )
{
	void * pvPtr;

	memcpy(xCharacteristicts[bletestsCHARE].xAttributeData.pucData, bletestsDEFAULT_CHAR_VALUE, sizeof(bletestsDEFAULT_CHAR_VALUE) - 1);
	xCharacteristicts[bletestsCHARE].xAttributeData.xSize = sizeof(bletestsDEFAULT_CHAR_VALUE) - 1;

	prvSendNotification(&xCharacteristicts[bletestsCHARE].xAttributeData, false);
	/* Wait a possible confirm for 2 max connections interval */
	xQueueReceive( xCallbackQueue, &pvPtr, ( TickType_t ) bletestsMAX_CONNECTION_INTERVAL*2 );
	vPortFree(pvPtr);
}

TEST( Full_BLE, BLE_Property_WriteNoResponse )
{
	prvWriteCheckAndResponse(&xCharacteristicts[bletestsCHARD].xAttributeData,
							 false,
							 false,
							 0);
}

void prvReadCheckAndResponse(BLEAttributeData_t * xAttributeData)
{
	BLETESTreadAttrCallback_t xReadEvent;
	BTGattResponse_t xGattResponse;
	BLETESTconfirmCallback_t xConfirmEvent;
	BTStatus_t xStatus;

	xStatus = prvWaitEventFromQueue(eBLEHALEventReadAttrCb, (void *) &xReadEvent, sizeof(BLETESTreadAttrCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	TEST_ASSERT_EQUAL(xAttributeData->xHandle, xReadEvent.usAttrHandle);
	TEST_ASSERT_EQUAL(usBLEConnId, xReadEvent.usConnId);
	TEST_ASSERT_EQUAL(0,xReadEvent.usOffset);
	TEST_ASSERT_EQUAL(0, memcmp(&xReadEvent.xBda, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));

	xGattResponse.usHandle = xReadEvent.usAttrHandle;
	xGattResponse.xAttrValue.usHandle = xReadEvent.usAttrHandle;
	xGattResponse.xAttrValue.usOffset = xReadEvent.usOffset;
	xGattResponse.xAttrValue.xLen = xAttributeData->xSize;
	xGattResponse.xAttrValue.pucValue = xAttributeData->pucData;
	pxGattServerInterface->pxSendResponse(xReadEvent.usConnId, xReadEvent.ulTransId, eBTStatusSuccess, &xGattResponse);

	xStatus = prvWaitEventFromQueue(eBLEHALEventConfimCb, (void *)&xConfirmEvent, sizeof(BLETESTconfirmCallback_t), BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess,xConfirmEvent.xStatus);
	TEST_ASSERT_EQUAL(xAttributeData->xHandle, xConfirmEvent.usAttrHandle);
}

TEST( Full_BLE, BLE_Property_ReadDescriptor )
{
	prvReadCheckAndResponse(&xDescriptors[bletestsCHAR_DESCRA].xAttributeData);
}

TEST( Full_BLE, BLE_Property_ReadCharacteristic )
{
	prvReadCheckAndResponse(&xCharacteristicts[bletestsCHARA].xAttributeData);
}

TEST( Full_BLE, BLE_Property_WriteDescriptor )
{
	prvWriteCheckAndResponse(&xDescriptors[bletestsCHAR_DESCRA].xAttributeData,
							 true,
							 false,
							 0);
}

TEST( Full_BLE, BLE_Property_WriteCharacteristic )
{
	prvWriteCheckAndResponse(&xCharacteristicts[bletestsCHARA].xAttributeData,
							 true,
							 false,
							 0);
}

TEST( Full_BLE, BLE_Connection_ChangeMTUsize )
{

}

TEST( Full_BLE, BLE_Connection_UpdateConnectionParamReq )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxBTLeAdapterInterface->pxConnParameterUpdateRequest(&xAddressConnectedDevice,
														xConnectionParamA.ulMinInterval,
														xConnectionParamA.ulMaxInterval,
														xConnectionParamA.ulLatency,
														xConnectionParamA.ulTimeout );
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventConnectionUpdateCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(0, memcmp(&xCbBda, &xAddressConnectedDevice, sizeof(BTBdaddr_t) ));
	TEST_ASSERT_EQUAL(xConnectionParamA.ulMinInterval, xCbConnectionParam.ulMinInterval);
	TEST_ASSERT_EQUAL(xConnectionParamA.ulMaxInterval, xCbConnectionParam.ulMaxInterval);
	TEST_ASSERT_EQUAL(xConnectionParamA.ulLatency, xCbConnectionParam.ulLatency );
	TEST_ASSERT_EQUAL(xConnectionParamA.ulTimeout, xCbConnectionParam.ulTimeout );

}

TEST( Full_BLE, BLE_Connection_SimpleConnection )
{
	prvWaitConnection(true);
}


void prvSetAdvertisement( BTGattAdvertismentParams_t * pxParams,
						  uint16_t usServiceDataLen,
						  char * pcServiceData,
						  BTUuid_t * pxServiceUuid,
						  size_t xNbServices )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxBTLeAdapterInterface->pxSetAdvData( ucBLEAdapterIf,
													 pxParams,
													 0,
													 NULL,
													 usServiceDataLen,
													 pcServiceData,
													 pxServiceUuid,
													 xNbServices);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventSetAdvCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);

}

void prvStartAdvertisement(void)
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxBTLeAdapterInterface->pxStartAdv(ucBLEAdapterIf);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventStartAdvCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
}

TEST( Full_BLE, BLE_Advertising_StartAdvertisement )
{

	prvStartAdvertisement();
}

TEST( Full_BLE, BLE_Advertising_SetAvertisementData )
{
	uint16_t usServiceDataLen;
	char * pcServiceData;
	BTUuid_t xServiceUuid = {
			.ucType = eBTuuidType128,
			.uu.uu128 = bletestsFREERTOS_SVC_UUID
	};

	size_t xNbServices;

	usServiceDataLen = 0;
	pcServiceData = NULL;
	xNbServices = 1;

	prvSetAdvertisement(&xAdvertisementConfigA,
						usServiceDataLen,
						pcServiceData,
						&xServiceUuid,
						xNbServices	);


	prvSetAdvertisement(&xAdvertisementConfigB,
						usServiceDataLen,
						pcServiceData,
						&xSrvcB.xAttributeData.xUuid,
						xNbServices	);
}

void prvSetGetProperty( BTProperty_t * pxProperty, bool bIsSet )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	if(bIsSet == true)
	{
		xStatus = pxBTInterface->pxSetDeviceProperty(pxProperty);
	}else
	{
		xStatus = pxBTInterface->pxGetDeviceProperty(pxProperty->xType);
	}

	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventPropertyCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(1, ulCbNumProperties);
	TEST_ASSERT_EQUAL(xCbProperties.xType, pxProperty->xType);

	if(bIsSet == true)
	{
		TEST_ASSERT_EQUAL(xCbProperties.xLen, pxProperty->xLen);
		TEST_ASSERT_EQUAL(0, memcmp(pxProperty->pvVal, xCbProperties.pvVal, xCbProperties.xLen ));
	}else
	{
		 pxProperty->xLen = xCbProperties.xLen;
		 pxProperty->pvVal = xCbProperties.pvVal;
	}
}

TEST( Full_BLE, BLE_Advertising_SetProperties )
{
	BTProperty_t pxProperty;
	uint16_t usMTUsize = bletestsMTU_SIZE1;

	pxProperty.xType = eBTpropertyBdname;
	pxProperty.xLen = strlen(bletestsDEVICE_NAME);
	pxProperty.pvVal = bletestsDEVICE_NAME;

	/* Set the name */
	prvSetGetProperty(&pxProperty, true);

	/* Get the name to check it is set */
//@TODO prvSetGetProperty(&pxProperty, false);

	pxProperty.xType = eBTpropertyLocalMTUSize;
	pxProperty.xLen = sizeof(usMTUsize);
	pxProperty.pvVal = &usMTUsize;

	/* Set the MTU size */
	prvSetGetProperty(&pxProperty, true);

	/* Get the MTU size to check it is set */
	//@TODOprvSetGetProperty(&pxProperty, false);
}

void prvStartService( BLEService_t * xRefSrvc )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxStartService(ucBLEServerIf, xRefSrvc->xAttributeData.xHandle, BTTransportLe);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventSeviceStartedCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(xRefSrvc->xAttributeData.xHandle, usCbSrvcHandle);
}

TEST( Full_BLE, BLE_CreateAttTable_StartService )
{
	/* Start service B */
	prvStartService(&xSrvcB);

	/* Start service A */
	prvStartService(&xSrvcA);
}

TEST( Full_BLE, BLE_CreateAttTable_IncludedService )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxAddIncludedService(ucBLEServerIf,
														  xIncludedServiceA.pxParentService->xAttributeData.xHandle,
													      xSrvcA.xAttributeData.xHandle);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventIncludedServiceAdded,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(xIncludedServiceA.pxParentService->xAttributeData.xHandle, usCbSrvcHandle);
	xIncludedServiceA.xAttributeData.xHandle = usCbHandle;
}

void prvCreateCharacteristicDescriptor( BLECharacteristicDescr_t * xRefCharDescr )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxAddDescriptor(ucBLEServerIf,
													 xRefCharDescr->pxParentService->xAttributeData.xHandle,
													 &xRefCharDescr->xAttributeData.xUuid,
													 xRefCharDescr->xPermissions);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventCharDescrAddedCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	if(xRefCharDescr->xAttributeData.xUuid.ucType == eBTuuidType16)
	{
		TEST_ASSERT_EQUAL(eBTuuidType16, xCbUuid.ucType);
		TEST_ASSERT_EQUAL(xRefCharDescr->xAttributeData.xUuid.uu.uu16, xCbUuid.uu.uu16);
	}else
	{
		TEST_ASSERT_EQUAL(0, memcmp(&xRefCharDescr->xAttributeData.xUuid, &xCbUuid, sizeof(BTUuid_t)));
	}

	TEST_ASSERT_EQUAL(xRefCharDescr->pxParentService->xAttributeData.xHandle, usCbSrvcHandle);
	xRefCharDescr->xAttributeData.xHandle = usCbHandle;
}

TEST( Full_BLE, BLE_CreateAttTable_Descriptors )
{
	prvCreateCharacteristicDescriptor(&xDescriptors[bletestsCHAR_DESCRA]);
	prvCreateCharacteristicDescriptor(&xDescriptors[bletestsCHAR_DESCRB]);
	prvCreateCharacteristicDescriptor(&xDescriptors[bletestsCHAR_DESCRC]);
	prvCreateCharacteristicDescriptor(&xDescriptors[bletestsCHAR_DESCRD]);
}

void prvCreateCharacteristic( BLECharacteristic_t * xRefChar )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxAddCharacteristic(ucBLEServerIf,
														   xRefChar->pxParentService->xAttributeData.xHandle,
														   &xRefChar->xAttributeData.xUuid,
														   xRefChar->xProperties,
														   xRefChar->xPermissions);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventCharAddedCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	if(xRefChar->xAttributeData.xUuid.ucType == eBTuuidType16)
	{
		TEST_ASSERT_EQUAL(eBTuuidType16, xCbUuid.ucType);
		TEST_ASSERT_EQUAL(xRefChar->xAttributeData.xUuid.uu.uu16, xCbUuid.uu.uu16);
	}else
	{
		TEST_ASSERT_EQUAL(0, memcmp(&xRefChar->xAttributeData.xUuid, &xCbUuid, sizeof(BTUuid_t)));
	}
	TEST_ASSERT_EQUAL(xRefChar->pxParentService->xAttributeData.xHandle, usCbSrvcHandle);
	xRefChar->xAttributeData.xHandle = usCbHandle;
}

TEST( Full_BLE, BLE_CreateAttTable_CreateCharacteristics )
{
	prvCreateCharacteristic(&xCharacteristicts[bletestsCHARA]);
	prvCreateCharacteristic(&xCharacteristicts[bletestsCHARB]);
	prvCreateCharacteristic(&xCharacteristicts[bletestsCHARC]);
	prvCreateCharacteristic(&xCharacteristicts[bletestsCHARD]);
	prvCreateCharacteristic(&xCharacteristicts[bletestsCHARE]);
	prvCreateCharacteristicDescriptor(&xDescriptors[bletestsCCCD_E]); /* CCCD need to be create first */
	prvCreateCharacteristic(&xCharacteristicts[bletestsCHARF]);
	prvCreateCharacteristicDescriptor(&xDescriptors[bletestsCCCD_F]); /* CCCD need to be create first */

	prvCreateCharacteristic(&xDummyCharacteristic);
}

void prvCreateService(BTGattSrvcId_t * pxSrvcToCreate, BLEService_t * xRefSrvc )
{
	BTStatus_t xStatus = eBTStatusSuccess;
	uint16_t usNumHandles = xRefSrvc->xNbCharacteristics*2 + xRefSrvc->xNbDescriptors + xRefSrvc->xNbIncludedServices + 1;

	pxSrvcToCreate->xId.ucInstId = xRefSrvc->ucInstId;
	memcpy(&pxSrvcToCreate->xId.xUuid, &xRefSrvc->xAttributeData.xUuid, sizeof(BTUuid_t));
	pxSrvcToCreate->xServiceType = xRefSrvc->xServiceType;
	xCbStatus = eBTStatusFail;

	xStatus = pxGattServerInterface->pxAddService(ucBLEServerIf, pxSrvcToCreate, usNumHandles);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventSeviceAddedCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(0,memcmp(&xCbSrvcId, pxSrvcToCreate, sizeof(BTGattSrvcId_t)));
	xRefSrvc->xAttributeData.xHandle = usCbSrvcHandle;
}

TEST( Full_BLE, BLE_CreateAttTable_CreateServices )
{
	BTGattSrvcId_t xSrvcId;

	/* Create service A */
	prvCreateService(&xSrvcId, &xSrvcA);

	/* Create service B */
	prvCreateService(&xSrvcId, &xSrvcB);
}

TEST( Full_BLE, BLE_Initialize_BLE_GATT )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	pxGattServerInterface = (BTGattServerInterface_t *)pxBTLeAdapterInterface->ppvGetGattServerInterface();
	TEST_ASSERT_NOT_EQUAL( NULL, pxGattServerInterface );

	pxGattServerInterface->pxGattServerInit(&xBTGattServerCb);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxRegisterServer(&xServerUUID);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventRegisterUnregisterGattServerCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
}

TEST( Full_BLE, BLE_Initialize_common_GAP )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	/* Get BT interface */
	pxBTInterface = (BTInterface_t* )BTGetBluetoothInterface();
	TEST_ASSERT_NOT_EQUAL( NULL, pxBTInterface );

	/* Initialize callbacks */
	xStatus = pxBTInterface->pxBtManagerInit(&xBTManagerCb);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	/* Enable RADIO and wait for callback. */
	xBLEState = eBTstateOff;
	xStatus = pxBTInterface->pxEnable(0);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventEnableDisableCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTstateOn, xBLEState);
}

TEST( Full_BLE, BLE_Initialize_BLE_GAP )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	pxBTLeAdapterInterface = (BTBleAdapter_t * )pxBTInterface->pxGetLeAdapter();
	TEST_ASSERT_NOT_EQUAL( NULL, pxBTLeAdapterInterface );

	xStatus = pxBTLeAdapterInterface->pxBleAdapterInit(&xBTBleAdapterCb);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xCbStatus = eBTStatusFail;
	xStatus = pxBTLeAdapterInterface->pxRegisterBleApp(&xAppUUID);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventRegisterBleAdapterCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
}

void prvStopAndDeleteService(BLEService_t * xRefSrvc)
{
	BTStatus_t xStatus = eBTStatusSuccess;

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxStopService(ucBLEServerIf, xRefSrvc->xAttributeData.xHandle);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventSeviceStoppedCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(xRefSrvc->xAttributeData.xHandle, usCbSrvcHandle);

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxDeleteService(ucBLEServerIf, xRefSrvc->xAttributeData.xHandle);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventSeviceDeletedCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);
	TEST_ASSERT_EQUAL(xRefSrvc->xAttributeData.xHandle, usCbSrvcHandle);
}

TEST( Full_BLE, BLE_DeInitialize )
{
	BTStatus_t xStatus = eBTStatusSuccess;

	prvStopAndDeleteService(&xSrvcA);
	prvStopAndDeleteService(&xSrvcB);

	xCbStatus = eBTStatusFail;
	xStatus = pxGattServerInterface->pxUnregisterServer(ucBLEServerIf);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventRegisterUnregisterGattServerCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xCbStatus);

	xStatus = pxBTLeAdapterInterface->pxUnregisterBleApp(ucBLEAdapterIf);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);

	xStatus = pxBTInterface->pxDisable(0);
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
	xEventGroupWaitBits((EventGroupHandle_t)&xWaitOperationComplete,
						1<<eBLEHALEventEnableDisableCb,
						pdTRUE,
						pdTRUE,
						BLE_TESTS_WAIT);
	TEST_ASSERT_EQUAL(eBTstateOff, xBLEState);


	xStatus = pxBTInterface->pxBtManagerCleanup();
	TEST_ASSERT_EQUAL(eBTStatusSuccess, xStatus);
}

void prvRegisterBleAdapterCb( BTStatus_t xStatus,
        uint8_t ucAdapter_if,
        BTUuid_t * pxAppUuid )
{
	xCbStatus = xStatus;
	ucBLEAdapterIf = ucAdapter_if;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(pxAppUuid == NULL)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	if(xCbStatus == eBTStatusSuccess)
	{
		if(pxAppUuid->ucType != xAppUUID.ucType)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	if(xCbStatus == eBTStatusSuccess)
	{
		if(memcmp(pxAppUuid->uu.uu128, xAppUUID.uu.uu128, bt128BIT_UUID_LEN ))
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventRegisterBleAdapterCb);
}

void prvDeviceStateChangedCb( BTState_t xState )
{
	xBLEState = xState;
	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventEnableDisableCb);
}

void prvGroupInit()
{
	uint16_t usIndex;
	uint8_t tmpCharA[] = bletestsFREERTOS_CHAR_A_UUID;
	uint8_t tmpCharB[] = bletestsFREERTOS_CHAR_B_UUID;
	uint8_t tmpCharC[] = bletestsFREERTOS_CHAR_C_UUID;
	uint8_t tmpCharD[] = bletestsFREERTOS_CHAR_D_UUID;
	uint8_t tmpCharE[] = bletestsFREERTOS_CHAR_E_UUID;
	uint8_t tmpCharF[] = bletestsFREERTOS_CHAR_F_UUID;
	uint8_t tmpCharDummy[] = bletestsFREERTOS_CHAR_DUMMY_UUID;
	uint8_t tmpCharDescrA[] = bletestsFREERTOS_DESCR_A_UUID;
	uint8_t tmpCharDescrB[] = bletestsFREERTOS_DESCR_B_UUID;
	uint8_t tmpCharDescrC[] = bletestsFREERTOS_DESCR_C_UUID;
	uint8_t tmpCharDescrD[] = bletestsFREERTOS_DESCR_D_UUID;

	/* Initialize event group before tests. */
	(void)xEventGroupCreateStatic((EventGroupHandle_t)&xWaitOperationComplete);

	/**
	 * Section initializing Characteristics.
	 */
	memset(xCharacteristicts, 0, sizeof(BLECharacteristic_t)*bletestsNB_CHARACTERISTICS);
	memset(&xDummyCharacteristic, 0, sizeof(BLECharacteristic_t));

	for(usIndex = 0; usIndex< bletestsNB_CHARACTERISTICS; usIndex++)
	{
		xCharacteristicts[usIndex].xAttributeData.xUuid.ucType = eBTuuidType128;
		xCharacteristicts[usIndex].pxParentService = &xSrvcB;
		xCharacteristicts[usIndex].xPermissions = bletestsFULL_PERMISSIONS;
		xCharacteristicts[usIndex].xAttributeData.pucData = ucResponseBufferChar[usIndex];
	}
	xDummyCharacteristic.xAttributeData.xUuid.ucType = eBTuuidType128;
	xDummyCharacteristic.pxParentService = &xSrvcA;
	xDummyCharacteristic.xPermissions = bletestsFULL_PERMISSIONS;

	/* Copy uuid. */
	memcpy( &xCharacteristicts[bletestsCHARA].xAttributeData.xUuid.uu.uu128, tmpCharA, bt128BIT_UUID_LEN);
	memcpy( &xCharacteristicts[bletestsCHARB].xAttributeData.xUuid.uu.uu128, tmpCharB, bt128BIT_UUID_LEN);
	memcpy( &xCharacteristicts[bletestsCHARC].xAttributeData.xUuid.uu.uu128, tmpCharC, bt128BIT_UUID_LEN);
	memcpy( &xCharacteristicts[bletestsCHARD].xAttributeData.xUuid.uu.uu128, tmpCharD, bt128BIT_UUID_LEN);
	memcpy( &xCharacteristicts[bletestsCHARE].xAttributeData.xUuid.uu.uu128, tmpCharE, bt128BIT_UUID_LEN);
	memcpy( &xCharacteristicts[bletestsCHARF].xAttributeData.xUuid.uu.uu128, tmpCharF, bt128BIT_UUID_LEN);
	memcpy( &xDummyCharacteristic.xAttributeData.xUuid.uu.uu128, tmpCharDummy, bt128BIT_UUID_LEN);

	/* Set properties. */
	xCharacteristicts[bletestsCHARA].xProperties = eBTPropRead|eBTPropWrite;
	xCharacteristicts[bletestsCHARB].xProperties = eBTPropRead|eBTPropWrite;
	xCharacteristicts[bletestsCHARC].xProperties = eBTPropRead|eBTPropWrite;
	xCharacteristicts[bletestsCHARD].xProperties = eBTPropWriteNoResponse;
	xCharacteristicts[bletestsCHARE].xProperties = eBTPropNotify;
	xCharacteristicts[bletestsCHARF].xProperties = eBTPropIndicate;
	xDummyCharacteristic.xProperties = eBTPropRead|eBTPropWrite;

	/* Set response buffer. */


	/* Set permissions */
	xCharacteristicts[bletestsCHARB].xPermissions = eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm;
	xCharacteristicts[bletestsCHARC].xPermissions = eBTPermReadEncrypted |  eBTPermWriteEncrypted;
	/**
	 * Section initializing Descriptors.
	 */
	memset(xDescriptors, 0, sizeof(BLECharacteristicDescr_t)*bletestsNB_CHARACTERISTICS_DESCRIPTORS);

	for(usIndex = 0; usIndex< bletestsNB_CHARACTERISTICS_DESCRIPTORS; usIndex++)
	{
		xDescriptors[usIndex].xAttributeData.xUuid.ucType = eBTuuidType128;
		xDescriptors[usIndex].pxParentService = &xSrvcB;
		xDescriptors[usIndex].xAttributeData.pucData = ucResponseBufferDescr[usIndex];
	}

	/* Copy uuid. */
	memcpy( &xDescriptors[bletestsCHAR_DESCRA].xAttributeData.xUuid.uu.uu128, tmpCharDescrA, bt128BIT_UUID_LEN);
	memcpy( &xDescriptors[bletestsCHAR_DESCRB].xAttributeData.xUuid.uu.uu128, tmpCharDescrB, bt128BIT_UUID_LEN);
	memcpy( &xDescriptors[bletestsCHAR_DESCRC].xAttributeData.xUuid.uu.uu128, tmpCharDescrC, bt128BIT_UUID_LEN);
	memcpy( &xDescriptors[bletestsCHAR_DESCRD].xAttributeData.xUuid.uu.uu128, tmpCharDescrD, bt128BIT_UUID_LEN);
	memcpy( &xDescriptors[bletestsCHAR_DESCRC].xAttributeData.xUuid.uu.uu128, tmpCharDescrC, bt128BIT_UUID_LEN);
	memcpy( &xDescriptors[bletestsCHAR_DESCRD].xAttributeData.xUuid.uu.uu128, tmpCharDescrD, bt128BIT_UUID_LEN);

	/* Descriptors are only 16 bits */
	xDescriptors[bletestsCCCD_E].xAttributeData.xUuid.uu.uu16 = bletestsCCCD;
	xDescriptors[bletestsCCCD_E].xAttributeData.xUuid.ucType = eBTuuidType16;
	xDescriptors[bletestsCCCD_F].xAttributeData.xUuid.uu.uu16 = bletestsCCCD;
	xDescriptors[bletestsCCCD_F].xAttributeData.xUuid.ucType = eBTuuidType16;


	/* Set permissions. */
	xDescriptors[bletestsCHAR_DESCRA].xPermissions = eBTPermRead| eBTPermWrite;
	xDescriptors[bletestsCHAR_DESCRB].xPermissions = eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm;
	xDescriptors[bletestsCHAR_DESCRC].xPermissions = eBTPermReadEncrypted |  eBTPermWriteEncrypted;
	xDescriptors[bletestsCHAR_DESCRD].xPermissions =  eBTPermRead;
	xDescriptors[bletestsCCCD_E].xPermissions =  eBTPermRead| eBTPermWrite;
	xDescriptors[bletestsCCCD_F].xPermissions =  eBTPermRead| eBTPermWrite;
	/**
	 * Section included service.
	 */
	memset(&xIncludedServiceA, 0, sizeof(BLEIncludedService_t));
	xIncludedServiceA.pxParentService  = &xSrvcB;
	xIncludedServiceA.pxPtrToService = &xSrvcA;

	/* Assign a buffer to property Cb */
	xCbProperties.pvVal = ucCbPropertyBuffer;


   /* Create a queue for callbacks. */
	xCallbackQueue = xQueueCreate( QUEUE_LENGTH,
								 ITEM_SIZE);
	listINIT_HEAD(&xWaitingEventQueue);
}

void prvGroupFree()
{
	void * pvPtr;
	Link_t *pxLink, *pxTmpLink;
	xWaitingEvents_t * pxTmpEvent;

	do{
		if( xQueueReceive( xCallbackQueue, &pvPtr, ( TickType_t ) 0 ) == pdPASS)
		{
			vPortFree(pvPtr);
		}else
		{
			break;
		}
	}while(1);/* Delete all objects in the queue. */

	vQueueDelete(xCallbackQueue);


	/* Remove everything in the waiting list that was not used. */
	listFOR_EACH_SAFE( pxLink, pxTmpLink, &( xWaitingEventQueue ) )
	{
		pxTmpEvent = listCONTAINER( pxLink, xWaitingEvents_t, xNextQueueItem );
		listREMOVE(pxLink);
		vPortFree(pxTmpEvent);
	}


	/* Initialize event group before tests. */
	if(xBLEState == eBTstateOn)
	{
		pxBTInterface->pxDisable(0);
	}

}

void prvBTRegisterServerCb( BTStatus_t xStatus,
								 uint8_t ucServerIf,
								 BTUuid_t * pxAppUuid )
{
	xCbStatus = xStatus;
	ucBLEServerIf = ucServerIf;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(pxAppUuid == NULL)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	if(xCbStatus == eBTStatusSuccess)
	{
		if(pxAppUuid->ucType != xServerUUID.ucType)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	if(xCbStatus == eBTStatusSuccess)
	{
		if(0 != memcmp(pxAppUuid->uu.uu128, xServerUUID.uu.uu128, bt128BIT_UUID_LEN ))
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventRegisterUnregisterGattServerCb);
}

void prvBTUnregisterServerCb( BTStatus_t xStatus,
								 uint8_t ucServerIf  )
{
	xCbStatus = xStatus;
	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventRegisterUnregisterGattServerCb);
}

void prvServiceAddedCb( BTStatus_t xStatus,
					   uint8_t ucServerIf,
					   BTGattSrvcId_t * pxSrvcId,
					   uint16_t usServiceHandle )
{
	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;

	if(pxSrvcId != NULL)
	{
		memcpy(&xCbSrvcId, pxSrvcId, sizeof(BTGattSrvcId_t));
	}else
	{
		memset(&xCbSrvcId, 0, sizeof(BTGattSrvcId_t));
		xCbStatus = eBTStatusFail;
	}

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventSeviceAddedCb);
}

void prvServiceStartedCb ( BTStatus_t xStatus,
								  uint8_t ucServerIf,
								  uint16_t usServiceHandle )
{
	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventSeviceStartedCb);
}

void prvServiceStoppedCb( BTStatus_t xStatus,
						  uint8_t ucServerIf,
						  uint16_t usServiceHandle )
{
	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}
	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventSeviceStoppedCb);
}

void prvServiceDeletedCb( BTStatus_t xStatus,
						  uint8_t ucServerIf,
						  uint16_t usServiceHandle )
{
	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventSeviceDeletedCb);
}

void prvCharacteristicAddedCb( BTStatus_t xStatus,
        uint8_t ucServerIf,
        BTUuid_t * pxUuid,
        uint16_t usServiceHandle,
        uint16_t usCharHandle )
{

	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;
	usCbHandle = usCharHandle;

	if(pxUuid != NULL)
	{
		memcpy( &xCbUuid, pxUuid, sizeof(BTUuid_t));
	}else
	{
		memset( &xCbUuid, 0, sizeof(BTUuid_t));
		xCbStatus = eBTStatusFail;
	}

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}
	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventCharAddedCb);
}

void prvCharacteristicDescrAddedCb( BTStatus_t xStatus,
        uint8_t ucServerIf,
        BTUuid_t * pxUuid,
        uint16_t usServiceHandle,
        uint16_t usCharHandle )
{

	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;
	usCbHandle = usCharHandle;

	if(pxUuid != NULL)
	{
		memcpy( &xCbUuid, pxUuid, sizeof(BTUuid_t));
	}else
	{
		memset( &xCbUuid, 0, sizeof(BTUuid_t));
		xCbStatus = eBTStatusFail;
	}


	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}
	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventCharDescrAddedCb);
}


void prvIncludedServiceAddedCb( BTStatus_t xStatus,
									uint8_t ucServerIf,
									uint16_t usServiceHandle,
									uint16_t usInclSrvcHandle )
{
	xCbStatus = xStatus;
	usCbSrvcHandle = usServiceHandle;
	usCbHandle = usInclSrvcHandle;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ucServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}


	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventIncludedServiceAdded);
}

void prvAdapterPropertiesCb( BTStatus_t xStatus,
        uint32_t ulNumProperties,
        BTProperty_t * pxProperties )
{
	xCbStatus = xStatus;
	ulCbNumProperties = ulNumProperties;
	xCbProperties.xLen = pxProperties->xLen;
	xCbProperties.xType = pxProperties->xType;

	if( pxProperties->xLen < bletestsMAX_PROPERTY_SIZE)
	{
		switch( pxProperties->xType )
		{
			case eBTpropertyAdapterBondedDevices:
				if(pxProperties->pvVal != NULL)
				{
					memcpy( xCbProperties.pvVal, pxProperties->pvVal, sizeof(BTBdaddr_t));
				}else
				{
					memset( xCbProperties.pvVal, 0, sizeof(BTBdaddr_t));
				}

				break;
			case eBTpropertyBdname:
				if(pxProperties->pvVal != NULL)
				{
					memcpy( xCbProperties.pvVal, pxProperties->pvVal, strlen(bletestsDEVICE_NAME));
				}else
				{
					memset( xCbProperties.pvVal, 0, strlen(bletestsDEVICE_NAME));
				}
				break;

			case eBTpropertyBdaddr:
				xCbStatus = eBTStatusFail;
				break;

			case eBTpropertyTypeOfDevice:
				xCbStatus = eBTStatusFail;
				break;

			case eBTpropertyLocalMTUSize:
				memcpy( xCbProperties.pvVal, pxProperties->pvVal, sizeof(uint16_t));
				break;

			case eBTpropertyBondable:
				xCbStatus = eBTStatusFail;
				break;

			case eBTpropertyIO:
				xCbStatus = eBTStatusFail;
				break;
			case eBTpropertySecureConnectionOnly:
			default:
				xCbStatus = eBTStatusFail;
		}

	}else
	{
		xCbStatus = eBTStatusFail;
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventPropertyCb);
}

void prvSetAdvDataCb( BTStatus_t xStatus )
{
	xCbStatus = xStatus;

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventSetAdvCb);
}

void prvAdvStartCb( BTStatus_t xStatus,
                    uint32_t ulServerIf )
{
	xCbStatus = xStatus;

	if(xCbStatus == eBTStatusSuccess)
	{
		if(ulServerIf != ucBLEServerIf)
		{
			xCbStatus = eBTStatusFail;
		}
	}

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventStartAdvCb);
}

void prvConnectionCb ( uint16_t usConnId,
						 uint8_t ucServerIf,
						 bool bConnected,
						 BTBdaddr_t * pxBda )
{
	BLETESTConnectionCallback_t * pxConnectionCallback = pvPortMalloc(sizeof(BLETESTConnectionCallback_t));

	xCbStatus = eBTStatusSuccess;

	if(pxConnectionCallback != NULL)
	{
		pxConnectionCallback->bConnected = bConnected;
		if(pxBda != NULL)
		{
			memcpy(&pxConnectionCallback->pxBda, pxBda, sizeof(BTBdaddr_t));
			memcpy(&xAddressConnectedDevice, pxBda, sizeof(BTBdaddr_t));
		}else
		{
			xCbStatus = eBTStatusFail;
			memset(&pxConnectionCallback->pxBda, 0, sizeof(BTBdaddr_t));
			memset(&xAddressConnectedDevice, 0, sizeof(BTBdaddr_t));
		}
		pxConnectionCallback->ucServerIf = ucServerIf;
		pxConnectionCallback->usConnId = usConnId;
		pxConnectionCallback->xEvent = eBLEHALEventConnectionCb;
		usBLEConnId = usConnId;
		xQueueSend( xCallbackQueue, ( void * ) &pxConnectionCallback, ( TickType_t ) 0 );
	}
}

void prvConnParameterUpdateCb (BTStatus_t xStatus,
								  const BTBdaddr_t * pxBdAddr,
								  uint32_t ulMinInterval,
								  uint32_t ulMaxInterval,
								  uint32_t ulLatency,
								  uint32_t usConnInterval,
								  uint32_t ulTimeout )
{
	xCbStatus = xStatus;
	xCbConnectionParam.ulMinInterval = ulMinInterval;
	xCbConnectionParam.ulMaxInterval = ulMaxInterval;
	xCbConnectionParam.ulLatency = ulLatency;
	xCbConnectionParam.ulTimeout = ulTimeout;

	if(pxBdAddr != NULL)
	{
		memcpy(&xCbBda, pxBdAddr, sizeof(BTBdaddr_t));
	}else
	{
		memset(&xCbBda, 0, sizeof(BTBdaddr_t));
	}
	usCbConnInterval = usConnInterval;

	(void)xEventGroupSetBits((EventGroupHandle_t)&xWaitOperationComplete, 1<<eBLEHALEventConnectionUpdateCb);
}
void prvRequestReadCb( uint16_t usConnId,
					  uint32_t ulTransId,
					  BTBdaddr_t * pxBda,
					  uint16_t usAttrHandle,
					  uint16_t usOffset )
{
	BLETESTreadAttrCallback_t * pxreadAttrCallback = pvPortMalloc(sizeof(BLETESTreadAttrCallback_t));

	if(pxreadAttrCallback != NULL)
	{
		pxreadAttrCallback->usConnId = usConnId;
		pxreadAttrCallback->ulTransId = ulTransId;

		if(pxBda != NULL)
		{
			memcpy(&pxreadAttrCallback->xBda, pxBda, sizeof(BTBdaddr_t));
		}else
		{
			memset(&pxreadAttrCallback->xBda, 0, sizeof(BTBdaddr_t));
		}
		pxreadAttrCallback->usAttrHandle = usAttrHandle;
		pxreadAttrCallback->usOffset = usOffset;
		pxreadAttrCallback->xEvent = eBLEHALEventReadAttrCb;

		xQueueSend( xCallbackQueue, ( void * ) &pxreadAttrCallback, ( TickType_t ) 0 );
	}
}

void prvRequestWriteCb( uint16_t usConnId,
					    uint32_t ulTransId,
					    BTBdaddr_t * pxBda,
					    uint16_t usAttrHandle,
					    uint16_t usOffset,
					    size_t xLength,
					    bool bNeedRsp,
 					    bool bIsPrep,
 					    uint8_t * pucValue )
{
	BLETESTwriteAttrCallback_t * pxWriteAttrCallback = pvPortMalloc(sizeof(BLETESTwriteAttrCallback_t));

	if(pxWriteAttrCallback != NULL)
	{
		pxWriteAttrCallback->usConnId = usConnId;
		pxWriteAttrCallback->ulTransId = ulTransId;

		if(pxBda != NULL)
		{
			memcpy(&pxWriteAttrCallback->xBda, pxBda, sizeof(BTBdaddr_t));
		}else
		{
			memset(&pxWriteAttrCallback->xBda, 0, sizeof(BTBdaddr_t));
		}
		pxWriteAttrCallback->usOffset = usOffset;
		pxWriteAttrCallback->xLength = xLength;
		pxWriteAttrCallback->bNeedRsp = bNeedRsp;
		pxWriteAttrCallback->bIsPrep = bIsPrep;
		pxWriteAttrCallback->usAttrHandle = usAttrHandle;
		if(( xLength <= bletestsSTRINGYFIED_UUID_SIZE)&&(pucValue != NULL))
		{
			memcpy( pxWriteAttrCallback->ucValue , pucValue, xLength);
		}else
		{
			memset( pxWriteAttrCallback->ucValue , 0, bletestsSTRINGYFIED_UUID_SIZE);
		}
		pxWriteAttrCallback->xEvent = eBLEHALEventWriteAttrCb;

		xQueueSend( xCallbackQueue, ( void * ) &pxWriteAttrCallback, ( TickType_t ) 0 );
	}
/*
	BTGattResponse_t xGattResponse;

	if ( xLength <= bletestsSTRINGYFIED_UUID_SIZE)
	{
		memcpy(ucResponseBuffer, pucValue, bletestsSTRINGYFIED_UUID_SIZE);
	}else
	{
		memset(ucResponseBuffer, 0, bletestsSTRINGYFIED_UUID_SIZE);
	}
	xGattResponse.usHandle = usAttrHandle;
	xGattResponse.xAttrValue.pucValue = ucResponseBuffer;
	xGattResponse.xAttrValue.xLen = bletestsSTRINGYFIED_UUID_SIZE;
	xGattResponse.xAttrValue.usOffset = 0;
	xGattResponse.xAttrValue.xRspErrorStatus = eBTRspErrorNone;
*/
}

/* This function first check if an event is waiting in the list. If not, it will go and wait on the queue.
 * When an event is received on the queue, if it is not the expected event, it goes on the waiting list.
 */
BTStatus_t prvWaitEventFromQueue( BLEHALEventsInternals_t xEventName, void * pxMessage, size_t xMessageLength, TickType_t xTestWait)
{
	BLEHALEventsInternals_t xEvent;
	BTStatus_t xStatus = eBTStatusSuccess;
	void * pvPtr = NULL;
	Link_t *pxTmpLink;
	xWaitingEvents_t * pxTmpEvent;


	/* Search for event in the waiting list */
	listFOR_EACH( pxTmpLink, &( xWaitingEventQueue ) )
	{
		pxTmpEvent = listCONTAINER( pxTmpLink, xWaitingEvents_t, xNextQueueItem );
		if(*((BLEHALEventsInternals_t *)pxTmpEvent->pxNextQueuedItem) == xEventName)
		{
			pvPtr = pxTmpEvent->pxNextQueuedItem;
			listREMOVE(pxTmpLink);
			vPortFree(pxTmpEvent);
			break;/* If the right event is received, exit. */
		}
	}

	/* If event is not waiting then wait for it. */
	if(pvPtr == NULL)
	{
		do{
			/* TODO check event list here */
			if( xQueueReceive( xCallbackQueue, &pvPtr, ( TickType_t ) xTestWait ) == pdPASS)
			{
				xEvent = *((BLEHALEventsInternals_t *)pvPtr);

				if(xEvent == xEventName){
					break;/* If the right event is received, exit. */
				}else
				{
					/* If this is not the event, push it to the waiting list */
					pxTmpEvent = pvPortMalloc(sizeof(xWaitingEvents_t));
					pxTmpEvent->pxNextQueuedItem = pvPtr;
					listADD(&xWaitingEventQueue, &pxTmpEvent->xNextQueueItem );
				}
			}else
			{
				xStatus = eBTStatusFail;
			}

		}while(xStatus == eBTStatusSuccess);/* If there is an error exit */
	}

	if(xStatus == eBTStatusSuccess)
	{
		memcpy(pxMessage, pvPtr, xMessageLength);
		vPortFree(pvPtr);
	}

	return xStatus;
}

void prvIndicationSentCb( uint16_t usConnId,
                          BTStatus_t xStatus )
{
	BLETESTindicateCallback_t * pxIndicateCallback = pvPortMalloc(sizeof(BLETESTindicateCallback_t));

	if(pxIndicateCallback != NULL)
	{
		pxIndicateCallback->xEvent = eBLEHALEventIndicateCb;
		pxIndicateCallback->usConnId = usConnId;
		pxIndicateCallback->xStatus = xStatus;

		xQueueSend( xCallbackQueue, ( void * ) &pxIndicateCallback, ( TickType_t ) 0 );
	}
}

void prvResponseConfirmationCb( BTStatus_t xStatus,
                                uint16_t usHandle )
{
	BLETESTconfirmCallback_t * pxConfirmCallback = pvPortMalloc(sizeof(BLETESTconfirmCallback_t));

	if(pxConfirmCallback != NULL)
	{
		pxConfirmCallback->xEvent = eBLEHALEventConfimCb;
		pxConfirmCallback->usAttrHandle = usHandle;
		pxConfirmCallback->xStatus = xStatus;

		xQueueSend( xCallbackQueue, ( void * ) &pxConfirmCallback, ( TickType_t ) 0 );
	}
}

void prvSspRequestCb( BTBdaddr_t * pxRemoteBdAddr,
                             BTBdname_t * pxRemoteBdName,
                             uint32_t ulCod,
                             BTSspVariant_t xPairingVariant,
                             uint32_t ulPassKey )
{
	BLETESTsspRequestCallback_t * pxSSPrequestCallback = pvPortMalloc(sizeof(BLETESTsspRequestCallback_t));

	if(pxSSPrequestCallback != NULL)
	{
		if(pxRemoteBdAddr != NULL)
		{
			memcpy(&pxSSPrequestCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof(BTBdaddr_t));
		}else
		{
			memset(&pxSSPrequestCallback->xRemoteBdAddr, 0, sizeof(BTBdaddr_t));
		}

		pxSSPrequestCallback->ulCod = ulCod;
		pxSSPrequestCallback->xPairingVariant = xPairingVariant;
		pxSSPrequestCallback->ulPassKey = ulPassKey;
		pxSSPrequestCallback->xEvent = eBLEHALEventSSPrequestCb;
		xQueueSend( xCallbackQueue, ( void * ) &pxSSPrequestCallback, ( TickType_t ) 0 );
	}
}
        
void prvPairingStateChangedCb( BTStatus_t xStatus,
							   BTBdaddr_t * pxRemoteBdAddr,
                                                           BTBondState_t xState,
							   BTSecurityLevel_t xSecurityLevel,
							   BTAuthFailureReason_t xReason )
{
	BLETESTPairingStateChangedCallback_t * pxPairingStateChangedCallback = pvPortMalloc(sizeof(BLETESTPairingStateChangedCallback_t));

	if(pxPairingStateChangedCallback != NULL)
	{
		pxPairingStateChangedCallback->xEvent = eBLEHALEventPairingStateChangedCb;
		pxPairingStateChangedCallback->xStatus = xStatus;
		if(pxRemoteBdAddr != NULL)
		{
			memcpy(&pxPairingStateChangedCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof(BTBdaddr_t));
		}else
		{
			memset(&pxPairingStateChangedCallback->xRemoteBdAddr, 0, sizeof(BTBdaddr_t));
		}
		pxPairingStateChangedCallback->xSecurityLevel = xSecurityLevel;
		pxPairingStateChangedCallback->xReason = xReason;
		xQueueSend( xCallbackQueue, ( void * ) &pxPairingStateChangedCallback, ( TickType_t ) 0 );
	}
}

void prvRequestExecWriteCb( uint16_t usConnId,
                            uint32_t ulTransId,
                            BTBdaddr_t * pxBda,
                            bool bExecWrite )
{
	BLETESTRequestExecWriteCallback_t * pxRequestExecWriteCallback = pvPortMalloc(sizeof(BLETESTRequestExecWriteCallback_t));

	if(pxRequestExecWriteCallback != NULL)
	{
		pxRequestExecWriteCallback->xEvent = eBLEHALEventRequestExecWriteCb;
		pxRequestExecWriteCallback->ulTransId = ulTransId;
		pxRequestExecWriteCallback->usConnId = usConnId;
		if(pxBda != NULL)
		{
			memcpy(&pxRequestExecWriteCallback->xBda, pxBda, sizeof(BTBdaddr_t));
		}else
		{
			memset(&pxRequestExecWriteCallback->xBda, 0, sizeof(BTBdaddr_t));
		}
		pxRequestExecWriteCallback->bExecWrite = bExecWrite;

		xQueueSend( xCallbackQueue, ( void * ) &pxRequestExecWriteCallback, ( TickType_t ) 0 );
	}
}

void prvBondedCb( BTStatus_t xStatus,
        BTBdaddr_t * pxRemoteBdAddr,
        bool bIsBonded )
{
	BLETESTBondedCallback_t * pxBondedCallback = pvPortMalloc(sizeof(BLETESTBondedCallback_t));

	if(pxBondedCallback != NULL)
	{
		pxBondedCallback->bIsBonded = bIsBonded;
		if(pxRemoteBdAddr != NULL)
		{
			memcpy(&pxBondedCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof(BTBdaddr_t));
		}else
		{
			memset(&pxBondedCallback->xRemoteBdAddr, 0, sizeof(BTBdaddr_t));
		}
		pxBondedCallback->xStatus = xStatus;
		pxBondedCallback->xEvent = eBLEHALEventBondedCb;

		xQueueSend( xCallbackQueue, ( void * ) &pxBondedCallback, ( TickType_t ) 0 );
	}

}
