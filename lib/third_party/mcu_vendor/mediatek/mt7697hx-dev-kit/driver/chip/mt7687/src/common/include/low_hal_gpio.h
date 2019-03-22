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
 * @file low_hal_gpio.h
 *
 *  GPIO related function.
 *
 */

#include "hal_eint.h"
#include "hal_gpio.h"
#if defined(HAL_EINT_MODULE_ENABLED) || defined(HAL_GPIO_MODULE_ENABLED)

#ifndef __GPIO_H__
#define __GPIO_H__

#include "stdint.h"
#include "pinmux.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    eDirection_IN,
    eDirection_OUT,
} ENUM_DIR_T;

typedef struct {
    uint32_t      GPIO_PIN;
    uint8_t       GPIO_OUTEN;
    uint8_t       GPI_DIN;
    uint8_t       GPO_DOUT;
} gpio_status;

typedef enum {
    eEdge_Trigger,
    eLevel_Trigger,
} ENUM_NVIC_SENSE_T;

typedef enum {
    eDebounce_Disable,
    eDebounce_Enable,
} ENUM_DEBOUNCE_EN_T;

typedef enum {
    eDebounce_Negative_Pol,
    eDebounce_Positive_Pol,
} ENUM_DEBOUNCE_POL_T;

typedef enum {
    eDebounce_Dual_No,
    eDebounce_Dual_Yes,
} ENUM_DEBOUNCE_DUAL_T;

typedef enum {
    eDebounce_Prescaler_8K,
    eDebounce_Prescaler_4K,
    eDebounce_Prescaler_2K,
    eDebounce_Prescaler_1K,
    eDebounce_Prescaler_Dot5K,
    eDebounce_Prescaler_Dot25K,
    eDebounce_Prescaler_Dot125K,
    eDebounce_Prescaler_Dot0625K,
} ENUM_DEBOUNCE_PRESCALER_T;

/**
 * @brief  Initialize GPIO with specific direction.
 *
 * @param  gpio_name [IN] The GPIO number which be configured.
 * @param  direction [IN] The direction of the GPIO: GPIO_DIRECTION_INPUT or GPIO_DIRECTION_OUTPUT.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_direction(ENUM_IOT_GPIO_NAME_T gpio_name, ENUM_DIR_T direction);

/**
 * @brief  Output GPIO with specific data.
 *
 * @param  gpio_name [IN] The GPIO number which output data.
 * @param  data [IN] The output data.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_write(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t data);


/**
 * @brief  GPIO Initial config.
 *
 * @param  gpio_name [IN] GPIO0~GPIO1, GPIO3~GPIO7, GPIO35, GPIO37~GPIO39.
 * @param  edgeLevelTrig  [IN] edge sensitive or level sensitive
 * @param  debounceEnable [IN]Enable or Disable De-bounce
 * @param  polarity       [IN]Positive: rising or positive trigger. Negative: falling or negative trigger
 * @param  dual           [IN] Edge: Rising and falling are trigger. Level: Negative and Positive are trigger.
 * @param  prescaler      [IN]8KHz , 4KHz , 2KHz , 1KHz , 0.5KHz , 0.25KHz , 0.125KHz , 0.0625KHz
 * @param  PrescalerCount [IN] The clock cycle period of de-bounce counter is determined by prescaler
 * @param  callback [IN] The callback function that GPIO interrupt is triggered.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_int(ENUM_IOT_GPIO_NAME_T       gpio_name,
                 ENUM_NVIC_SENSE_T         edgeLevelTrig,
                 ENUM_DEBOUNCE_EN_T        debounceEnable,
                 ENUM_DEBOUNCE_POL_T       polarity,
                 ENUM_DEBOUNCE_DUAL_T      dual,
                 ENUM_DEBOUNCE_PRESCALER_T prescaler,
                 uint8_t                     u1PrescalerCount,
                 void (*callback)(ENUM_IOT_GPIO_NAME_T gpio_name));


int32_t eint_set_trigger_mode(ENUM_IOT_GPIO_NAME_T gpio_name,
                 ENUM_NVIC_SENSE_T          edgeLevelTrig,
                 ENUM_DEBOUNCE_POL_T        polarity,
                 ENUM_DEBOUNCE_DUAL_T       dual);

int32_t eint_set_debounce(ENUM_IOT_GPIO_NAME_T gpio_name,
                 ENUM_DEBOUNCE_EN_T         debounceEnable,
                  ENUM_DEBOUNCE_PRESCALER_T prescaler,
                 uint8_t                    u1PrescalerCount);

int32_t eint_set_mask(ENUM_IOT_GPIO_NAME_T gpio_name);

int32_t eint_set_unmask(ENUM_IOT_GPIO_NAME_T gpio_name);

int32_t eint_set_software_trigger(ENUM_IOT_GPIO_NAME_T gpio_name);

int32_t eint_clear_software_trigger(ENUM_IOT_GPIO_NAME_T gpio_name);

void gpio_backup_all_registers(void);
void gpio_restore_all_registers(void);



/**
 * @brief  get gpio status.
 *
 * @param  gpio_name [IN] The GPIO number.
 *
 *
 * @return gpio_status struct.
 *
 */
gpio_status gpio_get_status(ENUM_IOT_GPIO_NAME_T gpio_name);


/**
 * @brief  Pullup gpio pin.
 *
 * @param  gpio_name [IN]  GPIO0~GPIO7, GPIO24~GPIO39,GPIO57~GPIO60.
 *
 *
 * @return  >=0 means success, <0 means fail.
 *
 */
int32_t gpio_PullUp(ENUM_IOT_GPIO_NAME_T gpio_name);

/**
 * @brief  Pull down gpio pin.
 *
 * @param  gpio_name [IN] GPIO0~GPIO7, GPIO24~GPIO39,GPIO57~GPIO60.
 *
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_PullDown(ENUM_IOT_GPIO_NAME_T gpio_name);

/**
 * @brief  Disable pull of gpio pin.
 *
 * @param  gpio_name [IN] GPIO0~GPIO7, GPIO24~GPIO39,GPIO57~GPIO60.
 *
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_PullDisable(ENUM_IOT_GPIO_NAME_T gpio_name);

/**
 * @brief  Set driving of gpio pin.
 *
 * @param  gpio_name [IN] GPIO0~GPIO7, GPIO24~GPIO39,GPIO57~GPIO60.
 * @param  driving [IN] The driving data.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_SetDriving(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t driving);


/**
 * @brief  Get driving of gpio pin.
 *
 * @param  gpio_name [IN] GPIO0~GPIO7, GPIO24~GPIO39,GPIO57~GPIO60.
 * @param  driving [IN] The driving data.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t gpio_GetDriving(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t *driving);
#ifdef __cplusplus
}
#endif

#endif
#endif

