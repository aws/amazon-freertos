# cellular management demo

afr_demo_module(cellular_manager)

afr_set_demo_metadata(ID "cellular_manager")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates cellular management")
afr_set_demo_metadata(DISPLAY_NAME "Cellular Manager")

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/standard/serializer/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/cellular_manager.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::cellular
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/include"
)
