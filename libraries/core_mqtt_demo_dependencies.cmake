# core_mqtt module.
afr_module(NAME core_mqtt INTERNAL )

# Include MQTT library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreMQTT/mqttFilePaths.cmake")

# Create a list of all header files in the coreMQTT library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(MQTT_HEADER_FILES "")
foreach(mqtt_public_include_dir ${MQTT_INCLUDE_PUBLIC_DIRS})
    file(GLOB mqtt_public_include_header_files
              LIST_DIRECTORIES false
              ${mqtt_public_include_dir}/* )
    list(APPEND MQTT_HEADER_FILES ${mqtt_public_include_header_files})
endforeach()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${MQTT_SOURCES}
        ${MQTT_SERIALIZER_SOURCES}
        # List of files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/coreMQTT/mqttFilePaths.cmake
        ${MQTT_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${MQTT_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
)
################################################################

# core_mqtt_demo_dependencies module.
# Metadata module used for the MQTT library in the FreeRTOS console.
# It represents a collection of module dependencies required 
# by the coreMQTT demos. 
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the MQTT library, so that the
# coreMQTT demos can be downloaded.
afr_module(NAME core_mqtt_demo_dependencies )

afr_set_lib_metadata(ID "core_mqtt_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "coreMQTT")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the 
        # core_mqtt_demo_dependencies target; otherwise, it gives the 
        # "Cannot determine link language for target" error.
        ${MQTT_SOURCES}
        # This file is added to the target so that it is available
        # in code downloaded from the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/core_mqtt_demo_dependencies.cmake
)

# Add dependencies of the coreMQTT demos in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt
)

# Add more dependencies for Secure Sockets based MQTT demo 
# (at demos/coreMQTT folder) ONLY if the board supports 
# the Secure Sockets library.
if(TARGET AFR::secure_sockets::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::retry_utils
            AFR::transport_interface_secure_sockets
            AFR::secure_sockets
    )
endif()

# Add dependency on WiFi module so that WiFi library is auto-included
# when selecting core MQTT library on FreeRTOS console for boards that
# support the WiFi library.
if(TARGET AFR::wifi::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::wifi
    )
endif()

# Add dependency on BLE module so that the BLE library is auto-included
# when selecting core MQTT library on FreeRTOS console for boards that
# support BLE.
if(BLE_SUPPORTED)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::ble
    )
endif()