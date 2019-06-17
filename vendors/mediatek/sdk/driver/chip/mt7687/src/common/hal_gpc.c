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

#include "hal_gpc.h"

#ifdef HAL_GPC_MODULE_ENABLED
#include <stdbool.h>
#include "type_def.h"
#include "stddef.h"
#include "hal_gpc.h"
#include "hal_pcnt.h"
#include "hal_log.h"

static hal_pcnt_edge_t pcnt_edge;
static bool pcnt_state_work;
static bool hal_gpc_has_initilized;

hal_gpc_status_t hal_gpc_init(hal_gpc_port_t gpc_port, const hal_gpc_config_t *gpc_config)
{
    if ((hal_gpc_has_initilized != false) || (pcnt_state_work == true)) {
        log_hal_error("Port:%d is running\r\n", gpc_port);
        return HAL_GPC_STATUS_ERROR;
    }
    if (gpc_port >= HAL_GPC_MAX_PORT) {
        return HAL_GPC_STATUS_ERROR_PORT;
    }

    if (gpc_config->edge == HAL_GPC_EDGE_RAISING) {
        pcnt_edge = HAL_PCNT_EDGE_RAISING;
    } else {
        pcnt_edge = HAL_PCNT_EDGE_FALLING;
    }

    pcnt_state_work = FALSE;
    hal_gpc_has_initilized = true;

    return HAL_GPC_STATUS_OK;
}

hal_gpc_status_t hal_gpc_deinit(hal_gpc_port_t gpc_port)
{

    if (gpc_port >= HAL_GPC_MAX_PORT) {
        return HAL_GPC_STATUS_ERROR_PORT;
    }

    if (pcnt_state_work != false) {
        return HAL_GPC_STATUS_ERROR;
    }
    pcnt_edge = HAL_PCNT_EDGE_RAISING;
    pcnt_state_work = FALSE;
    hal_gpc_has_initilized = false;
    return HAL_GPC_STATUS_OK;
}

hal_gpc_status_t hal_gpc_get_running_status(hal_gpc_port_t gpc_port, hal_gpc_running_status_t *running_state)
{
    if (gpc_port >= HAL_GPC_MAX_PORT) {
        return HAL_GPC_STATUS_ERROR_PORT;
    }

    if (pcnt_state_work == FALSE) {
        *running_state = HAL_GPC_STATUS_IDLE;
    } else {
        *running_state = HAL_GPC_STATUS_BUSY;
    }

    return HAL_GPC_STATUS_OK;

}

hal_gpc_status_t hal_gpc_enable(hal_gpc_port_t gpc_port)
{
    if (hal_gpc_has_initilized != true) {
        return HAL_GPC_STATUS_ERROR;
    }
    if (gpc_port >= HAL_GPC_MAX_PORT) {
        return HAL_GPC_STATUS_ERROR_PORT;
    }

    hal_pcnt_config(TRUE, pcnt_edge);
    pcnt_state_work = TRUE;

    return HAL_GPC_STATUS_OK;
}

hal_gpc_status_t hal_gpc_disable(hal_gpc_port_t gpc_port)
{
    if (hal_gpc_has_initilized != true) {
        return HAL_GPC_STATUS_ERROR;
    }

    if (gpc_port >= HAL_GPC_MAX_PORT) {
        return HAL_GPC_STATUS_ERROR_PORT;
    }

    hal_pcnt_config(FALSE, pcnt_edge);
    pcnt_state_work = FALSE;

    return HAL_GPC_STATUS_OK;
}

hal_gpc_status_t hal_gpc_get_and_clear_count(hal_gpc_port_t gpc_port, uint32_t *count)
{
    if (hal_gpc_has_initilized != true) {
        return HAL_GPC_STATUS_ERROR;
    }
    if (gpc_port >= HAL_GPC_MAX_PORT) {
        return HAL_GPC_STATUS_ERROR_PORT;
    }

    *count = hal_pcnt_read_and_clear();

    return HAL_GPC_STATUS_OK;
}

#endif

