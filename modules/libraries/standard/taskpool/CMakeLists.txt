afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_taskpool.c"
        "${src_dir}/private/iot_taskpool_internal.h"
        "${inc_dir}/iot_taskpool.h"
        "${inc_dir}/types/iot_taskpool_types.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        3rdparty::tinycbor
        3rdparty::mbedtls
)
