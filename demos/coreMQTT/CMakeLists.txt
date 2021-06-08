# FreeRTOS coreMQTT demos
afr_demo_module(core_mqtt)

# Set the default demo that is enabled in the downloaded package.
afr_set_demo_metadata(ID "CORE_MQTT_MUTUAL_AUTH_DEMO")
afr_set_demo_metadata(DESCRIPTION "Examples that demonstrate the MQTT library")
afr_set_demo_metadata(DISPLAY_NAME "coreMQTT Demos")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_mutual_auth.c"
        # Add the header file added to the target's metadata so that it
        # is available in code downloaded from the FreeRTOS console.
        ${AFR_DEMOS_DIR}/include/aws_iot_metrics.h
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
       "${AFR_DEMOS_DIR}/include"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::core_mqtt
        AFR::backoff_algorithm
        AFR::transport_interface_secure_sockets
        AFR::secure_sockets
        AFR::pkcs11_helpers
        # Add dependency on the core_mqtt_demo_dependencies metadata module 
        # so that FreeRTOS console shows this demo ONLY when the core MQTT library
        # (or another library depending on coreMQTT) is selected on the console.
        AFR::core_mqtt_demo_dependencies
)
