afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_common.c"
        "${src_dir}/iot_json_utils.c"

        "${src_dir}/static_memory/aws_iot_static_memory_defender.c"
        "${src_dir}/static_memory/aws_iot_static_memory_shadow.c"
        "${src_dir}/static_memory/iot_static_memory_common.c"
        "${src_dir}/static_memory/iot_static_memory_metrics.c"
        "${src_dir}/static_memory/iot_static_memory_mqtt.c"
        "${src_dir}/static_memory/iot_static_memory_serializer.c"
        "${src_dir}/static_memory/iot_static_memory_taskpool.c"

        "${inc_dir}/iot_common.h"
        "${inc_dir}/iot_json_utils.h"
        "${inc_dir}/private/iot_error.h"
        "${inc_dir}/private/iot_static_memory.h"

        "${inc_dir}/private/aws_default_root_certificates.h"
        "${inc_dir}/aws_appversion32.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::platform
)

# Memory test
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_test_module(memory)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_memory_leak.c"
)
