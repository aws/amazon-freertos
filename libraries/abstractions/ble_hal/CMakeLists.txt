afr_module()

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC
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
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        AFR::ble_hal::mcu_port
        AFR::ble
)

# BLE hal test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/src/iot_test_ble_hal_common.c"
        "${test_dir}/src/iot_test_ble_hal_afqp.c"
        "${test_dir}/src/iot_test_ble_hal_stress_test.c"
        "${test_dir}/src/iot_test_ble_hal_kpi.c"
        "${test_dir}/src/iot_test_ble_hal_integration.c"
        "${test_dir}/include/iot_test_ble_hal_common.h"
        "${test_dir}/include/iot_test_ble_hal_config_defaults.h"
        "${test_dir}/include/iot_test_ble_hal_afqp.h"
        "${test_dir}/include/iot_test_ble_hal_stress_test.h"
        "${test_dir}/include/iot_test_ble_hal_kpi.h"
        "${test_dir}/include/iot_test_ble_hal_integration.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${test_dir}/include"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble_hal
)
