afr_module()

afr_set_lib_metadata(ID "pkcs11")
afr_set_lib_metadata(DESCRIPTION "This library implements a PKCS#11 cryptographic module. PKCS#11 \
standard defines a platform-independent API to create and manipulate cryptographic tokens.")
afr_set_lib_metadata(DISPLAY_NAME "PKCS11")
afr_set_lib_metadata(CATEGORY "Security")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE "${inc_dir}/aws_pkcs11.h"
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
