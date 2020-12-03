/*
 * $ Copyright Cypress Semiconductor $
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
 *
 * @ingroup     wiced_bt_cfg
 *
 * @{
 */
/****************************************************************************/

/**
 *
 * Initialize the Bluetooth controller and stack; register
 * callback for Bluetooth event notification.
 *
 * @param[in] p_bt_management_cback     : Callback for receiving Bluetooth management events
 * @param[in] p_bt_cfg_settings         : Bluetooth stack configuration #wiced_bt_cfg_settings_t
 *
 * @return   <b> WICED_BT_SUCCESS </b> : on success; \n
 *           <b> WICED_BT_FAILED  </b> : if an error occurred
 * @note This API must be called before using any BT functionality. \n
 * If p_bt_cfg_settings is null, stack uses default parameters defined in wiced_bt_cfg.h \n
 *     However, it is strongly recommended that applications define the configuration to appropriate values based on the application use case.
 */
wiced_result_t wiced_bt_stack_init(wiced_bt_management_cback_t *p_bt_management_cback,
                                    const wiced_bt_cfg_settings_t *p_bt_cfg_settings);

/**
 * This is a blocking call (returns after all de-initialisation procedures are complete)
 * It is recommended that the application disconnect any outstanding connections prior to invoking this function.
 *
 * @return  <b>  WICED_BT_SUCCESS </b> : on success; \n
 *          <b>  WICED_BT_ERROR   </b> : if an error occurred
 */
wiced_result_t wiced_bt_stack_deinit( void );


/**@} wicedbt_Framework */


#ifdef __cplusplus
}
#endif


