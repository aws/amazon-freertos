# MQTT library for backward compatibility for MQTT V2.x.x APIs

This MQTT library is for the backward compatibility of the MQTT V2.x.x APIs using the [coreMQTT library](../../../coreMQTT/README.md). All the [APIs in MQTT v2.x.x](include/iot_mqtt.h) are supported in this implementation. This library will be supported only as a short term solution to provide backward compatibility. We recommend using the redesigned [coreMQTT library](../../../coreMQTT/README.md) for applications requiring long term support.


## Configs for MQTT library for backward compatibility for MQTT V2.x.x APIs

Configuration settings of the MQTT library for backward compatibility for MQTT V2.x.x APIs. These configurations are available in addition to the [configurations of MQTT V2.x.x library](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/mqtt/mqtt_config.html).

Configuration settings are C pre-processor constants. They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the library will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, a library must be rebuilt if a configuration setting is changed.

1. `MAX_NO_OF_MQTT_CONNECTIONS` - Maximum number of MQTT connections that can be handled by the MQTT library for backward compatibility for MQTT V2.x.x APIs at a time. Default value is 2.
2. `MAX_NO_OF_MQTT_SUBSCRIPTIONS` - Maximum number of active MQTT subscriptions per MQTT connection. Default value is 10.
3. `NETWORK_BUFFER_SIZE` - Static network buffer size for the MQTT packets to be sent using the MQTT library for backward compatibility for MQTT V2.x.x APIs. Please note that a different buffer is used for receiving MQTT packets and is dynamically allocated. Default value is 1024.


**Note:** The config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES` to override serializer APIs is not supported in this MQTT library for backward compatibility for MQTT V2.x.x APIs. [MQTT over BLE](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-mqtt) is redesigned to avoid the dependency on this config by using a [BLE transport implementation](../ble/src/services/mqtt_ble/iot_ble_mqtt_transport.c).

## Code size of MQTT library for backward compatibility for MQTT V2.x.x APIs

Code size for MQTT library for backward compatibility for MQTT V2.x.x APIs is calculated using GCC for ARM cortex-M4. Build for calculating code size is done using build configuration for [cypress CY8CKIT-064S0S2-4343W](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_psoc64.html) by using the `Release` configuration. All the logs were disabled in this build. Refer to the table below for calculated code sizes.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_mqtt_agent.c | 1.3K | 1.1K |
| iot_mqtt_api.c | 3.8K | 3.3K |
| iot_mqtt_context_connection.c | 0.2K | 0.2K |
| iot_mqtt_managed_function_wrapper.c | 0.9K | 0.9K |
| iot_mqtt_mutex_wrapper.c | 0.4K | 0.3K |
| iot_mqtt_network.c | 1.2K | 1.1K |
| iot_mqtt_operation.c | 2.6K | 2.4K |
| iot_mqtt_publish_duplicates.c | 0.2K | 0.2K |
| iot_mqtt_serializer_deserializer_wrapper.c | 2.0K | 1.8K |
| iot_mqtt_subscription.c | 1.3K | 1.2K |
| iot_mqtt_subscription_container.c | 0.6K | 0.6K |
| iot_mqtt_validate.c | 0.5K | 0.4K |
| **Total estimate** | **15K** | **13.5K** |

**Note:** MQTT library for backward compatibility for MQTT V2.x.x APIs is implemented using [coreMQTT library](../../../coreMQTT/README.md). Memory estimates for [coreMQTT library](../../../coreMQTT/README.md) can be found in the documentation [here](https://freertos.org/mqtt/index.html). In addition to that, this library for backward compatibility for MQTT V2.x.x APIs also maintains all the [dependencies of MQTT V2.x.x library](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/mqtt/index.html#mqtt_dependencies). In order to account for the total memory requirement of this library, shared cost of memory of these dependencies also need to considered.

For applications requiring low memory footprint, we recommend using [coreMQTT library](../../../coreMQTT/README.md), which doesn't have any dependency other than the standard C library, a customer-implemented network transport interface, and *optionally* a user-implemented platform time function.
