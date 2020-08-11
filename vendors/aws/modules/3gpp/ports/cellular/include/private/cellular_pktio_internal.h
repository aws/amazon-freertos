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


#ifndef __CELLULAR_PKTIO_INTERNAL_H__
#define __CELLULAR_PKTIO_INTERNAL_H__

#include "cellular_types.h"

/*-----------------------------------------------------------*/

/**
 * @brief The received AT response type.
 */
typedef enum _atRespType
{
    AT_SOLICITED = 0,
    AT_UNSOLICITED,
    AT_UNDEFINED
} _atRespType_t;

/**
 * @brief Callback used to inform packet received.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atRespType The received packet type.
 * @param[in] pBuf The input data buffer from packet IO.
 */
typedef void ( * _pPktioHandlePacketCallback_t ) ( CellularContext_t * pContext,
                                                   _atRespType_t atRespType,
                                                   const void * pBuffer );

/**
 * @brief Callback used to inform packet IO thread shutdown.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
typedef void ( * _pPktioShutdownCallback_t ) ( CellularContext_t * pContext );

/*-----------------------------------------------------------*/

/**
 * @brief Packet IO init function.
 *
 * This function init the packet IO in cellular HAL common.
 * Packet IO thread is created in this function.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] handlePacketCb The callback function to handle the packet received.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_PktioInit( CellularContext_t * pContext,
                                         _pPktioHandlePacketCallback_t handlePacketCb );

/**
 * @brief Packet IO shutdown function.
 *
 * This function shutdown the packet IO in cellular HAL common.
 * Packet IO thread is shutdown in this function.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_PktioShutdown( CellularContext_t * pContext );

/**
 * @brief Packet IO shutdown callback set function.
 *
 * This function set the shutdown callback set function.
 * Once the packet IO thread is shutdown, the packet IO thread will call this function.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] shutdownCb The shutdown callback function.
 */
void _Cellular_PktioSetShutdownCallback( CellularContext_t * pContext,
                                         _pPktioShutdownCallback_t shutdownCb );

/**
 * @brief Send AT command function.
 *
 * This function setup the internal data of pktio and send the AT command through comm interface.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pAtCmd The AT command to send.
 * @param[in] atType The AT command type.
 * @param[in] pAtRspPrefix The AT command response prefix.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_PktioSendAtCmd( CellularContext_t * pContext,
                                              const char * pAtCmd,
                                              CellularATCommandType_t atType,
                                              const char * pAtRspPrefix );

/**
 * @brief Send data command function.
 *
 * This function setup the internal data of pktio and send the data through comm interface.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pData The data to send.
 * @param[in] dataLen The data length of pData.
 *
 * @return The data actually send to the comm interface.
 */
uint32_t _Cellular_PktioSendData( CellularContext_t * pContext,
                                  const uint8_t * pData,
                                  uint32_t dataLen );

#endif /* __CELLULAR_PKTIO_INTERNAL_H__ */
