afr_module(name retry_utils)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/freertos/retry_utils_freertos.c"
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
