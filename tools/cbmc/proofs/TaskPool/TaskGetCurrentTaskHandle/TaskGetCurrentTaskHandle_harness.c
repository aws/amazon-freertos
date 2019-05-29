#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

BaseType_t xPrepareCurrentTCB( void );

/*
 * We set pxCurrentTCB to an unconstrained TCB. Then we call
 * xTaskGetCurrentTaskHandle() and check the return value is not NULL
 */
void harness()
{
	TaskHandle_t xTask;
	BaseType_t xTasksPrepared;

	xTasksPrepared = xPrepareCurrentTCB();

	if ( xTasksPrepared != pdFAIL )
	{
		xTask = xTaskGetCurrentTaskHandle();

		__CPROVER_assert( xTask != NULL , "Current task handle is NULL!");
	}
}
