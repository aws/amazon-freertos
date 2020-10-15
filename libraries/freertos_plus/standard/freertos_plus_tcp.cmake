afr_module(NAME "freertos_plus_tcp" INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}")

# Create a list of all pack_struct_start.h files in the FreeRTOS+TCP
# library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
file(GLOB_RECURSE PACK_STRUCT_START_FILES "pack_struct_start.h")

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
        "${inc_dir}/FreeRTOS_ARP.h"
        "${inc_dir}/FreeRTOS_DHCP.h"
        "${inc_dir}/FreeRTOS_DNS.h"
        "${inc_dir}/FreeRTOS_errno_TCP.h"
        "${inc_dir}/FreeRTOSIPConfigDefaults.h"
        "${inc_dir}/FreeRTOS_IP.h"
        "${inc_dir}/FreeRTOS_IP_Private.h"
        "${inc_dir}/FreeRTOS_Sockets.h"
        "${inc_dir}/FreeRTOS_Stream_Buffer.h"
        "${inc_dir}/FreeRTOS_TCP_IP.h"
        "${inc_dir}/FreeRTOS_TCP_WIN.h"
        "${inc_dir}/FreeRTOS_UDP_IP.h"
        "${inc_dir}/IPTraceMacroDefaults.h"
        "${inc_dir}/NetworkBufferManagement.h"
        "${inc_dir}/NetworkInterface.h"
        # List of files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/freertos_plus_tcp.cmake
        ${PACK_STRUCT_START_FILES}
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