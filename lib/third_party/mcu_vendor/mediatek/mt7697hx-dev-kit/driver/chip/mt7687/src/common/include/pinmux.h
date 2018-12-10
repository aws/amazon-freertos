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
 * @file pinmux.h
 *
 *  Configure pinmux settings.
 *
 */

#include "hal_gpio.h"
#ifdef HAL_GPIO_MODULE_ENABLED

#ifndef __PINMUX_H__
#define __PINMUX_H__

#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    eIOT_GPIO_IDX_0 = 0,
    eIOT_GPIO_IDX_1 = 1,
    eIOT_GPIO_IDX_2 = 2,
    eIOT_GPIO_IDX_3 = 3,
    eIOT_GPIO_IDX_4 = 4,
    eIOT_GPIO_IDX_5 = 5,
    eIOT_GPIO_IDX_6 = 6,
    eIOT_GPIO_IDX_7 = 7,

    eIOT_GPIO_IDX_24 = 24,
    eIOT_GPIO_IDX_25 = 25,
    eIOT_GPIO_IDX_26 = 26,
    eIOT_GPIO_IDX_27 = 27,
    eIOT_GPIO_IDX_28 = 28,
    eIOT_GPIO_IDX_29 = 29,
    eIOT_GPIO_IDX_30 = 30,
    eIOT_GPIO_IDX_31 = 31,
    eIOT_GPIO_IDX_32 = 32,
    eIOT_GPIO_IDX_33 = 33,
    eIOT_GPIO_IDX_34 = 34,
    eIOT_GPIO_IDX_35 = 35,
    eIOT_GPIO_IDX_36 = 36,
    eIOT_GPIO_IDX_37 = 37,
    eIOT_GPIO_IDX_38 = 38,
    eIOT_GPIO_IDX_39 = 39,

    eIOT_GPIO_IDX_57 = 57,
    eIOT_GPIO_IDX_58 = 58,
    eIOT_GPIO_IDX_59 = 59,
    eIOT_GPIO_IDX_60 = 60,

} ENUM_IOT_GPIO_NAME_T;


/**
 * @brief  Configure pinmux settings of GPIO pin for selected function.
 *
 * @param  gpio_index [IN] Index of GPIO pin.
 * @param  function [IN] Selected function of GPIO pin.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t pinmux_config(ENUM_IOT_GPIO_NAME_T gpio_index, uint32_t function);


#ifdef __cplusplus
}
#endif

#endif /* __PINMUX_H__ */
#endif

