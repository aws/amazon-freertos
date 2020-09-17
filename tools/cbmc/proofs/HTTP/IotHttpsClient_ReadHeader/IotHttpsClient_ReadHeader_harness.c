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
  if (respHandle) {
    __CPROVER_assume(is_valid_IotResponseHandle(respHandle));
    // Until a CBMC issue is addressed
    // https://github.com/diffblue/cbmc/issues/5004
    // copy an assumption from is_valid above as a simple assignment below.
    respHandle->httpParserInfo.readHeaderParser.data = respHandle;
  }
  initialize_IotResponseHandle(respHandle);

  uint32_t nameLen;
  uint32_t valueLen;
  __CPROVER_assume(nameLen < CBMC_MAX_OBJECT_SIZE);
  __CPROVER_assume(valueLen < CBMC_MAX_OBJECT_SIZE);
  char *pName = safeMalloc(nameLen);
  char *pValue = safeMalloc(valueLen);

  IotHttpsClient_ReadHeader(respHandle, pName, nameLen, pValue, valueLen);
}
