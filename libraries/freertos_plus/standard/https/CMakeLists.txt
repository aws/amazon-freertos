afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/iot_https_client.c"
        "${src_dir}/iot_https_utils.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC 
        "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::common
        3rdparty::http-parser
)
