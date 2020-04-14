afr_module()

afr_set_lib_metadata(ID "mqtt")
afr_set_lib_metadata(DESCRIPTION "This library implements the MQTT protocol that enables \
communication with AWS IoT. MQTT is an ISO standard publish-subscribe-based messaging protocol.")
afr_set_lib_metadata(DISPLAY_NAME "MQTT")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(c_sdk_mqtt_dir "${AFR_ROOT_DIR}/../aws-iot-device-sdk-embedded-C/libraries/standard/mqtt")
set(local_src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(c_sdk_src_dir "${c_sdk_mqtt_dir}/src")
set(local_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(c_sdk_inc_dir "${c_sdk_mqtt_dir}/include")
set(local_test_dir "${CMAKE_CURRENT_LIST_DIR}/test")
set(c_sdk_test_dir "${c_sdk_mqtt_dir}/test")

#set the directories to point to the C-SDK directories from
#Unified workspace.
set(src_dir "${c_sdk_src_dir}")
set(inc_dir "${c_sdk_inc_dir}")
set(test_dir "${c_sdk_test_dir}")

#Include the file paths directory to get all the files for source
#and include.
include("${c_sdk_mqtt_dir}/filePaths.cmake")

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

#Prepend all the MQTT source and include paths with relative
#path to C-SDK
list(TRANSFORM MQTT_SOURCES PREPEND "${c_sdk_mqtt_dir}/")
list(TRANSFORM MQTT_INCLUDE_PUBLIC_DIRS PREPEND "${c_sdk_mqtt_dir}/")
list(TRANSFORM MQTT_TEST_INCLUDE_PRIVATE_DIRS PREPEND "${c_sdk_mqtt_dir}/")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${MQTT_SOURCES}
        ${extra_mqtt_sources}
)

#Disable compiling of shim layer.
if(TARGET AFR::secure_sockets::mcu_port)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${local_src_dir}/iot_mqtt_agent.c"
)
endif()

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${MQTT_INCLUDE_PUBLIC_DIRS}"
        "$<${AFR_IS_TESTING}:${MQTT_TEST_INCLUDE_PRIVATE_DIRS}>"
        "${CMAKE_CURRENT_LIST_DIR}/v1_shim/include"
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

# MQTT test
afr_test_module()

# Enable tests for BLE-over-MQTT serializers on supported boards.
if(BLE_SUPPORTED)
    set(extra_test_mqtt_sources "${test_dir}/unit/iot_tests_mqtt_serialize_ble.c")
endif()

list(TRANSFORM MQTT_SYSTEM_TEST_SOURCES PREPEND "${c_sdk_mqtt_dir}/")
list(TRANSFORM MQTT_UNIT_TEST_SOURCES PREPEND "${c_sdk_mqtt_dir}/")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/mock/iot_tests_mqtt_mock.c"
        "${MQTT_SYSTEM_TEST_SOURCES}"
        "${MQTT_UNIT_TEST_SOURCES}"
        ${extra_test_mqtt_sources}
)

#Disable compiling of Shim layer tests.
#if(TARGET AFR::secure_sockets::mcu_port)
#afr_module_sources(
#    ${AFR_CURRENT_MODULE}
#    INTERFACE
#        "${test_dir}/iot_test_mqtt_agent.c"
#)
#endif()

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/access"
        "${test_dir}/mock"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::mqtt
)
