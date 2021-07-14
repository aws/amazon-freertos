afr_module()

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/platform/iot_clock.h"
        "${inc_dir}/platform/iot_network.h"
        "${inc_dir}/platform/iot_threads.h"
        "${inc_dir}/types/iot_platform_types.h"
        "${inc_dir}/types/iot_network_types.h"
        "${src_dir}/iot_clock_freertos.c"
        "${src_dir}/iot_threads_freertos.c"
        "${src_dir}/include/platform/iot_platform_types_freertos.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        "${src_dir}/include"
        "${inc_dir}/platform"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
)

if(TARGET AFR::secure_sockets::mcu_port)
    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        PRIVATE
        "${inc_dir}/platform/iot_metrics.h"
        "${src_dir}/iot_metrics.c"
        "${src_dir}/iot_network_freertos.c"
        "${src_dir}/include/platform/iot_network_freertos.h"
    )
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::secure_sockets
    )
endif()

# test for platform module
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
