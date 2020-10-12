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

#ifndef __CELLULAR_MANAGER_H__
#define __CELLULAR_MANAGER_H__

/* Iot CELLULAR Manager types. */
#include "cellular_manager_types.h"
#include "cellular_comm_interface.h"

/**
 * @brief One time initialization function.
 *
 * This function initializes the Cellular Manager context. <b>It must be called
 * once (and only once) before calling any other function of this library.</b>
 *
 * @param[in] The cellular manager context to initialize.
 * @param[in] pCommInterface Comm interface for communicating with the module.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_Init( CellularManagerContext_t ** pCellularManagerContext,
                                             const CellularCommInterface_t * pCommInterface );

/**
 * @brief One time de initialization function.
 *
 * This function frees resources taken in CellularManager_Init. After this
 * function returns, CellularManager_Init must be called again before calling
 * any other function of this library.
 *
 * @param[in] The cellular manager context to de-initialize.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_Cleanup( CellularManagerContext_t * pCellularManagerContext );

/**
 * @brief Establish CELLULAR connection synchronously.
 *
 * This is a blocking call and returns only when connection is established or
 * an error (including timeout) occurs.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 * @param[in] contextId Context ID of the PDN to set the config for. Context ID value.
 * @param[in] pPdnConfig PDN config to set.
 * @param[in] timeoutMilliseconds Time in milliseconds after which the connect
 * operation should timeout.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_Connect( CellularManagerContext_t * pCellularManagerContext,
                                                uint8_t contextId,
                                                const CellularPdnConfig_t * pPdnConfig,
                                                uint32_t timeoutMilliseconds );

/**
 * @brief Disconnect the WAN connection.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 * @param[in] contextId Context ID of the PDN to set the config for. Context ID value.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_Disconnect( CellularManagerContext_t * pCellularManagerContext,
                                                   uint8_t contextId );

/**
 * @brief Get the CELLULAR connection state.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 * @param[out] pConnectionState The output parameter to provide the state of the
 * connection.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_GetConnectionState( CellularManagerContext_t * pCellularManagerContext,
                                                           CellularManagerConnectionState_t * pConnectionState );

/**
 * @brief Turn on the airplane mode.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_AirplaneModeOn( CellularManagerContext_t * pCellularManagerContext );

/**
 * @brief Turn off the airplane mode.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_AirplaneModeOff( CellularManagerContext_t * pCellularManagerContext );

/**
 * @brief Set the callback to be invoked whenever connection state changes.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 * @param[in] pUserData Userdata to be provided in the callback.
 * @param[in] connectionStateChangedCallback The callback to be invoked. Set to
 * NULL to remove the previously registered callback.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_SetConnectionStateChangedCallback( CellularManagerContext_t * pCellularManagerContext,
                                                                          void * pUserData,
                                                                          CellularManagerConnectionStateChangedCallback_t connectionStateChangedCallback );

/**
 * @brief Set the callback to be invoked whenever signal strength changes.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 * @param[in] pUserData Userdata to be provided in the callback.
 * @param[in] signalStrengthChangedCallback The callback to be invoked. Set to
 * NULL to remove the previously registered callback.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_SetSignalStrengthChangedCallback( CellularManagerContext_t * pCellularManagerContext,
                                                                         void * pUserData,
                                                                         CellularManagerSignalStrengthChangedCallback_t signalStrengthChangedCallback );

/**
 * @brief Return the cellular handle init by cellular manager.
 *
 * @param[in] pCellularManagerContext The cellular manager context.
 * @param[in/out] pCellularHandle The pointer to store the cellular handle.
 *
 * @return CELLULAR_MANAGER_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularManagerError_t CellularManager_GetCellularHandle( const CellularManagerContext_t * pCellularManagerContext,
                                                          CellularHandle_t * pCellularHandle );

#endif /* __CELLULAR_MANAGER_H__ */
