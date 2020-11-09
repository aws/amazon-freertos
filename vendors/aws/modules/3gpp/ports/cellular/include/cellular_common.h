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

#ifndef __CELLULAR_COMMON_H__
#define __CELLULAR_COMMON_H__

#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "event_groups.h"
#include "queue.h"

/* Cellular includes. */
#include "cellular_comm_interface.h"
#include "cellular_types.h"
#include "cellular_at_core.h"

/*-----------------------------------------------------------*/

/**
 * @brief The AT command request structure.
 */
typedef struct CellularAtReq
{
    const char * pAtCmd;
    CellularATCommandType_t atCmdType;
    const char * pAtRspPrefix;
    CellularATCommandResponseReceivedCallback_t respCallback;
    void * pData;
    uint32_t dataLen;
} CellularAtReq_t;

/**
 * @brief The data command request structure.
 */
typedef struct CellularAtDataReq
{
    const uint8_t * pData;       /* Data to send. */
    uint32_t dataLen;            /* Data length to send. */
    uint32_t * pSentDataLength;  /* Data actually sent. */
    const uint8_t * pEndPattern; /* End pattern after pData is sent completely.
                                  * Set NULL if not required. Cellular modem uses
                                  * end pattern instead of length in AT command
                                  * can make use of this variable. */
    uint32_t endPatternLen;      /* End pattern length. */
} CellularAtDataReq_t;

/**
 * @brief URC handler function.
 */
typedef void ( * CellularAtParseTokenHandler_t )( CellularContext_t * pContext,
                                                  char * pInputStr );

/**
 * @brief the URC token and URC handler mapping structure used by pkthanlder.
 */
typedef struct CellularAtParseTokenMap
{
    const char * pStrValue;
    CellularAtParseTokenHandler_t parserFunc;
} CellularAtParseTokenMap_t;

/**
 * @brief enum representing different Socket State.
 */
typedef enum CellularSocketState
{
    SOCKETSTATE_ALLOCATED = 0, /* Socket is created. */
    SOCKETSTATE_CONNECTING,    /* Socket is connecting in progress with remote peer. */
    SOCKETSTATE_CONNECTED,     /* Socket is connected. */
    SOCKETSTATE_DISCONNECTED   /* Socket is disconnected by remote peer or due to network error. */
} CellularSocketState_t;

/**
 * @brief Parameters involved in sending/receiving data through sockets.
 */
typedef struct CellularSocketContext
{
    uint8_t contextId;                       /* PDN context ID on which this socket exists. */
    uint32_t socketId;                       /* Socket ID of this socket. */
    CellularSocketState_t socketState;       /* State of the socket, Allocated, Free etc. */
    CellularSocketType_t socketType;         /* Type of socket, DGRAM or STREAM. */
    CellularSocketDomain_t socketDomain;     /* Socket domain, IPV4 or V6. */
    CellularSocketProtocol_t socketProtocol; /* Socket transport protocol, TCP or UDP. */
    CellularIPAddress_t localIpAddress;      /* IP address assigned to the device. */
    uint16_t localPort;                      /* Local Port. */
    CellularSocketAccessMode_t dataMode;     /* Data Access mode enabled for this socket. */

    /* Set using socket options. */
    uint32_t sendTimeoutMs; /* Send timeout value in milliseconds. */
    uint32_t recvTimeoutMs; /* Receive timeout value in milliseconds. */

    /* Set during socket connect. */
    CellularSocketAddress_t remoteSocketAddress; /* Remote IP address and port. */

    /* Callback functions. */
    CellularSocketDataReadyCallback_t dataReadyCallback; /* Informs data on this socket. */
    void * pDataReadyCallbackContext;                    /* Data ready callback context. */
    CellularSocketOpenCallback_t openCallback;           /* Informs the socket open status. */
    void * pOpenCallbackContext;                         /* socket open callback context. */
    CellularSocketClosedCallback_t closedCallback;       /* Informs the socket is closed. */
    void * pClosedCallbackContext;                       /* socket closed callback context. */

    /* Modem data. */
    void * pModemData; /* Modem specific data. */
} CellularSocketContext_t;

/**
 * @brief Parameters to setup pktio and pkthandler token tables.
 */
typedef struct CellularTokenTable
{
    /* URC handler mapping table. */
    CellularAtParseTokenMap_t * pCellularUrcHandlerTable;
    uint32_t cellularPrefixToParserMapSize;

    /* Table to decide AT command respone status. */
    const char ** pCellularSrcTokenErrorTable;
    uint32_t cellularSrcTokenErrorTableSize;
    const char ** pCellularSrcTokenSuccessTable;
    uint32_t cellularSrcTokenSuccessTableSize;

    /* Table to URC with prefix Token. */
    const char ** pCellularUrcTokenWoPrefixTable;
    uint32_t cellularUrcTokenWoPrefixTableSize;

    /* Extra success token for specific AT command. */
    const char ** pCellularSrcExtraTokenSuccessTable;
    uint32_t cellularSrcExtraTokenSuccessTableSize;
} CellularTokenTable_t;

/**
 * @brief Callback used to inform pktio the data start and the length of the data.
 *
 * @param[in] pCallbackContext The pCallbackContext in _Cellular_TimeoutAtcmdDataRecvRequestWithCallback.
 * @param[in] pLine The input line form cellular modem.
 * @param[in] lineLength The length of the input line from cellular modem.
 * @param[out] pData Is the start of of data in pLine.
 * @param[out] pDataLength Is the data length.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful.
 * CELLULAR_PKT_STATUS_SIZE_MISMATCH if more data is required.
 * Otherwise an error code indicating the cause of the error.
 */
typedef CellularPktStatus_t ( * CellularATCommandDataPrefixCallback_t ) ( void * pCallbackContext,
                                                                          char * pLine,
                                                                          uint32_t lineLength,
                                                                          char ** pDataStart,
                                                                          uint32_t * pDataLength );

/**
 * @brief Callback used to fix the stream before the send data.
 *
 * @param[in] pCallbackContext The pCallbackContext in CellularATCommandDataSendPrefixCallback_t.
 * @param[in/out] pLine The input line form cellular modem.
 * @param[in/out] pBytesRead The length of the input line from cellular modem.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
typedef CellularPktStatus_t ( * CellularATCommandDataSendPrefixCallback_t ) ( void * pCallbackContext,
                                                                              char * pLine,
                                                                              uint32_t * pBytesRead );

/*-----------------------------------------------------------*/

/**
 * @brief One time initialization function.
 *
 * This function initializes and returns the supplied context. It must be called
 * once (and only once) before calling any other function of this library.
 *
 * @param[in/out] pCellularHandle The handle pointer to store the cellular handle.
 * @param[in] pCommInterface Comm interface for communicating with the module.
 * @param[in] pTokenTable Token tables for pkthandler and pktio.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_LibInit( CellularHandle_t * pCellularHandle,
                                   const CellularCommInterface_t * pCommInterface,
                                   const CellularTokenTable_t * pTokenTable );

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
CellularError_t _Cellular_LibCleanup( CellularHandle_t cellularHandle );

/**
 * @brief Call the network registration callback if the callback is previously set by
 * Cellular_CommonRegisterUrcNetworkRegistrationEventCallback.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] urcEvent URC Event that happened.
 * @param[in] pServiceStatus The status of the network service.
 */
void _Cellular_NetworkRegistrationCallback( const CellularContext_t * pContext,
                                            CellularUrcEvent_t urcEvent,
                                            const CellularServiceStatus_t * pServiceStatus );

/**
 * @brief Call the network registration callback if the callback is previously set by
 * Cellular_RegisterUrcPdnEventCallback.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] urcEvent URC Event that happened.
 * @param[in] contextId Context ID of the PDN context.
 */
void _Cellular_PdnEventCallback( const CellularContext_t * pContext,
                                 CellularUrcEvent_t urcEvent,
                                 uint8_t contextId );

/**
 * @brief Call the network registration callback if the callback is previously set by
 * Cellular_RegisterUrcSignalStrengthChangedCallback.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] urcEvent URC Event that happened.
 * @param[in] pSignalInfo The new signal information.
 */
void _Cellular_SignalStrengthChangedCallback( const CellularContext_t * pContext,
                                              CellularUrcEvent_t urcEvent,
                                              const CellularSignalInfo_t * pSignalInfo );

/**
 * @brief Call the network registration callback if the callback is previously set by
 * Cellular_RegisterUrcGenericCallback.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pRawData Raw data received in the URC event.
 */
void _Cellular_GenericCallback( const CellularContext_t * pContext,
                                const char * pRawData );

/**
 * @brief Call the network registration callback if the callback is previously set by
 * Cellular_RegisterModemEventCallback.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] modemEvent The modem event.
 */
void _Cellular_ModemEventCallback( const CellularContext_t * pContext,
                                   CellularModemEvent_t modemEvent );

/**
 * @brief Check Library Status.
 *
 * This Functions checks if the HAL library is already opened and set up
 * for cellular modem operation for control and data plane function.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 *
 * @return CELLULAR_SUCCESS if the operation is successful , otherwise return error code.
 */
CellularError_t _Cellular_CheckLibraryStatus( CellularContext_t * pContext );

/**
 * @brief Translate error code packet status to cellular Status.
 *
 * This Functions translates packet status to cellular Status.
 *
 * @param[in] status The packet status to translate.
 *
 * @return The corresponding CellularError_t.
 */
CellularError_t _Cellular_TranslatePktStatus( CellularPktStatus_t status );

/**
 * @brief Translate Error Code AT Core status to Packet Status.
 *
 * This Functions translates AT Core status to Packet Status.
 *
 * @param[in] status The AT Core status to translate.
 *
 * @return The corresponding CellularPktStatus_t.
 */
CellularPktStatus_t _Cellular_TranslateAtCoreStatus( CellularATError_t status );

/**
 * @brief Create a socket.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] contextId Pdn context id on which this socket needs to be created.
 * @param[in] socketDomain Socket domain.
 * @param[in] socketType Socket Type.
 * @param[in] socketProtocol Socket Protocol.
 * @param[out] pSocketHandle Out parameter to provide the created handle.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_CreateSocketData( CellularContext_t * pContext,
                                            uint8_t contextId,
                                            CellularSocketDomain_t socketDomain,
                                            CellularSocketType_t socketType,
                                            CellularSocketProtocol_t socketProtocol,
                                            CellularSocketHandle_t * pSocketHandle );

/**
 * @brief Remove the socket.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] socketHandle Socket handle returned from the Cellular_CreateSocket call.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_RemoveSocketData( CellularContext_t * pContext,
                                            CellularSocketHandle_t socketHandle );

/**
 * @brief Check socket index validity.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] sockIndex Socket index returned from cellular modem.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_IsValidSocket( const CellularContext_t * pContext,
                                         uint32_t sockIndex );

/**
 * @brief Get the socket data structure with socket index.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] sockIndex Socket index returned from cellular modem.
 *
 * @return The socket data pointer if the socket index is valid, otherwise NULL is returned.
 */
CellularSocketContext_t * _Cellular_GetSocketData( const CellularContext_t * pContext,
                                                   uint32_t sockIndex );

/**
 * @brief Check PDN context index validity.
 *
 * @param[in] contextId The PDN context index to check.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_IsValidPdn( uint8_t contextId );

/**
 * @brief Convert CSQ command returned RSSI value.
 *
 * @param[in] csqRssi The CSQ command returned RSSI index.
 * @param[out] pRssiValue The output parameter to return the converted
 * RSSI value in dBm.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_ConvertCsqSignalRssi( int16_t csqRssi,
                                                int16_t * pRssiValue );

/**
 * @brief Convert CSQ command retruned BER value.
 *
 * @param[in] csqBer The CSQ command returned BER index.
 * @param[out] pBerValue The output parameter to return the converted
 * BER value in 0.01%.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_ConvertCsqSignalBer( int16_t csqBer,
                                               int16_t * pBerValue );

/**
 * @brief Get the socket data structure with socket index.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[out] pModuleContext The module context set in Cellular_ModuleInit function.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_GetModuleContext( const CellularContext_t * pContext,
                                            void ** ppModuleContext );

/**
 * @brief Convert the signal to bar.
 *
 * @param[in] rat Current RAT of the registered network.
 * @param[in/out] pSignalInfo The signal value of current RAT. The result bar
 * value is assigned in this structure.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_ComputeSignalBars( CellularRat_t rat,
                                             CellularSignalInfo_t * pSignalInfo );

/**
 * @brief Return the current RAT if previously received with 3GPP AT command.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[out] rat Current RAT of the registered network.
 *
 * @return CELLULAR_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularError_t _Cellular_GetCurrentRat( CellularContext_t * pContext,
                                         CellularRat_t * pRat );

/**
 * @brief Send the AT command to cellular modem with default timeout.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atReq The AT command data structure with send command response callback.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_AtcmdRequestWithCallback( CellularContext_t * pContext,
                                                        CellularAtReq_t atReq );

/**
 * @brief Send the AT command to cellular modem.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atReq The AT command data structure with send command response callback.
 * @param[in] timeoutMS The timeout value to wait for the response from cellular modem.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_TimeoutAtcmdRequestWithCallback( CellularContext_t * pContext,
                                                               CellularAtReq_t atReq,
                                                               uint32_t timeoutMS );

/**
 * @brief Send the AT command to cellular modem with data buffer response.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atReq The AT command data structure with send command response callback.
 * @param[in] timeoutMS The timeout value to wait for the response from cellular modem.
 * @param[in] pktDataPrefixCallback The callback function to indicate the start of data and the length of data.
 * @param[in] pCallbackContext The pCallbackContext passed to the pktDataPrefixCallback callback function.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_TimeoutAtcmdDataRecvRequestWithCallback( CellularContext_t * pContext,
                                                                       CellularAtReq_t atReq,
                                                                       uint32_t timeoutMS,
                                                                       CellularATCommandDataPrefixCallback_t pktDataPrefixCallback,
                                                                       void * pCallbackContext );

/**
 * @brief Send the AT command to cellular modem with send data.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atReq The AT command data structure with send command response callback.
 * @param[in] dataReq The following data request after the at request.
 * @param[in] atTimeoutMS The timeout value to wait for the AT command response from cellular modem.
 * @param[in] dataTimeoutMS The timeout value to wait for the data command response from cellular modem.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_TimeoutAtcmdDataSendRequestWithCallback( CellularContext_t * pContext,
                                                                       CellularAtReq_t atReq,
                                                                       CellularAtDataReq_t dataReq,
                                                                       uint32_t atTimeoutMS,
                                                                       uint32_t dataTimeoutMS );

/**
 * @brief Send the AT command to cellular modem with send data.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atReq The AT command data structure with send command response callback.
 * @param[in] dataReq The following data request after the at request.
 * @param[in] atTimeoutMS The timeout value to wait for the AT command response from cellular modem.
 * @param[in] dataTimeoutMS The timeout value to wait for the data command response from cellular modem.
 * @param[in] interDelayMS The delay between AT command and data send.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_AtcmdDataSend( CellularContext_t * pContext,
                                             CellularAtReq_t atReq,
                                             CellularAtDataReq_t dataReq,
                                             CellularATCommandDataSendPrefixCallback_t pktDataSendPrefixCallback,
                                             void * pCallbackContext,
                                             uint32_t atTimeoutMS,
                                             uint32_t dataTimeoutMS,
                                             uint32_t interDelayMS );

/**
 * @brief Send the AT command to cellular modem with send data and extra success token table
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atReq The AT command data structure with send command response callback.
 * @param[in] dataReq The following data request after the at request.
 * @param[in] atTimeoutMS The timeout value to wait for the AT command response from cellular modem.
 * @param[in] dataTimeoutMS The timeout value to wait for the data command response from cellular modem.
 * @param[in] pCellularSrcTokenSuccessTable The extra success token table to indicate the send AT command success.
 * @param[in] cellularSrcTokenSuccessTableSize The size of extra success token table.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_TimeoutAtcmdDataSendSuccessToken( CellularContext_t * pContext,
                                                                CellularAtReq_t atReq,
                                                                CellularAtDataReq_t dataReq,
                                                                uint32_t atTimeoutMS,
                                                                uint32_t dataTimeoutMS,
                                                                const char ** pCellularSrcTokenSuccessTable,
                                                                uint32_t cellularSrcTokenSuccessTableSize );

#endif /* __CELLULAR_COMMON_H__ */
