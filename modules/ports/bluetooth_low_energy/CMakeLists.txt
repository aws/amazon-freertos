afr_module(NAME ble)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_ble_gap.c"
        "${src_dir}/iot_ble_gatt.c"

        "${src_dir}/services/device_information/iot_ble_device_information.c"
        "${src_dir}/services/mqtt_ble/iot_ble_mqtt.c"
        "${src_dir}/services/wifi_provisioning/iot_ble_wifi_provisioning.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::wifi
        AFR::ble::mcu_port
)

# BLE test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_ble.c"
        "${test_dir}/aws_test_ble_end_to_end.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble
)
