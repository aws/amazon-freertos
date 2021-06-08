/*
 * FreeRTOS BLE HAL V5.1.0
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
 * @file iot_test_ble_hal_common.h
 * @brief
 */

#ifndef _IOT_TEST_BLE_HAL_COMMON_H
#define _IOT_TEST_BLE_HAL_COMMON_H

#include "iot_config.h"
/* C standard library includes. */
#include <stddef.h>
#include <string.h>
#include IOT_LINEAR_CONTAINERS
#include IOT_THREADS
#include IOT_CLOCK
#include IOT_PLATFORM_TYPES
#include IOT_BT_HAL_MANAGER_ADAPTER_BLE
#include IOT_BT_HAL_MANAGER_ADAPTER
#include IOT_BT_HAL_GATT_SERVER
#include IOT_BT_HAL_GATT_TYPES
/* Test framework includes. */
#include IOT_UNITY_FIXTURE
#include IOT_UNITY

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_HAL_API_TEST" )
#include IOT_LOG

/* Configs for enable/diable test cases */
#include "iot_test_ble_hal_config_defaults.h"

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

#define bletestsAPP_UUID                  { 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsSERVER_UUID               { 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsFREERTOS_SVC_UUID_128     { 0x5A, 0xDB, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_SVC_UUID_32      0x8A7F1168
#define bletestsFREERTOS_SVC_UUID_16      0xabcd

#define NO_HANDLE                         -1

#define MAX_EVENT                         20

#define bletestsDEFAULT_CHAR_VALUE        "hello"
#define bletestsSTRINGYFIED_UUID_SIZE     36 /* like "8a7f1168-48af-4efb-83b5-e679f9320002" */
#define bletests_LONG_WRITE_LEN           512
#define bletests_LONG_READ_LEN            512
#define bletests_LONG_WRITE_HEADER_LEN    5
#define bletests_LONGEST_ATTR_LEN         512
#define bletestsFULL_PERMISSIONS          ( eBTPermRead | eBTPermWrite )
#define bletestsNB_INCLUDEDSERVICES       1

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

#define bletestsFREERTOS_SVC_C_UUID                                                                                     \
    {                                                                                                                   \
        .uu.uu128 = { 0x02, 0x00, 0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31 }, \
        .ucType = eBTuuidType128                                                                                        \
    }

#define bletestsFREERTOS_CHAR_G_UUID                                                                                    \
    {                                                                                                                   \
        .uu.uu128 = { 0x03, 0x00, 0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31 }, \
        .ucType = eBTuuidType128                                                                                        \
    }


#define bletestsDEVICE_NAME                   "TEST"
#define bletestsMAX_PROPERTY_SIZE             30

#define bletestsMIN_ADVERTISEMENT_INTERVAL    0x12C
#define bletestsMAX_ADVERTISEMENT_INTERVAL    0x258

#define bletestsMIN_CONNECTION_INTERVAL       32
#define bletestsMAX_CONNECTION_INTERVAL       64

#define QUEUE_LENGTH                          20
#define ITEM_SIZE                             sizeof( void * )

#define BLE_TESTS_WAIT                        60000                 /* Wait 60s max */
#define BLE_TESTS_SHORT_WAIT                  4000                  /* Wait 4s max */

#define BLE_TIME_LIMIT                        2500                  /* Set time limit to 2.5s */

enum
{
    eBLEHALEventNone = 0x00U,
    eBLEHALEventServerRegisteredCb = 0x01U,
    eBLEHALEventEnableDisableCb = 0x02U,
    eBLEHALEventCharAddedCb = 0x04U,
    eBLEHALEventServiceAddedCb = 0x08U,
    eBLEHALEventServiceStartedCb = 0x10U,
    eBLEHALEventServiceStoppedCb = 0x20U,
    eBLEHALEventServiceDeletedCb = 0x40U,
    eBLEHALEventCharDescrAddedCb = 0x80U,
    eBLEHALEventIncludedServiceAdded = 0x100U,
    eBLEHALEventRegisterBleAdapterCb = 0x200U,
    eBLEHALEventAdapterPropertiesCb = 0x400U,
    eBLEHALEventRegisterUnregisterGattServerCb = 0x800U,
    eBLEHALEventPropertyCb = 0x1000U,
    eBLEHALEventSetAdvCb = 0x2000U,
    eBLEHALEventStartAdvCb = 0x4000U,
    eBLEHALEventConnectionCb = 0x8000U,
    eBLEHALEventConnectionUpdateCb = 0x10000U,
    eBLEHALEventReadAttrCb = 0x20000U,
    eBLEHALEventWriteAttrCb = 0x40000U,
    eBLEHALEventIndicateCb = 0x80000U,
    eBLEHALEventConfimCb = 0x100000U,
    eBLEHALEventSSPrequestCb = 0x200000U,
    eBLEHALEventSSPrequestConfirmationCb = 0x400000U,
    eBLEHALEventPairingStateChangedCb = 0x800000U,
    eBLEHALEventRequestExecWriteCb = 0x1000000U,
    eBLEHALEventMtuChangedCb = 0x2000000U
};

typedef uint32_t BLEHALEventsTypes_t;

typedef struct
{
    IotLink_t eventList;
    BLEHALEventsTypes_t xEventTypes;
    int32_t lHandle;
} BLEHALEventsInternals_t;

typedef struct
{
    BLEHALEventsInternals_t xEvent;
    uint16_t usConnId;
    uint16_t usMtu;
} BLETESTMtuChangedCallback_t;

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
    BTBondState_t xBondState;
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

typedef enum
{
    bletestATTR_SRVCA_SERVICE,
    bletestATTR_SRVCA_CHAR_A,
    bletestATTR_SRVCA_NUMBER
} bletestAttSrvA_t;

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

typedef enum
{
    bletestATTR_SRVCC_SERVICE,
    bletestATTR_SRVCC_CHAR_A,
    bletestATTR_SRVCC_NUMBER
} bletestAttSrvC_t;

typedef struct
{
    size_t xLength;
    uint8_t ucBuffer[ bletestsSTRINGYFIED_UUID_SIZE ];
} response_t;

void prvDeviceStateChangedCb( BTState_t xState );
void prvRegisterBleAdapterCb( BTStatus_t xStatus,
                              uint8_t ucAdapterIf,
                              BTUuid_t * pxAppUuid );
void prvBTRegisterServerCb( BTStatus_t xStatus,
                            uint8_t ucServerIf,
                            BTUuid_t * pxAppUuid );
void prvBTUnregisterServerCb( BTStatus_t xStatus,
                              uint8_t ucServerIf );
void prvServiceAddedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        BTGattSrvcId_t * pxSrvcId,
                        uint16_t usServiceHandle );
void prvIncludedServiceAddedCb( BTStatus_t xStatus,
                                uint8_t ucServerIf,
                                uint16_t usServiceHandle,
                                uint16_t usInclSrvcHandle );
void prvServiceStartedCb( BTStatus_t xStatus,
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
void prvCharAddedNestedCb( BTStatus_t xStatus,
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
                     uint8_t ucAdapterIf,
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
void prvMtuChangedCb( uint16_t usConnId,
                      uint16_t usMtu );
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

BTStatus_t bleStackInit( void );
void IotTestBleHal_BLESetUp( void );
void IotTestBleHal_BLEFree( void );

void IotTestBleHal_StartAdvertisement( void );
bool IotTestBleHal_CheckBondState( void * pvEvent );
BTStatus_t IotTestBleHal_WaitEventFromQueue( BLEHALEventsTypes_t xEventName,
                                             int32_t lhandle,
                                             void * pxMessage,
                                             size_t xMessageLength,
                                             uint32_t timeoutMs );
BLEHALEventsTypes_t IotTestBleHal_WaitForEvents( BLEHALEventsTypes_t xEventsToWaitFor,
                                                 uint32_t timeoutMs );

BTStatus_t IotTestBleHal_GetEventFromQueueWithMatch( BLEHALEventsTypes_t xEventName,
                                                     int32_t lhandle,
                                                     void * pxMessage,
                                                     size_t xMessageLength,
                                                     bool ( * pxMatch )( void * pvEvent ) );

BTStatus_t IotTestBleHal_WaitEventFromQueueWithMatch( BLEHALEventsTypes_t xEventName,
                                                      int32_t lhandle,
                                                      void * pxMessage,
                                                      size_t xMessageLength,
                                                      uint32_t timeoutMs,
                                                      bool ( * pxMatch )( void * pvEvent ) );
void IotTestBleHal_ClearEventQueue( void );

void IotTestBleHal_BLEManagerInit( BTCallbacks_t * pBTmanagerCb );
void IotTestBleHal_BLEEnable( bool bEnable );
void IotTestBleHal_SetGetProperty( BTProperty_t * pxProperty,
                                   bool bIsSet );
void IotTestBleHal_StartService( BTService_t * xRefSrvc );
void IotTestBleHal_CreateServiceA( void );
void IotTestBleHal_CreateServiceB( bool bEnableIncludedSrvice );
void IotTestBleHal_CreateServiceC( void );
void IotTestBleHal_CreateStartServiceA();
void IotTestBleHal_CreateStartServiceB( bool bEnableIncludedSrvice );
void IotTestBleHal_CreateStartServiceC();
void IotTestBleHal_CreateServiceB_Nested( void );
void IotTestBleHal_WaitConnection( bool bConnected );
void IotTestBleHal_StopService( BTService_t * xRefSrvc );
void IotTestBleHal_DeleteService( BTService_t * xRefSrvc );
void IotTestBleHal_CreateSecureConnection_Model1Level4( bool IsBondSucc );
void IotTestBleHal_checkNotificationIndication( bletestAttSrvB_t xAttribute,
                                                bool enable );
void IotTestBleHal_BTUnregister( void );
void IotTestBleHal_BLEGAPInit( BTBleAdapterCallbacks_t * pBTBleAdapterCb,
                               bool bEnableCb );
void IotTestBleHal_BLEGATTInit( BTGattServerCallbacks_t * pBTGattServerCb,
                                bool bEnableCb );
void IotTestBleHal_SetAdvProperty( void );
void IotTestBleHal_SetAdvData( BTuuidType_t Type,
                               uint16_t usServiceDataLen,
                               char * pcServiceData,
                               uint16_t usManufacturerLen,
                               char * pcManufacturerData );

void prvSendNotification( bletestAttSrvB_t xAttribute,
                          bool bConfirm );
void IotTestBleHal_WriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                                          bool bNeedRsp,
                                          bool IsPrep,
                                          uint16_t usOffset );
void IotTestBleHal_CheckIndicationNotification( bool IsIndication,
                                                bool IsConnected );
BLETESTreadAttrCallback_t IotTestBleHal_ReadReceive( bletestAttSrvB_t xAttribute );
void IotTestBleHal_ReadResponse( bletestAttSrvB_t xAttribute,
                                 BLETESTreadAttrCallback_t xReadEvent,
                                 bool IsConnected );
BLETESTwriteAttrCallback_t IotTestBleHal_WriteReceive( bletestAttSrvB_t xAttribute,
                                                       bool bNeedRsp,
                                                       bool IsPrep,
                                                       uint16_t usOffset );
void IotTestBleHal_WriteResponse( bletestAttSrvB_t xAttribute,
                                  BLETESTwriteAttrCallback_t xWriteEvent,
                                  bool IsConnected );

void IotTestBleHal_StartStopAdvCheck( bool start );
void prvReadCheckAndResponse( bletestAttSrvB_t xAttribute );
size_t IotTestBleHal_ComputeNumberOfHandles( BTService_t * pxService );
extern uint16_t _bletestsMTU_SIZE;

#endif /* _IOT_TEST_BLE_HAL_COMMON_H */
