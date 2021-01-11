# Module for backoff algorithm library.
afr_module( NAME backoff_algorithm INTERNAL )

# Include file path configuration for backoff algorithm library.
include(${AFR_MODULES_DIR}/abstractions/backoff_algorithm/backoffAlgorithmFilePaths.cmake)

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${AFR_MODULES_DIR}/abstractions/backoff_algorithm/backoffAlgorithmFilePaths.cmake
    ${CMAKE_CURRENT_LIST_DIR}/backoff_algorithm.cmake
)

# Create a list of all header files in the backoffAlgorithm library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(BACKOFF_ALGORITHM_HEADER_FILES "")
foreach(backoff_algorithm_public_include_dir ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS})
    file(GLOB backoff_algorithm_public_include_header_files
              LIST_DIRECTORIES false
              ${backoff_algorithm_public_include_dir}/*.h )
    list(APPEND BACKOFF_ALGORITHM_HEADER_FILES ${backoff_algorithm_public_include_header_files})
endforeach()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${BACKOFF_ALGORITHM_SOURCES}
        # Header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${BACKOFF_ALGORITHM_HEADER_FILES}
)


afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS}
)
