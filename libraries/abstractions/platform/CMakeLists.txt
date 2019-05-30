afr_module()

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/platform/iot_metrics.h"
        "${inc_dir}/platform/iot_clock.h"
        "${inc_dir}/platform/iot_network.h"
        "${inc_dir}/platform/iot_threads.h"
        "${inc_dir}/types/iot_platform_types.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        $<TARGET_PROPERTY:AFR::${AFR_CURRENT_MODULE}::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE AFR::${AFR_CURRENT_MODULE}::mcu_port
)

# TODO, link to platform_freertos by default for now, should we remove this?
afr_mcu_port(platform DEPENDS AFR::platform_freertos)

# Link to this INTERFACE target to use the default implementation based on FreeRTOS.
afr_module(NAME platform_freertos INTERFACE)
set(src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/iot_clock_afr.c"
        "${src_dir}/iot_metrics.c"
        "${src_dir}/iot_network_afr.c"
        "${src_dir}/iot_threads_afr.c"
        "${src_dir}/include/platform/iot_network_afr.h"
        "${src_dir}/include/platform/iot_network_ble.h"
        "${src_dir}/include/platform/iot_platform_types_afr.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${src_dir}/include"
)

afr_module_dependencies(
    platform_freertos
    INTERFACE
        AFR::common
        AFR::secure_sockets
)

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
