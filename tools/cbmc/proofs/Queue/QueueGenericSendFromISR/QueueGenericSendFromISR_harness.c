#include "FreeRTOS.h"
#include "queue.h"
#include "queue_init.h"

#include "cbmc.h"

#ifndef ITEM_BOUND
	#define ITEM_BOUND 10
#endif

#if( configUSE_QUEUE_SETS == 0 )
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
#endif

void harness(){
	QueueHandle_t xQueue = xUnconstrainedQueueBoundedItemSize(ITEM_BOUND);


	if( xQueue ){
		void *pvItemToQueue = pvPortMalloc(xQueue->uxItemSize);
		BaseType_t *xHigherPriorityTaskWoken = pvPortMalloc(sizeof(BaseType_t));
		BaseType_t xCopyPosition;
		if(xQueue->uxItemSize == 0)
		{
			/* uxQueue->xQueueType is a pointer to the head of the queue storage area.
			   If an item has a sice, this pointer must not be modified after init.
			   Otherwise some of the write statements will fail. */
			xQueue->uxQueueType = nondet_int8_t();
			pvItemToQueue = 0;
		}
		/* This code checks explicitly for violations of the pxQueue->uxMessagesWaiting < pxQueue->uxLength
		   invariant. */
		xQueue->uxMessagesWaiting = nondet_UBaseType_t();
		if(!pvItemToQueue){
			xQueue->uxItemSize = 0;
		}
		if(xCopyPosition == 2 ){
			__CPROVER_assume(xQueue->uxLength == 1);
		}
		xQueueGenericSendFromISR( xQueue, pvItemToQueue, xHigherPriorityTaskWoken, xCopyPosition );
	}
}
