# Module for retry utils library.
afr_module( NAME retry_utils INTERNAL )

# Include file path configuration for retry utils library.
include(retryUtilsFilePaths.cmake)

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/retryUtilsFilePaths.cmake
    ${CMAKE_CURRENT_LIST_DIR}/retry_utils.cmake
)

# Create a list of all header files in the backoffAlgorithm library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(RETRY_UTILS_HEADER_FILES "")
foreach(retry_utils_public_include_dir ${RETRY_UTILS_INCLUDE_PUBLIC_DIRS})
    file(GLOB retry_utils_public_include_header_files
              LIST_DIRECTORIES false
              ${retry_utils_public_include_dir}/*.h )
    list(APPEND RETRY_UTILS_HEADER_FILES ${retry_utils_public_include_header_files})
endforeach()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${RETRY_UTILS_SOURCES}
        # Header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${RETRY_UTILS_HEADER_FILES}
)


afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${RETRY_UTILS_INCLUDE_PUBLIC_DIRS}
)
