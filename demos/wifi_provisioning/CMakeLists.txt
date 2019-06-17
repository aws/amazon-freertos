# Wifi Provisioning demo
afr_demo_module(wifi_provisioning)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${AFR_DEMOS_DIR}/include/aws_wifi_connect_task.h"
        "${CMAKE_CURRENT_LIST_DIR}/aws_wifi_connect_task.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::wifi
        AFR::ble
)
