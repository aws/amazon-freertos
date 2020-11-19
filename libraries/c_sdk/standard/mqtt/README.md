# MQTT compatibility layer for MQTT V2.x.x APIs

This MQTT compatibility layer for MQTT V2.x.x APIs, is using the [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md). All the [APIs in MQTT v2.x.x](include/iot_mqtt.h) are supported in this implementation. This compatibility layer will be supported only as a short term solution to provide backward compatibility. We recommend using the redesigned [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md) for applications requiring long term support.


## Configs for MQTT compatibility layer for MQTT V2.x.x APIs

Configuration settings for the MQTT compatibility layer for MQTT V2.x.x APIs are available in addition to the [configurations of MQTT V2.x.x library](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/mqtt/mqtt_config.html).

Configuration settings are C pre-processor constants. They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the compatibility layer will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, this compatibility layer must be rebuilt if a configuration setting is changed.

1. `MAX_NO_OF_MQTT_CONNECTIONS` - Maximum number of MQTT connections that can be handled by the MQTT compatibility layer for MQTT V2.x.x APIs at a time. Default value is 2.
2. `MAX_NO_OF_MQTT_SUBSCRIPTIONS` - Maximum number of active MQTT subscriptions per MQTT connection. Default value is 10.
3. `NETWORK_BUFFER_SIZE` - Static network buffer size for the MQTT packets to be sent using the MQTT compatibility layer for MQTT V2.x.x APIs. Please note that a different buffer is used for receiving MQTT packets. Default value is 1024.


**Note:** The config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES` to override serializer APIs is not supported in this MQTT compatibility layer for MQTT V2.x.x APIs. [MQTT over BLE](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-mqtt) is redesigned to avoid the dependency on this config by using a [BLE transport implementation](../ble/src/services/mqtt_ble/iot_ble_mqtt_transport.c).

## Code size of MQTT compatibility layer for MQTT V2.x.x APIs

Code size for MQTT compatibility layer for MQTT V2.x.x APIs is calculated using GCC for ARM cortex-M4 on [202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00). Build for calculating code size is done using build configuration for [cypress CY8CKIT-064S0S2-4343W](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_psoc64.html) by using the `Release` configuration. All the logs were disabled in this build. Refer to the table below for calculated code sizes.

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

MQTT compatibility layer for MQTT V2.x.x APIs is implemented using [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md). In addition to that, this compatibility layer for MQTT V2.x.x APIs also maintains all the dependencies of MQTT V2.x.x library. In order to account for the total code size cost of this compatibility layer, the code sizes for the dependencies also need to be considered. Please find the code sizes for dependencies below.

1. **coreMQTT library** : MQTT compatibility layer for MQTT V2.x.x APIs is implemented using [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md). Memory estimates for [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md) can be found in the documentation [here](https://freertos.org/mqtt/index.html).

2. **Task Pool** : MQTT compatibility layer for MQTT V2.x.x APIs depends on [Task Pool](../common/taskpool/). Refer to the table below for calculated code size of Task Pool.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_taskpool.c | 3.8K | 3.0K |
| **Total estimate** | **3.8K** | **3.0K** |

Please note that the Task Pool may not be exclusively used by this MQTT compatibility layer, but may be shared by other libraries. However, the Task Pool is not used by any of the [redesigned libraries for LTS](https://www.freertos.org/ltsroadmap.html).

3. **Platform Abstraction** :  MQTT compatibility layer for MQTT V2.x.x APIs depends on the implementation of Platform Abstraction for Network, Clock and Threads. The code size is calculated for the [FreeRTOS implementation of Abstraction Layer](../../../abstractions/platform/freertos) and is added in the table below.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_clock_freertos.c | 0.5K | 0.5K |
| iot_network_freertos.c | 1.2K | 1.1K |
| iot_threads_freertos.c | 0.7K | 0.7K |
| **Total estimate** | **2.4K** | **2.3K** |

Please note that the implementation for Platform Abstraction may not be exclusively used by this MQTT compatibility layer, but may be shared by other libraries. However, this Platform Abstraction is not used by any of the [redesigned libraries for LTS](https://www.freertos.org/ltsroadmap.html).

4. **Linear Containers** : MQTT compatibility layer for MQTT V2.x.x APIs depends on Linear Containers. However, this implementation is in [c header file](../common/include/iot_linear_containers.h) and the code sizes are already accounted for in the calculated sizes of this compatibility layer.

## Tasks created by dependencies of MQTT compatibility layer for MQTT V2.x.x APIs

MQTT compatibility layer for MQTT V2.x.x APIs depends on Task Pool and Network Interface implementation(Part of Platform Abstraction). Both Task Pool and Network Interface implementation for FreeRTOS creates additional tasks. Task Pool creates `IotTaskPoolInfo.minThreads` number of tasks. Network Interface implementation for FreeRTOS creates a task to receive from the network. Please note these tasks are required in addition to the main application task needed for the application using MQTT compatibility layer.


**Note** For applications requiring low memory footprint, we recommend using [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md), which doesn't have any dependency other than the standard C library, a customer-implemented network transport interface, and *optionally* a user-implemented platform time function. In addition to that, there are no tasks created by these dependencies of coreMQTT library.
Please find the table below for the comparison of code sizes of the **MQTT compatibility layer for MQTT V2.x.x APIs**, **coreMQTT library** and their dependencies. Please note that the *[Platform Abstraction implementation](../../../abstractions/platform/freertos)* and *[Transport implementation](../../../abstractions/transport/secure_sockets)* used in this comparison, are the implementations present in this repository. However, these implementations can be different for different platforms or applications.

**Code sizes for MQTT compatibility layer for MQTT V2.x.x APIs and dependencies**
| Modules | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| MQTT compatibility layer for MQTT V2.x.x APIs | 15K | 13.5K |
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
