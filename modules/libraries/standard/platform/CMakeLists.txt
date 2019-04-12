afr_module(INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_clock_afr.c"
        "${src_dir}/iot_network_afr.c"
        "${src_dir}/iot_threads_afr.c"

        "${inc_dir}/platform/iot_metrics.h"
        "${inc_dir}/platform/iot_clock.h"
        "${inc_dir}/platform/iot_network.h"
        "${inc_dir}/platform/iot_network_afr.h"
        "${inc_dir}/platform/iot_network_ble.h"
        "${inc_dir}/platform/iot_platform_types_afr.h"
        "${inc_dir}/platform/iot_threads.h"
        "${inc_dir}/types/iot_platform_types.h"
        "${inc_dir}/types/iot_network_types.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::secure_sockets
)

# C SDK platform layer test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_platform_clock.c"
        "${test_dir}/iot_test_platform_threads.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::platform
)
