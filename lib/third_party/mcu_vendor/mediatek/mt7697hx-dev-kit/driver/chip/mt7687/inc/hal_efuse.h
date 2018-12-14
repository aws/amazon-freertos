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

#ifndef __HAL_EFUSE_H__
#define __HAL_EFUSE_H__

#include <stdint.h>
//#include "type_def.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file hal_efuse.h
 *
 * @brief see sys_efuse.c in DVT tree.
 */

#define HAL_EFUSE_WRITE_MAGIC (0x35929054)

typedef enum {
    HAL_EFUSE_OK,
    HAL_EFUSE_INVALID_PARAMETER,
    HAL_EFUSE_INVALID_MAGIC,
    HAL_EFUSE_INVALID_ACCESS,
    HAL_EFUSE_INVALID_HARDWARE,
    HAL_EFUSE_FULL
} hal_efuse_status_t;


/**
 * Write data into eFuse using logical address.
 *
 * Write <i>len</i> bytes of data in buffer <i>buf</i> into eFuse at the
 * specified address <i>addr</i>.
 *
 * @param addr  address to write to.
 * @param buf   the data sequence to be write into eFuse.
 * @param len   the length of the date.
 *
 * @retval HAL_EFUSE_OK if write succeeded.
 * @retval HAL_EFUSE_INVALID_PARAMETER buf is incorrect, address is not supported,
 *         or length is not supported.
 * @retval HAL_EFUSE_INVALID_MAGIC the magic is wrong.
 */
hal_efuse_status_t hal_efuse_write(
    uint32_t        magic,
    uint16_t        addr,
    const uint8_t   *buf,
    uint8_t         len);


/**
 * Read data from eFuse using logical address.
 *
 * Read <i>len</i> bytes of data from specified address <i>addr</i> in eFuse
 * into buffer <i>buf</i>.
 *
 * @param addr  address to read from.
 * @param buf   keep the read data sequence into this buffer.
 * @param len   the length of the date.
 *
 * @retval HAL_EFUSE_OK if read succeeded.
 * @retval HAL_EFUSE_INVALID_HARDWARE failure occurred when accessing eFUSE hardware.
 * @retval HAL_EFUSE_INVALID_PARAMETER buf is incorrect, address is not supported,
 * @retval HAL_EFUSE_INVALID_ACCESS the logical eFUSE block was not created yet.
 */
hal_efuse_status_t hal_efuse_read(
    uint16_t    addr,
    uint8_t     *buf,
    uint8_t     len);


/**
 * Read 16 bytes eFUSE using physical address.
 *
 * @param position      the eFUSE address.
 * @param data          the pointer that contains 16 bytes of data buffer.
 * @param log_data_vld  status. when the read succeeded, returns 1. otherwise,
 *                      it is 0;
 * @param mode          the read mode, when mode = 0, position is a physical
 *                      address, when mode = 1, position is a logical address.
 *
 * @note this API is from old code base. Its naming convention is not the same
 *       as MediaTek IoT LinkIt SDK.
 */
void SYSefuse_Read16Bytes(
    uint32_t    position,
    uint32_t    *data,
    uint32_t    *log_data_vld,
    uint8_t     mode);


/**
 * Write 16 bytes eFUSE using physical address.
 *
 * @param position the eFUSE address.
 * @param data the pointer that contains 16 bytes of data buffer.
 *
 * @return Always return 0.
 *
 * @note this API is from old code base. Its naming convention is not the same
 *       as MediaTek IoT LinkIt SDK.
 */
uint8_t SYSefuse_Write_physical_efuse(
    uint32_t    position,
    uint8_t     *data);


#ifdef __cplusplus
}
#endif

#endif /* __HAL_EFUSE_H__ */

