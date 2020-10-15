afr_module(NAME "freertos_plus_tcp" INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}")

# Create a list of all FReeRTOS+TCP headers files in the FreeRTOS+TCP
# library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
file(GLOB_RECURSE INCLUDE_HEADER_FILES "${inc_dir}/*.h")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/FreeRTOS_ARP.c"
        "${src_dir}/FreeRTOS_DHCP.c"
        "${src_dir}/FreeRTOS_DNS.c"
        "${src_dir}/FreeRTOS_IP.c"
        "${src_dir}/FreeRTOS_Sockets.c"
        "${src_dir}/FreeRTOS_Stream_Buffer.c"
        "${src_dir}/FreeRTOS_TCP_IP.c"
        "${src_dir}/FreeRTOS_TCP_WIN.c"
        "${src_dir}/FreeRTOS_UDP_IP.c"
        # List of files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp.cmake
        ${INCLUDE_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    PRIVATE "${test_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC AFR::${AFR_CURRENT_MODULE}::mcu_port
)