afr_module( INTERNAL NAME ota_demo_version )

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ota_application_version.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::ota
)