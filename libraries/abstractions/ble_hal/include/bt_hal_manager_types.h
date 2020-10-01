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
 * @file bt_hal_manager_types.h
 * @addtogroup HAL_BLUETOOTH
 * @brief This BT HAL provides the interfaces to control the Bluetooth power states
 * ,local device control and device discovery functionalities
 * USAGE
 * -----
 *
 * @{
 */
#ifndef _BT_HAL_MANAGER_TYPES_H_
#define _BT_HAL_MANAGER_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Size in byte of 128bits UUID.
 */
#define bt128BIT_UUID_LEN    16

/**
 * @brief Size in byte of BT/BLE addresses.
 */
#define btADDRESS_LEN        6
#define btKEY_MAX_LEN        16

/**
 * @brief UUID types.
 */
typedef enum
{
    eBTuuidType16 = 1, /**< 16 bit UUID */
    eBTuuidType32 = 2, /**< 32 bit UUID */
    eBTuuidType128 = 3 /**< 128 bit UUID */
} BTuuidType_t;

/**
 * @brief BT/BLE address.
 */
typedef struct
{
    uint8_t ucAddress[ btADDRESS_LEN ]; /**< Address. */
} BTBdaddr_t;

/**
 * @brief Device Name.
 */
typedef struct
{
    uint8_t * ucName; /**< Device Name. */
} BTBdname_t;


/**
 * @brief UUID.
 */
typedef struct
{
    BTuuidType_t ucType; /**< UUID type */
    union
    {
        uint16_t uu16;                      /**< UUID, 16 bits. */
        uint32_t uu32;                      /**< UUID, 32 bits. */
        uint8_t uu128[ bt128BIT_UUID_LEN ]; /**< UUID, 128 bits. */
    } uu;
} BTUuid_t;

/**
 * @brief Bluetooth Error Status .
 */
typedef enum
{
    eBTStatusSuccess = 0,        /**< Success. */
    eBTStatusFail = 1,           /**< Generic fail. */
    eBTStatusNotReady = 2,       /**< Service/Driver/Peripheral not ready. */
    eBTStatusNoMem = 3,          /**< Failed to allocate memory. */
    eBTStatusBusy = 4,           /**< Service/Driver/Peripheral busy. */
    eBTStatusDone = 5,           /**< Request already completed. */
    eBTStatusUnsupported = 6,    /**< Unsupported request/call. */
    eBTStatusParamInvalid = 7,   /**< Invalid parameter. */
    eBTStatusUnHandled = 8,      /**< Unhandled */
    eBTStatusAuthFailure = 9,    /**< Authentication procedure failed. */
    eBTStatusRMTDevDown = 10,    /**< Remote Device Down */
    eBTStatusAuthRejected = 11,  /**< Authentication rejected. */
    eBTStatusWakeLockError = 12, /**< Wake lock error */
    eBTStatusLinkLoss = 13       /**< Link lost. */
} BTStatus_t;

typedef enum
{
    eProfileHeadset = 0,           /**< Headset and Handsfree profile. */
    eBTProfileA2DP = 1,            /**< A2DP Source profile. */
    eBTProfileAVRCPTarget = 2,     /**< AVRCP Target profile. */
    eBTProfileA2DPSink = 3,        /**< A2DP Sink profile. */
    eBTProfileAVRCPController = 4, /**< AVRCP Controller profile. */
    eBTProfileInputDevice = 5,     /**< Human Input Device (HID) Host profile. */
    eBTProfileHeadsetClient = 6,   /**< Headset Client (HFP-HF role) profile. */
    eBTProfileGATT = 7,            /**< GATT profile */
    eBTProfileSock = 8,            /**< Socket profile */
    /* TODO : Add other profiles? */
    eBTProfileMaxID = 9            /**< Max profile index */
} BTProfile_t;

/**
 * @brief Input Output device type.
 */
typedef enum
{
    eBTIONone = 0,            /**< No IO. */
    eBTIODisplayOnly = 1,     /**< No input, only display. */
    eBTIODisplayYesNo = 2,    /**< Display + yes/no input. */
    eBTIOKeyboardOnly = 3,    /**< Only input, keyboard. */
    eBTIOKeyboardDisplay = 4, /**< Keyboard and Display. */
} BTIOtypes_t;

#endif /* _BT_HAL_MANAGER_TYPES_H_ */
/** @} */
