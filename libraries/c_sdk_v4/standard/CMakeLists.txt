afr_module(NAME "coreMQTT" INTERFACE)

afr_set_lib_metadata(ID "coreMQTT")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT 3.1.1 specification. \
MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "coreMQTT")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "v1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

# Include filepaths for source and include.
include( ${CMAKE_CURRENT_LIST_DIR}/coreMQTT/mqttFilePaths.cmake )

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
         ${MQTT_SOURCES}
         ${MQTT_SERIALIZER_SOURCES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        ${MQTT_INCLUDE_PUBLIC_DIRS}
        ${MQTT_INCLUDE_PRIVATE_DIRS}
)


