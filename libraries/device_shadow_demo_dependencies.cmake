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
        AFR::common
)

################################################################

# device_shadow_demo_dependencies module.
# Metadata module used for the Device Shadow library in the FreeRTOS console.
# It represents a collection of module dependencies required 
# by the coreMQTT demos. 
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the MQTT library, so that the
# coreMQTT demos can be downloaded.
afr_module(NAME device_shadow_demo_dependencies )

afr_set_lib_metadata(ID "device_shadow_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "The AWS IoT Device Shadow library enables you to \
store and retrieve the current state (the “shadow”) of every registered device.")
afr_set_lib_metadata(DISPLAY_NAME "Device Shadow")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${SHADOW_SOURCES}
        # This file is added to the target so that it is available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/device_shadow_demo_dependencies.cmake
)

# Add dependencies of the coreMQTT demos in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt_demo_dependencies
        AFR::core_json
)
