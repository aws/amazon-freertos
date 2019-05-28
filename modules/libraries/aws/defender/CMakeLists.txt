afr_module()

afr_set_lib_metadata(ID "defender")
afr_set_lib_metadata(DESCRIPTION "This library enables metrics reporting with AWS IoT Device Defender.")
afr_set_lib_metadata(DISPLAY_NAME "Device Defender")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_iot_defender_api.c"
        "${src_dir}/aws_iot_defender_collector.c"
        "${src_dir}/aws_iot_defender_mqtt.c"
        "${src_dir}/aws_iot_defender_v1.c"
        "${src_dir}/private/aws_iot_defender_internal.h"
        "${inc_dir}/aws_iot_defender.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::mqtt
        AFR::serializer
)

# Defender test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_iot_tests_defender_api.c"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/private"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::defender
)
