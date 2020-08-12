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


/*-----------------------------------------------------------*/
/* The config header is always included first. */
#include "aws_cellular_config.h"

/* Cellular comm interface include file. */
#include "cellular_comm_interface.h"

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                      void * pUserData,
                                                      CellularCommInterfaceHandle_t * pCommInterfaceHandle );
static CellularCommInterfaceError_t _prvCommIntfSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                      const uint8_t * pData,
                                                      uint32_t dataLength,
                                                      uint32_t timeoutMilliseconds,
                                                      uint32_t * pDataSentLength );
static CellularCommInterfaceError_t _prvCommIntfReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                         uint8_t * pBuffer,
                                                         uint32_t bufferLength,
                                                         uint32_t timeoutMilliseconds,
                                                         uint32_t * pDataReceivedLength );
static CellularCommInterfaceError_t _prvCommIntfClose( CellularCommInterfaceHandle_t commInterfaceHandle );

/*-----------------------------------------------------------*/

CellularCommInterface_t CellularCommInterface =
{
    .open = _prvCommIntfOpen,
    .send = _prvCommIntfSend,
    .recv = _prvCommIntfReceive,
    .close = _prvCommIntfClose
};

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                      void * pUserData,
                                                      CellularCommInterfaceHandle_t * pCommInterfaceHandle )
{
    /* FIX ME. */
    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfClose( CellularCommInterfaceHandle_t commInterfaceHandle )
{
    /* FIX ME. */
    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                      const uint8_t * pData,
                                                      uint32_t dataLength,
                                                      uint32_t timeoutMilliseconds,
                                                      uint32_t * pDataSentLength )
{
    /* FIX ME. */
    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                         uint8_t * pBuffer,
                                                         uint32_t bufferLength,
                                                         uint32_t timeoutMilliseconds,
                                                         uint32_t * pDataReceivedLength )
{
    /* FIX ME. */
    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/
