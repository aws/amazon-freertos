afr_module(wifi)

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
