afr_module()

afr_set_lib_metadata(ID "wifi")
afr_set_lib_metadata(DESCRIPTION "This library implements an abstraction layer for Wi-Fi features such as setup, configuration, provisioning, security and power management.")
afr_set_lib_metadata(DISPLAY_NAME "Wi-Fi Management")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE "${inc_dir}/iot_wifi.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        # Requires standard/common/include/types/iot_network_types.h
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE AFR::wifi::mcu_port
)

# WiFi test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_wifi.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::wifi
)
