/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 * PSoC 6 platform clock driver
 */
#include <stdint.h>
#include <string.h>

#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define DIV_INT_8_BIT_MAX     (0x1 << 8)
#define DIV_INT_16_BIT_MAX    (0x1 << 16)
#define DIV_INT_24_BIT_MAX    (0x1 << 24)
#define DIV_FRAC_5_BIT_MAX    (0x1 << 5)

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

platform_root_clock_t platform_root_clocks[] =
{
    {.hf_clock = 0, .clock_path = CY_SYSCLK_CLKHF_IN_CLKPATH0},    /* HF[0] Clock */
    {.hf_clock = 1, .clock_path = CY_SYSCLK_CLKHF_IN_CLKPATH1},    /* HF[1] Clock */
    {.hf_clock = 2, .clock_path = CY_SYSCLK_CLKHF_IN_CLKPATH0},    /* HF[2] Clock */
    {.hf_clock = 3, .clock_path = CY_SYSCLK_CLKHF_IN_CLKPATH0},    /* HF[3] Clock */
    {.hf_clock = 4, .clock_path = CY_SYSCLK_CLKHF_IN_CLKPATH0},    /* HF[4] Clock */
};

/******************************************************
 *               Function Definitions
 ******************************************************/

platform_result_t platform_root_clock_init( uint32_t hf_clock, cy_en_clkhf_dividers_t hf_div )
{
    uint32_t idx;
    platform_root_clock_t *root_clock = NULL;

    for ( idx = 0 ; idx < (sizeof(platform_root_clocks)/sizeof(platform_root_clocks[0])) ; idx++ )
    {
        if ( platform_root_clocks[idx].hf_clock == hf_clock )
        {
            root_clock = &platform_root_clocks[idx];
            break;
        }
    }

    if ( root_clock == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    Cy_SysClk_ClkHfSetSource( root_clock->hf_clock, root_clock->clock_path );
    Cy_SysClk_ClkHfSetDivider( root_clock->hf_clock, hf_div );
    Cy_SysClk_ClkHfEnable( root_clock->hf_clock );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_peripheral_clock_init( platform_peripheral_clock_t const* peripheral_clock, uint32_t div_int, uint32_t div_frac)
{
    if ( peripheral_clock == NULL )
    {
        return PLATFORM_BADARG;
    }

    if ( peripheral_clock->divider_type == CY_SYSCLK_DIV_8_BIT )
    {
        if ( div_int >= DIV_INT_8_BIT_MAX )
        {
            return PLATFORM_ERROR;
        }
    }
    else if ( peripheral_clock->divider_type == CY_SYSCLK_DIV_16_BIT )
    {
        if ( div_int >= DIV_INT_16_BIT_MAX )
        {
            return PLATFORM_ERROR;
        }
    }
    else if ( peripheral_clock->divider_type == CY_SYSCLK_DIV_16_5_BIT )
    {
        if ( (div_int >= DIV_INT_16_BIT_MAX) || (div_frac == 0) || (div_frac > DIV_FRAC_5_BIT_MAX) )
        {
            return PLATFORM_ERROR;
        }
    }
    else if ( peripheral_clock->divider_type == CY_SYSCLK_DIV_24_5_BIT )
    {
        if ( (div_int >= DIV_INT_24_BIT_MAX) || (div_frac == 0) || (div_frac > DIV_FRAC_5_BIT_MAX) )
        {
            return PLATFORM_ERROR;
        }
    }

    Cy_SysClk_PeriphAssignDivider( peripheral_clock->clock_dest, peripheral_clock->divider_type, peripheral_clock->divider_num );

    if ( (peripheral_clock->divider_type == CY_SYSCLK_DIV_8_BIT) || (peripheral_clock->divider_type == CY_SYSCLK_DIV_16_BIT) )
    {
        Cy_SysClk_PeriphSetDivider( peripheral_clock->divider_type, peripheral_clock->divider_num, div_int );
    }
    else if ( (peripheral_clock->divider_type == CY_SYSCLK_DIV_16_5_BIT) || (peripheral_clock->divider_type == CY_SYSCLK_DIV_24_5_BIT) )
    {
        Cy_SysClk_PeriphSetFracDivider( peripheral_clock->divider_type, peripheral_clock->divider_num, div_int, div_frac );
    }

    Cy_SysClk_PeriphEnableDivider( peripheral_clock->divider_type, peripheral_clock->divider_num );

    return PLATFORM_SUCCESS;
}
