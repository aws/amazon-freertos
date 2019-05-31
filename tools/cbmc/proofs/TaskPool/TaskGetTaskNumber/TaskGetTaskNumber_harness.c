#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

void vPrepareTask( TaskHandle_t * xTask );

/* 
 * We prepare an unconstrained task (possibly NULL)
 * and then call `uxTaskGetTaskNumber`
 */
void harness()
{
	TaskHandle_t xTask;
	UBaseType_t uxTaskNumber;

	vPrepareTask( &xTask );

	uxTaskNumber = uxTaskGetTaskNumber( xTask );
}
