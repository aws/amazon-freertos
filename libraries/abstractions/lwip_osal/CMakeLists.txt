afr_module(INTERNAL)

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC 
        "${inc_dir}"
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/lwipopts_freertos.h"
        "${inc_dir}/arch/cc.h"
        "${inc_dir}/arch/perf.h"
        "${inc_dir}/arch/sys_arch.h"
        "${src_dir}/sys_arch.c"
)
