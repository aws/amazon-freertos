afr_module(NAME mqtt_agent)

# Include mqtt agent source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreMQTTAgent/mqttAgentFilePaths.cmake")

# Create a list of all header files in the coreJSON library.
# The list of header files will be added to metadata required
# for the FreeRTOS console.
set(MQTT_AGENT_HEADER_FILES "")
foreach(mqtt_agent_public_include_dir ${MQTT_AGENT_INCLUDE_PUBLIC_DIRS})
    file(GLOB mqtt_agent_public_include_header_files
              LIST_DIRECTORIES false
              ${mqtt_agent_public_include_dir}/* )
    list(APPEND MQTT_AGENT_HEADER_FILES ${mqtt_agent_public_include_header_files})
endforeach()

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/coreMQTTAgent/mqttAgentFilePaths.cmake
    ${CMAKE_CURRENT_LIST_DIR}/mqtt_agent.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${MQTT_AGENT_SOURCES}
        # List of header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        ${MQTT_AGENT_HEADER_FILES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${MQTT_AGENT_INCLUDE_PUBLIC_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::logging
        AFR::core_mqtt
)
