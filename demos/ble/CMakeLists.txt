# BLE Gatt Server demo
# Only build on boards that support BLE.
afr_demo_module(ble)

# Todo: Get the correct names from PM
afr_set_demo_metadata(ID "BLE_GATT_SERVER_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates BLE GATT Server")
afr_set_demo_metadata(DISPLAY_NAME "BLE Gatt Server Demo")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_ble_gatt_server_demo.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble
)

# BLE Number Comparison Demo
# Only build on boards that support BLE.
# We don't display the demo below in AFR console
if(AFR_METADATA_MODE)
    return()
endif()
afr_demo_module(ble_numeric_comparison)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/iot_ble_numericComparison.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble
)
