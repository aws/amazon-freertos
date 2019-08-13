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
 * @file iot_test_ble_hal_afqp.h
 * @brief
 */

#ifndef _IOT_TEST_BLE_HAL_AFQP_H_
#define _IOT_TEST_BLE_HAL_AFQP_H_

#include "iot_test_ble_hal_common.h"


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

static void prvSetGetProperty( BTProperty_t * pxProperty,
                               bool bIsSet );
static void prvStartAdvertisement( void );
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

#endif /* _IOT_TEST_BLE_HAL_AFQP_H_ */
