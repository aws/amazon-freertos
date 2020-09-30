#ifndef IOT_TEST_COMMON_IO_CONFIG_H
#define IOT_TEST_COMMON_IO_CONFIG_H

#include <iot_i2c.h>
#include <iot_uart.h>
#include <iot_spi.h>
#include <iot_gpio.h>
#include <iot_timer.h>
#include <iot_flash.h>
#include <iot_pwm.h>
#include <iot_rtc.h>
#include <iot_watchdog.h>

/* Supported on LaunchPad */
#define IOT_TEST_COMMON_IO_I2C_SUPPORTED 1
#define IOT_TEST_COMMON_IO_I2C_SUPPORTED_CANCEL 1
#define IOT_TEST_COMMON_IO_SPI_SUPPORTED 1
#define IOT_TEST_COMMON_IO_UART_SUPPORTED 1
#define IOT_TEST_COMMON_IO_UART_SUPPORTED_CANCEL 1
#define IOT_TEST_COMMON_IO_GPIO_SUPPORTED 2
#define IOT_TEST_COMMON_IO_TIMER_SUPPORTED 1
#define IOT_TEST_COMMON_IO_PWM_SUPPORTED 1
#define IOT_TEST_COMMON_IO_RTC_SUPPORTED 1
#define IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED 1

/* No external Flash to test agains on the LaunchPad */
#define IOT_TEST_COMMON_IO_FLASH_SUPPORTED 0

/* Not supported by the device */
#define IOT_TEST_COMMON_IO_PERFCOUNTER_SUPPORTED 0
#define IOT_TEST_COMMON_IO_ADC_SUPPORTED 0
#define IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED 0
#define IOT_TEST_COMMON_IO_POWER_SUPPORTED 0

#endif
