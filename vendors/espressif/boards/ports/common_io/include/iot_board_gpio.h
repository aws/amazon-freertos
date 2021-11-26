// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#include "driver/gpio.h"

#define ESP_GPIO_PORTS ( 2 )
#define ESP_GPIO_PORT0 ( 0 )
#define ESP_GPIO_PORT1 ( 1 )
#define ESP_GPIO_PORT2 ( 2 )

#define ESP_MAX_UART_PORTS 3
#define ESP_MAX_I2C_PORTS 2
#define ESP_MAX_LED_NUM 3
#define ESP_MAX_PWM_CHANNELS 3 /*Should be less then LEDC_CHANNEL_MAX(0-7)*/
#define ESP_MAX_SPI_PORTS 2

struct esp_uart_pin_config {
    uint8_t txd_pin;
    uint8_t rxd_pin;
};

struct esp_i2c_pin_config {
    uint8_t scl_pin;
    uint8_t sda_pin;
};

struct esp_led_pin_config {
    uint8_t led_num;
};

struct esp_pwm_pin_config {
    uint8_t pwm_num;
};

struct esp_spi_pin_config {
    uint8_t mosi_pin;
    uint8_t miso_pin;
    uint8_t clk_pin;
    uint8_t cs_pin;
};

/* Configuration to be assigned internally for UART to pin mappings */
#define ESP_UART_PIN_MAP  {       \
    /* UART 0 */                  \
    { GPIO_NUM_1, GPIO_NUM_3 },   \
    /* UART 1 */                  \
    { GPIO_NUM_33, GPIO_NUM_34 }, \
    /* UART 2 */                  \
    { GPIO_NUM_17, GPIO_NUM_16 }, \
}

/* Configuration to be assigned internally for I2C to pin mappings */
#define ESP_I2C_PIN_MAP  {       \
    /* I2C 0 */                  \
    { GPIO_NUM_26, GPIO_NUM_27 },   \
    /* I2C 1 */                  \
    { GPIO_NUM_19, GPIO_NUM_21 }, \
}

/* Configuration to be assigned internally for LED to pin mappings */
#define ESP_LED_PIN_MAP  {  \
    {GPIO_NUM_10},  \
    {GPIO_NUM_11},  \
    {GPIO_NUM_12},  \
}

/* Configuration to be assigned internally for PWM to pin mappings */
#define ESP_PWM_PIN_MAP  {  \
    {GPIO_NUM_5},  \
    {GPIO_NUM_0},  \
    {GPIO_NUM_27}, \
}

/* Configuration to be assigned internally for SPI to pin mappings */
#define ESP_SPI_PIN_MAP  {  \
    /* SPI 1: MOSI, MISO, CLK, CS */ \
    { GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15 },  \
    /* SPI 2 */ \
    { GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15 },  \
}

/* define GPIO ports used for AFQP test */
#define aws_hal_GPIO_TEST_PORT_A ( 3 )
#define aws_hal_GPIO_TEST_PORT_B ( 4 )

#endif /* _BOARD_GPIO_H_ */

