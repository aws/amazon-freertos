if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()
afr_module(INTERFACE)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${src_dir}/iot_system_init.c"
        "${inc_dir}/iot_system_init.h"
        "${src_dir}/iot_pki_utils.c"
        "${inc_dir}/iot_pki_utils.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::secure_sockets
    AFR::crypto
    3rdparty::mbedtls
)
