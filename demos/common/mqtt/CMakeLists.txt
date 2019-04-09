# MQTT hello world demo
afr_demo_module(demo_mqtt_hello_world)
afr_set_demo_metadata(ID "mqtt_hello_world_example")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates a round-trip connectivity with AWS \
IoT and validates the connectivity using the AWS Console")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Hello World")

afr_module_sources(
    demo_mqtt_hello_world
    INTERFACE
        "${AFR_DEMOS_DIR}/mqtt/aws_hello_world.c"
        "${AFR_DEMOS_DIR}/include/aws_hello_world.h"
)
afr_module_dependencies(
    demo_mqtt_hello_world
    INTERFACE
        AFR::mqtt
)

# We don't display the 2 demos below in AFR console
if(AFR_NON_BUILD_MODE)
    return()
endif()

# MQTT publish subscribe demo
afr_demo_module(demo_mqtt_pubsub)
afr_set_demo_metadata(ID "mqtt_pub_sub_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to publish and subscribe to MQTT messages via \
AWS IoT")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Publish Subscribe")

afr_module_sources(
    demo_mqtt_pubsub
    INTERFACE
        "${AFR_DEMOS_DIR}/mqtt/aws_subscribe_publish_loop.c"
        "${AFR_DEMOS_DIR}/include/aws_subscribe_publish_loop.h"
)
afr_module_dependencies(
    demo_mqtt_pubsub
    INTERFACE
        AFR::mqtt
)

# C SDK MQTT demo
afr_demo_module(demo_mqtt_v4)
afr_module_sources(
    demo_mqtt_v4
    INTERFACE
        "${AFR_DEMOS_DIR}/mqtt/iot_demo_mqtt.c"
)
afr_module_dependencies(
    demo_mqtt_v4
    INTERFACE
        AFR::mqtt
)

# MQTT-over-BLE demo.
# Only build on boards that support BLE.
if(${BLE_SUPPORTED})
    afr_demo_module(demo_mqtt_ble)
    afr_module_sources(
        demo_mqtt_ble
        INTERFACE
            "${AFR_DEMOS_DIR}/mqtt/aws_mqtt_ble_hello_world.c"
    )
endif()
