afr_module()

afr_set_lib_metadata(ID "mqttv2")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT v2 protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "MQTTv2")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/utest")

# Compile BLE MQTT serializers on supported platforms.
if(${BLE_SUPPORTED})
    set(extra_mqtt_sources "${src_dir}/iot_ble_mqtt_serialize.c")
    set(extra_mqtt_dependencies AFR::serializer AFR::ble)
endif()

# Enable test access and mocks if building tests.
if(${AFR_IS_TESTING})
    list(APPEND extra_mqtt_test_includes "${test_dir}/access")
    list(APPEND extra_mqtt_test_includes "${test_dir}/mock")
endif()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/mqtt.c"
        "${src_dir}/mqtt_lightweight.c"
        "${src_dir}/mqtt_state.c"
        ${extra_mqtt_sources}
)



afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        "${inc_dir}/mqtt_config.h"
        "${inc_dir}/logging_levels.h"
        "$<${AFR_IS_TESTING}:${src_dir}>"
    PRIVATE
        ${extra_mqtt_includes}
        ${extra_mqtt_test_includes}
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
        ${extra_mqtt_dependencies}
)


