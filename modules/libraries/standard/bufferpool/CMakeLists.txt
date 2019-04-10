afr_module(bufferpool OPTIONAL)

afr_set_lib_metadata(ID "bufferpool")
afr_set_lib_metadata(DESCRIPTION "This library implements a common pool of thread-safe buffers for use by other libraries.")
afr_set_lib_metadata(DISPLAY_NAME "Buffer Pool")
afr_set_lib_metadata(CATEGORY "Utility")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

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
