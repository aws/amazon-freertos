# Transport Interface 

if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()

afr_module()

afr_set_lib_metadata(ID "transport")
afr_set_lib_metadata(DESCRIPTION "Transport Interface over Secure Sockets")
afr_set_lib_metadata(DISPLAY_NAME "SECURE_SOCKETS_TRANSPORT")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "true")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/secure_sockets")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")


afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${src_dir}/transport_secure_sockets.c"
        "${src_dir}/transport_secure_sockets.h"
        "${inc_dir}/transport_interface.h"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::secure_sockets
        AFR::common
    INTERFACE
        AFR::transport
)
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
       "${inc_dir}"
       "${src_dir}"
       "${AFR_MODULES_C_SDK_DIR}/standard/logging_stack/"
)

