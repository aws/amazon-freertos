if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()

# Transport Interface
afr_module(NAME transport_interface_secure_sockets INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/secure_sockets")

# Include filepaths for source and include.
include( ${CMAKE_CURRENT_LIST_DIR}/transport_interface.cmake )

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/transport_interface.cmake
    ${CMAKE_CURRENT_LIST_DIR}/transport_interface_secure_sockets.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
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

