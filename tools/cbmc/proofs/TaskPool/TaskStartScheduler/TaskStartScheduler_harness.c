#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

BaseType_t xPrepareTasks( void );

/*
 * We prepare `pxCurrentTCB` and the idle/timer task buffers
 * (TCB + stack), then we call the function if allocation was ok
 */
void harness()
{
	BaseType_t xTasksPrepared;

    xTasksPrepared = xPrepareTasks();

    if ( xTasksPrepared != pdFAIL )
    {
	   vTaskStartScheduler();
    }
}
