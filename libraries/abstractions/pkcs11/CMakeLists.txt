afr_module()

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE "${inc_dir}/iot_pkcs11.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        3rdparty::pkcs11
    PRIVATE
        AFR::${AFR_CURRENT_MODULE}::mcu_port
)


# Link to this INTERFACE target to use the default implementation based on mbedtls.
afr_module(NAME pkcs11_mbedtls INTERFACE)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/mbedtls")

afr_module_sources(
    pkcs11_mbedtls
    INTERFACE
        "${src_dir}/aws_pkcs11_mbedtls.c"
        "${src_dir}/threading_alt.h"
)

afr_module_dependencies(
    pkcs11_mbedtls
    INTERFACE
        AFR::crypto
        3rdparty::mbedtls
)

# PKCS11 test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_pkcs11.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::pkcs11
        AFR::crypto
)
