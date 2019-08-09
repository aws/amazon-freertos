/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

void harness() {
  IotHttpsResponseHandle_t respHandle = allocate_IotResponseHandle();
  size_t nameLen;
  size_t valueLen;
  char *pName = safeMalloc(nameLen);
  char  *pValue = safeMalloc(valueLen);
  initialize_IotResponseHandle(respHandle);
  if (respHandle) {
    __CPROVER_assume(is_valid_IotResponseHandle(respHandle));
    // ???: replace assumption above with initialization
    respHandle->httpParserInfo.readHeaderParser.data = respHandle;
  }
  __CPROVER_assume(0 < valueLen && valueLen < UINT32_MAX);
  IotHttpsClient_ReadHeader(respHandle, pName, nameLen, pValue, valueLen);
}
