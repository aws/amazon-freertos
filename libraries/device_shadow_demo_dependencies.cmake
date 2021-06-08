# device_shadow module.
afr_module(NAME device_shadow )

# Include Shadow library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/device_shadow_for_aws/shadowFilePaths.cmake")

# Create a list of all header files in the Device Shadow library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(DEVICE_SHADOW_HEADER_FILES "")
foreach(shadow_public_include_dir ${SHADOW_INCLUDE_PUBLIC_DIRS})
    file(GLOB shadow_public_include_header_files
              LIST_DIRECTORIES false
              ${shadow_public_include_dir}/*.h )
    list(APPEND DEVICE_SHADOW_HEADER_FILES ${shadow_public_include_header_files})
endforeach()

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/device_shadow_for_aws/shadowFilePaths.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${SHADOW_SOURCES}
        # Header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${DEVICE_SHADOW_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${SHADOW_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
)

################################################################

# device_shadow_demo_dependencies module.
# Metadata module used for the Device Shadow library in the FreeRTOS console.
# It represents a collection of module dependencies required 
# by the Device Shadow demo. 
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the Device Shadow library, so that the
# Device Shadow demo can be downloaded.
afr_module(NAME device_shadow_demo_dependencies )

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/device_shadow_demo_dependencies.cmake
)

afr_set_lib_metadata(ID "device_shadow_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library enables a registered device to update and retrieve its current state (the \"shadow\") on AWS IoT.")
afr_set_lib_metadata(DISPLAY_NAME "Device Shadow")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the 
        # device_shadow_demo_dependencies target; otherwise, it gives the 
        # "Cannot determine link language for target" error.
        ${SHADOW_SOURCES}
)

# Add dependencies of the Device Shadow demo in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::device_shadow
        AFR::core_json
        # Add dependency on core_mqtt_demo_dependencies module 
        # so that coreMQTT library is auto-included when selecting
        # Device Shadow library on the FreeRTOS console.
        AFR::core_mqtt_demo_dependencies
)

# Add dependency on "mqtt_demo_helpers" module for the
# Device Shadow demo that uses Secure Sockets 
# (present at demos/device_shadow_for_aws folder) ONLY if
# the board supports the Secure Sockets library.
if(TARGET AFR::secure_sockets::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::mqtt_demo_helpers
    )
endif()
