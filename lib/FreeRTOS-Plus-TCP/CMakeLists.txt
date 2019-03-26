afr_module(freertos_plus_tcp OPTIONAL)

afr_module_sources(
    freertos_plus_tcp
    PRIVATE
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_ARP.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_DHCP.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_IP.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_Sockets.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_Stream_Buffer.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_IP.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/source/FreeRTOS_UDP_IP.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_ARP.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_DHCP.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_DNS.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_errno_TCP.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOSIPConfigDefaults.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_IP.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_IP_Private.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_Sockets.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_Stream_Buffer.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_TCP_IP.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_TCP_WIN.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/FreeRTOS_UDP_IP.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/IPTraceMacroDefaults.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/NetworkBufferManagement.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include/NetworkInterface.h"

)

afr_module_include_dirs(
    freertos_plus_tcp
    PUBLIC "${AFR_MODULES_DIR}/FreeRTOS-Plus-TCP/include"
)

afr_module_dependencies(
    freertos_plus_tcp PRIVATE
    AFR::freertos_plus_tcp::mcu_port
)
