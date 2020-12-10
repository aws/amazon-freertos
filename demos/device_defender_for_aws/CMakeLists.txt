# Demo module for AWS IoT Defender.
afr_demo_module(device_defender)

afr_set_demo_metadata(ID "DEVICE_DEFENDER_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the AWS IoT Defender library.")

# Add the CMakeLists.txt file of module to metadata list.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/defender_demo.c"
        "${CMAKE_CURRENT_LIST_DIR}/report_builder.c"
        # List of header files added to the target so that these are available
        # in code downloaded from the FreeRTOS console.
        "${CMAKE_CURRENT_LIST_DIR}/report_builder.h"
        "${CMAKE_CURRENT_LIST_DIR}/metrics_collector.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        ${AFR_DEMOS_DIR}/common/mqtt_demo_helpers
        ${CMAKE_CURRENT_LIST_DIR}
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::mqtt_demo_helpers
        AFR::core_json
        AFR::device_defender
        # Add dependency on the device_defender_demo_dependencies
        # metadata module so that this demo is only shown when the
        # Device Defender library is selected on the FreeRTOS console.
        AFR::device_defender_demo_dependencies
)

# The device defender demo collectes metrics using functions defined in
# metrics_collector.h. We provide an implementation of these functions for
# FreeRTOS+TCP and for LWIP in core locking mode.
# We check the availablitiy of FreeRTOS+TCP or supported lwIP in order to
# provide our metrics_collector implementations.
# If neither is available we include a stub implementation.

# Metrics are supported on all FreeRTOS+TCP configurations, so if it is
# available we use it.
if(TARGET AFR::freertos_plus_tcp::mcu_port)
    # Demo using FreeRTOS+TCP metrics
    afr_set_demo_metadata(DISPLAY_NAME "Device Defender Demo (FreeRTOS+TCP)")

    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/freertos_plus_tcp/metrics_collector.c"
    )

    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            AFR::freertos_plus_tcp_utils
    )

# The DEFENDER_METRICS_LWIP_SUPPORTED flag is defined in board configurations
# which support LWIP metrics collection.
elseif(${DEFENDER_METRICS_LWIP_SUPPORTED})
    # Demo using LWIP metrics
    afr_set_demo_metadata(DISPLAY_NAME "Device Defender Demo (LWIP)")

    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/lwip/metrics_collector.c"
            # List of header files added to the target so that these are available
            # in code downloaded from the FreeRTOS console.
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/lwip/netif_port.h"
    )

    afr_module_include_dirs(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/metrics_collector/lwip
    )

# If both previous checks failed, we do not have an implemented
# metrics_collector available, and thus we use the stub implementation.
else()
    # Demo using stubbed metrics for boards that do not support either
    # FreeRTOS+TCP or lwIP.
    afr_set_demo_metadata(DISPLAY_NAME "Device Defender Demo (Stubbed metrics)")

    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/stub/metrics_collector.c"
    )

endif()
