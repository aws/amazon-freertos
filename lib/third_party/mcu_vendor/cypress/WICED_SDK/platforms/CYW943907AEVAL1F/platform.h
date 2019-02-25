/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

#pragma once

#include "platform_sleep.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* UART port used for standard I/O */
#ifdef PLATFORM_WL_UART_ENABLED
#define STDIO_UART  ( WICED_UART_3 )
#else
#define STDIO_UART  ( WICED_UART_1 )
#endif /* PLATFORM_WL_UART_ENABLED */

/******************************************************
 *                   Enumerations
 ******************************************************/

/*
CYW943907AEVAL1F platform pin definitions ...
+--------------------------------------------------------------------------------------------------------+--------------+--------------------------------------------------+
| Enum ID       |Pin  |   Pin Name   | Mod |  Module Pin  Name       |  Board Net Name          | Header | Function     | Board Connection              | WICED Peripheral |
|               | #   |   on 43907   | Pin#|                         |                          | Conn   |              |                               | Alias            |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_1  | 141 | GPIO_0       | C11 | GPIO_0                  | GPIO_0                   | J10:1  |  GPIO        | ARD_GPIO0                     |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_2  | 142 | GPIO_1       | A30 | GPIO_1_GSPI_MODE        | GPIO_1_GSPI_MODE         | J10:2  |  GPIO        | ARD_GPIO1                     |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_3  | 95  | GPIO_7       | A8  | GPIO_7_WCPU_BOOT_MODE   | GPIO_7_WCPU_BOOT_MODE    | J10:4  |  GPIO        | ARD_GPIO3 (PWM)               | WICED_PWM_6      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_4  | 134 | GPIO_8       | C16 | GPIO_8_TAP_SEL          | GPIO_8_TAP_SEL           | N/C    |  GPIO        | USER_2 Button                 | WICED_BUTTON1    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_5  | 94  | GPIO_9       | C15 | GPIO_9_USB_SEL          | GPIO_9                   | J12:6  |  GPIO        | ARD_SCK/LED_2                 | WICED_LED2       |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_6  | 146 | GPIO_10      | A45 | GPIO_10                 | GPIO_10                  | J12:4  |  GPIO        | ARD_MOSI (PWM)                | WICED_PWM_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_7  | 140 | GPIO_11      | C4  | GPIO_11_ACPU_BOOT_MODE  | GPIO_11_ACPU_BOOT_MODE   | J12:3  |  GPIO        | ARD_SS (PWM)                  | WICED_PWM_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_8  | 143 | GPIO_12      | C14 | GPIO_12                 | GPIO_12                  | J12:5  |  GPIO        | ARD_MISO                      |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_9  | 131 | GPIO_13      | B28 | GPIO_13_SDIO_MODE       | GPIO_13_SDIO_MODE        | J10:3  |  GPIO        | ARD_GPIO2                     |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_10 | 130 | GPIO_14      | D8  | GPIO_14                 | GPIO_14                  | J10:5  |  GPIO        | ARD_GPIO4                     |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_11 | 145 | GPIO_15      | B26 | GPIO_15                 | GPIO_15                  | J10:7  |  GPIO        | ARD_GPIO6 (PWM)               | WICED_PWM_4      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_12 | 137 | GPIO_16      | B19 | GPIO_16                 | GPIO_16                  | J10:6  |  GPIO        | ARD_GPIO5 (PWM)               | WICED_PWM_3      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_13 | 185 | PWM_0        | C10 | PWM_0                   | PWM_0                    | N/C    |  PWM, GPIO   | uSD Connector/Slot Detect     |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_14 | 186 | PWM_1        | C9  | PWM_1                   | PWM_1                    | N/C    |  PWM, GPIO   | Auth Chip Reset               |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_15 | 190 | PWM_2        | A27 | PWM_2                   | PWM_2                    | N/C    |  PWM, GPIO   | Ethernet PHY Reset            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_16 | 189 | PWM_3        | A26 | PWM_3                   | PWM_3                    | J6:7   |  PWM, GPIO   | TP4/LED_1                     | WICED_LED1       |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_17 | 187 | PWM_4        | A31 | PWM_4                   | PWM_4                    | J6:1   |  PWM, GPIO   | ARD_GPIO9 (PWM)               | WICED_PWM_5      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_18 | 188 | PWM_5        | D7  | PWM_5                   | PWM_5                    | J6:2   |  PWM, GPIO   | USER_1 Button                 | WICED_BUTTON1    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_19 | 78  | SPI_0_MISO   | A12 | SPI_0_MISO              | SPI_0_MISO               | J6:17  |  SPI, GPIO   |                               | WICED_SPI_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_20 | 76  | SPI_0_CLK    | A11 | SPI_0_CLK               | SPI_0_CLK                | J6:12  |  SPI, GPIO   |                               | WICED_SPI_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_21 | 81  | SPI_0_MOSI   | A9  | SPI_0_MOSI              | SPI_0_MOSI               | J6:14  |  SPI, GPIO   |                               | WICED_SPI_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_22 | 82  | SPI_0_CS     | A10 | SPI_0_CS                | SPI_0_CS                 | J6:16  |  SPI, GPIO   |                               | WICED_SPI_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_23 | 139 | GPIO_2       | C13 | GPIO_2_JTAG_TCK         | GPIO_2_JTAG_TCK          | J3:9   |  GPIO        | DEBUGGER                      |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_24 | 144 | GPIO_3       | B20 | GPIO_3_JTAG_TMS         | GPIO_3_JTAG_TMS          | J3:7   |  GPIO        | DEBUGGER                      |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_25 | 136 | GPIO_4       | C3  | GPIO_4_JTAG_TDI         | GPIO_4_JTAG_TDI          | J3:3   |  GPIO        | DEBUGGER                      |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_26 | 132 | GPIO_5       | B4  | GPIO_5_JTAG_TDO         | GPIO_5_JTAG_TDO          | J3:5   |  GPIO        | DEBUGGER                      |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_27 | 133 | GPIO_6       | C2  | GPIO_6_JTAG_TRST_L      | GPIO_6_JTAG_TRST_L       | J3:1   |  GPIO        | DEBUGGER                      |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_28 | 153 | I2S_MCLK0    | B36 | I2S0_MCK                | I2S0_MCK                 | J6:3   |  I2S, GPIO   |                               | WICED_I2S_1/2    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_29 | 154 | I2S_SCLK0    | B35 | I2S0_SCK_BCLK           | I2S0_SCK_BCLK            | J6:5   |  I2S, GPIO   |                               | WICED_I2S_1/2    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_30 | 163 | I2S_LRCLK0   | A51 | I2S0_WS_LRCLK           | I2S0_WS_LRCLK            | J6:6   |  I2S, GPIO   |                               | WICED_I2S_1/2    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_31 | 164 | I2S_SDATAI0  | A50 | I2S0_SD_IN              | I2S0_SD_IN               | J10:8  |  I2S, GPIO   | ARD_GPIO7                     | WICED_I2S_1/2    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_32 | 161 | I2S_SDATAO0  | A48 | I2S0_SD_OUT             | I2S0_SD_OUT              | J6:4   |  I2S, GPIO   |                               | WICED_I2S_1/2    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_33 | 158 | I2S_MCLK1    | D5  | I2S1_MCK                | I2S1_MCK                 | J6:26  |  I2S, GPIO   |                               | WICED_I2S_3/4    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_34 | 160 | I2S_SCLK1    | B1  | I2S1_SCK_BCLK           | I2S1_SCK_BCLK            | J6:29  |  I2S, GPIO   |                               | WICED_I2S_3/4    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_35 | 162 | I2S_LRCLK1   | A49 | I2S1_WS_LRCLK           | I2S1_WS_LRCLK            | J6:27  |  I2S, GPIO   |                               | WICED_I2S_3/4    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_36 | 156 | I2S_SDATAI1  | B38 | I2S1_SD_IN              | I2S1_SD_IN               | J12:1  |  I2S, GPIO   | ARD_GPIO8                     | WICED_I2S_3/4    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_37 | 155 | I2S_SDATAO1  | B37 | I2S1_SD_OUT             | I2S1_SD_OUT              | J6:10  |  I2S, GPIO   |                               | WICED_I2S_3/4    |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_38 | 83  | SPI_1_CLK    | B32 | SPI_1_CLK               | SPI_1_CLK                | J6:9   |  SPI         |                               | WICED_SPI_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_39 | 84  | SPI_1_MISO   | B31 | SPI_1_MISO              | SPI_1_MISO               | J6:11  |  SPI         |                               | WICED_SPI_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_40 | 86  | SPI_1_MOSI   | B30 | SPI_1_MOSI              | SPI_1_MOSI               | J6:13  |  SPI         |                               | WICED_SPI_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_41 | 80  | SPI_1_CS     | B33 | SPI_1_CS                | SPI_1_CS                 | J6:15  |  SPI         |                               | WICED_SPI_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_42 | 152 | SDIO_CLK     | B25 | SDIO_CLK                | SDIO_CLK                 | J6:32  |  SDIO        | uSD Connector/Slot            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_43 | 151 | SDIO_CMD     | B24 | SDIO_CMD                | SDIO_CMD                 | J6:33  |  SDIO        | uSD Connector/Slot            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_44 | 150 | SDIO_DATA0   | B23 | SDIO_DATA0              | SDIO_DATA_0              | J6:31  |  SDIO        | uSD Connector/Slot            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_45 | 149 | SDIO_DATA1   | B22 | SDIO_DATA1              | SDIO_DATA_1              | J6:30  |  SDIO        | uSD Connector/Slot            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_46 | 148 | SDIO_DATA2   | B21 | SDIO_DATA2              | SDIO_DATA_2              | J6:35  |  SDIO        | uSD Connector/Slot            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_47 | 147 | SDIO_DATA3   | B27 | SDIO_DATA3              | SDIO_DATA_3              | J6:34  |  SDIO        | uSD Connector/Slot            |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_48 | 93  | I2C0_SDATA   | A46 | I2C_0_SDA               | I2C_0_SDA                | J6:25  |  I2C, GPIO   | External ADC / Auth chip      | WICED_I2C_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_49 | 92  | I2C0_CLK     | A47 | I2C_0_SCL               | I2C_0_SCL                | J6:23  |  I2C, GPIO   | External ADC / Auth chip      | WICED_I2C_1      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+------- +--------------+-------------------------------|------------------|
| WICED_GPIO_50 | 90  | I2C1_SDATA   | C1  | I2C_1_SDA               | I2C_1_SDA                | J12:9  |  I2C         |                               | WICED_I2C_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
| WICED_GPIO_51 | 89  | I2C1_CLK     | B2  | I2C_1_SCL               | I2C_1_SCL                | J12:10 |  I2C         |                               | WICED_I2C_2      |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
|               |     |              |     |                         | ARD_AD0                  | J13:1  |  ADC         | External ADC Channel 0        |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
|               |     |              |     |                         | ARD_AD1                  | J13:2  |  ADC         | External ADC Channel 1        |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
|               |     |              |     |                         | ARD_AD2                  | J13:3  |  ADC         | External ADC Channel 2        |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
|               |     |              |     |                         | ARD_AD3                  | J13:4  |  ADC         | External ADC Channel 3        |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
|               |     |              |     |                         | ARD_AD4_SDA              | J13:5  |  ADC         | External ADC Channel 4        |                  |
|---------------+-----+--------------+-----+-------------------------+--------------------------+--------+--------------+-------------------------------|------------------|
|               |     |              |     |                         | ARD_AD5_SCL              | J13:6  |  ADC         | External ADC Channel 5        |                  |
+-------------------------------------------------------------------------------------------------------------------------------------------------------+------------------+

+--------------------------------------------------------------------------------------------------------------------------------------------------------+
| Enum ID                 |Pin  |   Pin Name on |  Module Pin Name                         | Mod |  Board Net Name             | Header |  Peripheral    |
|                         | #   |      43907    |                                          | Pin#|                             | Conn   |  Available     |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_1  | 202 | RF_SW_CTRL_6  | RF_SW_CTRL_6_UART1_RX_IN                 | B18 |  RF_SW_CTRL_6_UART1_RXD     | J6:36  |  WICED_UART_1  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_2  | 201 | RF_SW_CTRL_7  | RF_SW_CTRL_7_RSRC_INIT_MODE_UART1_TX_OUT | B15 |  RF_SW_CTRL_7_UART1_TXD     | J6:37  |  WICED_UART_1  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_3  | 88  | UART0_RXD     | UART0_RXD_IN                             | A52 |  UART0_RXD_IN               | J6:18  |  WICED_UART_2  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_4  | 87  | UART0_TXD     | UART0_TXD_OUT                            | A55 |  UART0_TXD_OUT              | J6:20  |  WICED_UART_2  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_5  | 85  | UART0_CTS     | UART0_CTS_IN                             | A54 |  UART0_CTS_IN               | J6:22  |  WICED_UART_2  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_6  | 91  | UART0_RTS     | UART0_RTS_OUT                            | A53 |  UART0_RTS_OUT              | J6:24  |  WICED_UART_2  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_7  | 200 | RF_SW_CTRL_8  | RF_SW_CTRL_8_UART2_RXD                   | B14 |  RF_SW_CTRL_8_UART2_RXD     | J6:38  |  WICED_UART_3  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
| WICED_PERIPHERAL_PIN_8  | 196 | RF_SW_CTRL_9  | RF_SW_CTRL_9_HIB_LPO_SEL_UART2_TX        | C7  |  RF_SW_CTRL_9_UART2_TXD     | J6:39  |  WICED_UART_3  |
|-------------------------+-----+---------------+------------------------------------------+-----+-----------------------------+--------+----------------+
*/

typedef enum
{
    WICED_GPIO_1,
    WICED_GPIO_2,
    WICED_GPIO_3,
    WICED_GPIO_4,
    WICED_GPIO_5,
    WICED_GPIO_6,
    WICED_GPIO_7,
    WICED_GPIO_8,
    WICED_GPIO_9,
    WICED_GPIO_10,
    WICED_GPIO_11,
    WICED_GPIO_12,
    WICED_GPIO_13,
    WICED_GPIO_14,
    WICED_GPIO_15,
    WICED_GPIO_16,
    WICED_GPIO_17,
    WICED_GPIO_18,
    WICED_GPIO_19,
    WICED_GPIO_20,
    WICED_GPIO_21,
    WICED_GPIO_22,
    WICED_GPIO_23,
    WICED_GPIO_24,
    WICED_GPIO_25,
    WICED_GPIO_26,
    WICED_GPIO_27,
    WICED_GPIO_28,
    WICED_GPIO_29,
    WICED_GPIO_30,
    WICED_GPIO_31,
    WICED_GPIO_32,
    WICED_GPIO_33,
    WICED_GPIO_34,
    WICED_GPIO_35,
    WICED_GPIO_36,
    WICED_GPIO_37,
    WICED_GPIO_38,
    WICED_GPIO_39,
    WICED_GPIO_40,
    WICED_GPIO_41,
    WICED_GPIO_42,
    WICED_GPIO_43,
    WICED_GPIO_44,
    WICED_GPIO_45,
    WICED_GPIO_46,
    WICED_GPIO_47,
    WICED_GPIO_48,
    WICED_GPIO_49,
    WICED_GPIO_50,
    WICED_GPIO_51,

    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
} wiced_gpio_t;

typedef enum
{
    WICED_PERIPHERAL_PIN_1 = WICED_GPIO_MAX,
    WICED_PERIPHERAL_PIN_2,
    WICED_PERIPHERAL_PIN_3,
    WICED_PERIPHERAL_PIN_4,
    WICED_PERIPHERAL_PIN_5,
    WICED_PERIPHERAL_PIN_6,
    WICED_PERIPHERAL_PIN_7,
    WICED_PERIPHERAL_PIN_8,

    WICED_PERIPHERAL_PIN_MAX, /* Denotes the total number of GPIO and peripheral pins. Not a valid pin alias */
} wiced_peripheral_pin_t;

typedef enum
{
    WICED_SPI_1,
    WICED_SPI_2,
    WICED_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
} wiced_spi_t;

typedef enum
{
    WICED_I2C_1,
    WICED_I2C_2,
    WICED_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
} wiced_i2c_t;

typedef enum
{
    WICED_PWM_1,
    WICED_PWM_2,
    WICED_PWM_3,
    WICED_PWM_4,
    WICED_PWM_5,
    WICED_PWM_6,
    WICED_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
} wiced_pwm_t;

typedef enum
{
#ifdef USING_EXTERNAL_ADC
   WICED_ADC_1,
   WICED_ADC_2,
   WICED_ADC_3,
   WICED_ADC_4,
   WICED_ADC_5,
   WICED_ADC_6,
   WICED_ADC_MAX,
#else
   WICED_ADC_NONE = 0xff, /* No ADCs */
#endif /* ifdef USING_EXTERNAL_ADC */
} wiced_adc_t;

typedef enum
{
    WICED_UART_1,   /* ChipCommon Slow UART */
    WICED_UART_2,   /* ChipCommon Fast UART */
    WICED_UART_3,   /* GCI UART */
    WICED_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
} wiced_uart_t;

typedef enum
{
    WICED_I2S_1,
    WICED_I2S_2,
    WICED_I2S_3,
    WICED_I2S_4,
    WICED_I2S_5,
    WICED_I2S_6,
    WICED_I2S_MAX, /* Denotes the total number of I2S port aliases. Not a valid I2S alias */
} wiced_i2s_t;

/* Logical Button-ids which map to phyiscal buttons on the board */
typedef enum
{
    PLATFORM_BUTTON_1,
    PLATFORM_BUTTON_2,
    PLATFORM_BUTTON_MAX, /* Denotes the total number of Buttons on the board. Not a valid Button alias */
} platform_button_t;

/* Logical LED-ids which map to phyiscal gpio LEDs on the board (see platform_gpio_leds[] in platform.c)*/
typedef enum
{
    PLATFORM_LED_1,
    PLATFORM_LED_2,
    PLATFORM_LED_MAX, /* Denotes the total number of LEDs on the board. Not a valid alias */
} platform_led_t;

#define ARD_GPIO0  (WICED_GPIO_1)
#define ARD_GPIO1  (WICED_GPIO_2)
#define ARD_GPIO2  (WICED_GPIO_9)
#define ARD_GPIO3  (WICED_GPIO_3)
#define ARD_GPIO4  (WICED_GPIO_10)
#define ARD_GPIO5  (WICED_GPIO_12)
#define ARD_GPIO6  (WICED_GPIO_11)
#define ARD_GPIO7  (WICED_GPIO_31)
#define ARD_GPIO8  (WICED_GPIO_36)
#define ARD_GPIO9  (WICED_GPIO_17)

#define ARD_MISO   (WICED_GPIO_8)
#define ARD_SCK    (WICED_GPIO_5)
#define ARD_MOSI   (WICED_GPIO_6)
#define ARD_SS     (WICED_GPIO_7)



#define PLATFORM_LED_COUNT           ( 2 )

#define WICED_PLATFORM_BUTTON_COUNT  ( 2 )

#define WICED_LED1       ( WICED_GPIO_16 )
#define WICED_LED2       ( WICED_GPIO_5 )


/* for audio shields using the wiced2pi mezzanine we have to swap buttons */
#ifdef USE_RASPI_AUDIO
#define WICED_BUTTON1    ( WICED_GPIO_4 )
#define WICED_BUTTON2    ( WICED_GPIO_18 )
#else
#define WICED_BUTTON1    ( WICED_GPIO_18 )
#define WICED_BUTTON2    ( WICED_GPIO_4 )
#endif


#define WICED_GMAC_PHY_RESET (WICED_GPIO_15)

#define WICED_GPIO_AUTH_RST  (WICED_GPIO_14)

#define AUTH_IC_I2C_PORT     (WICED_I2C_1)

#define WICED_USB_HOST_OVERCURRENT      (WICED_GPIO_NONE) /* No GPIO available, must use controller */
#define WICED_USB_HOST_POWER_ENABLE     (WICED_GPIO_NONE) /* No GPIO available, must use controller */


/* audio default input/output definition */
#ifdef USE_CS47L24_AUDIO
/* DUET audio shield */
#define PLATFORM_DEFAULT_AUDIO_INPUT    AUDIO_DEVICE_ID_CS47L24_ADC_DIGITAL_MIC
#define PLATFORM_DEFAULT_AUDIO_OUTPUT   AUDIO_DEVICE_ID_CS47L24_DAC_LINE
#define PLATFORM_AUDIO_NUM_INPUTS       (4)
#define PLATFORM_AUDIO_NUM_OUTPUTS      (3)
#elif defined (USE_RASPI_AUDIO)
/* RASPIAUDIO dac+amp+mic audio shield */
#define PLATFORM_DEFAULT_AUDIO_INPUT    AUDIO_DEVICE_ID_I2S_1_ADC
#define PLATFORM_DEFAULT_AUDIO_OUTPUT   AUDIO_DEVICE_ID_I2S_1_DAC
#define PLATFORM_AUDIO_NUM_INPUTS       (3)
#define PLATFORM_AUDIO_NUM_OUTPUTS      (2)
#else /* default */
#define PLATFORM_DEFAULT_AUDIO_INPUT    AUDIO_DEVICE_ID_I2S_0_ADC
#define PLATFORM_DEFAULT_AUDIO_OUTPUT   AUDIO_DEVICE_ID_I2S_1_DAC
#define PLATFORM_AUDIO_NUM_INPUTS       (3)
#define PLATFORM_AUDIO_NUM_OUTPUTS      (2)
#endif /* input/output definition */


#define WICED_BUTTON_PAUSE_PLAY         ( WICED_BUTTON_2)
#define WICED_BUTTON_BACK               ( WICED_BUTTON_1)


/* Logical Button-ids which map to phyiscal buttons on the board (see platform_gpio_buttons[] in platform.c)*/
#define PLATFORM_BUTTON_BACK                    ( PLATFORM_BUTTON_1 )
#define PLATFORM_BUTTON_PAUSE_PLAY              ( PLATFORM_BUTTON_2 )

/* Bootloader OTA and OTA2 factory reset during settings */
#define PLATFORM_FACTORY_RESET_BUTTON_INDEX     ( PLATFORM_BUTTON_2 )
#define PLATFORM_FACTORY_RESET_TIMEOUT          ( 10000 )

/* Generic button checking defines */
#define PLATFORM_BUTTON_PRESS_CHECK_PERIOD      ( 100 )
#define PLATFORM_BUTTON_PRESSED_STATE           (   0 )

#define PLATFORM_GREEN_LED_INDEX                ( WICED_LED_INDEX_2 )
#define PLATFORM_RED_LED_INDEX                  ( WICED_LED_INDEX_1 )

#ifdef __cplusplus
} /*extern "C" */
#endif
