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

/*! \file mdev_pm.h
 *  \brief MCU power management module
 *
 * Allows application to put MCU in various low power modes.
 * MCU power modes in brief are :
 *
 * - PM0 (Active Mode): This is the full power state of MCU. Instruction
 *                      execution takes place only in PM0.
 *
 * - PM1 (Idle Mode)  : In this mode, the Cortex Mx core function clocks are
 *                      stopped until the occurrence of any interrupt.
 *                      This consumes lower power than PM0.
 *                      On exit from this mode, execution resumes from
 *                      the next instruction in SRAM.
 *
 * - PM2 (Standby Mode):In this mode, the Cortex Mx core, most of the
 *                      peripherals & SRAM arrays are in low-power mode.
 *                      The PMU and RTC are operational.
 *                      A wakeup can happen by timeout (RTC based)
 *                      or by asserting the WAKEUP 0/1 lines.
 *                      This consumes much lower power than PM1.
 *                      On exit from this mode, execution resumes from
 *                      the next instruction in SRAM.
 *
 * - PM3 (Sleep Mode) : This mode further aggressively conserves power.
 *                      Only 192 KB (160 KB in SRAM0  and 32 KB in SRAM1)
 *                      out of 512 KB of SRAM is alive. All peripherals
 *                      are turned off and register config is lost.
 *                      It is the application's responsibility to restore the
 *                      peripheral config after exit from PM3. This consumes
 *                      lower power than in PM2. A wakeup can happen by timeout
 *                      (RTC based)or by asserting the WAKEUP 0/1 lines.
 *                      On exit from this mode, execution resumes from
 *                      the next instruction in SRAM.
 *
 * - PM4(Shutoff Mode) : This simulates a shutdown condition.
 *                      A wakeup can happen by timeout (RTC based) or by
 *                      asserting the WAKEUP 0/1 lines.
 *                      This is the lowest power state of MCU.
 *                      On wakeup, execution begins from bootrom as
 *                      if a fresh bootup has occurred.
 *
 *  The power management module provides two APIs :
 *
 * - a power manager framework API, pm_mcu_cfg(), that allows applications to
 * define a power management policy. Once defined, the system will automatically
 * put the MCU into the configured power save modes on detection of idle
 * activity.
 *
 * - a raw API, pm_mcu_state(), which puts the MCU into the
 * configured power save states immediately.
 *
 * @section pm_table MCU Power Modes relationship Table.
 *
 * Cortex Mx state
 *
 | Run (C0)  | Idle (C1) | Standby (C2)  | Off (C3) |
 | ----:---- | --------- | ------------- | -------- |
 | HCLK On   | HCLK On   | HCLK Off      | Power removed |
 | FCLK On   | FCLK Off   | FCLK Off     | Power removed |
 *
 * SRAM Modes
 *
 | Run (M0)  | Standby (M2) | Off (M3)  |
 | ----:---- | --------- | ------------- |
 | On        | Low power    | Power off  |
 *
 * Power Modes with  Core , SRAM , Peripherals\n
 *

|             | PM0 | PM1 | PM2 | PM3 | PM4 |
 | ----:----- | --- | --- | --- | --- | --- |
 |  Cortex M3 | C0  | C1  | C2  | C3  |C3 |
 |   SRAM     | M0  | M0  | M2  | M2  |M3 |
 |   Peripherals | On  | On  | Retained  | Off  |Off |
 *
 *
 *
 * @cond uml_diag
 *
 * MCU state transiotions are as shown below:
 *
 * @startuml{states.jpg}
 *
 * [*] --> BootUp : On reset key press or reboot
 * PM0 : Active
 * PM1 : Idle
 * PM2 : Standby
 * PM3 : Sleep
 * PM4 : Shutoff
 * BootUp --> PM0
 * PM1 --> PM0 : Any interrupt
 * PM0 --> PM1 : Wait for interrupt (WFI)
 * PM2 --> PM0 : Wakeup 0/1 or timeout
 * PM0 --> PM2 : PMU_State PM2 + WFI
 * PM3 --> PM0 : Wakeup 0/1 or timeout
 * PM0 --> PM3 : PMU_State PM3 + WFI
 * PM4 --> BootUp : Wakeup 0/1 or timeout
 * PM0 --> PM4 : PMU_State PM4 + WFI
 *
 * @enduml
 * @endcond
 *
 */


#ifndef _MDEV_PM_H_
#define _MDEV_PM_H_

#include <lowlevel_drivers.h>

#define PM4_SIGN 0xA55AA55A

/**
 *  Indicates the wakeup source
 *  It can be any external GPIO pin or RTC.
 */
enum wakeup_cause {
	/** Wakeup source is EXT GPIO 0 */
	EGPIO0 = 1,
	/** Wakeup Source is EXT GPIO 1 */
	EGPIO1 = 1<<1,
	/** Wakeup source is RTC */
	ERTC   = 1<<2,
	/** ONLY for MW300 : Wakeup source is WLAN */
	EWLAN   = 1<<3
};

/** Enumeration for Voltage IO domains om MCU */
typedef enum {
	/** voltage domain 0 */
	IO_DOMAIN_0 = 1,
	/** voltage domain 1 */
	IO_DOMAIN_1 = 1<<1,
	/** voltage domain 2 */
	IO_DOMAIN_2 = 1<<2,
	/** voltage domain 3 only for MW300 */
	IO_DOMAIN_3 = 1<<3
} io_domain_t;


/** Power States of MCU */
typedef enum {

	/** (Active Mode): This is the full power state of MCU.
	 *  Instruction execution takes place only in PM0.
	*/
	PM0,
	/** (Idle Mode): In this mode Cortex M3 core function
	 *  clocks are stopped until the occurrence of any interrupt.
	 *  This consumes lower power than PM0. */
	PM1,

	/** (Standby Mode):In this mode, the Cortex M3,
	 *  most of the peripherals & SRAM arrays are in
	 *  low-power mode.The PMU and RTC are operational.
	 *  A wakeup can happen by timeout (RTC based) or by asserting the
	 *  WAKEUP 0/1 lines.This consumes much lower power than PM1.
	 */
	PM2,

	/**(Sleep Mode): This mode further aggressively conserves power.
	 * Only 192 KB (160 KB in SRAM0  and 32 KB in SRAM1)
	 * out of 512 KB of SRAM is alive. All peripherals
	 * are turned off and register config is lost.
	 * Application should restore the peripheral config
	 * after exit form PM3. This consumes lower power
	 * than in PM2. A wakeup can happen by timeout (RTC based)
	 * or by asserting the WAKEUP 0/1 lines.
	 */
	PM3,

	/** (Shutoff Mode): This simulates a shutdown condition.
	 * A wakeup can happen by timeout (RTC based) or by
	 * asserting the WAKEUP 0/1 lines.
	 * This is the lowest power state of MCU.
	 * On wakeup execution begins from bootrom as
	 * if a fresh bootup has occurred.
	 */
	PM4
} power_state_t;


/** Turn on individual IO domains
 *
 * MC200 MCU has 6 configurable IO Voltage domains.
 *  -# D0   : GPIO 0 to GPIO 17
 *  -# Aon  : GPIO 18 to GPIO 27 (Always On)
 *  -# D1   : GPIO 28 to GPIO 50
 *  -# SDIO : GPIO 51 to GPIO 56 (SDIO interface)
 *  -# D2   : GPIO 59 to GPIO 79
 *  -# FL   : Internal Flash


 * MW300 MCU has 5 configurable IO Voltage domains.
 *  -# D0   : GPIO 0 to GPIO 15
 *  -# D1   : GPIO 16 to GPIO 21
 *  -# Aon  : GPIO 22 to GPIO 26 (Always On)
 *  -# D2   : GPIO 28 to GPIO 33
 *  -# D3   : GPIO 27, GPIO 34 to GPIO 49
 *
 *  Each domain can be in 3 possible states
 *  - ON/Normal : Full power on.
 *  - Deep OFF : Low power state.
 *                Latency to go back to ON state is less.
 *  - OFF : Completely off.
 *          Latency to go back to ON state is higher than Deep Off.
 *
 *  When MCU enters @ref PM2, @ref PM3, @ref PM4 using
 *  power management APIs all IO voltage domains except
 *  Aon are turned off. Aon is kept in deep off. These APIs assist in
 *  controlling the IO domains in PM0.
 *
 *  @param [in] domain IO domain to be turned on
 *  @return none
 */
#define pmu_drv_poweron_vddio(domain)  PMU_PowerOnVDDIO(domain)

/** Turn off individual IO domains
 *  @param [in] domain IO domain to be turned off
 *  @return none
 *  @note For details of IO domains please see
 *        pmu_drv_poweron_vddio()
 */
#define pmu_drv_poweroff_vddio(domain)  PMU_PowerOffVDDIO(domain)

/** Deep off individual IO domains
 *  @param [in] domain IO domain to be set to deep off
 *  @return none
 *  @note For details of IO domains please see
 *        pmu_drv_poweron_vddio()
 */
#if defined(CONFIG_CPU_MC200)
#define pmu_drv_powerdeepoff_vddio(domain)  PMU_PowerDeepOffVDDIO(domain)
#else
#define pmu_drv_powerdeepoff_vddio(domain) PMU_PowerDownVDDIO(domain)

/** Set individual IO domains in Normal mode
 *  This API should be called to get IO domain out of Deep Off mode
 *  @param [in] domain IO domain to be set to normal mode
 *  @return none
 *  @note For details of IO domains please see
 *        pmu_drv_poweron_vddio()
 */

#define pmu_drv_powernormal_vddio(domain) PMU_PowerNormalVDDIO(domain)
#endif


/** Configure Power Mgr Framework's MCU Power Save
 *
 * This function is used to configure the power manager framework's MCU Power
 * Save. When enabled, the power manager framework will detect if the system is
 * idle and put the MCU processor in power save mode. Before entering power
 * save mode, the power manager framework will detect the next scheduled wakeup
 * for any process/timer and configure the sleep duration such that the
 * processor wakes up just in time to service that process. Once enabled the
 * power manager framework will continue to opportunistically put the system
 * into the configured low power mode.
 *
 * A threshold parameter is also supported. The power manager framework will
 * make the MCU enter power save mode, only if the next scheduled thread
 * wakeup is greater than the configured threshold.
 *
 * For example, say,
 * -# the threshold is configured to 10 milliseconds
 * -# the PM2 exit latency is 3 milliseconds
 * Now if the system is idle, (that is there is no running thread in the
 * system) at time t0, the power management framework will put the MCU into
 * power save mode, only if the next thread is scheduled to wakeup and execute
 * at t0 + 10 + 3 milliseconds. If a thread is scheduled to wakeup and execute
 * before this time, then the MCU doesn't enter power save at time t0.
 *
 * @param[in] enabled true to enable, false to disable power manager framework
 * @param[in] mode MCU power mode @ref power_state_t.
 * @param[in] threshold System will not go to power save state
 *                        if it is going to wake up in less than [threshold]
 *                        ticks.
 * @note This API should be used only for @ref PM2 and @ref PM3.
 *       The application should call this function only once
 *       to configure and enable power save scheme. Power manager
 *       framework takes care of opportunistically entering and
 *       exiting power save.
 * @note  Applications can register callbacks for low power entry and exit using
 *         pm_register_cb(). This will ensure that the callback gets called
 *        everytime before entering the power save mode or exiting from the
 *        power save mode. Applications can perform any state save-restore
 *        operations in this callback.
 */
void pm_mcu_cfg(bool enabled, power_state_t mode,
	 unsigned int threshold);


/** Enter low power state of MCU
 *
 * This function puts the core into the requested low power mode.
 * @param[in] state  the desired power mode of MCU  \ref power_state_t
 * @param[in] time_dur duration (in milliseconds) for which device
 *            should be in requested low power mode. The special value 0
 *            indicates maximum possible timeout that can be configured using
 *            RTC.
 *  @return   @ref wakeup_cause
 *
 *  @note The device wakes up automatically after the pre-configured
 *  time interval has expired or when the external gpio pin is asserted.
 *  For details please refer to the MCU data sheet.
 *  MCU by default keeps on toggling between PM0 and PM1.
 */
int pm_mcu_state(power_state_t state, uint32_t time_dur);



/** This function is used to get last source of wake up.
 *
 *  @return @ref wakeup_cause
 */
int pm_wakeup_source();

/** This function is used to check enable status of power manager framework.
 *
 *  @return  current power manager enable status,
 *  @return  true if  enabled
 *  @return  false if disabled
 */
bool pm_mcu_is_enabled();

/** Return target power save mode
 *
 * This function returns the configured power save mode that the MCU will
 * enter on detection of inactivity.
 *
 *  @return @ref power_state_t
 */

int pm_mcu_get_mode();

/** Manage IO Domains on/off in PM2
 *  This function allows application to determine which
 *  GPIO domain (D0, D1, D2, D3 (for MW300)) are to be kept
 *  on or off when MCU enters PM2
 *  @note This API should be used if  application
 *        wants to keep IO domains on in  PM2
 *        else by default they will be turned off while entering
 *        PM2 and turned on after exiting PM2.
 *  @param [in] domain_on Logical OR of IO domains to be
 *i		kept on in PM2.
 */
void pm_mcu_io_cfg(uint32_t domain_on);

/** Initialize Power Manager
 *
 * This is an MCU specific API to initialize Power manager module
 */
void arch_pm_init(void);

/** Reboot MCU
 *
 * This API can be used to reboot the MCU.
 */
void arch_reboot();

/** \cond mdev_pm_backward_compat
 * Macros for backward compatibility
 */

#define pm_mc200_cfg pm_mcu_cfg
#define pm_mc200_state pm_mcu_state
#define pm_mc200_io_cfg(x, y, z)	pm_mcu_io_cfg(\
		(x ? IO_DOMAIN_0 : 0) | \
		(y ? IO_DOMAIN_1 : 0) | \
		(z ? IO_DOMAIN_2 : 0))
/** \endcond */

#endif   /* _MDEV_PM_H_*/
