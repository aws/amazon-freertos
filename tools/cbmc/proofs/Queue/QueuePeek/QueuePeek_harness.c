#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"
#include "tasksStubs.h"

#include "cbmc.h"

#ifndef LOCK_BOUND
	#define LOCK_BOUND 4
#endif

#ifndef QUEUE_PEEK_BOUND
	#define QUEUE_PEEK_BOUND 4
#endif

QueueHandle_t xQueue;


/* This method is called to initialize pxTimeOut.
   Setting up the data structure is not interesting for the proof,
   but the harness uses it to model a release
   on the queue after first check. */
void vTaskInternalSetTimeOutState( TimeOut_t * const pxTimeOut ){
	xQueue-> uxMessagesWaiting = nondet_BaseType_t();
}

void harness(){
	xQueue = xUnconstrainedQueueBoundedItemSize(10);

	//Initialise the tasksStubs
	vInitTaskCheckForTimeOut(0, QUEUE_PEEK_BOUND - 1);

	TickType_t xTicksToWait;
	if(xState == taskSCHEDULER_SUSPENDED){
		xTicksToWait = 0;
	}

	if(xQueue){
		__CPROVER_assume(xQueue->cTxLock < LOCK_BOUND - 1);
		__CPROVER_assume(xQueue->cRxLock < LOCK_BOUND - 1);

		void *pvItemToQueue = pvPortMalloc(xQueue->uxItemSize);

		/* In case malloc fails as this is otherwise an invariant violation. */
		if(!pvItemToQueue){
			xQueue->uxItemSize = 0;
		}

		xQueuePeek( xQueue, pvItemToQueue, xTicksToWait );
	}
}
