# We don't display the demos below in AFR console
if(AFR_NON_BUILD_MODE)
    return()
endif()

# BLE Gatt Server demo
# Only build on boards that support BLE.
afr_demo_module(ble_gatt_server)

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




