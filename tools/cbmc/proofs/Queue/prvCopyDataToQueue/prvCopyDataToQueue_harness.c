#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"
#include "cbmc.h"

BaseType_t prvCopyDataToQueue( Queue_t * const pxQueue, const void *pvItemToQueue, const BaseType_t xPosition );

void harness(){
	QueueHandle_t xQueue = xUnconstrainedQueueBoundedItemSize(10);


	if( xQueue ){
		void *pvItemToQueue = pvPortMalloc(xQueue->uxItemSize);
		if( !pvItemToQueue )
		{
			xQueue->uxItemSize = 0;
		}
		if(xQueue->uxItemSize == 0)
		{
			xQueue->uxQueueType = nondet_int8_t();
		}
		BaseType_t xPosition;
		prvCopyDataToQueue( xQueue, pvItemToQueue, xPosition );
	}

}
