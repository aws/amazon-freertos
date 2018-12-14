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

#include "hal_spi_slave.h"

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

#include "hal_spi_slave_internal.h"
#include "hal_nvic.h"

static hal_spi_slave_callback_t g_spis_callback = NULL;
static void *g_spis_user_data = NULL;

static void hal_spi_slave_isr(hal_nvic_irq_t irq_number)
{
    SPIS_CLEAR_IRQ();

    g_spis_callback(g_spis_user_data);
}

static bool is_slave_config(const hal_spi_slave_config_t *spi_configure)
{
    bool ret = true;

    ret &= (((spi_configure->polarity) == HAL_SPI_SLAVE_CLOCK_POLARITY0) || \
            ((spi_configure->polarity) == HAL_SPI_SLAVE_CLOCK_POLARITY1));

    ret &= (((spi_configure->phase) == HAL_SPI_SLAVE_CLOCK_PHASE0) || \
            ((spi_configure->phase) == HAL_SPI_SLAVE_CLOCK_PHASE1));

    return ret;
}


hal_spi_slave_status_t hal_spi_slave_init(hal_spi_slave_port_t spi_port, hal_spi_slave_config_t *spi_configure)
{
    uint32_t spis_mode = 0;
    if (spi_port >= HAL_SPI_SLAVE_MAX) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (!is_slave_config(spi_configure)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    spis_mode = ((spi_configure->polarity << 1) | (spi_configure->phase));
    SPIS_SET_MODE(((spis_mode) & (0x3)));

    SPIS_20M_CLOCK_ENABLE();

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t hal_spi_slave_register_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t callback, void *user_data)
{
    if (spi_port >= HAL_SPI_SLAVE_MAX) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (NULL == callback) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    /* user_data can be as NULL */
    g_spis_callback = callback;
    g_spis_user_data = user_data;

    /* register lisr to nvic */
    hal_nvic_register_isr_handler(CM4_SPI_SLAVE_IRQ, hal_spi_slave_isr);
    NVIC_SetPriority(CM4_SPI_SLAVE_IRQ, CM4_SPI_SLAVE_PRI);
    NVIC_EnableIRQ(CM4_SPI_SLAVE_IRQ);

    return HAL_SPI_SLAVE_STATUS_OK;

}


#endif /*HAL_SPI_SLAVE_MODULE_ENABLED*/


