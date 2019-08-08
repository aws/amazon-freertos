/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

void harness() {
  IotHttpsConnectionHandle_t connHandle = allocate_IotConnectionHandle();
  if(connHandle) {
    __CPROVER_assume(is_valid_IotConnectionHandle(connHandle));
    __CPROVER_assume(is_stubbed_NetworkInterface(connHandle->pNetworkInterface));

    /* initialize connHandle->respQ to a list of two responses */
    _httpsResponse_t * p1Resp = malloc(sizeof(_httpsResponse_t));
    _httpsResponse_t * p2Resp = malloc(sizeof(_httpsResponse_t));
    IotDeQueue_EnqueueTail(&(connHandle->respQ), &(p1Resp->link));
    IotDeQueue_EnqueueTail(&(connHandle->respQ), &(p2Resp->link));
  }
  IotHttpsClient_Disconnect(connHandle);
}
