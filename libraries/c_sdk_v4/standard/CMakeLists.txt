afr_module(NAME "core_mqtt" )

afr_set_lib_metadata(ID "core_mqtt")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "Core MQTT")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

# Include MQTT library's source and header path variables.
include("${CMAKE_CURRENT_LIST_DIR}/coreMQTT/mqttFilePaths.cmake")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${MQTT_SOURCES}
        ${MQTT_SERIALIZER_SOURCES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${MQTT_INCLUDE_PUBLIC_DIRS}
    PRIVATE
        ${MQTT_INCLUDE_PRIVATE_DIRS}
)

# Dependency of module on logging stack.
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
)
