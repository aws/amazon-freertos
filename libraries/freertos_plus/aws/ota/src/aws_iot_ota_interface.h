/*
 * Amazon FreeRTOS OTA V1.0.4
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef __AWS_IOT_OTA_INTERFACE__H__
#define __AWS_IOT_OTA_INTERFACE__H__

/* OTA includes. */
#include "aws_iot_ota_agent.h"
#include "aws_iot_ota_agent_internal.h"

#define OTA_CONTROL_OVER_MQTT  0x00000001

#define OTA_DATA_OVER_MQTT     0x00000001
#define OTA_DATA_OVER_HTTP     0x00000002
#define OTA_DATA_OVER_ALL      ( OTA_DATA_OVER_MQTT | OTA_DATA_OVER_HTTP )

#define OTA_PRIMARY_DATA_PROTOCOL    ( "MQTT" )
#define OTA_SECONDARY_DATA_PROTOCOL  ( "HTTP" )

typedef struct
{
	OTA_Err_t ( *prvRequestJob )( OTA_AgentContext_t * pAgentCtx );
	OTA_Err_t( *prvUpdateJobStatus )( OTA_AgentContext_t * pxAgentCtx,
		OTA_JobStatus_t eStatus,
		int32_t lReason,
		int32_t lSubReason );

} OTA_ControlInterface_t;

typedef struct
{
	OTA_Err_t( *prvInitFileTransfer )( OTA_AgentContext_t * pAgentCtx );
	OTA_Err_t( *prvRequestFileBlock )( OTA_AgentContext_t * pAgentCtx );
	OTA_Err_t( *prvDecodeFileBlock )( uint8_t* pucMessageBuffer,
		size_t xMessageSize,
		int32_t* plFileId,
		int32_t* plBlockId,
		int32_t* plBlockSize,
		uint8_t** ppucPayload,
		size_t* pxPayloadSize );
	OTA_Err_t( *prvCleanup )( OTA_AgentContext_t* pAgentCtx );

} OTA_DataInterface_t;

void prvSetControlInterface( OTA_ControlInterface_t * pxInterface );

void prvSetDataInterface(OTA_DataInterface_t * pxInterface, uint8_t * pucProtocol);

#endif
