# Posix demo
afr_demo_module(posix)

afr_set_demo_metadata(ID "POSIX_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates Posix")
afr_set_demo_metadata(DISPLAY_NAME "Posix Demo")

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
