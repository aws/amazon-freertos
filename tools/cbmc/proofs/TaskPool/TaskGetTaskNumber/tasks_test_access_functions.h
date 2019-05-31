#include "cbmc.h"

/*
 * We allocate a TCB and set some members to basic values
 */
TaskHandle_t xUnconstrainedTCB( void )
{
  TCB_t * pxTCB = pvPortMalloc(sizeof(TCB_t));

  if ( pxTCB == NULL )
    return NULL;

  return pxTCB;
}

/*
 * We try to allocate an unconstrained TCB but do not
 * check for NULL errors here (the function does it)
 */
void vPrepareTask( TaskHandle_t * xTask )
{
  __CPROVER_assert_zero_allocation();

  *xTask = xUnconstrainedTCB();
}
