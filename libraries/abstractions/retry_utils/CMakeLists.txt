afr_module(NAME retry_utils)

# Metadata for the module.
afr_set_lib_metadata(ID "retry_utils")
afr_set_lib_metadata(DESCRIPTION "This library represents the exponential backoff with jitter retry utility.")
afr_set_lib_metadata(DISPLAY_NAME "Retry Utilities")
afr_set_lib_metadata(CATEGORY "Utilities")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/freertos/retry_utils_freertos.c"
        "${CMAKE_CURRENT_LIST_DIR}/retry_utils.h"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        AFR::kernel
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
