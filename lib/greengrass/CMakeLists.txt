afr_module(greengrass)

afr_module_sources(
    greengrass
    PRIVATE
        "${AFR_MODULES_DIR}/greengrass/aws_greengrass_discovery.c"
        "${AFR_MODULES_DIR}/greengrass/aws_helper_secure_connect.c"
        "${AFR_MODULES_DIR}/include/aws_greengrass_discovery.h"
        "${AFR_MODULES_DIR}/include/private/aws_helper_secure_connect.h"
        "${AFR_MODULES_DIR}/include/private/aws_ggd_config_defaults.h"
)

afr_module_include_dirs(
    greengrass
    PUBLIC
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

afr_module_dependencies(
    greengrass
    PUBLIC
        AFR::secure_sockets
    PRIVATE
        AFR::mqtt
        3rdparty::jsmn
)
