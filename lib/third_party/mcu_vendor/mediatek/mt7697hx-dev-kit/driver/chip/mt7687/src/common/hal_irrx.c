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

#include "hal_irrx.h"


#ifdef HAL_IRRX_MODULE_ENABLED
#include "low_hal_irrx.h"
#include "hal_log.h"
#include <stdio.h>
#include <string.h>
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"


static hal_irrx_running_status_t s_hal_irrx_status = HAL_IRRX_IDLE;

#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint8_t irrx_sleep_handler = 0;
#endif

hal_irrx_status_t hal_irrx_init(void)
{
    halIrRxReset();

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*get sleep handler*/
    irrx_sleep_handler = hal_sleep_manager_set_sleep_handle("irrx");
    if (irrx_sleep_handler == INVALID_SLEEP_HANDLE) {
        log_hal_error("there's no available handle when IRRX get sleep handle");
        return HAL_IRRX_STATUS_NOT_SUPPORTED;
    }
    /*lock sleep mode*/
    hal_sleep_manager_lock_sleep(irrx_sleep_handler);
#endif
    return HAL_IRRX_STATUS_OK;
}



hal_irrx_status_t hal_irrx_deinit(void)
{
    halIrRxReset();

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*unlock sleep mode*/
    hal_sleep_manager_unlock_sleep(irrx_sleep_handler);
    /*release sleep hander*/
    hal_sleep_manager_release_sleep_handle(irrx_sleep_handler);
#endif

    return HAL_IRRX_STATUS_OK;
}



hal_irrx_status_t hal_irrx_receive_rc5_start(uint8_t             format,
        hal_irrx_callback_t callback,
        void                *parameter)
{
    UINT8 ucInverse = 0;
    UINT8 ucBitReverse = 0;
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    halIrRxReset();

    ucInverse = format & HAL_IRRX_RC5_RECEIVE_FORMAT_SIGNAL_INVERSE;
    ucBitReverse = format & HAL_IRRX_RC5_RECEIVE_FORMAT_BIT_REVERSE;

    ret = halIrRxRC5(ucInverse, ucBitReverse, 1, 1600); // RC5 should use 1600(800ms) for sample_period.
    if (HAL_RET_FAIL == ret) {
        return HAL_IRRX_STATUS_INVALID_PARAM;
    }
    halIrRxRegisterCallback(LOW_HAL_IRRX_MODE_RC5, (low_hal_irrx_callback_t)callback,parameter);

    s_hal_irrx_status = HAL_IRRX_BUSY;

    return HAL_IRRX_STATUS_OK;
}


hal_irrx_status_t hal_irrx_receive_rc5(hal_irrx_rc5_code_t *code)
{
    if (HAL_IRRX_BUSY != s_hal_irrx_status) {
        return HAL_IRRX_STATUS_NOT_SUPPORTED;
    }

    halIrRxRC5Read((PUINT8)(&(code->bits)), (PUINT32)(&(code->code[0])), (PUINT32)(&(code->code[1])));

    s_hal_irrx_status = HAL_IRRX_IDLE;

    return HAL_IRRX_STATUS_OK;
}


hal_irrx_status_t hal_irrx_receive_pwd_start(
    const hal_irrx_pwd_config_t     *config,
    hal_irrx_callback_t             callback,
    uint32_t                        *precision_us)
{
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;
    void   *parameter = NULL;

    halIrRxReset();

    ret = halIrRxPulseWidthDetect(3, config->inverse, config->terminate_threshold, (unsigned int *)(precision_us));
    if (HAL_RET_FAIL == ret) {
        return HAL_IRRX_STATUS_INVALID_PARAM;
    }
    halIrRxRegisterCallback(LOW_HAL_IRRX_MODE_PWD, (low_hal_irrx_callback_t)callback,parameter);

    s_hal_irrx_status = HAL_IRRX_BUSY;

    return HAL_IRRX_STATUS_OK;

}


hal_irrx_status_t hal_irrx_receive_pwd(uint8_t  *received_length,
                                       uint8_t  *buffer,
                                       uint8_t  buffer_length)
{
    if (HAL_IRRX_BUSY != s_hal_irrx_status) {
        return HAL_IRRX_STATUS_NOT_SUPPORTED;
    }

    halIrRxPWDRead((PUINT8)received_length, (PUINT8)buffer, (UINT8)buffer_length);

    s_hal_irrx_status = HAL_IRRX_IDLE;

    return HAL_IRRX_STATUS_OK;
}


hal_irrx_status_t hal_irrx_get_running_status(hal_irrx_running_status_t *running_status)

{
    *running_status = s_hal_irrx_status;
    return HAL_IRRX_STATUS_OK;

}

#endif

