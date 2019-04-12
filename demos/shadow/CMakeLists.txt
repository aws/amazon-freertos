# Shadow demo
afr_demo_module()

afr_set_demo_metadata(ID "shadow_lightbulb_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to update and monitor the AWS IoT Device \
Shadow of a light bulb")
afr_set_demo_metadata(DISPLAY_NAME "AWS IoT Device Shadows")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_shadow_lightbulb_on_off.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::shadow
        3rdparty::jsmn
)

# C SDK Shadow demo
afr_demo_module(demo_shadow_v4)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_iot_demo_shadow.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::shadow
)
