/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Bluetooth Management (BTM) Application Programming Interface
 *
 * The BTM consists of several management entities:
 *      1. Device Control - controls the local device
 *      2. Device Discovery - manages inquiries, discover database
 *      3. ACL Channels - manages ACL connections (BR/EDR and LE)
 *      4. SCO Channels - manages SCO connections
 *      5. Security - manages all security functionality
 *      6. Power Management - manages park, sniff, hold, etc.
 *
 * @defgroup wicedbt      Bluetooth
 *
 * WICED Bluetooth Framework Functions
 */

#pragma once

#include "wiced_bt_cfg.h"

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/**
 * Framework Management Functions
 *
 * @addtogroup  wicedbt_Framework   Framework
 * @ingroup     wicedbt
 *
 *
 * Bluetooth Management (BTM) Application Programming Interface
 *
 * The BTM consists of several management entities:
 *      1. Device Control - controls the local device
 *      2. Device Discovery - manages inquiries, discover database
 *      3. ACL Channels - manages ACL connections (BR/EDR and LE)
 *      4. SCO Channels - manages SCO connections
 *      5. Security - manages all security functionality
 *      6. Power Management - manages park, sniff, hold, etc.
 *
 * @{
 */
/****************************************************************************/

/**
 * Function         wiced_bt_stack_init
 *
 *                  Initialize the Bluetooth controller and stack; register
 *                  callback for Bluetooth event notification.
 *
 * @param[in] p_bt_management_cback     : Callback for receiving Bluetooth management events
 * @param[in] p_bt_cfg_settings         : Bluetooth stack configuration
 * @param[in] wiced_bt_cfg_buf_pools    : Buffer pool configuration
 *
 * @return    WICED_BT_SUCCESS : on success;
 *            WICED_BT_ERROR   : if an error occurred
 */
wiced_result_t wiced_bt_stack_init(wiced_bt_management_cback_t *p_bt_management_cback,
                                   const wiced_bt_cfg_settings_t     *p_bt_cfg_settings,
                                   const wiced_bt_cfg_buf_pool_t     wiced_bt_cfg_buf_pools[WICED_BT_CFG_NUM_BUF_POOLS]);

/**
 * Function         wiced_bt_stack_deinit
 *
 *                  This function blocks until all de-initialisation procedures are complete.
 *                  It is recommended that the application disconnect any outstanding connections prior to invoking this function.
 *
 * @return    WICED_BT_SUCCESS : on success;
 *            WICED_BT_ERROR   : if an error occurred
 */
wiced_result_t wiced_bt_stack_deinit( void );

/**@} wicedbt_Framework */


#ifdef __cplusplus
}
#endif


