# We don't display the 2 demos below in AFR console
if(AFR_NON_BUILD_MODE)
    return()
endif()

# TCP echo demo
# TODO, tcp echo client separate tasks is not supported in NXP
afr_demo_module(demo_tcp)
afr_set_demo_metadata(ID "tcp_echo_server_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to create a simple TCP Echo server")
afr_set_demo_metadata(DISPLAY_NAME "TCP Echo Server")

afr_module_sources(
    demo_tcp
    INTERFACE
        #"${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_separate_tasks.c"
        "${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c"
        "${AFR_DEMOS_DIR}/include/aws_tcp_echo_client_single_tasks.h"
)
afr_module_dependencies(
    demo_tcp
    INTERFACE
        AFR::secure_sockets
)

afr_set_demo_metadata(ID "tcp_echo_client_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to use the secure sockets layer to connect to \
a TCP Echo server")
afr_set_demo_metadata(DISPLAY_NAME "TCP Echo Client")
