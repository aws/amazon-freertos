#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"

#include "cbmc.h"

void harness(){
	QueueHandle_t xQueue = xUnconstrainedMutex();
	BaseType_t *xHigherPriorityTaskWoken = pvPortMalloc(sizeof(BaseType_t));
	if(xQueue){
		xQueue->uxMessagesWaiting = nondet_UBaseType_t();
		xQueueGiveFromISR( xQueue, xHigherPriorityTaskWoken );
	}

}
