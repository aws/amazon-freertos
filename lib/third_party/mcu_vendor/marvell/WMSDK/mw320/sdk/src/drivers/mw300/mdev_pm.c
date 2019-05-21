/** @file mdev_pm.c
 *
 *  @brief This file provides  mdev driver for pm
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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


#include <stdlib.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <pwrmgr.h>
#include <lowlevel_drivers.h>
#include <board.h>
#include <wmtime.h>
#include <rtc.h>
#include <mdev.h>
#include <mdev_rtc.h>
#include <boot_flags.h>
#include <compat_attribute.h>

/*-------------------Macros--------------------*/
#define RTC_COUNT_ONE_MSEC 32

/* all 3 wakeup sources RTC , GPIO0 and GPIO1 */
#define WAKEUP_SRC_MASK  0x7
#define MASK_PAD_EN (1 << 31)
/*---------------------------------------------*/
/*------------------Data types-----------------*/
typedef struct {
	bool enabled;
	power_state_t mode;
	unsigned int threshold;
	unsigned int latency;
	uint32_t io_domain_on;
} pm_state_t;

typedef struct {
	volatile uint32_t ISER[2];
	volatile uint8_t  IP[64];
	volatile uint32_t ICSR;
	volatile uint32_t VTOR;
	volatile uint32_t AIRCR;
	volatile uint32_t SCR;
	volatile uint32_t CCR;
	volatile uint8_t  SHP[12];
	volatile uint32_t SHCSR;
	volatile uint32_t CFSR;
	volatile uint32_t HFSR;
	volatile uint32_t DFSR;
	volatile uint32_t MMFAR;
	volatile uint32_t BFAR;
	volatile uint32_t AFSR;
	volatile uint32_t CPACR;
	/* Sys tick registers  */
	volatile uint32_t SYSTICK_CTRL;
	volatile uint32_t SYSTICK_LOAD;

	/* Flash controller registers */
	volatile uint32_t FCCR;
	volatile uint32_t FCTR;
	volatile uint32_t FCSR;
	volatile uint32_t FCACR;
	volatile uint32_t FCHCR;
	volatile uint32_t FCMCR;
	volatile uint32_t FAOFFR;

	/* QSPI Configuration register */
	volatile uint32_t CLK_PRESCALE;
	volatile uint32_t CLK_CAPT_EDGE;

}  pm3_saved_sys_regs;
/*---------------------------------------------*/
/*------------------Static Variables-----------*/
static pm_state_t pm_state;
static pm3_saved_sys_regs sys_regs ;
static mdev_t *rtc_dev;
static int wakeup_reason;
#ifdef CONFIG_HW_RTC
static uint32_t  curr_rtc_ticks;
#endif
/*---------------------------------------------*/
/*------------------Extern Variables-----------*/
#if defined(CONFIG_ENABLE_MCU_PM3)
extern uint32_t *pm3_entryaddr;
#endif
#ifdef CONFIG_HW_RTC
extern uint8_t tcalib;
#endif
/*---------------------------------------------*/
/* This is used to detect wakeup form PM4*/
__SECTION__(.nvram_uninit)
uint32_t pm4_sig;

__USED__ uint32_t *pm3_entryaddr;
__USED__ __SECTION__(.nvram) uint32_t *nvram_saved_sp_addr;
/*---------------------------------------------*/
static void pm_idle_hook();
static void idle_hook();
#if defined(CONFIG_ENABLE_MCU_PM3)
static void  FLASHC_Disable();
#endif /* CONFIG_ENABLE_MCU_PM3 */

extern void CLK_Init(CLK_Src_Type input_ref_clk_src);

extern CLK_Src_Type CLK_Get_Current_Sfll_RefCLK();

/*---------------------------------------------*/

#ifndef CONFIG_HW_RTC
/*  This function is called on exit from low power modes
 *  due to RTC expiry.
 *  This updates OS global tick count  and also posix time
 *  counter on basis of the time duration for which system
 *  was in low power mode.
 *  @param [in] : time_duration in milliseconds.
 */
static void update_system_time_on_rtc_wakeup(uint32_t time_dur)
{
	time_t  curr_time;
	/* Wakeup was due to RTC expiry
	 * Simply update values using time duration for
	 * which RTC was configured
	 */
#if 0
	/* Update sys ticks*/
	os_update_tick_count(os_msec_to_ticks(time_dur));
#endif
	curr_time = wmtime_time_get_posix();
	curr_time += (time_dur / 1000);

	/* Update posix time in seconds*/
	wmtime_time_set_posix(curr_time);
}

/*  This function is called on exit from low power modes
 *  due to GPIO toggle.
 *  This updates OS global tick count  and also posix time
 *  counter on basis of the time duration for which system
 *  was in low power mode.
 */
static void update_system_time_on_gpio_wakeup()
{
	time_t  curr_time;
	/* Wakeup is by GPIO toggle
	 * read RTC counter value and convert it to
	 * ticks and seconds and update accordingly
	 */
	uint32_t rtc_count = rtc_drv_get(rtc_dev);
#if 0
	/* Update sys ticks*/
	os_update_tick_count(os_msec_to_ticks(rtc_count));
#endif
	curr_time = wmtime_time_get_posix();
	curr_time += (rtc_count / 1000);

	/* Update posix time in seconds*/
	wmtime_time_set_posix(curr_time);
}
#endif

void update_time(uint32_t time_dur)
{
#if 0
#ifdef CONFIG_HW_RTC
	if (wakeup_reason & ERTC)
		os_update_tick_count(time_dur);
	else
		os_update_tick_count((rtc_drv_get(rtc_dev) + tcalib)
				     - curr_rtc_ticks);
#else

	if (wakeup_reason & ERTC) {
		update_system_time_on_rtc_wakeup(time_dur);
	} else {
		update_system_time_on_gpio_wakeup();
	}
	rtc_drv_stop(rtc_dev);
	rtc_drv_set_cb(NULL);
#endif
#endif
}

static void populate_wakeup_reason()
{
	wakeup_reason |= NVIC->ISPR[0] & WAKEUP_SRC_MASK;

	/* WLAN NVIC line is 62 */
	volatile uint32_t nvic_ispr1 = NVIC->ISPR[1];
	if (nvic_ispr1 & (1 << (WIFIWKUP_IRQn - 32))) {
		wakeup_reason |= EWLAN;
	}
}

__USED__ __NOINLINE__
void ps_wait(int cycle)
{
	__asm volatile
	(
		 "loop:\n"
		 "nop\n"
		 "SUB r0 , r0 , #1\n"
		 "CMP r0 , #0\n"
		 "BNE.W loop\n"
		 "BX lr\n"
	);
}

static void switch_off_io_domains()
{
	/* Turn OFF different power domains. */
	/* This is done to make wakeup from PM2
	 * uisng Ext Pin 0 and Ext Pin 1.
	 * Please note that PM3 wakeup does
	 * not work after this change.
	*/
	PMU_PowerDownVDDIO(PMU_VDDIO_AON);
	if (pm_state.mode == PM2) {
		if (!(pm_state.io_domain_on & IO_DOMAIN_0))
			PMU_PowerOffVDDIO(PMU_VDDIO_0);
		if (!(pm_state.io_domain_on & IO_DOMAIN_1))
			PMU_PowerOffVDDIO(PMU_VDDIO_1);
		if (!(pm_state.io_domain_on & IO_DOMAIN_2))
			PMU_PowerOffVDDIO(PMU_VDDIO_2);
		if (!(pm_state.io_domain_on & IO_DOMAIN_3))
			PMU_PowerOffVDDIO(PMU_VDDIO_3);
	} else {
		PMU_PowerOffVDDIO(PMU_VDDIO_0);
		PMU_PowerOffVDDIO(PMU_VDDIO_1);
		PMU_PowerOffVDDIO(PMU_VDDIO_2);
		PMU_PowerOffVDDIO(PMU_VDDIO_3);
	}
}

static void switch_on_io_domains()
{
	/* Turn ON different power domains. */
	PMU_PowerOnVDDIO(PMU_VDDIO_0);
	PMU_PowerOnVDDIO(PMU_VDDIO_1);
	PMU_PowerOnVDDIO(PMU_VDDIO_2);
	PMU_PowerOnVDDIO(PMU_VDDIO_3);
	PMU_PowerNormalVDDIO(PMU_VDDIO_AON);
}

void extpin0_cb(void)
{
	wakeup_reason |= EGPIO0;
}

void extpin1_cb(void)
{
	wakeup_reason |= EGPIO1;
}

static void dummy_rtc_cb()
{
	wakeup_reason |= ERTC;
}

static void configure_enable_rtc(uint32_t low_pwr_duration)
{
	if (low_pwr_duration == 0)
		low_pwr_duration = 0xffffffff;

	_os_delay(1);
#ifdef CONFIG_HW_RTC
	curr_rtc_ticks = rtc_drv_get(rtc_dev);
	rtc_drv_set_alarm_cb(dummy_rtc_cb);
	rtc_drv_set_alarm(rtc_dev, low_pwr_duration);
#else
	rtc_drv_set(rtc_dev, low_pwr_duration);
	rtc_drv_reset(rtc_dev);
	rtc_drv_set_cb(dummy_rtc_cb);
	rtc_drv_start(rtc_dev);
#endif
}

/* Prepare to go to low power
 *  Change clock source to RC32M
*   Switch off PLLs, XTAL
*  Set Deep sleep bit in SRC register
*  Initiate state change
*/
static void prepare_for_low_power(uint32_t low_pwr_duration,
				  power_state_t state)
{
	/* Turn off Systick to avoid interrupt
	*  when entering low power state
	*/
	sys_regs.SYSTICK_CTRL = SysTick->CTRL;
	sys_regs.SYSTICK_LOAD = SysTick->LOAD;
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	configure_enable_rtc(low_pwr_duration);

	wakeup_reason = 0;

	switch (state) {
	case PM2:
		pm_invoke_ps_callback(ACTION_ENTER_PM2);
		break;
#if defined(CONFIG_ENABLE_MCU_PM3)
	case PM3: {
		/* On exit from PM3 bootrom jumps to address
		 * in pm3_entryaddr
		 */
		struct bootrom_info *info =
			(struct bootrom_info *)&_nvram_start;
		pm3_entryaddr = (&info->pm3_entryaddr);
		pm_invoke_ps_callback(ACTION_ENTER_PM3);
		FLASHC_Disable();
	}
		break;
#endif
	case PM4:
		pm_invoke_ps_callback(ACTION_ENTER_PM4);
		break;
	default:
		break;
	}
	/* Switch clock source to RC 32Mhz */
	CLK_SystemClkSrc(CLK_RC32M);

	/* Disable xtal */
	CLK_RefClkDisable(CLK_XTAL_REF);

	/* Power down analog unit */
	if (state == PM2) {
		PMU->ANA_GRP_CTRL0.BF.PU = 0;
	}
	/* Disable Audio PLL */
	CLK_AupllDisable();

	/* Disable System PLL */
	CLK_SfllDisable();

	FLASH_PowerDown(ENABLE);

	switch_off_io_domains();
}
/*
  Restore settings back to normal after low power mode exit
 */
static void pm2_post_low_power_operations(uint32_t time_dur)
{
	/* Restore to idle state */
	PMU_SetSleepMode(PMU_PM1);

	/* Power on the Analog controller group */
	PMU->ANA_GRP_CTRL0.BF.PU = 1;

	/* Switch on all IO domains which were switched off
	 * while entering PM2 standby mode
	 */
	switch_on_io_domains();
	FLASH_PowerDown(DISABLE);

	CLK_Init(CLK_Get_Current_Sfll_RefCLK());

	/* Switch on AUPLL clock domain */
	PMU->AUPLL_CTRL0.BF.PU = 1;

	populate_wakeup_reason();
	update_time(time_dur + pm_state.latency - 1);

	pm_invoke_ps_callback(ACTION_EXIT_PM2);

	/* restore Systick back to saved values */
	SysTick->CTRL = sys_regs.SYSTICK_CTRL;
	SysTick->LOAD = sys_regs.SYSTICK_LOAD;
}

/* This function puts system in standby state  */
void  mcu_pm2(uint32_t time_to_standby)
{
	/* Prepare for low power change  */
	prepare_for_low_power(time_to_standby, PM2);

	PMU_SetSleepMode(PMU_PM2);

	/*
	  This is needed to match up timing difference between
	  APB and AHB which causes malfunction.
	  PWR_MODE is written through APB bus and  then  SCR is written
	  through AHB bus followed by instruction WFI.
	  Due to the APB bus delay, there is possibility that CM3
	  enters sleep mode but PWR_MODE are not set to PM2 (or PM3 or PM4)
	  in time, which will result in failure to wakeup.
	*/

	/* The PMU -> PWR_MODE_STATUS.BF.PWR_MODE_STATUS is "0" before the
	 * chip really enter into the PM2/3/4 mode, so checking the bit is not
	 * the solution in current design. Hence add some delay instead.
	 */
	ps_wait(10);

	/*
	 * Execute WFI to generate a state change
	 * and system is in an unresponsive state
	 * press wakeup key to get it out of standby
	 * If time_to_standby is set to valid value
	 * RTC is programmed and RTC generates
	 * a wakeup signal
	*/
	__asm volatile ("wfi");
	pm2_post_low_power_operations(time_to_standby);
}

/* This function puts system in shut down state */
static void mcu_pm4(uint32_t time_to_wakeup)
{
	/* Prepare for low power */
	prepare_for_low_power(time_to_wakeup, PM4);

	pm4_sig = PM4_SIGN;
	/* Set the deepsleep bit
	   change PMU state to shutdown PM4 */
	PMU_SetSleepMode(PMU_PM4);
	/*
	  This is needed to match up timing difference between
	  APB and AHB which causes malfunction.
	  PWR_MODE is written through APB bus and  then  SCR is written
	  through AHB bus followed by instruction WFI.
	  Due to the APB bus delay, there is possibility that CM3
	  enters sleep mode but PWR_MODE are not set to PM2 (or PM3 or PM4)
	  in time, which will result in failure to wakeup.
	*/

	/* The PMU -> PWR_MODE_STATUS.BF.PWR_MODE_STATUS is "0" before the
	 * chip really enter into the PM2/3/4 mode, so checking the bit is not
	 * the solution in current design. Hence add some delay instead.
	 */
	ps_wait(10);
	/*
	 * Execute WFI to  generate a state change
	 * and system is in an unresponsive state
	 * press wakeup key to get it out of standby
	 */
	__asm volatile ("wfi");
}

void pm_mcu_io_cfg(uint32_t io_domains_on)
{
	pm_state.io_domain_on = io_domains_on;
}

void pm_mcu_cfg(bool enabled, power_state_t mode,
		  unsigned int threshold)
{
	if (mode > PM4 || mode < PM0) {
		return;
	}
	if (!is_pm_init_done()) {
		pm_e("Power Manager Module is not initialized.");
		return;
	}
	/* remove the default hook registered in pm_init */
	os_remove_idle_function(idle_hook);
	/* remove pm_idle_hook it might be already registered */
	os_remove_idle_function(pm_idle_hook);

	/* setup pm_idle_hook */
	os_setup_idle_function(pm_idle_hook);

	pm_state.enabled = enabled;
	pm_state.mode = mode;
	pm_state.threshold = os_msec_to_ticks(threshold);
	/*
	 *  SFLL config and  RC32M setup
	 *  takes approx 14 ms
	 */
	pm_state.latency = os_msec_to_ticks(15);
}

#if defined(CONFIG_ENABLE_MCU_PM3)

#if defined(__GNUC__)
void asm_mcu_pm3()
{
	/* Address: 0x480C0008 is the address in NVRAM which holds address
	 * where control returns after exit from PM3*/
	/* All general purpose registers and special registers
	*  are saved by pushing them on current thread's stack
	*  (psp is being used as sp) and finally SP is saved in NVRAM location*/

	__asm volatile
	(
		"push {r1}\n"
		"mrs r1 , msp\n"
		"push {r1}\n"
		"mrs r1 , basepri\n"
		"push {r1}\n"
		"mrs r1 , primask\n"
		"push {r1}\n"
		"mrs r1 , faultmask\n"
		"push  {r1}\n"
		"mrs r1 ,  control\n"
		"push {r1}\n"
		"push {r0-r12}\n"
		"push {lr}\n"
		"ldr r0 , =nvram_saved_sp_addr\n"
		"str sp , [r0]\n"
		"ldr r0 , =pm3_entryaddr\n"
		"mov r1 , pc\n"
		"add r1 , r1 , #20\n"
		"ldr r2 , [r0]\n"
		"str r1 , [r2]\n"
	 );
	/*
	 * Execute WFI to generate a state change
	 * and system is in an unresponsive state
	 * press wakeup key to get it out of standby
	 * If time_to_standby is set to valid value
	 * RTC is programmed and RTC generates
	 * a wakeup signal.
	 */
	__asm volatile ("wfi");

	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");

	/* When system exits PM3 all registers need to be
	 * restored as they are lost.
	 * After exit from PM3, control register is 0
	 * This indicates that msp is being used as sp
	 * psp is populated with saved_sp_addr value.
	 * When control register is popped, system starts using
	 * psp as sp and thread stack is now accessible.
	 */

	/*
	 * When MCU enters PM3 all Core registers
	 * r0-r12
	 * lr
	 * control
	 * primask
	 * faultmask
	 * basepri
	 * psp
	 * msp
	 * are lost (ZERO) as MCU power is tuned off
	 * On wakeup from PM3, this piece of code restores
	 * these registers which were saved before entry.
	 * The location of saving this register was on stack
	 * This stack was addressed as "sp" -> "psp".
	 * On exit "control" register becomes zero.
	 * Bit 1 of control register determines which register
	 * of "msp" or "psp" is mapped to "sp".
	 * If "control" register bit 1 is ZERO then use "msp"
	 * mapped to "sp" and restore the registers by popping them.
	 * If "control" register bit 1 is not ZERO then use "psp"
	 * mapped to "sp" and restore the registers by popping them.
	 *
	 */
	__asm volatile
	(
		"mrs r0 , control\n"
		"and r0 , #0x2\n"
		"cmp r0 , #0x2\n"
		"beq.w psp_lbl\n"
		"ldr r0 , =nvram_saved_sp_addr\n"
		"ldr sp , [r0]\n"
		"pop {lr}\n"
		"ldr r1 , [r0]\n"
		"msr psp , r1\n"
		"pop { r0-r12}\n"
		"mov r1 , sp\n"
		"add r1 , r1, #4\n"
		"msr psp , r1\n"
		"pop {r1}\n"
		"msr control , r1\n"
		"pop {r1}\n"
		"msr faultmask , r1\n"
		"pop {r1}\n"
		"msr primask , r1\n"
		"pop {r1}\n"
		"msr basepri , r1\n"
		"pop {r1}\n"
		"msr msp , r1\n"
		"pop {r1}\n"
		"b.w exit_lbl\n"
		"psp_lbl:\n"
		"pop {lr}\n"
		"pop {r0-r12}\n"
		"pop {r1}\n"
		"pop {r1}\n"
		"msr faultmask , r1\n"
		"pop {r1}\n"
		"msr primask , r1\n"
		"pop {r1}\n"
		"msr basepri , r1\n"
		"pop {r1}\n"
		"msr msp , r1\n"
		"pop {r1}\n"
		"exit_lbl:\n"
	 );
	/* In the assembly code above adding 4 in r1 is done to
	 * populate "psp" before "control" register is popped.
	 * This is needed as "control" register has bit 1 set to 1
	 * and it indicates that "sp" will be mapped to "psp" after
	 * the instruction that pops "control" register.
	 */
}
#elif defined(__ICCARM__)
void asm_mcu_pm3()
{
	__asm volatile ("push {r0-r12}");
	/* Address: 0x480C0008 is the address in NVRAM which holds address
	 * where control returns after exit from PM3*/
	/* All general purpose registers and special registers
	*  are saved by pushing them on current thread's stack
	*  (psp is being used as sp) and finally SP is saved in NVRAM location*/


	/* Important Note:
	 * IAR tool chain does not allow following syntax
	 *
	 * LDR Rx, =variable_name
	 * or
	 * LDR Ry, =0x10000000
	 *
	 * To overcome this issue a workaround is done to
	 * Load 0x480C0040 in a register using
	 * following instructions
	 *
	 * MOV, LSL and ADD
	 *
	 * 0x480C0040 will hold Stack Pointer
	 * before entering PM3
	 */

	__asm volatile ("mov r5, %0\n" : : "r"(&pm3_entryaddr));

	__asm volatile
	(
		"push {r1}\n"
		"mrs r1 , msp\n"
		"push {r1}\n"
		"mrs r1 , basepri\n"
		"push {r1}\n"
		"mrs r1 , primask\n"
		"push {r1}\n"
		"mrs r1 , faultmask\n"
		"push  {r1}\n"
		"mrs r1 ,  control\n"
		"push {r1}\n"
		"push {r0-r12}\n"
		"push {lr}\n"
		"mov r0 , #0x480C\n"
		"lsl r0 , r0 , #16\n"
		"add r0 , r0 , #0x0040\n"
		"str sp , [r0]\n"
		"mov r0 , r5\n"
		"mov r1 , pc\n"
		"add r1 , r1 , #20\n"
		"ldr r2 , [r0]\n"
		"str r1 , [r2]\n"
	 );
	/*
	 * Execute WFI to generate a state change
	 * and system is in an unresponsive state
	 * press wakeup key to get it out of standby
	 * If time_to_standby is set to valid value
	 * RTC is programmed and RTC generates
	 * a wakeup signal.
	 */
	__asm volatile ("wfi");

	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");
	__asm volatile ("nop");

	/* When system exits PM3 all registers need to be
	 * restored as they are lost.
	 * After exit from PM3, control register is 0
	 * This indicates that msp is being used as sp
	 * psp is populated with saved_sp_addr value.
	 * When control register is popped, system starts using
	 * psp as sp and thread stack is now accessible.
	 */

	/*
	 * When MCU enters PM3 all Core registers
	 * r0-r12
	 * lr
	 * control
	 * primask
	 * faultmask
	 * basepri
	 * psp
	 * msp
	 * are lost (ZERO) as MCU power is tuned off
	 * On wakeup from PM3, this piece of code restores
	 * these registers which were saved before entry.
	 * The location of saving this register was on stack
	 * This stack was addressed as "sp" -> "psp".
	 * On exit "control" register becomes zero.
	 * Bit 1 of control register determines which register
	 * of "msp" or "psp" is mapped to "sp".
	 * If "control" register bit 1 is ZERO then use "msp"
	 * mapped to "sp" and restore the registers by popping them.
	 * If "control" register bit 1 is not ZERO then use "psp"
	 * mapped to "sp" and restore the registers by popping them.
	 *
	 */
	__asm volatile
	(
		"mrs r0 , control\n"
		"and r0 , r0 , #0x2\n"
		"cmp r0 , #0x2\n"
		"beq.w psp_lbl\n"
		"mov r0 , #0x480C\n"
		"lsl r0 , r0 , #16\n"
		"add r0 , r0 , #0x0040\n"
		"ldr sp , [r0]\n"
		"pop {lr}\n"
		"ldr r1 , [r0]\n"
		"msr psp , r1\n"
		"pop { r0-r12}\n"
		"mov r1 , sp\n"
		"add r1 , r1, #4\n"
		"msr psp , r1\n"
		"pop {r1}\n"
		"msr control , r1\n"
		"pop {r1}\n"
		"msr faultmask , r1\n"
		"pop {r1}\n"
		"msr primask , r1\n"
		"pop {r1}\n"
		"msr basepri , r1\n"
		"pop {r1}\n"
		"msr msp , r1\n"
		"pop {r1}\n"
		"b.w exit_lbl\n"
		"psp_lbl:\n"
		"pop {lr}\n"
		"pop {r0-r12}\n"
		"pop {r1}\n"
		"pop {r1}\n"
		"msr faultmask , r1\n"
		"pop {r1}\n"
		"msr primask , r1\n"
		"pop {r1}\n"
		"msr basepri , r1\n"
		"pop {r1}\n"
		"msr msp , r1\n"
		"pop {r1}\n"
		"exit_lbl:\n"
	 );
	__asm volatile ("pop {r0-r12}");
	/* In the assembly code above adding 4 in r1 is done to
	 * populate "psp" before "control" register is popped.
	 * This is needed as "control" register has bit 1 set to 1
	 * and it indicates that "sp" will be mapped to "psp" after
	 * the instruction that pops "control" register.
	 */
}
#endif
/* Remove flash from continuous read mode and disable flash controller */
static void  FLASHC_Disable()
{
	if (FLASHC->FCCR.BF.FLASHC_PAD_EN) {
		uint32_t cacheMode =  FLASHC->FCCR.BF.CACHE_EN;
		FLASHC->FCCR.BF.CACHE_EN = DISABLE;

		/* Check if continuous read mode is enabled */
		if (FLASHC->FCCR.BF.CMD_TYPE == FLASHC_HW_CMD_FRQIOC) {
			FLASHC->FCCR.BF.CMD_TYPE = FLASHC_HW_CMD_ECRQ;
			FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE = 1;
			while (FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE == 0)
				;
		}

		FLASHC->FCCR.BF.CACHE_EN = cacheMode;
		PMU->LOW_PWR_CTRL.BF.CACHE_LINE_FLUSH = 0;
		FLASHC->FCCR.BF.FLASHC_PAD_EN = DISABLE;
	}
}

/* Enable flash controller if enabled before entering PM3 */
static void  FLASHC_Enable()
{
	if (sys_regs.FCCR & MASK_PAD_EN) {
		flashc_reg_t *fc = (flashc_reg_t *)&(sys_regs.FCCR);
		FLASHC->FCCR.BF.CACHE_LINE_FLUSH = 0;

		PMU->LOW_PWR_CTRL.BF.CACHE_LINE_FLUSH = 1;
		PMU->PERI1_CLK_DIV.BF.FLASH_CLK_DIV = 4;
		/* Enable Offset */
		FLASHC->FCACR.BF.OFFSET_EN = fc->FCACR.BF.OFFSET_EN;

		/* Get correct address offset */
		FLASHC->FAOFFR.WORDVAL = sys_regs.FAOFFR;

		/* Set the command type */
		FLASHC->FCCR.BF.CMD_TYPE = fc->FCCR.BF.CMD_TYPE;

		/* Set the clock prescale */
		FLASHC->FCCR.BF.CLK_PRESCALE = fc->FCCR.BF.CLK_PRESCALE;

		FLASHC->FCCR.BF.CACHE_EN = fc->FCCR.BF.CACHE_EN;

		FLASHC->FCCR.BF.SRAM_MODE_EN = fc->FCCR.BF.SRAM_MODE_EN;

		FLASHC->FCACR.BF.HIT_CNT_EN = fc->FCACR.BF.HIT_CNT_EN;
		FLASHC->FCACR.BF.MISS_CNT_EN = fc->FCACR.BF.MISS_CNT_EN;

		/* restore hit and miss count */
		FLASHC->FCMCR.WORDVAL = sys_regs.FCMCR;
		FLASHC->FCHCR.WORDVAL = sys_regs.FCHCR;

		/* Restore timing registers */
		FLASHC->FCTR.WORDVAL = sys_regs.FCTR;

		/* Finally enable the Flash Controller */
		FLASHC->FCCR.BF.FLASHC_PAD_EN = ENABLE;
	}
}

/*  This function saves SCB, NVIC and Sys Tick registers
 *  before entering low power state of PM3*/
static void save_system_context()
{
	volatile int cnt = 0;
	/* Save Interrupt Enable registers of NVIC */
	/* Since MCU defines only 64 interrupts
	 * Save only NVIC ISER [0]  NVIC ISER [1]
	 */
	for (cnt = 0 ; cnt < 2; cnt++)
		sys_regs.ISER[cnt] = NVIC->ISER[cnt];

	/* Save Interrupt Priority Registers of NVIC */
	for (cnt = 0 ; cnt < 64; cnt++)
		sys_regs.IP[cnt] = NVIC->IP[cnt];

	/* Save SCB configuration */
	sys_regs.ICSR = SCB->ICSR;
	sys_regs.VTOR = SCB->VTOR;
	sys_regs.AIRCR = SCB->AIRCR;
	sys_regs.SCR  =  SCB->SCR;
	sys_regs.CCR = SCB->CCR;
	for (cnt = 0 ; cnt < 12; cnt++)
		sys_regs.SHP[cnt] = SCB->SHP[cnt] ;

	sys_regs.SHCSR = SCB->SHCSR;
	sys_regs.CFSR = SCB->CFSR;
	sys_regs.HFSR = SCB->HFSR;
	sys_regs.DFSR = SCB->DFSR;
	sys_regs.MMFAR = SCB->MMFAR ;
	sys_regs.BFAR = SCB->BFAR;
	sys_regs.AFSR = SCB->AFSR;
	sys_regs.CPACR = SCB->CPACR ;

	/* Save sys tick registers */
	sys_regs.SYSTICK_CTRL = SysTick->CTRL;
	sys_regs.SYSTICK_LOAD = SysTick->LOAD;

	/* Save Flash controller configuration
	 * This needs to be restored on exit from PM3
	 */
	if (FLASHC->FCCR.BF.FLASHC_PAD_EN) {
		sys_regs.FCCR = FLASHC->FCCR.WORDVAL;
		sys_regs.FCTR = FLASHC->FCTR.WORDVAL;
		sys_regs.FCMCR = FLASHC->FCMCR.WORDVAL;
		sys_regs.FCHCR = FLASHC->FCHCR.WORDVAL;
		sys_regs.FCACR = FLASHC->FCACR.WORDVAL;
		sys_regs.FAOFFR = FLASHC->FAOFFR.WORDVAL;
	}

	/* Save QSPI configuration */
	sys_regs.CLK_PRESCALE = QSPI->CONF.BF.CLK_PRESCALE;
	sys_regs.CLK_CAPT_EDGE = QSPI->TIMING.BF.CLK_CAPT_EDGE;
}

static void restore_system_context()
{
	int cnt = 0;
	/* On exit from PM3 NVIc registers are not retained.
	 * A copy of ISER registers was saved in NVRAM beforee
	 * entering PM3.Using these saved values enable the interrupts.
	 */
	for (cnt = 0 ; cnt < 2 ; cnt++) {
		if (sys_regs.ISER[cnt]) {
			uint32_t temp_reg = sys_regs.ISER[cnt];
			int  i = 0;
			while (temp_reg) {
				if (temp_reg & 0x1)
					NVIC_EnableIRQ(i + cnt * 32);
				i++;
				temp_reg = temp_reg >> 1;
			}
		}
	}
	/* Restore NVIC interrupt priority registers  */
	for (cnt = 0 ; cnt < 64; cnt++)
		NVIC->IP[cnt] = sys_regs.IP[cnt];

	/* Restore SCB configuration  */
	SCB->ICSR = sys_regs.ICSR ;
	SCB->VTOR = sys_regs.VTOR;
	for (cnt = 0 ; cnt < 12; cnt++)
		SCB->SHP[cnt] =  sys_regs.SHP[cnt];
	SCB->AIRCR = sys_regs.AIRCR;
	SCB->SCR = sys_regs.SCR;
	SCB->CCR = sys_regs.CCR;

	SCB->SHCSR = sys_regs.SHCSR;
	SCB->CFSR = sys_regs.CFSR;
	SCB->HFSR = sys_regs.HFSR;
	SCB->DFSR = sys_regs.DFSR;
	SCB->MMFAR = sys_regs.MMFAR;
	SCB->BFAR = sys_regs.BFAR;
	SCB->AFSR = sys_regs.AFSR;
	SCB->CPACR = sys_regs.CPACR;

	/* Sys tick restore */
	SysTick->CTRL = sys_regs.SYSTICK_CTRL;
	SysTick->LOAD = sys_regs.SYSTICK_LOAD;

	/* Restore QSPI configuration */
	QSPI->CONF.BF.CLK_PRESCALE = sys_regs.CLK_PRESCALE;
	QSPI->TIMING.BF.CLK_CAPT_EDGE = sys_regs.CLK_CAPT_EDGE;
}

static void pm3_post_low_power_operations(uint32_t time_dur)
{
	/* Restore to idle state */
	PMU_SetSleepMode(PMU_PM1);
	switch_on_io_domains();

	FLASH_PowerDown(DISABLE);
	restore_system_context();

	populate_wakeup_reason();

	FLASHC_Enable();
	CLK_Init(CLK_Get_Current_Sfll_RefCLK());

	/* Switch on AUPLL clock domain */
	PMU->AUPLL_CTRL0.BF.PU = 1;

	update_time(time_dur + pm_state.latency - 1);
	pm_invoke_ps_callback(ACTION_EXIT_PM3);
}

/* This function puts system in sleep state */
static void mcu_pm3(uint32_t time_to_sleep)
{
	/* Save registers of NVIC and SCB  */
	save_system_context();

	/* Prepare for low power */
	prepare_for_low_power(time_to_sleep, PM3);

	/* Set the deepsleep bit
	   change PMU state to shutdown PM3 */
	PMU_SetSleepMode(PMU_PM3);

	/*
	  This is needed to match up timing difference between
	  APB and AHB which causes malfunction.
	  PWR_MODE is written through APB bus and  then  SCR is written
	  through AHB bus followed by instruction WFI.
	  Due to the APB bus delay, there is possibility that CM3
	  enters sleep mode but PWR_MODE are not set to PM2 (or PM3 or PM4)
	  in time, which will result in failure to wakeup.
	*/

	/* The PMU -> PWR_MODE_STATUS.BF.PWR_MODE_STATUS is "0" before the
	 * chip really enter into the PM2/3/4 mode, so checking the bit is not
	 * the solution in current design. Hence add some delay instead.
	 */
	ps_wait(10);
	asm_mcu_pm3();
	pm3_post_low_power_operations(time_to_sleep);
}
#endif
int pm_mcu_state(power_state_t state, uint32_t time_dur)
{
	if (state > PM4 || state < PM2) {
		return -WM_FAIL;
	}
	pm_state.mode = state;
	os_disable_all_interrupts();
	__disable_irq();
	switch (state) {
	case PM2:
		mcu_pm2(time_dur);
		break;
#if defined(CONFIG_ENABLE_MCU_PM3)
	case PM3:
		mcu_pm3(time_dur);
		break;
#endif
	case PM4:
		mcu_pm4(time_dur);
		break;
	default:
		break;
	}
	__enable_irq();
	os_enable_all_interrupts();
	return wakeup_reason;
}

int pm_wakeup_source()
{
	return wakeup_reason;
}

bool pm_mcu_is_enabled()
{
	return pm_state.enabled;
}

int pm_mcu_get_mode()
{
	return pm_state.mode;
}

static void idle_hook()
{
	PMU_SetSleepMode(PMU_PM1);
	__asm volatile ("wfi");
}

/* Function called when system executes idle thread */
static void pm_idle_hook()
{
#if 0
	os_disable_all_interrupts();
	__disable_irq();
	if (pm_state.enabled && !wakelock_isheld() &&
		(os_ticks_to_unblock() > (pm_state.threshold +
				pm_state.latency))) {
		switch (pm_state.mode) {
		case PM2:
			mcu_pm2(os_ticks_to_unblock()
					- pm_state.latency);
			break;
#if defined(CONFIG_ENABLE_MCU_PM3)
		case PM3:
			mcu_pm3(os_ticks_to_unblock()
					- pm_state.latency);
			break;
#endif
		default:
			break;
		}
		os_enable_all_interrupts();
	} else if (pm_state.mode != PM0) {
		os_enable_all_interrupts();
		PMU_SetSleepMode(PMU_PM1);
		__asm volatile ("wfi");
	} else
		/* enable interrupts for PM0 */
		os_enable_all_interrupts();

	__enable_irq();
#endif
}

void arch_pm_init(void)
{
	memset(&pm_state, 0, sizeof(pm_state));
	/*
	 * Set this to 1 so that WFI will be executed when
	 * core is in idle thread context. If it is set to 0
	 * by some application core will always be in PM0 mode..
	 */
	pm_state.mode = 1;
	os_setup_idle_function(idle_hook);

	rtc_dev = rtc_drv_open("MDEV_RTC");

	if (board_wakeup0_functional()) {
		install_int_callback(INT_EXTPIN0,
				     0, extpin0_cb);
		PMU_ClearWakeupSrcInt(PMU_WAKEUP_PIN0);
		NVIC_ClearPendingIRQ(ExtPin0_IRQn);
		NVIC_EnableIRQ(ExtPin0_IRQn);
		PMU_WakeupSrcIntMask(PMU_WAKEUP_PIN0, UNMASK);
		NVIC_SetPriority(ExtPin0_IRQn, 8);
	}
	if (board_wakeup1_functional()) {
		install_int_callback(INT_EXTPIN1,
				     0, extpin1_cb);
		PMU_ClearWakeupSrcInt(PMU_WAKEUP_PIN1);
		NVIC_ClearPendingIRQ(ExtPin1_IRQn);
		PMU_WakeupSrcIntMask(PMU_WAKEUP_PIN1, UNMASK);
		NVIC_EnableIRQ(ExtPin1_IRQn);
		NVIC_SetPriority(ExtPin1_IRQn, 8);

	}
	PMU_WakeupSrcIntMask(PMU_WAKEUP_RTC, UNMASK);
}

void arch_reboot()
{
	PMU_SetSleepMode(PMU_PM1);
	CLK_SystemClkSrc(CLK_RC32M);
	PMU_PowerDownWLAN();
	NVIC_SystemReset();
}

/* This API added to skip WFI in idle thread.
 *  It keeps MCU in PM0.
 *  Used in current measurement demo pm_mcu_demo.
 */
void pm_pm0_in_idle()
{
	/* remove the default hook registered in pm_init */
	os_remove_idle_function(idle_hook);
	/* remove pm_idl_hook it might be already registered */
	os_remove_idle_function(pm_idle_hook);
}

