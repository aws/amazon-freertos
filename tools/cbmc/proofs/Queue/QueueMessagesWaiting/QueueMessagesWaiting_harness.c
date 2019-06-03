#include "FreeRTOS.h"
#include "queue.h"
#include "queue_datastructure.h"

#include "cbmc.h"

void harness(){
	QueueHandle_t xQueue = pvPortMalloc(sizeof(Queue_t));

	if(xQueue){
		uxQueueMessagesWaiting( xQueue );
	}
}
