/*
 * FreeRTOS V202012.00
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
 */

/**
 * @file freertos_agent_message.h
 * @brief Functions to interact with queues.
 */
#ifndef FREERTOS_AGENT_MESSAGE_H
#define FREERTOS_AGENT_MESSAGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* Include MQTT agent messaging interface. */
#include "agent_message.h"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Logging related header files are required to be included in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define LIBRARY_LOG_NAME and  LIBRARY_LOG_LEVEL.
 * 3. Include the header file "logging_stack.h".
 */

/* Include header that defines log levels. */
#include "logging_levels.h"

/* Logging configuration for the Subscription Manager module. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "MQTT Agent Interface"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

/**
 * @ingroup mqtt_agent_struct_types
 * @brief Context with which tasks may deliver messages to the agent.
 */
struct AgentMessageContext
{
    QueueHandle_t queue;
};

/*-----------------------------------------------------------*/

/**
 * @brief Send a message to the specified context.
 * Must be thread safe.
 *
 * @param[in] pxMsgCtx An #AgentMessageContext_t.
 * @param[in] pData Pointer to element to send to queue.
 * @param[in] ulBlockTimeMs Block time to wait for a send.
 *
 * @return `true` if send was successful, else `false`.
 */
bool Agent_MessageSend( const AgentMessageContext_t * pxMsgCtx,
                        const void * pData,
                        uint32_t ulBlockTimeMs );

/**
 * @brief Receive a message from the specified context.
 * Must be thread safe.
 *
 * @param[in] pxMsgCtx An #AgentMessageContext_t.
 * @param[in] pBuffer Pointer to buffer to write received data.
 * @param[in] ulBlockTimeMs Block time to wait for a receive.
 *
 * @return `true` if receive was successful, else `false`.
 */
bool Agent_MessageReceive( const AgentMessageContext_t * pxMsgCtx,
                           void * pBuffer,
                           uint32_t ulBlockTimeMs );

#endif /* FREERTOS_AGENT_MESSAGE_H */
