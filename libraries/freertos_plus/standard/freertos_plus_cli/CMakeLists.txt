afr_module()

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/source")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(unit_test_dir "${CMAKE_CURRENT_LIST_DIR}/utest")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/FreeRTOS_CLI.c"
        "${src_dir}/FreeRTOS_CLI_Console.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
)

afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${unit_test_dir}/iot_test_freertos_cli.c"
        "${unit_test_dir}/iot_test_freertos_cli_console.c"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::freertos_plus_cli
)

afr_module(NAME freertos_cli_plus_uart)

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(src_dir "${CMAKE_CURRENT_LIST_DIR}/uart")

afr_module_sources(
    freertos_cli_plus_uart
    PRIVATE
        "${src_dir}/FreeRTOS_CLI_UART.c"
)

afr_module_include_dirs(
    freertos_cli_plus_uart
    PUBLIC "${inc_dir}"
)

afr_module_dependencies(
    freertos_cli_plus_uart
    PUBLIC
        AFR::freertos_plus_cli
        AFR::common_io
)
