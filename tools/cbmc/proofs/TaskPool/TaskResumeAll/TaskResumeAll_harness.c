#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

void vSetGlobalVariables( void );
BaseType_t xPrepareTaskLists( void );

/*
 * The harness test first calls two functions included in the tasks.c file
 * that initialize the task lists and other global variables
 */
void harness()
{
	BaseType_t xTasksPrepared;

	vSetGlobalVariables();
	xTasksPrepared = xPrepareTaskLists();

	if ( xTasksPrepared != pdFAIL )
	{
		xTaskResumeAll();
	}
}
