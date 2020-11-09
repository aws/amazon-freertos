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

#ifndef __CELLULAR_COMMON_INTERNAL_H__
#define __CELLULAR_COMMON_INTERNAL_H__

/* Cellular includes. */
#include "cellular_comm_interface.h"
#include "cellular_types.h"
#include "cellular_pkthandler_internal.h"
#include "cellular_at_core.h"
#include "cellular_pktio_internal.h"
#include "cellular_common.h"

/*-----------------------------------------------------------*/

#define PKTIO_READ_BUFFER_SIZE     ( 1600U )      /* This should be larger than TCP packet size. */
#define PKTIO_WRITE_BUFFER_SIZE    ( CELLULAR_AT_CMD_MAX_SIZE )

/*-----------------------------------------------------------*/

/**
 * @brief Cellular network register URC type.
 */
typedef enum CellularNetworkRegType
{
    CELLULAR_REG_TYPE_CREG = 0,
    CELLULAR_REG_TYPE_CGREG,
    CELLULAR_REG_TYPE_CEREG,
    CELLULAR_REG_TYPE_MAX,
    CELLULAR_REG_TYPE_UNKNOWN
} CellularNetworkRegType_t;

/**
 * @brief Represents different URC event callback registration function.
 */
typedef struct _callbackEvents
{
    CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback;
    void * pNetworkRegistrationCallbackContext;
    CellularUrcPdnEventCallback_t pdnEventCallback;
    void * pPdnEventCallbackContext;
    CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback;
    void * pSignalStrengthChangedCallbackContext;
    CellularUrcGenericCallback_t genericCallback;
    void * pGenericCallbackContext;
    CellularModemEventCallback_t modemEventCallback;
    void * pModemEventCallbackContext;
} _callbackEvents_t;

/**
 * @brief Structure containing all the control plane parameters of Modem.
 */
typedef struct cellularAtData
{
    CellularRat_t rat;                               /* Device registered Radio Access Technology (Cat-M, Cat-NB, GPRS etc).  */
    CellularNetworkRegistrationStatus_t csRegStatus; /* CS (Circuit Switched) registration status (registered/searching/roaming etc.). */
    CellularNetworkRegistrationStatus_t psRegStatus; /* PS (Packet Switched) registration status (registered/searching/roaming etc.). */
    uint8_t csRejectType;                            /* CS Reject Type. 0 - 3GPP specific Reject Cause. 1 - Manufacture specific. */
    uint8_t csRejCause;                              /* Circuit Switch Reject cause. */
    uint8_t psRejectType;                            /* PS Reject Type. 0 - 3GPP specific Reject Cause. 1 - Manufacture specific. */
    uint8_t psRejCause;                              /* Packet Switch Reject cause. */
    uint32_t cellId;                                 /* Registered network operator cell Id. */
    uint16_t lac;                                    /* Registered network operator Location Area Code. */
    uint16_t rac;                                    /* Registered network operator Routing Area Code. */
    uint16_t tac;                                    /* Registered network operator Tracking Area Code. */
} cellularAtData_t;

/**
 * @brief Parameters involved in maintaining the context for the modem.
 */
typedef struct CellularContext
{
    /* Communication interface for target specific. */
    const CellularCommInterface_t * pCommIntf;

    /* Common library. */
    bool bLibOpened;   /* CellularLib is currently open */
    bool bLibShutdown; /* CellularLib prematurely shut down */
    bool bLibClosing;  /* Graceful shutdown in progress */
    IotMutex_t libStatusMutex;
    IotMutex_t libAtDataMutex;
    _callbackEvents_t cbEvents; /* Call back functions registered to report events. */
    cellularAtData_t libAtData; /* Global variables. */

    /* Token table to config pkthandler and pktio. */
    CellularTokenTable_t tokenTable;

    /* Packet handler. */
    IotMutex_t pktRequestMutex;
    IotMutex_t PktRespMutex;
    QueueHandle_t pktRespQueue;
    CellularATCommandResponseReceivedCallback_t pktRespCB;
    CellularATCommandDataPrefixCallback_t pktDataPrefixCB;         /* Data prefix callback function for socket receive function. */
    void * pDataPrefixCBContext;                                   /* The pCallbackContext passed to CellularATCommandDataPrefixCallback_t. */
    CellularATCommandDataSendPrefixCallback_t pktDataSendPrefixCB; /* Data prefix callback function for socket send function. */
    void * pDataSendPrefixCBContext;                               /* The pCallbackContext passed to CellularATCommandDataSendPrefixCallback_t. */
    void * pPktUsrData;                                            /* The pData passed to CellularATCommandResponseReceivedCallback_t. */
    uint16_t PktUsrDataLen;                                        /* The dataLen passed to CellularATCommandResponseReceivedCallback_t. */
    const char * pCurrentCmd;                                      /* Debug purpose. */

    /* Packet IO. */
    bool bPktioUp;
    CellularCommInterfaceHandle_t hPktioCommIntf;
    EventGroupHandle_t pPktioCommEvent;
    _pPktioShutdownCallback_t pPktioShutdownCB;
    _pPktioHandlePacketCallback_t pPktioHandlepktCB;
    char pktioSendBuf[ PKTIO_WRITE_BUFFER_SIZE + 1 ];
    char pktioReadBuf[ PKTIO_READ_BUFFER_SIZE + 1 ];
    char * pPktioReadPtr;
    const char * pRespPrefix;
    CellularATCommandType_t PktioAtCmdType;
    _atRespType_t recvdMsgType;

    /* PktIo data handling. */
    uint32_t dataLength;
    uint32_t partialDataRcvdLen;

    /* Socket. */
    CellularSocketContext_t * pSocketData[ CELLULAR_NUM_SOCKET_MAX ]; /* All socket related information. */

    /* Module Context. */
    void * pModueContext;

    /* The context is already defined in types to hide internal data strcture from user. */
    /* coverity[misra_c_2012_rule_1_1_violation] */
} CellularContext_t;

/*-----------------------------------------------------------*/

/**
 * @brief Netwrok registration respone parsing function.
 *
 * Parse the network registration response from AT command response or URC.
 * The result is stored in the pContext.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pRegPayload The input string from AT command respnose or URC.
 * @param[in] isUrc The input string source type.
 * @param[in] regType The network registration type.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_ParseRegStatus( CellularContext_t * pContext,
                                              char * pRegPayload,
                                              bool isUrc,
                                              CellularNetworkRegType_t regType );

/**
 * @brief Initializes the AT data structures.
 *
 * Function is invokes to initialize the AT data structure during
 * very first initialization, upon deregistration and upon receiving
 * network reject during registration.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] mode Parameter that identifies which elements in Data Structure
 * to be initialized.
 */
void _Cellular_InitAtData( CellularContext_t * pContext,
                           uint32_t mode );

/**
 * @brief Create the AT data mutex.
 *
 * Create the mutex for AT data in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
bool _Cellular_CreateAtDataMutex( CellularContext_t * pContext );

/**
 * @brief Destroy the AT data mutex.
 *
 * Destroy the mutex for AT data in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 *
 */
void _Cellular_DestroyAtDataMutex( CellularContext_t * pContext );


/**
 * @brief Lock the AT data mutex.
 *
 * Lock the mutex for AT data in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_LockAtDataMutex( CellularContext_t * pContext );

/**
 * @brief Unlock the AT data mutex.
 *
 * Unlock the mutex for AT data in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_UnlockAtDataMutex( CellularContext_t * pContext );

#endif /* ifndef __CELLULAR_COMMON_INTERNAL_H__ */
