afr_module(crypto OPTIONAL)

afr_set_lib_metadata(ID "crypto")
afr_set_lib_metadata(DESCRIPTION "This library implements cryptographic signature verification. It \
can be used for code signing in the over-the-air update process.")
afr_set_lib_metadata(DISPLAY_NAME "Crypto Verification")
afr_set_lib_metadata(CATEGORY "Security")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

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
