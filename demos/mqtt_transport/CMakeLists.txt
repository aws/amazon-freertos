# C SDK MQTT demo
afr_demo_module(mqtt_transport)

afr_set_demo_metadata(ID "MQTT_TRANSPORT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the MQTT transport layer")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Transport Hello World")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_transport.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt_ble
)

