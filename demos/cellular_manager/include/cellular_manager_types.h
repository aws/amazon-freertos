/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

#ifndef __CELLULAR_MANAGER_TYPES_H__
#define __CELLULAR_MANAGER_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

#include "platform/iot_threads.h"
/* Iot Cellular HAL layer types. */
#include "cellular_types.h"


#define CELLULAR_MAX_SIM_RETRY    ( 5U )

/**
 * @brief Status code returns from APIs.
 */
typedef enum
{
    CELLULAR_MANAGER_SUCCESS = 0,    /**< The operation was successful. */
    CELLULAR_MANAGER_BAD_PARAMETER,  /**< One or more of the input parameters is not valid. */
    CELLULAR_MANAGER_NO_MEMORY,      /**< Memory allocation failure. */
    CELLULAR_MANAGER_TIMEOUT,        /**< The operation timed out. */
    CELLULAR_MANAGER_HAL_ERROR,      /**< Error in the HAL layer. */
    CELLULAR_MANAGER_LIB_NOT_OPENED, /**< Cellular manager is not init properly. */
    CELLULAR_MANAGER_UNSUPPORTED,    /**< The operation is not supported. */
    CELLULAR_MANAGER_UNKNOWN         /**< Any other error other than the above mentioned ones. */
} CellularManagerError_t;

/**
 * @brief Represents the state of CELLULAR Connection.
 */
typedef enum
{
    CELLULAR_CONNECTION_OFF,                                /**< The module is powered off. */
    CELLULAR_CONNECTION_REBOOTED,                           /**< The module is rebooted due to manual reboot or crash. */
    CELLULAR_CONNECTION_REGISTERED,                         /**< The module is attached to the network. */
    CELLULAR_CONNECTION_RF_OFF,                             /**< The module is in RF off state. */
    CELLULAR_CONNECTION_CONNECTED,                          /**< The module is connected with data */
    CELLULAR_CONNECTION_DISCONNECTED,                       /**< The module is not connected. */
    CELLULAR_CONNECTION_DISCONNECTED_NO_SIM,                /**< The module is not connected because of no sim. */
    CELLULAR_CONNECTION_DISCONNECTED_REGISTRATION_REJECTED, /**< The module is not connected because registration attempt was rejected. */
    CELLULAR_CONNECTION_DISCONNECTED_FOTA_STARTED,          /**< The module is not connected because FOTA is in progress. */
    CELLULAR_CONNECTION_DISCONNECTED_FOTA_IN_PROGRESS,      /**< The module is not connected because FOTA is in progress. */
    CELLULAR_CONNECTION_DISCONNECTED_FOTA_FINISHED,         /**< The module is not connected because FOTA is in progress. */
    CELLULAR_CONNECTION_UNKNOWN                             /**< any other connection state . */
} CellularManagerConnectionState_t;

/**
 * @states and events for cellular manager states tracking
 */
typedef enum
{
    CELLULAR_SM_OFF = 0,    /**< SateMachine: CELLULAR module is off */
    CELLULAR_SM_ON,         /**< SateMachine: CELLULAR module is on */
    CELLULAR_SM_CONNECTED,  /**< SateMachine: CELLULAR module is connected */
    CELLULAR_SM_REGISTERED, /**< SateMachine: CELLULAR module is registered */
    CELLULAR_SM_RFOFF,      /**< SateMachine: CELLULAR module is rfoff */
    CELLULAR_SM_OTA,        /**< SateMachine: CELLULAR module is OTA mode */
    CELLULAR_SM_UNKNOWN     /**< SateMachine: Unknown state */
} cellularSmState_t;

typedef enum
{
    CELLULAR_EVENT_PWR_ON = 0,    /**< trigger: an module is powered on */
    CELLULAR_EVENT_PWR_OFF,       /**< trigger: cellular module is powered off */
    CELLULAR_EVENT_RF_ON,         /**< trigger: airplane mode off */
    CELLULAR_EVENT_RF_OFF,        /**< trigger: airplane mode on */
    CELLULAR_EVENT_ATTACHED,      /**< trigger: attached to a network */
    CELLULAR_EVENT_DETACHED,      /**< trigger: detached from a network */
    CELLULAR_EVENT_DATA_ACTIVE,   /**< trigger: data connect is active */
    CELLULAR_EVENT_DATA_INACTIVE, /**< trigger: data connection is inactive */
    CELLULAR_EVENT_OTA,           /**< trigger: OTA starts */
    CELLULAR_EVENT_OTA_DONE,      /**< trigger: OTA finishes */
    CELLULAR_NUM_EVENT
} cellularSmEvent_t;

typedef struct enumStringMapStruct
{
    const char * str;
    uint32_t num;
} _enumStringMap_t;

/**
 * @brief Callback to be invoked whenever the connection state changes.
 *
 * @param[in] pUserData User data as provided while registering callback.
 * @param[in] connectionState The new state of the CELLULAR connection.
 */
typedef void ( * CellularManagerConnectionStateChangedCallback_t )( void * pUserData,
                                                                    CellularManagerConnectionState_t connectionState );

/**
 * @brief Callback to be invoked whenever signal strength changes.
 *
 * @param[in] pUserData User data as provided while registering callback.
 * @param[in] pSignalInfo The new signal information.
 */
typedef void ( * CellularManagerSignalStrengthChangedCallback_t )( void * pUserData,
                                                                   const CellularSignalInfo_t * pSignalInfo );

typedef struct _cellularMgrCallbackEventsStruct
{
    CellularManagerConnectionStateChangedCallback_t connectionStateChangedCallback;
    void * connectionStateChangedCallbackData;
    CellularManagerSignalStrengthChangedCallback_t signalStrengthChangedCallback;
    void * signalStrengthChangedCallbackData;
} _cellularMgrCallbackEvents_t;

typedef struct CellularManagerContextStruct
{
    CellularHandle_t cellularHandle;
    IotMutex_t cellularSmMutex;
    bool cellularSmMutexCreated;
    bool libOpened;
    bool isFirstTimeBootUp;
    cellularSmState_t cellularSmState;
    _cellularMgrCallbackEvents_t cellularMgrCallbackEvents;
} CellularManagerContext_t;

#endif /* __CELLULAR_MANAGER_TYPES_H__ */
