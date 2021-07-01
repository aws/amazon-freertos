# coreMQTT Agent module.
afr_module(NAME core_mqtt_agent INTERNAL )

# Include MQTT library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreMQTT-Agent/mqttAgentFilePaths.cmake")

# Create a list of all header files in the coreMQTT Agent library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(MQTT_AGENT_HEADER_FILES "")
foreach(mqtt_agent_public_include_dir ${MQTT_AGENT_INCLUDE_PUBLIC_DIRS})
    file(GLOB mqtt_agent_public_include_header_files
              LIST_DIRECTORIES false
              ${mqtt_agent_public_include_dir}/*.h )
    list(APPEND MQTT_AGENT_HEADER_FILES ${mqtt_agent_public_include_header_files})
endforeach()

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/coreMQTT-Agent/mqttAgentFilePaths.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${MQTT_AGENT_SOURCES}
        # Header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${MQTT_AGENT_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${MQTT_AGENT_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack and coreMQTT.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt
        AFR::logging
)
################################################################

# core_mqtt_agent_demo_dependencies module.
# Metadata module used for the MQTT Agent library in the FreeRTOS console.
# It represents a collection of module dependencies required
# by the coreMQTT Agent demos.
# This module enables the FreeRTOS console experience of enabling
# library dependencies when selecting the MQTT Agent library, so that the
# coreMQTT Agent demos can be downloaded.
afr_module(NAME core_mqtt_agent_demo_dependencies )

afr_set_lib_metadata(ID "core_mqtt_agent_demo_dependencies")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "coreMQTT-Agent")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/core_mqtt_agent_demo_dependencies.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Adding sources so that CMake can generate the
        # core_mqtt_agent_demo_dependencies target; otherwise, it gives the
        # "Cannot determine link language for target" error.
        ${MQTT_AGENT_SOURCES}
)

# Add dependencies of the coreMQTT Agent demos in this target
# to support metadata required for FreeRTOS console.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt
        AFR::core_mqtt_agent
        AFR::backoff_algorithm
        AFR::mqtt_agent_interface
        AFR::core_mqtt_demo_dependencies
)

# Add dependency on PKCS11 Helpers module, that is required
# by the Secure Sockets based coreMQTT demo, ONLY if the board
# supports the PKCS11 module.
if(TARGET AFR::pkcs11_implementation::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC
            AFR::pkcs11_helpers
    )
endif()

# Add more dependencies for Secure Sockets based MQTT demo
# (at demos/coreMQTT folder) ONLY if the board supports
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
