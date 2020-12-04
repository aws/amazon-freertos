/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 *  \addtogroup timer Timer Management Services
 *  \ingroup HardwareDrivers
 *
 *  @{
 *
 * Defines the interfaces for Timer Management Services
 */

#ifndef _WICED_TIMER_H_
#define _WICED_TIMER_H_

#include "wiced_result.h"

/** Timer callback argument type */
typedef void* wiced_timer_callback_arg_t;

/**
 * Function prototype for the timer call back.
 * @param[in]    cb_params      :Timer callback function
 */
typedef void ( wiced_timer_callback_ext_t )( wiced_timer_callback_arg_t cb_params );

/** Timer structure */
typedef struct _wiced_timer_ext_t
{
    struct _wiced_timer_ext_t   *p_next;    /**< pointer to next timer entry */

    wiced_timer_callback_arg_t  cb_arg;     /**< argument to timer callback */
    wiced_timer_callback_ext_t  *p_cback;   /**< timer callback function address */

    uint32_t                    periodic_interval_ms;   /**< periodic interval in milliseconds */

    uint64_t                    target_time;            /**< absolute target time */
}wiced_timer_ext_t;

#include "wiced_timer_legacy.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**  Initializes the timer
 *
 *@param[in]    p_timer         :Pointer to the timer structure
 *@param[in]    p_timer_cb      :Timer callback function to be invoked on timer expiry
 *@param[in]    cb_arg          :Parameter to be passed to the timer callback function which
 *                                              gets invoked on timer expiry,if any
 *@param[in]    periodic_timer  :WICED_TRUE, if periodic, WICED_FALSE if non-periodic
 *
 * @return   wiced_result_t
 */
wiced_result_t wiced_init_timer_ext( wiced_timer_ext_t* p_timer, wiced_timer_callback_ext_t *p_timer_cb,
                                 wiced_timer_callback_arg_t cb_arg, wiced_bool_t periodic_timer);

/** Starts the timer
 *
 * @param[in]    p_timer                :Pointer to the timer structure
 * @param[in]    timeout_ms             :timeout in milliseconds
 *
 * @return       wiced_result_t
 */
wiced_result_t wiced_start_timer_ext( wiced_timer_ext_t* p_timer,uint32_t timeout_ms );

/** Stops the timer
 *
 * @param[in]    p_timer      :Pointer to the timer structure
 *
 * @return       wiced_result_t
 */
wiced_result_t wiced_stop_timer_ext( wiced_timer_ext_t* p_timer );

/**  Checks if the timer is in use
*
*@param[in]    p_timer                  :Pointer to the timer structure
*
* @return   TRUE if the timer is in use and FALSE if the timer is not in use
*/
wiced_bool_t wiced_is_timer_in_use_ext(wiced_timer_ext_t* p_timer);

/** Deinitialize the timer instance and stops the timer if it is running
 *
 * @param[in]    p_timer                :Pointer to the timer
 *
 * @return       wiced_result_t
 */
wiced_result_t wiced_deinit_timer_ext(  wiced_timer_ext_t* p_timer );

/** @} */
#ifdef __cplusplus
}
#endif

#endif // _WICED_TIMER_H_

