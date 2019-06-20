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

#ifndef __HAL_DFS_H__
#define __HAL_DFS_H__

#include "hal_platform.h"

#ifdef HAL_DFS_MODULE_ENABLED


/*****************************************************************************
* Enum
*****************************************************************************/
/** @brief This enum defines return status of certain DFS HAL APIs. User should check return value after calling these APIs. */
typedef enum {
    HAL_DFS_STATUS_NOT_PERMITTED  = -5,      /**< The operation is not permitted. */
    HAL_DFS_STATUS_BUSY           = -4,      /**< Device or resource is busy. */
    HAL_DFS_STATUS_UNINITIALIZED  = -3,      /**< Non-initialized. */
    HAL_DFS_STATUS_INVALID_PARAM  = -2,      /**< Invalid parameter value. */
    HAL_DFS_STATUS_ERROR          = -1,      /**< The DFS function detected a common error. */
    HAL_DFS_STATUS_OK             =  0       /**< The DFS function executed successfully. */
} hal_dfs_status_t;


/*****************************************************************************
* extern global function
*****************************************************************************/
/**
 * @brief                       This function sets the current CPU frequency.
 * @param    target_freq        The CPU frequency is in mHz.
 * @return                      The operation status.
 */
hal_dfs_status_t hal_dfs_target_cpu_frequency(uint32_t target_freq);

/**
 * @brief       This function gets the current CPU frequency.
 * @return      The current CPU frequency is in mHz, if no error.
 */
uint32_t hal_dfs_get_cpu_frequency(void);

#endif /* HAL_DFS_MODULE_ENABLED */

#endif /* __HAL_DFS_H__ */
