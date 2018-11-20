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

// Refactored CheckOptions outer loop
// Loop variables passed by reference: VAL(var) is (*var).
enum CBMC_LOOP_CONDITION prvCheckOptions_outer(unsigned char *VAL(pucPtr),
					       unsigned char *VAL(pucLast),
					       UBaseType_t VAL(uxNewMSS),
					       FreeRTOS_Socket_t *VAL(pxSocket),
					       TCPWindow_t *VAL(pxTCPWindow));

// Abstraction of CheckOptions inner loop used on proof of outer loop
// Loop variables passed by reference: VAL(var) is (*var).
void prvCheckOptions_inner(unsigned char *VAL(pucPtr),
			   FreeRTOS_Socket_t *VAL(pxSocket),
			   unsigned char VAL(len))
{
  // Preconditions

  // pucPtr is a valid pointer
  __CPROVER_assert(EthernetBuffer <= VAL(pucPtr) &&
		   VAL(pucPtr) < EthernetBuffer + buffer_size,
		   "pucPtr is a valid pointer");
  // pucPtr + 8 is a valid pointer
  __CPROVER_assert(EthernetBuffer <= VAL(pucPtr) + 8 &&
		   VAL(pucPtr) + 8 <= EthernetBuffer + buffer_size,
		   "pucPtr+8 is a valid pointer");
  // len >= 8
  __CPROVER_assert(VAL(len) >= 8, "len >= 8");

  // Record initial values
  SAVE_OLDVAL(pucPtr, unsigned char *);
  SAVE_OLDVAL(len, unsigned char);

  // Model loop body
  VAL(pucPtr) += 8;
  VAL(len) -= 8;

  // Postconditions

  __CPROVER_assume(VAL(pucPtr) == OLDVAL(pucPtr) + 8);
  __CPROVER_assume(VAL(len) == OLDVAL(len) - 8);
  __CPROVER_assume(EthernetBuffer <= VAL(pucPtr) &&
		   VAL(pucPtr) <= EthernetBuffer + buffer_size);
}

// Proof of CheckOptions outer loop
void harness()
{
  // Buffer can be any buffer of size at most BUFFER_SIZE
  size_t offset;
  uint8_t buffer[BUFFER_SIZE];
  buffer_size = BUFFER_SIZE - offset;
  EthernetBuffer = buffer + offset;

  // pucPtr and pucLast can be any pointers into buffer
  size_t pucPtr_offset;
  size_t pucLast_offset;
  unsigned char *pucPtr = EthernetBuffer + pucPtr_offset;
  unsigned char *pucLast = EthernetBuffer + pucLast_offset;

  // uxNewMSS can by any byte
  UBaseType_t uxNewMSS;

  // pxSocket can be any socket
  FreeRTOS_Socket_t xSocket;
  FreeRTOS_Socket_t *pxSocket = &xSocket;

  // pxTCPWindow can be any window (points into socket)
  TCPWindow_t *pxTCPWindow = &xSocket.u.xTCP.xTCPWindow;

  ////////////////////////////////////////////////////////////////
  // Specification and proof of CheckOptions outer loop

  // CBMC pointer model (obviously true)
  __CPROVER_assume(buffer_size < CBMC_MAX_OBJECT_SIZE);

  // Preconditions

  // pucPtr is a valid pointer
  __CPROVER_assume(EthernetBuffer <= pucPtr &&
		   pucPtr < EthernetBuffer + buffer_size);
  // pucLast is a valid pointer (or one past)
  __CPROVER_assume(EthernetBuffer <= pucLast &&
		   pucLast <= EthernetBuffer + buffer_size);
  // pucPtr is before pucLast
  __CPROVER_assume(pucPtr < pucLast);

  // Record initial values
  SAVE_OLD(pucPtr, uint8_t *);
  SAVE_OLD(pucLast, uint8_t *);

  // Loop variables passed by reference
  // Return value describes loop exit: break or continue
  enum CBMC_LOOP_CONDITION rc =
    prvCheckOptions_outer(&pucPtr, &pucLast, &uxNewMSS, &pxSocket,
			  &pxTCPWindow);

  // Postconditions
  __CPROVER_assert(rc == CBMC_LOOP_BREAK || OLD(pucPtr) < pucPtr,
		   "pucPtr advanced");
  __CPROVER_assert(pucLast == OLD(pucLast), "pucLast unchanged");
  __CPROVER_assert(pucPtr <= pucLast, "pucPtr <= pucLast");

}
