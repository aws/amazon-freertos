# OTA over BLE demo
afr_demo_module(ota_mqtt_ble)

afr_set_demo_metadata(ID "OTA_MQTT_BLE_TRANSPORT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates OTA over BLE assisted by a companion device")
afr_set_demo_metadata(DISPLAY_NAME "OTA demo over BLE")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/ota_demo_mqtt_ble_transport.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::ble
        # Add dependency on the core_mqtt_demo_dependencies metadata module
        # so that FreeRTOS console shows this demo when BOTH the core MQTT
        # (or another library depending on coreMQTT) AND BLE libraries are
        # selected on the console.
        AFR::core_mqtt_demo_dependencies
        AFR::ota::mcu_port
        # Add dependency on the ota_demo_dependencies metadata module
        # so that FreeRTOS console shows this demo ONLY when the OTA library
        # (or another library depending on OTA) is selected on the console.
        AFR::ota_demo_dependencies
)
