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

/**
 * @author  Mediatek
 * @version 1.0
 * @date    2015.10.29
 *
 */

#ifndef __HAL_SLEEP_DRIVER_H__
#define __HAL_SLEEP_DRIVER_H__

#include "hal_platform.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED

//PSI master
/*Global SleepDrv data structure*/
typedef struct {
    bool                handleInit; // the handleCount is initalized or not
    uint8_t             handleCount;
    uint32_t            sleepDisable;
    hal_sleep_mode_t    sleepMode;
} sleep_driver_struct;

typedef void (*sleepdrv_cb)(void *data);

typedef struct sleepdrv_cb_t {
    sleepdrv_cb func;
    void        *para;
    uint32_t    mode;
} sleepdrv_cb_t;

#define MAX_SLEEP_HANDLE                32
#define INVALID_SLEEP_HANDLE            0xFF

void sleep_driver_init(void);
uint8_t sleepdrv_get_handle(const char *handle_name, uint8_t size);
void sleepdrv_release_handle(uint8_t handle);
bool sleepdrv_is_handle_valid(uint8_t handle_index);
bool sleepdrv_get_handle_status(uint8_t handle);
bool sleepdrv_check_sleep_locks(void);
uint32_t sleepdrv_get_lock_index(void);
uint32_t sleepdrv_get_lock_module(void);
bool sleepdrv_hold_sleep_lock(uint8_t handle_index);
bool sleepdrv_release_sleep_lock(uint8_t handle_index);
bool sleepdrv_register_suspend_cb(sleepdrv_cb func, void *data, uint32_t mode);
void sleepdrv_run_suspend_cbs(uint32_t mode);
bool sleepdrv_register_resume_cb(sleepdrv_cb func, void *data, uint32_t mode);
void sleepdrv_run_resume_cbs(uint32_t mode);
void sleepdrv_register_callback(void);
void sleepdrv_set_sleep_mode(hal_sleep_mode_t mode);
hal_sleep_mode_t sleepdrv_get_sleep_mode(void);

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif /* __HAL_SLEEP_DRIVER_H__ */
