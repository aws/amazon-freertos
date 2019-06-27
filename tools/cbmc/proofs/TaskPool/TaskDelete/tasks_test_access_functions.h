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

	pxTCB->pxStack = ( StackType_t * ) pvPortMalloc( ( ( ( size_t ) STACK_DEPTH ) * sizeof( StackType_t ) ) );
	if ( pxTCB->pxStack == NULL )
	{
		vPortFree( pxTCB );
		return NULL;
	}

	__CPROVER_assume( ucStaticAllocationFlag <= tskSTATICALLY_ALLOCATED_STACK_AND_TCB );
	__CPROVER_assume( ucStaticAllocationFlag >= tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB );
	pxTCB->ucStaticallyAllocated = ucStaticAllocationFlag;

	return pxTCB;
}

/*
 * We set the values of relevant global
 * variables to nondeterministic values
 */
void vSetGlobalVariables()
{
	xSchedulerRunning = nondet();
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

	/*
	 * The task handle passed to TaskDelete can be NULL. In that case, the
	 * task to delete is the one in `pxCurrentTCB`, see the macro `prvGetTCBFromHandle`
	 * in line 1165 (tasks.c) for reference. For that reason, we provide a similar
	 * initialization for an arbitrary task `pxTCB` and `pxCurrentTCB`.
	 */

	pxTCB = xUnconstrainedTCB();
	if ( pxTCB != NULL )
	{
		/* 
		 * The `return pdFAIL` statement that follows can be uncommented
		 * to test coverage when the task handle passed to TaskDelete is NULL
		 */
		// return pdFAIL;
		if ( nondet_bool() )
		{
			TCB_t * pxOtherTCB;
			pxOtherTCB = xUnconstrainedTCB();
			/*
			 * Nondeterministic insertion of another TCB in the same list
			 * to guarantee coverage in line 1174 (tasks.c)
			 */
			if ( pxOtherTCB != NULL )
			{
				vListInsert( &xPendingReadyList, &( pxOtherTCB->xStateListItem ) );
			}
		}
		vListInsert( &xPendingReadyList, &( pxTCB->xStateListItem ) );

		/*
		 * Nondeterministic insertion of an event list item to guarantee
		 * coverage in lines 1180-1184 (tasks.c)
		 */
		if ( nondet_bool() )
		{
			vListInsert( pxDelayedTaskList, &( pxTCB->xEventListItem ) );
		}
	}

	/* Note that `*xTask = NULL` can happen here, but this is fine -- `pxCurrentTCB` will be used instead */
	*xTask = pxTCB;

	/*
	 * `pxCurrentTCB` must be initialized the same way as the previous task, but an
	 * allocation failure cannot happen in this case (i.e., if the previous task is NULL)
	 */
	pxCurrentTCB = xUnconstrainedTCB();
	if ( pxCurrentTCB == NULL )
	{
		return pdFAIL;
	}

	if ( nondet_bool() )
	{
		TCB_t * pxOtherTCB;
		pxOtherTCB = xUnconstrainedTCB();
		if ( pxOtherTCB != NULL )
		{
			vListInsert( &pxReadyTasksLists[ pxOtherTCB->uxPriority ], &( pxOtherTCB->xStateListItem ) );
		}
	}
	vListInsert( &pxReadyTasksLists[ pxCurrentTCB->uxPriority ], &( pxCurrentTCB->xStateListItem ) );
	
	/* Use of this macro ensures coverage on line 185 (list.c) */
	listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB , &pxReadyTasksLists[ pxCurrentTCB->uxPriority ] );

	if ( nondet_bool() )
	{
		vListInsert( pxDelayedTaskList, &( pxCurrentTCB->xEventListItem ) );
	}

	return pdPASS;
}
