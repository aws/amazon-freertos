afr_module( NAME ota INTERNAL )

# Include OTA library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/otaFilePaths.cmake")

# Create a list of all header files in the OTA library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(OTA_HEADER_FILES "")
foreach(ota_public_include_dir ${OTA_INCLUDE_PUBLIC_DIRS})
    file(GLOB ota_public_include_header_files
              LIST_DIRECTORIES false
              ${ota_public_include_dir}/*.h )
    list(APPEND OTA_HEADER_FILES ${ota_public_include_header_files})
endforeach()

# Remove the hardcoded coreJSON and TinyCBOR dependancies and add 
# them manually later to enable them being used by other applications.
remove( OTA_SOURCES ${JSON_SOURCES} ${TINYCBOR_SOURCES} )
remove( OTA_INCLUDE_PRIVATE_DIRS ${JSON_INCLUDE_PUBLIC_DIRS} ${TINYCBOR_INCLUDE_DIRS} )

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/ota_for_aws/otaFilePaths.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${OTA_SOURCES}
        ${OTA_OS_FREERTOS_SOURCES} 
        ${OTA_MQTT_SOURCES}
        ${OTA_HTTP_SOURCES}
        ${JSON_SOURCES}
        ${TINYCBOR_SOURCES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${OTA_INCLUDE_PUBLIC_DIRS}
        ${OTA_INCLUDE_OS_FREERTOS_DIRS}
    PRIVATE
        ${OTA_INCLUDE_PRIVATE_DIRS}
        ${JSON_INCLUDE_PUBLIC_DIRS}
        ${TINYCBOR_INCLUDE_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
)

################################################################

# ota_demo_dependencies module.
# Metadata module used for the OTA library in the FreeRTOS console.
# It represents a collection of module dependencies required
# by the OTA demos.
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the MQTT library, so that the
# coreMQTT demos can be downloaded.
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
    PUBLIC
        ${OTA_INCLUDE_PUBLIC_DIRS}
        ${OTA_INCLUDE_OS_FREERTOS_DIRS}
    PRIVATE
        ${OTA_INCLUDE_PRIVATE_DIRS}
)

# Add dependencies of the coreMQTT demos in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::ota
        AFR::ota_demo_helpers
        AFR::mqtt_subscription_manager
        AFR::mqtt_agent
        AFR::core_mqtt_demo_dependencies
        AFR::core_http_demo_dependencies
        AFR::backoff_algorithm
        AFR::ota::mcu_port
)
