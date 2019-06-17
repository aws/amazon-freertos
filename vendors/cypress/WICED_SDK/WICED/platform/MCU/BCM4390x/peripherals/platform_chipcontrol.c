/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

#include "platform_peripheral.h"
#include "platform_map.h"

#include "typedefs.h"
#include "wiced_osl.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static uint32_t
platform_chipcontrol(volatile uint32_t* addr_reg, volatile uint32_t* ctrl_reg,
                     uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    uint32_t ret;
    uint32_t val;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS(flags);

    *addr_reg = reg_offset;
    val = *ctrl_reg;
    ret = (val & ~clear_mask) | set_mask;
    if (val != ret)
    {
        *ctrl_reg = ret;
    }

    WICED_RESTORE_INTERRUPTS(flags);

    return ret;
}

static uint32_t
platform_chipstatus(volatile uint32_t* addr_reg, volatile uint32_t* status_reg,
                    uint8_t reg_offset)
{
    uint32_t ret;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS(flags);

    *addr_reg = reg_offset;
    ret = *status_reg;

    WICED_RESTORE_INTERRUPTS(flags);

    return ret;
}

uint32_t platform_gci_chipcontrol(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(GCI_INDIRECT_ADDR_REG, GCI_CHIPCONTROL_REG,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_gci_gpiocontrol(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(GCI_INDIRECT_ADDR_REG, GCI_GPIOCONTROL_REG,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_gci_gpiostatus(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(GCI_INDIRECT_ADDR_REG, GCI_GPIOSTATUS_REG,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_gci_gpiowakemask(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(GCI_INDIRECT_ADDR_REG, GCI_GPIOWAKEMASK_REG,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_gci_chipstatus(uint8_t reg_offset)
{
    return platform_chipstatus(GCI_INDIRECT_ADDR_REG, GCI_CHIPSTATUS_REG,
                               reg_offset);
}

uint32_t platform_pmu_chipcontrol(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(&PLATFORM_PMU->chipcontrol_addr, &PLATFORM_PMU->chipcontrol_data,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_pmu_res_updown_time(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(&PLATFORM_PMU->res_table_sel, &PLATFORM_PMU->res_updn_timer,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_pmu_res_dep_mask(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(&PLATFORM_PMU->res_table_sel, &PLATFORM_PMU->res_dep_mask,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_pmu_regulatorcontrol(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(&PLATFORM_PMU->regcontrol_addr, &PLATFORM_PMU->regcontrol_data,
                                reg_offset, clear_mask, set_mask);
}

uint32_t platform_pmu_pllcontrol(uint8_t reg_offset, uint32_t clear_mask, uint32_t set_mask)
{
    return platform_chipcontrol(&PLATFORM_PMU->pllcontrol_addr, &PLATFORM_PMU->pllcontrol_data,
                                reg_offset, clear_mask, set_mask);
}

#if defined(PLATFORM_4390X_OVERCLOCK)
static wiced_bool_t platform_pmu_pllcontrol_mdiv_get_init_params(uint8_t channel, uint8_t divider, uint8_t* reg_offset, uint32_t* mask, uint32_t* val, uint16_t* shift)
{
    switch (channel)
    {
        case 1:
            *reg_offset = PLL_CONTROL_M1DIV_REG;
            *mask       = PLL_CONTROL_M1DIV_MASK;
            *val        = PLL_CONTROL_M1DIV_VAL(divider);
            *shift      = PLL_CONTROL_M1DIV_SHIFT;
            return WICED_TRUE;

        case 2:
            *reg_offset = PLL_CONTROL_M2DIV_REG;
            *mask       = PLL_CONTROL_M2DIV_MASK;
            *val        = PLL_CONTROL_M2DIV_VAL(divider);
            *shift      = PLL_CONTROL_M2DIV_SHIFT;
            return WICED_TRUE;

        case 3:
            *reg_offset = PLL_CONTROL_M3DIV_REG;
            *mask       = PLL_CONTROL_M3DIV_MASK;
            *val        = PLL_CONTROL_M3DIV_VAL(divider);
            *shift      = PLL_CONTROL_M3DIV_SHIFT;
            return WICED_TRUE;

        case 4:
            *reg_offset = PLL_CONTROL_M4DIV_REG;
            *mask       = PLL_CONTROL_M4DIV_MASK;
            *val        = PLL_CONTROL_M4DIV_VAL(divider);
            *shift      = PLL_CONTROL_M4DIV_SHIFT;
            return WICED_TRUE;

        case 5:
            *reg_offset = PLL_CONTROL_M5DIV_REG;
            *mask       = PLL_CONTROL_M5DIV_MASK;
            *val        = PLL_CONTROL_M5DIV_VAL(divider);
            *shift      = PLL_CONTROL_M5DIV_SHIFT;
            return WICED_TRUE;

        case 6:
            *reg_offset = PLL_CONTROL_M6DIV_REG;
            *mask       = PLL_CONTROL_M6DIV_MASK;
            *val        = PLL_CONTROL_M6DIV_VAL(divider);
            *shift      = PLL_CONTROL_M6DIV_SHIFT;
            return WICED_TRUE;

        default:
            return WICED_FALSE;
    }
}

uint8_t platform_pmu_pllcontrol_mdiv_get(uint8_t channel)
{
    uint8_t  reg_offset;
    uint32_t mask;
    uint32_t val;
    uint16_t shift;

    if (!platform_pmu_pllcontrol_mdiv_get_init_params(channel, 0, &reg_offset, &mask, &val, &shift))
    {
        return 0;
    }

    return ((platform_pmu_pllcontrol(reg_offset, 0x0, 0x0) & mask) >> shift);
}

void platform_pmu_pllcontrol_mdiv_set(uint8_t channel, uint8_t divider)
{
    uint32_t flags;
    uint32_t val;
    uint8_t  reg_offset;
    uint32_t mask;
    uint16_t shift;
    uint32_t new_val;
    uint32_t load_enable;
    uint32_t min_res_mask;
    uint32_t max_res_mask;
    uint32_t pmucontrol;

    if (!platform_pmu_pllcontrol_mdiv_get_init_params(channel, divider, &reg_offset, &mask, &new_val, &shift))
    {
        return;
    }

    WICED_SAVE_INTERRUPTS(flags);

    PLATFORM_PMU->pllcontrol_addr = reg_offset;
    val                           = PLATFORM_PMU->pllcontrol_data;

    if (new_val != (val & mask))
    {
        /* Force dropping HT resources */
        min_res_mask = PLATFORM_PMU->min_res_mask;
        max_res_mask = PLATFORM_PMU->max_res_mask;
        PLATFORM_PMU->min_res_mask = min_res_mask & ~PMU_RES_HT_CLOCKS_MASK;
        PLATFORM_PMU->max_res_mask = max_res_mask & ~PMU_RES_HT_CLOCKS_MASK;

        /* Wait till backplane start to run on ALP clock */
        while (PLATFORM_CLOCKSTATUS_REG(PLATFORM_APPSCR4_REGBASE(0x0))->bits.ht_clock_available);
        OSL_DELAY(100);

        /* Enable divider loading into PLL */
        PLATFORM_PMU->pllcontrol_addr = PLL_CONTROL_LOAD_ENABLE_REG;
        load_enable                   = PLATFORM_PMU->pllcontrol_data & ~PLL_CONTROL_LOAD_ENABLE_MASK;
        PLATFORM_PMU->pllcontrol_data = load_enable | PLL_CONTROL_LOAD_ENABLE_VAL(channel);

        /* Set new divider */
        PLATFORM_PMU->pllcontrol_addr = reg_offset;
        PLATFORM_PMU->pllcontrol_data = new_val | (val & ~mask);

        /* Trigger PLL update */
        pmucontrol = (PLATFORM_PMU->pmucontrol & ~PMU_CONTROL_PLLCTL_UPDATE_MASK) | PMU_CONTROL_PLLCTL_UPDATE_EXEC;
        PLATFORM_PMU->pmucontrol = pmucontrol;

        /* Disable divider loading */
        PLATFORM_PMU->pllcontrol_addr = PLL_CONTROL_LOAD_ENABLE_REG;
        PLATFORM_PMU->pllcontrol_data = load_enable;

        /* Trigger PLL update */
        PLATFORM_PMU->pmucontrol = pmucontrol;

        /* Restore masks */
        PLATFORM_PMU->max_res_mask = max_res_mask;
        PLATFORM_PMU->min_res_mask = min_res_mask;
    }

    WICED_RESTORE_INTERRUPTS(flags);
}
#endif  /* PLATFORM_4390X_OVERCLOCK */

uint32_t platform_common_chipcontrol(volatile uint32_t* reg, uint32_t clear_mask, uint32_t set_mask)
{
    uint32_t ret;
    uint32_t val;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS(flags);

    val = *reg;
    ret = (val & ~clear_mask) | set_mask;
    if (val != ret)
    {
        *reg = ret;
    }

    WICED_RESTORE_INTERRUPTS(flags);

    return ret;
}
