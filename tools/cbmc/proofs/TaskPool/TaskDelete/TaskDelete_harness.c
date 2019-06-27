#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

void vSetGlobalVariables();
BaseType_t xPrepareTaskLists( TaskHandle_t * xTask );

/*
 * The harness test first calls two functions included in the tasks.c file
 * that initialize the task lists and other global variables
 */
void harness()
{
	TaskHandle_t xTaskToDelete;
	BaseType_t xTasksPrepared;

	vSetGlobalVariables();
	xTasksPrepared = xPrepareTaskLists( &xTaskToDelete );

	if ( xTasksPrepared != pdFAIL )
	{
		vTaskDelete( xTaskToDelete );
	}
}
