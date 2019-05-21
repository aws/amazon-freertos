#include "cbmc.h"

/* 
 * pvPortMalloc is nondeterministic by definition, thus we do not need
 * to check for NULL allocation in this function
 */
void vNondetSetCurrentTCB( void )
{
	pxCurrentTCB = pvPortMalloc( sizeof(TCB_t) );
}
/*
 * We just require task lists to be initialized for this proof
 */
void vPrepareTaskLists( void )
{
	__CPROVER_assert_zero_allocation();

	prvInitialiseTaskLists();
}

/*
 * We set the values of relevant global
 * variables to nondeterministic values
 */
void vSetGlobalVariables( void )
{
	xSchedulerRunning = nondet();
	uxCurrentNumberOfTasks = nondet();
}

/*
 * pvPortMalloc is nondeterministic by definition, thus we do not need
 * to check for NULL allocation in this function
 */
TaskHandle_t *pxNondetSetTaskHandle( void )
{
	TaskHandle_t *pxNondetTaskHandle = pvPortMalloc( sizeof(TaskHandle_t) );
	return pxNondetTaskHandle;
}

/*
 * Tries to allocate a string of size xStringLength and sets the string
 * to be terminated using a nondeterministic index if allocation was successful
 */
char *pcNondetSetString( size_t xStringLength )
{
	char *pcName = pvPortMalloc( xStringLength );

	if ( pcName != NULL ) {
		uint8_t uNondetIndex;
		__CPROVER_assume( uNondetIndex < xStringLength );
		pcName[uNondetIndex] = '\0';
	}

	return pcName;
}
