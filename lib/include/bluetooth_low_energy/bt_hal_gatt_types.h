/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * USAGE
 * -----
 *
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
    eBTWriteTypePrepare = 0x0003,   /**< Prepared Write request */
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

typedef struct
{
    uint16_t usId;
    BTUuid_t xUuid;
    BTDbAttributeType_t xType;
    uint16_t usAttributeHandle;
    BTCharPermissions_t xPermissions;

    /*
     * If type is eBTDbPrimaryService, or
     * eBTDbSecondaryService, this contains the start and end attribute
     * handles.
     */
    uint16_t usStartHandle;
    uint16_t usEndHandle;

    /*
     * If type is eBTDbCharacteristic, this contains the properties of
     * the characteristic.
     */
    BTCharProperties_t xProperties;

    /*
     * If type is eBTDbIncludedService, this contains the handle of the included service.
     */
	uint16_t usIncludedServiceHandle;

} BTGattDbElement_t;


#endif /* _BT_HAL_GATT_TYPES_H_ */
