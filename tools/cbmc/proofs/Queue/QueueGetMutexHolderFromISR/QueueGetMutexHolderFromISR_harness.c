#include "FreeRTOS.h"
#include "queue.h"
#include "queue_datastructure.h"

#include "cbmc.h"

void harness(){
	QueueHandle_t xSemaphore = pvPortMalloc(sizeof(Queue_t));
	if (xSemaphore) {
		xQueueGetMutexHolderFromISR( xSemaphore );
	}
}

