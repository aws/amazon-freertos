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
 * We set uxPendedTicks since __CPROVER_assume does not work
 * well with statically initialised variables
 */
void vSetGlobalVariables( void ) {
	UBaseType_t uxNonZeroValue;

	__CPROVER_assume( uxNonZeroValue != 0 );

	uxSchedulerSuspended = uxNonZeroValue;
	uxPendedTicks = nondet_bool() ? PENDED_TICKS : 0;
	uxCurrentNumberOfTasks = nondet();
	xTickCount = nondet();
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

	/* This task will be moved to a ready list, granting coverage
	 * on lines 2780-2786 (tasks.c) */
	pxTCB = xUnconstrainedTCB();
	if ( pxTCB == NULL )
	{
		return pdFAIL;
	}
	vListInsert( pxOverflowDelayedTaskList, &( pxTCB->xStateListItem ) );

	/* Use of this macro ensures coverage on line 185 (list.c) */
	listGET_OWNER_OF_NEXT_ENTRY( pxTCB , pxOverflowDelayedTaskList );

	pxTCB = xUnconstrainedTCB();
	if ( pxTCB == NULL )
	{
		return pdFAIL;
	}
	vListInsert( &xPendingReadyList, &( pxTCB->xStateListItem ) );
	vListInsert( pxOverflowDelayedTaskList, &( pxTCB->xEventListItem ) );

	pxTCB = xUnconstrainedTCB();
	if ( pxTCB == NULL )
	{
		return pdFAIL;
	}
	vListInsert( pxOverflowDelayedTaskList, &( pxTCB->xStateListItem ) );

	/* This nondeterministic choice ensure coverage in line 2746 (tasks.c) */
	if ( nondet_bool() )
	{
		vListInsert( pxOverflowDelayedTaskList, &( pxTCB->xEventListItem ) );
	}

	pxCurrentTCB = xUnconstrainedTCB();
	if ( pxCurrentTCB == NULL )
	{
		return pdFAIL;
	}
	vListInsert( &pxReadyTasksLists[ pxCurrentTCB->uxPriority ], &( pxCurrentTCB->xStateListItem ) );

	return pdPASS;
}
