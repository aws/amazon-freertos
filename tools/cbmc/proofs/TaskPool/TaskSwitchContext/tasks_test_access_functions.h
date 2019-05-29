#include "cbmc.h"

/*
 * We allocate a TCB and set some members to basic values
 */
TaskHandle_t xUnconstrainedTCB( UBaseType_t uxPriority )
{
	TCB_t * pxTCB = pvPortMalloc(sizeof(TCB_t));

	if ( pxTCB == NULL )
		return NULL;

	/* uxPriority is set to a specific priority */
	pxTCB->uxPriority = uxPriority;

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
 * We set the values of relevant global
 * variables to nondeterministic values
 */
void vSetGlobalVariables( void )
{
	uxSchedulerSuspended = nondet();
}

/*
 * We initialize and fill with one item each ready tasks list
 * so that the assertion on line 175 (tasks.c) does not fail
 */
BaseType_t xPrepareTaskLists( void )
{
	TCB_t * pxTCB = NULL;

	__CPROVER_assert_zero_allocation();

	prvInitialiseTaskLists();

	for ( int i = 0; i < configMAX_PRIORITIES; ++i )
	{
		pxTCB = xUnconstrainedTCB( i );
		if ( pxTCB == NULL )
		{
			return pdFAIL;
		}
		vListInsert( &pxReadyTasksLists[ pxTCB->uxPriority ], &( pxTCB->xStateListItem ) );
	}
	listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &pxReadyTasksLists[ configMAX_PRIORITIES - 1 ] );

	return pdPASS;
}
