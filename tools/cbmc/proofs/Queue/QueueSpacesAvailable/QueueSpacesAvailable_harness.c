#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"
#include "cbmc.h"

void harness(){
	QueueHandle_t xQueue =
	xUnconstrainedQueue();

	if(xQueue){
		xQueue->uxMessagesWaiting = nondet_UBaseType_t();
		uxQueueSpacesAvailable( xQueue );
	}
}
