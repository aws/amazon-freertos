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

/****************************************************************/
// Following definitions taken from FreeRTOS_DHCP.c

BaseType_t prvProcessDHCPReplies( BaseType_t xExpectedMessageType );

typedef enum
  {
   eWaitingSendFirstDiscover = 0,
   /* Initial state.  Send a discover the first time it is called, and
      reset all timers. */
   eWaitingOffer,
   /* Either resend the discover, or, if the offer is forthcoming,
      send a request. */
   eWaitingAcknowledge,
   /* Either resend the request. */
#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
   eGetLinkLayerAddress,
   /* When DHCP didn't respond, try to obtain a LinkLayer address
      168.254.x.x. */
#endif
   eLeasedAddress,
   /* Resend the request at the appropriate time to renew the lease. */
   eNotUsingLeasedAddress
   /* DHCP failed, and a default IP address is being used. */
  } eDHCPState_t;

struct xDHCP_DATA
{
  uint32_t ulTransactionId;
  uint32_t ulOfferedIPAddress;
  uint32_t ulDHCPServerAddress;
  uint32_t ulLeaseTime;
  /* Hold information on the current timer state. */
  TickType_t xDHCPTxTime;
  TickType_t xDHCPTxPeriod;
  /* Try both without and with the broadcast flag */
  BaseType_t xUseBroadcast;
  /* Maintains the DHCP state machine state. */
  eDHCPState_t eDHCPState;
  /* The UDP socket used for all incoming and outgoing DHCP traffic. */
  Socket_t xDHCPSocket;
};

typedef struct xDHCP_DATA DHCPData_t;

/****************************************************************/
// Harness

extern DHCPData_t xDHCPData;

struct hostent FAR * PASCAL FAR gethostbyname(_In_z_ const char FAR * name) {};


void harness()
{
  uint32_t ulTransactionId;
  uint32_t ulOfferedIPAddress;
  uint32_t ulDHCPServerAddress;
  uint32_t ulLeaseTime;
  TickType_t xDHCPTxTime;
  TickType_t xDHCPTxPeriod;
  BaseType_t xUseBroadcast;
  eDHCPState_t eDHCPState;
  FreeRTOS_Socket_t xSocket;
  // xSocket.xEventGroup = xEventGroupCreate();
  vListInitialise(&xSocket.u.xUDP.xWaitingPacketsList);
  ListItem_t xListItem;
  vListInitialiseItem(&xListItem);
  vListInsert(
		 &xSocket.u.xUDP.xWaitingPacketsList,
		 &xListItem);
  DHCPData_t lxDHCPData =
    {
     .ulTransactionId = ulTransactionId,
     .ulOfferedIPAddress = ulOfferedIPAddress,
     .ulDHCPServerAddress = ulDHCPServerAddress,
     .ulLeaseTime = ulLeaseTime,
     .xDHCPTxTime = xDHCPTxTime,
     .xDHCPTxPeriod = xDHCPTxPeriod,
     .xUseBroadcast = xUseBroadcast,
     .eDHCPState = eDHCPState,
     .xDHCPSocket = &xSocket
    };
  xDHCPData = lxDHCPData;
  BaseType_t arg;
  prvProcessDHCPReplies(arg);
}
