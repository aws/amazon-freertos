afr_module(INTERFACE)

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${inc_dir}/bt_hal_avsrc_profile.h"
        "${inc_dir}/bt_hal_gatt_client.h"
        "${inc_dir}/bt_hal_gatt_server.h"
        "${inc_dir}/bt_hal_gatt_types.h"
        "${inc_dir}/bt_hal_manager_adapter_ble.h"
        "${inc_dir}/bt_hal_manager_adapter_classic.h"
        "${inc_dir}/bt_hal_manager_types.h"
        "${inc_dir}/bt_hal_manager.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::ble_hal::mcu_port
)

# BLE hal test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_ble_hal.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble_hal
)
