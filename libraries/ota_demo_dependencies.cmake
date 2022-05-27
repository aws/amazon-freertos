afr_module( NAME ota INTERNAL )

# Include OTA library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/otaFilePaths.cmake")

# Add cmake files of the OTA library and its dependencies to the metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/otaFilePaths.cmake
)

# Define a target for the Over-the-air Update library.
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${OTA_SOURCES}
        # Include missing OTA source .h files that are required to generate correct metadata.
        "${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/source/include/ota_appversion32.h"
        "${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/source/include/ota_config_defaults.h"
        ${OTA_OS_FREERTOS_SOURCES}
        # Include missing OTA FreeRTOS .h file that is required to generate correct metadata.
        "${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/source/portable/os/ota_os_freertos.h"
        ${OTA_MQTT_SOURCES}
        # Include missing OTA MQTT interface .h file that is required to generate correct metadata.
        "${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/source/include/ota_mqtt_interface.h"
        ${OTA_HTTP_SOURCES}
        # Include missing OTA HTTP interface .h file that is required to generate correct metadata.
        "${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/source/include/ota_http_interface.h"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${OTA_INCLUDE_PUBLIC_DIRS}
        ${OTA_INCLUDE_OS_FREERTOS_DIRS}
    PRIVATE
        ${OTA_INCLUDE_PRIVATE_DIRS}
)
# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
        AFR::ota::mcu_port
        3rdparty::tinycbor
        AFR::core_json
)
################################################################
# ota_demo_dependencies module.
# Metadata module used for the OTA library in the FreeRTOS console.
# It represents a collection of module dependencies required
# by the OTA demos.
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the OTA library, so that the
# OTA demos can be downloaded.
afr_module(NAME ota_demo_dependencies )
afr_set_lib_metadata(ID "ota_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library provides the interface to the over-the-air update agent, which allows devices to receive software updates securely via MQTT or HTTP.")
afr_set_lib_metadata(DISPLAY_NAME "Over-the-air Update")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "3.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")
# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/ota_demo_dependencies.cmake
)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the
        # ota_demo_dependencies target; otherwise, it gives the
        # "Cannot determine link language for target" error.
        ${OTA_MQTT_SOURCES}
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${OTA_INCLUDE_PUBLIC_DIRS}
        ${OTA_INCLUDE_OS_FREERTOS_DIRS}
        ${OTA_INCLUDE_PRIVATE_DIRS}
)
# Add dependencies of the OTA demos in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::ota
        AFR::ota_demo_version
        AFR::core_mqtt_demo_dependencies
        AFR::core_mqtt_agent
        AFR::mqtt_agent_interface
        AFR::mqtt_subscription_manager
        AFR::ota::mcu_port
)

# Add OTA over HTTP demo dependency and backoff connection retries only if the board
# supports secure sockets over TCP/IP.
if(TARGET AFR::secure_sockets::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            #Add a dependency on core MQTT agent demo only for non-BLE boards.
            AFR::core_mqtt_agent_demo_dependencies
            AFR::core_http_demo_dependencies
            AFR::backoff_algorithm
    )
endif()
