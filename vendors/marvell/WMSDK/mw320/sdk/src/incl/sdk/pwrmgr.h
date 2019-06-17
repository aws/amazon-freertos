/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/** @file pwrmgr.h
*
*  @brief Power Manager Framework
*
*  The SDK provides a power manager framework to enable applications to
*  enable various power saving mechanisms. This framework depends on the
*  processor-specific APIs as defined in  @link mdev_pm.h MCU Power
*  Management @endlink.
*
*  The power manager provides a functionality to register callbacks on entry or
*  exit from  power modes @ref power_state_t. Applications or drivers can
*  register a power save callback using the function pm_register_cb()
*  or deregister using pm_deregister_cb().
*
*  @section pm_usage Usage
*   Power management can be used by applications as follows:
*  -# Call @ref pm_init(). This initializes the power management module. The
*   most basic power-management scheme is enabled at this time, which puts the
*   MCU in PM1 mode on idle.
*  -# Register callback for low power entry and exit using @ref
*   pm_register_cb. This will ensure that the callback gets called everytime
*   before entering the power save mode or exiting from the power save
*   mode. Applications can perform any state save-restore operations in this
*   callback.
*  -# Set power save configuration @ref pm_mcu_cfg(). This call will ensure
*   that the system continues to enter the configured power save mode as and
*   when it detects inactivity. The power manager framework makes sure that the
*   system comes up just in time to service any software timers, or threads
*   waiting on timed sleep.
*  -# If an application/driver expects to receive events from an external source
*   (apart from RTC or wakeup0/1 lines), then it may have to ensure that the
*   system does not enter power save modes in this duration. This control can be
*   exerted on the power manager framework by using wakelocks. Acquiring a
*   wakelock using wakelock_get() prevents the power manager framework from
*   putting the system into low power modes. The wakelock can be released using
*   wakelock_put().
*/

#ifndef _PWRMGR_H_
#define _PWRMGR_H_

#include <wmtypes.h>
#include <wm_os.h>
#include <mdev_pm.h>
#include <wmlog.h>

#define pm_e(...)				\
	wmlog_e("pm", ##__VA_ARGS__)

#ifdef CONFIG_PWR_DEBUG
#define pm_d(...)				\
	wmlog("pm", ##__VA_ARGS__)
#else
#define pm_d(...)
#endif

/** Power save event enumerations */
typedef enum {
	/** On entering PM2  mode */
	ACTION_ENTER_PM2 = 1,
	/** On exiting PM2 mode */
	ACTION_EXIT_PM2 = 2,
	/** On entering PM3  mode */
	ACTION_ENTER_PM3 = 4,
	/** On exiting  PM3 mode */
	ACTION_EXIT_PM3 = 8,
	/** On entering PM4 mode */
	ACTION_ENTER_PM4 = 16
} power_save_event_t;

/** Function pointer for callback function called on low power entry and exit */
typedef void (*power_save_state_change_cb_fn) (power_save_event_t event,
					       void *data);

/** Register Power Save Callback
 *
 *   This function registers a callback handler with the power manager. Based on
 *   the 'action' specified, this callback will be invoked on the corresponding
 *   power-save enter or exit events of the processor.
 *
 * @note The callbacks are called with interrupts disabled.
 *
 *  @param[in] action Logical OR of set of power-save events
 *             @ref power_save_event_t for which callback should be invoked.
 *  @param[in] cb_fn pointer to the callback function
 *  @param[in] data Pointer which can be a pointer to any
 *                         object or can be NULL.
 *  @return handle to be used in other APIs on success,
 *  @return -WM_E_INVAL  if incorrect parameter
 *  @return -WM_E_NOMEM if no space left.
 */
int pm_register_cb(unsigned char action, power_save_state_change_cb_fn cb_fn,
		   void *data);

/** De-register Power Save Callback
 *
 *  This function de-registers the power save callback handlers from the power
 *  manager.
 *
 *  @param[in] handle handle returned in pm_register_cb()
 *  @return WM_SUCCESS on success
 *  @return -WM_FAIL otherwise.
 */

int pm_deregister_cb(int handle);

/** Modify Power Save Callback
 *
 *  Change the power-save events for which the callback will be invoked.
 *  This function allows the application to change the events without
 *  changing  callback function.

 *  @param[in] handle handle returned in pm_register_cb()
 *  @param[in] action logical OR of new set of power-save
 *             events @ref power_save_event_t
 *  @return WM_SUCCESS on success
 *  @return -WM_FAIL otherwise.
 */
int pm_change_event(int handle, unsigned char action);

/*
 * This function is called from platform specific power manager code.
 *
 * It should be called with interrupts disabled.
 *
 * @param[in] action Power save event for which
 *                        callbacks will be invoked
 *                        @ref power_save_event_t
 */
void pm_invoke_ps_callback(power_save_event_t action);

/** Initialize the Power Manager module.
 *
 * This functions initializes the Power Manager module and
 * registers an idle hook with OS which will allow processor to
 * toggle between Active (PM0) and Idle (PM1) mode.
 * Please see @ref power_state_t for details of power modes.
 * @return WM_SUCCESS on success
 * @return -WM_FAIL otherwise.
 */
int pm_init();

/** Check init status of the Power Manager module
 * This functions check the init status of Power Manager module.
 *
 * @return true if Power Manager module init is done.
 * @return false if not.
 */
bool is_pm_init_done();

/** Do a reboot of the chip
 *
 * This function forces a reboot of the chip, as if it were power-cycled.
 *
 * @note This function will not return as it will reboot the hardware.
 */
void pm_reboot_soc();


/*
wakeup *  Wake lock is used to control entry of core in low power mode
 */
/** Acquire WakeLock
 *
 * When the power manager framework is enabled in an application using a call to
 * pm_mc200_cfg(), the system opportunistically enters power save modes on
 * idle-time detection. Wakelocks are used to prevent the system from entering
 * such power save modes for certain sections of the code. The system will not
 * automatically enter the MCU power save states if wakelock is being
 * held.
 *
 * Typically wakelocks have to be held only if any process/thread is waiting for
 * an external event or completion of asynchronous data transmission.
 * Systems continue to work properly for all other cases without acquiring
 * the wakelocks.
 *
 * @param[in] id_str This is a logical wakelock ID string used for debugging.
 * Logically correlated get/put should use the same string. This string is used
 * internally to track the individual logical wakelock counts.
 * @note This function should NOT be called in an interrupt context.
 * @return WM_SUCCESS on success
 * @return -WM_FAIL otherwise.
 */
int wakelock_get(const char *id_str);

/** Release WakeLock
 *
 * Use this function to release a wakelock previously acquired using
 * wakelock_get().The system enters MCU power save states, only when all the
 * acquired wakelocks have been released.
 *
 * @note The number of wakelock_get() and wakelock_put() requests must exactly
 * match to ensure that the system can enter power save states.
 *
 * @param[in] id_str Logical ID string used for wakelock debugging. Please
 *               read the documentation for @ref wakelock_get.
 * @note This function should NOT be called in an interrupt context.
 * @return WM_SUCCESS  on success
 * @return -WM_FAIL otherwise.
 */
int wakelock_put(const char *id_str);

/** Check if WakeLock is held
 *
 * @return 0 if no one is holding a wake lock.
 * @return Non-zero otherwise.
 */
int wakelock_isheld();


/** Register Power-Mode Command Line Interface commands
 * @return  0  on success
 * @return  1  on failure
 * @note  This function is available to the application and can
 *        be directly called if the pm module is initialised.
 */
int pm_cli_init(void);


/** Register MCU Power Manager CLI
 *  @return  WM_SUCCESS on success
 *  @return  -WM_FAIL on failure
 *  @note  This function can be called after the power manager is initialized
*/
int pm_mcu_cli_init();


#endif /* _PWRMGR_H_*/
