afr_module(metrics)

afr_module_sources(
    metrics
    PRIVATE
        "${AFR_MODULES_DIR}/metrics/aws_secure_sockets_wrapper_metrics.c"
        "${AFR_MODULES_DIR}/metrics/iot_metrics.c"
        "${AFR_MODULES_DIR}/include/iot_linear_containers.h"
)

afr_module_include_dirs(
    metrics
    PUBLIC
        "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    metrics
    PUBLIC
        AFR::sdk_platform
)
