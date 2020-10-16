afr_module(NAME "device_shadow" )

afr_set_lib_metadata(ID "device_shadow")
afr_set_lib_metadata(DESCRIPTION "The AWS IoT Device Shadow library enables you to \
store and retrieve the current state (the “shadow”) of every registered device.")
afr_set_lib_metadata(DISPLAY_NAME "Device Shadow")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

# Include Shadow library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/device-shadow-for-aws-iot-embedded-sdk/shadowFilePaths.cmake")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${SHADOW_SOURCES}
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
