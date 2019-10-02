#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DHCP.h"

/*
 * CBMC automatically unwinds strlen on a fixed string
 */
const char * pcApplicationHostnameHook(void) {
  return "hostname";
}

/*
 * This stub allows us to overcome the unwinding error obtained
 * in the do-while loop within function prvCreatePartDHCPMessage.
 * The behaviour is similar to the original function, but failed allocations
 * are not considered here (this is a safe assumption that avoids the error)
 */
void *FreeRTOS_GetUDPPayloadBuffer( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks )
{
  NetworkBufferDescriptor_t xNetworkBuffer;
  void *pvReturn;

  xNetworkBuffer.xDataLength = ipUDP_PAYLOAD_OFFSET_IPv4 + xRequestedSizeBytes;
  xNetworkBuffer.pucEthernetBuffer = malloc( xNetworkBuffer.xDataLength );
  pvReturn = (void *) &( xNetworkBuffer.pucEthernetBuffer[ ipUDP_PAYLOAD_OFFSET_IPv4 ] );
  return pvReturn;
}

/*
 * We stub out FreeRTOS_recvfrom to do nothing but return a buffer of
 * arbitrary size (but size at most BUFFER_SIZE) containing arbitrary
 * data.  We need to bound the size of the buffer in order to bound
 * the number of iterations of the loop prvProcessDHCPReplies.0 that
 * iterates over the buffer contents.  The bound BUFFER_SIZE is chosen
 * to be large enough to ensure complete code coverage, and small
 * enough to ensure CBMC terminates within a reasonable amount of
 * time.
 */
int32_t FreeRTOS_recvfrom(
  Socket_t xSocket, void *pvBuffer, size_t xBufferLength,
  BaseType_t xFlags, struct freertos_sockaddr *pxSourceAddress,
  socklen_t *pxSourceAddressLength )
{
  __CPROVER_assert(xFlags & FREERTOS_ZERO_COPY, "I can only do ZERO_COPY");

  size_t xBufferSize;
  /* A DHCP message (min. size 241B) is preceded by the IP buffer padding (10B) and the UDP payload (42B) */
  __CPROVER_assume(xBufferSize >= ipBUFFER_PADDING + ipUDP_PAYLOAD_OFFSET_IPv4);
  /* The last field of a DHCP message (Options) is variable in length, but 6 additional bytes are enough */
  /* to obtain maximum coverage with this proof. Hence, we have BUFFER_SIZE=299 */
  __CPROVER_assume(xBufferSize <= BUFFER_SIZE);

  /* The buffer gets allocated and we set the pointer past the UDP payload (i.e., start of DHCP message) */
  *((char **)pvBuffer) = malloc(xBufferSize) + ipBUFFER_PADDING + ipUDP_PAYLOAD_OFFSET_IPv4;

  return xBufferSize - ipUDP_PAYLOAD_OFFSET_IPv4 - ipBUFFER_PADDING;
}

/*
 * The harness test proceeds to call DHCPProcess with an unconstrained value
 */
void harness()
{
  BaseType_t xReset;
  vDHCPProcess( xReset );
}
