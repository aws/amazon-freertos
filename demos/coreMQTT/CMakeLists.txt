# coreMQTT Mutual Auth Demo
afr_demo_module(core_mqtt)

afr_set_demo_metadata(ID "CORE_MQTT_DEMO_MUTUAL_AUTH")
afr_set_demo_metadata(DESCRIPTION "Examples that demonstrate the MQTT library.")
afr_set_demo_metadata(DISPLAY_NAME "coreMQTT Demos")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_mutual_auth.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::retry_utils
        AFR::transport_interface_secure_sockets
)
