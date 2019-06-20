# C SDK MQTT demo
afr_demo_module(mqtt)

afr_set_demo_metadata(ID "MQTT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates MQTT")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Hello World")

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



