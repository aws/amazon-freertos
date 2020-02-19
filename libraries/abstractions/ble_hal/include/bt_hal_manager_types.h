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
    eBTStatusSuccess,       /**< Success. */
    eBTStatusFail,          /**< Generic fail. */
    eBTStatusNotReady,      /**< Service/Driver/Peripheral not ready. */
    eBTStatusNoMem,         /**< Failed to allocate memory. */
    eBTStatusBusy,          /**< Service/Driver/Peripheral busy. */
    eBTStatusDone,          /**< Request already completed. */
    eBTStatusUnsupported,   /**< Unsupported request/call. */
    eBTStatusParamInvalid,  /**< Invalid parameter. */
    eBTStatusUnHandled,     /**< . */
    eBTStatusAuthFailure,   /**< Authentication procedure failed. */
    eBTStatusRMTDevDown,    /**< . */
    eBTStatusAuthRejected,  /**< Authentication rejected. */
    eBTStatusWakeLockError, /**< . */
    eBTStatusLinkLoss       /**< Link lost. */
} BTStatus_t;

typedef enum
{
    eProfileHeadset,           /**< Headset and Handsfree profile. */
    eBTProfileA2DP,            /**< A2DP Source profile. */
    eBTProfileAVRCPTarget,     /**< AVRCP Target profile. */
    eBTProfileA2DPSink,        /**< A2DP Sink profile. */
    eBTProfileAVRCPController, /**< AVRCP Controller profile. */
    eBTProfileInputDevice,     /**< Human Input Device (HID) Host profile. */
    eBTProfileHeadsetClient,   /**< Headset Client (HFP-HF role) profile. */
    eBTProfileGATT,
    eBTProfileSock,            /** Sockets */
    /* TODO : Add other profiles? */
    eBTProfileMaxID
} BTProfile_t;

/**
 * @brief Input Output device type.
 */
typedef enum
{
    eBTIONone,            /**< No IO. */
    eBTIODisplayOnly,     /**< No input, only display. */
    eBTIODisplayYesNo,    /**< Display + yes/no input. */
    eBTIOKeyboardOnly,    /**< Only input, keyboard. */
    eBTIOKeyboardDisplay, /**< Keyboard and Display. */
} BTIOtypes_t;

#endif /* _BT_HAL_MANAGER_TYPES_H_ */
/** @} */
