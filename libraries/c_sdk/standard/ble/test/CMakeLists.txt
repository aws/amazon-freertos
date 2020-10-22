#BLE integration Test with MQTT library
if(AFR_ENABLE_TESTS)
    afr_test_module(ble_mqtt_integration)
    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${test_dir}/iot_test_ble_mqtt_serialize.c"
            "${test_dir}/iot_mqtt_ble_system_test.c"
            "${test_dir}/iot_test_ble_end_to_end.c"
    )
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            AFR::ble
            AFR::core_mqtt
    )
endif()
