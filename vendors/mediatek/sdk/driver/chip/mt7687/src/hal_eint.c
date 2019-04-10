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

#include "hal_eint.h"
#ifdef HAL_EINT_MODULE_ENABLED

#include <string.h>
#include "hal_eint.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "low_hal_gpio.h"
#include "hal_gpio_7687.h"
#include "nvic.h"
#include "stdbool.h"

typedef struct {
    bool callback_present;
    hal_eint_callback_t callback;
    void *user_data;
} hal_eint_callback_context_t;



const  hal_eint_gpio_map_t eint_gpio_table[] = {
    {HAL_EINT_NUMBER_0,  eIOT_GPIO_IDX_0,  CM4_EINT_0_IRQ,  CM4_EINT_0_PRI},
    {HAL_EINT_NUMBER_1,  eIOT_GPIO_IDX_1,  CM4_EINT_1_IRQ,  CM4_EINT_1_PRI},
    {HAL_EINT_NUMBER_2,  eIOT_GPIO_IDX_3,  CM4_EINT_2_IRQ,  CM4_EINT_2_PRI},
    {HAL_EINT_NUMBER_3,  eIOT_GPIO_IDX_4,  CM4_EINT_3_IRQ,  CM4_EINT_3_PRI},
    {HAL_EINT_NUMBER_4,  eIOT_GPIO_IDX_5,  CM4_EINT_4_IRQ,  CM4_EINT_4_PRI},
    {HAL_EINT_NUMBER_5,  eIOT_GPIO_IDX_6,  CM4_EINT_5_IRQ,  CM4_EINT_5_PRI},
    {HAL_EINT_NUMBER_6,  eIOT_GPIO_IDX_7,  CM4_EINT_6_IRQ,  CM4_EINT_6_PRI},
    {HAL_EINT_NUMBER_19, eIOT_GPIO_IDX_35, CM4_EINT_19_IRQ, CM4_EINT_19_PRI},
    {HAL_EINT_NUMBER_20, eIOT_GPIO_IDX_37, CM4_EINT_20_IRQ, CM4_EINT_20_PRI},
    {HAL_EINT_NUMBER_21, eIOT_GPIO_IDX_38, CM4_EINT_21_IRQ, CM4_EINT_21_PRI},
    {HAL_EINT_NUMBER_22, eIOT_GPIO_IDX_39, CM4_EINT_22_IRQ, CM4_EINT_22_PRI},
    {HAL_WIC_NUMBER_0,   eIOT_GPIO_IDX_2,  CM4_WIC_0_IRQ,   CM4_WIC_0_PRI},
    {HAL_WIC_NUMBER_1,   eIOT_GPIO_IDX_25, CM4_WIC_1_IRQ,   CM4_WIC_1_PRI},
    {HAL_WIC_NUMBER_2,   eIOT_GPIO_IDX_27, CM4_WIC_2_IRQ,   CM4_WIC_2_PRI},
    {HAL_WIC_NUMBER_3,   eIOT_GPIO_IDX_29, CM4_WIC_3_IRQ,   CM4_WIC_3_PRI},
    {HAL_WIC_NUMBER_4,   eIOT_GPIO_IDX_32, CM4_WIC_4_IRQ,   CM4_WIC_4_PRI},
    {HAL_WIC_NUMBER_5,   eIOT_GPIO_IDX_33, CM4_WIC_5_IRQ,   CM4_WIC_5_PRI}, 
    {HAL_WIC_NUMBER_6,   eIOT_GPIO_IDX_34, CM4_WIC_6_IRQ,   CM4_WIC_6_PRI},
    {HAL_WIC_NUMBER_7,   eIOT_GPIO_IDX_36, CM4_WIC_7_IRQ,   CM4_WIC_7_PRI},
    {HAL_WIC_NUMBER_8,   eIOT_GPIO_IDX_57, CM4_WIC_8_IRQ,   CM4_WIC_8_PRI},
    {HAL_WIC_NUMBER_9,   eIOT_GPIO_IDX_58, CM4_WIC_9_IRQ,   CM4_WIC_9_PRI},
    {HAL_WIC_NUMBER_10,  eIOT_GPIO_IDX_59, CM4_WIC_10_IRQ,  CM4_WIC_10_PRI},
    {HAL_WIC_NUMBER_11,  eIOT_GPIO_IDX_60, CM4_WIC_11_IRQ,  CM4_WIC_11_PRI}
};


#define HAL_EINT_COUNT (sizeof(eint_gpio_table)/sizeof(hal_eint_gpio_map_t))

const uint32_t hal_eint_count_max = HAL_EINT_COUNT;

static hal_eint_callback_context_t g_eint_callback_context[HAL_EINT_COUNT];
static uint32_t is_eint_callback_registered[HAL_EINT_COUNT];
static uint32_t is_eint_masked[HAL_EINT_COUNT];

static uint32_t get_index_from_gpio_pin(ENUM_IOT_GPIO_NAME_T gpio_pin)
{
    uint32_t index = 0;
    for (index = 0; index < hal_eint_count_max; index++) {
        if (eint_gpio_table[index].gpio_pin == gpio_pin) {
            return index;
        }
    }
    return (0xFFFFFFFF);
}

static int32_t hal_eint_convert_for_gpio(hal_eint_number_t eint_number, ENUM_IOT_GPIO_NAME_T *gpio_pin)
{
    uint32_t count = sizeof(eint_gpio_table) / sizeof(eint_gpio_table[0]);
    uint32_t index = 0;
    for (index = 0; index < count; index++) {
        if (eint_gpio_table[index].eint_number == eint_number) {
            *gpio_pin = eint_gpio_table[index].gpio_pin;
            return 0;
        }
    }
    return -1;
}

static void hal_eint_calc_debounce_parameters(uint32_t debounce_time,
        ENUM_DEBOUNCE_PRESCALER_T *sample_rate,
        uint32_t *sample_count)
{
    /*
        We prefer a higher sample rate for debouncing, i.e. check a number of times
        to make sure any interference to be detected and filtered.
        But checking too many times doesn't help much, so a threshold is defined,
        to scale among different sample rates.
    */
    uint32_t const threshold = 10;
    uint32_t target_count = 0;
    if ((target_count = debounce_time * 8) <= threshold) {
        *sample_rate = eDebounce_Prescaler_8K;
    } else if ((target_count = debounce_time * 4) <= threshold) {
        *sample_rate = eDebounce_Prescaler_4K;
    } else if ((target_count = debounce_time * 2) <= threshold) {
        *sample_rate = eDebounce_Prescaler_2K;
    } else if ((target_count = debounce_time * 1) <= threshold) {
        *sample_rate = eDebounce_Prescaler_1K;
    } else if ((target_count = debounce_time / 2) <= threshold) {
        *sample_rate = eDebounce_Prescaler_Dot5K;
    } else if ((target_count = debounce_time / 4) <= threshold) {
        *sample_rate = eDebounce_Prescaler_Dot25K;
    } else if ((target_count = debounce_time / 8) <= threshold) {
        *sample_rate = eDebounce_Prescaler_Dot125K;
    } else {
        *sample_rate = eDebounce_Prescaler_Dot0625K;
        *sample_count = debounce_time / 16;
    }
    *sample_count = target_count;
}

static void hal_eint_isr(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_name)];
    if ((true == callback_context->callback_present)
            && (NULL != callback_context->callback)) {
        callback_context->callback(callback_context->user_data);
    }
}

static hal_eint_status_t hal_eint_apply_config(ENUM_IOT_GPIO_NAME_T gpio_pin,
        const hal_eint_config_t *eint_config)
{
    uint32_t sample_count;
    int32_t ret_value;
    ENUM_NVIC_SENSE_T trigger_mode;
    ENUM_DEBOUNCE_POL_T trigger_polarity;
    ENUM_DEBOUNCE_PRESCALER_T sample_rate;
    ENUM_DEBOUNCE_DUAL_T dual_edge;

    dual_edge = eDebounce_Dual_No;
    switch (eint_config->trigger_mode) {
        case  HAL_EINT_LEVEL_LOW : {
            trigger_mode = eLevel_Trigger;
            trigger_polarity = eDebounce_Positive_Pol;
            break;
        }
        case  HAL_EINT_LEVEL_HIGH : {
            trigger_mode = eLevel_Trigger;
            trigger_polarity = eDebounce_Negative_Pol;
            break;
        }
        case  HAL_EINT_EDGE_FALLING : {
            trigger_mode = eEdge_Trigger;
            trigger_polarity = eDebounce_Positive_Pol;
            break;
        }
        case  HAL_EINT_EDGE_RISING : {
            trigger_mode = eEdge_Trigger;
            trigger_polarity = eDebounce_Negative_Pol;
            break;
        }
        case  HAL_EINT_EDGE_FALLING_AND_RISING : {
            trigger_mode = eEdge_Trigger;
            trigger_polarity = eDebounce_Negative_Pol;
            dual_edge = eDebounce_Dual_Yes;
            break;
        }
        default :
            return HAL_EINT_STATUS_ERROR;
    }

    hal_eint_calc_debounce_parameters(eint_config->debounce_time,
                                      &sample_rate,
                                      &sample_count);
    if (sample_count == 0) {
        ret_value = gpio_int(gpio_pin,
                             trigger_mode,
                             eDebounce_Disable,
                             trigger_polarity,
                             dual_edge,
                             sample_rate,
                             sample_count,
                             hal_eint_isr);
        log_hal_error("hal_eint_init: debounce disable.");
    } else {
        ret_value = gpio_int(gpio_pin,
                             trigger_mode,
                             eDebounce_Enable,
                             trigger_polarity,
                             dual_edge,
                             sample_rate,
                             sample_count,
                             hal_eint_isr);
    }

    return (ret_value >= 0) ? HAL_EINT_STATUS_OK : HAL_EINT_STATUS_ERROR;
}

#ifdef HAL_EINT_FEATURE_MASK
hal_eint_status_t hal_eint_mask(hal_eint_number_t eint_number)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    eint_set_mask(gpio_pin);
    is_eint_masked[eint_number] = true;
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_unmask(hal_eint_number_t eint_number)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
     eint_set_unmask(gpio_pin);
     is_eint_masked[eint_number] = false;
     return HAL_EINT_STATUS_OK;
}
#endif

hal_eint_status_t hal_eint_init(hal_eint_number_t eint_number, const hal_eint_config_t *eint_config)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    hal_eint_status_t    ret_status;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    if (NULL == eint_config) {
        log_hal_error("eint_config is NULL.");
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }
    
    if (is_eint_callback_registered[eint_number] == true) {
         ret_status = hal_eint_apply_config(gpio_pin, eint_config);
         if (ret_status != HAL_EINT_STATUS_OK) {
            return ret_status;
         }
         
         ret_status = hal_eint_unmask(eint_number); // align with orignal behavior
         if (ret_status != HAL_EINT_STATUS_OK) {
            return ret_status;
         }
    }
    else {
         hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_pin)];
         memset(callback_context, 0, sizeof(hal_eint_callback_context_t));

         ret_status = hal_eint_apply_config(gpio_pin, eint_config);
         if (ret_status != HAL_EINT_STATUS_OK) {
            return ret_status;
         }

         ret_status = hal_eint_mask(eint_number);
         if (ret_status != HAL_EINT_STATUS_OK) {
            return ret_status;
         }
    }
   
    return HAL_EINT_STATUS_OK;
    
}

hal_eint_status_t hal_eint_deinit(hal_eint_number_t eint_number)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    hal_eint_status_t    ret_status;
    
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }

    ret_status = hal_eint_mask(eint_number);
    if (ret_status != HAL_EINT_STATUS_OK) {
    return ret_status;
    }
    
    hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_pin)];
    memset(callback_context, 0, sizeof(hal_eint_callback_context_t));
    is_eint_callback_registered[eint_number] = false;
    
    return HAL_EINT_STATUS_OK;
}


hal_eint_status_t hal_eint_register_callback(hal_eint_number_t eint_number,
        hal_eint_callback_t callback,
        void *user_data)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    hal_eint_status_t    ret_status;
    
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    hal_eint_callback_context_t *callback_context = &g_eint_callback_context[get_index_from_gpio_pin(gpio_pin)];
    callback_context->callback = callback;
    callback_context->user_data = user_data;
    callback_context->callback_present = true;
    
    is_eint_callback_registered[eint_number] = true;

    if (is_eint_masked[eint_number] == true) {
        ret_status = hal_eint_unmask(eint_number);
        if (ret_status != HAL_EINT_STATUS_OK) {
            return ret_status;
        }

    }
    
    return HAL_EINT_STATUS_OK;
    
}

hal_eint_status_t hal_eint_set_trigger_mode(hal_eint_number_t eint_number, hal_eint_trigger_mode_t trigger_mode)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    } else {
        ENUM_NVIC_SENSE_T trigger;
        ENUM_DEBOUNCE_POL_T pol;
        ENUM_DEBOUNCE_DUAL_T dual_en;
        
        switch (trigger_mode) {
            case  HAL_EINT_LEVEL_LOW : {
                trigger = eLevel_Trigger;
                pol = eDebounce_Positive_Pol;
                dual_en = eDebounce_Dual_No;
                break;
            }
            case  HAL_EINT_LEVEL_HIGH : {
                   trigger = eLevel_Trigger;
                pol = eDebounce_Negative_Pol;
                dual_en = eDebounce_Dual_No;
                break;
            }
            case  HAL_EINT_EDGE_FALLING : {
                   trigger = eEdge_Trigger;
                pol = eDebounce_Positive_Pol;
                dual_en = eDebounce_Dual_No; 
                break;
            }
            case  HAL_EINT_EDGE_RISING : {
                   trigger = eEdge_Trigger;
                pol = eDebounce_Negative_Pol;
                dual_en = eDebounce_Dual_No; 
                break;
            }
            case  HAL_EINT_EDGE_FALLING_AND_RISING : {
                   trigger = eLevel_Trigger;
                pol = eDebounce_Negative_Pol;
                dual_en = eDebounce_Dual_Yes;
                break;
            }
            default : {
                return HAL_EINT_STATUS_ERROR;
            }
        }
        eint_set_trigger_mode(gpio_pin, trigger, pol, dual_en);
        }

    return HAL_EINT_STATUS_OK;

}

hal_eint_status_t hal_eint_set_debounce_time(hal_eint_number_t eint_number, uint32_t time_ms)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    uint32_t sample_count;
    ENUM_DEBOUNCE_PRESCALER_T sample_rate;
    ENUM_DEBOUNCE_EN_T debounce_en;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }

    /*
          7687 EINT spec
           3 bit          |                      |      4bit counter(1 to 16)
           pre-scaler      clock(KHz)      |   min(ms)           max (ms)
              000                8                      0.13                  2.00
              001                4                      0.25                  4.00
              010                2                      0.50                  8.00
              011                1                      1.00                  16.00
              100                0.5                   2.00                  32.00
              101                0.25                 4.00                   64.00
              110                0.125                8.00                  128.00
              111                0.0625              16.00                 256.00
    */
    hal_eint_calc_debounce_parameters(time_ms, &sample_rate, &sample_count);

    if (time_ms != 0) {
        debounce_en = eDebounce_Enable;
    } else {
        debounce_en = eDebounce_Disable;
    }
    eint_set_debounce(gpio_pin, debounce_en, sample_rate, sample_count);
    return HAL_EINT_STATUS_OK;
}

#ifdef HAL_EINT_FEATURE_SW_TRIGGER_EINT
hal_eint_status_t hal_eint_set_software_trigger(hal_eint_number_t eint_number)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    eint_set_software_trigger(gpio_pin);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_clear_software_trigger(hal_eint_number_t eint_number)
{
    ENUM_IOT_GPIO_NAME_T gpio_pin;
    if (0 != hal_eint_convert_for_gpio(eint_number, &gpio_pin)) {
        log_hal_error("hal_eint_convert_for_gpio fail.");
        return HAL_EINT_STATUS_ERROR;
    }
    eint_clear_software_trigger(gpio_pin);
    return HAL_EINT_STATUS_OK;
}
#endif

#endif

