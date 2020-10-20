afr_module( NAME core_json )

# Include Json library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreJSON/jsonFilePaths.cmake")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${JSON_SOURCES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${JSON_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
)
