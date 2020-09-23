# MQTT test
afr_test_module(NAME mqtt_integration INTERFACE)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        ${MQTT_SOURCES}
        ${MQTT_SERIALIZER_SOURCES}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
        "${AFR_MODULES_C_SDK_DIR}/standard/logging_stack/"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE 
        AFR::core_mqtt
        AFR::transport
        AFR::common
)
