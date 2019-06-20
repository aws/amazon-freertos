# TCP echo demo
afr_demo_module(tcp)

afr_set_demo_metadata(ID "TCP_ECHO_CLIENT_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example on how to use the secure sockets layer to connect to \
a TCP Echo server")
afr_set_demo_metadata(DISPLAY_NAME "TCP Echo Client")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        # TODO, tcp echo client separate tasks is not supported in NXP
        "${CMAKE_CURRENT_LIST_DIR}/aws_tcp_echo_client_single_task.c"
        "${CMAKE_CURRENT_LIST_DIR}/aws_tcp_echo_client_single_tasks.h"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::secure_sockets
)
