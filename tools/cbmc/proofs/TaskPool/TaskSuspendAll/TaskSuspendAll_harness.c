#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

/*
 * We just call vTaskSuspendAll(). No assumption
 * or abstraction is required for this proof
 */
void harness()
{
	vTaskSuspendAll();
}
