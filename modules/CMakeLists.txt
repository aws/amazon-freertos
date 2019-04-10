# Common files.
afr_module(common INTERFACE)
afr_module_sources(
    common
    INTERFACE
        "${AFR_MODULES_DIR}/include/private/aws_default_root_certificates.h"
        "${AFR_MODULES_DIR}/include/aws_appversion32.h"
)

# Extra files for delivering source files from AFR console.
afr_module(extra INTERFACE)
afr_module_sources(
    extra
    INTERFACE
        "LICENSE"
        "README.md"
        "CHANGELOG.md"
        "directories.txt"
        "${AFR_MODULES_DIR}/utils/aws_system_init.c"
        "${AFR_ROOT_DIR}/tools/certificate_configuration/PEMfileToCString.html"
        "${AFR_ROOT_DIR}/tools/certificate_configuration/CertificateConfigurator.html"
        "${AFR_ROOT_DIR}/tools/certificate_configuration/js/generator.js"
)

# FreeRTOS module.
afr_module(freertos INTERFACE KERNEL)
afr_module_sources(
    freertos
    INTERFACE
        "${AFR_MODULES_DIR}/FreeRTOS/event_groups.c"
        "${AFR_MODULES_DIR}/FreeRTOS/list.c"
        "${AFR_MODULES_DIR}/FreeRTOS/queue.c"
        "${AFR_MODULES_DIR}/FreeRTOS/stream_buffer.c"
        "${AFR_MODULES_DIR}/FreeRTOS/tasks.c"
        "${AFR_MODULES_DIR}/FreeRTOS/timers.c"
        "${AFR_MODULES_DIR}/include/event_groups.h"
        "${AFR_MODULES_DIR}/include/FreeRTOS.h"
        "${AFR_MODULES_DIR}/include/message_buffer.h"
        "${AFR_MODULES_DIR}/include/queue.h"
        "${AFR_MODULES_DIR}/include/semphr.h"
        "${AFR_MODULES_DIR}/include/stream_buffer.h"
        "${AFR_MODULES_DIR}/include/task.h"
        "${AFR_MODULES_DIR}/include/timers.h"
        "${AFR_MODULES_DIR}/include/private/deprecated_definitions.h"
        "${AFR_MODULES_DIR}/include/private/list.h"
        "${AFR_MODULES_DIR}/include/private/mpu_wrappers.h"
        "${AFR_MODULES_DIR}/include/private/portable.h"
        "${AFR_MODULES_DIR}/include/private/projdefs.h"
        "${AFR_MODULES_DIR}/include/private/stack_macros.h"
        "${AFR_MODULES_DIR}/include/private/threading_alt.h"
)
afr_module_include_dirs(
    freertos
    INTERFACE
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

# Kernel module.
afr_module(kernel INTERNAL KERNEL)

afr_module_dependencies(
    kernel
    PRIVATE
        AFR::common
        AFR::freertos
        AFR::kernel::mcu_port
    PUBLIC
        AFR::compiler::mcu_port
)

# Add FreeRTOS and mcu portable layer include path to public interface.
afr_module_include_dirs(
    kernel
    PUBLIC
        $<TARGET_PROPERTY:AFR::freertos,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:AFR::kernel::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
        $<${AFR_IS_TESTING}:${AFR_3RDPARTY_DIR}/unity/src>
)

# Add other modules.
file(GLOB afr_modules "${AFR_MODULES_DIR}/*")
foreach(module IN LISTS afr_modules)
    if(IS_DIRECTORY "${module}" AND EXISTS "${module}/CMakeLists.txt")
        afr_add_subdirectory("${module}")
    endif()
endforeach()
