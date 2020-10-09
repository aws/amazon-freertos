# Transport Interface 
afr_module(NAME transport_interface_secure_sockets)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/secure_sockets")

# The single source of truth for the transport_interface.h is in the coreMQTT repository.
# In the FreeRTOS Device Software download page in AWS IoT Core, it is possible
# to choose coreHTTP instead of coreMQTT. In this case, it is necessary to check
# for the existence of the file in the downloaded software package.
if( EXISTS ${AFR_MODULES_DIR}/coreMQTT/source )
    set(transport_interface_dir "${AFR_MODULES_DIR}/coreMQTT/source/portable")
elseif( EXISTS ${AFR_MODULES_DIR}/coreHTTP/source )
    set(transport_interface_dir "${AFR_MODULES_DIR}/coreHTTP/source/portable")
else()
    message( FATAL_ERROR "No transport_interface.h exists for this included interface.")
endif()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC
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

