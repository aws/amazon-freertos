afr_module(NAME "freertos_plus_tcp_utils" INTERNAL)

set(utils_src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp/tools/tcp_utilities")
set(utils_inc_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp/tools/tcp_utilities/include")

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp_utils.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${utils_src_dir}/tcp_dump_packets.c"
        "${utils_src_dir}/tcp_mem_stats.c"
        "${utils_src_dir}/tcp_netstat.c"
        # Header files are added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        "${utils_inc_dir}/tcp_dump_packets.h"
        "${utils_inc_dir}/tcp_mem_stats.h"
        "${utils_inc_dir}/tcp_netstat.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${utils_inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC AFR::freertos_plus_tcp
)
