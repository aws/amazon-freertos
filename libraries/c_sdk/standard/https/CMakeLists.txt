afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

# Enable test access if building tests.
if(${AFR_IS_TESTING})
    list(APPEND extra_https_test_includes "${test_dir}/access")
endif()

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
    PRIVATE
        ${extra_https_test_includes}
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
        AFR::core_http
        # This dependency is required for auto-including
        # coreHTTP library on FreeRTOS console when a library
        # depends on AFR::https.
        # AFR::core_http_demo_dependencies is the visible library
        # target on FreeRTOS console.
        AFR::core_http_demo_dependencies
)

# HTTPS Client test
afr_test_module()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/unit/iot_tests_https_client.c"
        "${test_dir}/unit/iot_tests_https_utils.c"
        "${test_dir}/unit/iot_tests_https_common.c"
        "${test_dir}/unit/iot_tests_https_sync.c"
        "${test_dir}/unit/iot_tests_https_async.c"
        "${test_dir}/system/iot_tests_https_system.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/access"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::https
)
