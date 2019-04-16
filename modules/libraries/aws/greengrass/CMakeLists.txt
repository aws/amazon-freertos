afr_module()

afr_set_lib_metadata(ID "greengrass")
afr_set_lib_metadata(DESCRIPTION "This library enables the discovery and selection of AWS Greengrass Core devices.")
afr_set_lib_metadata(DISPLAY_NAME "Greengrass Discovery")
afr_set_lib_metadata(CATEGORY "Amazon Services")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_greengrass_discovery.c"
        "${src_dir}/aws_helper_secure_connect.c"
        "${src_dir}/aws_helper_secure_connect.h"
        "${inc_dir}/aws_ggd_config_defaults.h"
        "${inc_dir}/aws_greengrass_discovery.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}"
        "$<${AFR_IS_TESTING}:${src_dir}>"
    PRIVATE
        "$<${AFR_IS_TESTING}:${test_dir}>"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::secure_sockets
    PRIVATE
        AFR::mqtt
        3rdparty::jsmn
)

# Greengrass test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/aws_test_greengrass_discovery.c"
        "${test_dir}/aws_test_helper_secure_connect.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::greengrass
        3rdparty::jsmn
)
