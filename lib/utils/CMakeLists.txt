afr_module(utils INTERFACE)

afr_module_sources(
    utils
    INTERFACE
        "${AFR_MODULES_DIR}/utils/aws_system_init.c"
        "${AFR_MODULES_DIR}/include/aws_system_init.h"
        "${AFR_MODULES_DIR}/include/private/aws_lib_init.h"
)

afr_module_include_dirs(
    utils
    INTERFACE "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    utils
    INTERFACE
        AFR::secure_sockets
        AFR::metrics
)

# C SDK platform layer.
add_subdirectory(platform)
