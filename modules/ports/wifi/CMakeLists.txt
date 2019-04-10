afr_module(wifi)

afr_set_lib_metadata(ID "wifi")
afr_set_lib_metadata(DESCRIPTION "This library implements an abstraction layer for Wi-Fi features such as setup, configuration, provisioning, security and power management.")
afr_set_lib_metadata(DISPLAY_NAME "Wi-Fi Management")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

afr_module_sources(
    wifi
    PRIVATE "${AFR_MODULES_DIR}/include/aws_wifi.h"
)

afr_module_include_dirs(
    wifi
    PUBLIC "${AFR_MODULES_DIR}/include"
)

afr_module_dependencies(
    wifi
    PRIVATE AFR::wifi::mcu_port
)
