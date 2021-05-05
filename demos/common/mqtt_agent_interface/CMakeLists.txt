# Module for demo helper utilities based on the OTA library.
afr_module( INTERNAL NAME mqtt_agent_interface )

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/freertos_agent_message.c"
        "${CMAKE_CURRENT_LIST_DIR}/freertos_agent_message.h"
        "${CMAKE_CURRENT_LIST_DIR}/freertos_command_pool.c"
        "${CMAKE_CURRENT_LIST_DIR}/freertos_command_pool.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::mqtt_agent
)
