afr_module(NAME ble)

afr_set_lib_metadata(ID "ble")
# Need to update from PM
afr_set_lib_metadata(DESCRIPTION "This library implements an abstraction layer for Bluetooth Low Energy features such as setup, configuration, provisioning, security and power management")
afr_set_lib_metadata(DISPLAY_NAME "Bluetooth Low Energy")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

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
        "$<${AFR_IS_TESTING}:${inc_dir}/private>"
    PRIVATE
        "${inc_dir}/private"
        "${test_dir}"
        "${AFR_MODULES_STANDARD_DIR}/mqtt/include"
        "${AFR_MODULES_STANDARD_DIR}/serializer/include"
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
