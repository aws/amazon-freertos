# CLI demo using UART
afr_demo_module(cli_uart)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/cli_uart_demo.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::freertos_plus_cli
        AFR::common_io
        AFR::freertos_cli_plus_uart
)
