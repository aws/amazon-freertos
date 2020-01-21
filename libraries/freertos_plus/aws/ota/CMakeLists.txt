afr_module()

afr_set_lib_metadata(ID "ota")
afr_set_lib_metadata(DESCRIPTION "This library provides the interface to the over-the-air update agent, which allows devices to receive software updates securely via MQTT.")
afr_set_lib_metadata(DISPLAY_NAME "OTA Updates")
afr_set_lib_metadata(CATEGORY "Update")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/aws_iot_ota_agent.h"
        "${inc_dir}/aws_iot_ota_types.h"
        "${src_dir}/aws_iot_ota_agent_internal.h"
        "${src_dir}/aws_iot_ota_agent.c"
        "${src_dir}/aws_iot_ota_interface.c"
        "${src_dir}/aws_iot_ota_interface.h"
        "${src_dir}/aws_iot_ota_pal.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        "$<${AFR_IS_TESTING}:${src_dir}>"
    PRIVATE
        "${src_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
    PRIVATE
        AFR::${AFR_CURRENT_MODULE}::mcu_port
        3rdparty::jsmn
)

# OTA depends on only 1 file from mbedtls
afr_module_sources(
    ${AFR_CURRENT_MODULE} PRIVATE
    "${AFR_3RDPARTY_DIR}/mbedtls/library/base64.c"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE} PRIVATE
    "${AFR_3RDPARTY_DIR}/mbedtls/include"
)

# Test dependencies
if(AFR_IS_TESTING)
    afr_module_include_dirs(
        ${AFR_CURRENT_MODULE}
        PRIVATE "${test_dir}"
    )
endif()

# Link to this INTERFACE target to enable the MQTT backend.
afr_module(NAME ota_mqtt INTERFACE)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/mqtt/aws_iot_ota_cbor_internal.h"
        "${src_dir}/mqtt/aws_iot_ota_cbor.c"
        "${src_dir}/mqtt/aws_iot_ota_cbor.h"
        "${src_dir}/mqtt/aws_iot_ota_mqtt.c"
        "${src_dir}/mqtt/aws_iot_ota_mqtt.h"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt
        3rdparty::tinycbor
)

# Link to this INTERFACE target to enable the HTTP backend.
afr_module(NAME ota_http INTERFACE)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/http/aws_iot_ota_http.c"
        "${src_dir}/http/aws_iot_ota_http.h"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::https
)

# OTA test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_ota_agent.c"
        "${test_dir}/aws_test_ota_pal.c"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::ota
)
