afr_demo_module(shadow_ble)

afr_set_demo_metadata(ID "SHADOW_BLE_TRANSPORT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates device shadow updates over BLE assisted by a companion device")
afr_set_demo_metadata(DISPLAY_NAME "Shadow updates over BLE demo")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_ble_transport.c"
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
        AFR::core_json
        AFR::device_shadow
        AFR::ble
        # Add dependency on the Shadow metadata module, device_shadow_mqtt_demo_dependencies,
        # so that this demo is displayed ONLY when BOTH the BLE and Device Shadow libraries are
        # selected on the console.
        AFR::device_shadow_demo_dependencies
)
