# Device Defender demo
afr_demo_module(defender)

afr_set_demo_metadata(ID "DEFENDER_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates reporting metrics to AWS IoT with Device Defender")
afr_set_demo_metadata(DISPLAY_NAME "Device Defender")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_iot_demo_defender.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::defender
)
