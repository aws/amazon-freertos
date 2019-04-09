afr_module(mqtt)

afr_set_lib_metadata(ID "mqtt")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT protocol that enables communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "MQTT")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

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
)

afr_module_include_dirs(
    mqtt
    PUBLIC
        "${AFR_MODULES_DIR}/include"
    PRIVATE
        "${AFR_TESTS_DIR}/mqtt/access"
)

afr_module_dependencies(
    mqtt
    PUBLIC
        AFR::sdk_platform
        AFR::sdk_common
)
