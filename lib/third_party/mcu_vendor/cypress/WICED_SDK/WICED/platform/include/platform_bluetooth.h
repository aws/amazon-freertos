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

#include "platform_peripheral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef UART_RX_FIFO_SIZE
#define UART_RX_FIFO_SIZE (3000)
#endif
/******************************************************
 *                    Constants
 ******************************************************/
/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_BT_PIN_POWER,
    WICED_BT_PIN_RESET,
    WICED_BT_PIN_HOST_WAKE,
    WICED_BT_PIN_DEVICE_WAKE,
    WICED_BT_PIN_MAX,
} wiced_bt_control_pin_t;

typedef enum
{
    WICED_BT_PIN_UART_TX,
    WICED_BT_PIN_UART_RX,
    WICED_BT_PIN_UART_CTS,
    WICED_BT_PIN_UART_RTS,
} wiced_bt_uart_pin_t;

typedef enum
{
    PATCHRAM_DOWNLOAD_MODE_NO_MINIDRV_CMD,
    PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
} wiced_bt_patchram_download_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    uint32_t                          patchram_download_baud_rate;
    wiced_bt_patchram_download_mode_t patchram_download_mode;
    uint32_t                          featured_baud_rate;
} platform_bluetooth_config_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

//TODO: put all these variables into a single structure.
/* Variables to be defined by the Bluetooth supporting platform */
extern const platform_gpio_t*        wiced_bt_control_pins[];
extern const platform_gpio_t*        wiced_bt_uart_pins[];
extern const platform_uart_t*        wiced_bt_uart_peripheral;
extern       platform_uart_driver_t* wiced_bt_uart_driver;
extern const platform_uart_config_t  wiced_bt_uart_config;
extern const platform_bluetooth_config_t wiced_bt_config;

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif
