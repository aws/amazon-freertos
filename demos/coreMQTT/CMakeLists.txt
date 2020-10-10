# AFR coreMQTT demos
afr_demo_module(core_mqtt)

# Set the default demo that is enabled in the downloaded package.
afr_set_demo_metadata(ID "CORE_MQTT_MUTUAL_AUTH_DEMO")
afr_set_demo_metadata(DESCRIPTION "Examples that demonstrate the MQTT library")
afr_set_demo_metadata(DISPLAY_NAME "coreMQTT Demos")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_mutual_auth.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_plaintext.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_serializer.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::retry_utils
        AFR::transport_interface_secure_sockets
        AFR::secure_sockets
)
