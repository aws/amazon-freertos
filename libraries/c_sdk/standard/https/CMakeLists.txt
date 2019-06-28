afr_module()

afr_set_lib_metadata(ID "http")
afr_set_lib_metadata(DESCRIPTION "This library implements the HTTP/1.1 protocol that enables \
communication with AWS IoT.")
afr_set_lib_metadata(DISPLAY_NAME "HTTP")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_https_client.c"
        "${src_dir}/iot_https_utils.c"
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
        AFR::common
        3rdparty::http-parser
)

# HTTPS Client test
afr_test_module()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/unit/iot_tests_https_client.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        INTERFACE AFR::https
)
