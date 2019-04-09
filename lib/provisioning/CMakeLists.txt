afr_module(provisioning)

afr_module_sources(
    provisioning
    PRIVATE
        "${AFR_MODULES_DIR}/provisioning/aws_dev_mode_key_provisioning.c"
        "${AFR_MODULES_DIR}/include/aws_dev_mode_key_provisioning.h"
)
afr_module_include_dirs(
    provisioning
    PUBLIC "${AFR_MODULES_DIR}/include"
)
afr_module_dependencies(
    provisioning
    PUBLIC
        AFR::pkcs11
        3rdparty::mbedtls
)
