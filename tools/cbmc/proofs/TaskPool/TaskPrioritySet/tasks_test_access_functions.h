#include "cbmc.h"

/*
 * We allocate a TCB and set some members to basic values
 */
TaskHandle_t xUnconstrainedTCB( void )
{
	TCB_t * pxTCB = pvPortMalloc(sizeof(TCB_t));
	uint8_t ucStaticAllocationFlag;

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
 * We initialise and fill the task lists so coverage is optimal.
 * This initialization is not guaranteed to be minimal, but it
 * is quite efficient and it serves the same purpose
 */
BaseType_t xPrepareTaskLists( TaskHandle_t * xTask )
{
	TCB_t * pxTCB = NULL;

	__CPROVER_assert_zero_allocation();

	prvInitialiseTaskLists();

	pxTCB = xUnconstrainedTCB();

	/* Nondet. insertion of another task in the same list to allow both cases in line 1656 (tasks.c) */
	if ( nondet_bool() )
	{
		TCB_t * pxOtherTCB;
		pxOtherTCB = xUnconstrainedTCB();
		if ( pxOtherTCB != NULL )
		{
			vListInsert( &pxReadyTasksLists[ pxOtherTCB->uxPriority ], &( pxOtherTCB->xStateListItem ) );
		}
	}

	if ( pxTCB != NULL )
	{
		/* 
		 * The `return pdFAIL` statement that follows can be uncommented
		 * to test coverage when the task handle passed to TaskPrioritySet is NULL
		 */
		// return pdFAIL;
		if ( nondet_bool() )
		{
			/* Nondet. insertion of this task in ready tasks list to allow both cases in line 1651 (tasks.c) */
			vListInsert( &pxReadyTasksLists[ pxTCB->uxPriority ], &( pxTCB->xStateListItem ) );
		}
	}

	/* Note that `*xTask = NULL` can happen here, but this is fine -- `pxCurrentTCB` will be used instead */
	*xTask = pxTCB;

	pxCurrentTCB = xUnconstrainedTCB();
	if ( pxCurrentTCB == NULL )
	{
		return pdFAIL;
	}

	/* Nondet. insertion of this task in ready tasks list to allow both cases in line 1651 (tasks.c) */
	if ( nondet_bool() )
	{
		vListInsert( &pxReadyTasksLists[ pxCurrentTCB->uxPriority ], &( pxCurrentTCB->xStateListItem ) );

		/* Use of this macro ensures coverage on line 185 (list.c) */
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB , &pxReadyTasksLists[ pxCurrentTCB->uxPriority ] );
	}

	return pdPASS;
}
