# MQTT Compatibility Layer

MQTT Compatibility Layer provides backward compatibility from [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md) to [MQTT V2.x.x APIs](include/iot_mqtt.h). **We recommend that you use the coreMQTT library for optimized memory usage and modularity, but you can use the MQTT Compatibility Layer when you do not have the flexibility.**


## Configs for MQTT Compatibility Layer

Configuration settings for the MQTT Compatibility Layer, are available in addition to the [configurations of MQTT V2.x.x library](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/mqtt/mqtt_config.html).

Configuration settings are C pre-processor constants. They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the MQTT Compatibility Layer will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, this MQTT Compatibility Layer must be rebuilt if a configuration setting is changed.

1. `MAX_NO_OF_MQTT_CONNECTIONS` - Maximum number of MQTT connections that can be handled by the MQTT Compatibility Layer at a time. Default value is 2.
2. `MAX_NO_OF_MQTT_SUBSCRIPTIONS` - Maximum number of active MQTT subscriptions per MQTT connection. Default value is 10.
3. `NETWORK_BUFFER_SIZE` - Static network buffer size for the MQTT packets to be sent using the MQTT Compatibility Layer. Please note that a different buffer is used for receiving MQTT packets. Default value is 1024.


**Note:** The config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES` to override serializer APIs is not supported in the MQTT Compatibility Layer. [MQTT over BLE](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-mqtt) was dependent on this config. However, starting from [FreeRTOS 202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00), MQTT over BLE is redesigned to avoid the dependency on this config by using a [BLE transport implementation](../ble/src/services/mqtt_ble/iot_ble_mqtt_transport.c). Please note that applications using MQTT over BLE with MQTT V2.x.x APIs will continue to work with this MQTT Compatibility Layer without having a dependency on the config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES`.

## Code size of MQTT Compatibility Layer

Code sizes are calculated for the MQTT Compatibility Layer on [FreeRTOS 202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00). In order to calculate the total cost to memory of the MQTT Compatibility Layer, the code sizes of its dependencies are also included. MQTT Compatibility Layer and its dependencies have a code size of **24.5KB** with `-Os` compiler optimization.
Please be aware that, this code size is about 18KB higher than the code size required by the [coreMQTT library](https://github.com/FreeRTOS/coreMQTT/blob/master/README.md).

**Note** Refer to the [CODESIZE.md](CODESIZE.md) for more details about the calculation of code sizes for the MQTT Compatibility Layer and the [coreMQTT FreeRTOS documentation](https://freertos.org/mqtt/index.html) for coreMQTT library.


## Tasks required for MQTT Compatibility Layer

The MQTT Compatibility Layer has a run time dependency on tasks created by Task Pool and Network Interface implementation, in addition to the system tasks. Task Pool creates `IotTaskPoolInfo.minThreads` number of tasks and the Network Interface implementation creates a task to receive from the network.
