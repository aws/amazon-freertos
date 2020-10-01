# CLI demo using UART
afr_demo_module(cli_uart)

afr_set_demo_metadata(ID "CLI_UART_DEMO")
afr_set_demo_metadata(DESCRIPTION "An example that demonstrates FreeRTOS-Plus-CLI using UART")
afr_set_demo_metadata(DISPLAY_NAME "UART FreeRTOS-Plus-CLI Demo")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/cli_uart_demo.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::freertos_cli_plus_uart
)
