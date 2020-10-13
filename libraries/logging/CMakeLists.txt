afr_module(logging)

set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

# TODO, this is a workaround to remove iot_logging_task_dynamic_buffers.c from common, because
# winsim use a different logging implementation.
if(NOT AFR_BOARD MATCHES "pc.windows|microchip.ecc608a_plus_winsim")
    set(aws_logging_task "${CMAKE_CURRENT_LIST_DIR}/iot_logging_task_dynamic_buffers.c")
endif()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
       # Logging
        "${aws_logging_task}"
        "${CMAKE_CURRENT_LIST_DIR}/iot_logging.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::platform
)
