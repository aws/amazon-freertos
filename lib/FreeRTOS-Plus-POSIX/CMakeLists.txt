afr_module(posix KERNEL)

afr_module_sources(
    posix
    PRIVATE
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
)

afr_module_include_dirs(
    posix
    PUBLIC
        "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/include"
        $<TARGET_PROPERTY:AFR::kernel,INTERFACE_INCLUDE_DIRECTORIES>
        $<${AFR_IS_TESTING}:$<TARGET_PROPERTY:AFR::posix::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>>
)

afr_module_dependencies(
    posix
    PRIVATE AFR::posix::mcu_port
    PUBLIC AFR::compiler::mcu_port
)

# Link to this interface target to implicitly use standard POSIX headers from AFR, i.e., you can
# say #include <pthread.h> instead of #include <FreeRTOS_POSIX/pthread.h>.
# If you need to make sure it's put in front of other include directories, you can use generator
# expression and target_include_directories with BEFORE, e.g.,
# target_include_directories(
#     <target> BEFORE
#     PUBLIC $<TARGET_PROPERTY:AFR::use_posix,INTERFACE_INCLUDE_DIRECTORIES>
# )
afr_module(use_posix INTERFACE)
afr_module_include_dirs(
    use_posix
    INTERFACE "${AFR_MODULES_DIR}/include/FreeRTOS_POSIX"
)
afr_module_dependencies(
    use_posix
    INTERFACE AFR::posix
)
