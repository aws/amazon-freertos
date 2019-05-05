afr_module(INTERNAL)

afr_set_lib_metadata(ID "freertos_plus_tcp")
afr_set_lib_metadata(DESCRIPTION "This library implements a scalable and thread-safe TCP/IP stack for FreeRTOS kernel.")
afr_set_lib_metadata(DISPLAY_NAME "FreeRTOS Plus TCP")
afr_set_lib_metadata(CATEGORY "Operating System")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/source")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

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
