/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the platform abstraction layer APIs for os event/scheduler.
*
* \ingroup  grPAL
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"
#include "stdio.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden 

typedef struct callbacks {
	/// Callback function when timer elapses
	volatile register_callback func;
	/// Pointer to store upper layer callback context (For example: Ifx i2c context)
	void * args;
}pal_os_event_clbs_t;

static pal_os_event_clbs_t clb_ctx_0;

SemaphoreHandle_t xSemaphore = NULL;
TimerHandle_t     xTimer = NULL;

/**
*  Timer callback handler. 
*
*  This get called from the TIMER elapse event.<br>
*  Once the timer expires, the registered callback funtion gets called from the timer event handler, if
*  the call back is not NULL.<br>
*
*\param[in] args Callback argument
*
*/
void vTimerCallback( TimerHandle_t xTimer )
 {
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( xTimer );

    /*
     * You can't call callback from the timer callback, this might lead to a corruption
     * Use a semaphore instead
     * */
    xSemaphoreGive( xSemaphore );
}

/// @endcond

void vTaskCallbackHandler( void * pvParameters )
{
	register_callback func = NULL;
	void * func_args = NULL;
	/* See if we can obtain the element from the semaphore.  If the semaphore is not
	available wait block the task to see if it becomes free.
	portMAX_DELAY works only if INCLUDE_vTaskSuspend id define to 1
	*/
	printf("vTaskCallbackHandler\r\n");
	do {
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
        {
			if (clb_ctx_0.func)
			{
				func = clb_ctx_0.func;
				func_args = clb_ctx_0.args;
				func((void*)func_args);
			}
		}
	} while(1);
}


/**
* Platform specific event init function.
* <br>
*
* <b>API Details:</b>
*         This function initialise all required event related variables.<br>
*
*
*/
pal_status_t pal_os_event_init(void)
{
	pal_status_t status = PAL_STATUS_FAILURE;
	BaseType_t xReturned;
	
	do {
		/* Create a semaphore and take it now */
		xSemaphore = xSemaphoreCreateBinary();
		if( xSemaphore == NULL )
		{
			break;
		}
		xSemaphoreTake( xSemaphore, ( TickType_t ) 10 );

		/* Create the handler for the callbacks. */
		xReturned = xTaskCreate( vTaskCallbackHandler,       /* Function that implements the task. */
								"otx_os_tsk",                /* Text name for the task. */
								configMINIMAL_STACK_SIZE*5,  /* Stack size in words, not bytes. */
								NULL,        /* Parameter passed into the task. */
								5,           /* Priority at which the task is created. */
								NULL );      /* Used to pass out the created task's handle. */
		if( xReturned != pdPASS  )
		{
			break;
		}
			
		xTimer = xTimerCreate("otx_os_tmr",        /* Just a text name, not used by the kernel. */
							  1 / portTICK_PERIOD_MS,    /* The timer period in ticks. */
							  pdFALSE,         /* The timers will auto-reload themselves when they expire. */
							  ( void * ) NULL,   /* Assign each timer a unique id equal to its array index. */
							  vTimerCallback  /* Each timer calls the same callback when it expires. */
							  );
		if( xTimer == NULL )
		{
			break;
		}
		status = PAL_STATUS_SUCCESS;

	} while(0);
				
	return status;
}
/**
* Platform specific event call back registration function to trigger once when timer expires.
* <br>
*
* <b>API Details:</b>
*         This function registers the callback function supplied by the caller.<br>
*         It triggers a timer with the supplied time interval in microseconds.<br>
*         Once the timer expires, the registered callback function gets called.<br>
* 
* \param[in] callback              Callback function pointer
* \param[in] callback_args         Callback arguments
* \param[in] time_us               time in micro seconds to trigger the call back
*
*/
void pal_os_event_register_callback_oneshot(register_callback callback, 
                                            void* callback_args, 
                                            uint32_t time_us)
{
	if (time_us < 1000) {
		time_us = 1000;
	}
	
	clb_ctx_0.func = callback;
	clb_ctx_0.args = callback_args;
	
	xTimerChangePeriod( xTimer, (time_us / 1000) / portTICK_PERIOD_MS, 10 );
}

/**
* Platform specific task delay function.
* <br>
*
* <b>API Details:</b>
*         This function produces delay only for the. <br>
*         If you don't need this functionality just leave it empty.<br>
*         n this case a compiler should optimize this function out.<br>
* 
* \param[in] time_ms               time in milli seconds to delay
*
*/
void pal_os_event_delayms(uint32_t time_ms)
{
	const TickType_t xDelay = time_ms / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);
}

/**
* @}
*/

