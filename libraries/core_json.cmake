afr_module( NAME core_json )

# Include Json library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreJSON/jsonFilePaths.cmake")

# Create a list of all header files in the coreJSON library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(JSON_HEADER_FILES "")
foreach(json_public_include_dir ${JSON_INCLUDE_PUBLIC_DIRS})
    file(GLOB json_public_include_header_files
              LIST_DIRECTORIES false
              ${json_public_include_dir}/*.h )
    list(APPEND JSON_HEADER_FILES ${json_public_include_header_files})
endforeach()

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/coreJSON/jsonFilePaths.cmake
    ${CMAKE_CURRENT_LIST_DIR}/core_json.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${JSON_SOURCES}
        # List of header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${JSON_HEADER_FILES}
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
