# The single source of truth for the transport_interface.h is in the coreMQTT repository.
# In the FreeRTOS Device Software download page in AWS IoT Core, it is possible
# to choose coreHTTP instead of coreMQTT. In this case, it is necessary to check
# for the existence of the file in the downloaded software package.
if( EXISTS ${AFR_MODULES_DIR}/coreMQTT/source )
    set(transport_interface_dir "${AFR_MODULES_DIR}/coreMQTT/source/interface")
elseif( EXISTS ${AFR_MODULES_DIR}/coreHTTP/source )
    set(transport_interface_dir "${AFR_MODULES_DIR}/coreHTTP/source/interface")
else()
    message( FATAL_ERROR "No transport_interface.h exists for this included interface.")
endif()
