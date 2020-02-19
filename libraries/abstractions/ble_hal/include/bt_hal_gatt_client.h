/*
 * FreeRTOS BLE HAL V4.0.1
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
 * @brief BT GATT Server provides the interfaces to use Bluetooth GATT client feature
 * @addtogroup HAL_BLUETOOTH
 * USAGE
 * -----
 *
 * @{
 */


#ifndef _BT_HAL_GATT_CLIENT_H_
#define _BT_HAL_GATT_CLIENT_H_

#include <stdint.h>
#include "bt_hal_gatt_types.h"
#include "bt_hal_manager_types.h"


/** Buffer type for unformatted reads/writes */
typedef struct
{
    uint8_t ucValue[ btGATT_MAX_ATTR_LEN ];
    uint16_t usLen;
} BTGattUnformattedValue_t;

/** Parameters for GATT read operations */
typedef struct
{
    uint16_t usHandle;
    BTGattUnformattedValue_t xValue;
    uint16_t usValueType;
    uint8_t ucStatus;
} BTGattReadParams_t;

/** Parameters for GATT write operations */
typedef struct
{
    BTGattSrvcId_t xSrvcId;
    BTGattInstanceId_t xCharId;
    BTGattInstanceId_t xDescrId;
    uint8_t ucStatus;
} BTGattWriteParams_t;

/** Attribute change notification parameters */
typedef struct
{
    uint8_t ucValue[ btGATT_MAX_ATTR_LEN ];
    BTBdaddr_t xBda;
    uint16_t usHandle;
    size_t xLen;
    bool bIsNotify;
} BTGattNotifyParams_t;

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

/* BT GATT client error codes */
typedef enum
{
    eBTGattcCommandSuccess = 0,      /* 0  Command succeeded                 */
    eBTGattcCommandStarted,          /* 1  Command started OK.               */
    eBTGattcCommandBusy,             /* 2  Device busy with another command  */
    eBTGattcCommandStored,           /* 3 request is stored in control block */
    eBTGattcNoResources,             /* 4  No resources to issue command     */
    eBTGattcModeUnsupported,         /* 5  Request for 1 or more unsupported modes */
    eBTGattcIllegalValue,            /* 6  Illegal command /parameter value  */
    eBTGattcIncorrectState,          /* 7  Device in wrong state for request  */
    eBTGattcUnknownAddr,             /* 8  Unknown remote BD address         */
    eBTGattcDeviceTimeout,           /* 9  Device timeout                    */
    eBTGattcInvalidControllerOutput, /* 10  An incorrect value was received from HCI */
    eBTGattcSecurityError,           /* 11 Authorization or security failure or not authorized  */
    eBTGattcDelayedEncryptionCheck,  /*12 Delayed encryption check */
    eBTGattcErrProcessing            /* 12 Generic error                     */
} BTGattcError_t;

/** BT-GATT Client callback structure. */

/** Callback invoked in response to registerClient */
typedef void ( * BTRegisterClientCallback_t)( BTStatus_t xStatus,
                                              uint8_t ucClientIf,
                                              BTUuid_t * pxAppUuid );

/**
 * Invoked in response to searchService when the GATT service search
 * has been completed.
 */
typedef void ( * BTSearchCompleteCallback_t)( uint16_t usConnId,
                                              BTStatus_t xStatus );

/** Callback invoked in response to [de]registerForNotification */
typedef void ( * BTRegisterForNotificationCallback_t)( uint16_t usConnId,
                                                       bool bRegistered,
                                                       BTStatus_t xStatus,
                                                       uint16_t usHandle );

/**
 * Remote device notification callback, invoked when a remote device sends
 * a notification or indication that a client has registered for.
 */
typedef void ( * BTNotifyCallback_t)( uint16_t usConnId,
                                      BTGattNotifyParams_t * pxData );

/** Reports result of a GATT read operation */
typedef void ( * BTReadCharacteristicCallback_t)( uint16_t usConnId,
                                                  BTStatus_t xStatus,
                                                  BTGattReadParams_t * pxData );

/** GATT write characteristic operation callback */
typedef void ( * BTWriteCharacteristicCallback_t)( uint16_t usConnId,
                                                   BTStatus_t xStatus,
                                                   uint16_t usHandle );

/** GATT execute prepared write callback */
typedef void ( * BTExecuteWriteCallback_t)( uint16_t usConnId,
                                            BTStatus_t xStatus );

/** Callback invoked in response to readDescriptor */
typedef void ( * BTReadDescriptorCallback_t)( uint16_t usConnId,
                                              BTStatus_t xStatus,
                                              BTGattReadParams_t * pxData );

/** Callback invoked in response to writeDescriptor */
typedef void ( * BTWriteDescriptorCallback_t)( uint16_t usConnId,
                                               BTStatus_t xStatus,
                                               uint16_t usHandle );

/**
 * Callback indicating the status of a listen() operation
 */
typedef void ( * BTListenCallback_t)( BTStatus_t xStatus,
                                      uint32_t ulServerIf );

/** Callback invoked when the MTU for a given connection changes */
typedef void ( * BTConfigureMtuCallback_t)( uint16_t usConnId,
                                            BTStatus_t xStatus,
                                            uint32_t ulMtu );

/** GATT get database callback */
typedef void ( * BTGetGattDbCallback_t)( uint16_t usConnId,
                                         BTGattDbElement_t * pxDb,
                                         uint32_t ulCount );

/** GATT services between startHandle and endHandle were removed */
typedef void ( * BTServicesRemovedCallback_t)( uint16_t usConnId,
                                               uint16_t usStartHandle,
                                               uint16_t usEndHandle );

/** GATT services were added */
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
    BTListenCallback_t pxListenCb;
    BTConfigureMtuCallback_t pxConfigureMtuCb;
    BTCongestionCallback_t pxCongestionCb;
    BTGetGattDbCallback_t pxGetGattDbCb;
    BTServicesRemovedCallback_t pxServicesRemovedCb;
    BTServicesAddedCallback_t pxServicesAddedCb;
} BTGattClientCallbacks_t;

/** Represents the standard  BT-GATT client interface. */

typedef struct
{
    /** Registers a GATT client application with the stack */
    BTStatus_t ( * pxRegisterClient )( BTUuid_t * pxUuid );

    /** Unregister a client application from the stack */
    BTStatus_t ( * pxUnregisterClient )( uint8_t ucClientIf );

    /**
     * Initializes the interface and provides callback routines
     */
    BTStatus_t ( * pxGattClientInit )( const BTGattClientCallbacks_t * pxCallbacks );

    /** Create a connection to a remote LE or dual-mode device */
    BTStatus_t ( * pxConnect )( uint8_t ucClientIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                BTTransport_t xTransport );

    /** Disconnect a remote device or cancel a pending connection */
    BTStatus_t ( * pxDisconnect )( uint8_t ucClientIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );

    /** Clear the attribute cache for a given device */
    BTStatus_t ( * pxRefresh )( uint8_t ucClientIf,
                                const BTBdaddr_t * pxBdAddr );

    /**
     * Enumerate all GATT services on a connected device.
     * Optionally, the results can be filtered for a given UUID.
     */
    BTStatus_t ( * pxSearchService )( uint16_t usConnId,
                                      BTUuid_t * pxFilterUuid );

    /** Read a characteristic on a remote device */
    /*!!TODO Types for auth request */
    BTStatus_t ( * pxReadCharacteristic )( uint16_t usConnId,
                                           uint16_t usHandle,
                                           uint32_t ulAuthReq );

    /** Write a remote characteristic */
    /*!!TODO Types for auth request */
    BTStatus_t ( * pxWriteCharacteristic )( uint16_t usConnId,
                                            uint16_t usHandle,
                                            BTAttrWriteRequests_t xWriteType,
                                            size_t xLen,
                                            uint32_t ulAuthReq,
                                            char * pcValue );

    /** Read the descriptor for a given characteristic */
    /*!!TODO Types for auth request */
    BTStatus_t ( * pxReadDescriptor )( uint16_t usConnId,
                                       uint16_t usHandle,
                                       uint32_t ulAuthReq );

    /** Write a remote descriptor for a given characteristic */
    BTStatus_t ( * pxWriteDescriptor )( uint16_t usConnId,
                                        uint16_t usHandle,
                                        BTAttrWriteRequests_t xWriteType,
                                        size_t xLen,
                                        uint32_t ulAuthReq,
                                        char * pcValue );

    /** Execute a prepared write operation */
    BTStatus_t ( * pxExecuteWrite )( uint16_t usConnId,
                                     bool bExecute );

    /**
     * Register to receive notifications or indications for a given
     * characteristic
     */
    BTStatus_t ( * pxRegisterForNotification )( uint8_t ucClientIf,
                                                const BTBdaddr_t * pxBdAddr,
                                                uint16_t usHandle );

    /** Deregister a previous request for notifications/indications */
    BTStatus_t ( * pxUnregisterForNotification )( uint8_t ucClientIf,
                                                  const BTBdaddr_t * pxBdAddr,
                                                  uint16_t usHandle );

    /** Request RSSI for a given remote device */
    BTStatus_t ( * pxReadRemoteRssi )( uint8_t ucClientIf,
                                       const BTBdaddr_t * pxBdAddr );

    /** Determine the type of the remote device (LE, BR/EDR, Dual-mode) */
    BTTransport_t ( * pxGetDeviceType )( const BTBdaddr_t * pxBdAddr );

    /** Configure the MTU for a given connection */
    BTStatus_t ( * pxConfigureMtu )( uint16_t usConnId,
                                     uint16_t usMtu );

    /** Test mode interface */
    BTStatus_t ( * pxTestCommand )( uint32_t ulCommand,
                                    BTGattTestParams_t * pxParams );

    /** Get gatt db content */
    BTStatus_t ( * pxGetGattDb )( uint16_t usConnId );
} BTGattClientInterface_t;

#endif /* #ifndef _BT_HAL_GATT_CLIENT_H_ */
/** @} */
