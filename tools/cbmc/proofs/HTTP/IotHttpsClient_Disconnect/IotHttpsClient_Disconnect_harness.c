/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

void harness() {
  IotHttpsConnectionHandle_t connHandle = newIotConnectionHandle();
  if(connHandle) {
  /* This is done to insert an element in reqQ and get coverage in IotHttpsClient_Disconnect. */
  	_httpsRequest_t * pReq = malloc(sizeof(_httpsRequest_t));
  	pReq->pHttpsResponse = malloc(sizeof(_httpsResponse_t));
  	IotLink_t * pLink = pReq;
  /* This function inserts the element in the reqQ.*/
  	IotDeQueue_EnqueueTail(&(connHandle->reqQ), pLink);
  }
  IotHttpsClient_Disconnect(connHandle);
}
