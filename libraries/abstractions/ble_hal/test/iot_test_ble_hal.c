/*
 * Amazon FreeRTOS BLE HAL V2.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include "iot_config.h"
/* C standard library includes. */
#include <stddef.h>
#include <string.h>
#include "iot_linear_containers.h"
#include "platform/iot_threads.h"
#include "types/iot_platform_types.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "bt_hal_gatt_types.h"
/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @brief Connection parameters.
 */
typedef struct
{
    uint32_t minInterval; /**< Minimum connection interval. */
    uint32_t maxInterval; /**< Maximum connection interval. */
    uint32_t latency;     /**< Slave latency. */
    uint32_t timeout;     /**< Connection timeout. */
} IotBleConnectionParam_t;

typedef enum
{
    eBLEHALEventServerRegisteredCb = 0,
    eBLEHALEventEnableDisableCb = 1,
    eBLEHALEventCharAddedCb = 2,
    eBLEHALEventServiceAddedCb = 3,
    eBLEHALEventServiceStartedCb = 4,
    eBLEHALEventServiceStoppedCb = 5,
    eBLEHALEventServiceDeletedCb = 6,
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
    eBLEHALEventSSPrequestConfirmationCb = 22,
    eBLEHALEventPairingStateChangedCb = 23,
    eBLEHALEventRequestExecWriteCb = 24,
    eBLEHALEventBondedCb = 25,
    eBLENbHALEvents,
} BLEHALEventsTypes_t;

typedef struct
{
    IotLink_t eventList;
    BLEHALEventsTypes_t xEventTypes;
    int32_t lHandle;
} BLEHALEventsInternals_t;

#define NO_HANDLE                        -1

#define MAX_EVENT                        20

#define bletestsDEFAULT_CHAR_VALUE       "hello"
#define bletestsSTRINGYFIED_UUID_SIZE    36 /* like "8a7f1168-48af-4efb-83b5-e679f9320002" */
#define bletestsFULL_PERMISSIONS         ( eBTPermRead | eBTPermWrite )
#define bletestsNB_INCLUDEDSERVICES      1

#define bletestsAPP_UUID                 { 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsSERVER_UUID              { 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsFREERTOS_SVC_UUID        { 0x5A, 0xDB, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }

#define bletestsCCCD            \
    {                           \
        .uu.uu16 = 0x2902,      \
        .ucType = eBTuuidType16 \
    }

#define bletestsFREERTOS_SVC_A_UUID                                                                                     \
    {                                                                                                                   \
        .uu.uu128 = { 0x00, 0x00, 0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31 }, \
        .ucType = eBTuuidType128                                                                                        \
    }

#define bletestsFREERTOS_CHAR_DUMMY_UUID                                                                                \
    {                                                                                                                   \
        .uu.uu128 = { 0x01, 0x00, 0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31 }, \
        .ucType = eBTuuidType128                                                                                        \
    }

#define bletestsFREERTOS_SVC_B_UUID                                                                               \
    {                                                                                                             \
        .uu.uu128 = { 1, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_CHAR_A_UUID                                                                              \
    {                                                                                                             \
        .uu.uu128 = { 2, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_CHAR_B_UUID                                                                              \
    {                                                                                                             \
        .uu.uu128 = { 3, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_CHAR_C_UUID                                                                              \
    {                                                                                                             \
        .uu.uu128 = { 4, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_CHAR_D_UUID                                                                              \
    {                                                                                                             \
        .uu.uu128 = { 5, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_CHAR_E_UUID                                                                              \
    {                                                                                                             \
        .uu.uu128 = { 6, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_CHAR_F_UUID                                                                              \
    {                                                                                                             \
        .uu.uu128 = { 7, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }

#define bletestsFREERTOS_DESCR_A_UUID                                                                             \
    {                                                                                                             \
        .uu.uu128 = { 8, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }
#define bletestsFREERTOS_DESCR_B_UUID                                                                             \
    {                                                                                                             \
        .uu.uu128 = { 9, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                  \
    }
#define bletestsFREERTOS_DESCR_C_UUID                                                                              \
    {                                                                                                              \
        .uu.uu128 = { 10, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                   \
    }
#define bletestsFREERTOS_DESCR_D_UUID                                                                              \
    {                                                                                                              \
        .uu.uu128 = { 11, 0, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }, \
        .ucType = eBTuuidType128                                                                                   \
    }


#define bletestsDEVICE_NAME                   "TEST"
#define bletestsMAX_PROPERTY_SIZE             30

#define bletestsMTU_SIZE1                     200
#define bletestsMAX_ADVERTISEMENT_INTERVAL    64

#define bletestsMIN_CONNECTION_INTERVAL       32
#define bletestsMAX_CONNECTION_INTERVAL       64

#define QUEUE_LENGTH                          20
#define ITEM_SIZE                             sizeof( void * )

static IotListDouble_t eventQueueHead;
static IotMutex_t threadSafetyMutex;
static IotSemaphore_t eventSemaphore;

static BTInterface_t * pxBTInterface;
static BTBleAdapter_t * pxBTLeAdapterInterface;
static BTUuid_t xServerUUID =
{
    .ucType   = eBTuuidType128,
    .uu.uu128 = bletestsSERVER_UUID
};
static BTUuid_t xAppUUID =
{
    .ucType   = eBTuuidType128,
    .uu.uu128 = bletestsAPP_UUID
};
static uint8_t ucBLEAdapterIf;
static uint8_t ucBLEServerIf;
static BTBdaddr_t xAddressConnectedDevice;
static BTGattServerInterface_t * pxGattServerInterface;
static uint16_t usBLEConnId;

static uint8_t ucCbPropertyBuffer[ bletestsMAX_PROPERTY_SIZE ];
static uint32_t usCbConnInterval;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    BTState_t xBLEState;
} BLETESTInitDeinitCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    uint16_t usAttrHandle;
    uint16_t usSrvHandle;
    BTUuid_t xUUID;
} BLETESTAttrCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    uint16_t usAttrHandle;
    BTStatus_t xStatus;
    BTGattSrvcId_t xSrvcId;
} BLETESTServiceCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    uint32_t ulNumProperties;
    BTProperty_t xProperties;
} BLETESTSetGetPropertyCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    bool bStart;
} BLETESTAdvParamCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    IotBleConnectionParam_t xConnParam;
    BTBdaddr_t xBda;
} BLETESTUpdateConnectionParamReqCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    uint16_t usConnId;
    uint32_t ulTransId;
    BTBdaddr_t xBda;
    uint16_t usAttrHandle;
    uint16_t usOffset;
} BLETESTreadAttrCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    uint16_t usConnId;
    uint32_t ulTransId;
    BTBdaddr_t xBda;
    uint16_t usAttrHandle;
    uint16_t usOffset;
    size_t xLength;
    bool bNeedRsp;
    bool bIsPrep;
    uint8_t ucValue[ bletestsSTRINGYFIED_UUID_SIZE ];
} BLETESTwriteAttrCallback_t;


typedef struct
{
    BLEHALEventsInternals_t xEvent;
    uint16_t usConnId;
    BTStatus_t xStatus;
} BLETESTindicateCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    uint16_t usAttrHandle;
} BLETESTconfirmCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTBdaddr_t xRemoteBdAddr;
    BTBdname_t xRemoteBdName;
    uint32_t ulCod;
    BTSspVariant_t xPairingVariant;
    uint32_t ulPassKey;
} BLETESTsspRequestCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    BTBdaddr_t xRemoteBdAddr;
    BTSecurityLevel_t xSecurityLevel;
    BTAuthFailureReason_t xReason;
} BLETESTPairingStateChangedCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    uint16_t usConnId;
    uint32_t ulTransId;
    BTBdaddr_t xBda;
    bool bExecWrite;
} BLETESTRequestExecWriteCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    uint16_t usConnId;
    uint8_t ucServerIf;
    BTBdaddr_t pxBda;
    bool bConnected;
} BLETESTConnectionCallback_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    BTStatus_t xStatus;
    BTBdaddr_t xRemoteBdAddr;
    bool bIsBonded;
} BLETESTBondedCallback_t;

typedef enum
{
    bletestATTR_SRVCA_SERVICE,
    bletestATTR_SRVCA_CHAR_A,
    bletestATTR_SRVCA_NUMBER
} bletestAttSrvA_t;
static uint16_t usHandlesBufferA[ bletestATTR_SRVCA_NUMBER ];

static const BTAttribute_t pxAttributeTableA[] =
{
    {
        .xServiceUUID = bletestsFREERTOS_SVC_A_UUID
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_DUMMY_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    }
};

BTService_t xSrvcA =
{
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .xNumberOfAttributes = bletestATTR_SRVCA_NUMBER,
    .pusHandlesBuffer    = usHandlesBufferA,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTableA
};

typedef enum
{
    bletestATTR_SRVCB_SERVICE,
    bletestATTR_SRVCB_CHAR_A,
    bletestATTR_SRVCB_CHAR_B,
    bletestATTR_SRVCB_CHAR_C,
    bletestATTR_SRVCB_CHAR_D,
    bletestATTR_SRVCB_CHAR_E,
    bletestATTR_SRVCB_CCCD_E,
    bletestATTR_SRVCB_CHAR_F,
    bletestATTR_SRVCB_CCCD_F,
    bletestATTR_SRVCB_CHARF_DESCR_A,
    bletestATTR_SRVCB_CHARF_DESCR_B,
    bletestATTR_SRVCB_CHARF_DESCR_C,
    bletestATTR_SRVCB_CHARF_DESCR_D,
    bletestATTR_INCLUDED_SERVICE,
    bletestATTR_SRVCB_NUMBER
} bletestAttSrvB_t;

typedef struct
{
    size_t xLength;
    uint8_t ucBuffer[ bletestsSTRINGYFIED_UUID_SIZE ];
} response_t;

response_t ucRespBuffer[ bletestATTR_SRVCB_NUMBER ];

static uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];

static const BTAttribute_t pxAttributeTableB[] =
{
    {
        .xServiceUUID = bletestsFREERTOS_SVC_B_UUID
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_A_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_B_UUID,
            .xPermissions = ( eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_C_UUID,
            .xPermissions = ( eBTPermReadEncrypted | eBTPermWriteEncrypted ),
            .xProperties  = eBTPropRead | eBTPropWrite
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_D_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropWriteNoResponse )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_E_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropNotify )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsCCCD,
            .xPermissions = ( eBTPermRead | eBTPermWrite )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_F_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropIndicate )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsCCCD,
            .xPermissions = ( eBTPermRead | eBTPermWrite )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_A_UUID,
            .xPermissions = ( eBTPermRead | eBTPermWrite )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_B_UUID,
            .xPermissions = ( eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_C_UUID,
            .xPermissions = ( eBTPermReadEncrypted | eBTPermWriteEncrypted )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_D_UUID,
            .xPermissions = ( eBTPermRead )
        }
    },
    {
        .xAttributeType = eBTDbIncludedService,
        .xIncludedService =
        {
            .xUuid          = bletestsFREERTOS_SVC_A_UUID,
            .pxPtrToService = &xSrvcA
        }
    }
};

BTService_t xSrvcB =
{
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .xNumberOfAttributes = bletestATTR_SRVCB_NUMBER,
    .pusHandlesBuffer    = usHandlesBufferB,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTableB
};


BTGattAdvertismentParams_t xAdvertisementConfigA =
{
    .usAdvertisingEventProperties = BTAdvInd,
    .bIncludeTxPower              = true,
    .ucName                       = { BTGattAdvNameNone,                   0},
    .bSetScanRsp                  = false,
    .ulAppearance                 = 0,
    .ulMinInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL / 2,
    .ulMaxInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL,
    .ucChannelMap                 = 0,
    .ucPrimaryAdvertisingPhy      = 0,
    .ucSecondaryAdvertisingPhy    = 0,
    .xAddrType                    = BTAddrTypePublic,
};

BTGattAdvertismentParams_t xAdvertisementConfigB =
{
    .usAdvertisingEventProperties = BTAdvInd,
    .bIncludeTxPower              = true,
    .ucName                       = { BTGattAdvNameShort,                  4},
    .bSetScanRsp                  = true,
    .ulAppearance                 = 0,
    .ulMinInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL / 2,
    .ulMaxInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL,
    .ucChannelMap                 = 0,
    .ucPrimaryAdvertisingPhy      = 0,
    .ucSecondaryAdvertisingPhy    = 0,
    .xAddrType                    = BTAddrTypePublic,
};

IotBleConnectionParam_t xConnectionParamA =
{
    .minInterval = bletestsMIN_CONNECTION_INTERVAL,
    .maxInterval = bletestsMAX_CONNECTION_INTERVAL,
    .latency     = 4,
    .timeout     = 400
};

const uint32_t bletestWAIT_MODE1_LEVEL2_QUERY = 10000; /* Wait 10s max */
const uint32_t BLE_TESTS_WAIT = 60000;                 /* Wait 60s max */
const uint32_t BLE_TESTS_SHORT_WAIT = 4000;            /* Wait 4s max */

void pushToQueue( IotLink_t * pEventList );
static void prvSetGetProperty( BTProperty_t * pxProperty,
                               bool bIsSet );
static void prvStartAdvertisement( void );
static BTStatus_t prvWaitEventFromQueue( BLEHALEventsTypes_t xEventName,
                                         int32_t lhandle,
                                         void * pxMessage,
                                         size_t xMessageLength,
                                         uint32_t timeoutMs );
static void prvWriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                                      bool bNeedRsp,
                                      bool IsPrep,
                                      uint16_t usOffset );
static void prvReadCheckAndResponse( bletestAttSrvB_t xAttribute );
static void prvDeviceStateChangedCb( BTState_t xState );
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
static void prvServiceStartedCb( BTStatus_t xStatus,
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
void prvAdvStatusCb( BTStatus_t xStatus,
                     uint32_t ulServerIf,
                     bool bStart );
void prvConnectionCb( uint16_t usConnId,
                      uint8_t ucServerIf,
                      bool bConnected,
                      BTBdaddr_t * pxBda );
void prvConnParameterUpdateCb( BTStatus_t xStatus,
                               const BTBdaddr_t * pxBdAddr,
                               uint32_t minInterval,
                               uint32_t maxInterval,
                               uint32_t latency,
                               uint32_t usConnInterval,
                               uint32_t timeout );
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
void prvStartStopAdvCheck( bool start );
BTStatus_t bleStackInit( void );

static BTCallbacks_t xBTManagerCb =
{
    .pxDeviceStateChangedCb     = prvDeviceStateChangedCb,
    .pxAdapterPropertiesCb      = prvAdapterPropertiesCb,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = prvSspRequestCb,
    .pxPairingStateChangedCb    = prvPairingStateChangedCb,
    .pxBondedCb                 = prvBondedCb,
    .pxDutModeRecvCb            = NULL,
    .pxleTestModeCb             = NULL,
    .pxEnergyInfoCb             = NULL,
    .pxReadRssiCb               = NULL,
    .pxTxPowerCb                = NULL,
    .pxSlaveSecurityRequestCb   = NULL,
};

static BTBleAdapterCallbacks_t xBTBleAdapterCb =
{
    .pxRegisterBleAdapterCb          = prvRegisterBleAdapterCb,
    .pxScanResultCb                  = NULL,
    .pxBleAdapterPropertiesCb        = NULL,
    .pxBleRemoteDevicePropertiesCb   = NULL,
    .pxOpenCb                        = NULL,
    .pxCloseCb                       = NULL,
    .pxReadRemoteRssiCb              = NULL,
    .pxAdvStatusCb                   = prvAdvStatusCb,
    .pxSetAdvDataCb                  = prvSetAdvDataCb,
    .pxConnParameterUpdateCb         = prvConnParameterUpdateCb,
    .pxScanFilterCfgCb               = NULL,
    .pxScanFilterParamCb             = NULL,
    .pxScanFilterStatusCb            = NULL,
    .pxMultiAdvEnableCb              = NULL,
    .pxMultiAdvUpdateCb              = NULL,
    .pxMultiAdvDataCb                = NULL,
    .pxMultiAdvDisableCb             = NULL,
    .pxCongestionCb                  = NULL,
    .pxBatchscanCfgStorageCb         = NULL,
    .pxBatchscanEnbDisableCb         = NULL,
    .pxBatchscanReportsCb            = NULL,
    .pxBatchscanThresholdCb          = NULL,
    .pxTrackAdvEventCb               = NULL,
    .pxScanParameterSetupCompletedCb = NULL,
    .pxPhyUpdatedCb                  = NULL,
};


static BTGattServerCallbacks_t xBTGattServerCb =
{
    .pxRegisterServerCb       = prvBTRegisterServerCb,
    .pxUnregisterServerCb     = prvBTUnregisterServerCb,
    .pxConnectionCb           = prvConnectionCb,
    .pxServiceAddedCb         = prvServiceAddedCb,
    .pxIncludedServiceAddedCb = prvIncludedServiceAddedCb,
    .pxCharacteristicAddedCb  = prvCharacteristicAddedCb,
    .pxSetValCallbackCb       = NULL,
    .pxDescriptorAddedCb      = prvCharacteristicDescrAddedCb,
    .pxServiceStartedCb       = prvServiceStartedCb,
    .pxServiceStoppedCb       = prvServiceStoppedCb,
    .pxServiceDeletedCb       = prvServiceDeletedCb,
    .pxRequestReadCb          = prvRequestReadCb,
    .pxRequestWriteCb         = prvRequestWriteCb,
    .pxRequestExecWriteCb     = prvRequestExecWriteCb,
    .pxResponseConfirmationCb = prvResponseConfirmationCb,
    .pxIndicationSentCb       = prvIndicationSentCb,
    .pxCongestionCb           = NULL,
    .pxMtuChangedCb           = NULL
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

    RUN_TEST_CASE( Full_BLE, BLE_Setup );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_common_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GATT );


    RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_CreateServices );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetProperties ); /*@TOTO, incomplete */
    RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveAllBonds );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetAvertisementData ); /*@TOTO, incomplete */
    RUN_TEST_CASE( Full_BLE, BLE_Advertising_StartAdvertisement );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_SimpleConnection );
/*RUN_TEST_CASE( Full_BLE, BLE_Connection_UpdateConnectionParamReq ); */

/*RUN_TEST_CASE( Full_BLE, BLE_Connection_ChangeMTUsize ); */
    RUN_TEST_CASE( Full_BLE, BLE_Property_WriteCharacteristic );
    RUN_TEST_CASE( Full_BLE, BLE_Property_WriteDescriptor );
    RUN_TEST_CASE( Full_BLE, BLE_Property_ReadCharacteristic );
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

    RUN_TEST_CASE( Full_BLE, BLE_Connection_CheckBonding );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveBonding );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level2 );

    RUN_TEST_CASE( Full_BLE, BLE_DeInitialize );
    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

void prvRemoveBond( BTBdaddr_t * pxDeviceAddress )
{
    BTStatus_t xStatus;
    BLETESTBondedCallback_t xBondedEvent;

    xStatus = pxBTInterface->pxRemoveBond( pxDeviceAddress );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( false, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, pxDeviceAddress, sizeof( BTBdaddr_t ) ) );
}

TEST( Full_BLE, BLE_Setup )
{
    BTStatus_t xStatus;

    xStatus = bleStackInit();

    if( xStatus != eBTStatusSuccess )
    {
        TEST_FAIL_MESSAGE( "Unable to initialize BLE stask.\n" );
    }

    /* Create a queue, semaphore and mutexes for callbacks. */
    if( IotMutex_Create( &threadSafetyMutex, false ) != true )
    {
        TEST_FAIL_MESSAGE( "Could not create threadSafetyMutex.\n" );
    }

    if( IotSemaphore_Create( &eventSemaphore, 0, MAX_EVENT ) != true )
    {
        TEST_FAIL_MESSAGE( "Could not create eventSemaphore.\n" );
    }

    IotListDouble_Create( &eventQueueHead );
}

TEST( Full_BLE, BLE_Free )
{
    BLEHALEventsInternals_t * pEventIndex;
    IotLink_t * pEventListIndex;

    IotMutex_Lock( &threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &eventQueueHead, pEventListIndex )
    {
        pEventIndex = IotLink_Container( BLEHALEventsInternals_t, pEventListIndex, eventList );
        IotTest_Free( pEventIndex );
    }

    IotMutex_Unlock( &threadSafetyMutex );

    IotMutex_Destroy( &threadSafetyMutex );
    IotSemaphore_Destroy( &eventSemaphore );
}

TEST( Full_BLE, BLE_Connection_RemoveAllBonds )
{
    BTProperty_t pxProperty;
    uint16_t usIndex;

    /* Set the name */
    pxProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Get bonded devices */
    prvSetGetProperty( &pxProperty, false );

    for( usIndex = 0; usIndex < pxProperty.xLen; usIndex++ )
    {
        prvRemoveBond( &( ( BTBdaddr_t * ) pxProperty.pvVal )[ usIndex ] );
    }

    /* Get bonded devices. */
    prvSetGetProperty( &pxProperty, false );
    /* Check none are left. */
    TEST_ASSERT_EQUAL( 0, pxProperty.xLen );
}

bool prvGetCheckDeviceBonded( BTBdaddr_t * pxDeviceAddress )
{
    BTProperty_t pxProperty;
    uint16_t usIndex;
    bool bFoundRemoteDevice = false;

    pxProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Set the name */
    prvSetGetProperty( &pxProperty, false );

    for( usIndex = 0; usIndex < pxProperty.xLen; usIndex++ )
    {
        if( 0 == memcmp( &( ( BTBdaddr_t * ) pxProperty.pvVal )[ usIndex ], pxDeviceAddress, sizeof( BTBdaddr_t ) ) )
        {
            bFoundRemoteDevice = true;
            break;
        }
    }

    return bFoundRemoteDevice;
}

void prvWaitConnection( bool bConnected )
{
    BLETESTConnectionCallback_t xConnectionEvent;
    BTStatus_t xStatus;

    xStatus = prvWaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( bConnected, xConnectionEvent.bConnected );
    TEST_ASSERT_EQUAL( ucBLEServerIf, xConnectionEvent.ucServerIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConnectionEvent.xStatus );

    /* Stop advertisement. */
    xStatus = pxBTLeAdapterInterface->pxStopAdv( ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    prvStartStopAdvCheck( false );
}

TEST( Full_BLE, BLE_Connection_Mode1Level2 )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTsspRequestCallback_t xSSPrequestEvent;

    prvStartAdvertisement();
    prvWaitConnection( true );

    xStatus = prvWaitEventFromQueue( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

    xStatus = pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), bletestWAIT_MODE1_LEVEL2_QUERY );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );                        /* Pairing should never come since it is secure connection only */
    TEST_ASSERT_EQUAL( eBTStatusFail, xPairingStateChangedEvent.xStatus ); /* Pairing should never come since it is secure connection only */
    /* @TODO add correct flag */
    /* TEST_ASSERT_EQUAL(eBTauthFailInsuffSecurity, xPairingStateChangedEvent.xReason); */ /* Pairing should never come since it is secure connection only */
}

TEST( Full_BLE, BLE_Connection_RemoveBonding )
{
    bool bFoundRemoteDevice;

    prvRemoveBond( &xAddressConnectedDevice );

    bFoundRemoteDevice = prvGetCheckDeviceBonded( &xAddressConnectedDevice );
    TEST_ASSERT_EQUAL( false, bFoundRemoteDevice );
}


TEST( Full_BLE, BLE_Connection_CheckBonding )
{
    bool bFoundRemoteDevice;

    bFoundRemoteDevice = prvGetCheckDeviceBonded( &xAddressConnectedDevice );
    TEST_ASSERT_EQUAL( true, bFoundRemoteDevice );
}



TEST( Full_BLE, BLE_Connection_BondedReconnectAndPair )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTBondedCallback_t xBondedEvent;

    prvStartAdvertisement();
    prvWaitConnection( true );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );

    xStatus = prvWaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( true, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                              true,
                              false,
                              0 );
}


TEST( Full_BLE, BLE_Connection_Disconnect )
{
    prvWaitConnection( false );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteDescr )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_B,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteChar )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4 )
{
    BTStatus_t xStatus;
    BLETESTsspRequestCallback_t xSSPrequestEvent;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTBondedCallback_t xBondedEvent;

    /* Wait secure connection. Secure connection is triggered by writting to bletestsCHARB. */
    xStatus = prvWaitEventFromQueue( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

    xStatus = pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventSSPrequestConfirmationCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantPasskeyConfirmation, xSSPrequestEvent.xPairingVariant );

    xStatus = pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantPasskeyConfirmation, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );

    xStatus = prvWaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( true, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                              true,
                              false,
                              0 );
}


void prvWriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                               bool bNeedRsp,
                               bool IsPrep,
                               uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;

    /* Wait write event on char A*/
    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( IsPrep, xWriteEvent.bIsPrep );
    TEST_ASSERT_EQUAL( bNeedRsp, xWriteEvent.bNeedRsp );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( usOffset, xWriteEvent.usOffset );
    TEST_ASSERT_EQUAL( bletestsSTRINGYFIED_UUID_SIZE, xWriteEvent.xLength );

    if( bNeedRsp == true )
    {
        xGattResponse.usHandle = xWriteEvent.usAttrHandle;
        xGattResponse.xAttrValue.usHandle = xWriteEvent.usAttrHandle;
        xGattResponse.xAttrValue.usOffset = xWriteEvent.usOffset;
        xGattResponse.xAttrValue.xLen = xWriteEvent.xLength;
        ucRespBuffer[ xAttribute ].xLength = xWriteEvent.xLength;
        memcpy( ucRespBuffer[ xAttribute ].ucBuffer, xWriteEvent.ucValue, xWriteEvent.xLength );
        xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer;
        xStatus = pxGattServerInterface->pxSendResponse( xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = prvWaitEventFromQueue( eBLEHALEventConfimCb, xWriteEvent.usAttrHandle, ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
        TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
    }
}

void prvSendNotification( bletestAttSrvB_t xAttribute,
                          bool bConfirm )
{
    BTStatus_t xStatus;

    xStatus = pxGattServerInterface->pxSendIndication( ucBLEServerIf,
                                                       usHandlesBufferB[ xAttribute ],
                                                       usBLEConnId,
                                                       ucRespBuffer[ xAttribute ].xLength,
                                                       ucRespBuffer[ xAttribute ].ucBuffer,
                                                       bConfirm );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void checkNotificationIndication( bletestAttSrvB_t xAttribute,
                                  bool enable )
{
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTconfirmCallback_t xConfirmEvent;

    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    /* for Indication or Notification a flag on the first byte is toggled .*/
    if( enable == true )
    {
        TEST_ASSERT_NOT_EQUAL( 0, xWriteEvent.ucValue[ 0 ] );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, xWriteEvent.ucValue[ 0 ] );
    }

    xGattResponse.usHandle = xWriteEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xWriteEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = 0;
    xGattResponse.xAttrValue.xLen = xWriteEvent.xLength;
    xGattResponse.xAttrValue.pucValue = xWriteEvent.ucValue;
    pxGattServerInterface->pxSendResponse( xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventConfimCb, xWriteEvent.usAttrHandle, ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
}

TEST( Full_BLE, BLE_Property_Enable_Indication_Notification )
{
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, true );
}

TEST( Full_BLE, BLE_Property_Disable_Indication_Notification )
{
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, false );
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, false );
}


TEST( Full_BLE, BLE_Property_Indication )
{
    BTStatus_t xStatus;
    BLETESTindicateCallback_t xIndicateEvent;

    memcpy( ucRespBuffer[ bletestATTR_SRVCB_CHAR_F ].ucBuffer, bletestsDEFAULT_CHAR_VALUE, sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1 );
    ucRespBuffer[ bletestATTR_SRVCB_CHAR_F ].xLength = sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1;

    prvSendNotification( bletestATTR_SRVCB_CHAR_F, true );
    xStatus = prvWaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usBLEConnId, xIndicateEvent.usConnId );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xIndicateEvent.xStatus );
}

TEST( Full_BLE, BLE_Property_Notification )
{
    BTStatus_t xStatus;
    BLETESTindicateCallback_t xIndicateEvent;

    memcpy( ucRespBuffer[ bletestATTR_SRVCB_CHAR_E ].ucBuffer, bletestsDEFAULT_CHAR_VALUE, sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1 );
    ucRespBuffer[ bletestATTR_SRVCB_CHAR_E ].xLength = sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1;

    prvSendNotification( bletestATTR_SRVCB_CHAR_E, false );
    /* Wait a possible confirm for 2 max connections interval */
    xStatus = prvWaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usBLEConnId, xIndicateEvent.usConnId );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xIndicateEvent.xStatus );
}

TEST( Full_BLE, BLE_Property_WriteNoResponse )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_D,
                              false,
                              false,
                              0 );
}

void prvReadCheckAndResponse( bletestAttSrvB_t xAttribute )
{
    BLETESTreadAttrCallback_t xReadEvent;
    BTGattResponse_t xGattResponse;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTStatus_t xStatus;

    xStatus = prvWaitEventFromQueue( eBLEHALEventReadAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xReadEvent, sizeof( BLETESTreadAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xReadEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( usBLEConnId, xReadEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, xReadEvent.usOffset );
    TEST_ASSERT_EQUAL( 0, memcmp( &xReadEvent.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    xGattResponse.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = xReadEvent.usOffset;
    xGattResponse.xAttrValue.xLen = ucRespBuffer[ xAttribute ].xLength;
    xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer;
    pxGattServerInterface->pxSendResponse( xReadEvent.usConnId, xReadEvent.ulTransId, eBTStatusSuccess, &xGattResponse );

    xStatus = prvWaitEventFromQueue( eBLEHALEventConfimCb, usHandlesBufferB[ xAttribute ], ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
}

TEST( Full_BLE, BLE_Property_ReadDescriptor )
{
    prvReadCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_A );
}

TEST( Full_BLE, BLE_Property_ReadCharacteristic )
{
    prvReadCheckAndResponse( bletestATTR_SRVCB_CHAR_A );
}

TEST( Full_BLE, BLE_Property_WriteDescriptor )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_A,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Property_WriteCharacteristic )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_A,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Connection_ChangeMTUsize )
{
}

TEST( Full_BLE, BLE_Connection_UpdateConnectionParamReq )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTUpdateConnectionParamReqCallback_t xUpdateConnectionParamReq;

    xStatus = pxBTLeAdapterInterface->pxConnParameterUpdateRequest( &xAddressConnectedDevice,
                                                                    xConnectionParamA.minInterval,
                                                                    xConnectionParamA.maxInterval,
                                                                    xConnectionParamA.latency,
                                                                    xConnectionParamA.timeout );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventConnectionUpdateCb, NO_HANDLE, ( void * ) &xUpdateConnectionParamReq, sizeof( BLETESTUpdateConnectionParamReqCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xUpdateConnectionParamReq.xStatus );

    TEST_ASSERT_EQUAL( 0, memcmp( &xUpdateConnectionParamReq.xBda, &xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( xConnectionParamA.minInterval, xUpdateConnectionParamReq.xConnParam.minInterval );
    TEST_ASSERT_EQUAL( xConnectionParamA.maxInterval, xUpdateConnectionParamReq.xConnParam.maxInterval );
    TEST_ASSERT_EQUAL( xConnectionParamA.latency, xUpdateConnectionParamReq.xConnParam.latency );
    TEST_ASSERT_EQUAL( xConnectionParamA.timeout, xUpdateConnectionParamReq.xConnParam.timeout );
}

TEST( Full_BLE, BLE_Connection_SimpleConnection )
{
    prvWaitConnection( true );
}


void prvSetAdvertisement( BTGattAdvertismentParams_t * pxParams,
                          uint16_t usServiceDataLen,
                          char * pcServiceData,
                          BTUuid_t * pxServiceUuid,
                          size_t xNbServices )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAdvParamCallback_t xAdvParamCallback;

    xStatus = pxBTLeAdapterInterface->pxSetAdvData( ucBLEAdapterIf,
                                                    pxParams,
                                                    0,
                                                    NULL,
                                                    usServiceDataLen,
                                                    pcServiceData,
                                                    pxServiceUuid,
                                                    xNbServices );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventSetAdvCb, NO_HANDLE, ( void * ) &xAdvParamCallback, sizeof( BLETESTAdvParamCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xAdvParamCallback.xStatus );
}

void prvStartStopAdvCheck( bool start )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAdvParamCallback_t xAdvParamCallback;

    xStatus = prvWaitEventFromQueue( eBLEHALEventStartAdvCb, NO_HANDLE, ( void * ) &xAdvParamCallback, sizeof( BLETESTAdvParamCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( start, xAdvParamCallback.bStart );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xAdvParamCallback.xStatus );
}

void prvStartAdvertisement( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = pxBTLeAdapterInterface->pxStartAdv( ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    prvStartStopAdvCheck( true );
}

TEST( Full_BLE, BLE_Advertising_StartAdvertisement )
{
    prvStartAdvertisement();
}

TEST( Full_BLE, BLE_Advertising_SetAvertisementData )
{
    uint16_t usServiceDataLen;
    char * pcServiceData;
    BTUuid_t xServiceUuid =
    {
        .ucType   = eBTuuidType128,
        .uu.uu128 = bletestsFREERTOS_SVC_UUID
    };

    size_t xNbServices;

    usServiceDataLen = 0;
    pcServiceData = NULL;
    xNbServices = 1;

    prvSetAdvertisement( &xAdvertisementConfigA,
                         usServiceDataLen,
                         pcServiceData,
                         &xServiceUuid,
                         xNbServices );


    prvSetAdvertisement( &xAdvertisementConfigB,
                         usServiceDataLen,
                         pcServiceData,
                         NULL,
                         0 );
}

void prvSetGetProperty( BTProperty_t * pxProperty,
                        bool bIsSet )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTSetGetPropertyCallback_t xSetGetPropertyCb;

    if( bIsSet == true )
    {
        xStatus = pxBTInterface->pxSetDeviceProperty( pxProperty );
    }
    else
    {
        xStatus = pxBTInterface->pxGetDeviceProperty( pxProperty->xType );
    }

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPropertyCb, NO_HANDLE, ( void * ) &xSetGetPropertyCb, sizeof( BLETESTSetGetPropertyCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xSetGetPropertyCb.xStatus );
    TEST_ASSERT_EQUAL( 1, xSetGetPropertyCb.ulNumProperties );
    TEST_ASSERT_EQUAL( xSetGetPropertyCb.xProperties.xType, pxProperty->xType );

    if( bIsSet == true )
    {
        TEST_ASSERT_EQUAL( xSetGetPropertyCb.xProperties.xLen, pxProperty->xLen );
        TEST_ASSERT_EQUAL( 0, memcmp( pxProperty->pvVal, xSetGetPropertyCb.xProperties.pvVal, xSetGetPropertyCb.xProperties.xLen ) );
    }
    else
    {
        pxProperty->xLen = xSetGetPropertyCb.xProperties.xLen;
        pxProperty->pvVal = xSetGetPropertyCb.xProperties.pvVal;
    }
}

TEST( Full_BLE, BLE_Advertising_SetProperties )
{
    BTProperty_t pxProperty;
    uint16_t usMTUsize = bletestsMTU_SIZE1;

    pxProperty.xType = eBTpropertyBdname;
    pxProperty.xLen = strlen( bletestsDEVICE_NAME );
    pxProperty.pvVal = ( void * ) bletestsDEVICE_NAME;

    /* Set the name */
    prvSetGetProperty( &pxProperty, true );

    /* Get the name to check it is set */
/*@TODO prvSetGetProperty(&pxProperty, false); */

    pxProperty.xType = eBTpropertyLocalMTUSize;
    pxProperty.xLen = sizeof( usMTUsize );
    pxProperty.pvVal = &usMTUsize;

    /* Set the MTU size */
    prvSetGetProperty( &pxProperty, true );

    /* Get the MTU size to check it is set */
    /*@TODOprvSetGetProperty(&pxProperty, false); */
}

void prvStartService( BTService_t * xRefSrvc )
{
    BLETESTServiceCallback_t xStartServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = pxGattServerInterface->pxStartService( ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ], BTTransportLe );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceStartedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStartServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStartServiceCb.xStatus );
}

TEST( Full_BLE, BLE_CreateAttTable_IncludedService )
{
    BLETESTAttrCallback_t xBLETESTInclServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = pxGattServerInterface->pxAddIncludedService( ucBLEServerIf,
                                                           xSrvcB.pusHandlesBuffer[ 0 ],
                                                           xSrvcA.pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventIncludedServiceAdded, NO_HANDLE, ( void * ) &xBLETESTInclServiceCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTInclServiceCb.xStatus );
    TEST_ASSERT_EQUAL( xSrvcB.pusHandlesBuffer[ 0 ], xBLETESTInclServiceCb.usSrvHandle );
    xSrvcB.pusHandlesBuffer[ bletestATTR_INCLUDED_SERVICE ] = xBLETESTInclServiceCb.usAttrHandle;
}

void prvCreateCharacteristicDescriptor( BTService_t * xSrvc,
                                        int xAttribute )
{
    BLETESTAttrCallback_t xBLETESTCharDescrCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = pxGattServerInterface->pxAddDescriptor( ucBLEServerIf,
                                                      xSrvc->pusHandlesBuffer[ 0 ],
                                                      &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid,
                                                      xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xPermissions );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventCharDescrAddedCb, NO_HANDLE, ( void * ) &xBLETESTCharDescrCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTCharDescrCb.xStatus );

    if( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid.ucType == eBTuuidType16 )
    {
        TEST_ASSERT_EQUAL( eBTuuidType16, xBLETESTCharDescrCb.xUUID.ucType );
        TEST_ASSERT_EQUAL( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid.uu.uu16, xBLETESTCharDescrCb.xUUID.uu.uu16 );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, memcmp( &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid, &xBLETESTCharDescrCb.xUUID, sizeof( BTUuid_t ) ) );
    }

    TEST_ASSERT_EQUAL( xSrvc->pusHandlesBuffer[ 0 ], xBLETESTCharDescrCb.usSrvHandle );
    xSrvc->pusHandlesBuffer[ xAttribute ] = xBLETESTCharDescrCb.usAttrHandle;
}

void prvCreateCharacteristic( BTService_t * xSrvc,
                              int xAttribute )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAttrCallback_t xBLETESTCharCb;

    xStatus = pxGattServerInterface->pxAddCharacteristic( ucBLEServerIf,
                                                          xSrvc->pusHandlesBuffer[ 0 ],
                                                          &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid,
                                                          xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xProperties,
                                                          xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xPermissions );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventCharAddedCb, NO_HANDLE, ( void * ) &xBLETESTCharCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTCharCb.xStatus );

    if( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid.ucType == eBTuuidType16 )
    {
        TEST_ASSERT_EQUAL( eBTuuidType16, xBLETESTCharCb.xUUID.ucType );
        TEST_ASSERT_EQUAL( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid.uu.uu16, xBLETESTCharCb.xUUID.uu.uu16 );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, memcmp( &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid, &xBLETESTCharCb.xUUID, sizeof( BTUuid_t ) ) );
    }

    TEST_ASSERT_EQUAL( xSrvc->pusHandlesBuffer[ 0 ], xBLETESTCharCb.usSrvHandle );
    xSrvc->pusHandlesBuffer[ xAttribute ] = xBLETESTCharCb.usAttrHandle;
}

static size_t prvComputeNumberOfHandles( BTService_t * pxService )
{
    size_t xIndex;
    size_t nbHandles = 0;

    for( xIndex = 0; xIndex < pxService->xNumberOfAttributes; xIndex++ )
    {
        /* Increment by 2 to account for characteristic declaration */
        if( pxService->pxBLEAttributes[ xIndex ].xAttributeType == eBTDbCharacteristic )
        {
            nbHandles += 2;
        }
        else
        {
            nbHandles++;
        }
    }

    return nbHandles;
}

void prvCreateService( BTService_t * xRefSrvc )
{
    BLETESTServiceCallback_t xCreateServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;
    BTGattSrvcId_t xSrvcId;

    uint16_t usNumHandles = prvComputeNumberOfHandles( xRefSrvc );

    xSrvcId.xId.ucInstId = xRefSrvc->ucInstId;
    xSrvcId.xId.xUuid = xRefSrvc->pxBLEAttributes[ 0 ].xServiceUUID;
    xSrvcId.xServiceType = xRefSrvc->xType;

    xStatus = pxGattServerInterface->pxAddService( ucBLEServerIf, &xSrvcId, usNumHandles );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceAddedCb, NO_HANDLE, ( void * ) &xCreateServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xCreateServiceCb.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xCreateServiceCb.xSrvcId.xId.xUuid, &xRefSrvc->pxBLEAttributes[ 0 ].xServiceUUID, sizeof( BTUuid_t ) ) );
    TEST_ASSERT_EQUAL( xRefSrvc->ucInstId, xCreateServiceCb.xSrvcId.xId.ucInstId );
    TEST_ASSERT_EQUAL( xRefSrvc->xType, xCreateServiceCb.xSrvcId.xServiceType );

    xRefSrvc->pusHandlesBuffer[ 0 ] = xCreateServiceCb.usAttrHandle;
}

TEST( Full_BLE, BLE_CreateAttTable_CreateServices )
{
    BTStatus_t xStatus;

    /* Try to create using blob service API first.
     * If blob is not supported then try legacy APIs. */
    xStatus = pxGattServerInterface->pxAddServiceBlob( ucBLEServerIf, &xSrvcA );

    if( xStatus != eBTStatusUnsupported )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        xStatus = pxGattServerInterface->pxAddServiceBlob( ucBLEServerIf, &xSrvcB );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }
    else
    {
        /* Create service A */
        prvCreateService( &xSrvcA );
        prvCreateCharacteristic( &xSrvcA, bletestATTR_SRVCA_CHAR_A );

        /* Create service B */
        prvCreateService( &xSrvcB );
        prvCreateCharacteristic( &xSrvcB, bletestATTR_SRVCB_CHAR_A );
        prvCreateCharacteristic( &xSrvcB, bletestATTR_SRVCB_CHAR_B );
        prvCreateCharacteristic( &xSrvcB, bletestATTR_SRVCB_CHAR_C );
        prvCreateCharacteristic( &xSrvcB, bletestATTR_SRVCB_CHAR_D );
        prvCreateCharacteristic( &xSrvcB, bletestATTR_SRVCB_CHAR_E );
        prvCreateCharacteristicDescriptor( &xSrvcB, bletestATTR_SRVCB_CCCD_E );
        prvCreateCharacteristic( &xSrvcB, bletestATTR_SRVCB_CHAR_F );
        prvCreateCharacteristicDescriptor( &xSrvcB, bletestATTR_SRVCB_CCCD_F );
        prvCreateCharacteristicDescriptor( &xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_A );
        prvCreateCharacteristicDescriptor( &xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_B );
        prvCreateCharacteristicDescriptor( &xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_C );
        prvCreateCharacteristicDescriptor( &xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_D );

        /* Start service A */
        prvStartService( &xSrvcA );
        /* Start service B */
        prvStartService( &xSrvcB );
    }
}

TEST( Full_BLE, BLE_Initialize_BLE_GATT )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    pxGattServerInterface = ( BTGattServerInterface_t * ) pxBTLeAdapterInterface->ppvGetGattServerInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, pxGattServerInterface );

    pxGattServerInterface->pxGattServerInit( &xBTGattServerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = pxGattServerInterface->pxRegisterServer( &xServerUUID );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventRegisterUnregisterGattServerCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );
}

TEST( Full_BLE, BLE_Initialize_common_GAP )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    /* Get BT interface */
    pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, pxBTInterface );

    /* Initialize callbacks */
    xStatus = pxBTInterface->pxBtManagerInit( &xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    /* Enable RADIO and wait for callback. */
    xStatus = pxBTInterface->pxEnable( 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );
}

TEST( Full_BLE, BLE_Initialize_BLE_GAP )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    pxBTLeAdapterInterface = ( BTBleAdapter_t * ) pxBTInterface->pxGetLeAdapter();
    TEST_ASSERT_NOT_EQUAL( NULL, pxBTLeAdapterInterface );

    xStatus = pxBTLeAdapterInterface->pxBleAdapterInit( &xBTBleAdapterCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = pxBTLeAdapterInterface->pxRegisterBleApp( &xAppUUID );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventRegisterBleAdapterCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );
}

void prvStopAndDeleteService( BTService_t * xRefSrvc )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTServiceCallback_t xStopDeleteServiceCb;


    xStatus = pxGattServerInterface->pxStopService( ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceStoppedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStopDeleteServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStopDeleteServiceCb.xStatus );

    xStatus = pxGattServerInterface->pxDeleteService( ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceDeletedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStopDeleteServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStopDeleteServiceCb.xStatus );
}

TEST( Full_BLE, BLE_DeInitialize )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    prvStopAndDeleteService( &xSrvcA );
    prvStopAndDeleteService( &xSrvcB );

    xStatus = pxGattServerInterface->pxUnregisterServer( ucBLEServerIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventRegisterUnregisterGattServerCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );


    xStatus = pxBTLeAdapterInterface->pxUnregisterBleApp( ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = pxBTInterface->pxDisable( 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOff, xInitDeinitCb.xBLEState );


    xStatus = pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void prvRegisterBleAdapterCb( BTStatus_t xStatus,
                              uint8_t ucAdapter_if,
                              BTUuid_t * pxAppUuid )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xStatus = xStatus;
    ucBLEAdapterIf = ucAdapter_if;

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUuid == NULL )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUuid->ucType != xAppUUID.ucType )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( memcmp( pxAppUuid->uu.uu128, xAppUUID.uu.uu128, bt128BIT_UUID_LEN ) )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventRegisterBleAdapterCb;
    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvDeviceStateChangedCb( BTState_t xState )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xBLEState = xState;
    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventEnableDisableCb;
    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvBTRegisterServerCb( BTStatus_t xStatus,
                            uint8_t ucServerIf,
                            BTUuid_t * pxAppUuid )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xStatus = xStatus;
    ucBLEServerIf = ucServerIf;

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUuid == NULL )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUuid->ucType != xServerUUID.ucType )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( 0 != memcmp( pxAppUuid->uu.uu128, xServerUUID.uu.uu128, bt128BIT_UUID_LEN ) )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventRegisterUnregisterGattServerCb;

    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvBTUnregisterServerCb( BTStatus_t xStatus,
                              uint8_t ucServerIf )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xStatus = xStatus;

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventRegisterUnregisterGattServerCb;

    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvServiceAddedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        BTGattSrvcId_t * pxSrvcId,
                        uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxAddedServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxAddedServiceCb->xStatus = xStatus;

    if( pxSrvcId != NULL )
    {
        memcpy( &pxAddedServiceCb->xSrvcId, pxSrvcId, sizeof( BTGattSrvcId_t ) );
    }
    else
    {
        memset( &pxAddedServiceCb->xSrvcId, 0, sizeof( BTGattSrvcId_t ) );
        pxAddedServiceCb->xStatus = eBTStatusFail;
    }

    if( pxAddedServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxAddedServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxAddedServiceCb->usAttrHandle = usServiceHandle;
    pxAddedServiceCb->xEvent.xEventTypes = eBLEHALEventServiceAddedCb;
    pxAddedServiceCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAddedServiceCb->xEvent.eventList );
}

void prvServiceStartedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxStartServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxStartServiceCb->xStatus = xStatus;

    if( pxStartServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxStartServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxStartServiceCb->xEvent.xEventTypes = eBLEHALEventServiceStartedCb;
    pxStartServiceCb->xEvent.lHandle = usServiceHandle;

    pushToQueue( &pxStartServiceCb->xEvent.eventList );
}

void prvServiceStoppedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxStopServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxStopServiceCb->xStatus = xStatus;

    if( pxStopServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxStopServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxStopServiceCb->xEvent.xEventTypes = eBLEHALEventServiceStoppedCb;
    pxStopServiceCb->xEvent.lHandle = usServiceHandle;

    pushToQueue( &pxStopServiceCb->xEvent.eventList );
}

void prvServiceDeletedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxDeleteServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxDeleteServiceCb->xStatus = xStatus;

    if( pxDeleteServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxDeleteServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxDeleteServiceCb->xEvent.xEventTypes = eBLEHALEventServiceDeletedCb;
    pxDeleteServiceCb->xEvent.lHandle = usServiceHandle;

    pushToQueue( &pxDeleteServiceCb->xEvent.eventList );
}

void prvCharacteristicAddedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               BTUuid_t * pxUuid,
                               uint16_t usServiceHandle,
                               uint16_t usCharHandle )
{
    BLETESTAttrCallback_t * pxAttrCb = IotTest_Malloc( sizeof( BLETESTAttrCallback_t ) );

    pxAttrCb->xStatus = xStatus;
    pxAttrCb->usSrvHandle = usServiceHandle;
    pxAttrCb->usAttrHandle = usCharHandle;

    if( pxUuid != NULL )
    {
        memcpy( &pxAttrCb->xUUID, pxUuid, sizeof( BTUuid_t ) );
    }
    else
    {
        memset( &pxAttrCb->xUUID, 0, sizeof( BTUuid_t ) );
        pxAttrCb->xStatus = eBTStatusFail;
    }

    if( pxAttrCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxAttrCb->xStatus = eBTStatusFail;
        }
    }

    pxAttrCb->xEvent.xEventTypes = eBLEHALEventCharAddedCb;
    pxAttrCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAttrCb->xEvent.eventList );
}

void prvCharacteristicDescrAddedCb( BTStatus_t xStatus,
                                    uint8_t ucServerIf,
                                    BTUuid_t * pxUuid,
                                    uint16_t usServiceHandle,
                                    uint16_t usCharHandle )
{
    BLETESTAttrCallback_t * pxAttrCb = IotTest_Malloc( sizeof( BLETESTAttrCallback_t ) );

    pxAttrCb->xStatus = xStatus;
    pxAttrCb->usSrvHandle = usServiceHandle;
    pxAttrCb->usAttrHandle = usCharHandle;

    if( pxUuid != NULL )
    {
        memcpy( &pxAttrCb->xUUID, pxUuid, sizeof( BTUuid_t ) );
    }
    else
    {
        memset( &pxAttrCb->xUUID, 0, sizeof( BTUuid_t ) );
        pxAttrCb->xStatus = eBTStatusFail;
    }

    if( pxAttrCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxAttrCb->xStatus = eBTStatusFail;
        }
    }

    pxAttrCb->xEvent.xEventTypes = eBLEHALEventCharDescrAddedCb;
    pxAttrCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAttrCb->xEvent.eventList );
}


void prvIncludedServiceAddedCb( BTStatus_t xStatus,
                                uint8_t ucServerIf,
                                uint16_t usServiceHandle,
                                uint16_t usInclSrvcHandle )
{
    BLETESTAttrCallback_t * pxAttrCb = IotTest_Malloc( sizeof( BLETESTAttrCallback_t ) );

    pxAttrCb->xStatus = xStatus;
    pxAttrCb->usSrvHandle = usServiceHandle;
    pxAttrCb->usAttrHandle = usInclSrvcHandle;

    if( pxAttrCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != ucBLEServerIf )
        {
            pxAttrCb->xStatus = eBTStatusFail;
        }
    }

    pxAttrCb->xEvent.xEventTypes = eBLEHALEventIncludedServiceAdded;
    pxAttrCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAttrCb->xEvent.eventList );
}

void prvAdapterPropertiesCb( BTStatus_t xStatus,
                             uint32_t ulNumProperties,
                             BTProperty_t * pxProperties )
{
    BLETESTSetGetPropertyCallback_t * pxSetGetPropertyCb = IotTest_Malloc( sizeof( BLETESTSetGetPropertyCallback_t ) );

    pxSetGetPropertyCb->xStatus = xStatus;
    pxSetGetPropertyCb->ulNumProperties = ulNumProperties;
    pxSetGetPropertyCb->xProperties.xLen = pxProperties->xLen;
    pxSetGetPropertyCb->xProperties.xType = pxProperties->xType;
    pxSetGetPropertyCb->xProperties.pvVal = ucCbPropertyBuffer;

    if( pxProperties->xLen < bletestsMAX_PROPERTY_SIZE )
    {
        switch( pxProperties->xType )
        {
            case eBTpropertyAdapterBondedDevices:

                if( pxProperties->pvVal != NULL )
                {
                    memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, sizeof( BTBdaddr_t ) );
                }
                else
                {
                    memset( pxSetGetPropertyCb->xProperties.pvVal, 0, sizeof( BTBdaddr_t ) );
                }

                break;

            case eBTpropertyBdname:

                if( pxProperties->pvVal != NULL )
                {
                    memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, strlen( bletestsDEVICE_NAME ) );
                }
                else
                {
                    memset( pxSetGetPropertyCb->xProperties.pvVal, 0, strlen( bletestsDEVICE_NAME ) );
                }

                break;

            case eBTpropertyBdaddr:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertyTypeOfDevice:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertyLocalMTUSize:
                memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, sizeof( uint16_t ) );
                break;

            case eBTpropertyBondable:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertyIO:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertySecureConnectionOnly:
            default:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
        }
    }
    else
    {
        pxSetGetPropertyCb->xStatus = eBTStatusFail;
    }

    pxSetGetPropertyCb->xEvent.xEventTypes = eBLEHALEventPropertyCb;
    pxSetGetPropertyCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxSetGetPropertyCb->xEvent.eventList );
}

void prvSetAdvDataCb( BTStatus_t xStatus )
{
    BLETESTAdvParamCallback_t * xdvParamCallback = IotTest_Malloc( sizeof( BLETESTAdvParamCallback_t ) );

    xdvParamCallback->xStatus = xStatus;
    xdvParamCallback->xEvent.xEventTypes = eBLEHALEventSetAdvCb;
    xdvParamCallback->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &xdvParamCallback->xEvent.eventList );
}

void prvAdvStatusCb( BTStatus_t xStatus,
                     uint32_t ulServerIf,
                     bool bStart )
{
    BLETESTAdvParamCallback_t * xdvParamCallback = IotTest_Malloc( sizeof( BLETESTAdvParamCallback_t ) );

    xdvParamCallback->xStatus = xStatus;
    xdvParamCallback->bStart = bStart;
    xdvParamCallback->xEvent.xEventTypes = eBLEHALEventStartAdvCb;
    xdvParamCallback->xEvent.lHandle = NO_HANDLE;

    if( xdvParamCallback->xStatus == eBTStatusSuccess )
    {
        if( ulServerIf != ucBLEServerIf )
        {
            xdvParamCallback->xStatus = eBTStatusFail;
        }
    }

    pushToQueue( &xdvParamCallback->xEvent.eventList );
}

void prvConnectionCb( uint16_t usConnId,
                      uint8_t ucServerIf,
                      bool bConnected,
                      BTBdaddr_t * pxBda )
{
    BLETESTConnectionCallback_t * pxConnectionCallback = IotTest_Malloc( sizeof( BLETESTConnectionCallback_t ) );

    pxConnectionCallback->xStatus = eBTStatusSuccess;

    if( pxConnectionCallback != NULL )
    {
        pxConnectionCallback->bConnected = bConnected;

        if( pxBda != NULL )
        {
            memcpy( &pxConnectionCallback->pxBda, pxBda, sizeof( BTBdaddr_t ) );
            memcpy( &xAddressConnectedDevice, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            pxConnectionCallback->xStatus = eBTStatusFail;
            memset( &pxConnectionCallback->pxBda, 0, sizeof( BTBdaddr_t ) );
            memset( &xAddressConnectedDevice, 0, sizeof( BTBdaddr_t ) );
        }

        pxConnectionCallback->ucServerIf = ucServerIf;
        pxConnectionCallback->usConnId = usConnId;
        pxConnectionCallback->xEvent.xEventTypes = eBLEHALEventConnectionCb;
        pxConnectionCallback->xEvent.lHandle = NO_HANDLE;
        usBLEConnId = usConnId;

        pushToQueue( &pxConnectionCallback->xEvent.eventList );
    }
}

void prvConnParameterUpdateCb( BTStatus_t xStatus,
                               const BTBdaddr_t * pxBdAddr,
                               uint32_t minInterval,
                               uint32_t maxInterval,
                               uint32_t latency,
                               uint32_t usConnInterval,
                               uint32_t timeout )
{
    BLETESTUpdateConnectionParamReqCallback_t * pxUpdateConnectionParamReq = IotTest_Malloc( sizeof( BLETESTUpdateConnectionParamReqCallback_t ) );

    pxUpdateConnectionParamReq->xConnParam.minInterval = minInterval;
    pxUpdateConnectionParamReq->xConnParam.maxInterval = maxInterval;
    pxUpdateConnectionParamReq->xConnParam.latency = latency;
    pxUpdateConnectionParamReq->xConnParam.timeout = timeout;
    pxUpdateConnectionParamReq->xEvent.xEventTypes = eBLEHALEventConnectionUpdateCb;
    pxUpdateConnectionParamReq->xEvent.lHandle = NO_HANDLE;

    if( pxBdAddr != NULL )
    {
        memcpy( &pxUpdateConnectionParamReq->xBda, pxBdAddr, sizeof( BTBdaddr_t ) );
    }
    else
    {
        memset( &pxUpdateConnectionParamReq->xBda, 0, sizeof( BTBdaddr_t ) );
    }

    usCbConnInterval = usConnInterval;

    pushToQueue( &pxUpdateConnectionParamReq->xEvent.eventList );
}
void prvRequestReadCb( uint16_t usConnId,
                       uint32_t ulTransId,
                       BTBdaddr_t * pxBda,
                       uint16_t usAttrHandle,
                       uint16_t usOffset )
{
    BLETESTreadAttrCallback_t * pxreadAttrCallback = IotTest_Malloc( sizeof( BLETESTreadAttrCallback_t ) );

    if( pxreadAttrCallback != NULL )
    {
        pxreadAttrCallback->usConnId = usConnId;
        pxreadAttrCallback->ulTransId = ulTransId;

        if( pxBda != NULL )
        {
            memcpy( &pxreadAttrCallback->xBda, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxreadAttrCallback->xBda, 0, sizeof( BTBdaddr_t ) );
        }

        pxreadAttrCallback->usAttrHandle = usAttrHandle;
        pxreadAttrCallback->usOffset = usOffset;
        pxreadAttrCallback->xEvent.xEventTypes = eBLEHALEventReadAttrCb;
        pxreadAttrCallback->xEvent.lHandle = usAttrHandle;

        pushToQueue( &pxreadAttrCallback->xEvent.eventList );
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
    BLETESTwriteAttrCallback_t * pxWriteAttrCallback = IotTest_Malloc( sizeof( BLETESTwriteAttrCallback_t ) );

    if( pxWriteAttrCallback != NULL )
    {
        pxWriteAttrCallback->usConnId = usConnId;
        pxWriteAttrCallback->ulTransId = ulTransId;

        if( pxBda != NULL )
        {
            memcpy( &pxWriteAttrCallback->xBda, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxWriteAttrCallback->xBda, 0, sizeof( BTBdaddr_t ) );
        }

        pxWriteAttrCallback->usOffset = usOffset;
        pxWriteAttrCallback->xLength = xLength;
        pxWriteAttrCallback->bNeedRsp = bNeedRsp;
        pxWriteAttrCallback->bIsPrep = bIsPrep;
        pxWriteAttrCallback->usAttrHandle = usAttrHandle;

        if( ( xLength <= bletestsSTRINGYFIED_UUID_SIZE ) && ( pucValue != NULL ) )
        {
            memcpy( pxWriteAttrCallback->ucValue, pucValue, xLength );
        }
        else
        {
            memset( pxWriteAttrCallback->ucValue, 0, bletestsSTRINGYFIED_UUID_SIZE );
        }

        pxWriteAttrCallback->xEvent.xEventTypes = eBLEHALEventWriteAttrCb;
        pxWriteAttrCallback->xEvent.lHandle = usAttrHandle;

        pushToQueue( &pxWriteAttrCallback->xEvent.eventList );
    }

/*
 *  BTGattResponse_t xGattResponse;
 *
 *  if ( xLength <= bletestsSTRINGYFIED_UUID_SIZE)
 *  {
 *      memcpy(ucResponseBuffer, pucValue, bletestsSTRINGYFIED_UUID_SIZE);
 *  }else
 *  {
 *      memset(ucResponseBuffer, 0, bletestsSTRINGYFIED_UUID_SIZE);
 *  }
 *  xGattResponse.usHandle = usAttrHandle;
 *  xGattResponse.xAttrValue.pucValue = ucResponseBuffer;
 *  xGattResponse.xAttrValue.xLen = bletestsSTRINGYFIED_UUID_SIZE;
 *  xGattResponse.xAttrValue.usOffset = 0;
 *  xGattResponse.xAttrValue.xRspErrorStatus = eBTRspErrorNone;
 */
}

void * checkQueueForEvent( BLEHALEventsTypes_t xEventName,
                           int32_t lhandle )
{
    BLEHALEventsInternals_t * pEventIndex;
    IotLink_t * pEventListIndex;
    void * pvPtr = NULL;

    IotMutex_Lock( &threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &eventQueueHead, pEventListIndex )
    {
        pEventIndex = IotLink_Container( BLEHALEventsInternals_t, pEventListIndex, eventList );

        if( ( pEventIndex->xEventTypes == xEventName ) &&
            ( pEventIndex->lHandle == lhandle ) )
        {
            pvPtr = pEventIndex;
            IotListDouble_Remove( &pEventIndex->eventList );
            break; /* If the right event is received, exit. */
        }
    }

    IotMutex_Unlock( &threadSafetyMutex );

    return pvPtr;
}

/* This function first check if an event is waiting in the list. If not, it will go and wait on the queue.
 * When an event is received on the queue, if it is not the expected event, it goes on the waiting list.
 */
BTStatus_t prvWaitEventFromQueue( BLEHALEventsTypes_t xEventName,
                                  int32_t lhandle,
                                  void * pxMessage,
                                  size_t xMessageLength,
                                  uint32_t timeoutMs )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    void * pvPtr = NULL;

    pvPtr = checkQueueForEvent( xEventName, lhandle );

    /* If event is not waiting then wait for it. */
    if( pvPtr == NULL )
    {
        do
        {
            /* TODO check event list here */
            if( IotSemaphore_TimedWait( &eventSemaphore, timeoutMs ) == true )
            {
                pvPtr = checkQueueForEvent( xEventName, lhandle );

                if( pvPtr != NULL )
                {
                    break; /* If the right event is received, exit. */
                }
            }
            else
            {
                xStatus = eBTStatusFail;
            }
        } while( xStatus == eBTStatusSuccess ); /* If there is an error exit */
    }

    if( xStatus == eBTStatusSuccess )
    {
        memcpy( pxMessage, pvPtr, xMessageLength );
        IotTest_Free( pvPtr );
    }

    return xStatus;
}

void prvIndicationSentCb( uint16_t usConnId,
                          BTStatus_t xStatus )
{
    BLETESTindicateCallback_t * pxIndicateCallback = IotTest_Malloc( sizeof( BLETESTindicateCallback_t ) );

    if( pxIndicateCallback != NULL )
    {
        pxIndicateCallback->xEvent.xEventTypes = eBLEHALEventIndicateCb;
        pxIndicateCallback->xEvent.lHandle = NO_HANDLE;
        pxIndicateCallback->usConnId = usConnId;
        pxIndicateCallback->xStatus = xStatus;

        pushToQueue( &pxIndicateCallback->xEvent.eventList );
    }
}

void prvResponseConfirmationCb( BTStatus_t xStatus,
                                uint16_t usHandle )
{
    BLETESTconfirmCallback_t * pxConfirmCallback = IotTest_Malloc( sizeof( BLETESTconfirmCallback_t ) );

    if( pxConfirmCallback != NULL )
    {
        pxConfirmCallback->xEvent.xEventTypes = eBLEHALEventConfimCb;
        pxConfirmCallback->xEvent.lHandle = usHandle;
        pxConfirmCallback->usAttrHandle = usHandle;
        pxConfirmCallback->xStatus = xStatus;

        pushToQueue( &pxConfirmCallback->xEvent.eventList );
    }
}

void prvSspRequestCb( BTBdaddr_t * pxRemoteBdAddr,
                      BTBdname_t * pxRemoteBdName,
                      uint32_t ulCod,
                      BTSspVariant_t xPairingVariant,
                      uint32_t ulPassKey )
{
    BLETESTsspRequestCallback_t * pxSSPrequestCallback = IotTest_Malloc( sizeof( BLETESTsspRequestCallback_t ) );

    if( pxSSPrequestCallback != NULL )
    {
        if( pxRemoteBdAddr != NULL )
        {
            memcpy( &pxSSPrequestCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxSSPrequestCallback->xRemoteBdAddr, 0, sizeof( BTBdaddr_t ) );
        }

        pxSSPrequestCallback->xEvent.lHandle = NO_HANDLE;
        pxSSPrequestCallback->ulCod = ulCod;
        pxSSPrequestCallback->xPairingVariant = xPairingVariant;
        pxSSPrequestCallback->ulPassKey = ulPassKey;

        switch( pxSSPrequestCallback->xPairingVariant )
        {
            case eBTsspVariantPasskeyConfirmation:
                pxSSPrequestCallback->xEvent.xEventTypes = eBLEHALEventSSPrequestConfirmationCb;
                break;

            case eBTsspVariantConsent:
                pxSSPrequestCallback->xEvent.xEventTypes = eBLEHALEventSSPrequestCb;
                break;

            case eBTsspVariantPasskeyNotification:
            case eBTsspVariantPasskeyEntry:
            default:
                pxSSPrequestCallback->xEvent.xEventTypes = eBLEHALEventSSPrequestCb;
        }

        pushToQueue( &pxSSPrequestCallback->xEvent.eventList );
    }
}

void prvPairingStateChangedCb( BTStatus_t xStatus,
                               BTBdaddr_t * pxRemoteBdAddr,
                               BTBondState_t xState,
                               BTSecurityLevel_t xSecurityLevel,
                               BTAuthFailureReason_t xReason )
{
    BLETESTPairingStateChangedCallback_t * pxPairingStateChangedCallback = IotTest_Malloc( sizeof( BLETESTPairingStateChangedCallback_t ) );

    if( pxPairingStateChangedCallback != NULL )
    {
        pxPairingStateChangedCallback->xEvent.xEventTypes = eBLEHALEventPairingStateChangedCb;
        pxPairingStateChangedCallback->xEvent.lHandle = NO_HANDLE;
        pxPairingStateChangedCallback->xStatus = xStatus;

        if( pxRemoteBdAddr != NULL )
        {
            memcpy( &pxPairingStateChangedCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxPairingStateChangedCallback->xRemoteBdAddr, 0, sizeof( BTBdaddr_t ) );
        }

        pxPairingStateChangedCallback->xSecurityLevel = xSecurityLevel;
        pxPairingStateChangedCallback->xReason = xReason;

        pushToQueue( &pxPairingStateChangedCallback->xEvent.eventList );
    }
}

void prvRequestExecWriteCb( uint16_t usConnId,
                            uint32_t ulTransId,
                            BTBdaddr_t * pxBda,
                            bool bExecWrite )
{
    BLETESTRequestExecWriteCallback_t * pxRequestExecWriteCallback = IotTest_Malloc( sizeof( BLETESTRequestExecWriteCallback_t ) );

    if( pxRequestExecWriteCallback != NULL )
    {
        pxRequestExecWriteCallback->xEvent.xEventTypes = eBLEHALEventRequestExecWriteCb;
        pxRequestExecWriteCallback->xEvent.lHandle = NO_HANDLE;
        pxRequestExecWriteCallback->ulTransId = ulTransId;
        pxRequestExecWriteCallback->usConnId = usConnId;

        if( pxBda != NULL )
        {
            memcpy( &pxRequestExecWriteCallback->xBda, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxRequestExecWriteCallback->xBda, 0, sizeof( BTBdaddr_t ) );
        }

        pxRequestExecWriteCallback->bExecWrite = bExecWrite;

        pushToQueue( &pxRequestExecWriteCallback->xEvent.eventList );
    }
}

void prvBondedCb( BTStatus_t xStatus,
                  BTBdaddr_t * pxRemoteBdAddr,
                  bool bIsBonded )
{
    BLETESTBondedCallback_t * pxBondedCallback = IotTest_Malloc( sizeof( BLETESTBondedCallback_t ) );

    if( pxBondedCallback != NULL )
    {
        pxBondedCallback->bIsBonded = bIsBonded;

        if( pxRemoteBdAddr != NULL )
        {
            memcpy( &pxBondedCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxBondedCallback->xRemoteBdAddr, 0, sizeof( BTBdaddr_t ) );
        }

        pxBondedCallback->xStatus = xStatus;
        pxBondedCallback->xEvent.xEventTypes = eBLEHALEventBondedCb;
        pxBondedCallback->xEvent.lHandle = NO_HANDLE;

        pushToQueue( &pxBondedCallback->xEvent.eventList );
    }
}

void pushToQueue( IotLink_t * pEventList )
{
    IotMutex_Lock( &threadSafetyMutex );

    IotListDouble_InsertHead( &eventQueueHead,
                              pEventList );
    IotSemaphore_Post( &eventSemaphore );

    IotMutex_Unlock( &threadSafetyMutex );
}
