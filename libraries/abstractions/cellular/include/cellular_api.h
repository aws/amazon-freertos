/*
 * Amazon FreeRTOS Cellular Preview Release
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

#ifndef __CELLULAR_API_H__
#define __CELLULAR_API_H__

/* IoT Cellular data types. */
#include "cellular_types.h"

/* Hardware interface. */
#include "cellular_comm_interface.h"

/**
 * @brief One time initialization function.
 *
 * This function initializes and returns the supplied context. It must be called
 * once (and only once) before calling any other function of this library.
 *
 * @param[in/out] pCellularHandle the handle pointer to store the cellular handle.
 * @param[in] pCommInterface Comm interface for communicating with the module.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_Init( CellularHandle_t * pCellularHandle,
                               const CellularCommInterface_t * pCommInterface );

/**
 * @brief One time deinitialization function.
 *
 * This function frees resources taken in Cellular_Init. After this function
 * returns, Cellular_Init must be called again before calling any other function
 * of this library.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_Cleanup( CellularHandle_t cellularHandle );

/**
 * @brief Configure the priority order in which the networks are searched.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pRatPriorities An array of RATs in priority order. For example, to
 * search NB-IoT first, then Cat-M1 and then LTE, the array should contain:
 * { CELLULAR_RAT_NBIOT, CELLULAR_RAT_CATM1, CELLULAR_RAT_LTE }.
 * @param[in] ratPrioritiesLength Length of the pRatPriorities array.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SetRatPriority( CellularHandle_t cellularHandle,
                                         const CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength );

/**
 * @brief Get the priority order in which the networks are searched.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pRatPriorities An array of current RATs in priority order.
 * @param[in] ratPrioritiesLength Length of the pRatPriorities array.
 * @param[out] pOutputRatPrioritiesLength Actual number of items filled into pRatPriorities
 * by this function when it returns.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetRatPriority( CellularHandle_t cellularHandle,
                                         CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength,
                                         uint8_t * pOutputRatPrioritiesLength );

/**
 * @brief Turn on RF i.e. turn-off airplane mode.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RfOn( CellularHandle_t cellularHandle );

/**
 * @brief Turn off RF i.e. turn-on airplane mode.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RfOff( CellularHandle_t cellularHandle );

/**
 * @brief Get SIM card status (activated/Pin set etc.).
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pSimCardStatus Out parameter to provide the SIM card status.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetSimCardStatus( CellularHandle_t cellularHandle,
                                           CellularSimCardStatus_t * pSimCardStatus );

/**
 * @brief Get SIM card information (IMSI, SIM Card number etc.).
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pSimCardInfo Out parameter to provide the SIM card information.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetSimCardInfo( CellularHandle_t cellularHandle,
                                         CellularSimCardInfo_t * pSimCardInfo );

/**
 * @brief Get the information about the modem (HW version, FW version etc.).
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pModemInfo Out parameter to provide the modem information.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetModemInfo( CellularHandle_t cellularHandle,
                                       CellularModemInfo_t * pModemInfo );

/**
 * @brief Get the network on which SIM is currently registered/camped.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pNetworkInfo Out parameter to provide the network information.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetRegisteredNetwork( CellularHandle_t cellularHandle,
                                               CellularPlmnInfo_t * pNetworkInfo );

/**
 * @brief Get the network time.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pNetworkTime Out parameter to provide the network time.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetNetworkTime( CellularHandle_t cellularHandle,
                                         CellularTime_t * pNetworkTime );

/**
 * @brief Get signal information.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pSignalInfo Out parameter to provide the signal information.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetSignalInfo( CellularHandle_t cellularHandle,
                                        CellularSignalInfo_t * pSignalInfo );

/**
 * @brief Get network service status.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pServiceStatus Out parameter to provide the service status.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetServiceStatus( CellularHandle_t cellularHandle,
                                           CellularServiceStatus_t * pServiceStatus );

/**
 * @brief Set PDN config for a PDN context.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Context ID of the PDN to set the config for. Context ID value
 * should be in range between CELLULAR_PDN_CONTEXT_ID_MIN and CELLULAR_PDN_CONTEXT_ID_MAX.
 * The context ID with specified PDN config will be used with other CELLULAR APIs.
 * @param[in] pPdnConfig PDN config to set.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SetPdnConfig( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t * pPdnConfig );

/**
 * @brief Get status reports for all PDN contexts.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pPdnStatus Out parameter to provide array of PDN contexts.
 * @param[in] numStatusBuffers Number of CellularPdnStatus_t buffers in the
 * provided array pPdnStatusBuffers.
 * @param[out] pNumStatus Out parameter to provide the number of PDN statuses
 * returned.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetPdnStatus( CellularHandle_t cellularHandle,
                                       CellularPdnStatus_t * pPdnStatusBuffers,
                                       uint8_t numStatusBuffers,
                                       uint8_t * pNumStatus );

/**
 * @brief Activate a PDN context.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Context ID of the PDN context to activate. The same context ID
 * should be used with Cellular_SetPdnConfig before calling this API.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_ActivatePdn( CellularHandle_t cellularHandle,
                                      uint8_t contextId );

/**
 * @brief Deactivate a PDN context.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Context ID of the PDN context to deactivate.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_DeactivatePdn( CellularHandle_t cellularHandle,
                                        uint8_t contextId );

/**
 * @brief Get the IP Address assigned to the module.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Context ID of the PDN context for which IP address is requested.
 * @param[out] pBuffer the buffer to receive the IP address into. The returned pBuffer
 * is a NULL terminated string.
 * @param[in] Length of the buffer pBuffer.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetIPAddress( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       char * pBuffer,
                                       uint32_t bufferLength );

/**
 * @brief Set the DNS server to use.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Context ID of the PDN context for which DNS needs to be set.
 * @param[in] pDnsServerAddress The address of the DNS server to set.
 * It should be a NULL terminated string.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SetDns( CellularHandle_t cellularHandle,
                                 uint8_t contextId,
                                 const char * pDnsServerAddress );

/**
 * @brief Register/Remove callback for Network Registration URC events.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] networkRegistrationCallback The callback to register. Set to NULL
 * to remove the existing callback.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RegisterUrcNetworkRegistrationEventCallback( CellularHandle_t cellularHandle,
                                                                      CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback,
                                                                      void * pCallbackContext );

/**
 * @brief Register/Remove callback for PDN related URC events.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pdnEventCallback The callback to register. Set to NULL remove the
 * existing callback.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RegisterUrcPdnEventCallback( CellularHandle_t cellularHandle,
                                                      CellularUrcPdnEventCallback_t pdnEventCallback,
                                                      void * pCallbackContext );

/**
 * @brief Register callback for Signal Strength changed URC events.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] signalStrengthChangedCallback The callback to register. Set to
 * NULL to remove the existing callback.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RegisterUrcSignalStrengthChangedCallback( CellularHandle_t cellularHandle,
                                                                   CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback,
                                                                   void * pCallbackContext );

/**
 * @brief Register callback for all modem related events.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] modemEventCallback The callback to register. Set to NULL to remove
 * the existing callback.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RegisterModemEventCallback( CellularHandle_t cellularHandle,
                                                     CellularModemEventCallback_t modemEventCallback,
                                                     void * pCallbackContext );

/**
 * @brief Register generic callback for all other URC events than covered in
 * above callbacks.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] genericCallback The callback to register. Set to NULL to remove
 * the existing callback.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_RegisterUrcGenericCallback( CellularHandle_t cellularHandle,
                                                     CellularUrcGenericCallback_t genericCallback,
                                                     void * pCallbackContext );

/**
 * @brief Get current PSM settings.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pPsmSettings Out parameter to provide the PSM settings.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetPsmSettings( CellularHandle_t cellularHandle,
                                         CellularPsmSettings_t * pPsmSettings );

/**
 * @brief Set PSM settings.
 *
 * Enable/disable PSM by using CellularPsmSettings_t structure.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pPsmSettings PSM settings to set.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SetPsmSettings( CellularHandle_t cellularHandle,
                                         const CellularPsmSettings_t * pPsmSettings );

/**
 * @brief Get current e-I-DRX settings.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pEidrxSettingsList Out parameter to provide the e-I-DRX settings.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetEidrxSettings( CellularHandle_t cellularHandle,
                                           CellularEidrxSettingsList_t * pEidrxSettingsList );

/**
 * @brief Set e-I-DRX settings.
 *
 * This API can be used to enable/disable e-I-DRX by using the mode member of
 * CellularEidrxSettings_t.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pEidrxSettings e-I-DRX settings to set.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SetEidrxSettings( CellularHandle_t cellularHandle,
                                           const CellularEidrxSettings_t * pEidrxSettings );

/**
 * @brief Send the raw AT command to the module.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pATCommandPrefix The AT command response prefix. NULL if the response
 * has no prefix.
 * @param[in] pATCommandPayload The AT command to send. It should be a NULL terminated
 * string.
 * @param[in] atCommandType Type of AT command.
 * @param[in] responseReceivedCallback Callback to be invoked when a response for the
 * command is received.
 * @param[in] pData The pData pointer will be passed in responseReceivedCallback.
 * @param[in] dataLen The dataLen value will be passed in responseReceivedCallback.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_ATCommandRaw( CellularHandle_t cellularHandle,
                                       const char * pATCommandPrefix,
                                       const char * pATCommandPayload,
                                       CellularATCommandType_t atCommandType,
                                       CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                       void * pData,
                                       uint16_t dataLen );

/**
 * @brief Create a socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pdn context id on which this socket needs to be created. The pdn
 * context must be previously activated by Cellular_ActivatePdn function.
 * @param[in] socketDomain Socket domain.
 * @param[in] socketType Socket Type.
 * @param[in] socketProtocol Socket Protocol.
 * @param[out] pSocketHandle Out parameter to provide the created handle.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_CreateSocket( CellularHandle_t cellularHandle,
                                       uint8_t pdnContextId,
                                       CellularSocketDomain_t socketDomain,
                                       CellularSocketType_t socketType,
                                       CellularSocketProtocol_t socketProtocol,
                                       CellularSocketHandle_t * pSocketHandle );

/**
 * @brief Connect to a remote socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[in] dataAccessMode Data access mode of the socket. Buffer, Direct Push or Transparent.
 * @param[in] pRemoteSocketAddress Address (IP and Port) of the remote socket to connect to.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketConnect( CellularHandle_t cellularHandle,
                                        CellularSocketHandle_t socketHandle,
                                        CellularSocketAccessMode_t dataAccessMode,
                                        const CellularSocketAddress_t * pRemoteSocketAddress );

/**
 * @brief Send data to the connected remote socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[in] pData The buffer containing that data to be sent.
 * @param[in] dataLength Length of the data in the pData buffer.
 * @param[out] pSentDataLength Out parameter to provide the length of the actual
 * data sent. Note that it may be less than the dataLength in case complete data
 * could not be sent.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketSend( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     const uint8_t * pData,
                                     uint32_t dataLength,
                                     uint32_t * pSentDataLength );

/**
 * @brief Receive data on a connected socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[out] pBuffer The buffer to receive the data into.
 * @param[in] bufferLength Length of the buffer pBuffer.
 * @param[out] pReceivedDataLength Out parameter to provide the of the actual
 * data received in the buffer pBuffer. Note that it may be less than
 * bufferLength.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketRecv( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     uint8_t * pBuffer,
                                     uint32_t bufferLength,
                                     uint32_t * pReceivedDataLength );

/**
 * @brief Close the socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketClose( CellularHandle_t cellularHandle,
                                      CellularSocketHandle_t socketHandle );

/**
 * @brief Resolve a host name using Domain Name Service.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Context ID of the PDN context for which DNS is set.
 * @param[in] pcHostName The host name to resolve.
 * It should be a NULL terminated string.
 * The length of the string should not exceed RFC1035 defined.
 * @param[out] pResolvedAddress The output parameter to return the resolved
 * address. The length of this buffer should be at least CELLULAR_IP_ADDRESS_MAX_SIZE.
 * The returned buffer is a NULL terminated string.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_GetHostByName( CellularHandle_t cellularHandle,
                                        uint8_t contextId,
                                        const char * pcHostName,
                                        char * pResolvedAddress );

/**
 * @brief Set options for a socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[in] optionLevel Socket option level (IP or TCP/UDP).
 * @param[in] option Socket option to set.
 * @param[in] pOptionValue Buffer containing the socket option value.
 * @param[in] optionValueLength Length of the value pointed to by pOptionValue.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketSetSockOpt( CellularHandle_t cellularHandle,
                                           CellularSocketHandle_t socketHandle,
                                           CellularSocketOptionLevel_t optionLevel,
                                           CellularSocketOption_t option,
                                           const uint8_t * pOptionValue,
                                           uint32_t optionValueLength );

/**
 * @brief Register Socket open callback on the socket.
 *
 * This callback is invoked when modem confirms that a socket is opened.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[in] socketOpenCallback The callback to register.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketRegisterSocketOpenCallback( CellularHandle_t cellularHandle,
                                                           CellularSocketHandle_t socketHandle,
                                                           CellularSocketOpenCallback_t socketOpenCallback,
                                                           void * pCallbackContext );

/**
 * @brief Register data ready callback on the socket.
 *
 * This callback is invoked whenever data is ready to be read from the socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[in] dataReadyCallback The callback to register.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketRegisterDataReadyCallback( CellularHandle_t cellularHandle,
                                                          CellularSocketHandle_t socketHandle,
                                                          CellularSocketDataReadyCallback_t dataReadyCallback,
                                                          void * pCallbackContext );

/**
 * @brief Register closed callback on the socket.
 *
 * This callback is invoked whenever remote end closes the connection on a
 * connected socket.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 * @param[in] closedCallback The callback to register.
 * @param[in] pCallbackContext The context to be passed to callback function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t Cellular_SocketRegisterClosedCallback( CellularHandle_t cellularHandle,
                                                       CellularSocketHandle_t socketHandle,
                                                       CellularSocketClosedCallback_t closedCallback,
                                                       void * pCallbackContext );

#endif /* __CELLULAR_API_H__ */
