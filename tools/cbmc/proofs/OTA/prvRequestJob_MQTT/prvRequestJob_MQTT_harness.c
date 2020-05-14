/* Standard includes. */
#include <stdio.h>

/* prvRequestJob_Mqtt includes. */
#include "aws_iot_ota_agent.h"          /* OTA_Err_t */
#include "aws_iot_ota_agent_internal.h" /* OTA_AgentContext_t */
#include "iot_mqtt_types.h"             /* IotMqttError_t */

IotMqttError_t IotMqtt_TimedPublish(IotMqttConnection_t mqttConnection, const IotMqttPublishInfo_t *pPublishInfo, uint32_t flags, uint32_t timeoutMs)
{
  /* We may want to havoc parts of mqttConnection */
  IotMqttError_t unconstrainedIotMqttError;
  return unconstrainedIotMqttError;
}

IotMqttError_t IotMqtt_TimedSubscribe( IotMqttConnection_t mqttConnection,
                                       const IotMqttSubscription_t * pSubscriptionList,
                                       size_t subscriptionCount,
                                       uint32_t flags,
                                       uint32_t timeoutMs )
{
  /* We may want to havoc parts of mqttConnection */
  IotMqttError_t unconstrainedIotMqttError;
  return unconstrainedIotMqttError;
}

/* This is the function signature for the function under test */
OTA_Err_t prvRequestJob_Mqtt( OTA_AgentContext_t * pxAgentCtx );

void harness() {
  OTA_AgentContext_t pxAgentCtx;
  OTA_ConnectionContext_t connContext;
  pxAgentCtx.pvConnectionContext = nondet_bool() ? &connContext:NULL;
  __CPROVER_assume(pxAgentCtx.pvConnectionContext != NULL);

  prvRequestJob_Mqtt(&pxAgentCtx);
}
