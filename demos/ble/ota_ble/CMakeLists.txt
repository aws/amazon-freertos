# OTA over BLE demo
afr_demo_module()

afr_set_demo_metadata(ID "OTA_MQTT_BLE_TRANSPORT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates OTA over BLE assisted by a companion device")
afr_set_demo_metadata(DISPLAY_NAME "OTA demo over BLE")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/ota_demo_mqtt_ble_transport.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble
        AFR::ota
        AFR::ota::mcu_port
        AFR::core_mqtt_agent_demo_dependencies
        # Add dependency on the ota_demo_dependencies metadata module
        # so that FreeRTOS console shows this demo ONLY when the OTA library
        # (or another library depending on OTA) is selected on the console.
        AFR::ota_demo_dependencies
)
