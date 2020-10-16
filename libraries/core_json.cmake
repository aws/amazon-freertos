afr_module( NAME core_json )

afr_set_lib_metadata(ID "core_mqtt")
afr_set_lib_metadata(DESCRIPTION "The coreJSON library is a parser \
that strictly enforces the ECMA-404 JSON standard and is suitable for \
low memory footprint embedded devices.")
afr_set_lib_metadata(DISPLAY_NAME "Core JSON")
afr_set_lib_metadata(CATEGORY "Tool")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

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
        AFR::common
)
