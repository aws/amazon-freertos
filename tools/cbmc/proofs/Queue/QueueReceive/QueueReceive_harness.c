#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"
#include "tasksStubs.h"
#include "cbmc.h"

/* prvUnlockQueue is going to decrement this value to 0 in the loop.
   We need a bound for the loop. Using 4 has a reasonable performance resulting
   in 3 unwinding iterations of the loop. The loop is mostly modifying a
   data structure in task.c that is not in the scope of the proof. */
#ifndef LOCK_BOUND
	#define LOCK_BOUND 4
#endif

/* This code checks for time outs. This value is used to bound the time out
   wait period. The stub function xTaskCheckForTimeOut used to model
   this wait time will be bounded to this define. */
#ifndef QUEUE_RECEIVE_BOUND
	#define QUEUE_RECEIVE_BOUND 4
#endif

/* If the item size is not bounded, the proof does not finish in a reasonable
   time due to the involved memcpy commands. */
#ifndef MAX_ITEM_SIZE
	#define MAX_ITEM_SIZE 20
#endif

QueueHandle_t xQueue;

/* This method is used to model side effects of concurrency.
   The initialization of pxTimeOut is not relevant for this harness. */
void vTaskInternalSetTimeOutState( TimeOut_t * const pxTimeOut ){
	__CPROVER_assert(__CPROVER_w_ok(&(pxTimeOut->xOverflowCount), sizeof(BaseType_t)), "pxTimeOut should be a valid pointer and xOverflowCount writable");
	__CPROVER_assert(__CPROVER_w_ok(&(pxTimeOut->xTimeOnEntering), sizeof(TickType_t)), "pxTimeOut should be a valid pointer and xTimeOnEntering writable");
	xQueue->uxMessagesWaiting = nondet_BaseType_t();
}

void harness(){
	vInitTaskCheckForTimeOut(0, QUEUE_RECEIVE_BOUND - 1);

	xQueue = xUnconstrainedQueueBoundedItemSize(MAX_ITEM_SIZE);


	TickType_t xTicksToWait;
	if(xState == taskSCHEDULER_SUSPENDED){
		xTicksToWait = 0;
	}

    if(xQueue){
		xQueue->cTxLock = LOCK_BOUND - 1;
		xQueue->cRxLock = LOCK_BOUND - 1;

    	void *pvBuffer = pvPortMalloc(xQueue->uxItemSize);
    	if(!pvBuffer){
    		xQueue->uxItemSize = 0;
    	}
    	xQueueReceive( xQueue, pvBuffer, xTicksToWait );
    }

}
