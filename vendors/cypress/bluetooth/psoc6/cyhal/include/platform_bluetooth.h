/*
 * $ Copyright Cypress Semiconductor $
 */
#pragma once

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

#ifdef __cplusplus
} /* extern "C" */
#endif
