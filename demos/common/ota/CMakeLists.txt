# OTA demo
afr_demo_module(demo_ota)
afr_set_demo_metadata(ID "ota_update_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to update firmware over the air via AWS IoT")
afr_set_demo_metadata(DISPLAY_NAME "OTA Update")

afr_module_sources(
    demo_ota
    INTERFACE
        "${AFR_DEMOS_DIR}/ota/aws_ota_update_demo.c"
        "${AFR_DEMOS_DIR}/include/aws_ota_update_demo.h"
)
afr_module_dependencies(
    demo_ota
    INTERFACE
        AFR::ota
)
