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

#include <stdlib.h>
#include "hal_sys.h"
#include "low_hal_wdt.h"
#include "mt7687_cm4_hw_memmap.h"
#include "type_def.h"

static hal_sys_reboot_callback_t s_callback = NULL;

//P_IOT_CM4_RGU_MEM_CTRL_TypeDef pCM4RguMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + IOT_CM4_RGU_MEM_CTRL_OFFSET);

hal_sys_reboot_callback_t hal_sys_reboot_register_callback(hal_sys_reboot_callback_t callback)
{
    hal_sys_reboot_callback_t old_callback = s_callback;
    s_callback = callback;
    return old_callback;
}


hal_sys_status_t hal_sys_reboot(uint32_t magic, uint32_t command)
{
    if (magic != HAL_SYS_REBOOT_MAGIC) {
        return HAL_SYS_INVALID_MAGIC;
    }

    if (command == WHOLE_SYSTEM_REBOOT_COMMAND) {
        hal_sys_reboot_callback_t callback = s_callback;
        if (callback != NULL) {
            callback(command);
        }

        /*call hardware reboot here!!! WDT HW timer trigger whole chip reset*/
        //CR 0x8300917C[16] = 1 : CM4 WDT whole chip mode, CM4 WDT reset whole chip included N9 domain
        //DRV_Reg32(pCM4RguMemCtrlTypeDef -> CM4_FPGA_RGU_DUMMY_CR) |=  BIT(16);

        halWDTConfig(0, 0, 0, 1); //CM4_WDT_RESET_MODE

        //halWDTSetTimeout(1);
        // Kick WDT to count down
        //halWDTEnable(1);
        halWDTSoftwareReset(); //trigger WDT immediatly

        while (1)
            ;
    } else if (command == 0x12345678) {
#if 0
        // this is an example of rebooting some subsystem, but not implmented.
        return HAL_SYS_INVALID_COMMAND;
#endif
    }

    return HAL_SYS_OK;
}

