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

#ifndef IOT_BLE_HAL_TEST_H
#define IOT_BLE_HAL_TEST_H

#include "bt_hal_manager.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_gatt_server.h"

BTStatus_t prvBleTestBtManagerInit( const BTCallbacks_t * pxCallbacks );


BTStatus_t prvBleTestBtManagerCleanup( void );

BTStatus_t prvBleTestEnable( uint8_t ucGuestMode );
BTStatus_t prvBleTestDisable( void );

BTStatus_t prvBleTestGetAllDeviceProperties( void );

BTStatus_t prvBleTestGetDeviceProperty( BTPropertyType_t xType );

BTStatus_t prvBleTestSetDeviceProperty( const BTProperty_t * pxProperty );

BTStatus_t prvBleTestGetAllRemoteDeviceProperties( BTBdaddr_t * pxRemoteAddr );


BTStatus_t prvBleTestGetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                              BTPropertyType_t xType );


BTStatus_t prvBleTestSetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                              const BTProperty_t * pxProperty );

BTStatus_t prvBleTestPair( const BTBdaddr_t * pxBdAddr,
                           BTTransport_t xTransport,
                           bool bCreateBond );

BTStatus_t prvBleTestCreateBondOutOfBand( const BTBdaddr_t * pxBdAddr,
                                          BTTransport_t xTransport,
                                          const BTOutOfBandData_t * pxOobData );


BTStatus_t prvBleTestSendSlaveSecurityRequest( const BTBdaddr_t * pxBdAddr,
                                               BTSecurityLevel_t xSecurityLevel,
                                               bool bBonding );

BTStatus_t prvBleTestCancelBond( const BTBdaddr_t * pxBdAddr );


BTStatus_t prvBleTestRemoveBond( const BTBdaddr_t * pxBdAddr );

BTStatus_t prvBleTestGetConnectionState( const BTBdaddr_t * pxBdAddr,
                                         bool * bConnectionState );

BTStatus_t prvBleTestPinReply( const BTBdaddr_t * pxBdAddr,
                               uint8_t ucAccept,
                               uint8_t ucPinLen,
                               BTPinCode_t * pxPinCode );


BTStatus_t prvBleTestSspReply( const BTBdaddr_t * pxBdAddr,
                               BTSspVariant_t xVariant,
                               uint8_t ucAccept,
                               uint32_t ulPasskey );


BTStatus_t prvBleTestReadEnergyInfo( void );

BTStatus_t prvBleTestDutModeConfigure( bool bEnable );


BTStatus_t prvBleTestDutModeSend( uint16_t usOpcode,
                                  uint8_t * pucBuf,
                                  size_t xLen );

BTStatus_t prvBleTestLeTestMode( uint16_t usOpcode,
                                 uint8_t * pucBuf,
                                 size_t xLen );


BTStatus_t prvBleTestConfigHCISnoopLog( bool bEnable );

BTStatus_t prvBleTestConfigClear( void );

BTStatus_t prvBleTestReadRssi( const BTBdaddr_t * pxBdAddr );


BTStatus_t prvBleTestGetTxpower( const BTBdaddr_t * pxBdAddr,
                                 BTTransport_t xTransport );


const void * prvBleTestGetClassicAdapter( void );

const void * prvBleTestGetLeAdapter( void );


uint32_t prvBleTestGetLastError( void );

BTStatus_t prvBleTestGetStackFeaturesSupport( uint32_t * pulFeatureMask );

BTStatus_t prvBleTestRegisterBleApp( BTUuid_t * pxAppUuid );


BTStatus_t prvBleTestUnregisterBleApp( uint8_t ucAdapterIf );

BTStatus_t prvBleTestBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks );


BTStatus_t prvBleTestGetBleAdapterProperty( BTBlePropertyType_t xType );

BTStatus_t prvBleTestSetBleAdapterProperty( const BTBleProperty_t * pxProperty );


BTStatus_t prvBleTestGetallBleRemoteDeviceProperties( BTBdaddr_t * pxRremoteAddr );


BTStatus_t prvBleTestGetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                 BTBleProperty_t xType );

BTStatus_t prvBleTestSetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                 const BTBleProperty_t * pxProperty );

BTStatus_t prvBleTestScan( bool bStart );


BTStatus_t prvBleTestConnect( uint8_t ucAdapterIf,
                              const BTBdaddr_t * pxBdAddr,
                              bool bIsDirect,
                              BTTransport_t ulTransport );


BTStatus_t prvBleTestDisconnect( uint8_t ucAdapterIf,
                                 const BTBdaddr_t * pxBdAddr,
                                 uint16_t usConnId );

BTStatus_t prvBleTestStartAdv( uint8_t ucAdapterIf );


BTStatus_t prvBleTestStopAdv( uint8_t ucAdapterIf );


BTStatus_t prvBleTestReadRemoteRssi( uint8_t ucAdapterIf,
                                     const BTBdaddr_t * pxBdAddr );

BTStatus_t prvBleTestScanFilterParamSetup( BTGattFiltParamSetup_t xFiltParam );


BTStatus_t prvBleTestScanFilterAddRemove( uint8_t ucAdapterIf,
                                          uint32_t ulAction,
                                          uint32_t ulFiltType,
                                          uint32_t ulFiltIndex,
                                          uint32_t ulCompanyId,
                                          uint32_t ulCompanyIdMask,
                                          const BTUuid_t * pxUuid,
                                          const BTUuid_t * pxUuidMask,
                                          const BTBdaddr_t * pxBdAddr,
                                          char cAddrType,
                                          size_t xDataLen,
                                          char * pcData,
                                          size_t xMaskLen,
                                          char * pcMask );

BTStatus_t prvBleTestScanFilterClear( uint8_t ucAdapterIf,
                                      uint32_t ulFiltIndex );


BTStatus_t prvBleTestScanFilterEnable( uint8_t ucAdapterIf,
                                       bool bEnable );

BTTransport_t prvBleTestGetDeviceType( const BTBdaddr_t * pxBdAddr );


BTStatus_t prvBleTestSetAdvData( uint8_t ucAdapterIf,
                                 BTGattAdvertismentParams_t * pxParams,
                                 uint16_t usManufacturerLen,
                                 char * pcManufacturerData,
                                 uint16_t usServiceDataLen,
                                 char * pcServiceData,
                                 BTUuid_t * pxServiceUuid,
                                 size_t xNbServices );

BTStatus_t prvBleTestSetAdvRawData( uint8_t ucAdapterIf,
                                    uint8_t * pucData,
                                    uint8_t ucLen );

BTStatus_t prvBleTestConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                                 uint32_t ulMinInterval,
                                                 uint32_t ulMaxInterval,
                                                 uint32_t ulLatency,
                                                 uint32_t ulTimeout );

BTStatus_t prvBleTestSetScanParameters( uint8_t ucAdapterIf,
                                        uint32_t ulScanInterval,
                                        uint32_t ulScanWindow );

BTStatus_t prvBleTestMultiAdvEnable( uint8_t ucAdapterIf,
                                     BTGattAdvertismentParams_t * xAdvParams );
BTStatus_t prvBleTestMultiAdvUpdate( uint8_t ucAdapterIf,
                                     BTGattAdvertismentParams_t * advParams );

BTStatus_t prvBleTestMultiAdvSetInstData( uint8_t ucAdapterIf,
                                          bool bSetScanRsp,
                                          bool bIncludeName,
                                          bool bInclTxpower,
                                          uint32_t ulAppearance,
                                          size_t xManufacturerLen,
                                          char * pcManufacturerData,
                                          size_t xServiceDataLen,
                                          char * pcServiceData,
                                          BTUuid_t * pxServiceUuid,
                                          size_t xNbServices );

BTStatus_t prvBleTestMultiAdvDisable( uint8_t ucAdapterIf );

BTStatus_t prvBleTestBatchscanCfgStorage( uint8_t ucAdapterIf,
                                          uint32_t ulBatchScanFullMax,
                                          uint32_t ulBatchScanTruncMax,
                                          uint32_t ulBatchScanNotifyThreshold );

BTStatus_t prvBleTestBatchscanEndBatchScan( uint8_t ucAdapterIf,
                                            uint32_t ulScanMode,
                                            uint32_t ulScanInterval,
                                            uint32_t ulScanWindow,
                                            uint32_t ulAddrType,
                                            uint32_t ulDiscardRule );

BTStatus_t prvBleTestBatchscanDisBatchScan( uint8_t ucAdapterIf );


BTStatus_t prvBleTestBatchscanReadReports( uint8_t ucAdapterIf,
                                           uint32_t ulScanMode );

BTStatus_t prvBleTestSetPreferredPhy( uint16_t usConnId,
                                      uint8_t ucTxPhy,
                                      uint8_t ucRxPhy,
                                      uint16_t usPhyOptions );

BTStatus_t prvBleTestReadPhy( uint16_t usConnId,
                              BTReadClientPhyCallback_t xCb );


const void * prvBleTestGetGattClientInterface();


const void * prvBleTestGetGattServerInterface();

BTStatus_t prvBleTestMultiAdvSetInstRawData( uint8_t ucAdapterIf,
                                             bool bSetScanRsp,
                                             uint8_t * pucData,
                                             size_t xDataLen );
BTStatus_t prvBleTestSetBgScanParameters( uint8_t ucAdapterIf,
                                          uint32_t ulScanIntervalTime,
                                          uint32_t ulScanWindowTime );

BTStatus_t prvBleTestRegisterServer( BTUuid_t * prvBleTestUuid );
BTStatus_t prvBleTestUnregisterServer( uint8_t ucServerIf );
BTStatus_t prvBleTestGattServerInit( const BTGattServerCallbacks_t * prvBleTestCallbacks );
BTStatus_t prvBleTestConnect( uint8_t ucServerIf,
                              const BTBdaddr_t * prvBleTestBdAddr,
                              bool bIsDirect,
                              BTTransport_t xTransport );
BTStatus_t prvBleTestDisconnect( uint8_t ucServerIf,
                                 const BTBdaddr_t * prvBleTestBdAddr,
                                 uint16_t usConnId );
BTStatus_t prvBleTestAddServiceBlob( uint8_t ucServerIf,
                                     BTService_t * prvBleTestService );
BTStatus_t prvBleTestAddService( uint8_t ucServerIf,
                                 BTGattSrvcId_t * prvBleTestSrvcId,
                                 uint16_t usNumHandles );
BTStatus_t prvBleTestAddIncludedService( uint8_t ucServerIf,
                                         uint16_t usServiceHandle,
                                         uint16_t usIncludedHandle );
BTStatus_t prvBleTestAddCharacteristic( uint8_t ucServerIf,
                                        uint16_t usServiceHandle,
                                        BTUuid_t * prvBleTestUuid,
                                        BTCharProperties_t xProperties,
                                        BTCharPermissions_t xPermissions );
BTStatus_t prvBleTestSetVal( BTGattResponse_t * prvBleTestValue );
BTStatus_t prvBleTestAddDescriptor( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    BTUuid_t * prvBleTestUuid,
                                    BTCharPermissions_t ulPermissions );
BTStatus_t prvBleTestStartService( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTTransport_t xTransport );
BTStatus_t prvBleTestStopService( uint8_t ucServerIf,
                                  uint16_t usServiceHandle );
BTStatus_t prvBleTestDeleteService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle );
BTStatus_t prvBleTestSendIndication( uint8_t ucServerIf,
                                     uint16_t usAttributeHandle,
                                     uint16_t usConnId,
                                     size_t xLen,
                                     uint8_t * pucValue,
                                     bool bConfirm );
BTStatus_t prvBleTestSendResponse( uint16_t usConnId,
                                   uint32_t ulTransId,
                                   BTStatus_t xStatus,
                                   BTGattResponse_t * prvBleTestResponse );
BTStatus_t prvBleTestReconnect( uint8_t ucServerIf,
                                const BTBdaddr_t * prvBleTestBdAddr );
BTStatus_t prvBleTestAddDevicesToWhiteList( uint8_t ucServerIf,
                                            const BTBdaddr_t * prvBleTestBdAddr,
                                            uint32_t ulNumberOfDevices );
BTStatus_t prvBleTestRemoveDevicesFromWhiteList( uint8_t ucServerIf,
                                                 const BTBdaddr_t * prvBleTestBdAddr,
                                                 uint32_t ulNumberOfDevices );
BTStatus_t prvBleTestConfigureMtu( uint8_t ucServerIf,
                                   uint16_t usMtu );
#endif /* IOT_BLE_HAL_TEST_H */
