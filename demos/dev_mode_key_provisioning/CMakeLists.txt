afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_dev_mode_key_provisioning.c"
        "${inc_dir}/aws_dev_mode_key_provisioning.h"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    # Requires standard/common/include/private/iot_default_root_certificates.h
    PRIVATE "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::pkcs11
        AFR::utils
        3rdparty::mbedtls
)
