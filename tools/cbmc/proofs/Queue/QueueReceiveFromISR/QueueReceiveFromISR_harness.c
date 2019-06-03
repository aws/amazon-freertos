#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"
#include "cbmc.h"

/* If the item size is not bounded, the proof does not finish in a reasonable
	time due to the involved memcpy commands. */
#ifndef MAX_ITEM_SIZE
	#define MAX_ITEM_SIZE 10
#endif

void harness(){
	QueueHandle_t xQueue =
	xUnconstrainedQueueBoundedItemSize(MAX_ITEM_SIZE);

	BaseType_t *xHigherPriorityTaskWoken = pvPortMalloc(sizeof(BaseType_t));

	if(xQueue){
		void *pvBuffer = pvPortMalloc(xQueue->uxItemSize);
		if(!pvBuffer){
			xQueue->uxItemSize = 0;
		}
		xQueueReceiveFromISR( xQueue, pvBuffer, xHigherPriorityTaskWoken );
	}
}
