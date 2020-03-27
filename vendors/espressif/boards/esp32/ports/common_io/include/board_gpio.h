/*
 * Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
 * All rights reserved.
 *
 * You may not use this file except in compliance with the terms and
 * conditions set forth in the accompanying LICENSE.TXT file.
 *
 * THESE MATERIALS ARE PROVIDED ON AN "AS IS" BASIS. ESPRESSIF/AMAZON SPECIFICALLY
 * DISCLAIMS, WITH RESPECT TO THESE MATERIALS, ALL WARRANTIES, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 */

#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

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
    /* UART 1: this ACK team's preference */  \
    { GPIO_NUM_19, GPIO_NUM_22 }, \
    /* UART 2 */                  \
    { GPIO_NUM_17, GPIO_NUM_16 }, \
}

/* Configuration to be assigned internally for I2C to pin mappings */
#define ESP_I2C_PIN_MAP  {       \
    /* I2C 0 */                  \
    { GPIO_NUM_25, GPIO_NUM_26 },   \
    /* I2C 1 */                  \
    { GPIO_NUM_19, GPIO_NUM_22 }, \
}

/* Configuration to be assigned internally for LED to pin mappings */
#define ESP_LED_PIN_MAP  {  \
    {GPIO_NUM_25},  \
    {GPIO_NUM_22},  \
    {GPIO_NUM_26},  \
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

