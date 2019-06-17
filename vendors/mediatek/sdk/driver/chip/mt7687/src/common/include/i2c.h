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
 * @file i2c.h
 *
 *  Two I2C master modules and 7-bit address is supported.
 *
 */


#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"
#include "type_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_BUSY  1
#define I2C_IDLE  0
#define i2c_check_and_set_busy(i2c_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(s_i2c_master_status[i2c_port] == I2C_BUSY){ \
        busy_status = HAL_I2C_STATUS_ERROR_BUSY; \
    } else { \
        s_i2c_master_status[i2c_port] = I2C_BUSY;  \
        busy_status = HAL_I2C_STATUS_OK; \
    } \
	   restore_interrupt_mask(saved_mask); \
}while(0)

#define i2c_set_idle(i2c_port)   \
do{  \
       s_i2c_master_status[i2c_port] = I2C_IDLE;  \
}while(0)

typedef struct {
    UINT8 ucSlaveAddress;
    PUINT8 pucTxData;
    PUINT8 pucRxData;
    UINT8 ucPktNum;
    UINT8 ucPkt0RW;
    UINT16 u2Pkt0Len;
    UINT8 ucPkt1RW;
    UINT16 u2Pkt1Len;
    UINT8 ucPkt2RW;
    UINT16 u2Pkt2Len;
} i2c_package_information_t;


/**
 * @brief  Configure clock frequency of specific I2C master.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  frequency [IN] 0:50K, 1:100K, 2:200K, 3:400K are supported.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related pinmux setting should be configured before using I2C.
 */
int32_t i2c_configure(uint8_t index, uint8_t frequency);


/**
 * @brief  Write data to slave device. I2C interrupt is raised to indicate write done.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  slave_address [IN] 7-bit address is supported, bit6(MSB)~bit0(LSB) are used to indicate slave address.
 * @param  data [IN] The pointer indicates the writing data.
 * @param  length [IN] Total length of valid data.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related I2C interrupt setting is required before i2c_write.
 */
int32_t i2c_write(uint8_t index, uint8_t slave_address, const uint8_t *data, uint16_t length);

/**
 * @brief  Write data to slave device. The writing operation is performed by MCU synchronously.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  slave_address [IN] 7-bit address is supported, bit6(MSB)~bit0(LSB) are used to indicate slave address.
 * @param  data [IN] The pointer indicates the writing data.
 * @param  length [IN] Total length of valid data.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related I2C interrupt setting is required before i2c_write.
 */
int32_t i2c_write_via_mcu(uint8_t index, uint8_t slave_address, const uint8_t *data, uint16_t length);

/**
 * @brief  Read data from slave device. I2C interrupt is raised to indicate read done.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  slave_address [IN] 7-bit address is supported, bit6(MSB)~bit0(LSB) are used to indicate slave address.
 * @param  data [OUT] The pointer indicates the reading data.
 * @param  length [IN] The data length needed to be read.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related I2C interrupt setting is required before i2c_read.
 */
int32_t i2c_read(uint8_t index, uint8_t slave_address, uint8_t *data, uint16_t length);

/**
 * @brief  Read data from slave device. The reading operation is performed by MCU synchronously.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  slave_address [IN] 7-bit address is supported, bit6(MSB)~bit0(LSB) are used to indicate slave address.
 * @param  data [OUT] The pointer indicates the reading data.
 * @param  length [IN] The data length needed to be read.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related I2C interrupt setting is required before i2c_read.
 */
int32_t i2c_read_via_mcu(uint8_t index, uint8_t slave_address, uint8_t *data, uint16_t length);

/**
 * @brief  Write data to slave device, and then read data from slave device, seperated with repeated start in between and without a stop.
 *         I2C interrupt is raised to indicate read done.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  package_information Contains information for each package.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related I2C interrupt setting is required before i2c_write.
 */
int32_t i2c_read_write(uint8_t index, i2c_package_information_t *package_information);

/**
 * @brief  Write data to slave device, and then read data from slave device, seperated with repeated start in between and without a stop.
 *         The operation is performed by MCU synchronously.
 * @param  index [IN] I2C0 and I2C1 are supported.
 * @param  package_information Contains information for each package.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related I2C interrupt setting is required before i2c_read.
 */
int32_t i2c_read_write_via_mcu(uint8_t index, i2c_package_information_t *package_information);

/**
 * @brief  Enable indicated I2C to reserve DMA channel.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note i2c_disable is required to release DMA channel reserved by I2C.
 */
int32_t i2c_enable(uint8_t index);

/**
 * @brief  Disable indicated I2C to release reserved DMA channel.
 *
 * @param  index [IN] I2C0 and I2C1 are supported.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t i2c_disable(uint8_t index);


#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

