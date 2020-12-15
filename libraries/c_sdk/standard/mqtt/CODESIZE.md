# Code size of MQTT Compatibility Layer

Code size for the MQTT Compatibility Layer is calculated using GCC for ARM Cortex-M4 on [FreeRTOS 202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00). Build for calculating code size is done using build configuration for [Cypress CY8CKIT-064S0S2-4343W](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_psoc64.html) by using the Release configuration. All logs were disabled in this build. Refer to the table below for calculated code sizes.

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

The MQTT Compatibility Layer is implemented using the [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md). In addition, the MQTT Compatibility Layer maintains all dependencies of MQTT V2.x.x library. To account for the total code size of the MQTT Compatibility Layer, code sizes for all dependencies have been considered (see below).

1. **The coreMQTT library** : The MQTT Compatibility Layer is implemented using [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md). Memory estimates for [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md) can be found in the documentation [here](https://freertos.org/mqtt/index.html).

2. **Task Pool library** : The MQTT Compatibility Layer depends on [Task Pool library](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/taskpool/index.html). Refer to the table below for calculated code size of Task Pool library.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_taskpool.c | 3.8K | 3.0K |
| **Total estimate** | **3.8K** | **3.0K** |

Note that the Task Pool library may not be exclusively used by the MQTT Compatibility Layer, but may be shared by other libraries. However, this Task Pool library is not used by any of the [redesigned libraries for LTS](https://www.freertos.org/ltsroadmap.html).

3. **Platform Abstraction** :  This MQTT Compatibility Layer depends on the implementation of Platform Abstraction for Network, Clock and Threads. The code size is calculated for the [FreeRTOS implementation of Abstraction Layer](../../../abstractions/platform/freertos) and is added in the table below.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_clock_freertos.c | 0.5K | 0.5K |
| iot_network_freertos.c | 1.2K | 1.1K |
| iot_threads_freertos.c | 0.7K | 0.7K |
| **Total estimate** | **2.4K** | **2.3K** |

Note that the implementation for Platform Abstraction may not be exclusively used by the MQTT Compatibility Layer, but may be shared by other libraries. However, this Platform Abstraction is not used by any of the [redesigned libraries for LTS](https://www.freertos.org/ltsroadmap.html).

4. **Linear Containers** : This MQTT Compatibility Layer depends on [Linear Containers](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/linear_containers/index.html). However, this implementation is in [c header file](../common/include/iot_linear_containers.h) and the code sizes are already accounted for in the calculated sizes of this MQTT Compatibility Layer.
