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
 * @file bt_hal_manager_adapter_ble.h
 * @addtogroup HAL_BLUETOOTH
 * @brief BT manager BLE adapter, provide GAP API functions that are specific to BLE.
 *
 * Before calling pxRegisterBleApp, common GAP need to be enabled and started.
 * Flow of the functions:
 * 1. pxBleAdapterInit
 * 2. pxRegisterBleApp
 * 3. get the GATT interface to initialize GATT.
 *
 * @{
 */


#ifndef _BT_HAL_MANAGER_ADAPTER_BLE_H_
#define _BT_HAL_MANAGER_ADAPTER_BLE_H_

#include <stdint.h>
#include "bt_hal_gatt_types.h"
#include "bt_hal_manager_types.h"
#include "bt_hal_manager.h"

/* Ble Advertisement Power Levels Index. This index would translate into platform specific values power (bBm) values */
#define BT_HAL_BLE_ADV_TX_PWR_ULTRA_LOW     0              /* Ultra Low Adv Tx Power   */
#define BT_HAL_BLE_ADV_TX_PWR_LOW           1              /* Low Adv Tx Power           */
#define BT_HAL_BLE_ADV_TX_PWR_MEDIUM        2              /* Medium Adv Tx Power     */
#define BT_HAL_BLE_ADV_TX_PWR_HIGH          3              /* High Adv Tx Power          */
#define BT_HAL_BLE_ADV_TX_PWR_ULTRA_HIGH    4              /* Ultra High Adv Tx Power  */

/**
 * @brief Scan Filter Parameters
 */
typedef struct
{
    uint8_t ucAdapterIf;
    uint8_t ucAction;
    uint8_t ucFiltIndex;
    uint16_t usFeatSeln;
    uint16_t usListLogicType;
    uint8_t ucFiltLogicType;
    uint8_t ucRssiHighThres;
    uint8_t ucRssiLowThres;
    uint8_t ucDelayMode;
    uint16_t usFoundTimeout;
    uint16_t usLostTimeout;
    uint8_t ucFoundTimeout_cnt;
    uint16_t usNumOfTrackingEntries;
} BTGattFiltParamSetup_t;

/**
 * @brief Advertising Address Type
 */
typedef enum
{
    BTAddrTypePublic,
    BTAddrTypeRandom,
    BTAddrTypeStaticRandom,
    BTAddrTypeResolvable,
} BTAddrType_t;

/**
 * @brief Advertising Type
 */
typedef enum
{
    BTAdvInd,
    BTAdvDirectInd,
    BTAdvNonconnInd,
} BTAdvProperties_t;

/**
 * @brief Advertising Name Format
 */
typedef struct
{
    enum
    {
        BTGattAdvNameNone,
        BTGattAdvNameShort,
        BTGattAdvNameComplete
    } xType;
    uint8_t ucShortNameLen;
} BTGattAdvName_t;

/*TODO enum for usAdvertisingEventProperties */

/**
 * @brief Advertising Parameters
 */
typedef struct
{
    BTAdvProperties_t usAdvertisingEventProperties;
    bool bIncludeTxPower;
    BTGattAdvName_t ucName;
    bool bSetScanRsp;
    uint32_t ulAppearance;
    uint32_t ulMinInterval;            /**< Minimum Connection Interval. If set to 0, minimum connection interval is not included in advertisement/scan response data. */
    uint32_t ulMaxInterval;            /**< Maximum Connection Interval. If set to 0, maximum connection interval is not included in advertisement/scan response data. */
    uint16_t usMinAdvInterval;         /**< Minimum Advertising Interval in units of 0.625ms.
                                        *   Range: 0x0020 to 0x4000. Time Range: 20 ms to 10.24 s.
                                        *   If set to 0, stack specific default values will be used. */

    uint16_t usMaxAdvInterval;         /**< Maximum Advertising Interval in units of 0.625ms.
                                        *   Range: 0x0020 to 0x4000. Time Range: 20 ms to 10.24 s.
                                        *   If set to 0, stack specific default values will be used. */
    uint8_t ucChannelMap;              /**< The bit map to specify channel indices for advertising. If set to 0, stack specific values will be used. */
    uint8_t ucTxPower;
    uint8_t ucTimeout;                 /**< This is deprecated. Use usTimeout for advertisement duration value*/
    uint16_t usTimeout;                /**< Advertisement duration value in units of 10ms. Set to 0 for infinite timeout for advertisements. */
    uint8_t ucPrimaryAdvertisingPhy;   /* 5.0 Specific interface */
    uint8_t ucSecondaryAdvertisingPhy; /* 5.0 Specific interface */
    BTAddrType_t xAddrType;
} BTGattAdvertismentParams_t;

/**
 * @brief Local supported LE features
 */
typedef struct
{
    uint16_t usVersionSupported;
    uint8_t ucLocalPrivacyEnabled;
    uint8_t ucMaxAdvInstance;
    uint8_t ucRpaOffloadSupported;
    uint8_t ucMaxIrkListSize;
    uint8_t ucMaxAdFilterSupported;
    uint8_t ucActivityEnergyInfoSupported;
    uint16_t usScanResultStorageSize;
    uint16_t usTotalTrackableAdvertisers;
    bool bExtendedScanSupport;
    bool bDebugLoggingSupported;
} BTLocalLeFeatures_t;

/**
 * @brief BLE device property type
 */
typedef enum
{
    /**
     * Description - Local LE features
     * Access mode - GET.
     * Data type   - bt_local_le_features_t.
     */
    eBTPropertyLocalLeFeatures,
} BTBlePropertyType_t;

/**
 * @brief BLE Device Property
 */
typedef struct
{
    BTBlePropertyType_t xType;
    size_t xLen;
    void * pvVal;
} BTBleProperty_t;

/** BLE Adapter callback structure. */

/**
 *
 * @brief Callback invoked in response pxRegisterBleApp.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
 * @param[in] pxAppUuid UUID of the adapter.
 */
typedef void (* BTRegisterBleAdapterCallback_t)( BTStatus_t xStatus,
                                                 uint8_t ucAdapterIf,
                                                 BTUuid_t * pxAppUuid );

/**
 *
 * @brief  Callback invoked in response to pxGetBleAdapterProperty and pxSetBleAdapterProperty.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] ulNumProperties Number of BLE specific properties.
 * @param[in] pxProperties Properties array.
 */
typedef void (* BTBleDevicePropertiesCallback_t)( BTStatus_t xStatus,
                                                  uint32_t ulNumProperties,
                                                  BTBleProperty_t * pxProperties );

/**
 *
 * @brief Callback invoked in response to pxGetBleRemoteDeviceProperty and pxSetBleRemoteDeviceProperty.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxBdAddr Address of the Remote device
 * @param[in] ulNumProperties Number of BLE specific properties.
 * @param[in] pxProperties Properties array.
 */
typedef void (* BTBleRemoteDevicePropertiesCallback_t)( BTStatus_t xStatus,
                                                        BTBdaddr_t * pxBdAddr,
                                                        uint32_t ulNumProperties,
                                                        BTBleProperty_t * pxProperties );

/**
 *
 * @brief  Callback invoked in response to pxScan.
 *
 * @param[in] pxBda Remote device address.
 * @param[in] ulRssi
 * @param[in] pucAdvData
 */
typedef void (* BTScanResultCallback_t)( BTBdaddr_t * pxBda,
                                         uint32_t ulRssi,
                                         uint8_t * pucAdvData );

/** GATT open callback invoked in response to open */

/**
 *
 * @brief  Callback invoked on pxStartAdv and stop advertisement.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
 * @param[in] bStarted: True for start advertisement, flase for stop
 */
typedef void (* BTAdvStatusCallback_t)( BTStatus_t xStatus,
                                        uint8_t ucAdapterIf,
                                        bool bStart );

/**
 *
 * @brief Callback invoked on pxSetAdvData.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTSetAdvDataCallback_t) ( BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on when a peer device makes a request.
 * The callback is triggered by pxConnParameterUpdateRequest.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxBdAddr Address of the Remote device.
 * @param[in] ulMinInterval Minimum connection interval.
 * @param[in] ulMaxInterval Max connection interval.
 * @param[in] ulLatency Slave latency.
 * @param[in] usConnInterval Actual connection interval.
 * @param[in] ulTimeout Connection timeout.
 */
typedef void ( * BTConnParameterUpdateCallback_t )( BTStatus_t xStatus,
                                                    const BTBdaddr_t * pxBdAddr,
                                                    uint32_t ulMinInterval,
                                                    uint32_t ulMaxInterval,
                                                    uint32_t ulLatency,
                                                    uint32_t usConnInterval,
                                                    uint32_t ulTimeout );

/**
 *
 * @brief Callback invoked when a scan filter configuration command has completed.
 *
 * @param[in] ulAction
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] ulFiltType
 * @param[in] ulAvblSpace
 */
typedef void (* BTScanFilterCfgCallback_t)( uint32_t ulAction,
                                            uint8_t ucAdapterIf,
                                            BTStatus_t xStatus,
                                            uint32_t ulFiltType,
                                            uint32_t ulAvblSpace );

/**
 *
 * @brief Callback invoked on pxScanFilterParamSetup.
 *
 * @param[in] ulAction
 * @param[in] ucAdapterIf
 * @param[in] xStatus
 * @param[in] ulAvblSpace
 */
typedef void (* BTScanFilterParamCallback_t)( uint32_t ulAction,
                                              uint8_t ucAdapterIf,
                                              BTStatus_t xStatus,
                                              uint32_t ulAvblSpace );

/**
 *
 * @brief  Callback invoked on pxScanFilterEnable.
 *
 * @param[in] bEnable True if filter is enabled.
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTScanFilterStatusCallback_t)( bool bEnable,
                                               uint8_t ucAdapterIf,
                                               BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxMultiAdvEnable.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTMultiAdvEnableCallback_t)( uint8_t ucAdapterIf,
                                             BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxMultiAdvUpdate.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTMultiAdvUpdateCallback_t)( uint8_t ucAdapterIf,
                                             BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxMultiAdvSetInstData and pxMultiAdvSetInstRawData.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTMultiAdvDataCallback_t)( uint8_t ucAdapterIf,
                                           BTStatus_t xStatus );


/**
 *
 * @brief Callback invoked on pxMultiAdvDisable.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTMultiAdvDisableCallback_t)( uint8_t ucAdapterIf,
                                              BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxBatchscanCfgStorage.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTBatchscanCfgStorageCallback_t)( uint8_t ucAdapterIf,
                                                  BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxBatchscanDisBatchScan and TBD.
 *
 * @param[in] ulAction
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTBatchscanEnableDisableCallback_t)( uint32_t ulAction,
                                                     uint8_t ucAdapterIf,
                                                     BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxBatchscanReadReports.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] ulReportFormat
 * @param[in] ulNumRecords
 * @param[in] XDataLen
 * @param[in] pucRepData
 */
typedef void (* BTBatchscanReportsCallback_t)( uint8_t ucAdapterIf,
                                               BTStatus_t xStatus,
                                               uint32_t ulReportFormat,
                                               uint32_t ulNumRecords,
                                               size_t XDataLen,
                                               uint8_t * pucRepData );

/**
 *
 * @brief Callback invoked when batchscan storage threshold limit is crossed.
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
 */
typedef void (* BTBatchscanThresholdCallback_t)( uint8_t ucAdapterIf );

/**
 *
 * @brief Track ADV VSE callback invoked when tracked device is found or lost.
 *
 * @param[in] pxTrackAdvInfo
 */
typedef void (* BTTrackAdvEventCallback_t)( BTGattTrackAdvInfo_t * pxTrackAdvInfo );


/**
 *
 * @brief Callback invoked when scan parameter setup has completed
 *
 * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTScanParameterSetupCompletedCallback_t)( uint8_t ucAdapterIf,
                                                          BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxSetPreferredPhy.
 *
 * @param[in] usConnId Connection ID return from BTConnectCallback_t.
 * @param[in] ucTxPhy
 * @param[in] ucRxPhy
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTPhyUpdatedCallback_t)( uint16_t usConnId,
                                         uint8_t ucTxPhy,
                                         uint8_t ucRxPhy,
                                         BTStatus_t xStatus );

/**
 *
 * @brief Callback invoked on pxReadPhy.
 *
 * @param[in] usConnId Connection ID return from BTConnectCallback_t.
 * @param[in] ucTxPhy
 * @param[in] ucRxPhy
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTReadClientPhyCallback_t)( uint16_t usConnId,
                                            uint8_t ucTxPhy,
                                            uint8_t ucRxPhy,
                                            BTStatus_t xStatus );

/**
 * @brief Callback structure for BLE adapter.
 */
typedef struct
{
    BTRegisterBleAdapterCallback_t pxRegisterBleAdapterCb;
    BTScanResultCallback_t pxScanResultCb;
    BTBleDevicePropertiesCallback_t pxBleAdapterPropertiesCb;
    BTBleRemoteDevicePropertiesCallback_t pxBleRemoteDevicePropertiesCb;
    BTConnectCallback_t pxOpenCb;
    BTDisconnectCallback_t pxCloseCb;
    BTReadRemoteRssiCallback_t pxReadRemoteRssiCb;
    BTAdvStatusCallback_t pxAdvStatusCb;
    BTSetAdvDataCallback_t pxSetAdvDataCb;
    BTConnParameterUpdateCallback_t pxConnParameterUpdateCb;
    BTScanFilterCfgCallback_t pxScanFilterCfgCb;
    BTScanFilterParamCallback_t pxScanFilterParamCb;
    BTScanFilterStatusCallback_t pxScanFilterStatusCb;
    BTMultiAdvEnableCallback_t pxMultiAdvEnableCb;
    BTMultiAdvUpdateCallback_t pxMultiAdvUpdateCb;
    BTMultiAdvDataCallback_t pxMultiAdvDataCb;
    BTMultiAdvDisableCallback_t pxMultiAdvDisableCb;
    BTCongestionCallback_t pxCongestionCb;
    BTBatchscanCfgStorageCallback_t pxBatchscanCfgStorageCb;
    BTBatchscanEnableDisableCallback_t pxBatchscanEnbDisableCb;
    BTBatchscanReportsCallback_t pxBatchscanReportsCb;
    BTBatchscanThresholdCallback_t pxBatchscanThresholdCb;
    BTTrackAdvEventCallback_t pxTrackAdvEventCb;
    BTScanParameterSetupCompletedCallback_t pxScanParameterSetupCompletedCb;
    BTPhyUpdatedCallback_t pxPhyUpdatedCb;
} BTBleAdapterCallbacks_t;

/** Represents the standard BT-GATT client interface. */

/**
 * @brief BLE adapter API.
 */
typedef struct
{
    /**
     * @brief Registers a BLE application with the stack.
     *
     * Triggers BTRegisterBleAdapterCallback_t.
     *
     * @param[in] pxAppUuid APP UUID
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxRegisterBleApp )( BTUuid_t * pxAppUuid );

    /**
     *
     * @brief Unregister a BLE application from the stack.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxUnregisterBleApp )( uint8_t ucAdapterIf );

    /**
     *
     * @brief Initializes the interface and provides callback routines.
     *
     * @param[in] pxCallbacks Callbacks initializer for BLE adapter.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxBleAdapterInit )( const BTBleAdapterCallbacks_t * pxCallbacks );

    /**
     *
     * @brief Get Bluetooth LE Adapter property of 'type'.
     *
     * Triggers BTRegisterBleAdapterCallback_t.
     *
     * @param[in] xType Property type.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxGetBleAdapterProperty )( BTBlePropertyType_t xType );

    /**
     *
     * @brief Set Bluetooth LE Adapter property of 'type'.
     *
     * Triggers BTBleDevicePropertiesCallback_t.
     *
     * @param[in] pxProperty Property type.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetBleAdapterProperty )( const BTBleProperty_t * pxProperty );

    /**
     *
     * @brief Get all Remote Device properties.
     *
     * @param[in] pxRremoteAddr Remote device address.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxGetallBleRemoteDeviceProperties )( BTBdaddr_t * pxRremoteAddr );

    /**
     *
     * @brief Get Remote Device property of 'type'.
     *
     * @param[in] pxRemoteAddr Remote device address.
     * @param[in] xType
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxGetBleRemoteDeviceProperty )( BTBdaddr_t * pxRemoteAddr,
                                                   BTBleProperty_t xType );

    /**
     *
     * @brief Set Remote Device property of 'type' .
     *
     * @param[in] pxRemoteAddr Remote device address.
     * @param[in] pxProperty Remote device property
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetBleRemoteDeviceProperty )( BTBdaddr_t * pxRemoteAddr,
                                                   const BTBleProperty_t * pxProperty );

    /**
     *
     * @brief Start or stop LE device scanning.
     *
     * @param[in] bStart
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxScan )( bool bStart );

    /**
     *
     * @brief Create a connection to a remote LE or dual-mode device.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] pxBdAddr Address of the Remote device
     * @param[in] bIsDirect True if a direct connection is desired.
     * @param[in] xTransport Specify if BLE and BT classic is being used.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxConnect )( uint8_t ucAdapterIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                BTTransport_t ulTransport );

    /**
     *
     * @brief Disconnect a remote device or cancel a pending connection.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] pxBdAddr Address of the Remote device
     * @param[in] usConnId Connection ID return from BTConnectCallback_t
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxDisconnect )( uint8_t ucAdapterIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );

    /**
     *
     * @brief Start advertisements to listen for incoming connections.
     *
     * Triggers BTAdvStatusCallback_t with bStart = true.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxStartAdv )( uint8_t ucAdapterIf );

    /**
     *
     * @brief Stop advertisements to listen for incoming connections.
     *
     * Triggers BTAdvStatusCallback_t with bStart = false.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxStopAdv )( uint8_t ucAdapterIf );

    /**
     *
     * @brief Request RSSI for a given remote device.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] pxBdAddr Address of the Remote device.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxReadRemoteRssi )( uint8_t ucAdapterIf,
                                       const BTBdaddr_t * pxBdAddr );

    /**
     *
     * @brief Setup scan filter params.
     *
     * @param[in] xFiltParam
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxScanFilterParamSetup )( BTGattFiltParamSetup_t xFiltParam );

    /**
     *
     * @brief Configure a scan filter condition.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] ulAction
     * @param[in] ulFiltType
     * @param[in] ulFiltIndex
     * @param[in] ulCompanyId
     * @param[in] ulCompanyIdMask
     * @param[in] pxUuid UUID of the attribute.
     * @param[in] pxUuidMask
     * @param[in] pxBdAddr Address of the Remote device.
     * @param[in] cAddrType
     * @param[in] xDataLen
     * @param[in] pcData
     * @param[in] xMaskLen
     * @param[in] pcMask
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxScanFilterAddRemove )( uint8_t ucAdapterIf,
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

    /**
     *
     * @brief Clear all scan filter conditions for specific filter index.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] ulFiltIndex
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxScanFilterClear )( uint8_t ucAdapterIf,
                                        uint32_t ulFiltIndex );

    /**
     *
     * @brief Enable / disable scan filter feature.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] bEnable Set to true to enable the scan filter, false to disable.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxScanFilterEnable )( uint8_t ucAdapterIf,
                                         bool bEnable );

    /**
     *
     * @brief Determine the type of the remote device (LE, BR/EDR, Dual-mode).
     *
     * @param[in] pxBdAddr Address of the Remote device,
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTTransport_t ( * pxGetDeviceType )( const BTBdaddr_t * pxBdAddr );

    /**
     *
     * @brief Set the advertising data or scan response data.
     *
     * Triggers BTSetAdvDataCallback_t.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] pxParams Advertisement parameters.
     * @param[in] usManufacturerLen Length of Advertisement type Manufacturer data
     * @param[in] pcManufacturerData Advertisement type Manufacturer data
     * @param[in] usServiceDataLen Length of Advertisement type service data
     * @param[in] pcServiceData Advertisement type service data. UUIDs used in this service data can be only be 16bit.
     *                          If longer UUIDs are needed, use pxSetAdvRawData() instead.
     * @param[in] pxServiceUuid Array of UUIDs of advertised services.
     *                          At most one UUID of each size (16 bit, 32 bit, 128 bit) can be included in the advertisement packet.
     * @param[in] xNbServices Number of services.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetAdvData )( uint8_t ucAdapterIf,
                                   BTGattAdvertismentParams_t * pxParams,
                                   uint16_t usManufacturerLen,
                                   char * pcManufacturerData,
                                   uint16_t usServiceDataLen,
                                   char * pcServiceData,
                                   BTUuid_t * pxServiceUuid,
                                   size_t xNbServices );

    /**
     *
     * @brief Set the advertising data of complete 31 bytes or scan response data.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] pucData Ad Types serialized.
     * @param[in] ucLen Ad types serialized array len.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetAdvRawData )( uint8_t ucAdapterIf,
                                      uint8_t * pucData,
                                      uint8_t ucLen );

    /**
     *
     * @briefRequest a connection parameter update.
     * If the request is accepted, it will trigger BTConnParameterUpdateRequestCallback_t.
     *
     *
     * @param[in] pxBdAddr Address of the Remote device.
     * @param[in] ulMinInterval Minimum connection interval.
     * @param[in] ulMaxInterval Maximum connection interval.
     * @param[in] ulLatency Slave latency.
     * @param[in] ulTimeout Connection timeout.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxConnParameterUpdateRequest )( const BTBdaddr_t * pxBdAddr,
                                                   uint32_t ulMinInterval,
                                                   uint32_t ulMaxInterval,
                                                   uint32_t ulLatency,
                                                   uint32_t ulTimeout );

    /**
     *
     * @brief Sets the LE scan interval and window in units of N*0.625 msec.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] ulScanInterval
     * @param[in] ulScanWindow
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetScanParameters )( uint8_t ucAdapterIf,
                                          uint32_t ulScanInterval,
                                          uint32_t ulScanWindow );

    /**
     *
     * @brief Setup the parameters as per spec, user manual specified values and enable multi ADV.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] xAdvParams Advertisement parameters.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxMultiAdvEnable )( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t * xAdvParams );

    /**
     *
     * @brief Update the parameters as per spec, user manual specified values and restart multi ADV.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] advParams
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxMultiAdvUpdate )( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t * advParams );

    /**
     *
     * @brief Setup the data for the specified instance.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] bSetScanRsp
     * @param[in] bIncludeName
     * @param[in] bInclTxpower
     * @param[in] ulAppearance
     * @param[in] xManufacturerLen
     * @param[in] pcManufacturerData
     * @param[in] xServiceDataLen
     * @param[in] pcServiceData
     * @param[in] pxServiceUuid
     * @param[in] xNbServices
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxMultiAdvSetInstData )( uint8_t ucAdapterIf,
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

    /**
     *
     * @brief Disable the multi adv instance.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxMultiAdvDisable )( uint8_t ucAdapterIf );

    /**
     *
     * @brief Configure the batchscan storage.
     *
     * @param ucAdapterIf
     * @param ulBatchScanFullMax
     * @param ulBatchScanTruncMax
     * @param ulBatchScanNotifyThreshold
     * @return.
     */
    BTStatus_t ( * pxBatchscanCfgStorage )( uint8_t ucAdapterIf,
                                            uint32_t ulBatchScanFullMax,
                                            uint32_t ulBatchScanTruncMax,
                                            uint32_t ulBatchScanNotifyThreshold );

    /*!!TODO enum for addrType */

    /**
     *
     * @brief Enable batchscan.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] ulScanMode
     * @param[in] ulScanInterval
     * @param[in] ulScanWindow
     * @param[in] ulAddrType
     * @param[in] ulDiscardRule
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxBatchscanEndBatchScan )( uint8_t ucAdapterIf,
                                              uint32_t ulScanMode,
                                              uint32_t ulScanInterval,
                                              uint32_t ulScanWindow,
                                              uint32_t ulAddrType,
                                              uint32_t ulDiscardRule );

    /**
     *
     * @brief Disable batchscan.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @return.
     */
    BTStatus_t ( * pxBatchscanDisBatchScan )( uint8_t ucAdapterIf );

    /**
     *
     * @brief  Read out batchscan reports.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] ulScanMode
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxBatchscanReadReports )( uint8_t ucAdapterIf,
                                             uint32_t ulScanMode );

    /**
     *
     * @brief .
     *
     * @param[in] usConnId Connection ID return from BTConnectCallback_t.
     * @param[in] ucTxPhy
     * @param[in] ucRxPhy
     * @param[in] usPhyOptions
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetPreferredPhy )( uint16_t usConnId,
                                        uint8_t ucTxPhy,
                                        uint8_t ucRxPhy,
                                        uint16_t usPhyOptions );

    /**
     *
     * @brief .
     *
     * @param[in] usConnId Connection ID return from BTConnectCallback_t.
     * @param[in] xCb
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxReadPhy )( uint16_t usConnId,
                                BTReadClientPhyCallback_t xCb );

    /**
     *
     * @brief returns the GATT client interface, see bt_hal_gatt_client.h
     */
    const void * ( *ppvGetGattClientInterface )( );

    /**
     *
     * @brief returns the GATT server interface, see bt_hal_gatt_server.h
     */
    const void * ( *ppvGetGattServerInterface )( );

    /**
     *
     * @brief Setup the raw data for the specified instance.
     *
     * Triggers BTMultiAdvDataCallback_t.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp
     * @param[in] pucData raw data serialized
     * @param[in] xDataLen serialized data length
     * @param[in] bSetScanRsp
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxMultiAdvSetInstRawData )( uint8_t ucAdapterIf,
                                               bool bSetScanRsp,
                                               uint8_t * pucData,
                                               size_t xDataLen );

    /**
     *
     * @brief Sets the LE background scan interval and window in units of N*0.625 msec.
     *
     * @param[in] ucAdapterIf Adapter interface ID. Returned from BTRegisterBleAdapterCallback_t after calling pxRegisterBleApp.
     * @param[in] ulScanIntervalTime scan interval time in units of 0.625Ms
     * @param[in] ulScanWindowTime scan window time in units of 0.625Ms
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetBgScanParameters )( uint8_t ucAdapterIf,
                                            uint32_t ulScanIntervalTime,
                                            uint32_t ulScanWindowTime );
} BTBleAdapter_t;

#endif /* #ifndef _BT_HAL_MANAGER_ADAPTER_BLE_H_ */
/** @} */
