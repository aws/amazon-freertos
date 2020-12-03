# Demo module for AWS IoT Defender for platforms using LWIP.
afr_demo_module(device_defender_lwip)

if(TARGET AFR::freertos_plus_tcp::mcu_port)
    # FreeRTOS+TCP metrics
    afr_set_demo_metadata(ID "DEVICE_DEFENDER_DEMO")
    afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the AWS IoT Defender library.")
    afr_set_demo_metadata(DISPLAY_NAME "Device Defender Demo")

    # Add the CMakeLists.txt file of module to metadata list.
    afr_module_cmake_files(${AFR_CURRENT_MODULE}
        ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
    )

    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/defender_demo.c"
            "${CMAKE_CURRENT_LIST_DIR}/report_builder.c"
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/freertos_plus_tcp/metrics_collector.c"
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
            AFR::freertos_plus_tcp_utils
            # Add dependency on the device_defender_demo_dependencies
            # metadata module so that this demo is only shown when the
            # Device Defender library is selected on the FreeRTOS console.
            AFR::device_defender_demo_dependencies
    )

elseif(${DEFENDER_METRICS_LWIP_SUPPORTED})
    # lwIP metrics
    afr_set_demo_metadata(ID "DEVICE_DEFENDER_DEMO")
    afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the AWS IoT Defender library.")
    afr_set_demo_metadata(DISPLAY_NAME "Device Defender Demo")

    # Add the CMakeLists.txt file of module to metadata list.
    afr_module_cmake_files(${AFR_CURRENT_MODULE}
        ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
    )

    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/defender_demo.c"
            "${CMAKE_CURRENT_LIST_DIR}/report_builder.c"
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/lwip/metrics_collector.c"
            # List of header files added to the target so that these are available
            # in code downloaded from the FreeRTOS console.
            "${CMAKE_CURRENT_LIST_DIR}/report_builder.h"
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector.h"
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/lwip/netif_port.h"
    )

    afr_module_include_dirs(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            ${AFR_DEMOS_DIR}/common/mqtt_demo_helpers
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/metrics_collector/lwip
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

else()
    # Stubbed metrics
    afr_set_demo_metadata(ID "DEVICE_DEFENDER_DEMO")
    afr_set_demo_metadata(DESCRIPTION "An example that demonstrates the use of the AWS IoT Defender library.")
    afr_set_demo_metadata(DISPLAY_NAME "Device Defender Demo (Stubbed metrics)")

    # Add the CMakeLists.txt file of module to metadata list.
    afr_module_cmake_files(${AFR_CURRENT_MODULE}
        ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
    )

    afr_module_sources(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/defender_demo.c"
            "${CMAKE_CURRENT_LIST_DIR}/report_builder.c"
            "${CMAKE_CURRENT_LIST_DIR}/metrics_collector/stub/metrics_collector.c"
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

endif()



