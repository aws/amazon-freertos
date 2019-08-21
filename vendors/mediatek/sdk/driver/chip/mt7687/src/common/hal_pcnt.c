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
#include <stdio.h>
#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"
#include "hal_pcnt.h"

#define IOT_PCNT_CTRL   0x1A0
#define PCNT_EN 31
#define PCNT_CLR 30
#define PCNT_EDGE 29

hal_pcnt_status_t hal_pcnt_config(bool enable, hal_pcnt_edge_t edge)
{

    if (edge != HAL_PCNT_EDGE_FALLING && edge != HAL_PCNT_EDGE_RAISING) {
        return HAL_PCNT_STATUS_INVALID_PARAMETER;
    }

    // handle hardware setting here.
    if (enable == TRUE) {
        DRV_Reg32(CM4_TOPCFGAON_BASE + IOT_PCNT_CTRL) |= ((uint32_t)1 << PCNT_EN); // Enable PCNT

        if (edge == HAL_PCNT_EDGE_RAISING) {
            DRV_Reg32(CM4_TOPCFGAON_BASE + IOT_PCNT_CTRL) |= (1 << PCNT_EDGE);    // RAISING edge
        } else {
            DRV_Reg32(CM4_TOPCFGAON_BASE + IOT_PCNT_CTRL) &= ~(1 << PCNT_EDGE);    // FALLING edge
        }

    } else {
        DRV_Reg32(CM4_TOPCFGAON_BASE + IOT_PCNT_CTRL) &= ~((uint32_t)1 << PCNT_EN); // Enable PCNT

    }

    return HAL_PCNT_STATUS_OK;
}


uint32_t hal_pcnt_read_and_clear(void)
{
    uint32_t    reg = 0;

    // read cr and clear cr here
    reg = DRV_Reg32(CM4_TOPCFGAON_BASE + IOT_PCNT_CTRL) & 0x1FFFFFFF;
    DRV_Reg32(CM4_TOPCFGAON_BASE + IOT_PCNT_CTRL) |= (1 << PCNT_CLR);

    return reg;
}

#endif

