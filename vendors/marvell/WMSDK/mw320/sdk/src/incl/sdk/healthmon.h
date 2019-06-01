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

/** @file healthmon.h
*
*  @brief Health Monitor
*
*  Health monitor is a system process that monitors the health of the device
*  using. It queries the system services and applications periodically for their
*  health status. It uses the hardware watchdog (or approximate software
*  simulation, if hardware watchdog is not available) to recover if it detects
*  any problems.
*
*  \section healthmon_usage Usage
*
*  If application wishes to install its own periodic health check function, it
*  needs to instantiate \ref healthmon_handler structure that contains a valid
*  is_sick function. This function will be called by healthmon thread
*  periodically as specified by the handler. The function must return 0 in case
*  of healthy state and non-zero value in case of failure. The healthmon thread
*  obeys consecutive failures registered in handler before declaring system as
*  unhealthy. The \a about_to_die function is optional and gets called when
*  system is found unhealthy and is about to reboot.  In case of catastrophic
*  events, even though healthmon thread is unable to run (e.g. hardfault, frozen
*  system) the system reboots because the hardware watchdog is not strobed.
*
*/

#ifndef _HEALTHMON_H_
#define _HEALTHMON_H_

#include <wmtypes.h>

/** Maximum name size for module or application name */
#define HM_NAME_MAX	15
/** Maximum number of healthmon handlers that can be registered */
#define MAX_HM_HANDLERS	10

/** Default watchdog strobe interval. Please modify this if the hardware strobe
  requirement is different */
#define WD_STROBE_INTERVAL	20	/* 20 seconds */
/** Wakeup interval for healthmon thread. */
#define WKUP_INTERVAL		5	/* 5 seconds */

/** Healthmon handler that is used for registration with healthmon thread. This
 * structure instance's memory can be reclaimed immediately after successful
 * registration. */
struct healthmon_handler {
	/** Name of the module. This is used as identifier for handler */
	char name[HM_NAME_MAX + 1];
	/** Function pointer to be registered for periodic call back to check
	 * module health
	 *  \param cur_msec Milli-seconds since device bootup
	 *  \return Should return FALSE if module is healthy
	 *          Should return TRUE if module is not healthy
	 */
	bool (*is_sick) (unsigned int cur_msec);
	/** Function pointer to be registered that will be invoked before
	 *  system dies as any of the registered module is not healthy.
	 *  \param is_failed_module Flag to indicate if this module is found
	 *         unhealthy
	 */
	void (*about_to_die) (bool is_failed_module);
	/** Check interval for module represented in seconds
	 * \note check interval should be a multiple of WKUP_INTERVAL */
	unsigned int check_interval;
	/** Number of consecutive unhealthy probes after which system should
	 * die */
	unsigned char consecutive_failures;
};

/** Initialize healthmon.
 *
 * \return WM_SUCCESS if call succeeds
 * \return -WM_FAIL in case of failure
 */
int healthmon_init();

/** Starts the healthmon thread
 *
 * \return WM_SUCCESS if thread is successfully started
 * \return -WM_FAIL if thread is already running or healthmon is uninitialized
*/
int healthmon_start();

/** Register healthmon handler
 *
 * \param handler Pointer to properly initialized healthmon_handler structure
 * \return WM_SUCCESS Successful registration
 * \return -WM_FAIL if handler already present with same name or invalid
 *                  configuration
 */
int healthmon_register_handler(struct healthmon_handler *handler);

/** Unregister healthmon handler
 * \param name Pointer to the name of the module
 * \return WM_SUCCESS if handler unregistered
 * \return -WM_FAIL if no such handler found
 */
int healthmon_unregister_handler(const char *name);

/** Changes check interval for specified module
 *
 * \note check interval should be a multiple of \ref WKUP_INTERVAL
 *
 * \param name Name of the module for which check interval is to be changed
 * \param interval New check interval in seconds
 * \return WM_SUCCESS if check interval changed successfully
 * \return -WM_FAIL if invalid interval specified or no such module
 */
int healthmon_change_check_interval(const char *name, unsigned int interval);

/** Final about to die handler
 *
 * Set the final action handler to be called by the healthmon before
 * device resets due to the watchdog.  Every module registered to the
 * healthmon has its own about_to_die_handler which modules use to
 * perform their own actions required before the system dies. The
 * final_about_to_die_handler is optional and is executed after all
 * the modules' about_to_die handlers get called. Typically this can
 * be used to log diagnostics information captured by all the other
 * about_to_die handlers, or any other application specific
 * functions.
 *
 * \note It is not guaranteed that the final about the die handler will get
 * called before all watchdog resets. This function is called from the healthmon
 * thread's context, and in some system freeze cases, the healthmon may not get
 * a chance to execute before the hardware watchdog kicks in.
 *
 * \param fun Pointer to a function that needs to be executed
 * \return WM_SUCCESS if handler is registered successfully
 * \return -WM_E_PERM if this is called before healthmon_init
 */
int healthmon_set_final_about_to_die_handler(void (*fun)(void *data));

/** Register healthmon cli
 * \return WM_SUCCESS if cli registered successfully
 * \return -WM_FAIL in case of error
 */
int healthmon_register_cli(void);
#endif				/* _HEALTHMON_H_ */
