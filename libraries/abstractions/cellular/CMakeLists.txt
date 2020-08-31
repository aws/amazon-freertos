afr_module()

afr_set_lib_metadata(ID "cellular")
afr_set_lib_metadata(DESCRIPTION "This library implements Cellular interface.")
afr_set_lib_metadata(DISPLAY_NAME "Cellular Management")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/cellular_api.h"
        "${inc_dir}/cellular_types.h"
        "${inc_dir}/cellular_config_defaults.h"
        "${inc_dir}/cellular_comm_interface.h"
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
    PRIVATE
        AFR::cellular::mcu_port
        AFR::cellular_module_port
    PUBLIC
        AFR::platform
)

# CELLULAR test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/test_cellular_api.c"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${inc_dir}"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::cellular
)
