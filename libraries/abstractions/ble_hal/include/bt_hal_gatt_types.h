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
 * @file bt_hal_gatt_types.h
 *
 * @brief T HAL provides the standard defintions used by BT GATT interfaces.
 * @addtogroup HAL_BLUETOOTH
 * USAGE
 * -----
 *
 * @{
 */
#ifndef _BT_HAL_GATT_TYPES_H_
#define _BT_HAL_GATT_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

#include "bt_hal_manager.h"
#include "bt_hal_manager_types.h"

/**
 * Buffer sizes for maximum attribute length and maximum read/write
 * operation buffer size.
 */
#define btGATT_MAX_ATTR_LEN    600

/**
 * @brief GATT Status Codes
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F, 3.4.1.1 Error Response
 *
 */
typedef enum
{
    eBTGattStatusSuccess = 0x00,                      /**< Success */
    eBTInvalidHandle = 0x01,                          /**< Invalid handle */
    eBTGattStatusReadNotPermitted = 0x02,             /**< Characteristic does not support read */
    eBTGattStatusWriteNotPermitted = 0x03,            /**< Characteristic does not support write */
    eBTGattStatusInvalidPDU = 0x04,                   /**< Invalid PDU */
    eBTGattStatusInsufficientAuthentication = 0x05,   /**< Link is not properly Authenticated */
    eBTGattStatusRequestNotSupported = 0x06,          /**< Operation not supported */
    eBTGattStatusInvalidOffset = 0x07,                /**< Invalid offset (long writes/reads) */
    eBTGattStatusInsufficientAuthorization = 0x08,    /**< Link is not properly authorized */
    eBTGattStatusPrepareQueueFull = 0x09,             /**< Too many prepare writes queued */
    eBTGattStatusAttributeNotFound = 0x0A,            /**< No attribute found within handle range */
    eBTGattStatusAttributeNotLong = 0x0B,             /**< Cannot be read with Read Blob Request */
    eBTGattStatusInsufficientKeySize = 0x0C,          /**< Link is not properly Encrypted */
    eBTGattStatusInvalidAttributeLength = 0x0D,       /**< Bad Attribute Length */
    eBTGattStatusUnlikelyError = 0x0E,                /**< Encountered an unlikeley error */
    eBTGattStatusInsufficientEncryption = 0x0F,       /**< Link is not properly Encrypted */
    eBTGattStatusUnsupportedGroupType = 0x10,         /**< Unsupported grouping attribute */
    eBTGattStatusInsufficientResources = 0x11,        /**< Insufficient Resources */
    eBTGattStatusInternalError = 0x81,                /**< Internal GATT Error */
    eBTGattStatusError = 0x85,                        /**< Generic GATT Error */
    eBTGattStatusConnectionCongested = 0x8f,          /**< Congested connection */
    eBTGattStatusErrorConnEstFail = 0x93,             /**< Failed to establish gatt connection */
    eBTGattStatusErrorConnTimeout = 0x94,             /**< Connection Timed out */
    eBTGattStatusLocalHostTerminatedConnection = 0x99 /**< Disconnect from Local Host */
} BTGattStatus_t;

/**
 * @brief GATT Characteristic property.
 *
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G, 3.3.1.1 Characteristic Properties
 */
enum
{
    eBTPropNone = 0x0000,
    eBTPropBroadcast = 0x0001,
    eBTPropRead = 0x0002,
    eBTPropWriteNoResponse = 0x0004,
    eBTPropWrite = 0x0008,
    eBTPropNotify = 0x0010,
    eBTPropIndicate = 0x0020,
    eBTPropSignedWrite = 0x0040,
    eBTPropExtendedProps = 0x0080,
};
typedef uint16_t BTCharProperties_t;

/**
 * @brief GATT permissions.
 *
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F, 3.2.5 Attribute Permissions
 */
enum
{
    eBTPermNone = 0x0000,
    eBTPermRead = 0x0001,               /**< Readable, Encryption not required, No Authentication Required. */
    eBTPermReadEncrypted = 0x0002,      /**< Readable, Encryption required, No Authentication Required. */
    eBTPermReadEncryptedMitm = 0x0004,  /**< Readable, Encryption required, Authentication Required. */
    eBTPermWrite = 0x0010,              /**< Writable, Encryption not required, No Authentication Required. */
    eBTPermWriteEncrypted = 0x0020,     /**< Writable, Encryption required, No Authentication Required. */
    eBTPermWriteEncryptedMitm = 0x0040, /**< Writable, Encryption required, Authentication Required. */
    eBTPermWriteSigned = 0x0080,        /**< Writable, Signed required, No Authentication Required. */
    eBTPermWriteSignedMitm = 0x0100,    /**< Writable, Signed required, Authentication Required. */
};
typedef uint16_t BTCharPermissions_t;

/**
 * @brief Write request type.
 */
typedef enum
{
    eBTWriteTypeNoResponse = 0x0001, /**< Write request, do not need response. */
    eBTWriteTypeDefault = 0x0002,    /**< Write request, need a response. */
    eBTWriteTypePrepare = 0x0003,    /**< Prepared Write request */
    eBTWriteTypeSigned = 0x0004,     /**< Signed Write request, need a response. */
} BTAttrWriteRequests_t;


/**
 * @brief GATT Service types
 */
typedef enum
{
    eBTServiceTypePrimary,   /**< Primary service. */
    eBTServiceTypeSecondary, /**< Secondary service. */
} BTGattServiceTypes_t;

/**
 * @brief GATT service instance ID.
 */
typedef struct
{
    BTUuid_t xUuid;   /**< Service UUID. */
    uint8_t ucInstId; /**< Service instance ID. */
} BTGattInstanceId_t;

/**
 * @brief GATT Service ID.
 */
typedef struct
{
    BTGattInstanceId_t xId;            /**< Service instance ID. */
    BTGattServiceTypes_t xServiceType; /**< Service type. */
} BTGattSrvcId_t;


/**
 * @brief Structure that contains all advertisements info. Used to compress parameters in BTTrackAdvEventCallback_t.
 */
typedef struct
{
    uint8_t ucClientIf;              /**< Client GATT interface. */
    uint8_t ucFiltIndex;             /**< @TODO. */
    uint8_t ucAdvertiserState;       /**< @TODO. */
    uint8_t ucAdvertiserInfoPresent; /**< @TODO. */
    uint8_t ucAddrType;              /**< @TODO. */
    uint8_t ucTxPower;               /**< @TODO. */
    int8_t cRssiValue;               /**< @TODO. */
    uint16_t usTimeStamp;            /**< @TODO. */
    BTBdaddr_t xBdAddr;              /**< @TODO. */
    uint8_t ucAdvPktLen;             /**< @TODO. */
    uint8_t * pucAdvPktData;         /**< @TODO. */
    size_t xScanRspLen;              /**< @TODO. */
    uint8_t * pucScanRspData;        /**< @TODO. */
} BTGattTrackAdvInfo_t;

/**
 * @brief Attribute types.
 */
typedef enum
{
    eBTDbPrimaryService,     /**< Primary service. */
    eBTDbSecondaryService,   /**< Secondary service. */
    eBTDbIncludedService,    /**< Included service. */
    eBTDbCharacteristicDecl, /**< Characteristic declaration. */
    eBTDbCharacteristic,     /**< Characteristic. */
    eBTDbDescriptor,         /**< Characteristic descriptor. */
} BTDbAttributeType_t;

typedef struct BTService BTService_t;

/**
 * @brief Structure describing a characteristic.
 */
typedef struct
{
    BTUuid_t xUuid;                   /**< Attribute UUID*/
    BTCharProperties_t xProperties;   /**< Characteristic properties. */
    BTCharPermissions_t xPermissions; /**< Characteristic permissions. */
} BTCharacteristic_t;

/**
 * @brief Structure describing a characteristic descriptor.
 */
typedef struct
{
    BTUuid_t xUuid;                   /**< Attribute UUID*/
    BTCharPermissions_t xPermissions; /**< Descriptor permissions. */
} BTCharacteristicDescr_t;

/**
 * @brief  Structure describing an included service.
 */
typedef struct
{
    BTUuid_t xUuid;               /**< Attribute UUID*/
    BTService_t * pxPtrToService; /**< Pointer to the service being included. */
} BTIncludedService_t;

/**
 * @brief  Structure describing a service UUID.
 */
typedef BTUuid_t BTServiceUUID_t;

/**
 * @brief Generic BLE attribute.
 */
typedef struct
{
    BTDbAttributeType_t xAttributeType; /**< Type of attribute. */
    union
    {
        BTServiceUUID_t xServiceUUID;                 /**< UUID of the service. */
        BTCharacteristic_t xCharacteristic;           /**< Characteristic. */
        BTCharacteristicDescr_t xCharacteristicDescr; /**< Descriptor. */
        BTIncludedService_t xIncludedService;         /**< Included service. */
    };
} BTAttribute_t;

/**
 * @brief Structure describing a service.
 * Note, handles are allocated separately so the attribute array can be allocated in ROM.
 * pxHandlesBuffer has to dimensions: x and y [x][y] .
 * x : Number of copies of the service
 * y : needs to be equal to xNumberOfAttributes
 *
 * That structure has been constructed with the intent of putting most
 * of it in ROM. The whole structure can be put in ROM. If copies are needed then only pxBLEAttributes can be constant.
 * The first attribute is the UUID of the service.
 */
struct BTService
{
    uint8_t ucInstId;                /**< Service Instance ID. */
    BTGattServiceTypes_t xType;      /**< Service type. */
    size_t xNumberOfAttributes;      /**< Number of attributes. */
    uint16_t * pusHandlesBuffer;     /**< Array of handles, mapping to pxBLEAttributes. */
    BTAttribute_t * pxBLEAttributes; /**< Array of attribute, can be allocated in ROM. */
};

typedef struct
{
    uint16_t usId;
    BTUuid_t xUuid;
    BTDbAttributeType_t xType;
    uint16_t usAttributeHandle;

    /*
     * If |type| is |btDB_PRIMARY_SERVICE|, or
     * |btDB_SECONDARY_SERVICE|, this contains the start and end attribute
     * handles.
     */
    uint16_t usStartHandle;
    uint16_t usEndHandle;

    /*
     * If |type| is |btDB_CHARACTERISTIC|, this contains the properties of
     * the characteristic.
     */
    uint8_t ucProperties;
} BTGattDbElement_t;

/** GATT open callback invoked in response to open */
typedef void ( * BTConnectCallback_t)( uint16_t usConnId,
                                       BTGattStatus_t xStatus,
                                       uint8_t ucClientIf,
                                       BTBdaddr_t * pxBda );

/** Callback invoked in response to close */
typedef void ( * BTDisconnectCallback_t)( uint16_t usConnId,
                                          BTGattStatus_t xStatus,
                                          uint8_t ucClientIf,
                                          BTBdaddr_t * pxBda );


/** Callback triggered in response to readRemoteRssi */
typedef void ( * BTReadRemoteRssiCallback_t)( uint8_t ucClientIf,
                                              BTBdaddr_t * pxBda,
                                              uint32_t ulRssi,
                                              BTStatus_t xStatus );

/**
 * Callback notifying an application that a remote device connection is currently congested
 * and cannot receive any more data. An application should avoid sending more data until
 * a further callback is received indicating the congestion status has been cleared.
 */
typedef void ( * BTCongestionCallback_t)( uint16_t usConnId,
                                          bool bCongested );

#endif /* _BT_HAL_GATT_TYPES_H_ */
/** @} */
