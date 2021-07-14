# Module for demo helper utilities based on coreMQTT library. 
afr_module(INTERNAL NAME mqtt_demo_helpers)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/mqtt_demo_helpers.c
        # Add the header file added to the target's metadata so that it
        # is available in code downloaded from the FreeRTOS console.
        ${AFR_DEMOS_DIR}/include/aws_iot_metrics.h
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
       "${CMAKE_CURRENT_LIST_DIR}"
       "${AFR_DEMOS_DIR}/include"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt
        AFR::transport_interface_secure_sockets
        AFR::backoff_algorithm
        AFR::pkcs11_helpers
)
