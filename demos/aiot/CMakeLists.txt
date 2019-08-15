# C SDK AIOT demo
afr_demo_module(aiot)

afr_set_demo_metadata(ID "AIOT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates AIOT")
afr_set_demo_metadata(DISPLAY_NAME "AIOT Hello There")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/iot_demo_aiot.c"
        "${CMAKE_CURRENT_LIST_DIR}/app_camera.c" 
        "${CMAKE_CURRENT_LIST_DIR}/app_speech_wakeup.c"
        "${CMAKE_CURRENT_LIST_DIR}/app_speech_recsrc.c"       
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt
        3rdparty::espwho
)