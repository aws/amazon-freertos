# Link to this INTERFACE target to use the default implementation based on FreeRTOS.
afr_module(INTERFACE)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/source")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/FreeRTOS_POSIX_clock.c"
        "${src_dir}/FreeRTOS_POSIX_mqueue.c"
        "${src_dir}/FreeRTOS_POSIX_pthread_barrier.c"
        "${src_dir}/FreeRTOS_POSIX_pthread.c"
        "${src_dir}/FreeRTOS_POSIX_pthread_cond.c"
        "${src_dir}/FreeRTOS_POSIX_pthread_mutex.c"
        "${src_dir}/FreeRTOS_POSIX_sched.c"
        "${src_dir}/FreeRTOS_POSIX_semaphore.c"
        "${src_dir}/FreeRTOS_POSIX_timer.c"
        "${src_dir}/FreeRTOS_POSIX_unistd.c"
        "${src_dir}/FreeRTOS_POSIX_utils.c"
        "${inc_dir}/FreeRTOS_POSIX.h"
        "${inc_dir}/FreeRTOS_POSIX_types.h"
        "${inc_dir}/FreeRTOS_POSIX_internal.h"
        "${inc_dir}/FreeRTOS_POSIX_portable_default.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${inc_dir}"
        # Requires common/include/private/iot_doubly_linked_list.h
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
)

# Test
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_posix_clock.c"
        "${test_dir}/iot_test_posix_mqueue.c"
        "${test_dir}/iot_test_posix_pthread.c"
        "${test_dir}/iot_test_posix_semaphore.c"
        "${test_dir}/iot_test_posix_stress.c"
        "${test_dir}/iot_test_posix_timer.c"
        "${test_dir}/iot_test_posix_unistd.c"
        "${test_dir}/iot_test_posix_utils.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::posix
)
