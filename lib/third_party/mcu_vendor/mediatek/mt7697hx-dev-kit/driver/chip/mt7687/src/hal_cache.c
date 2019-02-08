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

#include "hal_cache.h"

#ifdef HAL_CACHE_MODULE_ENABLED

#include "hal_cache_internal.h"
#include "hal_log.h"
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CACHE_BUSY 1
#define CACHE_IDLE 0

bool volatile g_cache_status = CACHE_IDLE;

hal_cache_status_t hal_cache_init(void)
{
    hal_cache_region_t region;
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Check module status */
    if (g_cache_status == CACHE_BUSY) {
        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);

        return HAL_CACHE_STATUS_ERROR_BUSY;
    } else {
        /* Change status to busy */
        g_cache_status = CACHE_BUSY;

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);
    }

    /* Flush and invalidate all cache lines before use, whether CACHE is enabled or not */
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_FLUSH_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_INVALIDATE_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Set CACHE related registers to default value*/
    CACHE->CACHE_CON = 0;
    CACHE->CACHE_REGION_EN = 0;

    /* Update the global variable*/
    g_cache_con.w = 0;
    g_cache_region_en = 0;

    /* Set CACHE region registers to default value and update the global variable */
    for (region = HAL_CACHE_REGION_0; region < HAL_CACHE_REGION_MAX; region ++) {
        /* Set CACHE related registers to default value */
        CACHE->CACHE_ENTRY_N[region] = 0;
        CACHE->CACHE_END_ENTRY_N[region] = 0;

        /* Update the global variable */
        g_cache_entry[region].cache_entry_n.w = 0;
        g_cache_entry[region].cache_end_entry_n.w = 0;
    }

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_deinit(void)
{
    hal_cache_region_t region;

    /* flush and invalidate all cache lines */
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_FLUSH_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_INVALIDATE_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Set CACHE related registers to default value */
    CACHE->CACHE_CON = 0;
    CACHE->CACHE_REGION_EN = 0;

    /* Update the global variable*/
    g_cache_con.w = 0;
    g_cache_region_en = 0;

    /* Set CACHE region registers to default value and update the global variable */
    for (region = HAL_CACHE_REGION_0; region < HAL_CACHE_REGION_MAX; region ++) {
        /* Set CACHE related registers to default value */
        CACHE->CACHE_ENTRY_N[region] = 0;
        CACHE->CACHE_END_ENTRY_N[region] = 0;

        /* Update the global variable */
        g_cache_entry[region].cache_entry_n.w = 0;
        g_cache_entry[region].cache_end_entry_n.w = 0;
    }

    /* Change status to idle */
    g_cache_status = CACHE_IDLE;

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_enable(void)
{
    /* CACHE should not be enabled when the CACHE size is 0KB */
    if (HAL_CACHE_SIZE_0KB == g_cache_con.b.CACHESIZE) {
        return HAL_CACHE_STATUS_ERROR_CACHE_SIZE;
    }

    /* Enable CACHE */
    CACHE->CACHE_CON |= CACHE_CON_MCEN_MASK | CACHE_CON_CNTEN0_MASK | CACHE_CON_CNTEN1_MASK;

    /* Update the global variable */
    g_cache_con.w = CACHE->CACHE_CON;

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_disable(void)
{
    /* If CACHE is enabled, flush and invalidate all cache lines */
    if (CACHE->CACHE_CON & CACHE_CON_MCEN_MASK) {
        CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
        CACHE->CACHE_OP |= ((CACHE_FLUSH_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);
        CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
        CACHE->CACHE_OP |= ((CACHE_INVALIDATE_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);
    }

    /* Disable CACHE */
    CACHE->CACHE_CON &= ~CACHE_CON_MCEN_MASK;

    /* Update the global variable */
    g_cache_con.w = CACHE->CACHE_CON;

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_region_enable(hal_cache_region_t region)
{
    /* Region is invalid */
    if (region >= HAL_CACHE_REGION_MAX) {
        return HAL_CACHE_STATUS_ERROR_REGION;
    }

    /* The region should be configured before region is enabled */
    if (CACHE->CACHE_ENTRY_N[region] & CACHE_ENTRY_N_C_MASK) {
        CACHE->CACHE_REGION_EN |= (1 << region);
    } else {
        return HAL_CACHE_STATUS_ERROR;
    }

    /* Update the global variable */
    g_cache_region_en = CACHE->CACHE_REGION_EN;

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_region_disable(hal_cache_region_t region)
{
    /* Region is invalid */
    if (region >= HAL_CACHE_REGION_MAX) {
        return HAL_CACHE_STATUS_ERROR_REGION;
    }

    /* Disable the corresponding region */
    CACHE->CACHE_REGION_EN &= ~(1 << region);

    /* Update the global variable */
    g_cache_region_en = CACHE->CACHE_REGION_EN;

    /* The region setting information is cleard */
    g_cache_entry[region].cache_entry_n.w = 0;
    g_cache_entry[region].cache_end_entry_n.w = 0;

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_set_size(hal_cache_size_t cache_size)
{
    /* CACHE size is invalid */
    if (cache_size >= HAL_CACHE_SIZE_MAX) {
        return HAL_CACHE_STATUS_ERROR_CACHE_SIZE;
    }

    /* Set CACHE size */
    CACHE->CACHE_CON &= ~CACHE_CON_CACHESIZE_MASK;
    CACHE->CACHE_CON |= (cache_size << CACHE_CON_CACHESIZE_OFFSET);

    /* When CACHE size is 0KB, make sure the CACHE is disabled */
    if (cache_size == HAL_CACHE_SIZE_0KB) {
        CACHE->CACHE_CON = 0;
    }

    /* Update the global variable */
    g_cache_con.w = CACHE->CACHE_CON;

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_region_config(hal_cache_region_t region, const hal_cache_region_config_t *region_config)
{
    /* Region is invalid */
    if (region >= HAL_CACHE_REGION_MAX) {
        return HAL_CACHE_STATUS_ERROR_REGION;
    }

    /* Parameter check */
    if (region_config == NULL) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* The region address must be 4KB aligned */
    if (region_config->cache_region_address & (MTK_CACHE_REGION_SIZE_UNIT - 1)) {
        assert(0);
        return HAL_CACHE_STATUS_ERROR_REGION_ADDRESS;
    }

    /* The region size must be 4KB aligned */
    if (region_config->cache_region_size & (MTK_CACHE_REGION_SIZE_UNIT - 1)) {
        assert(0);
        return HAL_CACHE_STATUS_ERROR_REGION_SIZE;
    }

    /* Write the region setting to corresponding register */
    CACHE->CACHE_ENTRY_N[region] = region_config->cache_region_address;
    CACHE->CACHE_END_ENTRY_N[region] = region_config->cache_region_address + region_config->cache_region_size;

    /* Set this bit when region is configured, and this bit will be double checked in hal_cache_region_enable() function */
    CACHE->CACHE_ENTRY_N[region] |= CACHE_ENTRY_N_C_MASK;

    /* Update the global variable */
    g_cache_entry[region].cache_entry_n.w = CACHE->CACHE_ENTRY_N[region];
    g_cache_entry[region].cache_end_entry_n.w = CACHE->CACHE_END_ENTRY_N[region];

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_one_cache_line(uint32_t address)
{
    uint32_t irq_flag;

    /* Make sure address is cache line size aligned */
    if (address & (HAL_CACHE_LINE_SIZE - 1)) {
        assert(0);
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }
    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Invalidate CACHE line by address */
    CACHE->CACHE_OP = (address & CACHE_OP_TADDR_MASK);
    CACHE->CACHE_OP |= ((CACHE_INVALIDATE_ONE_LINE_BY_ADDRESS << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Restore the previous status of interrupt */
    restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_multiple_cache_lines(uint32_t address, uint32_t length)
{
    uint32_t irq_flag;
    uint32_t end_address = address + length;

    /* Make sure address and length are both cache line size aligned */
    if ((address & (HAL_CACHE_LINE_SIZE - 1)) || (length & (HAL_CACHE_LINE_SIZE - 1))) {
        assert(0);
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Invalidate CACHE lines by address and length */
    while (address < end_address) {
        hal_cache_invalidate_one_cache_line(address);
        address += HAL_CACHE_LINE_SIZE;
    }

    /* Restore the previous status of interrupt */
    restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_all_cache_lines(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Flush all CACHE lines before invalidate */
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_FLUSH_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Invalidate all CACHE lines */
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_INVALIDATE_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Restore the previous status of interrupt */
    restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_flush_one_cache_line(uint32_t address)
{
    uint32_t irq_flag;

    /* Make sure address is cache line size aligned */
    if (address & (HAL_CACHE_LINE_SIZE - 1)) {
        assert(0);
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* Interrupt is masked to make sure flush or invalidate operation can not be interrupted */
    irq_flag = save_and_set_interrupt_mask();

    /* Flush CACHE line by address */
    CACHE->CACHE_OP = (address & CACHE_OP_TADDR_MASK);
    CACHE->CACHE_OP |= ((CACHE_FLUSH_ONE_LINE_BY_ADDRESS << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Restore the previous status of interrupt */
    restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_flush_multiple_cache_lines(uint32_t address, uint32_t length)
{
    uint32_t irq_flag;
    uint32_t end_address = address + length;

    /* Make sure address and length are both cache line size aligned */
    if ((address & (HAL_CACHE_LINE_SIZE - 1)) || (length & (HAL_CACHE_LINE_SIZE - 1))) {
        assert(0);
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* Interrupt is masked to make sure flush or invalidate operation can not be interrupted */
    irq_flag = save_and_set_interrupt_mask();

    /* Flush CACHE lines by address and length */
    while (address < end_address) {
        hal_cache_flush_one_cache_line(address);
        address += HAL_CACHE_LINE_SIZE;
    }

    /* Restore the previous status of interrupt */
    restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_flush_all_cache_lines(void)
{
    uint32_t irq_flag;

    /* Interrupt is masked to make sure flush or invalidate operation can not be interrupted */
    irq_flag = save_and_set_interrupt_mask();

    /* Flush all CACHE lines */
    CACHE->CACHE_OP &= ~CACHE_OP_OP_MASK;
    CACHE->CACHE_OP |= ((CACHE_FLUSH_ALL_LINES << CACHE_OP_OP_OFFSET) | CACHE_OP_EN_MASK);

    /* Restore the previous status of interrupt */
    restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}


#ifdef __cplusplus
}
#endif

#endif /* HAL_CACHE_MODULE_ENABLED */

