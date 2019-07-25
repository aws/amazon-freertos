/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

#define HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH    ( 26 )

/* stubbing library functions */
int snprintf( char * contentLengthHeaderStr, size_t len, const char * data,  const char * content, uint32_t contentLength) {
  int ret;
  __CPROVER_assume(ret >= 0 && ret <= HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH);
  return ret;
}

void harness() {
  IotHttpsConnectionHandle_t pConnHandle = newIotConnectionHandle();
  IotHttpsRequestHandle_t reqHandle = newIotRequestHandle();
  IotHttpsResponseHandle_t pRespHandle = newIotResponseHandle();
  IotHttpsResponseInfo_t *pRespInfo = newIotResponseInfo();
  uint32_t timeoutMs;
  IotHttpsClient_SendSync(pConnHandle, reqHandle, &pRespHandle, pRespInfo, timeoutMs);
}
