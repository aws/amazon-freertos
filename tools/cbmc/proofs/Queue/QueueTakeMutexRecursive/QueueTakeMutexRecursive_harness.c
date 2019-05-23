#include "FreeRTOS.h"
#include "queue.h"
#include "tasksStubs.h"
#include "queue_datastructure.h"
#include "cbmc.h"

/* prvUnlockQueue is going to decrement this value to 0 in the loop.
   We need a bound for the loop. Using 5 has a reasonable performance resulting
   in 4 unwinding iterations of the loop. The loop is mostly modifying a
   data structure in task.c that is not in the scope of the proof. */
#ifndef PRV_UNLOCK_UNWINDING_BOUND
	#define PRV_UNLOCK_UNWINDING_BOUND 5
#endif

/* Bound for the timeout cycles */
#ifndef QueueSemaphoreTake_BOUND
	#define QueueSemaphoreTake_BOUND 3
#endif

QueueHandle_t xMutex;

void vTaskInternalSetTimeOutState( TimeOut_t * const pxTimeOut )
{
	/* QueueSemaphoreTake might be blocked to wait for
	   another process to release a token to the semaphore.
	   This is currently not in the CBMC model. Anyhow,
	   vTaskInternalSetTimeOutState is set a timeout for
	   QueueSemaphoreTake operation. We use this to model a successful
	   release during wait time. */
	UBaseType_t bound;
	__CPROVER_assume((bound >= 0 && xMutex->uxLength >= bound));
	xMutex->uxMessagesWaiting = bound;
}

BaseType_t xTaskGetSchedulerState( void ) {
	BaseType_t ret;
	__CPROVER_assume(ret != taskSCHEDULER_SUSPENDED);
	return ret;
}

void harness() {
	uint8_t ucQueueType;
	xMutex = xQueueCreateMutex(ucQueueType);
	TickType_t xTicksToWait;

	/* Init task stub to make sure that the QueueSemaphoreTake_BOUND - 1
	   loop iteration simulates a time out */
 	vInitTaskCheckForTimeOut(0, QueueSemaphoreTake_BOUND - 1);

	if(xMutex){
		xMutex->cTxLock = PRV_UNLOCK_UNWINDING_BOUND - 1;
		xMutex->cRxLock = PRV_UNLOCK_UNWINDING_BOUND - 1;
		xMutex->uxMessagesWaiting = nondet_UBaseType_t();
		xQueueTakeMutexRecursive(xMutex, xTicksToWait);
	}
}
