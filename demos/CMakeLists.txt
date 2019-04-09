# -------------------------------------------------------------------------------------------------
# Demos
# -------------------------------------------------------------------------------------------------
# Base demo target
afr_demo_module(demo_base)
afr_module_sources(
    demo_base
    INTERFACE
        "${AFR_DEMOS_DIR}/demo_runner/aws_demo_runner.c"
        "${AFR_DEMOS_DIR}/demo_runner/iot_demo_afr.c"
        "${AFR_DEMOS_DIR}/include/aws_demo.h"
        "${AFR_DEMOS_DIR}/include/aws_demo_runner.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential.h"
        "${AFR_DEMOS_DIR}/include/aws_clientcredential_keys.h"
        "${AFR_DEMOS_DIR}/include/aws_application_version.h"
)
afr_module_include_dirs(
    demo_base
    INTERFACE
        "${AFR_DEMOS_DIR}/include"
)
afr_module_dependencies(
    demo_base
    INTERFACE
        AFR::kernel
        AFR::provisioning
)

# Add other modules.
file(GLOB afr_demos "${AFR_DEMOS_DIR}/*")
foreach(demo IN LISTS afr_demos)
    if(IS_DIRECTORY "${demo}" AND EXISTS "${demo}/CMakeLists.txt")
        afr_add_subdirectory("${demo}")
    endif()
endforeach()
