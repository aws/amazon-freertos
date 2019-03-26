afr_module(crypto OPTIONAL)

afr_module_sources(
    crypto
    PRIVATE
        "${AFR_MODULES_DIR}/crypto/aws_crypto.c"
        "${AFR_MODULES_DIR}/include/aws_crypto.h"
)

afr_module_include_dirs(
    crypto
    PUBLIC "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    crypto
    PRIVATE 3rdparty::mbedtls
)
