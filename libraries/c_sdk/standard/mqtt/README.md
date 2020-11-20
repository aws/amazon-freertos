# MQTT Compatibility Layer for MQTT V2.x.x APIs

MQTT Compatibility Layer for MQTT V2.x.x APIs, is using the [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md). All the [APIs in MQTT v2.x.x](include/iot_mqtt.h) are supported in this implementation. This Compatibility Layer will be supported only as a short term solution to provide backward compatibility. **We recommend using the redesigned [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md) for applications requiring long term support.**


## Configs for the Compatibility Layer

Configuration settings for the Compatibility Layer are available in addition to the [configurations of MQTT V2.x.x library](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/mqtt/mqtt_config.html).

Configuration settings are C pre-processor constants. They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the Compatibility Layer will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, this Compatibility Layer must be rebuilt if a configuration setting is changed.

1. `MAX_NO_OF_MQTT_CONNECTIONS` - Maximum number of MQTT connections that can be handled by the Compatibility Layer at a time. Default value is 2.
2. `MAX_NO_OF_MQTT_SUBSCRIPTIONS` - Maximum number of active MQTT subscriptions per MQTT connection. Default value is 10.
3. `NETWORK_BUFFER_SIZE` - Static network buffer size for the MQTT packets to be sent using the Compatibility Layer. Please note that a different buffer is used for receiving MQTT packets. Default value is 1024.


**Note:** The config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES` to override serializer APIs is not supported in the Compatibility Layer. [MQTT over BLE](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-mqtt) was dependent on this config. However, starting from [202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00), MQTT over BLE is redesigned to avoid the dependency on this config by using a [BLE transport implementation](../ble/src/services/mqtt_ble/iot_ble_mqtt_transport.c). Please note that applications using MQTT over BLE with MQTT V2.x.x APIs will continue to work with this Compatibility Layer without having a dependency on the config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES`.

## Comparison of code sizes of the Compatibility Layer and the coreMQTT library

Code sizes are calculated on [202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00). Please note that only the unique dependencies of these libraries are accounted for in this comparison and common dependencies such as the FreeRTOS Kernel, are not considered. Hence, the comparison will only show the added cost to the memory when choosing one of these libraries for developing an MQTT application. Refer to the tables below for the comparison of code sizes of the **Compatibility Layer** and **coreMQTT library** with their unique dependencies.

**Code sizes for Compatibility Layer and dependencies**
| Modules | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| Compatibility Layer | 15K | 13.5K |
| coreMQTT | 6.9K | 5.7K |
| Task Pool | 3.8K | 3.0K |
| Platform Abstraction | 2.4K | 2.3K |
| **Total estimate** | **28.1K** | **24.5K** |

**Code sizes for coreMQTT library and dependencies**
| Modules | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| coreMQTT | 6.9K | 5.7K |
| Transport implementation | 3.1K | 2.9K |
| **Total estimate** | **10K** | **8.6K** |

**Note** Refer to the [CODESIZE.md](CODESIZE.md) for more details about the calculation of code sizes for the Compatibility Layer and the [coreMQTT FreeRTOS documentation](https://freertos.org/mqtt/index.html) for coreMQTT library.

**For applications requiring low memory footprint, we recommend using [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md), which doesn't have any dependency other than the standard C library, a customer-implemented network transport interface, and *optionally* a user-implemented platform time function.**

## Comparison of tasks needed for Compatibility Layer and coreMQTT library

The Compatibility Layer needs tasks created by some of its unique dependencies to operate, in addition to the application task/s and system tasks. Task Pool creates `IotTaskPoolInfo.minThreads` number of tasks and Network Interface implementation creates a task to receive from the network.
On the other hand, coreMQTT library doesn't need any additional tasks to operate other than application task/s and system tasks.
