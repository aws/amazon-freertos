#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

// The harness test proceeds to call SendEventToIPTask with an unconstrained value
void harness()
{
  eIPEvent_t eEvent;
  xSendEventToIPTask( eEvent );
}
