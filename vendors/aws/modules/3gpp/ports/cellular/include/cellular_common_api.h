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

#ifndef __CELLULAR_COMMON_API_H__
#define __CELLULAR_COMMON_API_H__

#include "cellular_types.h"
#include "cellular_comm_interface.h"

/*-----------------------------------------------------------*/

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_Init in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonInit( CellularHandle_t * pCellularHandle,
                                     const CellularCommInterface_t * pCommInterface );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_Cleanup in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonCleanup( CellularHandle_t cellularHandle );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RegisterUrcNetworkRegistrationEventCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRegisterUrcNetworkRegistrationEventCallback( CellularHandle_t cellularHandle,
                                                                            CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback,
                                                                            void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RegisterUrcPdnEventCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRegisterUrcPdnEventCallback( CellularHandle_t cellularHandle,
                                                            CellularUrcPdnEventCallback_t pdnEventCallback,
                                                            void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RegisterUrcSignalStrengthChangedCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRegisterUrcSignalStrengthChangedCallback( CellularHandle_t cellularHandle,
                                                                         CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback,
                                                                         void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RegisterUrcGenericCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRegisterUrcGenericCallback( CellularHandle_t cellularHandle,
                                                           CellularUrcGenericCallback_t genericCallback,
                                                           void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RegisterModemEventCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRegisterModemEventCallback( CellularHandle_t cellularHandle,
                                                           CellularModemEventCallback_t modemEventCallback,
                                                           void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_ATCommandRaw in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonATCommandRaw( CellularHandle_t cellularHandle,
                                             const char * pATCommandPrefix,
                                             const char * pATCommandPayload,
                                             CellularATCommandType_t atCommandType,
                                             CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                             void * pData,
                                             uint16_t dataLen );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_CreateSocket in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonCreateSocket( CellularHandle_t cellularHandle,
                                             uint8_t pdnContextId,
                                             CellularSocketDomain_t socketDomain,
                                             CellularSocketType_t socketType,
                                             CellularSocketProtocol_t socketProtocol,
                                             CellularSocketHandle_t * pSocketHandle );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_SocketSetSockOpt in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSocketSetSockOpt( CellularHandle_t cellularHandle,
                                                 CellularSocketHandle_t socketHandle,
                                                 CellularSocketOptionLevel_t optionLevel,
                                                 CellularSocketOption_t option,
                                                 const uint8_t * pOptionValue,
                                                 uint32_t optionValueLength );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_SocketRegisterDataReadyCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSocketRegisterDataReadyCallback( CellularHandle_t cellularHandle,
                                                                CellularSocketHandle_t socketHandle,
                                                                CellularSocketDataReadyCallback_t dataReadyCallback,
                                                                void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_SocketRegisterSocketOpenCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSocketRegisterSocketOpenCallback( CellularHandle_t cellularHandle,
                                                                 CellularSocketHandle_t socketHandle,
                                                                 CellularSocketOpenCallback_t socketOpenCallback,
                                                                 void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_SocketRegisterClosedCallback in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSocketRegisterClosedCallback( CellularHandle_t cellularHandle,
                                                             CellularSocketHandle_t socketHandle,
                                                             CellularSocketClosedCallback_t closedCallback,
                                                             void * pCallbackContext );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RfOn in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRfOn( CellularHandle_t cellularHandle );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_RfOff in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonRfOff( CellularHandle_t cellularHandle );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetIPAddress in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetIPAddress( CellularHandle_t cellularHandle,
                                             uint8_t contextId,
                                             char * pBuffer,
                                             uint32_t bufferLength );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetModemInfo in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetModemInfo( CellularHandle_t cellularHandle,
                                             CellularModemInfo_t * pModemInfo );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetEidrxSettings in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetEidrxSettings( CellularHandle_t cellularHandle,
                                                 CellularEidrxSettingsList_t * pEidrxSettingsList );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_SetEidrxSettings in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSetEidrxSettings( CellularHandle_t cellularHandle,
                                                 const CellularEidrxSettings_t * pEidrxSettings );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetRegisteredNetwork in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetRegisteredNetwork( CellularHandle_t cellularHandle,
                                                     CellularPlmnInfo_t * pNetworkInfo );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetNetworkTime in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetNetworkTime( CellularHandle_t cellularHandle,
                                               CellularTime_t * pNetworkTime );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetServiceStatus in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetServiceStatus( CellularHandle_t cellularHandle,
                                                 CellularServiceStatus_t * pServiceStatus );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetServiceStatus in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSetPdnConfig( CellularHandle_t cellularHandle,
                                             uint8_t contextId,
                                             const CellularPdnConfig_t * pPdnConfig );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetServiceStatus in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetPsmSettings( CellularHandle_t cellularHandle,
                                               CellularPsmSettings_t * pPsmSettings );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetServiceStatus in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonSetPsmSettings( CellularHandle_t cellularHandle,
                                               const CellularPsmSettings_t * pPsmSettings );

/**
 * @brief This function is the implementationof cellular HAL API.
 * Reference Cellular_GetServiceStatus in cellular_api.h for definition.
 */
CellularError_t Cellular_CommonGetSimCardInfo( CellularHandle_t cellularHandle,
                                               CellularSimCardInfo_t * pSimCardInfo );

/**
 * @brief Get SIM card lock status.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pSimCardStatus Out parameter to provide the SIM card status.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_CommonGetSimCardLockStatus( CellularHandle_t cellularHandle,
                                                     CellularSimCardStatus_t * pSimCardStatus );

/**
 * @brief 3GPP URC AT+CEREG handler for cellular HAL common.
 *
 * This function handles the incoming URC and callback function.
 *
 * @param[in/out] pContext cellular HAL context created in Cellular_Init.
 * @param[in] pInputLine the input URC string.
 *
 */
void Cellular_CommonUrcProcessCereg( CellularContext_t * pContext,
                                     char * pInputLine );

/**
 * @brief 3GPP URC AT+CGREG handler for cellular HAL common.
 *
 * This function handles the incoming URC and callback function.
 *
 * @param[in/out] pContext cellular HAL context created in Cellular_Init.
 * @param[in] pInputLine the input URC string.
 *
 */
void Cellular_CommonUrcProcessCgreg( CellularContext_t * pContext,
                                     char * pInputLine );

/**
 * @brief 3GPP URC AT+CREG handler for cellular HAL common.
 *
 * This function handles the incoming URC and callback function.
 *
 * @param[in/out] pContext cellular HAL context created in Cellular_Init.
 * @param[in] pInputLine the input URC string.
 *
 */
void Cellular_CommonUrcProcessCreg( CellularContext_t * pContext,
                                    char * pInputLine );

#endif /* __CELLULAR_COMMON_API_H__ */
