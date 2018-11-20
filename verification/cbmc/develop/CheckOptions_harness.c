/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_TCP_WIN.h"

#include "cbmc.h"

// Used in specification and abstraction of CheckOptions inner and outer loops
// Given unconstrained values in harness
size_t buffer_size;
uint8_t *EthernetBuffer;

void prvCheckOptions(FreeRTOS_Socket_t *pxSocket,
		     NetworkBufferDescriptor_t *pxNetworkBuffer );

// Abstraction of CheckOptions outer loop used in proof of CheckOptions
// Loop variables passed by reference: VAL(var) is (*var).
enum CBMC_LOOP_CONDITION prvCheckOptions_outer(unsigned char *VAL(pucPtr),
					       unsigned char *VAL(pucLast),
					       UBaseType_t VAL(uxNewMSS),
					       FreeRTOS_Socket_t *VAL(pxSocket),
					       TCPWindow_t *VAL(pxTCPWindow))
{
  // CBMC pointer model (obviously true)
  __CPROVER_assume(buffer_size < CBMC_MAX_OBJECT_SIZE);

  // Preconditions

  // pucPtr is a valid pointer
  __CPROVER_assert(EthernetBuffer <= VAL(pucPtr) &&
		   VAL(pucPtr) < EthernetBuffer + buffer_size,
		   "pucPtr is a valid pointer");
  // pucLast is a valid pointer (or one past)
  __CPROVER_assert(EthernetBuffer <= VAL(pucLast) &&
		   VAL(pucLast) <= EthernetBuffer + buffer_size,
		   "pucLast is a valid pointer");
  // pucPtr is before pucLast
  __CPROVER_assert(VAL(pucPtr) < VAL(pucLast),
		   "pucPtr < pucLast");

  // Record initial values
  SAVE_OLDVAL(pucPtr, unsigned char *);
  SAVE_OLDVAL(pucLast, unsigned char *);

  // Model loop body
  size_t offset;
  enum CBMC_LOOP_CONDITION rc;
  VAL(pucPtr) += offset;

  // Postconditions

  // pucPtr advanced
  __CPROVER_assume(rc == CBMC_LOOP_BREAK || OLDVAL(pucPtr) < VAL(pucPtr));
  // pucLast unchanged
  __CPROVER_assume(VAL(pucLast) == OLDVAL(pucLast));
  // pucPtr <= pucLast
  __CPROVER_assume(VAL(pucPtr) <= OLDVAL(pucLast));

  return rc;
}

// Proof of CheckOptions
void harness()
{
  // Buffer can be any buffer of size at most BUFFER_SIZE
  size_t offset;
  uint8_t buffer[BUFFER_SIZE];
  buffer_size = BUFFER_SIZE - offset;
  EthernetBuffer = buffer + offset;

  // pxSocket can be any socket
  FreeRTOS_Socket_t pxSocket;

  // pxNetworkBuffer can be any buffer descriptor with any buffer
  NetworkBufferDescriptor_t pxNetworkBuffer;
  pxNetworkBuffer.pucEthernetBuffer = EthernetBuffer;
  pxNetworkBuffer.xDataLength = buffer_size;

  ////////////////////////////////////////////////////////////////
  // Specification and proof of CheckOptions

  // CBMC pointer model (obviously true)
  __CPROVER_assume(buffer_size < CBMC_MAX_OBJECT_SIZE);

  // Buffer overflow on pathologically small buffers
  // Must be big enough to hold pxTCPPacket and pxTCPHeader
  __CPROVER_assume(buffer_size > 47);

  // EthernetBuffer is a valid pointer (or one past when buffer_size==0)
  __CPROVER_assume(buffer <= EthernetBuffer &&
		   EthernetBuffer <= buffer + BUFFER_SIZE);

  // Loop variables passed by reference
  prvCheckOptions(&pxSocket, &pxNetworkBuffer);
}
