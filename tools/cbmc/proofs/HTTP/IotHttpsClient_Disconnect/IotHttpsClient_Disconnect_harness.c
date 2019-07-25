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
  /* This is done to insert two elements in reqQ and get coverage in IotHttpsClient_Disconnect. */
  	_httpsRequest_t * p1Req = malloc(sizeof(_httpsRequest_t));
  	p1Req->pHttpsResponse = malloc(sizeof(_httpsResponse_t));
  	_httpsRequest_t * p2Req = malloc(sizeof(_httpsRequest_t));
  	p2Req->pHttpsResponse = malloc(sizeof(_httpsResponse_t));
  /* This function inserts an element in the reqQ.*/
  	IotDeQueue_EnqueueTail(&(connHandle->reqQ), &(p1Req->link));
  	IotDeQueue_EnqueueTail(&(connHandle->reqQ), &(p2Req->link));
  }
  IotHttpsClient_Disconnect(connHandle);
}
