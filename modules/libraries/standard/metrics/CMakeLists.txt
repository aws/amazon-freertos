afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/aws_secure_sockets_wrapper_metrics.c"
        "${src_dir}/iot_metrics.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        # Needs standard/common/include/private/aws_lib_init.h
        "${AFR_MODULES_STANDARD_DIR}/common/include/private"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
        AFR::secure_sockets
)
