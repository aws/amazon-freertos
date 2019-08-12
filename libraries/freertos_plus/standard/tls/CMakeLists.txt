afr_module(INTERNAL)

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
    PRIVATE "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
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
