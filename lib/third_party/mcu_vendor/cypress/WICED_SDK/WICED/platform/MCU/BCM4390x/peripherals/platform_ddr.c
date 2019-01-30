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
#include "platform_config.h"
#include "wiced_defaults.h"

#include "wwd_assert.h"

#include "typedefs.h"
#include "hndsoc.h"
#include "wiced_osl.h"

#include "tlsf.h"

#ifdef NO_MALLOC_H
#include <stdlib.h>
#else
#include <malloc.h>
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_DDR_SIZE_1_MBYTE    ( 1024 * 1024 )
#define PLATFORM_DDR_SIZE_1_GBYTE    ( 1024 * PLATFORM_DDR_SIZE_1_MBYTE )

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    uint16_t offset;
    uint16_t num;
    uint32_t regs[];
} ddr_seq_config_t;

typedef struct
{
   uint16_t offset;
   uint32_t reg;
} ddr_reg_config_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

const static ddr_seq_config_t ctl_0_nanya_nt5cb64m16dp_cf =
{
    .offset = 0x400,
    .num    = 134,
    .regs   =
    {
        0x00000600,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000004,
        0x00000002,
        0x00000040,
        0x000000a0,
        0x00000000,
        0x02000000,
        0x00050a00,
        0x0400060b,
        0x0b100404,
        0x040f0404,
        0x02040503,
        0x0c040406,
        0x0300579c,
        0x0c040403,
        0x03002247,
        0x05040003,
        0x01000202,
        0x03070901,
        0x00000204,
        0x00240100,
        0x000e09b4,
        0x000003c7,
        0x00030003,
        0x000a000a,
        0x00000000,
        0x00000000,
        0x00270200,
        0x000f0200,
        0x00010000,
        0x05050500,
        0x00000005,
        0x00000000,
        0x00000000,
        0x10020100,
        0x00100400,
        0x01000400,
        0x00000120,
        0x00000000,
        0x00000001,
        0x00000000,
        0x00000000,
        0x00021000,
        0x00000046,
        0x00030220,
        0x00000008,
        0x00000000,
        0x00010100,
        0x00000000,
        0x00000000,
        0x00020000,
        0x00400100,
        0x01000200,
        0x02000040,
        0x00000040,
        0x01030000,
        0x01ffff0a,
        0x01010101,
        0x03010101,
        0x0c000001,
        0x00000000,
        0x00010000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x04000000,
        0x00010605,
        0x00000000,
        0x00000002,
        0x00000000,
        0x00000000,
        0x0d000000,
        0x00000028,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00404000,
        0x40400000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00004040,
        0x00004040,
        0x01000101,
        0x00000001,
        0x02020200,
        0x00640002,
        0x01010101,
        0x00006401,
        0x00000000,
        0x0c0d0000,
        0x00000000,
        0x02001368,
        0x02000200,
        0x13680200,
        0x00006108,
        0x078e0505,
        0x02000200,
        0x02000200,
        0x0000078e,
        0x000025c6,
        0x02020605,
        0x000a0301,
        0x00000019,
        0x00000000,
        0x00000000,
        0x04038000,
        0x07030a07,
        0xffff1d1c,
        0x00190010,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000204,
        0x00000000,
        0x00000001
    }
};

const static ddr_seq_config_t phy_0_nanya_nt5cb64m16dp_cf =
{
    .offset = 0x880,
    .num    = 6,
    .regs   =
    {
        0x00000000,
        0x00005006,
        0x00105005,
        0x00000000,
        0x00000000,
        0x00000000
    }
};

const static ddr_seq_config_t phy_1_nanya_nt5cb64m16dp_cf =
{
    .offset = 0x800,
    .num    = 32,
    .regs   =
    {
        0x04130413,
        0x04150415,
        0x00010080,
        0x00000044,
        0x0a12002d,
        0x40404040,
        0x04130413,
        0x04150415,
        0x00010060,
        0x00000033,
        0x0912007f,
        0x40404040,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x04130413,
        0x04150415,
        0x00010080,
        0x00000044,
        0x0a12002d,
        0x40404040,
        0x04130413,
        0x04150415,
        0x00010060,
        0x00000033,
        0x0912007f,
        0x40404040,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000
    }
};

const static ddr_seq_config_t ctl_0_hynix_bp162_cl5_bl8_320 =
{
    .offset = 0x400,
    .num    = 134,
    .regs   =
    {
        0x00000600,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000004,
        0x00000002,
        0x00000040,
        0x000000a0,
        0x00000000,
        0x02000000,
        0x00050a00,
        0x0400060b,
        0x0b100404,
        0x04090504,
        0x02040503,
        0x0c040404,
        0x0300579c,
        0x0c040403,
        0x03002247,
        0x05050003,
        0x01000202,
        0x03070a01,
        0x00000205,
        0x00340100,
        0x001409b4,
        0x000003c7,
        0x00030003,
        0x000a000a,
        0x00000000,
        0x00000000,
        0x00370200,
        0x00160200,
        0x00010000,
        0x05050500,
        0x00000005,
        0x00000000,
        0x00000000,
        0x10020100,
        0x00100400,
        0x01000400,
        0x00000120,
        0x00000000,
        0x00000001,
        0x00000000,
        0x00000000,
        0x00021000,
        0x00000046,
        0x00030220,
        0x00000008,
        0x00000000,
        0x00010100,
        0x00000000,
        0x00000000,
        0x00020000,
        0x00400100,
        0x01000200,
        0x02000040,
        0x00000040,
        0x01000000,
        0x01ffff0a,
        0x01010101,
        0x03010101,
        0x0c000001,
        0x00000000,
        0x00010000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x04000000,
        0x00010605,
        0x00000000,
        0x00000002,
        0x00000000,
        0x00000000,
        0x0d000000,
        0x00000028,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00404000,
        0x40400000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00004040,
        0x00004040,
        0x01000101,
        0x00000001,
        0x02020200,
        0x00640002,
        0x01010101,
        0x00006401,
        0x00000000,
        0x0c0d0000,
        0x00000000,
        0x02001368,
        0x02000200,
        0x13680200,
        0x00006108,
        0x078e0505,
        0x02000200,
        0x02000200,
        0x0000078e,
        0x000025c6,
        0x02020605,
        0x000a0301,
        0x00000019,
        0x00000000,
        0x00000000,
        0x04038000,
        0x07030a07,
        0xffff1d1c,
        0x00190010,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000204,
        0x00000000,
        0x00000001
    }
};

const static ddr_seq_config_t phy_0_hynix_bp162_cl5_bl8_320 =
{
    .offset = 0x880,
    .num    = 6,
    .regs   =
    {
        0x00000000,
        0x00005006,
        0x00105005,
        0x00000000,
        0x00000000,
        0x00000000
    }
};

const static ddr_seq_config_t phy_1_hynix_bp162_cl5_bl8_320 =
{
    .offset = 0x800,
    .num    = 32,
    .regs   =
    {
        0x04130413,
        0x04150415,
        0x00010080,
        0x00000044,
        0x0a12002d,
        0x40404040,
        0x04130413,
        0x04150415,
        0x00010060,
        0x00000033,
        0x0912007f,
        0x40404040,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x04130413,
        0x04150415,
        0x00010080,
        0x00000044,
        0x0a12002d,
        0x40404040,
        0x04130413,
        0x04150415,
        0x00010060,
        0x00000033,
        0x0912007f,
        0x40404040,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000
    }
};

static tlsf_t ddr_heap;

/******************************************************
 *               Function Definitions
 ******************************************************/

static inline void
ddr_write_reg(uint16_t offset, uint32_t val)
{
    *(volatile uint32_t*)PLATFORM_DDR_CONTROLLER_REGBASE(offset) = val;
}

static inline uint32_t
ddr_read_reg(uint16_t offset)
{
    return *(volatile uint32_t*)PLATFORM_DDR_CONTROLLER_REGBASE(offset);
}

static inline void
ddr_mask_reg(uint16_t offset, uint32_t and_mask, uint32_t or_mask)
{
    ddr_write_reg(offset, (ddr_read_reg(offset) & and_mask) | or_mask);
}

static void
ddr_init_seq(const ddr_seq_config_t *cfg)
{
    uint16_t i;

    for (i = 0; i < cfg->num; ++i)
    {
        ddr_write_reg(cfg->offset + sizeof(cfg->regs[0]) * i, cfg->regs[i]);
    }
}

static void
ddr_setup_reference_clock(void)
{
    /*
     * Pads are calibrated for 320MHZ, so reference clock cannot exceeds this value.
     * For overclocking 480MHZ mode let's switch to 160MHZ reference clock.
     */
#if defined(PLATFORM_4390X_OVERCLOCK)
    /* Overclocking */
    platform_gci_chipcontrol(GCI_CHIPCONTROL_DDR_FREQ_REG, GCI_CHIPCONTROL_DDR_FREQ_MASK, GCI_CHIPCONTROL_DDR_FREQ_160);
#else
    /* Normal */
    platform_gci_chipcontrol(GCI_CHIPCONTROL_DDR_FREQ_REG, GCI_CHIPCONTROL_DDR_FREQ_MASK, GCI_CHIPCONTROL_DDR_FREQ_320_480);
#endif  /* PLATFORM_4390X_OVERCLOCK */
}

static void
ddr_setup_bss_sections(void)
{
    if (PLATFORM_DDR_BSS_SIZE != 0)
    {
        wiced_assert("BSS location is outside of valid range", (size_t) &link_ddr_bss_location >= PLATFORM_DDR_BASE(0x0));
        wiced_assert("BSS location is outside of valid range", (size_t) &link_ddr_bss_end <= PLATFORM_DDR_BASE(0x0) + platform_ddr_get_size());

        memset(&link_ddr_bss_location, 0, PLATFORM_DDR_BSS_SIZE);
    }
}

static void
ddr_setup_heap(void)
{
    if (PLATFORM_DDR_HEAP_SIZE != 0)
    {
        wiced_assert("Heap location is outside of valid range", (size_t) &link_ddr_heap_location >= PLATFORM_DDR_BASE(0x0));
        wiced_assert("Heap location is outside of valid range", (size_t) &link_ddr_heap_end <= PLATFORM_DDR_BASE(0x0) + platform_ddr_get_size());

        ddr_heap = tlsf_create_with_pool(&link_ddr_heap_location, PLATFORM_DDR_HEAP_SIZE);
    }
}

static void
ddr_post_init(uint32_t chip_size)
{
    /* Announce via DDR size assigning that it is ready */
    platform_ddr_size = MIN(PLATFORM_DDR_MAX_SIZE, chip_size);

    /* Ready to initialize BSS sections */
    ddr_setup_bss_sections();

    /* Ready to initialize heap */
    ddr_setup_heap();
}

static void
ddr_pre_init(void)
{
    if (!PLATFORM_DDR_SKIP_INIT)
    {
        /* Setup clock */
        ddr_setup_reference_clock();

        /* Enable DDR controller */
        osl_core_enable(DMEMC_CORE_ID);
    }
}

PLATFORM_DDR_FUNCDECL(nanya_nt5cb64m16dp_cf)
{
    ddr_pre_init();

    if (!PLATFORM_DDR_SKIP_INIT)
    {
        /* Turn the pads RX on and enable termination */
        ddr_write_reg(0x48, 0x0);
        ddr_write_reg(0x4c, 0x01010101);

        /* Program DDR core */
        ddr_init_seq(&ctl_0_nanya_nt5cb64m16dp_cf);
        ddr_init_seq(&phy_0_nanya_nt5cb64m16dp_cf);
        ddr_init_seq(&phy_1_nanya_nt5cb64m16dp_cf);
        ddr_write_reg(0x45c, 0x03000000);
        ddr_mask_reg(0x504, 0xffffffff, 0x10000);
        ddr_mask_reg(0x4fc, 0xfffeffff, 0x0);
        ddr_mask_reg(0x400, 0xffffffff, 0x1);
    }

    ddr_post_init(128 * PLATFORM_DDR_SIZE_1_MBYTE);

    return PLATFORM_SUCCESS;
}

PLATFORM_DDR_FUNCDECL(hynix_bp162_cl5_bl8_320)
{
    ddr_pre_init();

    if (!PLATFORM_DDR_SKIP_INIT)
    {
        /* Turn the pads RX on and enable termination */
        ddr_write_reg(0x48, 0x0);
        ddr_write_reg(0x4c, 0x01010101);

        /* Program DDR core */
        ddr_init_seq(&ctl_0_hynix_bp162_cl5_bl8_320);
        ddr_init_seq(&phy_0_hynix_bp162_cl5_bl8_320);
        ddr_init_seq(&phy_1_hynix_bp162_cl5_bl8_320);
        ddr_write_reg(0x45c, 0x03000000);
        ddr_mask_reg(0x504, 0xffffffff, 0x10000);
        ddr_mask_reg(0x4fc, 0xfffeffff, 0x0);
        ddr_mask_reg(0x400, 0xffffffff, 0x1);
    }

    ddr_post_init(128 * PLATFORM_DDR_SIZE_1_MBYTE);

    return PLATFORM_SUCCESS;
}

static inline wiced_bool_t platform_heap_is_ddr_address(void* ptr)
{
    wiced_bool_t result = WICED_FALSE;
    uint32_t addr = (uint32_t)ptr;

    if ((addr >= PLATFORM_DDR_BASE(0x0)) && (addr < PLATFORM_DDR_BASE(PLATFORM_DDR_MAX_SIZE)))
    {
        result = WICED_TRUE;
    }

    return result;
}

void* platform_heap_malloc_record_caller(platform_heap_type_t heap, size_t bytes, const char* filename, int line)
{
    void* res;

    UNUSED_PARAMETER(filename);
    UNUSED_PARAMETER(line);

    if ((PLATFORM_DDR_HEAP_SIZE != 0) && (heap == PLATFORM_HEAP_DDR))
    {
        uint32_t flags;

        WICED_SAVE_INTERRUPTS(flags);
        res = tlsf_malloc(ddr_heap, bytes);
        WICED_RESTORE_INTERRUPTS(flags);
    }
    else
    {
        res = malloc(bytes);
    }

    return res;
}

void* platform_heap_memalign_record_caller(platform_heap_type_t heap, size_t align, size_t bytes, const char* filename, int line)
{
    void* res;

    UNUSED_PARAMETER(filename);
    UNUSED_PARAMETER(line);

    if ((PLATFORM_DDR_HEAP_SIZE != 0) && (heap == PLATFORM_HEAP_DDR))
    {
        uint32_t flags;

        WICED_SAVE_INTERRUPTS(flags);
        res = tlsf_memalign(ddr_heap, align, bytes);
        WICED_RESTORE_INTERRUPTS(flags);
    }
    else
    {
        res = memalign(align, bytes);
    }

    return res;
}

void* platform_heap_realloc_record_caller(platform_heap_type_t heap, void* ptr, size_t size, const char* filename, int line)
{
    void* res;

    UNUSED_PARAMETER(filename);
    UNUSED_PARAMETER(line);

    if ((PLATFORM_DDR_HEAP_SIZE != 0) && (heap == PLATFORM_HEAP_DDR))
    {
        uint32_t flags;

        wiced_assert("Must be DDR address", platform_heap_is_ddr_address(ptr));

        WICED_SAVE_INTERRUPTS(flags);
        res = tlsf_realloc(ddr_heap, ptr, size);
        WICED_RESTORE_INTERRUPTS(flags);
    }
    else
    {
        wiced_assert("Must be not DDR address", !platform_heap_is_ddr_address(ptr));

        res = realloc(ptr, size);
    }

    return res;
}

void platform_heap_free_record_caller(platform_heap_type_t heap, void* ptr, const char* filename, int line)
{
    UNUSED_PARAMETER(filename);
    UNUSED_PARAMETER(line);

    if ((PLATFORM_DDR_HEAP_SIZE != 0) && (heap == PLATFORM_HEAP_DDR))
    {
        uint32_t flags;

        wiced_assert("Must be DDR address", platform_heap_is_ddr_address(ptr));

        WICED_SAVE_INTERRUPTS(flags);
        tlsf_free(ddr_heap, ptr);
        WICED_RESTORE_INTERRUPTS(flags);
    }
    else
    {
        wiced_assert("Must be not DDR address", !platform_heap_is_ddr_address(ptr));

        free(ptr);
    }
}
