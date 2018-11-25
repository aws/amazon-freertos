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
        add_subdirectory("${module}")
    endif()
endforeach()
