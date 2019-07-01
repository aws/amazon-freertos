#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"

#include "cbmc.h"

struct QueueDefinition;

void harness() {
	BaseType_t xNewQueue;

	QueueHandle_t xQueue = xUnconstrainedQueue();
	if(xQueue != NULL)
	{
		xQueueGenericReset(xQueue, xNewQueue);
	}
}
