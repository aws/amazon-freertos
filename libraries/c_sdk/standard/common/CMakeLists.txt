afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Common files
        "${src_dir}/iot_init.c"
        "${inc_dir}/iot_init.h"
        "${inc_dir}/iot_linear_containers.h"

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
        AFR::logging
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
        AFR::logging
)
