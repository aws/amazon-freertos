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

void prvCheckOptions( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer );

void harness() {
  FreeRTOS_Socket_t pxSocket;
  vListInitialise(&pxSocket.u.xTCP.xTCPWindow.xWaitQueue);
  vListInitialise(&pxSocket.u.xTCP.xTCPWindow.xTxSegments);

  ListItem_t item1;
  vListInitialiseItem(&item1);
  vListInsert(&pxSocket.u.xTCP.xTCPWindow.xWaitQueue, &item1);

  ListItem_t item2;
  vListInitialiseItem(&item2);
  vListInsert(&pxSocket.u.xTCP.xTCPWindow.xTxSegments, &item2);

  NetworkBufferDescriptor_t pxNetworkBuffer;
  unsigned buffer_size; //sizeof(TCPPacket_t);
  __CPROVER_assume(buffer_size>=
		   ipSIZE_OF_ETH_HEADER
		   + ipSIZE_OF_IPv4_HEADER
		   + ipSIZE_OF_TCP_HEADER);
  uint8_t *EthernetBuffer = (uint8_t *)malloc(buffer_size);
  pxNetworkBuffer.pucEthernetBuffer = EthernetBuffer;
  pxNetworkBuffer.xDataLength = buffer_size;

  prvCheckOptions(&pxSocket, &pxNetworkBuffer);
}
