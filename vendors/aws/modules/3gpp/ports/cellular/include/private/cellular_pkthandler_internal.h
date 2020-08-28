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

#ifndef __CELLULAR_PKTHANDLER_INTERNAL_H__
#define __CELLULAR_PKTHANDLER_INTERNAL_H__

#include "iot_config.h"
#include "cellular_types.h"
#include "cellular_common.h"
#include "cellular_pktio_internal.h"

/*-----------------------------------------------------------*/

/*  AT Command timeout for general AT commands. */
#define PACKET_REQ_TIMEOUT_MS    ( 5000UL )

/*-----------------------------------------------------------*/

/**
 * @brief Create the packet request mutex.
 *
 * Create the mutex for packet rquest in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
bool _Cellular_CreatePktRequestMutex( CellularContext_t * pContext );

/**
 * @brief Destroy the packet request mutex.
 *
 * Destroy the mutex for packet rquest in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_DestroyPktRequestMutex( CellularContext_t * pContext );

/**
 * @brief Create the packet response mutex.
 *
 * Create the mutex for packet response in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
bool _Cellular_CreatePktResponseMutex( CellularContext_t * pContext );

/**
 * @brief Destroy the packet response mutex.
 *
 * Destroy the mutex for packet response in cellular context.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_DestroyPktResponseMutex( CellularContext_t * pContext );

/**
 * @brief Packet handler init function.
 *
 * This function init the packet handler in cellular HAL common.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularPktStatus_t _Cellular_PktHandlerInit( CellularContext_t * pContext );

/**
 * @brief Packet handler cleanup function.
 *
 * This function cleanup the pakcet handler in cellular HAL common.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_PktHandlerCleanup( CellularContext_t * pContext );

/**
 * @brief Packet handler function.
 *
 * This function is the callback function of packet handler. It is called by packet IO thread.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 * @param[in] atRespType The AT response type from packet IO.
 * @param[in] pBuf The input data buffer from packet IO.
 */
void _Cellular_HandlePacket( CellularContext_t * pContext,
                             _atRespType_t atRespType,
                             const void * pBuf );

/**
 * @brief The URC handler init function.
 *
 * This function setup the URC handler table query function.
 *
 * @param[in] pContext The opaque cellular context pointer created by Cellular_Init.
 */
void _Cellular_AtParseInit( const CellularContext_t * pContext );

#endif /* __CELLULAR_PKTHANDLER_INTERNAL_H__ */
