afr_module(sdk_platform)

afr_module_sources(
    sdk_platform
    PRIVATE
        "${AFR_MODULES_DIR}/utils/platform/iot_clock_afr.c"
        "${AFR_MODULES_DIR}/utils/platform/iot_network_afr.c"
        "${AFR_MODULES_DIR}/utils/platform/iot_threads_afr.c"

        "${AFR_MODULES_DIR}/include/iot_logging_setup.h"
        "${AFR_MODULES_DIR}/include/platform/iot_clock.h"
        "${AFR_MODULES_DIR}/include/platform/iot_network.h"
        "${AFR_MODULES_DIR}/include/platform/iot_network_afr.h"
        "${AFR_MODULES_DIR}/include/platform/iot_platform_types_afr.h"
        "${AFR_MODULES_DIR}/include/platform/iot_threads.h"
        "${AFR_MODULES_DIR}/include/types/iot_platform_types.h"

        "${AFR_MODULES_DIR}/include/private/iot_error.h"
        "${AFR_MODULES_DIR}/include/private/iot_logging.h"
)

afr_module_include_dirs(
    sdk_platform
    PUBLIC
        "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    sdk_platform
    PUBLIC
        AFR::secure_sockets
)
