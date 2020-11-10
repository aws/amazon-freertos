# Only build on boards that support BLE.
# BLE Number Comparison Demo
afr_demo_module()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${AFR_DEMOS_DIR}/include/iot_ble_numericComparison.h"
        "${CMAKE_CURRENT_LIST_DIR}/iot_ble_numericComparison.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble
)
