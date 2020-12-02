# device_defender module.
afr_module(NAME device_defender )

# Include Defender library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/device_defender_for_aws/defenderFilePaths.cmake")

# Create a list of all header files in the Device Defender library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(DEVICE_DEFENDER_HEADER_FILES "")
foreach(defender_public_include_dir ${DEFENDER_INCLUDE_PUBLIC_DIRS})
    file(GLOB defender_public_include_header_files
              LIST_DIRECTORIES false
              ${defender_public_include_dir}/*.h )
    list(APPEND DEVICE_DEFENDER_HEADER_FILES ${defender_public_include_header_files})
endforeach()

afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/device_defender_for_aws/defenderFilePaths.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${DEFENDER_SOURCES}
        # List of header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${DEVICE_DEFENDER_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${DEFENDER_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
)

################################################################

# device_defender_demo_dependencies module.
# Metadata module used for the Device Defender library in the FreeRTOS console.
# It represents a collection of module dependencies required 
# by the Device Defender demo. 
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the Device Defender library, so that the
# Device Defender demo can be downloaded.
afr_module(NAME device_defender_demo_dependencies )

afr_set_lib_metadata(ID "device_defender_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library enables device metrics reporting with AWS IoT Device Defender.")
afr_set_lib_metadata(DISPLAY_NAME "Device Defender")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/device_defender_demo_dependencies.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the 
        # device_defender_demo_dependencies target; otherwise, it gives the 
        # "Cannot determine link language for target" error.
        ${DEFENDER_SOURCES}
)

# Add dependencies of the Device Defender demo in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::device_defender
        AFR::core_json
        AFR::mqtt_demo_helpers
        # Add dependency on core_mqtt_demo_dependencies module 
        # so that coreMQTT library is auto-included when selecting
        # Device Defender library on the FreeRTOS console.
        AFR::core_mqtt_demo_dependencies
)

# The Device Defender demo depends on either of LWIP or FreeRTOS+TCP
# network stacks for collecting network metrics for the demo. 
# Therefore, add dependency on the network stack supported by the board.
if(TARGET AFR::freertos_plus_tcp::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::secure_sockets_freertos_plus_tcp
    )
# Note: Even though ESP32 boards support LWIP, the Defender Demo
# is not supported for the ESP32 boards. Thus, we prevent visibility
# of the Defender Library in list of libraries for ESP32 boards on
# the FreeRTOS console with this special check.
elseif(NOT "${AFR_BOARD_NAME}" MATCHES "^(esp32).*" )
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::secure_sockets_lwip
    )
endif()