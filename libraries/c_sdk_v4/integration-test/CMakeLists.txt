# MQTT test
afr_test_module(core_mqtt_integration)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/core_mqtt_system_test.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE 
        AFR::core_mqtt
        AFR::transport_interface_secure_sockets
        AFR::common
        AFR::platform
)
