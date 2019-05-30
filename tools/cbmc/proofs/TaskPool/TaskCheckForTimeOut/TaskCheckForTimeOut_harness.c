#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

BaseType_t xPrepareCurrentTCB( void );

/*
 * We just need to make sure that `pxTimeOut`, `pxTicksToWait`
 * and `pxCurrentTCB` are not NULL values before calling the function
 */
void harness()
{
	UBaseType_t xTasksPrepared;
	TimeOut_t pxTimeOut;
	TickType_t pxTicksToWait;
	UBaseType_t xResult;
	
	xTasksPrepared = xPrepareCurrentTCB();

    if ( xTasksPrepared != pdFAIL )
    {
    	xResult = xTaskCheckForTimeOut( &pxTimeOut, &pxTicksToWait );
    }
}
