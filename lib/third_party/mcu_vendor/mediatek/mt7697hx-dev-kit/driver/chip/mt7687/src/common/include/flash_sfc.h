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
 * @file flash_sfc.h
 *
 *  Flash related access function.
 *
 */

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#ifndef __FLASH_SFC_H__
#define __FLASH_SFC_H__

#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_MODE_SPI      (0)
#define FLASH_MODE_QPI      (1)
#define FLASH_MODE_NUM      (2)
#define FLASH_MODE_SPIQ     (3)

#ifdef OTP_FEATURE_SUPPORT
#define FLASH_MODE_INVALID  (0xFFFFFFFF)

typedef enum {
    OTP_READ,
    OTP_WRITE,
    OTP_LOCK
} ACCESS_TYPE_T;
#endif

/**
 * @brief  Configure flash.
 *
 * @param  mode [IN] Flash mode 0:SPI, 1:QPI are supported
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note   !!! IMPORTANT !!!
 *          If system is running on XIP, this routine itself should be resident at SYSRAM.
 *
 */
int32_t flash_sfc_config(uint8_t mode);


/**
 * @brief  Read flash data from specific address to buffer.
 *
 * @param  address [IN] Flash address.
 * @param  length [IN] Data length of read operation.
 * @param  buffer [OUT] The data would be resident at the memory that buffer is point to.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note   !!! IMPORTANT !!!
 *          If system is running on XIP, this routine itself should be resident at SYSRAM.
 *
 */
int32_t flash_sfc_read(uint32_t address, uint32_t length, uint8_t *buffer);


/**
 * @brief  Write flash data to specific address from memory pointed by buffer.
 *
 * @param  address [IN] Flash address.
 * @param  length [IN] Data length of write operation.
 * @param  buffer [OUT] The data would be resident at the memory that buffer is point to.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note   !!! IMPORTANT !!!
 *          If system is running on XIP, this routine itself should be resident at SYSRAM.
 *
 */
int32_t flash_sfc_write(uint32_t address, uint32_t length, const uint8_t *buffer);


/**
 * @brief  Erase a page(4kbytes) flash data on the specific flash address.
 *
 * @param  address [IN] Flash address, and it should be alias with 4096.
 * @param  type [IN] erase type
 * @return >=0 means success, <0 means fail.
 *
 * @note   !!! IMPORTANT !!!
 *          If system is running on XIP, this routine itself should be resident at SYSRAM.
 *
 */
int32_t flash_sfc_erase(uint32_t address, uint32_t type);

/**
 * @brief  read falsh by direct mode.
 *
 */
void hal_flash_direct_read(void *absolute_address, uint8_t *buffer, uint32_t length);


/**
 * @brief  suspend flash when flash programing if enter isr handler.
 *
 * @note   !!! IMPORTANT !!!
 *          If system is running on XIP, this routine itself should be resident at SYSRAM.
 *
 */
void Flash_ReturnReady(void);

int32_t SF_DAL_CheckDeviceReady(void *MTDData, uint32_t BlockIndex);

int32_t SF_DAL_CheckReadyAndResume(void *MTDData, uint32_t addr, uint8_t data);

void SF_DAL_FLAG_BUSY_SET(void);

void SF_DAL_FLAG_BUSY_CLR(void);

void SF_DAL_FLAG_SUS_SET(void);

void SF_DAL_FLAG_SUS_CLR(void);

#if defined(MTK_FLASH_POWER_SUPPLY_LOCK_DOWN)
int32_t flash_software_lockdown(uint32_t size);
#endif

#ifdef OTP_FEATURE_SUPPORT
int SF_DAL_OTPAccess(void *MTDData, int accesstype, uint32_t Offset, void *Buffer, uint32_t Length);
int SF_DAL_OTPQueryLength(void *MTDData, uint32_t *Length);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_SFC_ */
#endif

