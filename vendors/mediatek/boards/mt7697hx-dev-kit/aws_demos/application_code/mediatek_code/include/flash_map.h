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

#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__


#define LOADER_LENGTH           0x8000        /*  32KB */
#define RESERVED_LENGTH         0x8000        /*  32KB */
#define N9_RAMCODE_LENGTH       0x69000       /* 420KB */
#define CM4_CODE_LENGTH         0x1ED000      /* 1972KB */
#define FOTA_LENGTH             0x18A000      /* 1576KB */
#define NVDM_LENGTH             0x10000       /*  64KB */

#define LOADER_BASE             0x0
#define RESERVED_BASE           (LOADER_BASE     + LOADER_LENGTH)
#define N9_RAMCODE_BASE         (RESERVED_BASE   + RESERVED_LENGTH)
#define CM4_CODE_BASE           (N9_RAMCODE_BASE + N9_RAMCODE_LENGTH)
#define FOTA_BASE               (CM4_CODE_BASE   + CM4_CODE_LENGTH)
#define NVDM_BASE               (FOTA_BASE       + FOTA_LENGTH)
#define LOGGING_BASE            FOTA_BASE       



#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) 

/*which should NOT be changed by customer*/

#define CM4_FLASH_MEM_DUMP_1_ADDR   (LOGGING_BASE) 

#define CRASH_SYSLOG_BASE           CM4_FLASH_MEM_DUMP_1_ADDR 
#define CRASH_SYSLOG_LENGTH         0x3000      /* 12KB     */
#define NORMAL_SYSLOG_BASE          (CRASH_SYSLOG_BASE + CRASH_SYSLOG_LENGTH)
#define NORMAL_SYSLOG_LENGTH        0x14000    /* 80KB     */
#define CRASH_CONTEXT_BASE          (NORMAL_SYSLOG_BASE + NORMAL_SYSLOG_LENGTH)
#define CRASH_CONTEXT_LENGTH        0x53000    /* 332KB    */
#define CRASH_N9_DUMP_BASE          (CRASH_CONTEXT_BASE + CRASH_CONTEXT_LENGTH)
#define CRASH_N9_DUMP_LENGTH         0x4B000   /* 300KB     */

#endif


#define FLASH_BASE		        0x10000000

#define TCM_BASE                0x00100000
#define TCM_LENGTH              0x00010000  /* 64kB */



#endif // __FLASH_MAP_H__

