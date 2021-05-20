# Module for demo helper utilities based on the OTA library.
afr_module( INTERNAL NAME ota_demo_helpers )

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ota_demo_helpers.c"
        "${CMAKE_CURRENT_LIST_DIR}/ota_demo_helpers.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt
)

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