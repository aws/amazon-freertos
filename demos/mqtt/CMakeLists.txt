# MQTT hello world demo
afr_demo_module(mqtt_hello_world)

afr_set_demo_metadata(ID "mqtt_hello_world_example")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates a round-trip connectivity with AWS \
IoT and validates the connectivity using the AWS Console")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Hello World")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_hello_world.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt
)

# We don't display the demos below in AFR console
if(AFR_NON_BUILD_MODE)
    return()
endif()

# MQTT publish subscribe demo
afr_demo_module(mqtt_pubsub)

afr_set_demo_metadata(ID "mqtt_pub_sub_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to publish and subscribe to MQTT messages via \
AWS IoT")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Publish Subscribe")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_subscribe_publish_loop.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt
)

# C SDK MQTT demo
afr_demo_module(mqtt_v4)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_mqtt.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt
)

# MQTT-over-BLE demo.
# Only build on boards that support BLE.
afr_demo_module(mqtt_ble)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_mqtt_ble_hello_world.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt
        AFR::ble
)
