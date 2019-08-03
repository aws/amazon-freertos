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
  /* This is done to insert two elements in respQ and get coverage in IotHttpsClient_Disconnect. */
  	_httpsResponse_t * p1Resp = malloc(sizeof(_httpsResponse_t));
  	_httpsResponse_t * p2Resp = malloc(sizeof(_httpsResponse_t));
  /* This function inserts an element in the respQ.*/
  	IotDeQueue_EnqueueTail(&(connHandle->respQ), &(p1Resp->link));
  	IotDeQueue_EnqueueTail(&(connHandle->respQ), &(p2Resp->link));
  }
  IotHttpsClient_Disconnect(connHandle);
}
