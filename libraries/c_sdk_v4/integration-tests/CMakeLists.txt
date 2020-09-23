# MQTT test
afr_test_module(mqtt_integration)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_system_test.c"
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
