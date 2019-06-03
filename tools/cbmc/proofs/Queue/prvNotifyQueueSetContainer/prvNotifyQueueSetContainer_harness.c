#include "FreeRTOS.h"
#include "queue.h"
#include "queue_datastructure.h"
#include "cbmc.h"

#ifndef LOCK_BOUND
	#define LOCK_BOUND 4
#endif

BaseType_t prvNotifyQueueSetContainer( const Queue_t * const pxQueue, const BaseType_t xCopyPosition );

BaseType_t prvCopyDataToQueue( Queue_t * const pxQueue, const void *pvItemToQueue, const BaseType_t xPosition )
{
	if(pxQueue->uxItemSize > ( UBaseType_t ) 0)
	{
		__CPROVER_assert(__CPROVER_r_ok(pvItemToQueue, ( size_t ) pxQueue->uxItemSize), "pvItemToQueue region must be readable");
		if(xPosition == queueSEND_TO_BACK){
			__CPROVER_assert(__CPROVER_w_ok(( void * ) pxQueue->pcWriteTo, ( size_t ) pxQueue->uxItemSize), "pxQueue->pcWriteTo region must be writable");
		}else{
			__CPROVER_assert(__CPROVER_w_ok(( void * ) pxQueue->u.xQueue.pcReadFrom, ( size_t ) pxQueue->uxItemSize), "pxQueue->u.xQueue.pcReadFrom region must be writable");
		}
		return pdFALSE;
	}else
	{
		return nondet_BaseType_t();
	}
}

QueueSetHandle_t xUnconstrainedQueueSet()
{
	UBaseType_t uxEventQueueLength = 2;
	QueueSetHandle_t xSet = xQueueCreateSet(uxEventQueueLength);
	if( xSet )
	{
		xSet->cTxLock = nondet_int8_t();
		xSet->cRxLock = nondet_int8_t();
		xSet->uxMessagesWaiting = nondet_UBaseType_t();
		xSet->xTasksWaitingToReceive.uxNumberOfItems = nondet_UBaseType_t();
		xSet->xTasksWaitingToSend.uxNumberOfItems = nondet_UBaseType_t();
	}
	return xSet;
}

void harness(){
	UBaseType_t uxQueueLength;
	UBaseType_t uxItemSize;
	uint8_t ucQueueType;
	__CPROVER_assume(uxQueueLength > 0);
	__CPROVER_assume(uxItemSize < 10);
	/* The implicit assumption for the QueueGenericCreate method is,
	   that there are no overflows in the computation and the inputs are safe.
	   There is no check for this in the code base */
	UBaseType_t upper_bound = portMAX_DELAY - sizeof(Queue_t);
	__CPROVER_assume(uxItemSize < (upper_bound)/uxQueueLength);
	QueueHandle_t xQueue =
		xQueueGenericCreate(uxQueueLength, uxItemSize, ucQueueType);
	if( xQueue ){
		xQueueAddToSet(xQueue, xUnconstrainedQueueSet());
		if(xQueue->pxQueueSetContainer) {
			__CPROVER_assume(xQueue->pxQueueSetContainer->uxMessagesWaiting < xQueue->pxQueueSetContainer->uxLength);
			BaseType_t xCopyPosition = nondet_BaseType_t();
			prvNotifyQueueSetContainer(xQueue, xCopyPosition );
		}
	}
}
