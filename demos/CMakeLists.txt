# -------------------------------------------------------------------------------------------------
# Demos
# -------------------------------------------------------------------------------------------------
# Base demo target
afr_demo_module(base)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "$<$<TARGET_EXISTS:AFR::freertos_plus_tcp>:${AFR_DEMOS_DIR}/demo_runner/aws_demo_network_addr.c>"
        "${AFR_DEMOS_DIR}/demo_runner/aws_demo.c"
        "${AFR_DEMOS_DIR}/network_manager/aws_iot_network_manager.c"
        "${AFR_DEMOS_DIR}/network_manager/iot_network_manager_private.h"
        "${AFR_DEMOS_DIR}/demo_runner/iot_demo_freertos.c"
        "${AFR_DEMOS_DIR}/demo_runner/iot_demo_runner.c"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential_keys.h"
        "${AFR_DEMOS_DIR}/include/aws_demo.h"
        "${AFR_DEMOS_DIR}/include/iot_config_common.h"
        "${AFR_DEMOS_DIR}/include/iot_demo_logging.h"
        "${AFR_DEMOS_DIR}/include/iot_demo_runner.h"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE 
        "${AFR_DEMOS_DIR}/include"
        "${AFR_DEMOS_DIR}/network_manager"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::kernel
        AFR::common
)
if(TARGET AFR::pkcs11_implementation::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        INTERFACE
            AFR::dev_mode_key_provisioning
    )
endif()

if(TARGET AFR::wifi::mcu_port AND TARGET AFR::ble_hal::mcu_port)
    afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        INTERFACE
        AFR::ble_wifi_provisioning
    )
endif()

# Add other modules.
file(GLOB_RECURSE afr_demos LIST_DIRECTORIES true "${AFR_DEMOS_DIR}/*")
foreach(demo IN LISTS afr_demos)
    if(IS_DIRECTORY "${demo}" AND EXISTS "${demo}/CMakeLists.txt")
        afr_add_subdirectory("${demo}")
    endif()
endforeach()
