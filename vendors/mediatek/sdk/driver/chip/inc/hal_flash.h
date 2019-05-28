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

#ifndef __HAL_FLASH_H__
#define __HAL_FLASH_H__
#include "hal_platform.h"

#ifdef HAL_FLASH_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup FLASH
 * @{
 * This section describes the programming interfaces of the FLASH driver.
 *
 * @section HAL_FLASH_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the FLASH driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b XIP                        | eXecute In Place: A method of executing programs directly from long term storage rather than copying it into the RAM |
 *
 * @section HAL_FLASH_Features_Chapter Supported features
 * The general purpose of FLASH is to save data into the embedded FLASH storage and read them back. The features supported by this module are listed below:
 * - \b FLASH \b erase \n
 * It has to be erased before data can be saved into the FLASH storage, function  #hal_flash_erase() implements that operation.
 * - \b FLASH \b write \n
 * Function #hal_flash_write() is called to save data into the FLASH storage after it's erased.
 * - \b FLASH \b read \n
 * Function #hal_flash_read() is called to read data from the FLASH storage.
 * @section HAL_FLASH_Driver_Usage_Chapter How to use this driver
 * - \b FLASH \b read \b and \b Write
 *   - step1: call #hal_flash_init() to initialize the FLASH module.
 *   - step2: call #hal_flash_erase() to erase the target block of the FLASH storage
 *   - step3: call #hal_flash_read() to read data from the FLASH storage
 *   - step4: call #hal_flash_write() to write data to the FLASH storage
 *   - Sample code:
 *   @code
 *  if (HAL_FLASH_STATUS_OK != hal_flash_init()) {
 *      //error handling
 *  }
 *  start_address = 0xAC000;// Start_address should be within flash size.
 *  if (HAL_FLASH_STATUS_OK != hal_flash_erase(start_address, HAL_FLASH_BLOCK_4K)) {
 *      //error handling
 *  }
 *  #define MAX_DATA (16)
 *  uint8_t data_to_write[MAX_DATA] = {0};
 *  if (HAL_FLASH_STATUS_OK != hal_flash_write(start_address, data_to_write, sizeof(data_to_write))) {
 *      //error handling
 *  }
 *  uint8_t data_read[MAX_DATA] = {0};
 *  if (HAL_FLASH_STATUS_OK != hal_flash_read(start_address, data_read, sizeof(data_read))) {
 *      //error handling
 *  }
 *  //data handling
 *   @endcode
 *
 */

#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_flash_enum Enum
 *  @{
 */


/** @brief flash API return status definition */
typedef enum {
    HAL_FLASH_STATUS_ERROR_LOCKDOWN_NOT_SUPPORT = -10,
    HAL_FLASH_STATUS_ERROR_LOCKDOWN_SIZE_INVALIED = -9,
    HAL_FLASH_STATUS_ERROR_LOCKDOWN_FAIL = -8,
    HAL_FLASH_STATUS_ERROR_ERASE_FAIL = -7,       /**< flash erase fail */
    HAL_FLASH_STATUS_ERROR_PROG_FAIL = -6,        /**< flash program fail */
    HAL_FLASH_STATUS_ERROR_NO_INIT = -5,          /**< flash driver don't initiate */
    HAL_FLASH_STATUS_ERROR_NO_SPACE = -4,         /**< flash has no space to write */
    HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS = -3,    /**< flash invalid access address */
    HAL_FLASH_STATUS_ERROR_LOCKED = -2,           /**< flash had locked */
    HAL_FLASH_STATUS_ERROR = -1,                  /**< flash function error */
    HAL_FLASH_STATUS_OK = 0                       /**< flash function ok */
} hal_flash_status_t;


/** @brief flash block definition */
typedef enum {
    HAL_FLASH_BLOCK_4K  = 0,          /**< flash erase block size 4k   */
    HAL_FLASH_BLOCK_32K = 1,          /**< flash erase block size 32k */
    HAL_FLASH_BLOCK_64K = 2           /**< flash erase block size 64k */
} hal_flash_block_t;


#ifdef MTK_FLASH_POWER_SUPPLY_LOCK_DOWN
/** @brief flash protect area definition  this is only for CMP = 0 of Winbond W25Q32JV. Please should confirm the define if use different device*/
typedef enum {
    HAL_FLASH_LOCK_DOWN_4K = 0,          /**< lock down 4k byte   */
    HAL_FLASH_LOCK_DOWN_8K = 1,          /**< lock down 8k byte   */
    HAL_FLASH_LOCK_DOWN_16K = 2,         /**< lock down 16k byte   */
    HAL_FLASH_LOCK_DOWN_32K = 3,         /**< lock down 32k byte   */
    HAL_FLASH_LOCK_DOWN_64K  = 4,        /**< lock down 64k byte   */
    HAL_FLASH_LOCK_DOWN_128K = 5,        /**< lock down 128k byte   */
    HAL_FLASH_LOCK_DOWN_256K = 6,        /**< lock down 256k byte   */
    HAL_FLASH_LOCK_DOWN_512K = 7,        /**< lock down 512k byte   */
    HAL_FLASH_LOCK_DOWN_1M   = 8,        /**< lock down 1M byte   */
    HAL_FLASH_LOCK_DOWN_2M   = 9,        /**< lock down 2M byte   */
    HAL_FLASH_LOCK_DOWN_BT_4K = 10,      /**< lock down from [4M -4k ~ 4M] */
    HAL_FLASH_LOCK_DOWN_BT_8K = 11,      /**< lock down from [4M -8k ~ 4M] */
    HAL_FLASH_LOCK_DOWN_BT_16K = 12,     /**< lock down from [4M -16k ~ 4M] */
    HAL_FLASH_LOCK_DOWN_BT_32K = 13,     /**< lock down from [4M -32k ~ 4M] */
    HAL_FLASH_LOCK_DOWN_4M = 14,         /**< lock down 4M byte   */
}hal_flash_lock_size_t;
#endif

/**
  * @}
 */


/*****************************************************************************
 * Structures
 *****************************************************************************/



/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief     flash init
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_init(void);


/**
 * @brief     flash deinit
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_deinit(void);


/**
 * @brief     flash erase
 * @param[in]  start_address is starting address to erase from
 * @param[in]  block_type is the size of block to be erased
 * @return
 * #HAL_FLASH_STATUS_OK on success
 * @note
 *  The start_address should be align with the block_type
 */
hal_flash_status_t hal_flash_erase(uint32_t start_address,  hal_flash_block_t block_type);


/**
 * @brief     flash read
 * @param[in]  start_address is starting address to read the data from
 * @param[out]  buffer is place to hold the incoming data
 * @param[in]  length is the length of the data content
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_read(uint32_t start_address, uint8_t *buffer, uint32_t length);


/**
 * @brief     flash write
 * @param[in]  address is starting address to write from.Before the address can be written to for the first time,
 *            the address located sector or block must first be erased.
 * @param[in]  length is data length
 * @param[in]  data is source data to be written
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_write(uint32_t address, const uint8_t *data, uint32_t length);


#if defined(MTK_FLASH_POWER_SUPPLY_LOCK_DOWN)
/**
 * @brief  flash lock down.
              it only supports device Winbond W25Q32JV. 
              please should check specification if use different device.
 * @param[in] it only supports some specific size.
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_lockdown(hal_flash_lock_size_t size);
#endif

#ifdef __cplusplus
}
#endif


/**
 * @}
 * @}
*/
#endif /*HAL_FLASH_MODULE_ENABLED*/
#endif /* __HAL_FLASH_H__ */


