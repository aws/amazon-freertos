#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

/*
 * We assume xTime to allocated since the function includes a configASSERT
 * call at the beginning to make sure the pointer to it is not NULL
 */
void harness()
{
	TimeOut_t xTime;

	vTaskSetTimeOutState( &xTime );
}
