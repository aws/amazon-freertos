/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

/* We prove memory safety of IotHttpsClient_Disconnect separately. */
IotHttpsReturnCode_t IotHttpsClient_Disconnect(IotHttpsConnectionHandle_t connHandle) {}


void harness() {
  IotHttpsConnectionHandle_t pConnHandle = newIotConnectionHandle();
  IotHttpsConnectionInfo_t *pConnConfig = newConnectionInfo();
  IotHttpsClient_Connect(&pConnHandle, pConnConfig);
}
