/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef _WICED_TIMER_LEGACY_H_
#define _WICED_TIMER_LEGACY_H_

/* for backward compatability */
/**
 * Defines the wiced timer types. These timers are system tick driven and a
 * systick is 1 millisecond.So the minimum timer resolution supported is
 * 1 millisecond
 */
enum wiced_timer_type_e
{
    WICED_SECONDS_TIMER = 1,
    WICED_MILLI_SECONDS_TIMER, /* The minimum resolution supported is 1 ms */
    WICED_SECONDS_PERIODIC_TIMER,
    WICED_MILLI_SECONDS_PERIODIC_TIMER /*The minimum resolution supported is 1 ms */
};
/** Wiced Timer type  see @cond #wiced_timer_type_e @endcond */
typedef uint8_t wiced_timer_type_t;

/** Wiced Timer Parameter Type */
#define WICED_TIMER_PARAM_TYPE    void *

/** Wiced timer legacy callback 
* @param[in]    cb_params      :Timer callback function
*/
typedef void(*wiced_timer_callback_legacy_t)(WICED_TIMER_PARAM_TYPE cb_params);
/** Wiced timer callbacl Funtciont pointer */
#define wiced_timer_callback_fp wiced_timer_callback_legacy_t // TODO: remove. Only for backward compatability

/** Convert from ms to us*/
#define QUICK_TIMER_MS_TO_US(tout)    tout*1000

/** Wiced Init Timer */
#define wiced_init_timer        wiced_init_timer_legacy
/** Wiced Start Timer */
#define wiced_start_timer       wiced_start_timer_legacy
/** Wiced Stop Timer*/
#define wiced_stop_timer        wiced_stop_timer_legacy
/** Wiced is timer in use*/
#define wiced_is_timer_in_use   wiced_is_timer_in_use_legacy
/** Wiced deinit Timer*/
#define wiced_deinit_timer      wiced_deinit_timer_legacy

/** Legacy Timer structure*/
typedef struct
{
    wiced_timer_ext_t   timer_obj;   /**< Timer Object */
    wiced_timer_type_t  timer_type;  /**< Timer Type */
}wiced_timer_legacy_t;
/** Wiced Timer*/
#define wiced_timer_t   wiced_timer_legacy_t

#ifdef __cplusplus
extern "C"
{
#endif

/**  Initializes the timer
 *
 *@param[in]    p_timer             :Pointer to the timer structure
 *@param[in]    TimerCb             :Timer callback function to be invoked on timer expiry
 *@param[in]    cBackparam          :Parameter to be passed to the timer callback function which
 *                                              gets invoked on timer expiry,if any
 *@param[in]    type                :Type of the timer
 *
 * @return   wiced_result_t
 */
wiced_result_t wiced_init_timer_legacy( wiced_timer_t* p_timer, wiced_timer_callback_legacy_t TimerCb,
                                 WICED_TIMER_PARAM_TYPE cBackparam, wiced_timer_type_t type);

/**  Starts the timer
 * Timer should be initialized before starting the timer. Running the timer interfere with the
 * low power modes of the chip. Time to remain in the low power mode is dependent on the
 * timeout values of the running timers, ie time to sleep is dependent on the time after which
 * the next timer among the active timers expires.
 *
 * @param[in]    p_timer           :Pointer to the timer structure
 * @param[in]    timeout           :timeout in milliseconds
 *
 * @return       wiced_result_t
 */
wiced_result_t wiced_start_timer_legacy(wiced_timer_t* p_timer, uint32_t timeout);

/** Stops the timer
 *
 * @param[in]    p_timer      :Pointer to the timer structure
 *
 * @return       wiced_result_t
 */

wiced_result_t wiced_stop_timer_legacy( wiced_timer_t* p_timer );

/**  Checks if the timer is in use
*
*@param[in]    p_timer                  :Pointer to the timer structure
*
* @return   TRUE if the timer is in use and FALSE if the timer is not in use
*/
wiced_bool_t wiced_is_timer_in_use_legacy( wiced_timer_t* p_timer );

/** Deinitialize the timer instance and stops the timer if it is running
 *
 * @param[in]    p_timer                :Pointer to the timer
 *
 * @return       wiced_result_t
 */
wiced_result_t wiced_deinit_timer_legacy( wiced_timer_t* p_timer );

#ifdef __cplusplus
}
#endif

#endif // _WICED_TIMER_LEGACY_H_

