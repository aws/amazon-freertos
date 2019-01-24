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

// Refactored CheckOptions inner loop
// Loop variables passed by reference: VAL(var) is (*var).
void prvCheckOptions_inner(unsigned char *VAL(pucPtr),
			   FreeRTOS_Socket_t *VAL(pxSocket),
			   unsigned char VAL(len));

// Proof of CheckOptions inner loop
void harness()
{
  // Buffer can be any buffer of size at most BUFFER_SIZE
  size_t offset;
  uint8_t buffer[BUFFER_SIZE];
  buffer_size = BUFFER_SIZE - offset;
  EthernetBuffer = buffer + offset;

  // pucPtr can be any pointer into buffer
  size_t pucPtr_offset;
  unsigned char *pucPtr = EthernetBuffer + pucPtr_offset;

  // len can be any byte
  unsigned char len;

  // pxSocket can be any socket with some initialized values
  FreeRTOS_Socket_t xSocket;
  FreeRTOS_Socket_t *pxSocket = &xSocket;

  extern List_t xSegmentList;
  vListInitialise(&xSocket.u.xTCP.xTCPWindow.xWaitQueue);
  vListInitialise(&xSocket.u.xTCP.xTCPWindow.xTxSegments);
  vListInitialise(&xSocket.u.xTCP.xTCPWindow.xPriorityQueue);
  vListInitialise(&xSegmentList);
  StreamBuffer_t txStreamBuffer;
  xSocket.u.xTCP.txStream=&txStreamBuffer;

  // A singleton list
  TCPSegment_t segment_w;
  vListInitialiseItem(&segment_w.xQueueItem);
  vListInitialiseItem(&segment_w.xListItem);
  listSET_LIST_ITEM_OWNER(&segment_w.xQueueItem, (void *) &segment_w);
  vListInsertEnd(&xSocket.u.xTCP.xTCPWindow.xWaitQueue, &segment_w.xQueueItem);

  // A singleton list
  TCPSegment_t segment_t;
  vListInitialiseItem(&segment_t.xQueueItem);
  vListInitialiseItem(&segment_t.xListItem);
  listSET_LIST_ITEM_OWNER(&segment_t.xQueueItem, (void *) &segment_t);
  vListInsertEnd(&xSocket.u.xTCP.xTCPWindow.xTxSegments, &segment_t.xQueueItem);

  ////////////////////////////////////////////////////////////////
  // Specification and proof of CheckOptions inner loop

  // CBMC pointer model (obviously true)
  __CPROVER_assume(buffer_size < CBMC_MAX_OBJECT_SIZE);

  // Preconditions

  // pucPtr is a valid pointer
  __CPROVER_assume(EthernetBuffer <= pucPtr &&
  		   pucPtr < EthernetBuffer + buffer_size);
  // pucPtr + 8 is a valid pointer (xBytesRemaining > len)
  __CPROVER_assume(EthernetBuffer <= pucPtr + 8 &&
  		   pucPtr + 8 <= EthernetBuffer + buffer_size);
  // len >= 8 (while loop condition)
  __CPROVER_assume(len >= 8);

  // Record initial values
  SAVE_OLD(pucPtr, unsigned char*);
  SAVE_OLD(len, unsigned char);

  // Loop variables passed by reference
  prvCheckOptions_inner(&pucPtr, &pxSocket, &len);

  // Postconditions

  __CPROVER_assert(pucPtr == OLD(pucPtr) + 8, "pucPtr advanced");
  __CPROVER_assert(len == OLD(len) - 8, "len decremented");
  __CPROVER_assert(EthernetBuffer <= pucPtr &&
		   pucPtr <= EthernetBuffer + buffer_size,
		   "pucPtr a valid pointer");
}
