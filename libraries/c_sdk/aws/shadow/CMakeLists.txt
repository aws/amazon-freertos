afr_module()

afr_set_lib_metadata(ID "shadow")
afr_set_lib_metadata(DESCRIPTION "This library enables communication with AWS IoT Thing Shadows.")
afr_set_lib_metadata(DISPLAY_NAME "Thing Shadow")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_iot_shadow_api.c"
        "${src_dir}/aws_iot_shadow_operation.c"
        "${src_dir}/aws_iot_shadow_parser.c"
        "${src_dir}/aws_iot_shadow_static_memory.c"
        "${src_dir}/aws_iot_shadow_subscription.c"
        "${inc_dir}/aws_iot_shadow.h"
)

if(TARGET AFR::secure_sockets::mcu_port)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_shadow.c"
        "${src_dir}/aws_shadow_config_defaults.h"
        "${inc_dir}/aws_shadow.h"
)
endif()

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
    PRIVATE
        AFR::serializer
)

# Shadow test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/unit/aws_iot_tests_shadow_api.c"
        "${test_dir}/unit/aws_iot_tests_shadow_parser.c"
        "${test_dir}/system/aws_iot_tests_shadow_system.c"
)

if(TARGET AFR::secure_sockets::mcu_port)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_shadow.c"
)
endif()

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/test/access"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::shadow
)
