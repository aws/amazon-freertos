afr_module(sdk_common)

afr_module_sources(
    sdk_common
    PRIVATE
        "${AFR_MODULES_DIR}/common/aws_logging_task_dynamic_buffers.c"
        "${AFR_MODULES_DIR}/common/iot_common.c"
        "${AFR_MODULES_DIR}/common/iot_json_utils.c"
        "${AFR_MODULES_DIR}/common/iot_logging.c"
        "${AFR_MODULES_DIR}/common/iot_taskpool.c"

        "${AFR_MODULES_DIR}/common/static_memory/aws_iot_static_memory_defender.c"
        "${AFR_MODULES_DIR}/common/static_memory/aws_iot_static_memory_shadow.c"
        "${AFR_MODULES_DIR}/common/static_memory/iot_static_memory_common.c"
        "${AFR_MODULES_DIR}/common/static_memory/iot_static_memory_metrics.c"
        "${AFR_MODULES_DIR}/common/static_memory/iot_static_memory_mqtt.c"
        "${AFR_MODULES_DIR}/common/static_memory/iot_static_memory_serializer.c"
        "${AFR_MODULES_DIR}/common/static_memory/iot_static_memory_taskpool.c"

        "${AFR_MODULES_DIR}/include/iot_common.h"
        "${AFR_MODULES_DIR}/include/iot_json_utils.h"
        "${AFR_MODULES_DIR}/include/iot_linear_containers.h"
        "${AFR_MODULES_DIR}/include/iot_logging_setup.h"
        "${AFR_MODULES_DIR}/include/iot_taskpool.h"
        "${AFR_MODULES_DIR}/include/private/iot_error.h"
        "${AFR_MODULES_DIR}/include/private/iot_logging.h"
        "${AFR_MODULES_DIR}/include/private/iot_static_memory.h"
        "${AFR_MODULES_DIR}/include/private/iot_taskpool_internal.h"
)

afr_module_include_dirs(
    sdk_common
    PUBLIC
        "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    sdk_common
    PUBLIC
        AFR::sdk_platform
        AFR::metrics
)
