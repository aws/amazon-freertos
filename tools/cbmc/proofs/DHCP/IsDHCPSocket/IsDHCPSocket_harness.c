#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCP.h"

/*
 * The harness test proceeds to call IsDHCPSocket with an unconstrained value
 */
void harness()
{
  Socket_t xSocket;
  BaseType_t xResult;
  
  xResult = xIsDHCPSocket( xSocket );
}
