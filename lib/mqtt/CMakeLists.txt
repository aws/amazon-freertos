afr_module(mqtt)

# Compile BLE MQTT serializers on supported platforms.
if(${BLE_SUPPORTED})
    set(extra_mqtt_sources "${AFR_MODULES_DIR}/mqtt/iot_ble_mqtt_serialize.c")
    set(extra_mqtt_private_includes ${BLE_LIBRARY_INCLUDE_DIR})
    set(extra_mqtt_dependencies AFR::serializer)
endif()

# Enable test access if building tests.
if(${AFR_IS_TESTING})
    list(APPEND extra_mqtt_private_includes "${AFR_TESTS_DIR}/mqtt/access")
endif()

afr_module_sources(
    mqtt
    PRIVATE
    "${AFR_MODULES_DIR}/mqtt/aws_mqtt_agent.c"
    "${AFR_MODULES_DIR}/mqtt/iot_mqtt_api.c"
    "${AFR_MODULES_DIR}/mqtt/iot_mqtt_network.c"
    "${AFR_MODULES_DIR}/mqtt/iot_mqtt_operation.c"
    "${AFR_MODULES_DIR}/mqtt/iot_mqtt_serialize.c"
    "${AFR_MODULES_DIR}/mqtt/iot_mqtt_subscription.c"
    "${AFR_MODULES_DIR}/mqtt/iot_mqtt_validate.c"
    ${extra_mqtt_sources}
)
afr_module_include_dirs(
    mqtt
    PUBLIC
        "${AFR_MODULES_DIR}/include"
    PRIVATE
        ${extra_mqtt_private_includes}
)
afr_module_dependencies(
    mqtt
    PUBLIC
        AFR::sdk_platform
        AFR::sdk_common
        ${extra_mqtt_dependencies}
)
