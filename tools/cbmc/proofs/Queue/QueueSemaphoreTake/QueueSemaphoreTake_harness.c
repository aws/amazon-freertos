#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"
#include "tasksStubs.h"
#include "cbmc.h"

/* prvUnlockQueue is going to decrement this value to 0 in the loop.
    We need a bound for the loop. Using 5 has a reasonable performance resulting
    in 4 unwinding iterations of the loop. The loop is mostly modifying a
    data structure in task.c that is not in the scope of the proof. */
#ifndef PRV_UNLOCK_UNWINDING_BOUND
	#define PRV_UNLOCK_UNWINDING_BOUND 5
#endif

BaseType_t state;
QueueHandle_t xQueue;
BaseType_t counter;

BaseType_t xTaskGetSchedulerState(void)
{
	return state;
}

void vTaskInternalSetTimeOutState( TimeOut_t * const pxTimeOut )
{
	/* QueueSemaphoreTake might be blocked to wait for
	   another process to release a token to the semaphore.
	   This is currently not in the CBMC model. Anyhow,
	   vTaskInternalSetTimeOutState is set a timeout for
	   QueueSemaphoreTake operation. We use this to model a successful
	   release during wait time. */
	UBaseType_t bound;
	__CPROVER_assume((bound >= 0 && xQueue->uxLength >= bound));
	xQueue->uxMessagesWaiting = bound;
}

void harness()
{
	/* Init task stub to make sure that the third loop iteration
	simulates a time out */
	vInitTaskCheckForTimeOut(0, 3);

	xQueue = xUnconstrainedMutex();
	TickType_t xTicksToWait;

	if(state == taskSCHEDULER_SUSPENDED){
		xTicksToWait = 0;
	}
	if (xQueue) {
		/* Bounding the loop in prvUnlockQueue to PRV_UNLOCK_UNWINDING_BOUND
		   As the loop is not relevant in this proof the value
		   might be set to any positive 8-bit integer value. We subtract one,
		   because the bound must be one greater
		   than the amount of loop iterations. */
		__CPROVER_assert(PRV_UNLOCK_UNWINDING_BOUND > 0, "Make sure, a valid macro value is chosen.");
		xQueue->cTxLock = PRV_UNLOCK_UNWINDING_BOUND - 1;
		xQueue->cRxLock = PRV_UNLOCK_UNWINDING_BOUND - 1;
		((&(xQueue->xTasksWaitingToReceive))->xListEnd).pxNext->xItemValue = nondet();

		/* This assumptions is required to prevent an overflow in l. 2057 of queue.c
		   in the prvGetDisinheritPriorityAfterTimeout() function. */
		__CPROVER_assume( (
		  ( UBaseType_t ) listGET_ITEM_VALUE_OF_HEAD_ENTRY( &( xQueue->xTasksWaitingToReceive ) )
		   <= ( ( UBaseType_t ) configMAX_PRIORITIES)));
		xQueueSemaphoreTake(xQueue, xTicksToWait);
	}
}
