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
 * @file bt_hal_gatt_server.h
 *
 * @brief BT GATT Server provides the interfaces to use Bluetooth GATT server feature
 * @addtogroup HAL_BLUETOOTH
 *
 * Before calling any GATT server function, the Generic Access Profile needs to be initialized (see bt_hal_manager.h and bt_hal_manager_adapter*.h).
 * bt_hal_manager.h give the starting point.
 * After GAP has been initialized, the functions need to be called in that order:
 * 1. pxRegisterServer: Need to be called first to get the server Interface
 * 2. pxGattServerInit: Provided callbacks
 * 3. pxAddService: Create memory space for a service
 * 4. pxAddCharacteristic: Then all calls to pxAddDescriptor will add the descriptor to that characteristic
 * 5. pxStartService: Complete service create. Another service can be create afterward.
 *
 * @{
 */

#ifndef _BT_HAL_GATT_SERVER_H_
#define _BT_HAL_GATT_SERVER_H_

#include "bt_hal_gatt_types.h"


/**
 * @brief Error status sent inside response to a read or write from the client.
 */
typedef enum
{
    eBTRspErrorNone = 0,         /**< No error. */
    eBTRspErrorNo_mitm = 1,      /**< Require simple encryption. */
    eBTRspErrorMitm = 2,         /**< Require simple encryption + authentication. */
    eBTRspErrorSignedNoMitm = 3, /**< Require simple signed data. */
    eBTRspErrorSignedMitm = 4,   /**< Require simple signed data + authentication. */
} BTRspErrorStatus_t;

/**
 * @brief GATT value type used in response to remote read/Write requests.
 */
typedef struct
{
    uint16_t usHandle;                  /**< Handle # of the attribute accessed. */
    uint8_t * pucValue;                 /**< Value of the attribute accessed. */
    uint16_t usOffset;                  /**< Offset pointer for the data being read. */
    size_t xLen;                        /**< xLen of the pucValue field. */
    BTRspErrorStatus_t xRspErrorStatus; /**< Error status, see BTRspErrorStatus_t. */
} BTGattValue_t;

/**
 * @brief GATT response to read and write.
 */
typedef union
{
    BTGattValue_t xAttrValue; /**< GATT value of an attribute, see  BTGattValue_t. */
    uint16_t usHandle;        /**< Handle # of the attribute accessed. */
} BTGattResponse_t;

/** BT-GATT Server callback structure. */

/**
 * @brief Callback invoked in response to pxRegisterServer.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface.
 *
 * @param[in] pxAppUuid GATT server UUID.
 */
typedef void (* BTRegisterServerCallback_t)( BTStatus_t xStatus,
                                             uint8_t ucServerIf,
                                             BTUuid_t * pxAppUuid );

/**
 * @brief Callback invoked in response to pxUnregisterServer.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @return
 */
typedef void (* BTUnregisterServerCallback_t)( BTStatus_t xStatus,
                                               uint8_t ucServerIf );

/**
 * @brief Callback invoked on a connection event
 *
 * @param[in] usConnId Connection Identifier. Uniquely identify a connection.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] bConnected Flag set to true on a connect and to false on a disconnect.
 *
 * @param[in] pxBda Address of the remote device.
 */
typedef void (* BTConnectionCallback_t)( uint16_t usConnId,
                                         uint8_t ucServerIf,
                                         bool bConnected,
                                         BTBdaddr_t * pxBda );

/**
 * @brief Callback invoked in response to pxAddService.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] pxSrvcId Service ID, returned on BTServiceAddedCallback_t after calling pxAddService.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 *
 */
typedef void (* BTServiceAddedCallback_t)( BTStatus_t xStatus,
                                           uint8_t ucServerIf,
                                           BTGattSrvcId_t * pxSrvcId,
                                           uint16_t usServiceHandle );

/**
 * @brief Callback invoked in response to pxAddIncludedService.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 *
 * @param[in] usInclSrvcHandle Handle of the Attribute "included service".
 */
typedef void (* BTIncludedServiceAddedCallback_t)( BTStatus_t xStatus,
                                                   uint8_t ucServerIf,
                                                   uint16_t usServiceHandle,
                                                   uint16_t usInclSrvcHandle );

/**
 * @brief Callback invoked in response to pxAddCharacteristic.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] pxUuid UUID of the characteristic.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 *
 * @param[in] usCharHandle Handle of the characteristic being added.
 */
typedef void (* BTCharacteristicAddedCallback_t)( BTStatus_t xStatus,
                                                  uint8_t ucServerIf,
                                                  BTUuid_t * pxUuid,
                                                  uint16_t usServiceHandle,
                                                  uint16_t usCharHandle );

/**
 * @brief Callback invoked in response to pxSetVal.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] usAttrHandle Handle of the attribute.
 */
typedef void (* BTSetValCallback_t)( BTStatus_t xStatus,
                                     uint16_t usAttrHandle );

/**
 * @brief Callback invoked in response to pxAddDescriptor.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] pxUuid UUID of the descriptor.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 *
 * @param[in] usDescrHandle Handle of the descriptor being added.
 */
typedef void (* BTDescriptorAddedCallback_t)( BTStatus_t xStatus,
                                              uint8_t ucServerIf,
                                              BTUuid_t * pxUuid,
                                              uint16_t usServiceHandle,
                                              uint16_t usDescrHandle );

/**
 * @brief Callback invoked in response to pxStartService.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 */
typedef void (* BTServiceStartedCallback_t)( BTStatus_t xStatus,
                                             uint8_t ucServerIf,
                                             uint16_t usServiceHandle );

/**
 * @brief Callback invoked in response to pxStopService.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 */
typedef void (* BTServiceStoppedCallback_t)( BTStatus_t xStatus,
                                             uint8_t ucServerIf,
                                             uint16_t usServiceHandle );

/**
 * @brief Callback invoked in response to pxDeleteService.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
 *  after successful pxRegisterServer call.
 *
 * @param[in] usServiceHandle Handle of the service containing the attribute.
 */
typedef void (* BTServiceDeletedCallback_t)( BTStatus_t xStatus,
                                             uint8_t ucServerIf,
                                             uint16_t usServiceHandle );

/**
 * @brief Callback invoked on read event.
 *
 * Callback invoked when a remote device has requested to read a characteristic
 * or descriptor. The application must respond by calling sendResponse.
 *
 * @param[in] usConnId Connection Identifier, created and return on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] ulTransId Transaction ID.
 *
 * @param[in] pxBda Remote device address.
 *
 * @param[in] usAttrHandle Handle of the attribute.
 *
 * @param[in] usOffset Index of read.
 */
typedef void (* BTRequestReadCallback_t)( uint16_t usConnId,
                                          uint32_t ulTransId,
                                          BTBdaddr_t * pxBda,
                                          uint16_t usAttrHandle,
                                          uint16_t usOffset );

/**
 * @brief Callback invoked on write event.
 *
 * Callback invoked when a remote device has requested to write to a
 * characteristic or descriptor.
 *
 * @param[in] usConnId Connection Identifier, created and return on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] ulTransId Transaction ID.
 *
 * @param[in] pxBda Remote device address.
 *
 * @param[in] usAttrHandle Handle of the attribute.
 *
 * @param[in] usOffset Index of write.
 *
 * @param[in] xLength Length of pucValue to write.
 *
 * @param[in] bNeedRsp True if a response is needed.
 *
 * @param[in] bIsPrep True if it is a prepare write command.
 *
 * @param[in] pucValue Data to write.
 */
typedef void (* BTRequestWriteCallback_t)( uint16_t usConnId,
                                           uint32_t ulTransId,
                                           BTBdaddr_t * pxBda,
                                           uint16_t usAttrHandle,
                                           uint16_t usOffset,
                                           size_t xLength,
                                           bool bNeedRsp,
                                           bool bIsPrep,
                                           uint8_t * pucValue );

/**
 * @brief Callback invoked on execute write event.
 *
 * Callback invoked when a remote device that has done prepare write is now executing the command.
 *
 * @param[in] usConnId Connection Identifier, created and return on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] ulTransId Transaction ID.
 *
 * @param[in] pxBda Remote device address.
 *
 * @param[in] bExecWrite True is the write order needs to be executed.
 */
typedef void (* BTRequestExecWriteCallback_t)( uint16_t usConnId,
                                               uint32_t ulTransId,
                                               BTBdaddr_t * pxBda,
                                               bool bExecWrite );

/**
 * @brief Callback triggered in response to sendResponse if the remote device
 * sends a confirmation.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 *
 * @param[in] usHandle Handle of the attribute.
 */
typedef void (* BTResponseConfirmationCallback_t)( BTStatus_t xStatus,
                                                   uint16_t usHandle );

/**
 * @brief Callback  on confirmation device from the remote device to an indication.
 *  Or confirm a Notification as been sent.
 * @param[in] usConnId Connection Identifier, created and return on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] xStatus returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BTIndicationSentCallback_t)( uint16_t usConnId,
                                             BTStatus_t xStatus );

/**
 * @brief  Callback invoked when the MTU for a given connection changes.
 *
 * @param[in] usConnId Connection Identifier, created and return on connection event,
 *  when BTConnectionCallback_t is invoked.
 *
 * @param[in] usMtu MTU size.
 */
typedef void (* BTMtuChangedCallback_t)( uint16_t usConnId,
                                         uint16_t usMtu );

/**
 * @brief  Callback for notifying modification in white list
 *
 * @param[in] pxBdAddr Address of the Remote device.
 *
 * @param[in] bIsAdded true if device added false if device not in white list
 */
typedef void (* BTWhiteListChangedCallback_t)( const BTBdaddr_t * pxBdAddr,
                                               bool bIsAdded );

/**
 * @brief Callback structure for GATT server.
 */
typedef struct
{
    BTRegisterServerCallback_t pxRegisterServerCb;
    BTUnregisterServerCallback_t pxUnregisterServerCb;
    BTConnectionCallback_t pxConnectionCb;
    BTServiceAddedCallback_t pxServiceAddedCb;
    BTIncludedServiceAddedCallback_t pxIncludedServiceAddedCb;
    BTCharacteristicAddedCallback_t pxCharacteristicAddedCb;
    BTSetValCallback_t pxSetValCallbackCb;
    BTDescriptorAddedCallback_t pxDescriptorAddedCb;
    BTServiceStartedCallback_t pxServiceStartedCb;
    BTServiceStoppedCallback_t pxServiceStoppedCb;
    BTServiceDeletedCallback_t pxServiceDeletedCb;
    BTRequestReadCallback_t pxRequestReadCb;
    BTRequestWriteCallback_t pxRequestWriteCb;
    BTRequestExecWriteCallback_t pxRequestExecWriteCb;
    BTResponseConfirmationCallback_t pxResponseConfirmationCb;
    BTIndicationSentCallback_t pxIndicationSentCb;
    BTCongestionCallback_t pxCongestionCb;
    BTMtuChangedCallback_t pxMtuChangedCb;
    BTWhiteListChangedCallback_t pxBTWhiteListChangedCb;
} BTGattServerCallbacks_t;

/** Represents the standard BT-GATT server interface. */
typedef struct
{
    /**
     * @brief Registers a GATT server application with the stack.
     *
     * Triggers BTRegisterServerCallback_t.
     *
     * @param[in] pxUuid Server UUID.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxRegisterServer )( BTUuid_t * pxUuid );

    /**
     * @brief  Unregister a server application from the stack.
     *
     * Triggers BTUnregisterServerCallback_t.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxUnregisterServer )( uint8_t ucServerIf );

    /**
     * @brief  Initializes the interface and provides callback routines.
     *
     * @param[in] pxCallbacks Initialized callbacks for GAT server.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxGattServerInit )( const BTGattServerCallbacks_t * pxCallbacks );

    /**
     * @brief Create a connection to a remote peripheral.
     *
     * Triggers BTConnectionCallback_t with bConnected = true.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @param[in] bIsDirect Set to True for direct connection.
     *
     * @param[in] xTransport Specify if BLE and BT classic is being used.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxConnect )( uint8_t ucServerIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                BTTransport_t xTransport );

    /**
     * @brief Disconnect an established connection or cancel a pending one.
     *
     * Triggers BTConnectionCallback_t with bConnected = false.
     * When available the returned reason for disconnection should be REMOTE USER TERMINATED CONNECTION (0x13)"
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     * @param[in] usConnId Connection Identifier, created and return on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxDisconnect )( uint8_t ucServerIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );

    /**
     * @brief Create a new service with all its components (descriptors/characteristic/included services, etc..).
     * The service is started automatically.
     * @Warning: Not all platform supports changes to the attribute table while connected.
     * It should be checked that calling this API while connected  is supported by the vendor's API.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     * @param[in,out] pxService GATT Service structure. When @ref pxAddServiceBlob returns,
     * pxService->pusHandlesBuffer is filled with corresponding attribute handles.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxAddServiceBlob )( uint8_t ucServerIf,
                                       BTService_t * pxService );

    /**
     * @brief Create a new service.
     *
     * Triggers BTServiceAddedCallback_t.
     * @Warning: Not all platform supports changes to the attribute table while connected.
     * It should be checked that calling this API while connected  is supported by the vendor's API.e
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] pxSrvcId Service ID, returned on BTServiceAddedCallback_t after calling pxAddService.
     *
     * @param[in] usNumHandles Number of handles in the service.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxAddService )( uint8_t ucServerIf,
                                   BTGattSrvcId_t * pxSrvcId,
                                   uint16_t usNumHandles );

    /**
     * @brief Assign an included service to it's parent service.
     *
     * Triggers BTIncludedServiceAddedCallback_t.
     * @Warning: Not all platform supports changes to the attribute table while connected.
     * It should be checked that calling this API while connected  is supported by the vendor's API.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usServiceHandle Handle of the service containing the attributes.
     *
     * @param[in] usIncludedHandle Handle of the included service.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxAddIncludedService )( uint8_t ucServerIf,
                                           uint16_t usServiceHandle,
                                           uint16_t usIncludedHandle );

    /**
     * @brief Add a characteristic to a service. Note: Characteristic declaration is added automatically.
     *
     * Characteristics are created in order in which the function pxAddCharacteristic is called.
     * Descriptor created will belong to the characteristic that was created last.
     * Triggers BTCharacteristicAddedCallback_t.
     * @Warning: Not all platform supports changes to the attribute table while connected.
     * It should be checked that calling this API while connected  is supported by the vendor's API.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usServiceHandle Handle of the service containing the attributes.
     *
     * @param[in] pxUuid UUID of the attribute.
     *
     * @param[in] xProperties Properties of the characteristic.
     *
     * @param[in] xPermissions Permission to read/write the characteristic.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxAddCharacteristic )( uint8_t ucServerIf,
                                          uint16_t usServiceHandle,
                                          BTUuid_t * pxUuid,
                                          BTCharProperties_t xProperties,
                                          BTCharPermissions_t xPermissions );

    /**
     * @brief Set the primitive/complete values only. i.e data can be read in one transaction,
     *   if we need multiple then we need to use send response.
     *
     * Triggers BTSetValCallback_t.
     *
     * @param[in] pxValue APP response.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSetVal )( BTGattResponse_t * pxValue );

    /**
     * @brief Add a descriptor to a given service.
     *
     * The descriptor will belong to the first characteristic above it in the attribute table for that service.
     * @WARNING: CCCD need to be created first so they are just below their respective characteristics.
     * Since some stacks will add them automatically right after the characteristic.
     * @Warning: Not all platform supports changes to the attribute table while connected.
     * It should be checked that calling this API while connected  is supported by the vendor's API.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usServiceHandle Handle of the service containing the attributes.
     *
     * @param[in] pxUuid UUID of the attribute.
     *
     * @param[in] ulPermissions Permissions to read/write the descriptor.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxAddDescriptor )( uint8_t ucServerIf,
                                      uint16_t usServiceHandle,
                                      BTUuid_t * pxUuid,
                                      BTCharPermissions_t ulPermissions );

    /**
     * @brief Starts a local service.
     *
     * Triggers BTServiceStartedCallback_t.
     * A service become discoverable after being started.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usServiceHandle Handle of the service containing the attributes.
     *
     * @param[in] xTransport Specify if BLE and BT classic is being used.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxStartService )( uint8_t ucServerIf,
                                     uint16_t usServiceHandle,
                                     BTTransport_t xTransport );

    /**
     * @brief Stops a local service if supported by the underlying stack.
     *
     * Triggers BTServiceStoppedCallback_t.
     * @NOTE: Not a mandatory API as not every stack supports it.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usServiceHandle Handle of the service containing the attributes.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxStopService )( uint8_t ucServerIf,
                                    uint16_t usServiceHandle );

    /**
     * @brief Delete a local service.
     *
     * Triggers BTServiceDeletedCallback_t.
     * @Warning: Not all platform supports changes to the attribute table while connected.
     * It should be checked that calling this API while connected  is supported by the vendor's API.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usServiceHandle Handle of the service containing the attributes.
     *
     * @return Returns eBTStatusSuccess on successful call
     */
    BTStatus_t ( * pxDeleteService )( uint8_t ucServerIf,
                                      uint16_t usServiceHandle );

    /**
     * @brief Send value indication to a remote device.
     *
     * Triggers BTIndicationSentCallback_t.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] usAttributeHandle.
     *
     * @param[in] usConnId Connection Identifier, created and return on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] xLen Data length.
     *
     * @param[in] pucValue Data.
     *
     * @param[in] bConfirm True for indication, false for notification.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSendIndication )( uint8_t ucServerIf,
                                       uint16_t usAttributeHandle,
                                       uint16_t usConnId,
                                       size_t xLen,
                                       uint8_t * pucValue,
                                       bool bConfirm );

    /**
     * @brief Send a response to a read/write operation.
     *
     * Triggers BTResponseConfirmationCallback_t.
     *
     * @param[in] usConnId Connection Identifier, created and return on connection event,
     *  when BTConnectionCallback_t is invoked.
     *
     * @param[in] ulTransId. Transaction ID
     *
     * @param[in] xStatus Status returned by the APP.
     *
     * @param[in] pxResponse APP response.
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxSendResponse )( uint16_t usConnId,
                                     uint32_t ulTransId,
                                     BTStatus_t xStatus,
                                     BTGattResponse_t * pxResponse );

    /**
     * @brief Start directed advertising to the device address passed this device should
     * be already part of the white list
     *
     * On connection triggers BTConnectionCallback_t with bConnected = true.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] pxBdAddr Address of the Remote device.
     *
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxReconnect )( uint8_t ucServerIf,
                                  const BTBdaddr_t * pxBdAddr );

    /**
     * @brief Add devices to whitelist
     *
     * Triggers BTWhiteListChangedCallback_t with bIsAdded = true.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] pxBdAddr Array of addresses of Remote devices.
     *
     * @param[in] ucNumberOfDevices Number of bluetooth device address passes
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxAddDevicesToWhiteList )( uint8_t ucServerIf,
                                              const BTBdaddr_t * pxBdAddr,
                                              uint32_t ulNumberOfDevices );

    /**
     * @brief Remove device from whitelist and stops any ongoing directed advertisements
     *
     * Triggers BTWhiteListChangedCallback_t with bIsAdded = false.
     *
     * @param[in] ucServerIf Server interface, return on the callback BTRegisterServerCallback_t
     *  after successful pxRegisterServer call.
     *
     * @param[in] pxBdAddr Array of addresses of Remote devices.
     *
     * @param[in] ucNumberOfDevices Number of bluetooth device address passes
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxRemoveDevicesFromWhiteList )( uint8_t ucServerIf,
                                                   const BTBdaddr_t * pxBdAddr,
                                                   uint32_t ulNumberOfDevices );

    /**
     * @brief Configure the MTU for gatt server
     *
     *
     * @param[in] ucServerIf Server interface
     *
     * @param[in] usMtu mtu that needs to be used for the gatt connection
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxConfigureMtu )( uint8_t ucServerIf,
                                     uint16_t usMtu );
} BTGattServerInterface_t;

#endif /* _BT_HAL_GATT_SERVER_H_ */
/** @} */
