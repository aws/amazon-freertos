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
#if 1
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"
#include "stdio.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
#define MAX_CALLBACKS	5
/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden 

typedef struct callbacks {
	/// Callback function when timer elapses
	volatile register_callback clb;
	/// Pointer to store upper layer callback context (For example: Ifx i2c context)
	void * clb_ctx;
}pal_os_event_clbs_t;

static TimerHandle_t otxTimer[MAX_CALLBACKS];
static pal_os_event_clbs_t clbs[MAX_CALLBACKS];

QueueHandle_t xQueueCallbacks;

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
	uint8_t timer_id = 0;
	pal_os_event_clbs_t clb_params;
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( xTimer );
//
    portENTER_CRITICAL();

    /* The number of times this timer has expired is saved as the
	timer's ID.  Obtain the count. */
    timer_id = ( uint8_t ) pvTimerGetTimerID( xTimer );

    clb_params.clb = clbs[timer_id].clb;
    clb_params.clb_ctx = clbs[timer_id].clb_ctx;
    /*
     * You cann't call callback from the timer callback, this might lead to a corruption
     * Use queues instead to activate corresponding handler
     * */
    xQueueSend( xQueueCallbacks, ( void * ) &clb_params, ( TickType_t ) 10 );

    portEXIT_CRITICAL();
}

/// @endcond

void vTaskCallbackHandler( void * pvParameters )
{
	pal_os_event_clbs_t clb_params;
	register_callback func = NULL;
	void * func_args = NULL;
	/* See if we can obtain the element from the Queue.  If the Queue is not
	available wait block the task to see if it becomes free.
	portMAX_DELAY works only if INCLUDE_vTaskSuspend id define to 1
	*/
	do {
		if( xQueueReceive( xQueueCallbacks, &( clb_params ), ( TickType_t ) portMAX_DELAY ) )
		{
			if (clb_params.clb)
			{
				func = clb_params.clb;
				func_args = clb_params.clb_ctx;
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
	uint8_t i = 0;
	char tmr_name[10];
	TaskHandle_t xHandle = NULL;

	for (i = 0; i < MAX_CALLBACKS; i++)
	{
		if (otxTimer[i] == NULL)
		{
			sprintf(tmr_name, "OTXTmr_%d", i);
			otxTimer[i] = xTimerCreate(  tmr_name,        /* Just a text name, not used by the kernel. */
									pdMS_TO_TICKS(100),    /* The timer period in ticks. */
									pdFALSE,         /* The timers will auto-reload themselves when they expire. */
									( void * ) i,   /* Assign each timer a unique id equal to its array index. */
									vTimerCallback  /* Each timer calls the same callback when it expires. */
									);

			if( otxTimer[i] == NULL )
			{
				/* There was insufficient FreeRTOS heap available for the semaphore to
				be created successfully. */
			    return PAL_STATUS_FAILURE;
			}
		}

	}

	/* Create a queue capable of containing MAX_CALLBACKS timers id values. */
	xQueueCallbacks = xQueueCreate( MAX_CALLBACKS, sizeof( pal_os_event_clbs_t ) );

	/* Create the handler for the callbacks. */
	xTaskCreate( vTaskCallbackHandler,       /* Function that implements the task. */
				"ClbksHndlr",          /* Text name for the task. */
				configMINIMAL_STACK_SIZE*5,      /* Stack size in words, not bytes. */
				NULL,    /* Parameter passed into the task. */
				5,/* Priority at which the task is created. */
				&xHandle );      /* Used to pass out the created task's handle. */


	return PAL_STATUS_SUCCESS;
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
	uint8_t i = 0;

    for (i = 0; i < MAX_CALLBACKS; i++)
    {
    	portENTER_CRITICAL();
    	if( xTimerIsTimerActive( otxTimer[i] ) == pdFALSE )
		{
    		if (time_us < 1000) {
    			time_us = 1000;
    		}
    		xTimerChangePeriod( otxTimer[i], pdMS_TO_TICKS(time_us / 1000), 10 );
    		clbs[i].clb = callback;
    		clbs[i].clb_ctx = callback_args;

    		portEXIT_CRITICAL();
    		break;
		} else if ( i == (MAX_CALLBACKS - 1) )
		{
			//ESP_LOGE("PAL_OS_EVENT", "No available Timers");
		}
    	portEXIT_CRITICAL();
    }
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
	const TickType_t xDelay = pdMS_TO_TICKS(time_ms);
	vTaskDelay(xDelay);
}

#else
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

#include "global_ccu4/global_ccu4.h"
#include "timer/timer.h"
#include "interrupt/interrupt.h"
#include "optiga/pal/pal_os_event.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden
/// Callback function when timer elapses
static volatile register_callback callback_registered = NULL;
/// Pointer to store upper layer callback context (For example: Ifx i2c context)
static void * callback_ctx;

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
void scheduler_timer_isr(void)
{
    register_callback callback;

    TIMER_ClearEvent(&scheduler_timer);
    //lint --e{534} suppress "Return value is not required to be checked"
    TIMER_Stop(&scheduler_timer);
    TIMER_Clear(&scheduler_timer);

    if (callback_registered)
    {
        callback = callback_registered;
        callback_registered = NULL;
        callback((void*)callback_ctx);
    }
}
/// @endcond

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
    callback_registered = callback;
    callback_ctx = callback_args;

    //lint --e{534} suppress "Return value is not required to be checked"
    TIMER_SetTimeInterval(&scheduler_timer , (time_us*100));
    TIMER_Start(&scheduler_timer);
}

/**
* @}
*/


#endif
/**
* @}
*/

