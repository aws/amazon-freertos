# C SDK Shadow demo
afr_demo_module(core_mqtt)
afr_demo_module(core_json)
afr_demo_module(device_shadow)

afr_set_demo_metadata(ID "SHADOW_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates Shadow")
afr_set_demo_metadata(DISPLAY_NAME "Shadow Demo")


afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_helpers.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::core_json
        AFR::device_shadow
        AFR::retry_utils
        AFR::transport_interface_secure_sockets
)
