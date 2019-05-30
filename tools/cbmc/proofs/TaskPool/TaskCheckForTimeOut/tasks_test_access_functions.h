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
 * We just need to allocate a totally unconstrained TCB
 */
BaseType_t xPrepareCurrentTCB( void )
{
  __CPROVER_assert_zero_allocation();

  pxCurrentTCB = xUnconstrainedTCB();
  if ( pxCurrentTCB == NULL )
  {
    return pdFAIL;
  }

  return pdPASS;
}
