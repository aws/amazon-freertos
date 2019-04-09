afr_module(tls OPTIONAL)

afr_set_lib_metadata(ID "tls")
afr_set_lib_metadata(DESCRIPTION "This library implements an abstraction layer for the TLS protocol.")
afr_set_lib_metadata(DISPLAY_NAME "TLS")
afr_set_lib_metadata(CATEGORY "Security")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

afr_module_sources(
    tls
    PRIVATE
        "${AFR_MODULES_DIR}/tls/aws_tls.c"
        "${AFR_MODULES_DIR}/include/aws_tls.h"
)

afr_module_include_dirs(
    tls
    PUBLIC
        "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    tls
    PRIVATE
        AFR::crypto
        AFR::pkcs11
        3rdparty::mbedtls
)
