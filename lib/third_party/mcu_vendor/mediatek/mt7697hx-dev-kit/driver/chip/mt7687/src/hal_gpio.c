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

#include "hal_gpio.h"
#ifdef HAL_GPIO_MODULE_ENABLED

//#include "types.h"
#include "stddef.h"
#include "low_hal_gpio.h"
#include "hal_log.h"


static inline ENUM_DIR_T hal_gpio_translate_direction(hal_gpio_direction_t direction)
{
    return (ENUM_DIR_T)direction;
}


hal_pinmux_status_t hal_pinmux_set_function(hal_gpio_pin_t gpio_pin, uint8_t function_index)
{
    int32_t ret_value;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_PINMUX_STATUS_ERROR_PORT;
    }

    if (function_index >= 16) {
        return HAL_PINMUX_STATUS_INVALID_FUNCTION;
    }
    ret_value = pinmux_config((ENUM_IOT_GPIO_NAME_T)gpio_pin, function_index);
    return ret_value >= 0 ? HAL_PINMUX_STATUS_OK : HAL_PINMUX_STATUS_ERROR;
}

hal_gpio_status_t hal_gpio_init(hal_gpio_pin_t gpio_pin)
{
    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_deinit(hal_gpio_pin_t gpio_pin)
{
    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_get_input(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data)
{
    gpio_status status;
    if (NULL == gpio_data) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    status = gpio_get_status((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    *gpio_data = (hal_gpio_data_t)status.GPI_DIN;
    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_set_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t gpio_data)
{
    int32_t ret_value;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    ret_value = gpio_write((ENUM_IOT_GPIO_NAME_T)gpio_pin, (uint8_t)gpio_data);
    return ret_value >= 0 ? HAL_GPIO_STATUS_OK : HAL_GPIO_STATUS_ERROR;
}

hal_gpio_status_t hal_gpio_get_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data)
{
    gpio_status status;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_data) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    status = gpio_get_status((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    *gpio_data = (hal_gpio_data_t)status.GPO_DOUT;
    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_set_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t direction)
{
    int32_t ret_value;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }
    ret_value = gpio_direction((ENUM_IOT_GPIO_NAME_T)gpio_pin,
                               hal_gpio_translate_direction(direction));
    return ret_value >= 0 ? HAL_GPIO_STATUS_OK : HAL_GPIO_STATUS_ERROR;
}

hal_gpio_status_t hal_gpio_get_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t *direction)
{

    gpio_status status;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == direction) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    status = gpio_get_status((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    *direction = status.GPIO_OUTEN ? HAL_GPIO_DIRECTION_OUTPUT : HAL_GPIO_DIRECTION_INPUT;
    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_toggle_pin(hal_gpio_pin_t gpio_pin)
{
    gpio_status status;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    status = gpio_get_status((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    return hal_gpio_set_output(gpio_pin, (hal_gpio_data_t)(status.GPO_DOUT ^ 1));
}

hal_gpio_status_t hal_gpio_pull_up(hal_gpio_pin_t gpio_pin)
{
    int32_t ret_value;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    ret_value = gpio_PullDisable((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    if (ret_value < 0) {
        return HAL_GPIO_STATUS_ERROR;
    }
    ret_value = gpio_PullUp((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    return ret_value >= 0 ? HAL_GPIO_STATUS_OK : HAL_GPIO_STATUS_ERROR;
}

hal_gpio_status_t hal_gpio_pull_down(hal_gpio_pin_t gpio_pin)
{
    int32_t ret_value;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    ret_value = gpio_PullDisable((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    if (ret_value < 0) {
        return HAL_GPIO_STATUS_ERROR;
    }
    ret_value = gpio_PullDown((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    return ret_value >= 0 ? HAL_GPIO_STATUS_OK : HAL_GPIO_STATUS_ERROR;
}

hal_gpio_status_t hal_gpio_disable_pull(hal_gpio_pin_t gpio_pin)
{
    int32_t ret_value;
    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    ret_value = gpio_PullDisable((ENUM_IOT_GPIO_NAME_T)gpio_pin);
    return ret_value >= 0 ? HAL_GPIO_STATUS_OK : HAL_GPIO_STATUS_ERROR;
}
#endif

