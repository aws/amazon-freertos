afr_module(pkcs11)

afr_set_lib_metadata(ID "pkcs11")
afr_set_lib_metadata(DESCRIPTION "This library implements a PKCS#11 cryptographic module. PKCS#11 standard defines a platform-independent API to create and manipulate cryptographic tokens.")
afr_set_lib_metadata(DISPLAY_NAME "PKCS11")
afr_set_lib_metadata(CATEGORY "Security")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

afr_module_sources(
    pkcs11
    PRIVATE "${AFR_MODULES_DIR}/include/aws_pkcs11.h"
)

afr_module_include_dirs(
    pkcs11
    PUBLIC
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

afr_module_dependencies(
    pkcs11
    PUBLIC
        3rdparty::pkcs11
    PRIVATE
        AFR::pkcs11::mcu_port
)


# Link to this INTERFACE target to use the default implementation based on mbedtls.
afr_module(pkcs11_mbedtls INTERFACE)
afr_module_sources(
    pkcs11_mbedtls INTERFACE
    "${AFR_MODULES_DIR}/pkcs11/mbedtls/aws_pkcs11_mbedtls.c"
    "${AFR_MODULES_DIR}/include/private/threading_alt.h"
)
afr_module_dependencies(
    pkcs11_mbedtls INTERFACE
    AFR::crypto
    3rdparty::mbedtls
)
