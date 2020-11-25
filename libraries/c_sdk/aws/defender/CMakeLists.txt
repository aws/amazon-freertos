afr_module()

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
        "$<${AFR_IS_TESTING}:${src_dir}>"
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
        "${test_dir}/unit/aws_iot_tests_defender_unit.c"
        "${test_dir}/system/aws_iot_tests_defender_system.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::defender
)
