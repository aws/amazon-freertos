afr_module(bufferpool INTERNAL)

afr_module_sources(
    bufferpool
    PRIVATE
        "${AFR_MODULES_DIR}/bufferpool/aws_bufferpool_static_thread_safe.c"
        "${AFR_MODULES_DIR}/include/private/aws_bufferpool.h"
)

afr_module_include_dirs(
    bufferpool
    PUBLIC
        "${AFR_MODULES_DIR}/include/private"
)
