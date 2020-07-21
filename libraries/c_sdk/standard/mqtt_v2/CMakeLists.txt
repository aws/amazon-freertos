set(CSDK_ROOT_DIR "${AFR_ROOT_DIR}/../aws-iot-device-sdk-embedded-C")
set(MODULES_DIR "${CSDK_ROOT_DIR}/libraries")
include(mqttFilePaths.cmake)

if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()

afr_module(NAME "mqtt_v2" INTERFACE)

afr_set_lib_metadata(ID "mqtt_v2")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT v2 protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "MQTTv2")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${MQTT_SOURCES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${MQTT_INCLUDE_PUBLIC_DIRS}
    PRIVATE
        ${MQTT_INCLUDE_PRIVATE_DIRS}
)
