afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${inc_dir}/iot_pkcs11.h"
        "${src_dir}/iot_pkcs11.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        3rdparty::pkcs11
        AFR::pkcs11_implementation
)

