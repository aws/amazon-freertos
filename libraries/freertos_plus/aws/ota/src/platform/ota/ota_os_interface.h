/*
 * FreeRTOS OTA V1.2.0
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

#ifndef _AWS_OTA_OS_INTERFACE_H_
#define _AWS_OTA_OS_INTERFACE_H_

 /* Standard library includes. */
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

struct OtaEventContext;
typedef struct OtaEventContext OtaEventContext_t;

struct OtaTimerContext;
typedef struct OtaTimerContext OtaTimerContext_t;

/**
 * @brief Initialize the OTA events.
 *
 * This function initializes the OTA events mechanism for maximum events passed as a
 * parameter.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @param[size]          Maximum number of events.
 *
 * @return               OTA_OS_ERR_OK if success , other error code on failure.
 */

typedef int32_t ( * ota_InitEvent_t ) (OtaEventContext_t* pContext );

/**
 * @brief Sends an OTA event.
 *
 * This function sends an event to the OTA event handler.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @param[pEventMsg]     Event to be sent to the OTA handler.
 *
 * @param[timeout]       The maximum amount of time (msec) the task should block.
 *
 * @return               OTA_OS_ERR_OK if success , other error code on failure.
 */
typedef int32_t ( * ota_SendEvent_t )( OtaEventContext_t* pContext,
	                                   const void* pEventMsg,
	                                   unsigned int timeout);

/**
 * @brief Receive an OTA event.
 *
 * This function receives an event from the pending OTA events.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @param[pEventMsg]     Pointer to store message.
 *
 * @param[timeout]       The maximum amount of time the task should block.
 *
 * @return               OTA_OS_ERR_OK if success , other error code on failure.
 */
typedef int32_t ( * ota_ReceiveEvent_t )( OtaEventContext_t* pContext,
	                                      void** pEventMsg,
	                                      uint32_t timeout);

/**
 * @brief Denitialize the OTA Events mechanism.
 *
 * This function de-initializes the OTA events mechanism and frees any resources
 * used.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @return               None
 */

typedef void ( * ota_DeinitEvent_t )( OtaEventContext_t* pContext );

 /**
  * @brief Create timer.
  *
  * This function creates a timer with the timeout passed as parameter as well as 
  * registers the callback.
  *
  * @param[pTimerName]           Timer name string.
  *
  * @param[timeout]              Period of the timer in msec.
  *
  * @param[pTimerCallback]       Callback function when timer expires.
  *
  * @return[OtaTimerContext_t]   Timer context created or NULL on failure.
  */

typedef int32_t ( * ota_CreateTimer_t ) ( OtaTimerContext_t* pContext,
										  const char* const pTimerName,
	                                      const uint32_t timeout );

/**
 * @brief start timer.
 *
 * This function starts the timer and reset it if it is already started.
 *
 * @param[mutexHandle ]     Pointer to the timer context to start/reset.
 *
 * @param[timeout ]         timeout for the timer.
 *
 * @return                  0 if success , otaOSErr_t on failure.
 */
typedef int32_t ( * ota_StartTimer_t ) ( OtaTimerContext_t* pContext );

/**
 * @brief stop timer.
 *
 * This function stops the timer.
 *
 * @param[mutexHandle ]     Pointer to the timer context to start/reset. to stop.
 *
 * @return                  0 if success , otaOSErr_t on failure.
 */
typedef int32_t ( * ota_StopTimer_t ) ( OtaTimerContext_t* pContext );

/**
 * @brief Delete a timer.
 *
 * This function deletes a timer.
 *
 * @param[timerHandle  ]    Pointer to the timer object to delete.
 *
 * @return                  0 if success , otaOSErr_t on failure.
 */
typedef void ( * ota_DeleteTimer_t ) ( OtaTimerContext_t* pContext );

/**
 * @brief TTimer Callback
 *
 * This function deletes a timer.
 *
 * @param[timerHandle  ]    Pointer to the timer object to delete.
 *
 * @return                  0 if success , otaOSErr_t on failure.
 */
typedef void ( * ota_TimerCallback_t ) ( OtaTimerContext_t* pContext );

/**
 *  OTA Event Interface structure.
 */
typedef struct OtaEventInterface
{
	ota_InitEvent_t init;
	ota_SendEvent_t send;
	ota_ReceiveEvent_t recv;
	ota_DeinitEvent_t deinit;
	OtaEventContext_t* pEventCtx;  /**< Implementation-defined ota event context. */
}OtaEventInterface_t;

/**
 *  OTA Retry Timer Interface.
 */
typedef struct OtaTimerInterface
{
	ota_StartTimer_t start;
	ota_StopTimer_t stop;
	ota_DeleteTimer_t delete;
	ota_TimerCallback_t callback;
	OtaTimerContext_t* PTimerCtx[2];  /**< Implementation-defined ota timer context. */
} OtaTimerInterface_t;

/**
 * @brief  OTA OS Interface.
 */
typedef struct OtaOSInterface
{
	OtaEventInterface_t event;           /**< OTA Event interface. */
	OtaTimerInterface_t timer;           /**< OTA Timer interface. */
} OtaOsInterface_t;

#endif
