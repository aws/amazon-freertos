# device_shadow module.
afr_module(NAME device_shadow )

# Include Shadow library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/device_shadow_for_aws_iot_embedded_sdk/shadowFilePaths.cmake")

# Create a list of all header files in the Device Shadow library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(DEVICE_SHADOW_HEADER_FILES "")
foreach(shadow_public_include_dir ${SHADOW_INCLUDE_PUBLIC_DIRS})
    file(GLOB shadow_public_include_header_files
              LIST_DIRECTORIES false
              ${shadow_public_include_dir}/* )
    list(APPEND DEVICE_SHADOW_HEADER_FILES ${shadow_public_include_header_files})
endforeach()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${SHADOW_SOURCES}
        # List of files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/device_shadow_for_aws_iot_embedded_sdk/shadowFilePaths.cmake
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

afr_set_lib_metadata(ID "device_shadow_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library enables you to store and retrieve the \
current state (the \"shadow\") of every registered device on AWS IoT.")
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
        # This file is added to the target so that it is available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/device_shadow_demo_dependencies.cmake
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
