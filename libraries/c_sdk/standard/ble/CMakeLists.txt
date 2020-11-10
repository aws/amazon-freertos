if (AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest/ble_mqtt_transport)
    add_subdirectory(utest/data_transfer)
    return()
endif()

afr_module()

afr_set_lib_metadata(ID "ble")
# Need to update from PM
afr_set_lib_metadata(DESCRIPTION "This library implements an abstraction layer for Bluetooth Low Energy GAP and GATT protocols")
afr_set_lib_metadata(DISPLAY_NAME "BLE Management")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_ble_gap.c"
        "${src_dir}/iot_ble_gatt.c"
        "${src_dir}/iot_ble_internal.h"
        "${src_dir}/services/device_information/iot_ble_device_information.c"
        "${src_dir}/services/data_transfer/iot_ble_data_transfer.c"
        "${src_dir}/services/mqtt_ble/iot_network_ble.c"
        "${src_dir}/services/mqtt_ble/iot_ble_mqtt_serialize.c"
        "${src_dir}/services/mqtt_ble/iot_ble_mqtt_transport.c"
        "${inc_dir}/iot_ble_config_defaults.h"
        "${inc_dir}/iot_ble_data_transfer.h"
        "${inc_dir}/iot_ble_device_information.h"
        "${inc_dir}/iot_ble_mqtt_serialize.h"
        "${inc_dir}/iot_ble_mqtt_transport.h"
        "${inc_dir}/iot_ble_mqtt_transport_config.h"
        "${inc_dir}/iot_ble_wifi_provisioning.h"
        "${inc_dir}/iot_ble.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        "${AFR_MODULES_DIR}/logging/include"
    PRIVATE
        "${test_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::ble_hal
        AFR::common
        AFR::serializer
)

if( EXISTS ${CMAKE_CURRENT_LIST_DIR}/test )
    afr_add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
endif()


afr_module(NAME ble_wifi_provisioning)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/services/wifi_provisioning/iot_ble_wifi_provisioning.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    PRIVATE
        "${test_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::serializer
        AFR::ble
        AFR::wifi
)

#BLE Wifi provisioning test
afr_test_module(ble_wifi_provisioning)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_wifi_provisioning.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ble_wifi_provisioning
)

