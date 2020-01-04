afr_module(INTERFACE)

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${inc_dir}/iot_i2c.h"
        "${inc_dir}/iot_uart.h"
        "${inc_dir}/iot_spi.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::common_io::mcu_port
)

# Common I/O tests
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_common_io.c"
        "${test_dir}/test_iot_i2c.c"
        "${test_dir}/test_iot_uart.c"
        "${test_dir}/test_iot_spi.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${test_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::common_io
)
