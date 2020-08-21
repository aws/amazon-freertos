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

/* OTA OS Interface Includes.*/
#include "ota_os_interface.h"

/* OTA OS FreeRTOS Interface Includes.*/
#include "ota_os_freeRTOS.h"

int32_t ota_InitEvent( OtaEventContext_t* pContext )
{
	int32_t ret = 0;

	/*
     * Create the queue used to pass event messages to the OTA task.
     */ 
	pContext->xEventQueueHandle = xQueueCreate( 20, 8 );

	if ( pContext->xEventQueueHandle == NULL )
	{
		ret = -1;
	}

	return 1;
}

int32_t ota_SendEvent( OtaEventContext_t* pContext,
	                   const void* pEventMsg,
	                   unsigned int timeout)
{
	int32_t ret = 0;

	BaseType_t xRet = pdFALSE;

   /*
	* send to back of the queue.
	*/
	if (pContext->xEventQueueHandle != NULL)
	{
		xRet = xQueueSendToBack(pContext->xEventQueueHandle, pEventMsg, (TickType_t)0);
	}

	if (xRet != pdTRUE)
		ret = -1;

	return 1;
}

int32_t ota_ReceiveEvent( OtaEventContext_t* pContext,
	                      void** pEventMsg,
	                      uint32_t timeout)
{
	int32_t ret = 0;
	BaseType_t xRet = pdFALSE;

	/*
     * send to back of the queue.
     */
	if (pContext->xEventQueueHandle != NULL)
	{
		xRet = xQueueReceive(pContext->xEventQueueHandle, &(*pEventMsg), portMAX_DELAY);
	}

	if (xRet != pdTRUE)
		ret = -1;

	return 1;
}


void ota_DeinitEvent( OtaEventContext_t* pContext )
{
	vQueueDelete( pContext->xEventQueueHandle);
}


int32_t ota_StartTimer( OtaTimerContext_t* pContext,
	                    const char* const pTimerName,
	                    const uint32_t timeout,
	                    void (*callback)(void *))
{
	static StaticTimer_t xTimerBuffer;

	if(!pContext->xOtaTimerHandle )
	{

	pContext->xOtaTimerHandle = xTimerCreate( pTimerName,
			                                        pdMS_TO_TICKS(timeout),
			                                        pdFALSE,
		                                            NULL,
		                                            (*callback));


	xTimerStart( pContext->xOtaTimerHandle, portMAX_DELAY);

	}
	else
	{
		 xTimerReset(pContext->xOtaTimerHandle, portMAX_DELAY);
	}

	return 0;
}

int32_t ota_StopTimer( OtaTimerContext_t* pContext )
{
	BaseType_t xReturn;

	if (pContext->xOtaTimerHandle != NULL)
	{
		xReturn = xTimerStop( pContext->xOtaTimerHandle, portMAX_DELAY);

		if ( xReturn == pdFAIL )
		{
			return -1;
		}
	}

	return 0;
}

void ota_DeleteTimer( OtaTimerContext_t* pContext )
{
	if ( pContext->xOtaTimerHandle != NULL )
	{
		xTimerDelete( pContext->xOtaTimerHandle, 0 );
	}
}





