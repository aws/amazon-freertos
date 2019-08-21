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

#ifndef __HAL_GDMA_INTERNAL_H__
#define __HAL_GDMA_INTERNAL_H__


#include "hal_platform.h"


#include "hal_nvic.h"
#include "hal_nvic_internal.h"

/* gdma channel number definition
*/
#define GDMA_NUMBER  2


/** @brief general dma transfer size format */
typedef enum {
    GDMA_BYTE = 0,            /**< general dma transfer size -byte format */
    GDMA_HALF_WORD = 1,       /**<general  dma transfer size -half word format */
    GDMA_WORD = 2             /**<general  dma transfer size -word  format */
} gdma_transfer_size_format_t;


/*define general dma's  callback function structure*/
typedef struct {
    hal_gdma_callback_t func;
    void *argument;
} gdma_user_callback_t;


typedef struct 
{
    uint32_t source_address;
    uint32_t dest_address;
    uint32_t count;
    uint32_t wppt;
    uint32_t wpto;
    uint32_t limiter;
    uint32_t wrap;  /* 0:no wrap; 1: wrap on src; 2: wrap on dst */
    bool  inc_src;
    bool  inc_dst;
    uint32_t size;   /* DMA_SIZE_1B or DMA_SIZE_2B or DMA_SIZE_4B */
    uint32_t burst;  /* DMA_BURST_SINGLE or DMA_BURST_4BEAT */
    bool iten;       /* interrupt enable */
    bool start;
} gdma_config_t;

#define MIN_LENGHT_VALUE 0x0001
#define MAX_LENGTH_VALUE 0x3FFFC
#define MAX_COUNT_VALUE  0xFFFF


#define INVALID_INDEX  0xff

#define DMA_BURST_SINGLE 0
#define DMA_BURST_4BEAT 2

#define DMA_SIZE_1B 0
#define DMA_SIZE_2B 1
#define DMA_SIZE_4B 2
#define DMA_MAX_SIZE 3


#define GDMA_INIT  1
#define GDMA_DEINIT  0
#define GDMA_CHECK_AND_SET_BUSY(gdma_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(gdma_init_status[gdma_port] == GDMA_INIT){ \
        busy_status = HAL_GDMA_STATUS_ERROR; \
    } else { \
        gdma_init_status[gdma_port] = GDMA_INIT;  \
        busy_status = HAL_GDMA_STATUS_OK; \
    } \
        restore_interrupt_mask(saved_mask); \
}while(0)

#define GDMA_SET_IDLE(gdma_port)   \
do{  \
	uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
       gdma_init_status[gdma_port] = GDMA_DEINIT;  \
    restore_interrupt_mask(saved_mask); \
}while(0)

gdma_transfer_size_format_t gdma_calculate_size(
    uint32_t length,
    uint32_t *count);

void gdma_reset(uint32_t channel);
uint32_t gdma_get_global_status(uint32_t channel);
void gdma_register_callback(uint32_t channel, hal_gdma_callback_t callback, void *user_data);
void gdma_stop(uint32_t channel);
void gdma_start(uint32_t channel);
void gdma_set_size(uint32_t channel, gdma_transfer_size_format_t size);
void gdma_set_direction(uint32_t channel, uint8_t direction);
void gdma_set_iten(uint32_t channel, bool enable);
void gdma_set_address(uint32_t channel, uint32_t destination, uint32_t source);
void gdma_set_count(uint32_t channel, uint32_t count);
void gdma_set_control(uint32_t index, uint32_t control, uint32_t count);
void gdma_init(uint32_t channel);
void gdma_deinit(uint32_t channel);

#endif //__HAL_GDMA_INTERNAL_H__
