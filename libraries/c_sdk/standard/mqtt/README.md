# MQTT Shim for MQTT V2.x.x APIs

This MQTT shim library supports backward compatibility for the MQTT V2.x.x APIs using the [coreMQTT library](../../../coreMQTT/README.md). All the [APIs in MQTT v2.x.x](include/iot_mqtt.h) are supported in this shim implementation. 
This shim will be supported only as a short term solution to provide backward compatibility. We recommend using the redesigned [coreMQTT library](../../../coreMQTT/README.md) for applications requiring long term support.


## MQTT Shim library configs

Configuration settings of the MQTT shim library. These configurations are available in addition to the configurations of MQTT V2.x.x library.

Configuration settings are C pre-processor constants. They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the library will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, a library must be rebuilt if a configuration setting is changed.

1. `MAX_NO_OF_MQTT_CONNECTIONS` - Maximum number of MQTT connections that can be handled by the MQTT shim at a time. Default value is 2.
2. `MAX_NO_OF_MQTT_SUBSCRIPTIONS` - Maximum number of active MQTT subscriptions per MQTT connection. Default value is 10.
3. `NETWORK_BUFFER_SIZE` - Static network buffer size for the MQTT packets to be sent using the MQTT shim. Please note that a different buffer is used for receiving MQTT packets and is dynamically allocated. Default value is 1024.


**Note:** The config `IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES` to override serializer APIs is not supported in this MQTT shim implementation. [MQTT over BLE](https://docs.aws.amazon.com/freertos/latest/userguide/ble-demo.html#ble-demo-mqtt) is redesigned to avoid the dependency on this config by using a [BLE transport implementation](../ble/src/services/mqtt_ble/iot_ble_mqtt_transport.c).
