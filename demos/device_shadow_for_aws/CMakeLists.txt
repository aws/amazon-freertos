# Device Shadow demo module.
afr_demo_module(device_shadow)

afr_set_demo_metadata(ID "DEVICE_SHADOW_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the Device Shadow library.")
afr_set_demo_metadata(DISPLAY_NAME "Device Shadow Demo")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_main.c"
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_helpers.c"
        # Add the header file to generate their metadata so that 
        # they are present in code downloaded from FreeRTOS console.
        "${CMAKE_CURRENT_LIST_DIR}/shadow_demo_helpers.h"
        # As the containing directory name (i.e. device_shadow_for_aws)
        # does not match the module name (i.e. device_shadow), 
        # we add the CMake file to the source list so that metadata is
        # generated for it, and it is present in code downloaded from
        # the FreeRTOS console.
        ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::core_json
        AFR::device_shadow
        AFR::retry_utils
        AFR::transport_interface_secure_sockets
        # Add dependency on the device_shadow_demo_dependencies 
        # metadata module so that this demo is only shown when the
        # Device Shadow library is selected on the FreeRTOS console.
        AFR::device_shadow_demo_dependencies
)
