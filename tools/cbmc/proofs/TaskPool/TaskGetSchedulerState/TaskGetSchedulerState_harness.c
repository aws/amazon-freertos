#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

void vSetGlobalVariables( void );

/*
 * We just require scheduler flags to be nondeterministic
 * values before calling `xTaskGetSchedulerState`
 */
void harness()
{
	BaseType_t xResult;

	vSetGlobalVariables();

	xResult = xTaskGetSchedulerState();
}
