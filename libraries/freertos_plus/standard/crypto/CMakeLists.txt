afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_crypto.c"
        "${inc_dir}/iot_crypto.h"
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
        "${test_dir}/iot_test_crypto.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::crypto
)
