afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Common files
        "${src_dir}/iot_init.c"
        "${src_dir}/iot_json_utils.c"
        "${inc_dir}/aws_appversion32.h"
        "${inc_dir}/aws_json_utils.h"
        "${inc_dir}/iot_init.h"
        "${inc_dir}/iot_json_utils.h"
        "${inc_dir}/iot_linear_containers.h"

        # Logging
        "${src_dir}/logging/aws_logging_task_dynamic_buffers.c"
        "${src_dir}/logging/iot_logging.c"
        "${inc_dir}/private/iot_logging.h"
        "${inc_dir}/aws_logging_task.h"
        "${inc_dir}/iot_logging_setup.h"

        # Platform
        "${src_dir}/platform/iot_clock_afr.c"
        "${src_dir}/platform/iot_network_afr.c"
        "${src_dir}/platform/iot_threads_afr.c"
        "${inc_dir}/platform/iot_metrics.h"
        "${inc_dir}/platform/iot_clock.h"
        "${inc_dir}/platform/iot_network.h"
        "${inc_dir}/platform/iot_network_afr.h"
        "${inc_dir}/platform/iot_network_ble.h"
        "${inc_dir}/platform/iot_platform_types_afr.h"
        "${inc_dir}/platform/iot_threads.h"
        "${inc_dir}/types/iot_platform_types.h"
        "${inc_dir}/types/iot_network_types.h"

        # Static memory
        "${src_dir}/static_memory/aws_iot_static_memory_defender.c"
        "${src_dir}/static_memory/aws_iot_static_memory_shadow.c"
        "${src_dir}/static_memory/iot_static_memory_common.c"
        "${src_dir}/static_memory/iot_static_memory_metrics.c"
        "${src_dir}/static_memory/iot_static_memory_mqtt.c"
        "${src_dir}/static_memory/iot_static_memory_serializer.c"
        "${src_dir}/static_memory/iot_static_memory_taskpool.c"

        # Task pool
        "${src_dir}/taskpool/iot_taskpool.c"
        "${src_dir}/taskpool/private/iot_taskpool_internal.h"
        "${inc_dir}/types/iot_taskpool_types.h"
        "${inc_dir}/iot_taskpool.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    PRIVATE "${inc_dir}/private"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::secure_sockets
)

# test for common module
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_memory_leak.c"
        "${test_dir}/iot_test_platform_clock.c"
        "${test_dir}/iot_test_platform_threads.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::common
)
