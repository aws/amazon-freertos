/*******************************************************************************
* \file cybt_platform_config.h
*
* \brief
* Provides interface to configurate platform settings, including HCI, sleep mode
* and OS task memory pool.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CYBT_PLATFORM_CONFIG_H
#define CYBT_PLATFORM_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "cyhal_gpio.h"
#include "cyhal_uart.h"

/**
 *  @addtogroup    platform_cfg   Bluetooth Platform Configuration
 *
 * The Bluetooth platform-specific configurations, 
 * including hardware pin assignment, HCI (Host Controller Interface) format,
 * sleep mode settings and memory pool size for OS tasks.
 *
 *  @{
 */

/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define CYBT_SLEEP_MODE_DISABLED      (0)
#define CYBT_SLEEP_MODE_ENABLED       (1)

#define CYBT_WAKE_ACTIVE_LOW          (0)
#define CYBT_WAKE_ACTIVE_HIGH         (1)


/*****************************************************************************
 *                           Type Definitions
 *****************************************************************************/
/**
 *  The BT HCI transport type
 */
typedef enum
{
    CYBT_HCI_UNKNOWN = 0x00,
    CYBT_HCI_UART    = 0x01
} cybt_hci_transport_t;

/**
 *  The HCI UART configuration, including:
 *  * hardware pin assignment
 *  * baud rate
 *  * data format
 *  * flow control support
 */
typedef struct
{
    cyhal_gpio_t         uart_tx_pin;  /**< Uart TXD pin */
    cyhal_gpio_t         uart_rx_pin;  /**< Uart RXD pin */
    cyhal_gpio_t         uart_rts_pin;  /**< Uart RTS pin */
    cyhal_gpio_t         uart_cts_pin;  /**< Uart CTS pin */

    uint32_t             baud_rate_for_fw_download; /**< Uart baud rate for firmware downloading */
    uint32_t             baud_rate_for_feature; /**< Uart baud rate for feature */

    uint32_t             data_bits;  /**< the size of data bits */
    uint32_t             stop_bits;  /**< the size of stop bits */
    cyhal_uart_parity_t  parity;     /**< parity check control */
    bool                 flow_control;  /**< flow control status */
} cybt_hci_uart_config_t;

/**
 *  The configuration of BT HCI transport, to specify which interface
 *  is used and its format.
 *
 *  Currently only UART is supported.
 */
typedef struct
{
    cybt_hci_transport_t  hci_transport;  /**< HCI transport selection */

    union
    {
        cybt_hci_uart_config_t  hci_uart; /**< HCI UART configuration */
    } hci;
} cybt_hci_transport_config_t;

/** 
  * Cypress Bluetooth chip sleep mode configuration parameters,
  * including enable status, wakeup pins assignment and 
  * their trigger polarity.
  */
typedef struct
{
    uint8_t         sleep_mode_enabled;    /**< Enable or disable the sleep mode of BT chip.
                                           *  Either assign the value
                                           *      CYBT_SLEEP_MODE_DISABLED, or
                                           *      CYBT_SLEEP_MODE_ENABLED
                                           *
                                           *  or CYCFG_BT_LP_ENABLED for ModusToolBox
                                           *  LPA configuration
                                           *
                                           *  Notice that sleep mode will be enabled 
                                           *  only if both device and host wakeup pins
                                           *  are assigned. 
                                           */
    cyhal_gpio_t    device_wakeup_pin;     /**< The gpio definition for BT device wakeup pin.
                                           *  It can be assigned by the value with the type
                                           *  cyhal_gpio_t directly. For ModusToolBox, 
                                           *  CYCFG_BT_DEV_WAKE_GPIO can be used.
                                           *
                                           *  NC is used as this pin is NOT connected.
                                           */
    cyhal_gpio_t    host_wakeup_pin;       /**< The gpio definition for BT host wakeup pin.
                                           *  It can be assigned by the value with the type
                                           *  cyhal_gpio_t directly. For ModusToolBox, 
                                           *  CYCFG_BT_HOST_WAKE_GPIO can be used.
                                           *
                                           *  NC is used as this pin is NOT connected.
                                           */
    uint8_t         device_wake_polarity;  /**< The trigger level of device wakeup pin.
                                            * Either assign the value
                                            *      CYBT_WAKE_ACTIVE_LOW, or<BR>
                                            *      CYBT_WAKE_ACTIVE_HIGH.
                                            *
                                            *  Or CYCFG_BT_DEV_WAKE_POLARITY 
                                            *  for ModusToolBox LPA configuration.
                                            *
                                            *  The default value will be active low.
                                            */
    uint8_t         host_wake_polarity;    /**< The trigger level of host wakeup pin.
                                            * Either assign the value
                                            *      CYBT_WAKE_ACTIVE_LOW, or <BR>
                                            *      CYBT_WAKE_ACTIVE_HIGH,
                                            *
                                            *  Or CYCFG_BT_DEV_WAKE_POLARITY 
                                            *  for ModusToolBox LPA configuration.
                                            *
                                            *  The default value will be active low.
                                            */
} cybt_controller_sleep_config_t;

/**
 *  The BT chip control configuration
 */
typedef struct
{
    cyhal_gpio_t                    bt_power_pin;    /**< BT controller power pin */
    cybt_controller_sleep_config_t  sleep_mode;      /**< sleep mode setting */
} cybt_controller_config_t;

/**
 *  The overall configuration for Cypress WICED BT/BLE stack and BT chip
 */
typedef struct
{
    cybt_hci_transport_config_t    hci_config;         /**< Bluetooth HCI transport configuration */
    cybt_controller_config_t       controller_config;  /**< Cypress Bluetooth chip configuration*/
    uint32_t                       task_mem_pool_size; /**< memory pool size for Bluetotoh task communication.
                                                        * The default size is 2048 bytes if it wasn't specified.
                                                        */

} cybt_platform_config_t;


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 *                           Function Declarations
 ****************************************************************************/

/**
 * Configurate the Bluetotoh platform specific settings.
 *
 * @param[in]       p_bt_platform_cfg : point to the configuration structure
 *
 * @returns         void
 */
void cybt_platform_config_init(const cybt_platform_config_t *p_bt_platform_cfg);


#ifdef __cplusplus
} /* extern "C" */
#endif

/**@} */

#endif

