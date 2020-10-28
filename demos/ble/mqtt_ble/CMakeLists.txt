# MQTT over BLE demo
afr_demo_module(mqtt_ble)

afr_set_demo_metadata(ID "MQTT_BLE_TRANSPORT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates MQTT over BLE assisted by a companion device")
afr_set_demo_metadata(DISPLAY_NAME "MQTT Hello World demo over BLE")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_ble_transport.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        ${AFR_MODULES_DIR}/logging/include
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
)
