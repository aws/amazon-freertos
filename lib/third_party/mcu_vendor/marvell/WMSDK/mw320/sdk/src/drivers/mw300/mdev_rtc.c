/** @file mdev_rtc.c
 *
 *  @brief This file provides  mdev driver for RTC
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


#include <wmstdio.h>
#include <wm_os.h>
#include <mdev_rtc.h>
#include <mdev_gpt.h>
#include <lowlevel_drivers.h>
#include <board.h>
#include <boot_flags.h>
#include <pwrmgr.h>

static mdev_t MDEV_rtc;
static const char *MDEV_NAME_rtc = "MDEV_RTC";

static void rtc_xtal32k_ps_cb(power_save_event_t event, void *data)
{
	if (board_32k_xtal()) {
		GPIO_PinMuxFun(GPIO_25, GPIO25_XTAL32K_IN);
		GPIO_PinMuxFun(GPIO_26, GPIO26_XTAL32K_OUT);
	} else {
		GPIO_PinMuxFun(GPIO_25, GPIO25_XTAL32K_IN);
	}
}

inline void rtc_drv_set_alarm_cb(void (*user_cb)())
{
	NVIC_ClearPendingIRQ(RTC_IRQn);
	install_int_callback(INT_RTC, RTC_INT_CNT_ALARM,
		(intCallback_Type *) user_cb);
	if (user_cb != NULL)
		RTC_IntMask(RTC_INT_CNT_ALARM, UNMASK);
	else
		RTC_IntMask(RTC_INT_CNT_ALARM, MASK);
}

inline void rtc_drv_set_cb(void (*user_cb)())
{
	NVIC_ClearPendingIRQ(RTC_IRQn);
	install_int_callback(INT_RTC, RTC_INT_CNT_UPP,
		(intCallback_Type *) user_cb);
	if (user_cb != NULL)
		RTC_IntMask(RTC_INT_CNT_UPP, UNMASK);
	else
		RTC_IntMask(RTC_INT_CNT_UPP, MASK);
}

inline void rtc_drv_set(mdev_t *dev, uint32_t cnt_upp)
{
	RTC_IntClr(RTC_INT_CNT_UPP);
	RTC_SetCounterUppVal(cnt_upp);
}

inline void rtc_drv_set_alarm(mdev_t *dev,
			      uint32_t cnt_alarm)
{
	/* RTC clock is at 32768 Hz.
	 * Ticks to be loaded should be
	 * in multiples of 1024 for accurate
	 * interrupt generation
	 */
	RTC_IntClr(RTC_INT_CNT_ALARM);
	uint64_t new_cnt = (((uint64_t) cnt_alarm << 10) / 1000) + rtc_drv_get(dev);
	if (new_cnt > 0xFFFFFFFF) {
		new_cnt = 0xFFFFFFFF;
	}
	RTC_SetCounterAlarmVal((uint32_t)new_cnt);
}

inline void rtc_drv_reset(mdev_t *dev)
{
	RTC_CounterReset();
}

inline unsigned int rtc_drv_get(mdev_t *dev)
{
	uint32_t cnt = 0;
	volatile uint32_t cnt_val;
	cnt_val = RTC_GetCounterVal();
	/* If RTC is not enabled, return the current value */
	if (!RTC_GetCntStatus())
		return cnt_val;

	/* Otherwise wait for at the most 10000 iterations for the counter to
	 * get incremented */
	while (cnt_val == 0 && cnt++ < 10000)
		cnt_val = RTC_GetCounterVal();

	return cnt_val;
}

inline unsigned int rtc_drv_get_uppval(mdev_t *dev)
{
	return RTC_GetCounterUppVal();
}

inline void rtc_drv_stop(mdev_t *dev)
{
	RTC_Stop();
}

inline void rtc_drv_start(mdev_t *dev)
{
	RTC_Start();
}

inline int rtc_drv_close(mdev_t *dev)
{
	return 0;
}

mdev_t *rtc_drv_open(const char *name)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		RTC_LOG("driver open called without registering device"
							" (%s)\n\r", name);
		return NULL;
	}

	return mdev_p;
}

static void rtc_drv_rc32k_calib()
{
	mdev_t *gpt_dev;
	volatile int32_t cnt = 1000;

	gpt_drv_init(GPT1_ID);
	gpt_dev = gpt_drv_open(GPT1_ID);

	/* Set GPT1 at ~ 32.768KHz and use it for calibration */
	PMU_PowerOnVDDIO(PMU_VDDIO_1);
	PMU_PowerOnVDDIO(PMU_VDDIO_AON);
	GPIO_PinMuxFun(GPIO_24, GPIO24_GPT1_CH5);
	GPIO_PinMuxFun(GPIO_25, GPIO25_XTAL32K_IN);
	CLK_GPTInternalClkSrc(CLK_GPT_ID_1, CLK_XTAL_REF);
	CLK_ModuleClkDivider(CLK_GPT1, 0);

	gpt_drv_set(gpt_dev, 0x51EB851); /* 0xffffffff / 50 */
	gpt_drv_pwm(gpt_dev, GPT_CH_5, 586, 586);
	gpt_drv_start(gpt_dev);

	CLK_Xtal32KEnable(CLK_OSC_EXTERN);
	/* Wait for about 1 sec */
	while ((CLK_GetClkStatus(CLK_XTAL32K) == RESET) && cnt--)
		_os_delay(1);
	if (cnt < 0)
		RTC_LOG("XTAL32K not ready");

	CLK_RC32KEnable();
	while (CLK_GetClkStatus(CLK_RC32K) == RESET)
		;
	CLK_RC32KCalibration(CLK_AUTO_CAL, 0);

	CLK_Xtal32KDisable();
	gpt_drv_stop(gpt_dev);
	gpt_drv_close(gpt_dev);
}

int rtc_drv_init(void)
{
	if (mdev_get_handle(MDEV_NAME_rtc) != NULL)
		return WM_SUCCESS;

	MDEV_rtc.name = MDEV_NAME_rtc;

	if (board_32k_xtal() || board_32k_osc()) {
		if (board_32k_xtal()) {
			/* Enable XTAL32K */
			GPIO_PinMuxFun(GPIO_25, GPIO25_XTAL32K_IN);
			GPIO_PinMuxFun(GPIO_26, GPIO26_XTAL32K_OUT);
		} else {
			/* Enable OSC32K */
			GPIO_PinMuxFun(GPIO_25, GPIO25_XTAL32K_IN);
		}

		/* Register a callback with  power manager of MCU
		 * this callback will be called on exit of PM3 mode
		 * of MW300
		 */
		pm_register_cb(ACTION_EXIT_PM3, rtc_xtal32k_ps_cb, NULL);
		if (board_32k_xtal())
			CLK_Xtal32KEnable(CLK_OSC_INTERN);
		else
			CLK_Xtal32KEnable(CLK_OSC_EXTERN);
		while (CLK_GetClkStatus(CLK_XTAL32K) == RESET)
			;
		CLK_RTCClkSrc(CLK_XTAL32K);
	} else if (board_rc32k_calib()) {
		/* Calibrate and use RC32K */
		CLK_RTCClkSrc(CLK_RC32K);
		rtc_drv_rc32k_calib();
	} else {
		/* Use uncalibrated RC32K */
		CLK_RTCClkSrc(CLK_RC32K);
		CLK_RC32KEnable();
	}
	CLK_ModuleClkEnable(CLK_RTC);

	/* Configure if not already running */
	if (!RTC_GetCntStatus()) {
		RTC_Config_Type rtc_cfg;
		rtc_cfg.CntValUpdateMode = RTC_CNT_VAL_UPDATE_AUTO;
		/* Set the clock to 1024 Hz */
		rtc_cfg.clockDivider = 5;
		rtc_cfg.uppVal = 0xffffffff;
		while (!RTC->CNT_EN_REG.BF.STS_RESETN)
			;
		RTC_Init(&rtc_cfg);
	}

	/* This needs to be higher than kernel priority
	 * so that RTC ISR is executed before the
	 * scheduler
	 */
	NVIC_SetPriority(RTC_IRQn, 0x8);
	NVIC_EnableIRQ(RTC_IRQn);

	mdev_register(&MDEV_rtc);

	return WM_SUCCESS;
}
