# jobs module.
afr_module(NAME jobs )

# Include Jobs library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/jobs_for_aws/jobsFilePaths.cmake")

# Create a list of all header files in the Jobs library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(JOBS_HEADER_FILES "")
foreach(jobs_public_include_dir ${JOBS_INCLUDE_PUBLIC_DIRS})
    file(GLOB jobs_public_include_header_files
              LIST_DIRECTORIES false
              ${jobs_public_include_dir}/*.h )
    list(APPEND JOBS_HEADER_FILES ${jobs_public_include_header_files})
endforeach()

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/jobs_for_aws/jobsFilePaths.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${JOBS_SOURCES}
        # Header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${JOBS_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${JOBS_INCLUDE_PUBLIC_DIRS}
)

################################################################

# jobs_demo_dependencies module.
# Metadata module used for the Jobs library in the FreeRTOS console.
# It represents a collection of module dependencies required 
# by the Jobs demo. 
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the Jobs library, so that the
# Jobs demo can be downloaded.
afr_module(NAME jobs_demo_dependencies )

afr_set_lib_metadata(ID "jobs_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library enables a device registered on AWS IoT to receive pending job requests and post job updates to the AWS IoT Jobs service.")
afr_set_lib_metadata(DISPLAY_NAME "Jobs")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/jobs_demo_dependencies.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the 
        # jobs_demo_dependencies target; otherwise, it gives the 
        # "Cannot determine link language for target" error.
        ${JOBS_SOURCES}
)

# Add dependencies of the Jobs demo in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::jobs
        AFR::core_json
        AFR::mqtt_demo_helpers
        # Add dependency on core_mqtt_demo_dependencies module 
        # so that coreMQTT library is auto-included when selecting
        # Jobs library on the FreeRTOS console.
        AFR::core_mqtt_demo_dependencies
)
