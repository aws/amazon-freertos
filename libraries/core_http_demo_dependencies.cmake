# core_http module.
afr_module(NAME core_http INTERNAL )

# Include HTTP library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreHTTP/httpFilePaths.cmake")

# Create a list of all header files in the coreHTTP library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(HTTP_HEADER_FILES "")
foreach(http_public_include_dir ${HTTP_INCLUDE_PUBLIC_DIRS})
    file(GLOB http_public_include_header_files
              LIST_DIRECTORIES false
              ${http_public_include_dir}/*.h )
    list(APPEND HTTP_HEADER_FILES ${http_public_include_header_files})
endforeach()

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/coreHTTP/httpFilePaths.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${HTTP_SOURCES}
        # Header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${HTTP_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${HTTP_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
)
################################################################

# core_http_demo_dependencies module.
# Metadata module used for the HTTP library in the FreeRTOS console.
# It represents a collection of module dependencies required 
# by the coreHTTP demos. 
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the HTTP library, so that the
# coreHTTP demos can be downloaded.
afr_module(NAME core_http_demo_dependencies )

afr_set_lib_metadata(ID "core_http_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library implements the HTTP 1.1 client-side protocol.")
afr_set_lib_metadata(DISPLAY_NAME "coreHTTP")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/core_http_demo_dependencies.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the 
        # core_http_demo_dependencies target; otherwise, it gives the 
        # "Cannot determine link language for target" error.
        ${HTTP_SOURCES}
)

# Add dependencies of the coreHTTP demos in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_http
        AFR::http_demo_helpers
        AFR::backoff_algorithm
        AFR::pkcs11_helpers
)

# Add more dependencies for Secure Sockets based HTTP demo 
# (at demos/coreHTTP folder) ONLY if the board supports 
# the Secure Sockets library.
if(TARGET AFR::secure_sockets::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::transport_interface_secure_sockets
            AFR::secure_sockets
    )
endif()

# Add dependency on WiFi module so that WiFi library is auto-included
# when selecting core HTTP library on FreeRTOS console for boards that
# support the WiFi library.
if(TARGET AFR::wifi::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::wifi
    )
endif()