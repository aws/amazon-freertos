# FreeRTOS module.

if (AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(abstractions/secure_sockets)
    add_subdirectory(freertos_plus/standard/pkcs11/)
    add_subdirectory(freertos_plus/standard/utils/)
    add_subdirectory(abstractions/pkcs11/)
    add_subdirectory(c_sdk/standard/ble)
    return()
endif()

afr_module(NAME freertos INTERFACE KERNEL)
afr_glob_src(freertos_inc DIRECTORY "${AFR_KERNEL_DIR}/include" RECURSE)
afr_module_sources(
    freertos
    INTERFACE
        "${AFR_KERNEL_DIR}/event_groups.c"
        "${AFR_KERNEL_DIR}/list.c"
        "${AFR_KERNEL_DIR}/queue.c"
        "${AFR_KERNEL_DIR}/stream_buffer.c"
        "${AFR_KERNEL_DIR}/tasks.c"
        "${AFR_KERNEL_DIR}/timers.c"
        ${freertos_inc}
)
afr_module_include_dirs(
    freertos
    INTERFACE
        "${AFR_KERNEL_DIR}/include"
)

# Kernel module.
afr_module(NAME kernel INTERNAL KERNEL)

afr_module_dependencies(
    kernel
    PRIVATE
        AFR::freertos
        AFR::kernel::mcu_port
    PUBLIC
        AFR::compiler::mcu_port
)

# Add FreeRTOS and mcu portable layer include path to kernel's public interface.
afr_module_include_dirs(
    kernel
    PUBLIC
        $<TARGET_PROPERTY:AFR::freertos,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:AFR::kernel::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
        $<${AFR_IS_TESTING}:${AFR_3RDPARTY_DIR}/unity/src>
)

# Add other modules.
file(GLOB afr_modules_csdk "${AFR_MODULES_DIR}/c_sdk/*/*")
file(GLOB afr_modules_freertos_plus "${AFR_MODULES_DIR}/freertos_plus/*/*")
file(GLOB afr_modules_abstractions "${AFR_MODULES_DIR}/abstractions/*")
foreach(module IN LISTS afr_modules_csdk afr_modules_freertos_plus afr_modules_abstractions)
    if(IS_DIRECTORY "${module}" AND EXISTS "${module}/CMakeLists.txt")
        afr_add_subdirectory("${module}")
    endif()
endforeach()
