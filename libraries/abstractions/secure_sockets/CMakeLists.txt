
if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()

afr_module()

afr_set_lib_metadata(ID "secure_sockets")
afr_set_lib_metadata(DESCRIPTION "This library implements the Berkeley socket interface with encryption capabilities.")
afr_set_lib_metadata(DISPLAY_NAME "Secure Sockets Management")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/iot_secure_sockets.h"
        "${inc_dir}/iot_secure_sockets_config_defaults.h"
        "${inc_dir}/iot_secure_sockets_wrapper_metrics.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
        # Requires standard/common/include/private/iot_lib_init.h
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE AFR::${AFR_CURRENT_MODULE}::mcu_port
)

# Link to this INTERFACE target to use the default implementation based on FreeRTOS-Plus-TCP.
afr_module(NAME secure_sockets_freertos_plus_tcp INTERFACE)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp")

afr_module_sources(
    secure_sockets_freertos_plus_tcp INTERFACE
    "${src_dir}/iot_secure_sockets.c"
)

afr_module_dependencies(
    secure_sockets_freertos_plus_tcp
    INTERFACE
        AFR::freertos_plus_tcp
        AFR::tls
        AFR::crypto
        AFR::pkcs11
)

# Link to this INTERFACE target to use the default implementation based on lwip.
afr_module(NAME secure_sockets_lwip INTERFACE)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/lwip")

afr_module_sources(
    secure_sockets_lwip
    INTERFACE
        "${src_dir}/iot_secure_sockets.c"
)

afr_module_dependencies(
    secure_sockets_lwip
    INTERFACE
        AFR::tls
        AFR::wifi
        3rdparty::lwip
)

# Secure sockets test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_tcp.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::secure_sockets
)
