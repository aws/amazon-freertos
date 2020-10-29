# OTA demo
afr_demo_module()

afr_set_demo_metadata(ID "OTA_UPDATE_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example on how to update firmware over the air via AWS IoT")
afr_set_demo_metadata(DISPLAY_NAME "OTA Update")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_iot_ota_update_demo.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::ota
        AFR::mqtt
)
