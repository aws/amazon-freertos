# We don't display this demo below in AFR console
if(AFR_METADATA_MODE)
    return()
endif()

# Posix demo
afr_demo_module(posix)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/aws_posix_demo.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::posix
)
