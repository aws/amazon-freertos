# -------------------------------------------------------------------------------------------------
# Demos
# -------------------------------------------------------------------------------------------------
# Base demo target
afr_demo_module(base)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${AFR_DEMOS_DIR}/demo_runner/aws_demo_network_addr.c"
        "${AFR_DEMOS_DIR}/demo_runner/aws_demo_version.c"
        "${AFR_DEMOS_DIR}/demo_runner/aws_demo.c"
        "${AFR_DEMOS_DIR}/demo_runner/iot_demo_afr.c"
        "${AFR_DEMOS_DIR}/demo_runner/iot_demo_runner.c"
        "${AFR_DEMOS_DIR}/include/aws_demo.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential_keys.h"
        "${AFR_DEMOS_DIR}/include/aws_application_version.h"
        "${AFR_DEMOS_DIR}/include/iot_demo_runner.h"
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${AFR_DEMOS_DIR}/include"
        "${AFR_DEMOS_DIR}/network_manager"
        "${AFR_MODULES_PORTS_DIR}/bluetooth_low_energy/include/private"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::kernel
        AFR::provisioning
        AFR::common
)

# Add other modules.
file(GLOB afr_demos "${AFR_DEMOS_DIR}/*")
foreach(demo IN LISTS afr_demos)
    if(IS_DIRECTORY "${demo}" AND EXISTS "${demo}/CMakeLists.txt")
        afr_add_subdirectory("${demo}")
    endif()
endforeach()
