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

#include "hal_spi_master.h"
#ifdef HAL_SPI_MASTER_MODULE_ENABLED

#include <string.h>
#include "hal_log.h"
#include "spim.h"
#include "low_hal_gpio.h"
#include "hal_spim.h"
#include "hal_sleep_manager.h"

extern uint32_t save_and_set_interrupt_mask(void);
extern void restore_interrupt_mask(uint32_t mask);

/* Only one port for 7687 */
static hal_spi_master_config_t g_spi_master_context;

#ifdef HAL_SLEEP_MANAGER_ENABLED
uint8_t spim_sleep_handler[HAL_SPI_MASTER_MAX] = {0};
static char *spim_lock_sleep_name[HAL_SPI_MASTER_MAX] = {"SPIM0"};
static uint32_t spim_mode_reg;
static uint32_t spim_ctrl_reg;
#endif

/* status for thread safe */
volatile static uint8_t g_spi_master_status;
hal_gpio_pin_t g_spi_master_cs_n;

static bool hal_spi_master_is_master_port_valid(hal_spi_master_port_t master_port)
{
    return master_port < HAL_SPI_MASTER_MAX;
}

static uint32_t hal_spi_master_build_settings(hal_spi_master_config_t *spi_config)
{
    uint32_t settings = 0;
    if (HAL_SPI_MASTER_SLAVE_0 == spi_config->slave_port) {
        settings |= SPI_MASTER_SLAVE_SEL_0;
    } else {
        settings |= SPI_MASTER_SLAVE_SEL_1;
    }
    if (HAL_SPI_MASTER_MSB_FIRST == spi_config->bit_order) {
        settings |= SPI_MASTER_MB_MSB_FIRST;
    } else {
        settings |= SPI_MASTER_MB_LSB_FIRST;
    }
    if (HAL_SPI_MASTER_CLOCK_POLARITY0 == spi_config->polarity) {
        settings |= SPI_MASTER_CPOL_0;
    } else {
        settings |= SPI_MASTER_CPOL_1;
    }
    if (HAL_SPI_MASTER_CLOCK_PHASE0 == spi_config->phase) {
        settings |= SPI_MASTER_CPHA_0;
    } else {
        settings |= SPI_MASTER_CPHA_1;
    }
    return settings;
}

static uint32_t hal_spi_master_calc_divisor(uint32_t frequency)
{
    /*
        formula: frequency = 120MHz/(n+2)
    */
    static const uint32_t hal_spi_source_clock = 120 * 1000 * 1000;
    return hal_spi_source_clock / frequency - 2;
}

static hal_gpio_pin_t hal_spi_master_map_cs_pin(hal_spi_master_slave_port_t slave_port)
{
    /* GPIO32 or GPIO7 for slave 0,  GPIO6 for slave 1 */
    if (HAL_SPI_MASTER_SLAVE_0 == slave_port) {
        if (SPI_USE_GPIO32_AS_CS) {
            g_spi_master_cs_n = HAL_GPIO_32;
            log_hal_info("[SPIM]: g_spi_master_cs_n: %d.\r\n", g_spi_master_cs_n);
        } else if (SPI_USE_GPIO7_AS_CS) {
            g_spi_master_cs_n = HAL_GPIO_7;
            log_hal_info("[SPIM]: g_spi_master_cs_n: %d.\r\n", g_spi_master_cs_n);
        } else {
            log_hal_error("[SPIM]: cannot decide the GPIO for CS_N.\r\n");
            g_spi_master_cs_n = HAL_GPIO_32;
        }
    } else {
        g_spi_master_cs_n = HAL_GPIO_6;
        log_hal_info("[SPIM]: g_spi_master_cs_n: %d.\r\n", g_spi_master_cs_n);
    }
    return g_spi_master_cs_n;
}

static uint32_t hal_spi_build_op_addr(const hal_spi_master_send_and_receive_config_t *config)
{
    uint32_t op_addr = 0;
    uint32_t index = 0;

    for (index = 0; index < config->send_length; index++) {
        op_addr |= (uint32_t)(config->send_data[index] << ((config->send_length - index - 1) * 8));
    }
    return op_addr;
}

hal_spi_master_status_t hal_spi_master_init(hal_spi_master_port_t master_port,
        hal_spi_master_config_t *spi_config)
{
    uint32_t settings;
    int32_t ret_val;
    hal_spi_master_status_t busy_status;
    if (!hal_spi_master_is_master_port_valid(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == spi_config) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (spi_config->clock_frequency > SPIM_MAX_FREQUENCY) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* initialize lock protect */
    spim_check_and_set_busy(busy_status);
    if (busy_status != HAL_SPI_MASTER_STATUS_OK) {
        return busy_status;
    }
    hal_spi_master_map_cs_pin(spi_config->slave_port);

    settings = hal_spi_master_build_settings(spi_config);
    ret_val = spim_init(settings,
                        hal_spi_master_calc_divisor(spi_config->clock_frequency));
    if (ret_val < 0) {
        return HAL_SPI_MASTER_STATUS_ERROR;
    }
    memset(&g_spi_master_context, 0, sizeof(hal_spi_master_config_t));
    memcpy(&g_spi_master_context, spi_config, sizeof(hal_spi_master_config_t));

#ifdef HAL_SLEEP_MANAGER_ENABLED
    spim_sleep_handler[master_port] = hal_sleep_manager_set_sleep_handle(spim_lock_sleep_name[master_port]);
#endif

    return HAL_SPI_MASTER_STATUS_OK;
}

hal_spi_master_status_t hal_spi_master_deinit(hal_spi_master_port_t master_port)
{
    if (!hal_spi_master_is_master_port_valid(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    memset(&g_spi_master_context, 0, sizeof(hal_spi_master_config_t));
    /* initialize lock protect */
    spim_set_idle();

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_release_sleep_handle(spim_sleep_handler[master_port]);
#endif
    return HAL_SPI_MASTER_STATUS_OK;
}


hal_spi_master_status_t hal_spi_master_send_polling(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    int32_t ret_val;
    if (!hal_spi_master_is_master_port_valid(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == data) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* hardware busy protect */
    if ((spi_reg_inl(SPI_REG_CTL) & SPI_CTL_BUSY) != 0) {
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }
    hal_gpio_set_output(g_spi_master_cs_n, HAL_GPIO_DATA_LOW);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
    ret_val = spim_write(data, size);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(spim_sleep_handler[master_port]);
#endif
    hal_gpio_set_output(g_spi_master_cs_n, HAL_GPIO_DATA_HIGH);
    return ret_val >= 0 ? HAL_SPI_MASTER_STATUS_OK : HAL_SPI_MASTER_STATUS_ERROR;
}

/* This is full-duplex API, make sure the receiving buffer is large enough to hold
   all the data received during sending period and receiving period */
hal_spi_master_status_t hal_spi_master_send_and_receive_polling(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    int32_t ret_val = 0;
    if (!hal_spi_master_is_master_port_valid(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (spi_send_and_receive_config->send_length > 4) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length) > (spi_send_and_receive_config->receive_length)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* hardware busy protect */
    if ((spi_reg_inl(SPI_REG_CTL) & SPI_CTL_BUSY) != 0) {
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }
    hal_gpio_set_output(g_spi_master_cs_n, HAL_GPIO_DATA_LOW);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
    ret_val = spim_read(hal_spi_build_op_addr(spi_send_and_receive_config),
                        spi_send_and_receive_config->send_length,
                        spi_send_and_receive_config->receive_buffer + spi_send_and_receive_config->send_length,
                        spi_send_and_receive_config->receive_length - spi_send_and_receive_config->send_length);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(spim_sleep_handler[master_port]);
#endif
    hal_gpio_set_output(g_spi_master_cs_n, HAL_GPIO_DATA_HIGH);

    return ret_val >= 0 ? HAL_SPI_MASTER_STATUS_OK : HAL_SPI_MASTER_STATUS_ERROR;
}

/* This is full-duplex API, make sure the receiving buffer is large enough to hold
   all the data received during sending period and receiving period */
hal_spi_master_status_t hal_spi_master_send_and_receive_polling_both_directional(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    int32_t ret_val = 0;
    if (!hal_spi_master_is_master_port_valid(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length == 0) || (spi_send_and_receive_config->receive_length == 0)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* hardware busy protect */
    if ((spi_reg_inl(SPI_REG_CTL) & SPI_CTL_BUSY) != 0) {
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }
    hal_gpio_set_output(g_spi_master_cs_n, HAL_GPIO_DATA_LOW);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
    ret_val = spim_write_read(spi_send_and_receive_config->send_data,
                              spi_send_and_receive_config->send_length,
                              spi_send_and_receive_config->receive_buffer,
                              spi_send_and_receive_config->receive_length);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(spim_sleep_handler[master_port]);
#endif
    hal_gpio_set_output(g_spi_master_cs_n, HAL_GPIO_DATA_HIGH);

    return ret_val >= 0 ? HAL_SPI_MASTER_STATUS_OK : HAL_SPI_MASTER_STATUS_ERROR;
}

hal_spi_master_status_t hal_spi_master_get_running_status(hal_spi_master_port_t master_port,
        hal_spi_master_running_status_t *running_status)
{
    if (!hal_spi_master_is_master_port_valid(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == running_status) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    if ((spi_reg_inl(SPI_REG_CTL) & SPI_CTL_BUSY) == 0) {
        *running_status = HAL_SPI_MASTER_IDLE;
    } else {
        *running_status = HAL_SPI_MASTER_BUSY;
    }

    return HAL_SPI_MASTER_STATUS_OK;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void spim_backup_register_callback(void)
{
    /* backup related spim register values */
    spim_mode_reg = DRV_Reg32(SPI_REG_MASTER);
    spim_ctrl_reg = DRV_Reg32(SPI_REG_CS_POLAR);
}

void spim_restore_register_callback(void)
{
    /* restore related spim register values */
    DRV_Reg32(SPI_REG_MASTER) = spim_mode_reg;
    DRV_Reg32(SPI_REG_CS_POLAR) = spim_ctrl_reg;
}
#endif

#endif /* HAL_SPI_MASTER_MODULE_ENABLED */

