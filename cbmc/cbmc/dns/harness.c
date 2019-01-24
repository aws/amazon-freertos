/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "IPTraceMacroDefaults.h"

uint32_t prvParseDNSReply(uint8_t *pucUDPPayloadBuffer,
			  size_t xBufferLength,
			  TickType_t xIdentifier);

#define SIZE 100

void harness() {
  uint8_t buffer[SIZE];
  size_t len;
  TickType_t id;

  __CPROVER_assume(len <= SIZE);

  prvParseDNSReply(&buffer, len, id);
}
