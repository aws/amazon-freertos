# Device Shadow demo module.
afr_demo_module(device_shadow)

afr_set_demo_metadata(ID "DEVICE_SHADOW_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the Device Shadow library.")
afr_set_demo_metadata(DISPLAY_NAME "Device Shadow Demo")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_main.c"
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
        AFR::device_shadow
        # Add dependency on the device_shadow_demo_dependencies 
        # metadata module so that this demo is only shown when the
        # Device Shadow library is selected on the FreeRTOS console.
        AFR::device_shadow_demo_dependencies
)
