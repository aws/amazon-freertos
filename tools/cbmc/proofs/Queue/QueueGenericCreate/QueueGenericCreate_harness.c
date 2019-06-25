#include "FreeRTOS.h"
#include "queue.h"
#include "queue_datastructure.h"
#include "cbmc.h"


void harness(){
	UBaseType_t uxQueueLength;
	UBaseType_t uxItemSize;
	uint8_t ucQueueType;
	__CPROVER_assume(uxQueueLength > ( UBaseType_t ) 0);

	/* The implicit assumption for the QueueGenericCreate method is,
	   that there are no overflows in the computation and the inputs are safe.
	   There is no check for this in the code base */
	UBaseType_t upper_bound = portMAX_DELAY - sizeof(Queue_t);
	__CPROVER_assume(uxItemSize < (upper_bound)/uxQueueLength);
	xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType );
}
