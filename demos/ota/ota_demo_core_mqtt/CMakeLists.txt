# OTA demo module.
afr_demo_module(ota_core_mqtt)

# Set the default demo that is enabled in the downloaded package.
afr_set_demo_metadata(ID "OTA_MQTT_UPDATE_DEMO")
afr_set_demo_metadata(DESCRIPTION "Example that demonstrate the OTA and coreMQTT libraries")
afr_set_demo_metadata(DISPLAY_NAME "OTA over MQTT Demo")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/ota_demo_core_mqtt.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ota
        AFR::backoff_algorithm
        AFR::transport_interface_secure_sockets
        AFR::secure_sockets
        AFR::pkcs11_helpers
        AFR::ota::mcu_port
        # Add dependency on the ota_demo_dependencies metadata module
        # so that FreeRTOS console shows this demo ONLY when the OTA library
        # (or another library depending on OTA) is selected on the console.
        AFR::ota_demo_dependencies
)