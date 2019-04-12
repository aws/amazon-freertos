afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_logging_task_dynamic_buffers.c"
        "${src_dir}/iot_logging.c"
        "${src_dir}/private/iot_logging.h"
        "${inc_dir}/aws_logging_task.h"
        "${inc_dir}/iot_logging_setup.h"
)
