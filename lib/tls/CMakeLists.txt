afr_module(tls OPTIONAL)

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
