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
 * @file bt_hal_gatt_client.h
 *
 * @brief BT GATT Client provides the interfaces to use Bluetooth GATT client feature
 * @addtogroup HAL_BLUETOOTH
 *
 * Before calling any GATT client function, the Generic Access Profile needs
 * to be initialized (see bt_hal_manager.h and bt_hal_manager_adapter.h).
 * After GAP has been initialized, pxGattClientInit must be called to register
 * callbacks. After this, pxConnect can be called to connect to a GATT server.
 *
 * @{
 */

#ifndef _BT_HAL_GATT_CLIENT_H_
#define _BT_HAL_GATT_CLIENT_H_

#include <stdint.h>
#include "bt_hal_gatt_types.h"
#include "bt_hal_manager_types.h"


/**
 * @brief Buffer type for unformatted reads/writes.
 */
typedef struct
{
    uint8_t ucValue[ btGATT_MAX_ATTR_LEN ];
    uint16_t usLen;
} BTGattUnformattedValue_t;

/**
 * @brief Parameters for GATT read operations.
 */
typedef struct
{
    uint16_t usHandle;
    BTGattUnformattedValue_t xValue;
    uint16_t usValueType;
    uint8_t ucStatus;
} BTGattReadParams_t;

/**
 * @brief Parameters for GATT write operations.
 */
typedef struct
{
    BTGattSrvcId_t xSrvcId;
    BTGattInstanceId_t xCharId;
    BTGattInstanceId_t xDescrId;
    uint8_t ucStatus;
} BTGattWriteParams_t;

/**
 * @brief Parameters for attribute change notifications.
 */
typedef struct
{
    uint8_t ucValue[ btGATT_MAX_ATTR_LEN ];
    BTBdaddr_t xBda;
    uint16_t usHandle;
    size_t xLen;
    bool bIsNotify;
} BTGattNotifyParams_t;

/**
 * @brief Parameters for test command interface.
 */
typedef struct
{
    BTBdaddr_t * pxBda1;
    BTUuid_t * pxUuid1;
    uint16_t usU1;
    uint16_t usU2;
    uint16_t usU3;
    uint16_t usU4;
    uint16_t usU5;
} BTGattTestParams_t;


/**
 * @brief BT GATT client error codes.
 */
typedef enum
{
    eBTGattcCommandSuccess = 0,           /**< Command succeeded. */
    eBTGattcCommandStarted = 1,           /**< Command started OK. */
    eBTGattcCommandBusy = 2,              /**< Device busy with another command. */
    eBTGattcCommandStored = 3,            /**< Request is stored in control block. */
    eBTGattcNoResources = 4,              /**< No resources to issue command. */
    eBTGattcModeUnsupported = 5,          /**< Request for 1 or more unsupported modes. */
    eBTGattcIllegalValue = 6,             /**< Illegal command/parameter value. */
    eBTGattcIncorrectState = 7,           /**< Device in wrong state for request. */
    eBTGattcUnknownAddr = 8,              /**< Unknown remote BD address. */
    eBTGattcDeviceTimeout = 9,            /**< Device timeout. */
    eBTGattcInvalidControllerOutput = 10, /**< An incorrect value was received from HCI. */
    eBTGattcSecurityError = 11,           /**< Authorization or security failure or not authorized. */
    eBTGattcDelayedEncryptionCheck = 12,  /**< Delayed encryption check. */
    eBTGattcErrProcessing = 13            /**< Generic error. */
} BTGattcError_t;

/** BT-GATT Client callback structure. */

/**
 * @brief Callback invoked in response pxRegisterClient when the GATT client
 * registration has been completed.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucClientIf Client interface.
 *
 * @param[in] pxAppUuid GATT client UUID.
 */
typedef void ( * BTRegisterClientCallback_t)( BTGattStatus_t xStatus,
                                              uint8_t ucClientIf,
                                              BTUuid_t * pxAppUuid );

/**
 * @brief Callback invoked in response to pxSearchService when the GATT service search
 *  has been completed.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 */
typedef void ( * BTSearchCompleteCallback_t)( uint16_t usConnId,
                                              BTGattStatus_t xStatus );

/**
 * @brief Callback invoked in response to pxRegisterForNotification and
 *   pxUnregisterForNotification.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] bRegistered Set to True if attribute id registered for notifications,
 *  set to False otherwise.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 *
 * @param[in] usHandle Handle number of attribute [de]registered for notification.
 */
typedef void ( * BTRegisterForNotificationCallback_t)( uint16_t usConnId,
                                                       bool bRegistered,
                                                       BTGattStatus_t xStatus,
                                                       uint16_t usHandle );

/**
 * @brief Remote device notification callback, invoked when a remote device sends
 *  a notification or indication that a client has registered for.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] pxData Parameters for attribute change notifications.
 */
typedef void ( * BTNotifyCallback_t)( uint16_t usConnId,
                                      BTGattNotifyParams_t * pxData );

/**
 * @brief Reports result of a GATT read operation. Triggered by pxReadCharacteristic.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 *
 * @param[in] pxData Parameters for GATT read operation.
 */
typedef void ( * BTReadCharacteristicCallback_t)( uint16_t usConnId,
                                                  BTGattStatus_t xStatus,
                                                  BTGattReadParams_t * pxData );

/**
 * @brief GATT write characteristic operation callback. Triggered by
 *  pxWriteCharacteristic.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 *
 * @param[in] usHandle Handle of the attribute written to.
 */
typedef void ( * BTWriteCharacteristicCallback_t)( uint16_t usConnId,
                                                   BTGattStatus_t xStatus,
                                                   uint16_t usHandle );

/**
 * @brief GATT execute prepared write callback. Triggered by pxExecuteWrite.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 */
typedef void ( * BTExecuteWriteCallback_t)( uint16_t usConnId,
                                            BTGattStatus_t xStatus );

/**
 * @brief Callback invoked in response to pxReadDescriptor.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 *
 * @param[in] pxData Parameters for GATT read operation.
 */
typedef void ( * BTReadDescriptorCallback_t)( uint16_t usConnId,
                                              BTGattStatus_t xStatus,
                                              BTGattReadParams_t * pxData );

/**
 * @brief Callback invoked in response to pxWriteDescriptor.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 *
 * @param[in] usHandle Handle of the attribute written to.
 */
typedef void ( * BTWriteDescriptorCallback_t)( uint16_t usConnId,
                                               BTGattStatus_t xStatus,
                                               uint16_t usHandle );

/**
 * @brief Callback indicating the status of a listen() operation.
 *
 *  Callback has been deprecated, use BTAdvStatusCallback_t
 *  in BTBleAdapterCallbacks_t instead.
 */
typedef void ( * BTListenCallback_t)( BTGattStatus_t xStatus,
                                      uint32_t ulServerIf );

/**
 * @brief Callback invoked when the MTU for a given connection changes.
 *  Triggered by pxConfigureMtu.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTGattStatusSuccess if operation succeeded.
 *
 * @param[in] ulMtu MTU size.
 */
typedef void ( * BTConfigureMtuCallback_t)( uint16_t usConnId,
                                            BTGattStatus_t xStatus,
                                            uint32_t ulMtu );

/**
 * @brief GATT get database callback. Triggered by pxGetGattDb.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] pxDb GATT database elements
 *
 * @param[in] ulCount Number of elements in GATT database.
 */
typedef void ( * BTGetGattDbCallback_t)( uint16_t usConnId,
                                         BTGattDbElement_t * pxDb,
                                         uint32_t ulCount );

/**
 * @brief GATT services between startHandle and endHandle were removed.
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] usStartHandle First service handle removed from server.
 *
 * @param[in] usEndHandle Last service handle removed from server.
 */
typedef void ( * BTServicesRemovedCallback_t)( uint16_t usConnId,
                                               uint16_t usStartHandle,
                                               uint16_t usEndHandle );

/**
 * @brief GATT services were added
 *
 * @param[in] usConnId Connection Identifier, created and returned on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] pxAdded GATT database elements added.
 *
 * @param[in] ulAddedCount Number of GATT database elements added.
 */
typedef void ( * BTServicesAddedCallback_t)( uint16_t usConnId,
                                             BTGattDbElement_t * pxAdded,
                                             uint32_t ulAddedCount );

typedef struct
{
    BTRegisterClientCallback_t pxRegisterClientCb;
    BTConnectCallback_t pxOpenCb;
    BTDisconnectCallback_t pxCloseCb;
    BTSearchCompleteCallback_t pxSearchCompleteCb;
    BTRegisterForNotificationCallback_t pxRegisterForNotificationCb;
    BTNotifyCallback_t pxNotifyCb;
    BTReadCharacteristicCallback_t pxReadCharacteristicCb;
    BTWriteCharacteristicCallback_t pxWriteCharacteristicCb;
    BTReadDescriptorCallback_t pxReadDescriptorCb;
    BTWriteDescriptorCallback_t pxWriteDescriptorCb;
    BTExecuteWriteCallback_t pxExecuteWriteCb;
    BTReadRemoteRssiCallback_t pxReadRemoteRssiCb;
    BTListenCallback_t pxListenCb; /** Deprecated */
    BTConfigureMtuCallback_t pxConfigureMtuCb;
    BTCongestionCallback_t pxCongestionCb;
    BTGetGattDbCallback_t pxGetGattDbCb;
    BTServicesRemovedCallback_t pxServicesRemovedCb;
    BTServicesAddedCallback_t pxServicesAddedCb;
} BTGattClientCallbacks_t;

/** Represents the standard  BT-GATT client interface. */
typedef struct
{
    /**
     * @brief Registers a GATT client application with the stack.
     *  Triggers BTRegisterClientCallback_t.
     *
     * @param[in] pxUuid Server UUID.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxRegisterClient )( BTUuid_t * pxUuid );

    /**
     * @brief  Unregister a client application from the stack.
     *  This api does not trigger a callback.
     *
     * @param[in] ucClientIf Client interface, returned on the callback BTRegisterClientCallback_t
     *  after successful pxRegisterClient call.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxUnregisterClient )( uint8_t ucClientIf );

    /**
     * @brief Initializes the interface and provides callback routines.
     *  This api does not trigger a callback.
     *
     * @param[in] pxCallbacks Initialized callbacks for GATT client.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxGattClientInit )( const BTGattClientCallbacks_t * pxCallbacks );

    /**
     * @brief Create a connection to a remote LE or dual-mode device.
     *  Triggers BTConnectCallback_t.
     *
     * @param[in] ucClientIf Client interface, return on the callback BTRegisterClientCallback_t
     *  after successful pxRegisterClient call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @param[in] bIsDirect Set to True for direct connection.
     *
     * @param[in] xTransport Specify if BLE or BT classic is being used.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxConnect )( uint8_t ucClientIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                BTTransport_t xTransport );

    /**
     * @brief Disconnect a remote device or cancel a pending connection.
     *  Triggers BTDisconnectCallback_t.
     *
     * @param[in] ucClientIf Client interface, return on the callback BTRegisterClientCallback_t
     *  after successful pxRegisterClient call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectCallback_t is invoked.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxDisconnect )( uint8_t ucClientIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );

    /**
     * @brief Clear the attribute cache for a given device.
     *  This API does not trigger a callback.
     *
     * @param[in] ucClientIf Client interface, return on the callback BTRegisterClientCallback_t
     *  after successful pxRegisterClient call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxRefresh )( uint8_t ucClientIf,
                                const BTBdaddr_t * pxBdAddr );

    /**
     * @brief Enumerate all GATT services on a connected device.
     *  Optionally, the results can be filtered for a given UUID.
     *  Triggers BTSearchCompleteCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] pxFilterUuid Filter UUID, if set will filter by UUID.
     *  If set to NULL, no filtering will occur.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSearchService )( uint16_t usConnId,
                                      BTUuid_t * pxFilterUuid );

    /**
     * @brief Read a characteristic on a remote device.
     *  Triggers BTReadCharacteristicCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] usHandle Handle of the characteristic to be read.
     *
     * @param[in] ulAuthReq Authentication requirement.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxReadCharacteristic )( uint16_t usConnId,
                                           uint16_t usHandle,
                                           uint32_t ulAuthReq );

    /**
     * @brief Write a characteristic on a remote device. Triggers BTWriteCharacteristicCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] usHandle Handle of the characteristic to be written.
     *
     * @param[in] xWriteType Write request type.
     *
     * @param[in] xLen Data length of data to be written to characteristic attribute.
     *
     * @param[in] ulAuthReq Authentication requirement.
     *
     * @param[in] pcValue Data to be written to characteristic attribute.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxWriteCharacteristic )( uint16_t usConnId,
                                            uint16_t usHandle,
                                            BTAttrWriteRequests_t xWriteType,
                                            size_t xLen,
                                            uint32_t ulAuthReq,
                                            char * pcValue );

    /**
     * @brief Read the descriptor for a given characteristic on a remote
     *  device. Triggers BTReadDescriptorCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] usHandle Handle of the descriptor to be read.
     *
     * @param[in] ulAuthReq Authentication requirement.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxReadDescriptor )( uint16_t usConnId,
                                       uint16_t usHandle,
                                       uint32_t ulAuthReq );

    /**
     * @brief Write a remote descriptor for a given characteristic
     *  on a remote device. Triggers BTWriteDescriptorCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] usHandle Handle of the descriptor to be read.
     *
     * @param[in] xWriteType Write request type.
     *
     * @param[in] xLen Data length of data to be written to descriptor attribute.
     *
     * @param[in] ulAuthReq Authentication requirement.
     *
     * @param[in] pcValue Data to be written to descriptor attribute.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxWriteDescriptor )( uint16_t usConnId,
                                        uint16_t usHandle,
                                        BTAttrWriteRequests_t xWriteType,
                                        size_t xLen,
                                        uint32_t ulAuthReq,
                                        char * pcValue );

    /**
     * @brief Execute (or abort) a prepared write operation. Triggers BTExecuteWriteCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] bExecute If set to True, will execute the prepared write.
     *  If set to false, will abort prepared write.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxExecuteWrite )( uint16_t usConnId,
                                     bool bExecute );

    /**
     * @brief Register to receive notifications or indications for a given
     *  characteristic. Triggers BTRegisterForNotificationCallback_t.
     *
     * @param[in] ucClientIf Client interface, return on the callback
     *  BTRegisterClientCallback_t after successful pxRegisterClient call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @param[in] usHandle Handle number of attribute registered for
     *   notifications.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxRegisterForNotification )( uint8_t ucClientIf,
                                                const BTBdaddr_t * pxBdAddr,
                                                uint16_t usHandle );

    /**
     * @brief Deregister a previous request for notifications/indications.
     *  Triggers BTRegisterForNotificationCallback_t.
     *
     * @param[in] ucClientIf Client interface, return on the callback
     *  BTRegisterClientCallback_t after successful pxRegisterClient call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @param[in] usHandle Handle number of attribute deregistered for
     *  notifications.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxUnregisterForNotification )( uint8_t ucClientIf,
                                                  const BTBdaddr_t * pxBdAddr,
                                                  uint16_t usHandle );

    /**
     * @brief Request RSSI for a given remote device. Triggers
     *  BTReadRemoteRssiCallback_t in found in bt_hal_gatt_types.h.
     *
     * @param[in] ucClientIf Client interface, return on the callback
     *  BTRegisterClientCallback_t after successful pxRegisterClient call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxReadRemoteRssi )( uint8_t ucClientIf,
                                       const BTBdaddr_t * pxBdAddr );

    /**
     * @brief Determine the type of the remote device (LE, BR/EDR, Dual-mode).
     *  This api does not trigger a callback.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTTransport_t ( * pxGetDeviceType )( const BTBdaddr_t * pxBdAddr );

    /**
     * @brief Configure the MTU for a given connection.
     *  Triggers BTConfigureMtuCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on
     *  connection event, when BTConnectionCallback_t is invoked.
     *
     * @param[in] usMtu MTU size.
     *
     * @return returns eBTStatusSuccess if operation succeeded.
     */
    BTStatus_t ( * pxConfigureMtu )( uint16_t usConnId,
                                     uint16_t usMtu );

    /**
     * @brief Test mode interface.
     *
     * @param[in] ulCommand Test command to be executed.
     *
     * @param[in] pxParams Parameters for test command interface.
     *
     * @return returns eBTStatusSuccess if operation succeeded.
     */
    BTStatus_t ( * pxTestCommand )( uint32_t ulCommand,
                                    BTGattTestParams_t * pxParams );

    /**
     * @brief Get gatt db content. Triggers BTGetGattDbCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and returned on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @return returns eBTStatusSuccess if operation succeeded.
     */
    BTStatus_t ( * pxGetGattDb )( uint16_t usConnId );
} BTGattClientInterface_t;

#endif /* #ifndef _BT_HAL_GATT_CLIENT_H_ */
/** @} */
