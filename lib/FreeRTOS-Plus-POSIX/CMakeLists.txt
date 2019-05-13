afr_module(NAME posix KERNEL)

afr_glob_src(posix_std_headers DIRECTORY "${AFR_MODULES_DIR}/include/FreeRTOS_POSIX" RECURSE)
afr_module_sources(
    posix PUBLIC ${posix_std_headers}
)

# If you need to make sure the this POSIX include dir is put in front of other include directories,
# you can use generator expression and target_include_directories with BEFORE, e.g.,
# target_include_directories(
#     <target> BEFORE
#     PUBLIC $<TARGET_PROPERTY:AFR::posix,INTERFACE_INCLUDE_DIRECTORIES>
# )
afr_module_include_dirs(
    posix
    PUBLIC
        "${AFR_MODULES_DIR}/include/FreeRTOS_POSIX"
        $<${AFR_IS_TESTING}:$<TARGET_PROPERTY:AFR::posix::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>>
    PRIVATE
        $<TARGET_PROPERTY:AFR::kernel,INTERFACE_INCLUDE_DIRECTORIES>
)

afr_module_dependencies(
    posix
    PRIVATE
        AFR::posix::mcu_port
        AFR::compiler::mcu_port
)

# Link to this INTERFACE target to use the default implementation based on FreeRTOS.
afr_module(NAME freertos_plus_posix INTERFACE INTERNAL)

afr_module_sources(
    freertos_plus_posix
    INTERFACE
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/include/FreeRTOS_POSIX.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/include/FreeRTOS_POSIX_internal.h"
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/include/portable/FreeRTOS_POSIX_portable_default.h"
)

afr_module_include_dirs(
    freertos_plus_posix
    INTERFACE
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/include"
)
