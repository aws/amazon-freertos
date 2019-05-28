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

#include "hal_pwm.h"

#ifdef HAL_PWM_MODULE_ENABLED
#include "hal_log.h"
#include "pwm.h"
#include "low_hal_pwm.h"
#include <string.h>


/* PWM source clock */
static hal_pwm_source_clock_t g_source_clock;
static int32_t init_time = 0;


static bool hal_pwm_is_channel_valid(hal_pwm_channel_t pwm_channel)
{
    return pwm_channel < HAL_PWM_MAX;
}

static uint8_t hal_pwm_translate_source_clock(hal_pwm_source_clock_t source_clock)
{
    if (HAL_PWM_CLOCK_32KHZ == source_clock) {
        return PWM_CLK_SRC_32K;
    } else if (HAL_PWM_CLOCK_2MHZ == source_clock) {
        return PWM_CLK_SRC_2M;
    } else {
        return PWM_CLK_SRC_XTAL;
    }
}

static hal_pwm_status_t hal_pwm_calc_total_count(hal_pwm_source_clock_t source_clock,
        uint32_t frequency,
        uint32_t *total_count)
{
    static const uint32_t source_clock_table[] = {
        32700, 2 * 1000 * 1000, 20 * 1000 * 1000, 26 * 1000 * 1000, 40 * 1000 * 1000, 52 * 1000 * 1000
    };
    uint32_t source_clock_rate = source_clock_table[source_clock];
    if (frequency > source_clock_rate) {
        log_hal_error("target frequency %lu cannot be generated with source clock %lu.",
                      frequency, source_clock_rate);
        return HAL_PWM_STATUS_ERROR;
    }
    *total_count = source_clock_rate / frequency;
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_init(hal_pwm_source_clock_t source_clock)
{

    int32_t ret_val;
    if (init_time > 0) {
        log_hal_error(" Second init,which is abnormal behavior");
        return HAL_PWM_STATUS_ERROR;
    }

    ret_val = pwm_init(hal_pwm_translate_source_clock(source_clock));
    if (ret_val < 0) {
        log_hal_error("pwm_init fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    if (source_clock >= HAL_PWM_CLOCK_MAX) {
        log_hal_error("invalid source clock: %d.", source_clock);
        return HAL_PWM_STATUS_ERROR;
    }
    g_source_clock = source_clock;
    init_time = 1;
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_deinit(void)
{

    g_source_clock = HAL_PWM_CLOCK_MAX;
    init_time = 0;
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_set_frequency(hal_pwm_channel_t pwm_channel,
                                       uint32_t frequency,
                                       uint32_t *total_count)
{
    uint32_t smallest_frequency = 0;
    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    if (g_source_clock >= HAL_PWM_CLOCK_MAX) {
        log_hal_error("pwm not initilized.");
        return HAL_PWM_STATUS_ERROR;
    }
    if (0 == frequency) {
        log_hal_error("frequency cannot be %lu", frequency);
        return HAL_PWM_STATUS_ERROR;
    }
    uint32_t calculated = 0;
    if (HAL_PWM_STATUS_OK != hal_pwm_calc_total_count(g_source_clock, frequency, &calculated)) {
        log_hal_error("hal_pwm_calc_total_count fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    smallest_frequency = calculated * frequency / 0XFFFF;
    if (frequency < smallest_frequency) {
        return HAL_PWM_STATUS_INVALID_FREQUENCY;
    }
    int32_t ret_val = pwm_set((uint8_t)pwm_channel,
                              frequency,
                              0,
                              1);
    if (ret_val < 0) {
        log_hal_error("pwm_set fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    *total_count = calculated;
    return HAL_PWM_STATUS_OK;
}


hal_pwm_status_t hal_pwm_set_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t duty_cycle)
{
    int32_t ret_val;

    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    if (g_source_clock >= HAL_PWM_CLOCK_MAX) {
        log_hal_error("pwm not initilized.");
        return HAL_PWM_STATUS_ERROR;
    }
    uint32_t frequency = 0;
    uint16_t internal_duty = 0;
    uint8_t pwm_enable = 0;
    pwm_get((uint8_t)pwm_channel, &frequency, &internal_duty, &pwm_enable);

    if (0 == frequency) {
        log_hal_error("frequency is invalid.");
        return HAL_PWM_STATUS_ERROR;
    }
    uint32_t total_count = 0;
    if (HAL_PWM_STATUS_OK != hal_pwm_calc_total_count(g_source_clock, frequency, &total_count)) {
        log_hal_error("hal_pwm_calc_total_count fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    ret_val = pwm_set((uint8_t)pwm_channel,
                      frequency,
                      (uint16_t)duty_cycle,
                      pwm_enable);
    if (ret_val < 0) {
        log_hal_error("pwm_set fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_start(hal_pwm_channel_t pwm_channel)
{

    int32_t ret_val;
    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    uint32_t frequency = 0;
    uint16_t duty_cycle = 0;
    uint8_t pwm_enable = 0;
    pwm_get((uint8_t)pwm_channel, &frequency, &duty_cycle, &pwm_enable);

    if (0 == frequency) {
        log_hal_error("frequency is invalid.");
        return HAL_PWM_STATUS_ERROR;
    }
    ret_val = pwm_set((uint8_t)pwm_channel,
                      frequency,
                      duty_cycle,
                      1);
    if (ret_val < 0) {
        log_hal_error("pwm_set fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_stop(hal_pwm_channel_t pwm_channel)
{
    int32_t ret_val;
    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    uint32_t frequency = 0;
    uint16_t duty_cycle = 0;
    uint8_t pwm_enable = false;
    pwm_get((uint8_t)pwm_channel, &frequency, &duty_cycle, &pwm_enable);

    if (0 == frequency) {
        log_hal_error("frequency is invalid.");
        return HAL_PWM_STATUS_ERROR;
    }
    ret_val = pwm_set((uint8_t)pwm_channel,
                      frequency,
                      duty_cycle,
                      0);
    if (ret_val < 0) {
        log_hal_error("pwm_set fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_get_frequency(hal_pwm_channel_t pwm_channel, uint32_t *frequency)
{
    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    if (NULL == frequency) {
        log_hal_error("frequency is null");
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    uint16_t duty_cycle = 0;
    uint8_t pwm_enable = false;
    pwm_get((uint8_t)pwm_channel, frequency, &duty_cycle, &pwm_enable);

    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_get_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t *duty_cycle)
{
    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    uint32_t frequency = 0;
    uint16_t working_duty_cycle = 0;
    uint8_t pwm_enable = 0;
    pwm_get((uint8_t)pwm_channel, &frequency, &working_duty_cycle, &pwm_enable);

    if (0 == frequency) {
        log_hal_error("frequency is invalid.");
        return HAL_PWM_STATUS_ERROR;
    }
    uint32_t total_count = 0;
    if (HAL_PWM_STATUS_OK != hal_pwm_calc_total_count(g_source_clock, frequency, &total_count)) {
        log_hal_error("hal_pwm_calc_total_count fail.");
        return HAL_PWM_STATUS_ERROR;
    }
    *duty_cycle = working_duty_cycle;
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_get_running_status(hal_pwm_channel_t pwm_channel,
        hal_pwm_running_status_t *running_status)
{
    if (!hal_pwm_is_channel_valid(pwm_channel)) {
        log_hal_error("Invalid channel: %d", pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    uint32_t frequency = 0;
    uint16_t duty_cycle = 0;
    uint8_t pwm_enable = false;
    pwm_get((uint8_t)pwm_channel, &frequency, &duty_cycle, &pwm_enable);
    *running_status = pwm_enable ? HAL_PWM_BUSY : HAL_PWM_IDLE;
    return HAL_PWM_STATUS_OK;
}

#endif

