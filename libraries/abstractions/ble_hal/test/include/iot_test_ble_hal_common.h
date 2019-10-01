/*
 * Amazon FreeRTOS BLE HAL V1.0.0
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
#include IOT_PLATFORM_TYPES
#include IOT_BT_HAL_MANAGER_ADAPTER_BLE
#include IOT_BT_HAL_MANAGER_ADAPTER
#include IOT_BT_HAL_GATT_SERVER
#include IOT_BT_HAL_GATT_TYPES
/* Test framework includes. */
#include IOT_UNITY_FIXTURE
#include IOT_UNITY

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

#define bletestsAPP_UUID                 { 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsSERVER_UUID              { 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define bletestsFREERTOS_SVC_UUID_128    { 0x5A, 0xDB, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define bletestsFREERTOS_SVC_UUID_32     0x8A7F1168
#define bletestsFREERTOS_SVC_UUID_16     0xabcd

#define NO_HANDLE                        -1

#define MAX_EVENT                        20

#define bletestsDEFAULT_CHAR_VALUE       "hello"
#define bletestsSTRINGYFIED_UUID_SIZE    36 /* like "8a7f1168-48af-4efb-83b5-e679f9320002" */
#define bletestsFULL_PERMISSIONS         ( eBTPermRead | eBTPermWrite )
#define bletestsNB_INCLUDEDSERVICES      1

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

#define BLE_TESTS_WAIT                        60000                 /* Wait 60s max */
#define BLE_TESTS_SHORT_WAIT                  4000                  /* Wait 4s max */

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

BTStatus_t bleStackInit( void );
void IotTestBleHal_BLESetUp( void );
void IotTestBleHal_BLEFree( void );

void IotTestBleHal_StartAdvertisement( void );
BTStatus_t IotTestBleHal_WaitEventFromQueue( BLEHALEventsTypes_t xEventName,
                                             int32_t lhandle,
                                             void * pxMessage,
                                             size_t xMessageLength,
                                             uint32_t timeoutMs );
void IotTestBleHal_BLEManagerInit( void );
void IotTestBleHal_BLEEnable( bool bEnable );
void IotTestBleHal_StartService( BTService_t * xRefSrvc );
void IotTestBleHal_CreateServiceA( void );
void IotTestBleHal_CreateServiceB( void );
void IotTestBleHal_WaitConnection( bool bConnected );
void IotTestBleHal_StopService( BTService_t * xRefSrvc );
void IotTestBleHal_DeleteService( BTService_t * xRefSrvc );
void IotTestBleHal_checkNotificationIndication( bletestAttSrvB_t xAttribute,
                                                bool enable );
void IotTestBleHal_BTUnregister( void );
void IotTestBleHal_BLEGAPInit( void );
void IotTestBleHal_BLEGATTInit( void );
void IotTestBleHal_SetAdvProperty( void );
void IotTestBleHal_SetAdvData( BTuuidType_t Type,
                               uint16_t usManufacturerLen,
                               char * pcManufacturerData );

void prvSendNotification( bletestAttSrvB_t xAttribute,
                          bool bConfirm );
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

#endif /* _IOT_TEST_BLE_HAL_COMMON_H */
