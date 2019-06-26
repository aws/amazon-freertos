afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

# TODO, this is a workaround to remove iot_logging_task_dynamic_buffers.c from common, because
# winsim use a different logging implementation.
if(NOT AFR_BOARD STREQUAL "pc.windows")
    set(aws_logging_task "${src_dir}/logging/iot_logging_task_dynamic_buffers.c")
endif()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Common files
        "${src_dir}/iot_init.c"
        "${inc_dir}/iot_appversion32.h"
        "${inc_dir}/iot_init.h"
        "${inc_dir}/iot_linear_containers.h"

        # Logging
        "${aws_logging_task}"
        "${src_dir}/logging/iot_logging.c"
        "${inc_dir}/private/iot_logging.h"
        "${inc_dir}/iot_logging_task.h"
        "${inc_dir}/iot_logging_setup.h"

        # Platform
        "${inc_dir}/types/iot_network_types.h"  # TODO, should this belong to platform?

        # Static memory
        "${src_dir}/iot_static_memory_common.c"

	# Device metrics
	"${src_dir}/iot_device_metrics.c"

        # Task pool
        "${src_dir}/taskpool/iot_taskpool.c"
        "${src_dir}/taskpool/iot_taskpool_static_memory.c"
        "${inc_dir}/iot_taskpool.h"
        "${inc_dir}/types/iot_taskpool_types.h"
        "${inc_dir}/private/iot_taskpool_internal.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    PRIVATE "${inc_dir}/private"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::platform
)

# test for common module
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_memory_leak.c"
        "${test_dir}/iot_tests_taskpool.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::common
)
