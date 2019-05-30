#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

BaseType_t xPrepareTaskLists( TaskHandle_t * xTask );

/*
 * We assume `uxNewPriority` to be a valid priority (avoids failed assert).
 * The harness test first calls two functions included in the tasks.c file
 * that initialize the task lists and other global variables
 */
void harness()
{
	TaskHandle_t xTask;
	UBaseType_t uxNewPriority;
	BaseType_t xTasksPrepared;

	__CPROVER_assume( uxNewPriority < configMAX_PRIORITIES );

	xTasksPrepared = xPrepareTaskLists( &xTask );

	if ( xPrepareTaskLists( &xTask ) != pdFAIL )
	{
		vTaskPrioritySet( xTask, uxNewPriority );
	}
}
