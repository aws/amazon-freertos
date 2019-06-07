#include "cbmc.h"

/*
 * We allocate a TCB and set some members to basic values
 */
TaskHandle_t xUnconstrainedTCB( void )
{
	TCB_t * pxTCB = pvPortMalloc(sizeof(TCB_t));

	if ( pxTCB == NULL )
		return NULL;

	__CPROVER_assume( pxTCB->uxPriority < configMAX_PRIORITIES );

	vListInitialiseItem( &( pxTCB->xStateListItem ) );
	vListInitialiseItem( &( pxTCB->xEventListItem ) );

	listSET_LIST_ITEM_OWNER( &( pxTCB->xStateListItem ), pxTCB );
	listSET_LIST_ITEM_OWNER( &( pxTCB->xEventListItem ), pxTCB );

	if ( nondet_bool() )
	{
		listSET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ), pxTCB->uxPriority );
	}
	else
	{
		listSET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ), portMAX_DELAY );
	}

	if ( nondet_bool() )
	{
		listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) pxTCB->uxPriority );
	}
	else
	{
		listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), portMAX_DELAY );
	}
	return pxTCB;
}

/*
 * We set the values of relevant global variables to
 * nondeterministic values, except for `uxSchedulerSuspended`
 * which is set to 0 (to pass through the assertion)
 */
 
void vSetGlobalVariables( void )
{
	uxSchedulerSuspended = pdFALSE;
	xTickCount = nondet();
	xNextTaskUnblockTime = nondet();
}

/*
 * We initialise and fill the task lists so coverage is optimal.
 * This initialization is not guaranteed to be minimal, but it
 * is quite efficient and it serves the same purpose
 */
BaseType_t xPrepareTaskLists( void )
{
	TCB_t * pxTCB = NULL;

	__CPROVER_assert_zero_allocation();

	prvInitialiseTaskLists();

	/* The current task will be moved to the delayed list */
	pxCurrentTCB = xUnconstrainedTCB();
	if ( pxCurrentTCB == NULL )
	{
		return pdFAIL;
	}
	vListInsert( &pxReadyTasksLists[ pxCurrentTCB->uxPriority ], &( pxCurrentTCB->xStateListItem ) );

	/*
	 * Nondeterministic insertion of a task in the ready tasks list
	 * guarantees coverage in line 5104 (tasks.c)
	 */
	if ( nondet_bool() )
	{
		pxTCB = xUnconstrainedTCB();
		if ( pxTCB == NULL )
		{
			return pdFAIL;
		}
		vListInsert( &pxReadyTasksLists[ pxTCB->uxPriority ], &( pxTCB->xStateListItem ) );
		
		/* Use of this macro ensures coverage on line 185 (list.c) */
		listGET_OWNER_OF_NEXT_ENTRY( pxTCB , &pxReadyTasksLists[ pxTCB->uxPriority ] );
	}
	

	return pdPASS;
}

/*
 * We stub out `xTaskResumeAll` including the assertion and change on
 * variables `uxSchedulerSuspended`. We assume that `xPendingReadyList`
 * is empty to avoid the first loop, and uxPendedTicks to avoid the second
 * one. Finally, we return a nondeterministic value (overapproximation)
 */
BaseType_t xTaskResumeAllStub( void )
{
	BaseType_t xAlreadyYielded;

	configASSERT( uxSchedulerSuspended );

	taskENTER_CRITICAL();
	{
		--uxSchedulerSuspended;
		__CPROVER_assert( listLIST_IS_EMPTY( &xPendingReadyList ), "Pending ready tasks list not empty." );
		__CPROVER_assert( uxPendedTicks == 0 , "uxPendedTicks is not equal to zero.");
	}
	taskEXIT_CRITICAL();

	return xAlreadyYielded;
}
