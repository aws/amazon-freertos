if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()

# Transport Interface 
afr_module(NAME transport_interface_secure_sockets)

# Metadata for the module.
afr_set_lib_metadata(ID "transport_interface_secure_sockets")
afr_set_lib_metadata(DESCRIPTION "This library implements the transport interface using Secure Sockets.")
afr_set_lib_metadata(DISPLAY_NAME "Secure Sockets based Transport Interface")
afr_set_lib_metadata(CATEGORY "Connectivity")
afr_set_lib_metadata(VERSION "1.0.0")
afr_set_lib_metadata(IS_VISIBLE "false")

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/secure_sockets")

# Include filepaths for source and include.
include( ${CMAKE_CURRENT_LIST_DIR}/transport_interface.cmake )

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${src_dir}/transport_secure_sockets.h"
        "${src_dir}/transport_secure_sockets.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::secure_sockets
        AFR::common
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
       "${transport_interface_dir}"
       "${src_dir}"
)

