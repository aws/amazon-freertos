/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS_Sockets.h"

/* FreeRTOS+TCP includes. */
#include "iot_https_client.h"
#include "iot_https_internal.h"

#include "../global_state_HTTP.c"

#ifndef HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH
#define HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH    ( 26 )
#endif

/****************************************************************
* Stub out snprintf so that it writes nothing but simply checks that
* the arguments are readable and writeable, and returns an
* unconstrained length.
*
* MacOS header file /usr/include/secure/_stdio.h defines snprintf to
* use a builtin function supported by CBMC, so we stub out the builtin
* snprintf instead of the standard snprintf.
****************************************************************/

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin___sprintf_chk)
int __builtin___snprintf_chk (char *buf, size_t size, int flag, size_t os,
			      const char *fmt, ...)
{
  int ret;
  __CPROVER_assert(__CPROVER_w_ok(buf, size), "sprintf output writeable");
  __CPROVER_assert(fmt, "sprintf format nonnull");
  __CPROVER_assume(ret >= 0 && ret <= HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH);
  return ret;
}

#else
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
  int ret;
  __CPROVER_assert(__CPROVER_w_ok(buf, size), "sprintf output writeable");
  __CPROVER_assert(fmt, "sprintf format nonnull");
  __CPROVER_assume(ret >= 0 && ret <= HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH);
  return ret;
}
#endif

void harness() {
  IotHttpsConnectionHandle_t pConnHandle = newIotConnectionHandle();
  IotHttpsRequestHandle_t reqHandle = newIotRequestHandle();
  IotHttpsResponseHandle_t pRespHandle = newIotResponseHandle();
  IotHttpsResponseInfo_t *pRespInfo = newIotResponseInfo();
  uint32_t timeoutMs;
  IotHttpsClient_SendSync(pConnHandle, reqHandle, &pRespHandle, pRespInfo, timeoutMs);
}
