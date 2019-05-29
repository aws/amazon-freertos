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
 * We prepare task lists by inserting one item in each list,
 * and with the macro redefinition we ensure only valid ready
 * task lists are checked in `taskSELECT_HIGHEST_PRIORITY_TASK()`
 */
void harness()
{
	BaseType_t xTasksPrepared;

	vSetGlobalVariables();
	xTasksPrepared = xPrepareTaskLists();

	if ( xTasksPrepared != pdFAIL )
	{
		vTaskSwitchContext();
	}
}
