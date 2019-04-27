# We don't display the demos below in AFR console
if(AFR_NON_BUILD_MODE)
    return()
endif()

# C SDK MQTT demo
afr_demo_module(mqtt)

afr_set_demo_metadata(ID "mqtt_example")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates MQTT")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Demo")

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

afr_set_demo_metadata(ID "mqtt_ble_example")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates MQTT over BLE")
afr_set_demo_metadata(DISPLAY_NAME "BLE MQTT Demo")

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




