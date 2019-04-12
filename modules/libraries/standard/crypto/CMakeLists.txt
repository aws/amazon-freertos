afr_module(OPTIONAL)

afr_set_lib_metadata(ID "crypto")
afr_set_lib_metadata(DESCRIPTION "This library implements cryptographic signature verification. It \
can be used for code signing in the over-the-air update process.")
afr_set_lib_metadata(DISPLAY_NAME "Crypto Verification")
afr_set_lib_metadata(CATEGORY "Security")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_crypto.c"
        "${inc_dir}/aws_crypto.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE 3rdparty::mbedtls
)

# Crypto test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_crypto.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::crypto
)
