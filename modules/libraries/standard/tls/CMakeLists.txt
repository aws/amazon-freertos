afr_module(INTERNAL)

afr_set_lib_metadata(ID "tls")
afr_set_lib_metadata(DESCRIPTION "This library implements an abstraction layer for the TLS protocol.")
afr_set_lib_metadata(DISPLAY_NAME "TLS")
afr_set_lib_metadata(CATEGORY "Security")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_tls.c"
        "${inc_dir}/aws_tls.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    # Requires standard/common/include/private/aws_default_root_certificates.h
    PRIVATE "${AFR_MODULES_STANDARD_DIR}/common/include/private"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        AFR::crypto
        AFR::pkcs11
        3rdparty::mbedtls
)

# TLS test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_tls.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::secure_sockets
)
