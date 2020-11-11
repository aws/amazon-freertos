# Demo module for AWS IoT Jobs library.
afr_demo_module(jobs)

afr_set_demo_metadata(ID "JOBS_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the AWS IoT Jobs library.")
afr_set_demo_metadata(DISPLAY_NAME "Jobs Demo")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/jobs_demo.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        ${AFR_DEMOS_DIR}/common/mqtt_demo_helpers
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt_demo_helpers
        AFR::core_json
        AFR::jobs
        # Add dependency on the device_shadow_demo_dependencies 
        # metadata module so that this demo is only shown when the
        # Device Shadow library is selected on the FreeRTOS console.
        AFR::jobs_demo_dependencies
)
