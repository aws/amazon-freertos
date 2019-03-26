afr_module(shadow)

afr_module_sources(
    shadow
    PRIVATE
        "${AFR_MODULES_DIR}/shadow/aws_shadow.c"
        "${AFR_MODULES_DIR}/shadow/aws_shadow_json.c"
        "${AFR_MODULES_DIR}/include/aws_shadow.h"
        "${AFR_MODULES_DIR}/include/private/aws_shadow_json.h"
        "${AFR_MODULES_DIR}/include/private/aws_shadow_config_defaults.h"
)

afr_module_include_dirs(
    shadow
    PUBLIC
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

afr_module_dependencies(
    shadow
    PUBLIC
        AFR::mqtt
    PRIVATE
        3rdparty::jsmn
)
