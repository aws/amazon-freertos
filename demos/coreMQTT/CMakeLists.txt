# FreeRTOS coreMQTT demos
afr_demo_module(core_mqtt)

# Set the default demo that is enabled in the downloaded package.
afr_set_demo_metadata(ID "CORE_MQTT_MUTUAL_AUTH_DEMO")
afr_set_demo_metadata(DESCRIPTION "Examples that demonstrate the MQTT library")
afr_set_demo_metadata(DISPLAY_NAME "coreMQTT Demos")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_basic_tls.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_mutual_auth.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_plaintext.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_serializer.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_keep_alive.c"
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_connection_sharing.c"
        # As the containing directory name (coreMQTT) does not match the
        # module name (core_mqtt), we add dependency on the CMake file so
        # that metadata is generated for it, and it is present in code 
        # downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::retry_utils
        AFR::transport_interface_secure_sockets
        AFR::secure_sockets
        # Add dependency on the core_mqtt_demo_dependencies metadata module 
        # so that FreeRTOS console shows this demo ONLY when the core MQTT library
        # (or another library depending on coreMQTT) is selected on the console.
        AFR::core_mqtt_demo_dependencies
)
