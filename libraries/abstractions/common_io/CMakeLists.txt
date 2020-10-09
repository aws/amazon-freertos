afr_module()

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${inc_dir}/iot_adc.h"
        "${inc_dir}/iot_battery.h"
        "${inc_dir}/iot_efuse.h"
        "${inc_dir}/iot_flash.h"
        "${inc_dir}/iot_gpio.h"
        "${inc_dir}/iot_hw.h"
        "${inc_dir}/iot_i2c.h"
        "${inc_dir}/iot_i2s.h"
        "${inc_dir}/iot_perfcounter.h"
        "${inc_dir}/iot_power.h"
        "${inc_dir}/iot_pwm.h"
        "${inc_dir}/iot_reset.h"
        "${inc_dir}/iot_rtc.h"
        "${inc_dir}/iot_sdio.h"
        "${inc_dir}/iot_spi.h"
        "${inc_dir}/iot_timer.h"
        "${inc_dir}/iot_tsensor.h"
        "${inc_dir}/iot_uart.h"
        "${inc_dir}/iot_usb_device.h"
        "${inc_dir}/iot_usb_host.h"
        "${inc_dir}/iot_watchdog.h"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC 
        "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC 
        AFR::common_io::mcu_port
)

# Common I/O tests
afr_test_module()
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_common_io_internal.h"
        "${test_dir}/iot_test_common_io.c"
        "${test_dir}/test_iot_adc.c"
        "${test_dir}/test_iot_battery.c"
        "${test_dir}/test_iot_efuse.c"
        "${test_dir}/test_iot_flash.c"
        "${test_dir}/test_iot_gpio.c"
        "${test_dir}/test_iot_i2c.c"
        "${test_dir}/test_iot_i2s.c"
        "${test_dir}/test_iot_perfcounter.c"
        "${test_dir}/test_iot_power.c"
        "${test_dir}/test_iot_pwm.c"
        "${test_dir}/test_iot_reset.c"
        "${test_dir}/test_iot_rtc.c"
        "${test_dir}/test_iot_sdio.c"
        "${test_dir}/test_iot_spi.c"
        "${test_dir}/test_iot_timer.c"
        "${test_dir}/test_iot_tsensor.c"
        "${test_dir}/test_iot_uart.c"
        "${test_dir}/test_iot_watchdog.c"
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    INTERFACE "${test_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE AFR::common_io
)
