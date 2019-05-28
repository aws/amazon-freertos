afr_module()

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/aws_secure_sockets.h"
        "${inc_dir}/aws_secure_sockets_config_defaults.h"
        "${inc_dir}/aws_secure_sockets_wrapper_metrics.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
        # Requires standard/common/include/private/aws_lib_init.h
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE AFR::${AFR_CURRENT_MODULE}::mcu_port
)

# Link to this INTERFACE target to use the default implementation based on FreeRTOS-Plus-TCP.
afr_module(NAME secure_sockets_freertos_tcp INTERFACE)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp")

afr_module_sources(
    secure_sockets_freertos_tcp INTERFACE
    "${src_dir}/aws_secure_sockets.c"
)

afr_module_dependencies(
    secure_sockets_freertos_tcp
    INTERFACE
        AFR::freertos_plus_tcp
        AFR::tls
        AFR::crypto
        AFR::pkcs11
)

# Secure sockets test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_tcp.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::secure_sockets
)
