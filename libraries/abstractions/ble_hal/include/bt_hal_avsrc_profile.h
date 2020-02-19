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
 * @file bt_avsrc_profile.h
 * @brief BT HAL provides the interfaces for A2DP Source profile for local device.
 * @addtogroup HAL_BLUETOOTH
 * USAGE
 * -----
 *
 * @{
 */


#ifndef _BT_HAL_AVSRC_PROFILE_H
#define _BT_HAL_AVSRC_PROFILE_H

#include <stdint.h>
#include "bt_hal_manager_types.h"

/** Bluetooth AV connection states */
typedef enum
{
    eBTAvsrcConnectionStateDisconnected = 0,
    eBTAvsrcConnectionStateConnecting,
    eBTAvsrcConnectionStateConnected,
    eBTAvsrcConnectionStateDisconnecting
} BTAvConnectionState_t;

/** Bluetooth AV data path states */
typedef enum
{
    eBTAvsrcAudioStateRemoteSuspend = 0,
    eBTAvsrcAudioStateStopped,
    eBTAvsrcAudioStateStarted,
} BTAvAudioState_t;

/** Callback invoked in to notifiy AV connection state change */
typedef void (* BTAvsrcConnectionStateCallback_t)( BTAvConnectionState_t xState,
                                                   BTBdaddr_t * pxBdAddr );

/** Callback invoked in to notifiy AV Audio state change */
typedef void (* BTAvsrcAudioStateCallback_t)( BTAvAudioState_t xState,
                                              BTBdaddr_t * pxBdAddr );

typedef struct
{
    size_t xSize;
    BTAvsrcConnectionStateCallback_t xConnStateCback;
    BTAvsrcAudioStateCallback_t xAudioStateCback;
} BTAvsrcCallbacks_t;

/** Represents the standard AV connection interface. */
typedef struct
{
    /** Set this to size of  BTAvsrcInterface_t*/
    size_t xSize;

    /**
     * @brief Initializes A2DP module.
     * @param[in] callbacks, BTAvsrcCallbacks_t callbacks.
     * @return  BTHAL_STATUS_SUCCESS if init is successful. Or any other error code
     */
    BTStatus_t ( * pxAvsrcInit )( BTAvsrcCallbacks_t * pxCallbacks );

    /**
     * @brief Cleans up A2DP module
     * @param None
     * @return  BT_STATUS_SUCCESS if cleanup is successful
     */
    BTStatus_t ( * pxAvsrcCleanup )();

    /**
     * @brief Starts connect for A2DP Source profile
     * @param[in] BTBdaddr_t :BT Address of remote device
     * @return BTHAL_STATUS_SUCCESS if the operation is successful, else error code.
     */
    BTStatus_t ( * pxAvsrcConnect )( BTBdaddr_t * pxRemote_addr );

    /**
     * @brief Starts disconnect for A2DP Source profile
     *
     * @param[in] BTBdaddr_t :BT Address of remote device
     * @return BTHAL_STATUS_SUCCESS if the operation is successful, else error code.
     */
    BTStatus_t ( * pxAvsrcDisconnect )( BTBdaddr_t * pxRemote_addr );
} BTAvsrcInterface_t;

extern const BTAvsrcInterface_t * BT_GetAvsrcInterface();

#endif /* _BT_HAL_AVSRC_PROFILE_H */
/** @} */
