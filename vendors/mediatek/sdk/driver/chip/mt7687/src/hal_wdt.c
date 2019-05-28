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

#include "hal_wdt.h"
#if defined(HAL_WDT_MODULE_ENABLED)
#include "low_hal_wdt.h"
#include "mt7687.h"
#include "hal_nvic.h"

static hal_wdt_callback_t   s_hal_wdt_callback;
static bool                 s_hal_enable;
static hal_wdt_mode_t       s_hal_mode;

static void hal_wdt_isr(hal_nvic_irq_t irq_number)
{
    if (s_hal_wdt_callback) {
        s_hal_wdt_callback(HAL_WDT_TIMEOUT_RESET);
    }

}


hal_wdt_status_t hal_wdt_init(hal_wdt_config_t *wdt_config)
{
    if (!wdt_config) {
        return HAL_WDT_STATUS_INVALID_PARAMETER;
    }

    if (wdt_config->mode != HAL_WDT_MODE_RESET &&
            wdt_config->mode != HAL_WDT_MODE_INTERRUPT) {
        return HAL_WDT_STATUS_INVALID_PARAMETER;
    }

    s_hal_mode = (wdt_config->mode);

    // Call lhal WDT.
    halWDTConfig((wdt_config->mode), 0, 0, 1);
    halWDTSetTimeout((((wdt_config->seconds) * 1000) / 32) - 1);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_deinit(void)
{
    /* disable wdt */
    halWDTEnable(0);
    s_hal_enable = false;
    return HAL_WDT_STATUS_OK;
}


hal_wdt_status_t hal_wdt_enable(uint32_t magic)
{
    if (magic != HAL_WDT_ENABLE_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    s_hal_enable = true;

    // Call lhal WDT.
    halWDTEnable(1);

    return HAL_WDT_STATUS_OK;
}


hal_wdt_status_t hal_wdt_disable(uint32_t magic)
{
    if (magic != HAL_WDT_DISABLE_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    s_hal_enable = false;

    // Call lhal WDT.
    halWDTEnable(0);

    return HAL_WDT_STATUS_OK;
}


hal_wdt_status_t hal_wdt_feed(uint32_t magic)
{
    if (magic != HAL_WDT_FEED_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    // Call lhal WDT.
    halWDTRestart();

    return HAL_WDT_STATUS_OK;
}


hal_wdt_callback_t hal_wdt_register_callback(const hal_wdt_callback_t wdt_callback)
{
    hal_wdt_callback_t  old_callback;

    old_callback       = s_hal_wdt_callback;
    s_hal_wdt_callback = wdt_callback;

    // register ISR
    hal_nvic_register_isr_handler(CM4_WDT_IRQ, hal_wdt_isr);
    NVIC_SetPriority((IRQn_Type)CM4_WDT_IRQ, (uint32_t)CM4_WDT_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_WDT_IRQ);

    return old_callback;
}

hal_wdt_status_t hal_wdt_software_reset(void)
{
    halWDTSoftwareReset();
    return HAL_WDT_STATUS_OK;
}


bool hal_wdt_get_enable_status(void)
{
    return s_hal_enable;
}


hal_wdt_mode_t hal_wdt_get_mode(void)
{
    return s_hal_mode;
}

hal_wdt_reset_status_t hal_wdt_get_reset_status(void)
{
    uint32_t return_status;
    return_status = halWDTReadInterruptStatus();
    if (0 != (return_status & (0x1 << WDT_STA_WDT_OFFSET))) {
        return HAL_WDT_TIMEOUT_RESET;
    } else if (0 != (return_status & (0x1 << WDT_STA_SW_WDT_OFFSET))) {
        return HAL_WDT_SOFTWARE_RESET;
    }
    return HAL_WDT_NONE_RESET;
}

#endif
