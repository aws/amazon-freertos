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
  	uint32_t * pContentLength = malloc(sizeof(uint32_t));
  	IotHttpsClient_ReadContentLength(respHandle, pContentLength);
}
