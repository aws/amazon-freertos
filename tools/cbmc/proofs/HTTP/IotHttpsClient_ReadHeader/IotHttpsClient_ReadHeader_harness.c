/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

void harness() {
	IotHttpsResponseHandle_t respHandle = newIotResponseHandle();
  size_t pName_len;
  __CPROVER_assume(pName_len >= 0 && pName_len <= IOT_HTTPS_MAX_HOST_NAME_LENGTH);
  char *pName = safeMalloc(pName_len);
  uint32_t len;
  __CPROVER_assume(len >= 0 && len <= MAX_ACCEPTED_SIZE);
  char  *pValue = safeMalloc(len);
  if (respHandle) {
    /* We need to bound respHandle->readHeaderValueLength in order to terminate strncpy */
    __CPROVER_assume(respHandle->readHeaderValueLength >= 0 && respHandle->readHeaderValueLength <= MAX_ACCEPTED_SIZE + 1);
  }
  IotHttpsClient_ReadHeader(respHandle, pName, pValue, len);
}
