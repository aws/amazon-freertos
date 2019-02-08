/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_rtc.h"
#ifdef HAL_RTC_MODULE_ENABLED

#include "low_hal_rtc.h"
#include "mt7687.h"
#include "hal_nvic.h"
#include "hal_gpio.h"
#include <stdio.h>

volatile uint32_t *FRE_DET_CR = (volatile uint32_t *)0x80021120;
static hal_rtc_alarm_callback_t s_hal_rtc_alarm_callback = NULL;
static void                     *s_hal_rtc_user_data     = NULL;


static void hal_rtc_isr(hal_nvic_irq_t irq_number)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    // Clear interrupt status (Mandatory, or interrupt keeps raising)
    (pRTCCtrlTypeDef->RTC_PMU_EN) |= BIT(RTC_ALARM_STA_OFFSET);

    if (s_hal_rtc_alarm_callback) {
        s_hal_rtc_alarm_callback(s_hal_rtc_user_data);
    }
}

static hal_rtc_status_t hal_rtc_time_sanity(const hal_rtc_time_t *rtc_time)
{
    // Sanity check
    if (((rtc_time->rtc_year) > 99) ||
        (((rtc_time->rtc_mon) < 1) || ((rtc_time->rtc_mon) > 12)) ||
        (((rtc_time->rtc_day) < 1) || ((rtc_time->rtc_day) > 31)) ||
        ((rtc_time->rtc_week) > 6) ||
        ((rtc_time->rtc_hour) > 23) ||
        ((rtc_time->rtc_min) > 59) ||
        ((rtc_time->rtc_sec) > 59)) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_init(void)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCInit();

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_deinit(void)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCDeInit();

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_time(const hal_rtc_time_t *rtc_time)
{
    ENUM_HAL_RET_T      ret = HAL_RET_FAIL;
    hal_rtc_status_t    status;

    // Sanity check
    status = hal_rtc_time_sanity(rtc_time);

    if (HAL_RTC_STATUS_INVALID_PARAM == status) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    ret = halRTCSetTime(rtc_time->rtc_year,
                        rtc_time->rtc_mon,
                        rtc_time->rtc_day,
                        rtc_time->rtc_week,
                        rtc_time->rtc_hour,
                        rtc_time->rtc_min,
                        rtc_time->rtc_sec);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_time(hal_rtc_time_t *rtc_time)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCGetTime(&(rtc_time->rtc_year),
                        &(rtc_time->rtc_mon),
                        &(rtc_time->rtc_day),
                        &(rtc_time->rtc_week),
                        &(rtc_time->rtc_hour),
                        &(rtc_time->rtc_min),
                        &(rtc_time->rtc_sec));

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;

}

hal_rtc_status_t hal_rtc_set_alarm(const hal_rtc_time_t *rtc_time)
{
    ENUM_HAL_RET_T      ret = HAL_RET_FAIL;
    hal_rtc_status_t    status;

    // Sanity check
    status = hal_rtc_time_sanity(rtc_time);

    if (HAL_RTC_STATUS_INVALID_PARAM == status) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    ret = halRTCSetAlarm(rtc_time->rtc_year,
                         rtc_time->rtc_mon,
                         rtc_time->rtc_day,
                         rtc_time->rtc_week,
                         rtc_time->rtc_hour,
                         rtc_time->rtc_min,
                         rtc_time->rtc_sec,
                         0xFE);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_alarm(hal_rtc_time_t *rtc_time)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCGetAlarm(&(rtc_time->rtc_year),
                         &(rtc_time->rtc_mon),
                         &(rtc_time->rtc_day),
                         &(rtc_time->rtc_week),
                         &(rtc_time->rtc_hour),
                         &(rtc_time->rtc_min),
                         &(rtc_time->rtc_sec));

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_enable_alarm(void)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCEnableAlarm(true);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_disable_alarm(void)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCEnableAlarm(false);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_alarm_callback(const hal_rtc_alarm_callback_t callback_function, void *user_data)
{

    s_hal_rtc_alarm_callback = callback_function;
    s_hal_rtc_user_data      = user_data;

    // Alarm interrupt ISR
    hal_nvic_register_isr_handler(RTC_ALARM_IRQ, hal_rtc_isr);
    NVIC_SetPriority(RTC_ALARM_IRQ, CM4_RTC_ALARM_PRI);
    NVIC_EnableIRQ(RTC_ALARM_IRQ);

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_sleep(uint32_t magic)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;
    uint32_t irq_status;
    printf("hal_rtc_sleep entered\r\n");

    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    if (HAL_RTC_SLEEP_MAGIC == magic) {
        ret = halRTCEnterMode();
        if (HAL_RET_FAIL == ret) {
            hal_nvic_restore_interrupt_mask(irq_status);
            return HAL_RTC_STATUS_ERROR;
        }
    } else {
        hal_nvic_restore_interrupt_mask(irq_status);
        return HAL_RTC_STATUS_INVALID_PARAM;
    }
    hal_nvic_restore_interrupt_mask(irq_status);
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_data(uint16_t addr, const char *buf, uint16_t len)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCSetBackup(addr, buf, len);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_data(uint16_t addr, char *buf, uint16_t len)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCGetBackup(addr, buf, len);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_clear_data(uint16_t addr, uint16_t len)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    ret = halRTCClearBackup(addr, len);

    if (HAL_RET_FAIL == ret) {
        return HAL_RTC_STATUS_ERROR;
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_f32k_frequency(uint32_t *frequency)
{
    uint32_t frequency_cr = 0;
    uint32_t measure_result = 0;
    uint32_t cr_data = 0;
    uint32_t data = 0;

    /* check 32k source with strap pin GPIO6*/
    hal_gpio_init(HAL_GPIO_6);
    hal_gpio_set_direction(HAL_GPIO_6, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_get_input(HAL_GPIO_6, (hal_gpio_data_t *)&data);

    cr_data = *(volatile uint32_t *)0x81021100;
    cr_data &= 0xfffcffff; // clear bit 17:16
    if (data == 0) {
        //use internal 32k
        cr_data |= 0x20000; // bit[17:16] = 2
    } else if (data == 1) {
        //use external 32k
        cr_data |= 0x10000; // bit[17:16] = 1
    }
    *(volatile uint32_t *)0x81021100 = cr_data;

    /* CNT_CYCLE[2:0]=6, OSC_CK */
    *FRE_DET_CR = 0x000e0000;

    while (!(frequency_cr & 0x00008000)) {
        frequency_cr = *FRE_DET_CR;
    }

    measure_result = *FRE_DET_CR & 0x1fff;

    *frequency = 40000000 * 6 / measure_result;

    return HAL_RTC_STATUS_OK;
}


#endif /* HAL_RTC_MODULE_ENABLED */

