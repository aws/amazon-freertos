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

/** @file
 * 'Standard' SDIO HOST CONTROLLER driver
 * $Id: bcmsdstd.c 467328 2014-04-03 01:23:40Z sudhirbs $
 */

#include <typedefs.h>

#include <bcmdevs.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <osl.h>
#include <siutils.h>
#include <sdio.h>    /* SDIO Device and Protocol Specs */
#include <sdioh.h>    /* Standard SDIO Host Controller Specification */
#include <bcmsdbus.h>    /* bcmsdh to/from specific controller APIs */
#include <sdiovar.h>    /* ioctl/iovars */
#include <bcmsdstd.h>
/* XXX Quick NDIS hack */
#include <hndsoc.h>

/* Main SDIO clock freq. */
#define SDIO_CLK_FREQ   (400)
#define SDIO_CLK_HI_FREQ (50000)

#ifdef NDIS
#define inline  __inline
#define    PCI_CFG_VID        0
#define    PCI_CFG_BAR0    0x10
#endif


#ifdef __IOPOS__
#include <ios.h>
#define SD_PAGE_BITS    11
#else
#define SD_PAGE_BITS    12
#endif
#define SD_PAGE     (1 << SD_PAGE_BITS)
#define SDSTD_MAX_TUNING_PHASE    5


/*
 * Upper GPIO 16 - 31 are available on J22
 *   J22.pin3 == gpio16, J22.pin5 == gpio17, etc.
 * Lower GPIO 0 - 15 are available on J15 (WL_GPIO)
 */
#define SDH_GPIO16        16
#define SDH_GPIO_ENABLE        0xffff

#include <bcmsdstd.h>
#include <sbsdio.h> /* SDIOH (host controller) core hardware definitions */
#include "platform_peripheral.h"

/* Globals */
uint sd_msglevel = SDH_ERROR_VAL;

#ifdef __IOPOS__
uint sd_hiok = FALSE;            /* 4318 on RVL uses low speed mode */
#else
uint sd_hiok = FALSE;            /* Use low speed mode by default */
#endif
uint sd_sdmode = SDIOH_MODE_SD4;    /* Use SD4 mode by default */
uint sd_f2_blocksize = 64;        /* Default blocksize */

#define SD_CARD    0
#define MMC_CARD   1
uint sd_card_type = SD_CARD;

#define sd3_trace(x)

/*    sd3ClkMode:     0-SDR12 [25MHz]
 *                1-SDR25 [50MHz]+SHS=1
 *                2-SDR50 [100MHz]+SSDR50=1
 *                3-SDR104 [208MHz]+SSDR104=1
 *                4-DDR50 [50MHz]+SDDR50=1
 */
#define SD3CLKMODE_0_SDR12    (0)
#define SD3CLKMODE_1_SDR25    (1)
#define SD3CLKMODE_2_SDR50    (2)
#define SD3CLKMODE_3_SDR104    (3)
#define SD3CLKMODE_4_DDR50    (4)
#define SD3CLKMODE_DISABLED    (-1)
#define SD3CLKMODE_AUTO        (99)

/* values for global_UHSI_Supp : Means host and card caps match. */
#define HOST_SDR_UNSUPP            (0)
#define HOST_SDR_12_25            (1)
#define HOST_SDR_50_104_DDR        (2)

/* depends-on/affects sd3_autoselect_uhsi_max.
 *    see sd3_autoselect_uhsi_max
 */
int sd_uhsimode = SD3CLKMODE_DISABLED;
uint    sd_tuning_period = 0;
uint sd_delay_value = 500000;
/*     Enables host to dongle glomming. Also increases the
 *  dma buffer size. This will increase the rx throughput
 *  as there will be lesser CMD53 transactions
 */
#ifdef BCMSDIOH_TXGLOM
uint sd_txglom;
#ifdef LINUX
module_param(sd_txglom, uint, 0);
#endif
#endif /* BCMSDIOH_TXGLOM */

char dhd_sdiod_uhsi_ds_override[2] = {' '};

#define MAX_DTS_INDEX        (3)
#define DRVSTRN_MAX_CHAR    ('D')
#define DRVSTRN_IGNORE_CHAR    (' ')

char DTS_vals[MAX_DTS_INDEX + 1] = {
    0x1, /* Driver Strength Type-A */
    0x0, /* Driver Strength Type-B */
    0x2, /* Driver Strength Type-C */
    0x3, /* Driver Strength Type-D */
    };

/* Power Override for BCM43342 */
#define CHIP_CONTROL_BASE            0x18000000
#define CHIP_CONTROL_ADDR            0x650
#define CHIP_CONTROL_DATA            0x654
#define CHIP_CONTROL_CHIPID_MASK     0xFFFF
#define CHIP_CONTROL_REG3            0x3
#define CC3_VDDIO_SELECT_OVERRIDE    (1 << 6)
#define CC3_VDDIO_OVERRIDE_1_8V      (1 << 7)

/* depends-on/affects sd_uhsimode.
    select MAX speed automatically based on caps of host and card.
    If this is 1, sd_uhsimode will be ignored. If the sd_uhsimode is set
    by the user specifically, this var becomes 0. default value: 0. [XXX:TBD: for future]
    */
uint32 sd3_autoselect_uhsi_max = 0;

#define MAX_TUNING_ITERS        (40)
/* (150+10)millisecs total time; so dividing it for per-loop */
#define PER_TRY_TUNING_DELAY_MS        (160/MAX_TUNING_ITERS)
#define CLKTUNING_MAX_BRR_RETRIES    (1000)    /* 1 ms: 1000 retries with 1 us delay per loop */

/* table analogous to preset value register.
*    This is bcos current HC doesn't have preset value reg support.
*    All has DrvStr as 'B' [val:0] and CLKGEN as 0.
*/
static unsigned short presetval_sw_table[] = {
    0x0520, /* initialization:     DrvStr:'B' [0]; CLKGen:0;
            * SDCLKFreqSel: 520 [division: 320*2 = 640: ~400 KHz]
            */
    0x0008, /* default speed:DrvStr:'B' [0]; CLKGen:0;
            * SDCLKFreqSel: 8 [division: 6*2 = 12: ~25 MHz]
            */
    0x0004, /* High speed:     DrvStr:'B' [0]; CLKGen:0;
            * SDCLKFreqSel: 4 [division: 3*2 = 6: ~50 MHz]
            */
    0x0008, /* SDR12:         DrvStr:'B' [0]; CLKGen:0;
            * SDCLKFreqSel: 8 [division: 6*2 = 12: ~25 MHz]
            */
    0x0004, /* SDR25:         DrvStr:'B' [0]; CLKGen:0;
            * SDCLKFreqSel: 4 [division: 3*2 = 6: ~50 MHz]
            */
    0x0001, /* SDR50:         DrvStr:'B' [0]; CLKGen:0;
            * SDCLKFreqSel: 2 [division: 1*2 = 2: ~100 MHz]
            */
    0x0001, /* SDR104:         DrvStr:'B' [0]; CLKGen:0;
            SDCLKFreqSel: 1 [no division: ~255/~208 MHz]
            */
    0x0002  /* DDR50:         DrvStr:'B' [0]; CLKGen:0;
            SDCLKFreqSel: 4 [division: 3*2 = 6: ~50 MHz]
            */
};

/* This is to have software overrides to the hardware. Info follows:
    For override [1]:     Preset registers: not supported
    Voltage switch:  not supported
    Clock Tuning: not supported
*/
bool sd3_sw_override1 = FALSE;
bool sd3_sw_read_magic_bytes = FALSE;


#define SD3_TUNING_REQD(sd, sd_uhsimode) ((sd_uhsimode != SD3CLKMODE_DISABLED) && \
            (sd->version == HOST_CONTR_VER_3) &&    \
            ((sd_uhsimode == SD3CLKMODE_3_SDR104) ||    \
            ((sd_uhsimode == SD3CLKMODE_2_SDR50) &&    \
            (GFIELD(sd->caps3, CAP3_TUNING_SDR50)))))

/* find next power of 2 */
#define NEXT_POW2(n)  {n--; n |= n>>1; n |= n>>2; n |= n>>4; n++;}

#ifdef BCMSDYIELD
bool sd_yieldcpu = TRUE;        /* Allow CPU yielding for buffer requests */
uint sd_minyield = 0;            /* Minimum xfer size to allow CPU yield */
bool sd_forcerb = FALSE;        /* Force sync readback in intrs_on/off */
#endif

/* XXX: Issues with CMD14 enter/exit sleep
 * XXX: Temp fix for special CMD14 handling
 */
#define F1_SLEEPCSR_ADDR    0x1001F

uint sd_divisor = 7;    /* Default 160MHz/7 = 23MHz */

uint sd_power = 1;        /* Default to SD Slot powered ON */
uint sd_3_power_save = 1;        /* Default to SDIO 3.0 power save */
uint sd_clock = 1;        /* Default to SD Clock turned ON */
uint sd_pci_slot = 0xFFFFffff; /* Used to force selection of a particular PCI slot */
uint8 sd_dma_mode = DMA_MODE_AUTO; /* Default to AUTO & program based on capability */

/* XXX Base timeout counter value on 48MHz (2^20 @ 48MHz => 21845us)
 * Could adjust by adding sd_divisor (to maintain bit count) but really
 * need something more elaborate to do that right.  Still allows xfer
 * of about 1000 bytes at 400KHz, so constant is ok.
 * Timeout control N produces 2^(13+N) counter.
 */
uint sd_toctl = 7;
static bool trap_errs = FALSE;

static const char *dma_mode_description[] = { "PIO", "SDMA", "ADMA1", "32b ADMA2", "64b ADMA2" };

/* for time calc in ms */
static uint32_t now, prev;

/* Prototypes */
int sdstd_client_init(sdioh_info_t *sd);
int sdstd_sdmmc_init(sdioh_info_t *sd);
static int sdstd_sd_init(sdioh_info_t *sd);
static int sdstd_mmc_init(sdioh_info_t *sd);

static bool sdstd_start_clock(sdioh_info_t *sd, uint16 divisor);
static uint16 sdstd_start_power(sdioh_info_t *sd, int volts_req);
static bool sdstd_bus_width(sdioh_info_t *sd, int width);
static bool sdstd_sdmmc_bus_width(sdioh_info_t *sd, int width);
static bool sdstd_mmc_bus_width(sdioh_info_t *sd, int width);
static int sdstd_set_highspeed_mode(sdioh_info_t *sd, bool HSMode);
static int sdstd_set_dma_mode(sdioh_info_t *sd, int8 dma_mode);
static int sdstd_card_enablefuncs(sdioh_info_t *sd);
static void sdstd_cmd_getrsp(sdioh_info_t *sd, uint32 *rsp_buffer, int count);
static int sdstd_cmd_issue(sdioh_info_t *sd, bool use_dma, uint32 cmd, uint32 arg);
static int sdstd_card_regread(sdioh_info_t *sd, int func, uint32 regaddr,
                              int regsize, uint32 *data);
static int sdstd_card_regwrite(sdioh_info_t *sd, int func, uint32 regaddr,
                               int regsize, uint32 data);
static int sdstd_driver_init(sdioh_info_t *sd);
static bool sdstd_reset(sdioh_info_t *sd, bool host_reset, bool client_reset);
static int sdstd_card_buf(sdioh_info_t *sd, int rw, int func, bool fifo,
                          uint32 addr, int nbytes, uint32 *data);
static int sdstd_abort(sdioh_info_t *sd, uint func);
static int sdstd_check_errs(sdioh_info_t *sdioh_info, uint32 cmd, uint32 arg);
static int set_client_block_size(sdioh_info_t *sd, int func, int blocksize);
static int set_sdmmc_block_size(sdioh_info_t *sd, int func, int blocksize);
static void sd_map_dma(sdioh_info_t * sd);
static void sd_unmap_dma(sdioh_info_t * sd);
static void sd_clear_adma_dscr_buf(sdioh_info_t *sd);
static void sd_fill_dma_data_buf(sdioh_info_t *sd, uint8 data);
static void sd_create_adma_descriptor(sdioh_info_t *sd,
                                      uint32 index, uint32 addr_phys,
                                      uint16 length, uint16 flags);
static void sd_dump_adma_dscr(sdioh_info_t *sd);
static void sdstd_dumpregs(sdioh_info_t *sd);

static int sdstd_3_set_highspeed_uhsi_mode(sdioh_info_t *sd, int sd3ClkMode);
static int sdstd_3_sigvoltswitch_proc(sdioh_info_t *sd);
static int sdstd_3_get_matching_uhsi_clkmode(sdioh_info_t *sd,
    int sd3_requested_clkmode);
static bool sdstd_3_get_matching_drvstrn(sdioh_info_t *sd,
    int sd3_requested_clkmode, uint32 *drvstrn, uint16 *presetval);
static int sdstd_3_clock_wrapper(sdioh_info_t *sd);
static int sdstd_3_power_override(sdioh_info_t *sd);

static int sdstd_clock_wrapper(sdioh_info_t *sd);

#ifdef BCMINTERNAL
#ifdef NOTUSED
static int parse_caps(uint32 caps_reg, char *buf, int len);
static int parse_state(uint32 state_reg, char *buf, int len);
static void cis_fetch(sdioh_info_t *sd, int func, char *data, int len);
#endif /* NOTUSED */
#endif /* BCMINTERNAL */

#ifdef BCMDBG
static void print_regs(sdioh_info_t *sd);
#endif

/*
 * Private register access routines.
 */
#ifdef __IOPOS__
/* Same as regular routines, except no printfs. Used so
 * sd_data()/sd_ctrl() don't produce extraneous data.
 */
static uint32
priv_sdstd_rreg(sdioh_info_t *sd, uint reg)
{
    volatile uint32 data = *(volatile uint32 *)(sd->mem_space + reg);
    return data;
}
static inline void
priv_sdstd_wreg(sdioh_info_t *sd, uint reg, uint32 data)
{
    *(volatile uint32 *)(sd->mem_space + reg) = (volatile uint32)data;
}
#endif /* __IOPOS__ */

/* 16 bit PCI regs */

/* XXX This is a hack to satisfy the -Wmissing-prototypes warning */
extern uint16 sdstd_rreg16(sdioh_info_t *sd, uint reg);
uint16
sdstd_rreg16(sdioh_info_t *sd, uint reg)
{

#ifdef __IOPOS__
    uint16 data;
    int shift = 0;
    uint32 mask;
    uint32 tmp = priv_sdstd_rreg(sd, reg & ~0x3);

    shift = reg & 0x2 ? 16 : 0;
    mask = 0xffff << shift;
    data = (tmp & mask) >> shift;
#else
    volatile uint16 data = *(volatile uint16 *)(sd->mem_space + reg);
#endif
    sd_ctrl(("16: R Reg 0x%02x, Data 0x%x\n", reg, data));
    return data;
}

/* XXX This is a hack to satisfy the -Wmissing-prototypes warning */
extern void sdstd_wreg16(sdioh_info_t *sd, uint reg, uint16 data);
void
sdstd_wreg16(sdioh_info_t *sd, uint reg, uint16 data)
{
#ifdef __IOPOS__
    int shift = 0;
    uint32 mask;
    uint32 tmp = priv_sdstd_rreg(sd, reg & ~0x3);

    shift = reg & 0x2 ? 16 : 0;
    mask = 0xffff << shift;
    tmp = (tmp & ~mask) | (data << shift);
    priv_sdstd_wreg(sd, reg & ~0x3, tmp);
#else
    *(volatile uint16 *)(sd->mem_space + reg) = (uint16) data;
#endif
    sd_ctrl(("16: W Reg 0x%02x, Data 0x%x\n", reg, data));
}

#ifndef __IOPOS__    /* XXX Just leave out until properly tested for IOP */
static void
sdstd_or_reg16(sdioh_info_t *sd, uint reg, uint16 val)
{
#ifdef __IOPOS__    /* XXX This has not been tested, and could be optimized. */
    volatile uint16 data = sdstd_rreg16(sd, reg);
    data |= val;
    sdstd_wreg16(sd, reg, data);
#endif /* __IOPOS__ */
    volatile uint16 data = *(volatile uint16 *)(sd->mem_space + reg);
    sd_ctrl(("16: OR Reg 0x%02x, Val 0x%x\n", reg, val));
    data |= val;
    *(volatile uint16 *)(sd->mem_space + reg) = (uint16)data;

}
static void
sdstd_mod_reg16(sdioh_info_t *sd, uint reg, int16 mask, uint16 val)
{

#ifdef __IOPOS__    /* XXX This has not been tested, and could be optimized. */
    volatile uint16 data = sdstd_rreg16(sd, reg);
    data &= ~mask;
    data |= (val & mask);
    sdstd_wreg16(sd, reg, data);
#endif /* __IOPOS__ */
    volatile uint16 data = *(volatile uint16 *)(sd->mem_space + reg);
    sd_ctrl(("16: MOD Reg 0x%02x, Mask 0x%x, Val 0x%x\n", reg, mask, val));
    data &= ~mask;
    data |= (val & mask);
    *(volatile uint16 *)(sd->mem_space + reg) = (uint16)data;
}
#endif /* __IOPOS__ - leave mod_reg and or_reg out until it can be tested */


/* 32 bit PCI regs */
static uint32
sdstd_rreg(sdioh_info_t *sd, uint reg)
{
    volatile uint32 data = *(volatile uint32 *)(sd->mem_space + reg);
    sd_ctrl(("32: R Reg 0x%02x, Data 0x%x\n", reg, data));
    return data;
}
static inline void
sdstd_wreg(sdioh_info_t *sd, uint reg, uint32 data)
{
    *(volatile uint32 *)(sd->mem_space + reg) = (uint32)data;
    sd_ctrl(("32: W Reg 0x%02x, Data 0x%x\n", reg, data));

}
#ifdef BCMINTERNAL
#ifdef NOTUSED
static void
sdstd_or_reg(sdioh_info_t *sd, uint reg, uint32 val)
{
    volatile uint32 data = *(volatile uint32 *)(sd->mem_space + reg);
    data |= val;
    *(volatile uint32 *)(sd->mem_space + reg) = (volatile uint32)data;
}
static void
sdstd_mod_reg(sdioh_info_t *sd, uint reg, uint32 mask, uint32 val)
{
    volatile uint32 data = *(volatile uint32 *)(sd->mem_space + reg);
    data &= ~mask;
    data |= (val & mask);
    *(volatile uint32 *)(sd->mem_space + reg) = (volatile uint32)data;
}
#endif /* NOTUSED */
#endif /* BCMINTERNAL */

/* 8 bit PCI regs */
static inline void
sdstd_wreg8(sdioh_info_t *sd, uint reg, uint8 data)
{
#ifdef __IOPOS__
    int shift = 0;
    uint32 mask;
    uint32 tmp = priv_sdstd_rreg(sd, reg & ~0x3);

    shift = (reg & 0x3) * 8;
    mask = 0xff << shift;
    tmp = (tmp & ~mask) | (data << shift);

    priv_sdstd_wreg(sd, reg & ~0x3, tmp);
#else
    *(volatile uint8 *)(sd->mem_space + reg) = (uint8)data;
#endif
    sd_ctrl(("08: W Reg 0x%02x, Data 0x%x\n", reg, data));
}
static uint8
sdstd_rreg8(sdioh_info_t *sd, uint reg)
{
#ifdef __IOPOS__
    uint8 data;
    int shift = 0;
    uint32 mask;
    uint32 tmp = priv_sdstd_rreg(sd, reg & ~0x3);
    shift = (reg & 0x3) * 8;
    mask = 0xff << shift;
    data = (tmp & mask) >> shift;
#else
    volatile uint8 data = *(volatile uint8 *)(sd->mem_space + reg);
#endif
    sd_ctrl(("08: R Reg 0x%02x, Data 0x%x\n", reg, data));
    return data;
}

/*
 * Private work routines
 */

sdioh_info_t *glob_sd = NULL;

/*
 *  Public entry points & extern's
 */
extern sdioh_info_t *
sdioh_attach(osl_t *osh, void *intr_handler, uint irq)
{
    sdioh_info_t *sd;

    sd_trace(("%s\n", __FUNCTION__));
    if ((sd = (sdioh_info_t *)MALLOC(osh, sizeof(sdioh_info_t))) == NULL) {
        sd_err(("sdioh_attach: out of memory, malloced %d bytes\n", MALLOCED(osh)));
        return NULL;
    }
    bzero((char *)sd, sizeof(sdioh_info_t));
    glob_sd = sd;
    sd->osh = osh;
    if (sdstd_osinit(sd) != 0) {
        sd_err(("%s:sdstd_osinit() failed\n", __FUNCTION__));
        MFREE(sd->osh, sd, sizeof(sdioh_info_t));
        return NULL;
    }

    /* Make sure SDIOH core is enabled */
    osl_core_enable(SDIOH_CORE_ID);

    sd->mem_space = (volatile char *)PLATFORM_SDIOH_REGBASE(0x0);
    sd_init_dma(sd);
    sd->irq = irq;
    if (sd->mem_space == NULL) {
        sd_err(("%s:ioremap() failed\n", __FUNCTION__));
        sdstd_osfree(sd);
        MFREE(sd->osh, sd, sizeof(sdioh_info_t));
        return NULL;
    }
    sd_info(("%s:sd->mem_space = %p\n", __FUNCTION__, sd->mem_space));

    if(intr_handler != NULL) {
        sdioh_interrupt_register(sd, intr_handler, NULL);
    } else {
        sd->intr_handler = NULL;
        sd->intr_handler_arg = NULL;
        sd->intr_handler_valid = FALSE;
    }

    /* Set defaults */
    sd->sd_blockmode = TRUE;
    sd->sd_dma_mode = sd_dma_mode;
    sd->card_rca = 0;
#ifdef WICED_SDMMC_SUPPORT
    sd->use_client_ints = FALSE;
    sd->client_init = sdstd_sdmmc_init;
#else /* not WICED_SDMMC_SUPPORT */
    sd->use_client_ints = TRUE;
    sd->client_init = sdstd_client_init;
#endif /* not WICED_SDMMC_SUPPORT */

    /* XXX Haven't figured out how to make bytemode work with dma */
    if (!sd->sd_blockmode)
        sd->sd_dma_mode = DMA_MODE_NONE;

    if (sdstd_driver_init(sd) != SUCCESS) {
        /* If host CPU was reset without resetting SD bus or
           SD device, the device will still have its RCA but
           driver no longer knows what it is (since driver has been restarted).
           go through once to clear the RCA and a gain reassign it.
         */
        sd_info(("driver_init failed - Reset RCA and try again\n"));
        if (sdstd_driver_init(sd) != SUCCESS) {
            sd_err(("%s:driver_init() failed()\n", __FUNCTION__));
            if (sd->mem_space) {
                sdstd_reg_unmap(osh, (ulong)sd->mem_space, SDIOH_REG_WINSZ);
                sd->mem_space = NULL;
            }
            sdstd_osfree(sd);
            MFREE(sd->osh, sd, sizeof(sdioh_info_t));
            return (NULL);
        }
    }

    /* XXX Needed for NDIS as its OSL checks for correct dma address width
     * This value is normally set by wlc_attach() which has yet to run
     */
    OSL_DMADDRWIDTH(osh, 32);

    /* Always map DMA buffers, so we can switch between DMA modes. */
    sd_map_dma(sd);

    if (sdstd_register_irq(sd, irq) != SUCCESS) {
        sd_err(("%s: sdstd_register_irq() failed for irq = %d\n", __FUNCTION__, irq));
        sdstd_free_irq(sd->irq, sd);
        if (sd->mem_space) {
            sdstd_reg_unmap(osh, (ulong)sd->mem_space, SDIOH_REG_WINSZ);
            sd->mem_space = NULL;
        }

        sdstd_osfree(sd);
        MFREE(sd->osh, sd, sizeof(sdioh_info_t));
        return (NULL);
    }

    sd_trace(("%s: Done\n", __FUNCTION__));
    return sd;
}

extern SDIOH_API_RC
sdioh_detach(osl_t *osh, sdioh_info_t *sd)
{
    sd_trace(("%s\n", __FUNCTION__));
    if (sd) {
        sd_unmap_dma(sd);
        sdstd_wreg16(sd, SD_IntrSignalEnable, 0);
        if (sd->sd3_tuning_reqd == TRUE) {
            sdstd_3_osclean_tuning(sd);
            sd->sd3_tuning_reqd = FALSE;
        }
        sd_trace(("%s: freeing irq %d\n", __FUNCTION__, sd->irq));
        sdstd_free_irq(sd->irq, sd);
        if (sd->card_init_done)
            sdstd_reset(sd, 1, 1);
        if (sd->mem_space) {
            sdstd_reg_unmap(osh, (ulong)sd->mem_space, SDIOH_REG_WINSZ);
            sd->mem_space = NULL;
        }

        sdstd_osfree(sd);
        MFREE(sd->osh, sd, sizeof(sdioh_info_t));
        glob_sd = NULL;

        /* Reset to low speed mode by default */
        sd_hiok = FALSE;
    }
    return SDIOH_API_RC_SUCCESS;
}

/* Configure callback to client when we receive client interrupt */
extern SDIOH_API_RC
sdioh_interrupt_register(sdioh_info_t *sd, sdioh_cb_fn_t fn, void *argh)
{
    sd_trace(("%s: Entering\n", __FUNCTION__));
    sd->intr_handler = fn;
    sd->intr_handler_arg = argh;
    sd->intr_handler_valid = TRUE;
    return SDIOH_API_RC_SUCCESS;
}

extern SDIOH_API_RC
sdioh_interrupt_deregister(sdioh_info_t *sd)
{
    sd_trace(("%s: Entering\n", __FUNCTION__));
    sd->intr_handler_valid = FALSE;
    sd->intr_handler = NULL;
    sd->intr_handler_arg = NULL;
    return SDIOH_API_RC_SUCCESS;
}

extern SDIOH_API_RC
sdioh_interrupt_query(sdioh_info_t *sd, bool *onoff)
{
    sd_trace(("%s: Entering\n", __FUNCTION__));
    *onoff = sd->client_intr_enabled;
    return SDIOH_API_RC_SUCCESS;
}

#if defined(DHD_DEBUG) || defined(BCMDBG)
extern bool
sdioh_interrupt_pending(sdioh_info_t *sd)
{
    uint16 intrstatus;
    intrstatus = sdstd_rreg16(sd, SD_IntrStatus);
    return !!(intrstatus & CLIENT_INTR);
}
#endif

uint
sdioh_query_iofnum(sdioh_info_t *sd)
{
    return sd->num_funcs;
}

/* IOVar table */
enum {
    IOV_MSGLEVEL = 1,
    IOV_BLOCKMODE,
    IOV_BLOCKSIZE,
    IOV_DMA,
    IOV_USEINTS,
    IOV_NUMINTS,
    IOV_NUMLOCALINTS,
    IOV_HOSTREG,
    IOV_DEVREG,
    IOV_DIVISOR,
    IOV_SDMODE,
    IOV_HISPEED,
    IOV_HCIREGS,
    IOV_POWER,
    IOV_POWER_SAVE,
    IOV_YIELDCPU,
    IOV_MINYIELD,
    IOV_FORCERB,
    IOV_CLOCK,
    IOV_UHSIMOD,
    IOV_TUNEMOD
};

const bcm_iovar_t sdioh_iovars[] = {
    {"sd_msglevel",    IOV_MSGLEVEL,     0,    IOVT_UINT32,    0 },
    {"sd_blockmode", IOV_BLOCKMODE,    0,    IOVT_BOOL,    0 },
    {"sd_blocksize", IOV_BLOCKSIZE, 0,    IOVT_UINT32,    0 }, /* ((fn << 16) | size) */
    {"sd_dma",    IOV_DMA,    0,    IOVT_UINT32,    0 },
#ifdef BCMSDYIELD
    {"sd_yieldcpu",    IOV_YIELDCPU,    0,    IOVT_BOOL,    0 },
    {"sd_minyield",    IOV_MINYIELD,    0,    IOVT_UINT32,    0 },
    {"sd_forcerb",    IOV_FORCERB,    0,    IOVT_BOOL,    0 },
#endif
    {"sd_ints",    IOV_USEINTS,    0,    IOVT_BOOL,    0 },
    {"sd_numints",    IOV_NUMINTS,    0,    IOVT_UINT32,    0 },
    {"sd_numlocalints", IOV_NUMLOCALINTS, 0, IOVT_UINT32,    0 },
    {"sd_hostreg",    IOV_HOSTREG,    0,    IOVT_BUFFER,    sizeof(sdreg_t) },
    {"sd_devreg",    IOV_DEVREG,    0,    IOVT_BUFFER,    sizeof(sdreg_t)    },
    {"sd_divisor",    IOV_DIVISOR,    0,    IOVT_UINT32,    0 },
    {"sd_power",    IOV_POWER,    0,    IOVT_UINT32,    0 },
    {"sd_power_save",    IOV_POWER_SAVE,    0,    IOVT_UINT32,    0 },
    {"sd_clock",    IOV_CLOCK,    0,    IOVT_UINT32,    0 },
    {"sd_mode",    IOV_SDMODE,    0,    IOVT_UINT32,    100},
    {"sd_highspeed",    IOV_HISPEED,    0,    IOVT_UINT32,    0},
    {"sd_uhsimode",    IOV_UHSIMOD,    0,    IOVT_UINT32,    0},
#ifdef BCMDBG
    {"sd_hciregs",    IOV_HCIREGS,    0,    IOVT_BUFFER,    0 },
#endif
    {"tuning_mode", IOV_TUNEMOD,    0,    IOVT_UINT32,    0},

    {NULL, 0, 0, 0, 0 }
};
uint8 sdstd_turn_on_clock(sdioh_info_t *sd)
{
    sdstd_or_reg16(sd, SD_ClockCntrl, 0x4);
    return 0;
}

uint8 sdstd_turn_off_clock(sdioh_info_t *sd)
{
    sdstd_wreg16(sd, SD_ClockCntrl, sdstd_rreg16(sd, SD_ClockCntrl) & ~((uint16)0x4));
    return 0;
}

int
sdioh_iovar_op(sdioh_info_t *si, const char *name,
               void *params, int plen, void *arg, int len, bool set)
{
    const bcm_iovar_t *vi = NULL;
    int bcmerror = 0;
    int val_size;
    int32 int_val = 0;
    bool bool_val;
    uint32 actionid;

    ASSERT(name);
    ASSERT(len >= 0);

    /* Get must have return space; Set does not take qualifiers */
    ASSERT(set || (arg && len));
    ASSERT(!set || (!params && !plen));

    sd_trace(("%s: Enter (%s %s)\n", __FUNCTION__, (set ? "set" : "get"), name));

    if ((vi = bcm_iovar_lookup(sdioh_iovars, name)) == NULL) {
        bcmerror = BCME_UNSUPPORTED;
        goto exit;
    }

    if ((bcmerror = bcm_iovar_lencheck(vi, arg, len, set)) != 0)
        goto exit;

    /* XXX Copied from dhd, copied from wl; certainly overkill here? */
    /* Set up params so get and set can share the convenience variables */
    if (params == NULL) {
        params = arg;
        plen = len;
    }

    if (vi->type == IOVT_VOID)
        val_size = 0;
    else if (vi->type == IOVT_BUFFER)
        val_size = len;
    else
        val_size = sizeof(int);

    if (plen >= (int)sizeof(int_val))
        bcopy(params, &int_val, sizeof(int_val));

    bool_val = (int_val != 0) ? TRUE : FALSE;
    BCM_REFERENCE(bool_val);

    actionid = set ? IOV_SVAL(vi->varid) : IOV_GVAL(vi->varid);
    switch (actionid) {
    case IOV_GVAL(IOV_MSGLEVEL):
        int_val = (int32)sd_msglevel;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_MSGLEVEL):
        sd_msglevel = int_val;
        break;

    case IOV_GVAL(IOV_BLOCKMODE):
        int_val = (int32)si->sd_blockmode;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_BLOCKMODE):
        si->sd_blockmode = (bool)int_val;
        /* Haven't figured out how to make non-block mode with DMA */
        if (!si->sd_blockmode)
            si->sd_dma_mode = DMA_MODE_NONE;
        break;

#ifdef BCMSDYIELD
    case IOV_GVAL(IOV_YIELDCPU):
        int_val = sd_yieldcpu;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_YIELDCPU):
        sd_yieldcpu = (bool)int_val;
        break;

    case IOV_GVAL(IOV_MINYIELD):
        int_val = sd_minyield;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_MINYIELD):
        sd_minyield = (bool)int_val;
        break;

    case IOV_GVAL(IOV_FORCERB):
        int_val = sd_forcerb;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_FORCERB):
        sd_forcerb = (bool)int_val;
        break;
#endif /* BCMSDYIELD */

    case IOV_GVAL(IOV_BLOCKSIZE):
        if ((uint32)int_val > si->num_funcs) {
            bcmerror = BCME_BADARG;
            break;
        }
        int_val = (int32)si->client_block_size[int_val];
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_BLOCKSIZE):
    {
        uint func = ((uint32)int_val >> 16);
        uint blksize = (uint16)int_val;
        uint maxsize;

        if (func > si->num_funcs) {
            bcmerror = BCME_BADARG;
            break;
        }

        /* XXX These hardcoded sizes are a hack, remove after proper CIS parsing. */
        switch (func) {
        case 0: maxsize = 32; break;
        case 1: maxsize = BLOCK_SIZE_4318; break;
        case 2: maxsize = BLOCK_SIZE_4328; break;
        default: maxsize = 0;
        }
        if (blksize > maxsize) {
            bcmerror = BCME_BADARG;
            break;
        }
        if (!blksize) {
            blksize = maxsize;
        }

        /* Now set it */
        sdstd_lock(si);
        bcmerror = set_client_block_size(si, func, blksize);
        sdstd_unlock(si);
        break;
    }

    case IOV_GVAL(IOV_DMA):
        int_val = (int32)si->sd_dma_mode;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_DMA):
        si->sd_dma_mode = (char)int_val;
        sdstd_set_dma_mode(si, si->sd_dma_mode);
        break;

    case IOV_GVAL(IOV_USEINTS):
        int_val = (int32)si->use_client_ints;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_USEINTS):
        si->use_client_ints = (bool)int_val;
        if (si->use_client_ints)
            si->intmask |= CLIENT_INTR;
        else
            si->intmask &= ~CLIENT_INTR;
        break;

    case IOV_GVAL(IOV_DIVISOR):
        int_val = (uint32)sd_divisor;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_DIVISOR):
        sd_divisor = int_val;
        if (!sdstd_start_clock(si, (uint16)sd_divisor)) {
            sd_err(("set clock failed!\n"));
            bcmerror = BCME_ERROR;
        }
        break;

    case IOV_GVAL(IOV_POWER):
        int_val = (uint32)sd_power;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_GVAL(IOV_POWER_SAVE):
        int_val = (uint32)sd_3_power_save;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_POWER):
        sd_power = int_val;
        if (sd_power == 1) {
            if (sdstd_driver_init(si) != SUCCESS) {
                sd_err(("set SD Slot power failed!\n"));
                bcmerror = BCME_ERROR;
            } else {
                sd_err(("SD Slot Powered ON.\n"));
            }
        } else {
            uint8 pwr = 0;

            pwr = SFIELD(pwr, PWR_BUS_EN, 0);
            sdstd_wreg8(si, SD_PwrCntrl, pwr); /* Set Voltage level */
            sd_err(("SD Slot Powered OFF.\n"));
        }
        break;

    case IOV_SVAL(IOV_POWER_SAVE):
        sd_3_power_save = int_val;
        break;

    case IOV_GVAL(IOV_CLOCK):
        int_val = (uint32)sd_clock;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_CLOCK):
        sd_clock = int_val;
        if (sd_clock == 1) {
            sd_info(("SD Clock turned ON.\n"));
            if (!sdstd_start_clock(si, (uint16)sd_divisor)) {
                sd_err(("sdstd_start_clock failed\n"));
                bcmerror = BCME_ERROR;
            }
        } else {
            /* turn off HC clock */
            sdstd_wreg16(si, SD_ClockCntrl,
                         sdstd_rreg16(si, SD_ClockCntrl) & ~((uint16)0x4));

            sd_info(("SD Clock turned OFF.\n"));
        }
        break;

    case IOV_GVAL(IOV_SDMODE):
        int_val = (uint32)sd_sdmode;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_SDMODE):
        sd_sdmode = int_val;

        if (!sdstd_bus_width(si, sd_sdmode)) {
            sd_err(("sdstd_bus_width failed\n"));
            bcmerror = BCME_ERROR;
        }
        break;

    case IOV_GVAL(IOV_HISPEED):
        int_val = (uint32)sd_hiok;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_HISPEED):
        sd_hiok = int_val;
        bcmerror = sdstd_set_highspeed_mode(si, (bool)sd_hiok);
        break;

    case IOV_GVAL(IOV_UHSIMOD):
        sd3_trace(("%s: Get UHSI: \n", __FUNCTION__));
        int_val = (int)sd_uhsimode;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_SVAL(IOV_UHSIMOD):
        {
            int oldval = sd_uhsimode; /* save old, working value */
            sd3_trace(("%s: Set UHSI: \n", __FUNCTION__));
            /* check if UHSI is supported by card/host */
            if (!(si->card_UHSI_voltage_Supported && si->host_UHSISupported)) {
                sd_err(("%s:UHSI not suppoted!\n", __FUNCTION__));
                bcmerror = BCME_UNSUPPORTED;
                break;
            }
            /* check for valid values */
            if (!((int_val == SD3CLKMODE_AUTO) ||
                (int_val == SD3CLKMODE_DISABLED) ||
                ((int_val >= SD3CLKMODE_0_SDR12) &&
                (int_val <= SD3CLKMODE_4_DDR50)))) {
                sd_err(("%s:CLK: bad arg!\n", __FUNCTION__));
                bcmerror = BCME_BADARG;
                break;
            }

            sd_uhsimode = int_val;
            if (SUCCESS != sdstd_3_clock_wrapper(si)) {
                sd_err(("%s:Error in setting uhsi clkmode:%d,"
                    "restoring back to %d\n", __FUNCTION__,
                    sd_uhsimode, oldval));
                /* try to set back the old one */
                sd_uhsimode = oldval;
                if (SUCCESS != sdstd_3_clock_wrapper(si)) {
                    sd_err(("%s:Error in setting uhsi to old mode;"
                        "ignoring:\n", __FUNCTION__));
                }
            }
            break;
        }
#ifdef DHD_DEBUG
    case IOV_SVAL(IOV_TUNEMOD):
    {

        if( int_val == SD_DHD_DISABLE_PERIODIC_TUNING) { /* do tuning single time */
            sd3_trace(("Start tuning from Iovar\n"));
            si->sd3_tuning_reqd = TRUE;
            sdstd_enable_disable_periodic_timer(si, int_val);
            sdstd_lock(si);
            sdstd_3_clk_tuning(si, sdstd_3_get_uhsi_clkmode(si));
            sdstd_unlock(si);
            si->sd3_tuning_reqd = FALSE;
        }
        if (int_val == SD_DHD_ENABLE_PERIODIC_TUNING) {
            sd3_trace(("Enabling  automatic tuning\n"));
            si->sd3_tuning_reqd = TRUE;
            sdstd_enable_disable_periodic_timer(si, int_val);
        }
        break;
    }
#endif /* debugging purpose */
    case IOV_GVAL(IOV_NUMINTS):
        int_val = (int32)si->intrcount;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_GVAL(IOV_NUMLOCALINTS):
        int_val = (int32)si->local_intrcount;
        bcopy(&int_val, arg, val_size);
        break;

    case IOV_GVAL(IOV_HOSTREG):
    {
        /* XXX Should copy for alignment reasons */
        sdreg_t *sd_ptr = (sdreg_t *)params;

        if (sd_ptr->offset < SD_SysAddr || sd_ptr->offset > SD3_WL_BT_reset_register) {
            sd_err(("%s: bad offset 0x%x\n", __FUNCTION__, sd_ptr->offset));
            bcmerror = BCME_BADARG;
            break;
        }

        sd_trace(("%s: rreg%d at offset %d\n", __FUNCTION__,
                  (sd_ptr->offset & 1) ? 8 : ((sd_ptr->offset & 2) ? 16 : 32),
                  sd_ptr->offset));
        if (sd_ptr->offset & 1)
            int_val = sdstd_rreg8(si, sd_ptr->offset);
        else if (sd_ptr->offset & 2)
            int_val = sdstd_rreg16(si, sd_ptr->offset);
        else
            int_val = sdstd_rreg(si, sd_ptr->offset);

        bcopy(&int_val, arg, sizeof(int_val));
        break;
    }

    case IOV_SVAL(IOV_HOSTREG):
    {
        /* XXX Should copy for alignment reasons */
        sdreg_t *sd_ptr = (sdreg_t *)params;

        if (sd_ptr->offset < SD_SysAddr || sd_ptr->offset > SD3_WL_BT_reset_register) {
            sd_err(("%s: bad offset 0x%x\n", __FUNCTION__, sd_ptr->offset));
            bcmerror = BCME_BADARG;
            break;
        }

        sd_trace(("%s: wreg%d value 0x%08x at offset %d\n", __FUNCTION__, sd_ptr->value,
                  (sd_ptr->offset & 1) ? 8 : ((sd_ptr->offset & 2) ? 16 : 32),
                  sd_ptr->offset));
        if (sd_ptr->offset & 1)
            sdstd_wreg8(si, sd_ptr->offset, (uint8)sd_ptr->value);
        else if (sd_ptr->offset & 2)
            sdstd_wreg16(si, sd_ptr->offset, (uint16)sd_ptr->value);
        else
            sdstd_wreg(si, sd_ptr->offset, (uint32)sd_ptr->value);

        break;
    }

    case IOV_GVAL(IOV_DEVREG):
    {
        /* XXX Should copy for alignment reasons */
        sdreg_t *sd_ptr = (sdreg_t *)params;
        uint8 data;

        if (sdioh_cfg_read(si, sd_ptr->func, sd_ptr->offset, &data)) {
            bcmerror = BCME_SDIO_ERROR;
            break;
        }

        int_val = (int)data;
        bcopy(&int_val, arg, sizeof(int_val));
        break;
    }

    case IOV_SVAL(IOV_DEVREG):
    {
        /* XXX Should copy for alignment reasons */
        sdreg_t *sd_ptr = (sdreg_t *)params;
        uint8 data = (uint8)sd_ptr->value;

        if (sdioh_cfg_write(si, sd_ptr->func, sd_ptr->offset, &data)) {
            bcmerror = BCME_SDIO_ERROR;
            break;
        }
        break;
    }

#ifdef BCMDBG
    case IOV_GVAL(IOV_HCIREGS):
    {
        struct bcmstrbuf b;
        bcm_binit(&b, arg, len);

        sdstd_lock(si);
        bcm_bprintf(&b, "IntrStatus:       0x%04x ErrorIntrStatus       0x%04x\n",
                    sdstd_rreg16(si, SD_IntrStatus),
                    sdstd_rreg16(si, SD_ErrorIntrStatus));
        bcm_bprintf(&b, "IntrStatusEnable: 0x%04x ErrorIntrStatusEnable 0x%04x\n",
                    sdstd_rreg16(si, SD_IntrStatusEnable),
                    sdstd_rreg16(si, SD_ErrorIntrStatusEnable));
        bcm_bprintf(&b, "IntrSignalEnable: 0x%04x ErrorIntrSignalEnable 0x%04x\n",
                    sdstd_rreg16(si, SD_IntrSignalEnable),
                    sdstd_rreg16(si, SD_ErrorIntrSignalEnable));
        print_regs(si);

        sdstd_unlock(si);

        if (!b.size)
            bcmerror = BCME_BUFTOOSHORT;
        break;
    }
#endif /* BCMDBG */

    default:
        bcmerror = BCME_UNSUPPORTED;
        break;
    }
exit:

    /* XXX Remove protective lock after clients all clean... */
    return bcmerror;
}

extern SDIOH_API_RC
sdioh_cfg_read(sdioh_info_t *sd, uint fnc_num, uint32 addr, uint8 *data)
{
    SDIOH_API_RC status;
    /* No lock needed since sdioh_request_byte does locking */
    status = sdioh_request_byte(sd, SDIOH_READ, fnc_num, addr, data);
    return status;
}

extern SDIOH_API_RC
sdioh_cfg_write(sdioh_info_t *sd, uint fnc_num, uint32 addr, uint8 *data)
{
    /* No lock needed since sdioh_request_byte does locking */
    SDIOH_API_RC status;
    status = sdioh_request_byte(sd, SDIOH_WRITE, fnc_num, addr, data);
    return status;
}

extern SDIOH_API_RC
sdioh_cis_read(sdioh_info_t *sd, uint func, uint8 *cisd, uint32 length)
{
    uint32 count;
    int offset;
    uint32 foo;
    uint8 *cis = cisd;

    sd_trace(("%s: Func = %d\n", __FUNCTION__, func));

    if (!sd->func_cis_ptr[func]) {
        bzero(cis, length);
        return SDIOH_API_RC_FAIL;
    }

    sdstd_lock(sd);
    *cis = 0;
    for (count = 0; count < length; count++) {
        offset =  sd->func_cis_ptr[func] + count;
        if (sdstd_card_regread(sd, 0, offset, 1, &foo)) {
            sd_err(("%s: regread failed: Can't read CIS\n", __FUNCTION__));
            sdstd_unlock(sd);
            return SDIOH_API_RC_FAIL;
        }
        *cis = (uint8)(foo & 0xff);
        cis++;
    }
    sdstd_unlock(sd);
    return SDIOH_API_RC_SUCCESS;
}

extern SDIOH_API_RC
sdioh_request_byte(sdioh_info_t *sd, uint rw, uint func, uint regaddr, uint8 *byte)
{
    int status = SDIOH_API_RC_SUCCESS;
    uint32 cmd_arg;
    uint32 rsp5;

    sdstd_lock(sd);
    if (rw == SDIOH_READ)
    sdstd_3_check_and_do_tuning(sd, CHECK_TUNING_PRE_DATA);

    /* Change to DATA_TRANSFER_ONGOING , protection against tuning tasklet */
    sdstd_3_set_data_state(sd, DATA_TRANSFER_ONGOING);

#ifdef BCMDBG
    if (sdstd_rreg16 (sd, SD_ErrorIntrStatus) != 0) {
        sd_err(("%s: Entering: ErririntrStatus 0x%x, intstat = 0x%x\n",
                __FUNCTION__, sdstd_rreg16(sd, SD_ErrorIntrStatus),
                sdstd_rreg16(sd, SD_IntrStatus)));
    }
#endif
    cmd_arg = 0;
    cmd_arg = SFIELD(cmd_arg, CMD52_FUNCTION, func);
    cmd_arg = SFIELD(cmd_arg, CMD52_REG_ADDR, regaddr);
    cmd_arg = SFIELD(cmd_arg, CMD52_RW_FLAG, rw == SDIOH_READ ? 0 : 1);
    cmd_arg = SFIELD(cmd_arg, CMD52_RAW, 0);
    cmd_arg = SFIELD(cmd_arg, CMD52_DATA, rw == SDIOH_READ ? 0 : *byte);

    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_52, cmd_arg)) != SUCCESS) {
        /* Change to DATA_TRANSFER_IDLE */
        sd->sd3_dat_state = DATA_TRANSFER_IDLE;
        sdstd_unlock(sd);
        return status;
    }

    sdstd_cmd_getrsp(sd, &rsp5, 1);
    if (sdstd_rreg16 (sd, SD_ErrorIntrStatus) != 0) {
        sd_err(("%s: 1: ErrorintrStatus 0x%x\n",
                __FUNCTION__, sdstd_rreg16(sd, SD_ErrorIntrStatus)));
        status = SDIOH_API_RC_FAIL;
    }
    if (GFIELD(rsp5, RSP5_FLAGS) != 0x10) {
        /* PR 101351: sdiod_aos sleep followed by immediate wakeup
         *    before sdiod_aos takes over has a problem.
         * While exiting sleep with CMD14, device returning 0x00
         * Don't flag as error for now for 0x1001f.
         */
        if (GFIELD(cmd_arg, CMD52_REG_ADDR) != F1_SLEEPCSR_ADDR) {
            sd_err(("%s: rsp5 flags is 0x%x\t %d \n",
                __FUNCTION__, GFIELD(rsp5, RSP5_FLAGS), func));
        }
        status = SDIOH_API_RC_FAIL;
    }

    if (GFIELD(rsp5, RSP5_STUFF)) {
        sd_err(("%s: rsp5 stuff is 0x%x: should be 0\n",
                __FUNCTION__, GFIELD(rsp5, RSP5_STUFF)));
        status = SDIOH_API_RC_FAIL;
    }

    if (rw == SDIOH_READ)
        *byte = GFIELD(rsp5, RSP5_DATA);

    /* Change to DATA_TRANSFER_IDLE */
    sdstd_3_set_data_state(sd, DATA_TRANSFER_IDLE);

    /* check if we have to do tuning; if so, start */
    sdstd_3_check_and_do_tuning(sd, CHECK_TUNING_POST_DATA);

    sdstd_unlock(sd);
    return status;
}

extern SDIOH_API_RC
sdioh_request_word(sdioh_info_t *sd, uint cmd_type, uint rw, uint func, uint addr,
                   uint32 *word, uint nbytes)
{
    int status;
    bool swap = FALSE;
#ifdef __IOPOS__
    uint tmp_addr = addr;
#endif

    sdstd_lock(sd);

    sdstd_3_check_and_do_tuning(sd, CHECK_TUNING_PRE_DATA);

    /* Change to DATA_TRANSFER_ONGOING , protection against tuning tasklet */
    sdstd_3_set_data_state(sd, DATA_TRANSFER_ONGOING);

#ifdef __IOPOS__
    tmp_addr &= ~0x8000;     /* SBSDIO_SB_ACCESS_2_4B_FLAG */
    /* XXX Move the decision to swap to the higher layer (wlc_pio.c), since
     * this is not efficient and will not scale.
     */
#if 0
    if (
        /* Read FIFOs */
        (tmp_addr == 0x130c) || (tmp_addr == 0x131c) || (tmp_addr == 0x132c) ||
        (tmp_addr == 0x133c) || (tmp_addr == 0x134c) || (tmp_addr == 0x135c) ||
        /* Write FIFOs */
        (tmp_addr == 0x1304) || (tmp_addr == 0x1314) || (tmp_addr == 0x1324) ||
        (tmp_addr == 0x1334) || (tmp_addr == 0x1344) || (tmp_addr == 0x1354)) {
            swap = TRUE;
    }
#endif
#endif /* __IOPOS__ */
    if (rw == SDIOH_READ) {
        status = sdstd_card_regread(sd, func, addr, nbytes, word);
        if (swap)
            *word = BCMSWAP32(*word);
    } else {
        if (swap)
            *word = BCMSWAP32(*word);
        status = sdstd_card_regwrite(sd, func, addr, nbytes, *word);
    }

    /* Change to DATA_TRANSFER_IDLE */
    sdstd_3_set_data_state(sd, DATA_TRANSFER_IDLE);

    /* check if we have to do tuning; if so, start */
    sdstd_3_check_and_do_tuning(sd, CHECK_TUNING_POST_DATA);

    sdstd_unlock(sd);
    return (status == SUCCESS ?  SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL);
}

#ifdef BCMSDIOH_TXGLOM
void
sdioh_glom_post(sdioh_info_t *sd, uint8 *frame, void *pkt, uint len)
{
    BCM_REFERENCE(pkt);
    sd->glom_info.dma_buf_arr[sd->glom_info.count] = frame;
    sd->glom_info.nbytes[sd->glom_info.count] = len;
    /* Convert the frame addr to phy addr for DMA in case of host controller version3 */
    if (sd->txglom_mode == SDPCM_TXGLOM_MDESC) {
        sd->glom_info.dma_phys_arr[sd->glom_info.count] = DMA_MAP(sd->osh,
            frame,
            len,
            DMA_TX, 0, 0);
    }
    sd->glom_info.count++;
}

void
sdioh_glom_clear(sdioh_info_t *sd)
{
    int i;
    /* DMA_MAP is done per frame only if host controller version is 3 */
    if (sd->txglom_mode == SDPCM_TXGLOM_MDESC) {
        for (i = 0; i < sd->glom_info.count; i++) {
            DMA_UNMAP(sd->osh,
                sd->glom_info.dma_phys_arr[i],
                sd->glom_info.nbytes[i],
                DMA_TX, 0, 0);
        }
    }
    sd->glom_info.count = 0;
}

uint
sdioh_set_mode(sdioh_info_t *sd, uint mode)
{
    if (mode == SDPCM_TXGLOM_CPY)
        sd->txglom_mode = mode;
    else if ((mode == SDPCM_TXGLOM_MDESC) && (sd->version == HOST_CONTR_VER_3))
        sd->txglom_mode = mode;

    return (sd->txglom_mode);
}

bool
sdioh_glom_enabled(void)
{
    return sd_txglom;
}
#endif /* BCMSDIOH_TXGLOM */

extern SDIOH_API_RC
sdioh_request_buffer(sdioh_info_t *sd, uint pio_dma, uint fix_inc, uint rw, uint func,
                     uint addr, uint reg_width, uint buflen_u, uint8 *buffer, void *pkt)
{
    uint8 is_ddr50 = FALSE;
    int len;
    int buflen = (int)buflen_u;
    bool fifo = (fix_inc == SDIOH_DATA_FIX);
    uint8 *localbuf = NULL, *tmpbuf = NULL;
    bool local_blockmode = sd->sd_blockmode;
    SDIOH_API_RC status = SDIOH_API_RC_SUCCESS;

    sdstd_lock(sd);

    is_ddr50 = (sd_uhsimode == SD3CLKMODE_4_DDR50) ? TRUE : FALSE;

    sdstd_3_check_and_do_tuning(sd, CHECK_TUNING_PRE_DATA);

    /* Change to DATA_TRANSFER_ONGOING , protection against tuning tasklet */
    sdstd_3_set_data_state(sd, DATA_TRANSFER_ONGOING);

    ASSERT(reg_width == 4);
    ASSERT(buflen_u < (1 << 30));
    ASSERT(sd->client_block_size[func]);

#ifdef BCMSDIOH_TXGLOM
    if (sd_txglom) {
        while (pkt) {
            sdioh_glom_post(sd, PKTDATA(sd->osh, pkt), pkt, PKTLEN(sd->osh, pkt));
            pkt = PKTNEXT(sd->osh, pkt);
        }
    }
#endif
    sd_data(("%s: %c len %d r_cnt %d t_cnt %d, pkt @0x%p\n",
             __FUNCTION__, rw == SDIOH_READ ? 'R' : 'W',
             buflen_u, sd->r_cnt, sd->t_cnt, pkt));

    /* Break buffer down into blocksize chunks:
     * Bytemode: 1 block at a time.
     * Blockmode: Multiples of blocksizes at a time w/ max of SD_PAGE.
     * Both: leftovers are handled last (will be sent via bytemode).
     */
    while (buflen > 0) {
        if (local_blockmode) {
            int max_tran_size = SD_PAGE;
#ifdef BCMSDIOH_TXGLOM
            /* There is no alignment requirement for HC3 */
            if ((sd->version == HOST_CONTR_VER_3) && sd_txglom)
                max_tran_size = SD_PAGE * 4;
#endif
            /* Max xfer is Page size */
            len = MIN(max_tran_size, buflen);

            /* Round down to a block boundry */
            if (buflen > sd->client_block_size[func])
                len = (len/sd->client_block_size[func]) *
                        sd->client_block_size[func];
            /* XXX Arasan trashes 3-byte transfers, WAR to add one byte extra. */
            /* XXX In Case of SDIO3.0 DDR50 mode if no of bytes to be
             * transferred is odd append one more byte to make it even.
             * Check If odd bytes can come for SDIO_FUNC_2 also.
             */
            if ((func == SDIO_FUNC_1) && (((len % 4) == 3) || (((len % 2) == 1) &&
                (is_ddr50))) && ((rw == SDIOH_WRITE) || (rw == SDIOH_READ))) {
                sd_err(("%s: Rounding up buffer to mod4 length.\n", __FUNCTION__));
                len++;
                tmpbuf = buffer;
                if ((localbuf = (uint8 *)MALLOC(sd->osh, len)) == NULL) {
                    sd_err(("out of memory, malloced %d bytes\n",
                            MALLOCED(sd->osh)));
                    status = SDIOH_API_RC_FAIL;
                    goto done;
                }
                bcopy(buffer, localbuf, len);
                buffer = localbuf;
            }
        } else {
            /* Byte mode: One block at a time */
            len = MIN(sd->client_block_size[func], buflen);
        }

        if (sdstd_card_buf(sd, rw, func, fifo, addr, len, (uint32 *)buffer) != SUCCESS) {
            status = SDIOH_API_RC_FAIL;
        }

        /* XXX Restore len and buffer pointer WAR'ed for Arasan 3-byte transfer problem */
        /* XXX WAR for SDIO3.0 DDR50 mode. */
        if (local_blockmode && localbuf) {
            MFREE(sd->osh, localbuf, len);
            localbuf = NULL;
            len--;
            buffer = tmpbuf;
            sd_err(("%s: Restoring back buffer ptr and len.\n", __FUNCTION__));
        }

        if (status == SDIOH_API_RC_FAIL) {
            goto done;
        }

        buffer += len;
        buflen -= len;
        if (!fifo)
            addr += len;
#ifdef BCMSDIOH_TXGLOM
        /* This loop should not come in case of glommed pkts as it is send in
         * multiple of blocks or total pkt size less than a block
         */
        if (sd->glom_info.count != 0)
            buflen = 0;
#endif
    }
done:

    /* Change to DATA_TRANSFER_IDLE */
    sdstd_3_set_data_state(sd, DATA_TRANSFER_IDLE);

    /* check if we have to do tuning; if so, start */
    sdstd_3_check_and_do_tuning(sd, CHECK_TUNING_POST_DATA);

    sdstd_unlock(sd);

#ifdef BCMSDIOH_TXGLOM
    if (sd_txglom)
        sdioh_glom_clear(sd);
#endif

    return status;
}

extern SDIOH_API_RC
sdioh_gpioouten(sdioh_info_t *sd, uint32 gpio)
{
    uint offset = 0;
    uint16 val;

    /* check if upper bank */
    if (gpio >= SDH_GPIO16) {
        gpio -= SDH_GPIO16;
        offset = 2;
    }

    val = sdstd_rreg16(sd, SD_GPIO_OE + offset);
    val |= (1 << gpio);
    sdstd_wreg16(sd, SD_GPIO_OE + offset, val);

    return SDIOH_API_RC_SUCCESS;
}

extern SDIOH_API_RC
sdioh_gpioout(sdioh_info_t *sd, uint32 gpio, bool enab)
{
    uint offset = 0;
    uint16 val;

    /* check if upper bank */
    if (gpio >= SDH_GPIO16) {
        gpio -= SDH_GPIO16;
        offset = 2;
    }

    val = sdstd_rreg16(sd, SD_GPIO_Reg + offset);
    if (enab == TRUE)
        val |= (1 << gpio);
    else
        val &= ~(1 << gpio);
    sdstd_wreg16(sd, SD_GPIO_Reg + offset, val);

    return SDIOH_API_RC_SUCCESS;
}

extern bool
sdioh_gpioin(sdioh_info_t *sd, uint32 gpio)
{
    uint offset = 0;
    uint16 val;

    /* check if upper bank */
    if (gpio >= SDH_GPIO16) {
        gpio -= SDH_GPIO16;
        offset = 2;
    }

    val = sdstd_rreg16(sd, SD_GPIO_Reg + offset);
    val = (val >> gpio) & 1;

    return (val == 1);
}

extern SDIOH_API_RC
sdioh_gpio_init(sdioh_info_t *sd)
{
    uint rev;

    rev = sdstd_rreg16(sd, SD_HostControllerVersion) >> 8;

    /* Only P206 (fpga rev >= 16) supports gpio */
    if (rev < 16) {
        sd_err(("%s: gpio not supported in rev %d \n", __FUNCTION__, rev));
        return SDIOH_API_RC_FAIL;
    }

    sdstd_wreg16(sd, SD_GPIO_Enable, SDH_GPIO_ENABLE);
    sdstd_wreg16(sd, SD_GPIO_Enable + 2, SDH_GPIO_ENABLE);

    /* Default to input */
    sdstd_wreg16(sd, SD_GPIO_OE, 0);
    sdstd_wreg16(sd, SD_GPIO_OE + 2, 0);

    return SDIOH_API_RC_SUCCESS;
}

extern SDIOH_API_RC
sdioh_sleep(sdioh_info_t *sd, bool enab)
{
    SDIOH_API_RC status;
    uint32 cmd_arg = 0, rsp1 = 0;
    int retry = 100;

    sdstd_lock(sd);

    cmd_arg = SFIELD(cmd_arg, CMD14_RCA, sd->card_rca);
    cmd_arg = SFIELD(cmd_arg, CMD14_SLEEP, enab);

    /*
     * For ExitSleep:
     *  1) Repeat CMD14 until R1 is received
     *  2) Send CMD7
     */
    status = SDIOH_API_RC_FAIL;
    while (retry-- > 0) {
        if ((sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_14, cmd_arg)) == SUCCESS) {
            status = SDIOH_API_RC_SUCCESS;
            break;
        }
        OSL_DELAY(1400);
    }

    if (status == SDIOH_API_RC_FAIL) {
        sd_err(("%s: CMD14: failed! enable:%d\n", __FUNCTION__, enab));
        goto exit;
    }

    sdstd_cmd_getrsp(sd, &rsp1, 1);
    sd_info(("%s: CMD14 OK: cmd_resp:0x%x\n", __FUNCTION__, rsp1));

    /* ExitSleep: Send CMD7 After R1 */
    if (enab == FALSE) {
        /* Select the card */
        cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_7, cmd_arg)) != SUCCESS) {
            sd_err(("%s: CMD14 send CMD7 failed!\n", __FUNCTION__));
            status = SDIOH_API_RC_FAIL;
            goto exit;
        }

        sdstd_cmd_getrsp(sd, &rsp1, 1);
        if (rsp1 != SDIOH_CMD7_EXP_STATUS) {
            sd_err(("%s: CMD7 response error. Response = 0x%x!\n",
                    __FUNCTION__, rsp1));
            status = SDIOH_API_RC_FAIL;
            goto exit;
        }
    }

exit:
    sdstd_unlock(sd);

    return status;
}

/* XXX Copied guts of request_byte and cmd_issue.  Might make sense to fold this into
 * those by passing another parameter indicating command type (abort).  [But maybe
 * keeping it separate is better -- if called internally on command failure it's less
 * recursion to wrap your head around?]
 */
static int
sdstd_abort(sdioh_info_t *sd, uint func)
{
    int err = 0;
    int retries;

    uint16 cmd_reg;
    uint32 cmd_arg;
    uint32 rsp5;
    uint8 rflags;

    uint16 int_reg = 0;
    uint16 plain_intstatus;

    /* Argument is write to F0 (CCCR) IOAbort with function number */
    cmd_arg = 0;
    cmd_arg = SFIELD(cmd_arg, CMD52_FUNCTION, SDIO_FUNC_0);
    cmd_arg = SFIELD(cmd_arg, CMD52_REG_ADDR, SDIOD_CCCR_IOABORT);
    cmd_arg = SFIELD(cmd_arg, CMD52_RW_FLAG, SD_IO_OP_WRITE);
    cmd_arg = SFIELD(cmd_arg, CMD52_RAW, 0);
    cmd_arg = SFIELD(cmd_arg, CMD52_DATA, func);

    /* Command is CMD52 write */
    cmd_reg = 0;
    cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48_BUSY);
    cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 1);
    cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 1);
    cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
    cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_ABORT);
    cmd_reg = SFIELD(cmd_reg, CMD_INDEX, SDIOH_CMD_52);

    /* XXX Copied from cmd_issue(), but no SPI response handling! */
    if (sd->sd_mode == SDIOH_MODE_SPI) {
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 0);
    }

    /* Wait for CMD_INHIBIT to go away as per spec section 3.6.1.1 */
    /* XXX For a single-threaded driver, what circumstances would result
     * in cmd_inhibit being on but going off in a short time?  Experiment
     * shows a HW command timeout doesn't leave inhibit on, so maybe a SW
     * timeout?  Then that command should be responsible for clearing...
     */
    retries = RETRIES_SMALL;
    while (GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_CMD_INHIBIT)) {
        if (retries == RETRIES_SMALL)
            sd_err(("%s: Waiting for Command Inhibit, state 0x%08x\n",
                    __FUNCTION__, sdstd_rreg(sd, SD_PresentState)));
        if (!--retries) {
            sd_err(("%s: Command Inhibit timeout, state 0x%08x\n",
                    __FUNCTION__, sdstd_rreg(sd, SD_PresentState)));
            if (trap_errs)
                ASSERT(0);
            err = BCME_SDIO_ERROR;
            goto done;
        }
    }

    /* Clear errors from any previous commands */
    if ((plain_intstatus = sdstd_rreg16(sd, SD_ErrorIntrStatus)) != 0) {
        sd_err(("abort: clearing errstat 0x%04x\n", plain_intstatus));
        sdstd_wreg16(sd, SD_ErrorIntrStatus, plain_intstatus);
    }
    plain_intstatus = sdstd_rreg16(sd, SD_IntrStatus);
    if (plain_intstatus & ~(SFIELD(0, INTSTAT_CARD_INT, 1))) {
        sd_err(("abort: intstatus 0x%04x\n", plain_intstatus));
        if (GFIELD(plain_intstatus, INTSTAT_CMD_COMPLETE)) {
            sd_err(("SDSTD_ABORT: CMD COMPLETE SET BEFORE COMMAND GIVEN!!!\n"));
        }
        if (GFIELD(plain_intstatus, INTSTAT_CARD_REMOVAL)) {
            sd_err(("SDSTD_ABORT: INTSTAT_CARD_REMOVAL\n"));
            err = BCME_NODEVICE;
            goto done;
        }
    }

    /* Issue the command */
    sdstd_wreg(sd, SD_Arg0, cmd_arg);
    sdstd_wreg16(sd, SD_Command, cmd_reg);

    /* In interrupt mode return, expect later CMD_COMPLETE interrupt */
    if (!sd->polled_mode)
        return err;

    /* Otherwise, wait for the command to complete */
    retries = RETRIES_LARGE;
    do {
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (--retries &&
             (GFIELD(int_reg, INTSTAT_ERROR_INT) == 0) &&
             (GFIELD(int_reg, INTSTAT_CMD_COMPLETE) == 0));

    /* If command completion fails, do a cmd reset and note the error */
    if (!retries) {
        sd_err(("%s: CMD_COMPLETE timeout: intr 0x%04x err 0x%04x state 0x%08x\n",
                __FUNCTION__, int_reg,
                sdstd_rreg16(sd, SD_ErrorIntrStatus),
                sdstd_rreg(sd, SD_PresentState)));

        sdstd_wreg8(sd, SD_SoftwareReset, SFIELD(0, SW_RESET_CMD, 1));
        retries = RETRIES_LARGE;
        do {
            sd_trace(("%s: waiting for CMD line reset\n", __FUNCTION__));
        } while ((GFIELD(sdstd_rreg8(sd, SD_SoftwareReset),
                         SW_RESET_CMD)) && retries--);

        if (!retries) {
            sd_err(("%s: Timeout waiting for CMD line reset\n", __FUNCTION__));
        }

        if (trap_errs)
            ASSERT(0);

        err = BCME_SDIO_ERROR;
    }

    /* Clear Command Complete interrupt */
    int_reg = SFIELD(0, INTSTAT_CMD_COMPLETE, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    /* Check for Errors */
    if ((plain_intstatus = sdstd_rreg16 (sd, SD_ErrorIntrStatus)) != 0) {
        sd_err(("%s: ErrorintrStatus: 0x%x, "
                "(intrstatus = 0x%x, present state 0x%x) clearing\n",
                __FUNCTION__, plain_intstatus,
                sdstd_rreg16(sd, SD_IntrStatus),
                sdstd_rreg(sd, SD_PresentState)));

        sdstd_wreg16(sd, SD_ErrorIntrStatus, plain_intstatus);

        sdstd_wreg8(sd, SD_SoftwareReset, SFIELD(0, SW_RESET_DAT, 1));
        retries = RETRIES_LARGE;
        do {
            sd_trace(("%s: waiting for DAT line reset\n", __FUNCTION__));
        } while ((GFIELD(sdstd_rreg8(sd, SD_SoftwareReset),
                         SW_RESET_DAT)) && retries--);

        if (!retries) {
            sd_err(("%s: Timeout waiting for DAT line reset\n", __FUNCTION__));
        }

        if (trap_errs)
            ASSERT(0);

        /* ABORT is dataless, only cmd errs count */
        /* XXX But what about busy timeout?  Response valid? */
        if (plain_intstatus & ERRINT_CMD_ERRS)
            err = BCME_SDIO_ERROR;
    }

    /* If command failed don't bother looking at response */
    if (err)
        goto done;

    /* Otherwise, check the response */
    sdstd_cmd_getrsp(sd, &rsp5, 1);
    rflags = GFIELD(rsp5, RSP5_FLAGS);

    if (rflags & SD_RSP_R5_ERRBITS) {
        sd_err(("%s: R5 flags include errbits: 0x%02x\n", __FUNCTION__, rflags));

        /* The CRC error flag applies to the previous command */
        if (rflags & (SD_RSP_R5_ERRBITS & ~SD_RSP_R5_COM_CRC_ERROR)) {
            err = BCME_SDIO_ERROR;
            goto done;
        }
    }

    if (((rflags & (SD_RSP_R5_IO_CURRENTSTATE0 | SD_RSP_R5_IO_CURRENTSTATE1)) != 0x10) &&
        ((rflags & (SD_RSP_R5_IO_CURRENTSTATE0 | SD_RSP_R5_IO_CURRENTSTATE1)) != 0x20)) {
        sd_err(("%s: R5 flags has bad state: 0x%02x\n", __FUNCTION__, rflags));
        err = BCME_SDIO_ERROR;
        goto done;
    }

    if (GFIELD(rsp5, RSP5_STUFF)) {
        sd_err(("%s: rsp5 stuff is 0x%x: should be 0\n",
                __FUNCTION__, GFIELD(rsp5, RSP5_STUFF)));
        err = BCME_SDIO_ERROR;
        goto done;
    }

done:
    if (err == BCME_NODEVICE)
        return err;

    /* XXX As per spec 3.7.1 (and to be safe) do the resets here */
    sdstd_wreg8(sd, SD_SoftwareReset,
                SFIELD(SFIELD(0, SW_RESET_DAT, 1), SW_RESET_CMD, 1));

    retries = RETRIES_LARGE;
    do {
        rflags = sdstd_rreg8(sd, SD_SoftwareReset);
        if (!GFIELD(rflags, SW_RESET_DAT) && !GFIELD(rflags, SW_RESET_CMD))
            break;
    } while (--retries);

    if (!retries) {
        sd_err(("%s: Timeout waiting for DAT/CMD reset: 0x%02x\n",
                __FUNCTION__, rflags));
        err = BCME_SDIO_ERROR;
    }

    return err;
}

extern int
sdioh_abort(sdioh_info_t *sd, uint fnum)
{
    int ret;

    sdstd_lock(sd);
    ret = sdstd_abort(sd, fnum);
    sdstd_unlock(sd);

    return ret;
}

int
sdioh_start(sdioh_info_t *sd, int stage)
{
    return SUCCESS;
}

int
sdioh_stop(sdioh_info_t *sd)
{
    return SUCCESS;
}

int
sdioh_waitlockfree(sdioh_info_t *sd)
{
    sdstd_waitlockfree(sd);
    return SUCCESS;
}

static int
sdstd_check_errs(sdioh_info_t *sdioh_info, uint32 cmd, uint32 arg)
{
    uint16 regval;
    uint retries;
    uint function = 0;

    /* If no errors, we're done */
    if ((regval = sdstd_rreg16(sdioh_info, SD_ErrorIntrStatus)) == 0)
        return SUCCESS;

#ifdef BCMQT
    if (regval == 0xFFFF) {
        /* XXX - Getting bogus errors under QT
         * XXX - Not sure why; Just ignore for now
         */
        sd_err(("%s: Bogus SD_ErrorIntrStatus: 0x%x????\n", __FUNCTION__, regval));
        sdstd_wreg16(sdioh_info, SD_ErrorIntrStatus, regval);
        return SUCCESS;
    }
#endif

    sd_info(("%s: ErrorIntrStatus 0x%04x (clearing), IntrStatus 0x%04x PresentState 0x%08x\n",
            __FUNCTION__, regval, sdstd_rreg16(sdioh_info, SD_IntrStatus),
            sdstd_rreg(sdioh_info, SD_PresentState)));
    sdstd_wreg16(sdioh_info, SD_ErrorIntrStatus, regval);

    if (cmd == SDIOH_CMD_14) {
        if (regval & ERRINT_CMD_TIMEOUT_BIT) {
            /* PR 101351: sdiod_aos sleep followed by immediate wakeup
             *    before sdiod_aos takes over has a problem.
             * Getting command timeouts while exiting sleep
             * with CMD14. Ignore this error due to this PR.
             */
            regval &= ~ERRINT_CMD_TIMEOUT_BIT;
        }
    }

    /* On command error, issue CMD reset */
    if (regval & ERRINT_CMD_ERRS) {
        sd_trace(("%s: issuing CMD reset\n", __FUNCTION__));
        sdstd_wreg8(sdioh_info, SD_SoftwareReset, SFIELD(0, SW_RESET_CMD, 1));
        for (retries = RETRIES_LARGE; retries; retries--)
            if (!(GFIELD(sdstd_rreg8(sdioh_info, SD_SoftwareReset), SW_RESET_CMD)))
                break;
        if (!retries) {
            sd_err(("%s: Timeout waiting for CMD line reset\n", __FUNCTION__));
        }
    }

    /* On data error, issue DAT reset */
    if (regval & ERRINT_DATA_ERRS) {
        if (regval & ERRINT_ADMA_BIT)
            sd_err(("%s:ADMAError: status:0x%x\n",
                __FUNCTION__, sdstd_rreg(sdioh_info, SD_ADMA_ErrStatus)));
        sd_trace(("%s: issuing DAT reset\n", __FUNCTION__));
        sdstd_wreg8(sdioh_info, SD_SoftwareReset, SFIELD(0, SW_RESET_DAT, 1));
        for (retries = RETRIES_LARGE; retries; retries--)
            if (!(GFIELD(sdstd_rreg8(sdioh_info, SD_SoftwareReset), SW_RESET_DAT)))
                break;
        if (!retries) {
            sd_err(("%s: Timeout waiting for DAT line reset\n", __FUNCTION__));
        }
    }

    /* For an IO command (CMD52 or CMD53) issue an abort to the appropriate function */
    if (cmd == SDIOH_CMD_53)
        function = GFIELD(arg, CMD53_FUNCTION);
    else if (cmd == SDIOH_CMD_52) {
        /* PR 101351: sdiod_aos sleep followed by immediate wakeup
         *    before sdiod_aos takes over has a problem.
         */
        if (GFIELD(arg, CMD52_REG_ADDR) != F1_SLEEPCSR_ADDR)
            function = GFIELD(arg, CMD52_FUNCTION);
    }
    if (function) {
        sd_trace(("%s: requesting abort for function %d after cmd %d\n",
                  __FUNCTION__, function, cmd));
        sdstd_abort(sdioh_info, function);
    }

    if (trap_errs)
        ASSERT(0);

    return ERROR;
}


#ifdef BCMINTERNAL
extern SDIOH_API_RC
sdioh_test_diag(sdioh_info_t *sd)
{
    sd_err(("%s: Implement me\n", __FUNCTION__));
    return (0);
}
#endif /* BCMINTERNAL */

/*
 * Private/Static work routines
 */
static bool
sdstd_reset(sdioh_info_t *sd, bool host_reset, bool client_reset)
{
    int retries = RETRIES_LARGE;
    uchar regval;

    if (!sd)
        return TRUE;

    sdstd_lock(sd);
    /* Reset client card */
    if (client_reset && (sd->adapter_slot != -1)) {
        if (sdstd_card_regwrite(sd, 0, SDIOD_CCCR_IOABORT, 1, 0x8) != SUCCESS)
            sd_err(("%s: Cannot write to card reg 0x%lx\n",
                    __FUNCTION__, SDIOD_CCCR_IOABORT));
        else
            sd->card_rca = 0;
    }

    /* Reset host controller */
    if (host_reset) {
#ifdef __IOPOS__
        regval = 7;
#else
        regval = SFIELD(0, SW_RESET_ALL, 1);
#endif
        sdstd_wreg8(sd, SD_SoftwareReset, regval);
        do {
            sd_trace(("%s: waiting for reset\n", __FUNCTION__));
        } while ((sdstd_rreg8(sd, SD_SoftwareReset) & regval) && retries--);

        if (!retries) {
            sd_err(("%s: Timeout waiting for host reset\n", __FUNCTION__));
            sdstd_unlock(sd);
            return (FALSE);
        }

        /* A reset should reset bus back to 1 bit mode */
        sd->sd_mode = SDIOH_MODE_SD1;
        sdstd_set_dma_mode(sd, sd->sd_dma_mode);
    }
    sdstd_unlock(sd);
    return TRUE;
}

/* Disable device interrupt */
void
sdstd_devintr_off(sdioh_info_t *sd)
{
    sd_trace(("%s: %d\n", __FUNCTION__, sd->use_client_ints));
    if (sd->use_client_ints) {
        sd->intmask &= ~CLIENT_INTR;
        sdstd_wreg16(sd, SD_IntrSignalEnable, sd->intmask);
#ifndef __IOPOS__
        sdstd_rreg16(sd, SD_IntrSignalEnable); /* Sync readback */
#endif
    }
}

/* Enable device interrupt */
void
sdstd_devintr_on(sdioh_info_t *sd)
{
    ASSERT(sd->lockcount == 0);
    sd_trace(("%s: %d\n", __FUNCTION__, sd->use_client_ints));
    if (sd->use_client_ints) {
        if (sd->version < HOST_CONTR_VER_3) {
            uint16 status = sdstd_rreg16(sd, SD_IntrStatusEnable);
            sdstd_wreg16(sd, SD_IntrStatusEnable, SFIELD(status, INTSTAT_CARD_INT, 0));
            sdstd_wreg16(sd, SD_IntrStatusEnable, status);
        }

        sd->intmask |= CLIENT_INTR;
        sdstd_wreg16(sd, SD_IntrSignalEnable, sd->intmask);
#ifndef __IOPOS__
        sdstd_rreg16(sd, SD_IntrSignalEnable); /* Sync readback */
#endif
    }
}

#ifdef BCMSDYIELD
/* Enable/disable other interrupts */
void
sdstd_intrs_on(sdioh_info_t *sd, uint16 norm, uint16 err)
{
    if (err) {
        norm = SFIELD(norm, INTSTAT_ERROR_INT, 1);
        sdstd_wreg16(sd, SD_ErrorIntrSignalEnable, err);
    }

    sd->intmask |= norm;
    sdstd_wreg16(sd, SD_IntrSignalEnable, sd->intmask);
    if (sd_forcerb)
        sdstd_rreg16(sd, SD_IntrSignalEnable); /* Sync readback */
}

void
sdstd_intrs_off(sdioh_info_t *sd, uint16 norm, uint16 err)
{
    if (err) {
        norm = SFIELD(norm, INTSTAT_ERROR_INT, 1);
        sdstd_wreg16(sd, SD_ErrorIntrSignalEnable, 0);
    }

    sd->intmask &= ~norm;
    sdstd_wreg16(sd, SD_IntrSignalEnable, sd->intmask);
    if (sd_forcerb)
        sdstd_rreg16(sd, SD_IntrSignalEnable); /* Sync readback */
}
#endif /* BCMSDYIELD */

static int
sdstd_host_init(sdioh_info_t *sd)
{
    int         full_slot;
    uint8        reg8, baseclk;

    sd->controller_type = SDIOH_TYPE_BCM4390X;

    /* Controller supports ADMA2, so turn it on here. */
    sd->sd_dma_mode = DMA_MODE_ADMA2;

    full_slot = 0;

    sd->adapter_slot = full_slot;

    sd->version = sdstd_rreg16(sd, SD_HostControllerVersion) & 0xFF;
    switch (sd->version) {
        case 0:
            sd_err(("Host Controller version 1.0, Vendor Revision: 0x%02x\n",
                sdstd_rreg16(sd, SD_HostControllerVersion) >> 8));
            break;
        case 1:
            sd_err(("Host Controller version 2.0, Vendor Revision: 0x%02x\n",
                sdstd_rreg16(sd, SD_HostControllerVersion) >> 8));
            break;
        case 2:
            sd_err(("Host Controller version 3.0, Vendor Revision: 0x%02x\n",
                sdstd_rreg16(sd, SD_HostControllerVersion) >> 8));
            break;
        default:
            sd_err(("%s: Host Controller version 0x%02x not supported.\n",
                __FUNCTION__, sd->version));
            break;
    }

    sd->caps = sdstd_rreg(sd, SD_Capabilities);    /* Cache this for later use */

    if (GFIELD(sd->caps, CAP_BASECLK) == 0) {
        /* XXX: base clock freq is 0, gets the HT clock as the base clock */
        uint32 ht_clk = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_BACKPLANE );
        sd->caps |= (ht_clk / 1000000) << 8;
        sd_err(("CAP_BASECLK = 0, force to 0x%02X\n", GFIELD(sd->caps, CAP_BASECLK)));
    }

    /* Get base clock in MHz */
    baseclk = GFIELD(sd->caps, CAP_BASECLK);
    /* Calculate default divisor number */
    sd_divisor = DIV_ROUND_UP(baseclk, (sd_hiok == TRUE ? 50 : 25));

    /* MSB 32 bits of caps supported in sdio 3.0 */
    sd->caps3 = sdstd_rreg(sd, SD_Capabilities3); /* Cache this for later use */
    sd3_trace(("sd3: %s: caps: 0x%x; MCCap:0x%x\n", __FUNCTION__, sd->caps, sd->curr_caps));
    sd3_trace(("sd3: %s: caps3: 0x%x\n", __FUNCTION__, sd->caps3));
    sd->curr_caps = sdstd_rreg(sd, SD_MaxCurCap);

    sd_info(("%s: caps: 0x%x; MCCap:0x%x\n", __FUNCTION__, sd->caps, sd->curr_caps));

    sdstd_set_dma_mode(sd, sd->sd_dma_mode);

#if defined(BCMINTERNAL)
    if (OSL_PCI_READ_CONFIG(sd->osh, PCI_CFG_VID, 4) ==
        ((SDIOH_FPGA_ID << 16) | VENDOR_BROADCOM)) {
            sd_err(("* * * SDIO20H FPGA Build Date: 0x%04x\n", sdstd_rreg(sd, 0x110)));
    }

    if (GFIELD(sd->caps, CAP_MAXBLOCK) == 0x3) {
        sd_info(("SD HOST CAPS: Max block size is INVALID\n"));
    } else {
        sd_info(("SD HOST CAPS: Max block size is %d bytes\n",
            512 << GFIELD(sd->caps, CAP_MAXBLOCK)));
    }

    sd_info(("SD HOST CAPS: 64-bit DMA is %ssupported.\n",
        GFIELD(sd->caps, CAP_64BIT_HOST) ? "" : "not "));
    sd_info(("SD HOST CAPS: Suspend/Resume is %ssupported.\n",
        GFIELD(sd->caps, CAP_SUSPEND) ? "" : "not "));

    sd_err(("SD HOST CAPS: SD Host supports "));
    if (GFIELD(sd->caps, CAP_VOLT_3_3)) {
        sd_err(("3.3V"));
        if (GFIELD(sd->curr_caps, CAP_CURR_3_3)) {
            sd_err(("@%dmA\n", 4*GFIELD(sd->curr_caps, CAP_CURR_3_3)));
        }
    }
    if (GFIELD(sd->caps, CAP_VOLT_3_0)) {
        sd_err((", 3.0V"));
        if (GFIELD(sd->curr_caps, CAP_CURR_3_0)) {
            sd_err(("@%dmA\n", 4*GFIELD(sd->curr_caps, CAP_CURR_3_0)));
        }
    }
    if (GFIELD(sd->caps, CAP_VOLT_1_8)) {
        sd_err((", 1.8V"));
        if (GFIELD(sd->curr_caps, CAP_CURR_1_8)) {
            sd_err(("@%dmA\n", 4*GFIELD(sd->curr_caps, CAP_CURR_1_8)));
        }
    }
    sd_err(("\n"));
#endif /* defined(BCMINTERNAL) */

#ifndef __IOPOS__
    sdstd_reset(sd, 1, 0);
#else
    sdstd_reset(sd, 1, 1);
#endif

    /* Read SD4/SD1 mode */
    if ((reg8 = sdstd_rreg8(sd, SD_HostCntrl))) {
        if (reg8 & SD4_MODE) {
            sd_err(("%s: Host cntrlr already in 4 bit mode: 0x%x\n",
                    __FUNCTION__,  reg8));
        }
    }

    /* Default power on mode is SD1 */
    sd->sd_mode = SDIOH_MODE_SD1;
    sd->polled_mode = TRUE;
    sd->host_init_done = TRUE;
    sd->card_init_done = FALSE;
    sd->adapter_slot = full_slot;

    /* XXX: If sd_uhsimode is disabled, which means that, use the HC in SDIO 2.0 mode. */
    if (sd_uhsimode == SD3CLKMODE_DISABLED) {
        //sd->version = HOST_CONTR_VER_2;
        sd3_trace(("%s:forcing to SDIO HC 2.0\n", __FUNCTION__));
    }

    if (sd->version == HOST_CONTR_VER_3 && sd_uhsimode != SD3CLKMODE_DISABLED) {
        /* read host ctrl 2 */
        uint16 reg16 = 0;
        sd3_trace(("sd3: %s: HC3: reading additional regs\n", __FUNCTION__));

        reg16 = sdstd_rreg16(sd, SD3_HostCntrl2);

        sd_info(("%s: HCtrl: 0x%x; HCtrl2:0x%x\n", __FUNCTION__, reg8, reg16));
        BCM_REFERENCE(reg16);

        /* if HC supports 1.8V and one of the SDR/DDR modes, hc uhci support is PRESENT */
        if ((GFIELD(sd->caps, CAP_VOLT_1_8)) &&
            (GFIELD(sd->caps3, CAP3_SDR50_SUP) ||
            GFIELD(sd->caps3, CAP3_SDR104_SUP) ||
            GFIELD(sd->caps3, CAP3_DDR50_SUP)))
            sd->host_UHSISupported = 1;
    }

    return (SUCCESS);
}
#define CMD5_RETRIES 200
static int
get_ocr(sdioh_info_t *sd, uint32 *cmd_arg, uint32 *cmd_rsp)
{
    int retries, status;

    /* Get the Card's Operation Condition.  Occasionally the board
     * takes a while to become ready
     */
    retries = CMD5_RETRIES;
    do {
        *cmd_rsp = 0;
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_5, *cmd_arg))
            != SUCCESS) {
            sd_err(("%s: CMD5 failed\n", __FUNCTION__));
            return status;
        }
        sdstd_cmd_getrsp(sd, cmd_rsp, 1);
        if (!GFIELD(*cmd_rsp, RSP4_CARD_READY))
            sd_trace(("%s: Waiting for card to become ready\n", __FUNCTION__));
    } while ((!GFIELD(*cmd_rsp, RSP4_CARD_READY)) && --retries);
    if (!retries)
        return ERROR;

    return (SUCCESS);
}

int
sdstd_client_init(sdioh_info_t *sd)
{
    uint32 cmd_arg, cmd_rsp;
    int status;
    uint8 fn_ints;
    uint32 regdata;
    uint16 powerstat = 0;

#ifdef BCMINTERNAL
#ifdef NOTUSED
    /* Handy routine to dump capabilities. */
    static char caps_buf[500];
    parse_caps(sd->caps, caps_buf, 500);
    sd_err((caps_buf));
#endif /* NOTUSED */
#endif /* BCMINTERNAL */

    sd_trace(("%s: Powering up slot %d\n", __FUNCTION__, sd->adapter_slot));

    /* Clear any pending ints */
    sdstd_wreg16(sd, SD_IntrStatus, 0x1fff);
    sdstd_wreg16(sd, SD_ErrorIntrStatus, 0x0fff);

    /* Enable both Normal and Error Status.  This does not enable
     * interrupts, it only enables the status bits to
     * become 'live'
     */

    if (!sd->host_UHSISupported)
        sdstd_wreg16(sd, SD_IntrStatusEnable, 0x1ff);
    else
    {
        /* INT_x interrupts, but DO NOT enable signalling [enable retuning
         * will happen later]
         */
        sdstd_wreg16(sd, SD_IntrStatusEnable, 0x0fff);
    }
    sdstd_wreg16(sd, SD_ErrorIntrStatusEnable, 0xffff /*& ~ERRINT_DATA_CRC_BIT*/);

    sdstd_wreg16(sd, SD_IntrSignalEnable, CLIENT_INTR);      /* Enable card interrupt. */

    if (sd->host_UHSISupported) {
        /* when HC is started for SDIO 3.0 mode, start in lowest voltage mode first. */
        powerstat = sdstd_start_power(sd, 1);
        if (SDIO_OCR_READ_FAIL == powerstat) {
            /* This could be because the device is 3.3V, and possible does
             * not have sdio3.0 support. So, try in highest voltage
             */
            sd_err(("sdstd_start_power: legacy device: trying highest voltage\n"));
            sd_err(("%s failed\n", __FUNCTION__));
            return ERROR;
        } else if (TRUE != powerstat) {
            sd_err(("sdstd_start_power failed\n"));
            return ERROR;
        }
    } else
    /* XXX legacy driver: start in highest voltage mode first.
     * CAUTION: trying to start a legacy dhd with sdio3.0HC and sdio3.0 device could
     * burn the sdio3.0device if the device has started in 1.8V.
     */
    if (TRUE != sdstd_start_power(sd, 0)) {
        sd_err(("sdstd_start_power failed\n"));
        return ERROR;
    }

    if (sd->num_funcs == 0) {
        sd_err(("%s: No IO funcs!\n", __FUNCTION__));
        return ERROR;
    }

    /* In SPI mode, issue CMD0 first */
    if (sd->sd_mode == SDIOH_MODE_SPI) {
        cmd_arg = 0;
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_0, cmd_arg))
            != SUCCESS) {
            sd_err(("BCMSDIOH: cardinit: CMD0 failed!\n"));
            return status;
        }
    }

    if (sd->sd_mode != SDIOH_MODE_SPI) {
        uint16 rsp6_status;

        /* Card is operational. Ask it to send an RCA */
        cmd_arg = 0;
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_3, cmd_arg))
            != SUCCESS) {
            sd_err(("%s: CMD3 failed!\n", __FUNCTION__));
            return status;
        }

        /* Verify the card status returned with the cmd response */
        sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
        rsp6_status = GFIELD(cmd_rsp, RSP6_STATUS);
        if (GFIELD(rsp6_status, RSP6STAT_COM_CRC_ERROR) ||
            GFIELD(rsp6_status, RSP6STAT_ILLEGAL_CMD) ||
            GFIELD(rsp6_status, RSP6STAT_ERROR)) {
            sd_err(("%s: CMD3 response error. Response = 0x%x!\n",
                    __FUNCTION__, rsp6_status));
            return ERROR;
        }

        /* Save the Card's RCA */
        sd->card_rca = GFIELD(cmd_rsp, RSP6_IO_RCA);
        sd_info(("RCA is 0x%x\n", sd->card_rca));

        if (rsp6_status)
            sd_err(("raw status is 0x%x\n", rsp6_status));

        /* Select the card */
        cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_7, cmd_arg))
            != SUCCESS) {
            sd_err(("%s: CMD7 failed!\n", __FUNCTION__));
            return status;
        }
        sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
        if (cmd_rsp != SDIOH_CMD7_EXP_STATUS) {
            sd_err(("%s: CMD7 response error. Response = 0x%x!\n",
                    __FUNCTION__, cmd_rsp));
            return ERROR;
        }
    }

    /* Disable default/power-up device Card Detect (CD) pull up resistor on DAT3
     * via CCCR bus interface control register.  Set CD disable bit while leaving
     * others alone.
     */
    if (sdstd_card_regread (sd, 0, SDIOD_CCCR_BICTRL, 1, &regdata) != SUCCESS) {
        sd_err(("Disabling card detect: read of device CCCR BICTRL register failed\n"));
        return ERROR;
    }
    regdata |= BUS_CARD_DETECT_DIS;

    if (sdstd_card_regwrite (sd, 0, SDIOD_CCCR_BICTRL, 1, regdata) != SUCCESS) {
        sd_err(("Disabling card detect: write of device CCCR BICTRL register failed\n"));
        return ERROR;
    }

    sdstd_card_enablefuncs(sd);

    if (!sdstd_bus_width(sd, sd_sdmode)) {
        sd_err(("sdstd_bus_width failed\n"));
        return ERROR;
    }

    set_client_block_size(sd, 1, BLOCK_SIZE_4318);
    fn_ints = INTR_CTL_FUNC1_EN;

    if (sd->num_funcs >= 2) {
        /* XXX Device side can't handle 512 yet */
        set_client_block_size(sd, 2, sd_f2_blocksize /* BLOCK_SIZE_4328 */);
        fn_ints |= INTR_CTL_FUNC2_EN;
    }

    /* Enable/Disable Client interrupts */
    /* Turn on here but disable at host controller? */
    if (sdstd_card_regwrite(sd, 0, SDIOD_CCCR_INTEN, 1,
                            (fn_ints | INTR_CTL_MASTER_EN)) != SUCCESS) {
        sd_err(("%s: Could not enable ints in CCCR\n", __FUNCTION__));
        return ERROR;
    }

    if (sd_uhsimode != SD3CLKMODE_DISABLED) {
    /* Switch to High-speed clocking mode if both host and device support it */
        if (sdstd_3_clock_wrapper(sd) != SUCCESS) {
                sd_err(("sdstd_3_clock_wrapper failed\n"));
                return ERROR;
        }
    } else
    {
        sd_err(("%s:SDHOST3-legacy\n", __FUNCTION__));
        if (sdstd_clock_wrapper(sd)) {
            sd_err(("sdstd_start_clock failed\n"));
            return ERROR;
        }
    }
    sd->card_init_done = TRUE;

    return SUCCESS;
}

static int
sdstd_clock_wrapper(sdioh_info_t *sd)
{
    sd_trace(("%s:Enter\n", __FUNCTION__));
    /* After configuring for High-Speed mode, set the desired clock rate. */
    sdstd_set_highspeed_mode(sd, (bool)sd_hiok);

    if (FALSE == sdstd_start_clock(sd, (uint16)sd_divisor)) {
        sd_err(("sdstd_start_clock failed\n"));
        return ERROR;
    }
    return SUCCESS;
}

static int
sdstd_3_clock_wrapper(sdioh_info_t *sd)
{
    int retclk = 0;
    sd_info(("%s: Enter\n", __FUNCTION__));
    if (sd->card_UHSI_voltage_Supported) {
        /* check if clk config requested is supported by both host and target. */
        retclk = sdstd_3_get_matching_uhsi_clkmode(sd, sd_uhsimode);

        /* if no match for requested caps, try to get the max match possible */
        if (retclk == -1) {
            /* if auto enabled */
            if (sd3_autoselect_uhsi_max == 1) {
                retclk = sdstd_3_get_matching_uhsi_clkmode(sd, SD3CLKMODE_AUTO);
                /* still NO match */
                if (retclk == -1) {
                    /* NO match with HC and card capabilities. Now try the
                     * High speed/legacy mode if possible.
                     */

                    sd_err(("%s: Not able to set requested clock\n",
                            __FUNCTION__));
                    return ERROR;
                }
            } else {
                /* means user doesn't want auto clock. So return ERROR */
                sd_err(("%s: Not able to set requested clock, Try"
                "auto mode\n", __FUNCTION__));
                return ERROR;
            }
        }

        if (retclk != -1) {
            /* set the current clk to be selected clock */
            sd_uhsimode = retclk;

            if (BCME_OK != sdstd_3_set_highspeed_uhsi_mode(sd, sd_uhsimode)) {
                sd_err(("%s: Not able to set requested clock\n", __FUNCTION__));
                return ERROR;
            }
        } else {
            /* try legacy mode */
            if (SUCCESS != sdstd_clock_wrapper(sd)) {
                sd_err(("sdstd_start_clock failed\n"));
                return ERROR;
            }
        }
    } else {
        sd_info(("%s: Legacy Mode Clock\n", __FUNCTION__));
        /* try legacy mode */
        if (SUCCESS != sdstd_clock_wrapper(sd)) {
            sd_err(("%s sdstd_clock_wrapper failed\n", __FUNCTION__));
            return ERROR;
        }
    }
    return SUCCESS;
}

int
sdstd_3_clk_tuning(sdioh_info_t *sd, uint32 sd3ClkMode)
{
    int status, lcount = 0, brr_count = 0;
    uint16 val1 = 0, bufready = 0;
    uint32 val2 = 0;
    uint8 phase_info_local = 0;

    sd3_trace(("sd3: %s: Enter\n", __FUNCTION__));
    /* if (NOT SDR104) OR
     *    (SDR_50 AND sdr50_tuning_reqd is NOT enabled)
     * return success, as tuning not reqd.
     */
    if (!sd->sd3_tuning_reqd) {
        sd_info(("%s: Tuning NOT reqd!\n", __FUNCTION__));
        return SUCCESS;
    }

    /* execute tuning procedure */

    /* enable Buffer ready  status. [donot enable the interrupt right now] */
    /* Execute tuning */
    sd_trace(("sd3: %s: Execute tuning\n", __FUNCTION__));
    val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
    val1 = SFIELD(val1, HOSTCtrl2_EXEC_TUNING, 1);
    sdstd_wreg16(sd, SD3_HostCntrl2, val1);

    do {
        sd3_trace(("sd3: %s: cmd19 issue\n", __FUNCTION__));
        /* Issue cmd19 */
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_19, 0))
            != SUCCESS) {
            sd_err(("%s: CMD19 failed\n", __FUNCTION__));
            val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
            val1 = SFIELD(val1, HOSTCtrl2_EXEC_TUNING, 0);
            sdstd_wreg16(sd, SD3_HostCntrl2, val1);
            return status;
        }

#if 0 /*  do we need to check response for CMD19?         */
        /* check response */
        sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
#endif

        /* wait for buffer read ready */
        brr_count = 0;
        do {
            bufready = sdstd_rreg16(sd, SD_IntrStatus);

            if (GFIELD(bufready, INTSTAT_BUF_READ_READY))
                break;

            /* delay after checking bufready becuase INTSTAT_BUF_READ_READY
               might have been most likely set already in the first check
            */
            OSL_DELAY(1);
        } while (++brr_count < CLKTUNING_MAX_BRR_RETRIES);

        /* buffer read ready timedout */
        if (brr_count == CLKTUNING_MAX_BRR_RETRIES) {
            sd_err(("%s: TUNINGFAILED: BRR response timedout!\n",
                __FUNCTION__));
            val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
            val1 = SFIELD(val1, HOSTCtrl2_EXEC_TUNING, 0);
            sdstd_wreg16(sd, SD3_HostCntrl2, val1);
            return ERROR;
        }

        /* In response to CMD19 card will send 64 magic bytes.
         * Current Aizyc HC h/w doesn't auto clear those bytes.
         * So read 64 bytes send by card.
         * Aizyc need to implement in hw to do an auto clear.
        */
        if (sd3_sw_read_magic_bytes == TRUE)
        {
            uint8 l_cnt_1 = 0;
            uint32 l_val_1 = 0;
            for (l_cnt_1 = 0; l_cnt_1 < 16; l_cnt_1++) {
                l_val_1 = sdstd_rreg(sd, SD_BufferDataPort0);
                sd_trace(("%s:l_val_1 = 0x%x", __FUNCTION__, l_val_1));
            }
            BCM_REFERENCE(l_val_1);
        }

        /* clear BuffReadReady int */
        bufready = SFIELD(bufready, INTSTAT_BUF_READ_READY, 1);
        sdstd_wreg16(sd, SD_IntrStatus, bufready);

        /* wait before continuing */
        /* OSL_DELAY(PER_TRY_TUNING_DELAY_MS * 1000); */ /* Not required */

        /* check execute tuning bit */
        val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
        if (!GFIELD(val1, HOSTCtrl2_EXEC_TUNING)) {
            /* done tuning, break from loop */
            break;
        }

        /* max tuning iterations exceeded */
        if (lcount++ > MAX_TUNING_ITERS) {
            sd_err(("%s: TUNINGFAILED: Max tuning iterations"
                "exceeded!\n", __FUNCTION__));
            val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
            val1 = SFIELD(val1, HOSTCtrl2_EXEC_TUNING, 0);
            sdstd_wreg16(sd, SD3_HostCntrl2, val1);
            return ERROR;
        }
    } while (1);

    val2 = sdstd_rreg(sd, SD3_Tuning_Info_Register);
    phase_info_local = ((val2>>15)& 0x7);
    sd_info(("Phase passed info: 0x%x\n", (val2>>8)& 0x3F));
    sd_info(("Phase selected post tune: 0x%x\n", phase_info_local));

    if (phase_info_local > SDSTD_MAX_TUNING_PHASE) {
        sd_err(("!!Phase selected:%x\n", phase_info_local));
    }

    /* check sampling clk select */
    val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
    if (!GFIELD(val1, HOSTCtrl2_SAMPCLK_SEL)) {
        /* error in selecting clk */
        sd_err(("%s: TUNINGFAILED: SamplClkSel failed!\n", __FUNCTION__));
        val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
        val1 = SFIELD(val1, HOSTCtrl2_EXEC_TUNING, 0);
        sdstd_wreg16(sd, SD3_HostCntrl2, val1);
        return ERROR;
    }
/* done: */
    sd_info(("%s: TUNING Success!\n", __FUNCTION__));
    return SUCCESS;
}

void
sdstd_3_enable_retuning_int(sdioh_info_t *sd)
{
    uint16 raw_int;
    unsigned long flags;

    sdstd_os_lock_irqsave(sd, &flags);
    raw_int = sdstd_rreg16(sd, SD_IntrSignalEnable);
    sdstd_wreg16(sd, SD_IntrSignalEnable, (raw_int | HC_INTR_RETUNING));
    /* Enable retuning status */
    raw_int = sdstd_rreg16(sd, SD_IntrStatusEnable);
    sdstd_wreg16(sd, SD_IntrStatusEnable, (raw_int | HC_INTR_RETUNING));
    sdstd_os_unlock_irqrestore(sd, &flags);
}

void
sdstd_3_disable_retuning_int(sdioh_info_t *sd)
{
    uint16 raw_int;
    unsigned long flags;

    sdstd_os_lock_irqsave(sd, &flags);
    sd->intmask &= ~HC_INTR_RETUNING;
    raw_int = sdstd_rreg16(sd, SD_IntrSignalEnable);
    sdstd_wreg16(sd, SD_IntrSignalEnable, (raw_int & (~HC_INTR_RETUNING)));
    /* Disable retuning status */
    raw_int = sdstd_rreg16(sd, SD_IntrStatusEnable);
    sdstd_wreg16(sd, SD_IntrStatusEnable, (raw_int & (~HC_INTR_RETUNING)));
    sdstd_os_unlock_irqrestore(sd, &flags);
}

bool
sdstd_3_is_retuning_int_set(sdioh_info_t *sd)
{
    uint16 raw_int;

    raw_int = sdstd_rreg16(sd, SD_IntrStatus);

    if (GFIELD(raw_int, INTSTAT_RETUNING_INT))
        return TRUE;

    return FALSE;
}

/*
    Assumption: sd3ClkMode is checked to be present in both host/card
    capabilities before entering this function. VALID values for sd3ClkMode
    in this function: SD3CLKMODE_2, 3, 4 [0 and 1 NOT supported as
    they are legacy] For that, need to call
    sdstd_3_get_matching_uhsi_clkmode()
*/
static int
sdstd_3_set_highspeed_uhsi_mode(sdioh_info_t *sd, int sd3ClkMode)
{
    uint32 drvstrn;
    int status;
    uint8 hc_reg8;
    uint16 val1 = 0, presetval = 0;
    uint32 regdata;

    sd3_trace(("sd3: %s:enter:clkmode:%d\n", __FUNCTION__, sd3ClkMode));

    hc_reg8 = sdstd_rreg8(sd, SD_HostCntrl);

    if (HOST_SDR_UNSUPP == sd->global_UHSI_Supp) {
        sd_err(("%s:Trying to set clk with unsupported global support\n", __FUNCTION__));
        return BCME_ERROR;
    }

    /* get [double check, as this is already done in
        sdstd_3_get_matching_uhsi_clkmode] drvstrn
    */
    if (!sdstd_3_get_matching_drvstrn(sd, sd3ClkMode, &drvstrn, &presetval)) {
        sd_err(("%s:DRVStrn mismatch!: card strn:0x%x; HC preset"
            "val:0x%x\n", __FUNCTION__, drvstrn, presetval));
        return BCME_SDIO_ERROR;
    }

    /* also set driver type select in CCCR */
    if ((status = sdstd_card_regwrite(sd, 0, SDIOD_CCCR_DRIVER_STRENGTH,
        1, drvstrn)) != BCME_OK) {
        sd_err(("%s:Setting SDIOD_CCCR_DRIVER_STRENGTH in card Failed!\n", __FUNCTION__));
        return BCME_SDIO_ERROR;
    }

    /* Power override: BCM43342 for now */
    if ((status = sdstd_3_power_override(sd)) != BCME_OK) {
        sd_err(("%s:Power Override failed - status: %d\n", __FUNCTION__, status));
        return BCME_SDIO_ERROR;
    }

    /* ********** change Bus speed select in device */
    if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                     1, &regdata)) != SUCCESS) {
        sd_err(("%s:FAILED 1\n", __FUNCTION__));
        return BCME_SDIO_ERROR;
    }
    sd_info(("Attempting to change BSS.current val:0x%x\n", regdata));

    if (regdata & SDIO_SPEED_SHS) {
        sd_info(("Device supports High-Speed mode.\n"));
        /* clear existing BSS */
        regdata &= ~0xE;

        regdata |= (sd3ClkMode << 1);

        sd_info(("Writing %08x to Card at %08lx\n",
                 regdata, SDIOD_CCCR_SPEED_CONTROL));
        if ((status = sdstd_card_regwrite(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                          1, regdata)) != BCME_OK) {
            sd_err(("%s:FAILED 2\n", __FUNCTION__));
            return BCME_SDIO_ERROR;
        }

        if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                         1, &regdata)) != BCME_OK) {
            sd_err(("%s:FAILED 3\n", __FUNCTION__));
            return BCME_SDIO_ERROR;
        }

        sd_info(("Read %08x from Card at %08lx\n", regdata, SDIOD_CCCR_SPEED_CONTROL));
    }
    else {
        sd_err(("Device does not support High-Speed Mode.\n"));
    }

    /* SD Clock Enable = 0 */
    sdstd_wreg16(sd, SD_ClockCntrl,
        sdstd_rreg16(sd, SD_ClockCntrl) & ~((uint16)0x4));

    /* set to HighSpeed mode */
    /* TBD: is these to change SD_HostCntrl reqd for UHSI? */
    hc_reg8 = SFIELD(hc_reg8, HOST_HI_SPEED_EN, 1);
    sdstd_wreg8(sd, SD_HostCntrl, hc_reg8);

    /* set UHS Mode select in HC2 and also set preset */
    val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
    val1 = SFIELD(val1, HOSTCtrl2_UHSMODE_SEL, sd3ClkMode);
    if (TRUE != sd3_sw_override1) {
        val1 = SFIELD(val1, HOSTCtrl2_PRESVAL_EN, 1);
    } else {
        /* set hC registers manually using the retreived values */
        /* *set drvstrn */
        val1 = SFIELD(val1, HOSTCtrl2_DRIVSTRENGTH_SEL,
            GFIELD(presetval, PRESET_DRIVR_SELECT));
        val1 = SFIELD(val1, HOSTCtrl2_PRESVAL_EN, 0);
    }

    /* finally write Hcontrol2 */
    sdstd_wreg16(sd, SD3_HostCntrl2, val1);

    sd_err(("%s:HostCtrl2 final value:0x%x\n", __FUNCTION__, val1));

    /* start clock : clk will be enabled inside. */
    if (FALSE == sdstd_start_clock(sd, GFIELD(presetval, PRESET_CLK_DIV))) {
        sd_err(("sdstd_start_clock failed\n"));
        return ERROR;
    }

    /* execute first tuning procedure */
    if (!sd3_sw_override1) {
        if (SD3_TUNING_REQD(sd, sd3ClkMode)) {
            sd_err(("%s: Tuning start..\n", __FUNCTION__));
            sd->sd3_tuning_reqd = TRUE;
            /* TBD: first time: enabling INT's could be problem? */
            sdstd_3_start_tuning(sd);
        }
        else
            sd->sd3_tuning_reqd = FALSE;
    }

    return BCME_OK;
}

/* Check & do tuning if required */
void sdstd_3_check_and_do_tuning(sdioh_info_t *sd, int tuning_param)
{
    int retries = 0;

    if (sd->sd3_tuning_reqd) {
        sd3_trace(("sd3: %s: tuning reqd\n", __FUNCTION__));
            if (tuning_param == CHECK_TUNING_PRE_DATA) {
            if (sd->sd3_tun_state == TUNING_ONGOING) {
                retries = RETRIES_SMALL;
                /* check if tuning is already going on */
                while ((GFIELD(sdstd_rreg(sd, SD3_HostCntrl2),
                    HOSTCtrl2_EXEC_TUNING)) && retries--) {
                    if (retries == RETRIES_SMALL)
                        sd_err(("%s: Waiting for Tuning to complete\n",
                        __FUNCTION__));
                }

                if (!retries) {
                    sd_err(("%s: Tuning wait timeout\n", __FUNCTION__));
                    if (trap_errs)
                        ASSERT(0);
                }
            } else if (sd->sd3_tun_state == TUNING_START) {
                /* check and start tuning if required. */
                sd3_trace(("sd3 : %s : Doing Tuning before Data Transfer\n",
                    __FUNCTION__));
                sdstd_3_start_tuning(sd);
            }
        } else if (tuning_param == CHECK_TUNING_POST_DATA) {
            if (sd->sd3_tun_state == TUNING_START_AFTER_DAT) {
                sd3_trace(("sd3: %s: tuning start\n", __FUNCTION__));
                /* check and start tuning if required. */
                sdstd_3_start_tuning(sd);
            }
        }
    }
}
/* Need to run this function in interrupt-disabled context */
bool sdstd_3_check_and_set_retuning(sdioh_info_t *sd)
{
    sd3_trace(("sd3: %s:\n", __FUNCTION__));

    /* if already initiated, just return without anything */
    if ((sd->sd3_tun_state == TUNING_START) ||
        (sd->sd3_tun_state == TUNING_ONGOING) ||
        (sd->sd3_tun_state == TUNING_START_AFTER_DAT)) {
        /* do nothing */
        return FALSE;
    }

    if (sd->sd3_dat_state == DATA_TRANSFER_IDLE) {
        sd->sd3_tun_state = TUNING_START; /* tuning to be started by the tasklet */
        return TRUE;
    } else {
        /* tuning to be started after finishing the existing data transfer */
        sd->sd3_tun_state = TUNING_START_AFTER_DAT;
    }
    return FALSE;
}

int sdstd_3_get_data_state(sdioh_info_t *sd)
{
    return sd->sd3_dat_state;
}

void sdstd_3_set_data_state(sdioh_info_t *sd, int state)
{
    sd->sd3_dat_state = state;
}

int sdstd_3_get_tune_state(sdioh_info_t *sd)
{
    return sd->sd3_tun_state;
}

void sdstd_3_set_tune_state(sdioh_info_t *sd, int state)
{
    sd->sd3_tun_state = state;
}

uint8 sdstd_3_get_tuning_exp(sdioh_info_t *sd)
{
    if (sd_tuning_period == 0)
        return GFIELD(sd->caps3, CAP3_RETUNING_TC);
    else
        return (uint8)sd_tuning_period;
}

uint32 sdstd_3_get_uhsi_clkmode(sdioh_info_t *sd)
{
    return sd_uhsimode;
}

/* check, to see if the card supports driver_type corr to the driver_type
    in preset value, which will be selected by requested UHSI mode
    input:
    clk mode: valid values: SD3CLKMODE_2_SDR50, SD3CLKMODE_3_SDR104,
            SD3CLKMODE_4_DDR50, SD3CLKMODE_AUTO
    outputs:
    return_val:     TRUE; if a matching drvstrn for the given clkmode is
        found in both HC and card. otherwise, FALSE.
        [other outputs below valid ONLY if return_val is TRUE]
    drvstrn    :      driver strength read from CCCR.
    presetval:     value of preset reg, corr to the clkmode.
 */
static bool
sdstd_3_get_matching_drvstrn(sdioh_info_t *sd, int sd3_requested_clkmode,
    uint32 *drvstrn, uint16 *presetval)
{
    int status;
    uint8 presetreg;
    uint8 cccr_reqd_dtype_mask = 1;

    sd3_trace(("sd3: %s:\n", __FUNCTION__));

    if (sd3_requested_clkmode != SD3CLKMODE_AUTO) {
        /* CARD: get the card driver strength from cccr */
        if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_DRIVER_STRENGTH,
            1, drvstrn)) != BCME_OK) {
            sd_err(("%s:Reading SDIOD_CCCR_DRIVER_STRENGTH from card"
                "Failed!\n", __FUNCTION__));
            return FALSE;
        }
        if (TRUE != sd3_sw_override1) {
        /* HOSTC: get the addr of preset register indexed by the clkmode */
        presetreg = SD3_PresetValStart +
            (2*sd3_requested_clkmode + 6);
        *presetval = sdstd_rreg16(sd, presetreg);
        } else {
            /* Note: +3 for mapping between SD3CLKMODE_xxx and presetval_sw_table */
            *presetval = presetval_sw_table[sd3_requested_clkmode + 3];
        }
        sd_err(("%s:reqCLK: %d, presetval: 0x%x\n",
            __FUNCTION__, sd3_requested_clkmode, *presetval));

        cccr_reqd_dtype_mask <<= GFIELD(*presetval, PRESET_DRIVR_SELECT);

        /* compare/match */
        if (!(cccr_reqd_dtype_mask & GFIELD(*drvstrn, SDIO_BUS_DRVR_TYPE_CAP))) {
            sd_err(("%s:cccr_reqd_dtype_mask and SDIO_BUS_DRVR_TYPE_CAP"
                "not matching!:reqd:0x%x, cap:0x%x\n", __FUNCTION__,
                cccr_reqd_dtype_mask, GFIELD(*drvstrn, SDIO_BUS_DRVR_TYPE_CAP)));
            return FALSE;
        } else {
            /* check if drive strength override is required. If so, first setit */
            if (*dhd_sdiod_uhsi_ds_override != DRVSTRN_IGNORE_CHAR) {
                int ds_offset = 0;
                uint32 temp = 0;

                /* drvstrn to reflect the preset val: this is default */
                *drvstrn = GFIELD(*presetval, PRESET_DRIVR_SELECT);

                /* now check override */
                ds_offset = (((int)DRVSTRN_MAX_CHAR -
                    (int)(*dhd_sdiod_uhsi_ds_override)));
                if ((ds_offset >= 0) && (ds_offset <= MAX_DTS_INDEX)) {
                    ds_offset = MAX_DTS_INDEX - ds_offset;
                    sd_err(("%s:Drive strength override: %c, offset: "
                        "%d, val: %d\n", __FUNCTION__,
                        *dhd_sdiod_uhsi_ds_override,
                        ds_offset, DTS_vals[ds_offset]));
                    temp = SFIELD(*drvstrn, SDIO_BUS_DRVR_TYPE_SEL,
                        DTS_vals[ds_offset]);
                    sd_err(("%s:DrvStrn orig: 0x%x, modif: 0x%x\n",
                        __FUNCTION__, *drvstrn, temp));
                    *drvstrn = temp;
                } else {
                    /* else case is default: use preset val */
                    sd_err(("%s:override invalid: DrvStrn is from "
                        "preset: 0x%x\n",
                        __FUNCTION__, *drvstrn));
                }
            } else {
                sd_err(("%s:DrvStrn is from preset: 0x%x\n",
                    __FUNCTION__, *drvstrn));
            }
        }
    } else {
        /* TBD check for sd3_requested_clkmode : -1 also.  */
        sd_err(("%s: Automode not supported!\n", __FUNCTION__));
        return FALSE;
    }
    return TRUE;
}

/* Returns a matching UHSI clk speed is found. If not, returns -1.
    Also, if  sd3_requested_clkmode is -1, finds the closest max match clk and returns.
 */
static int
sdstd_3_get_matching_uhsi_clkmode(sdioh_info_t *sd, int sd3_requested_clkmode)
{
    uint32 card_val_uhsisupp;
    uint8 speedmask = 1;
    uint32 drvstrn;
    uint16 presetval = 0;
    int status;

    sd3_trace(("sd3: %s:\n", __FUNCTION__));

    sd->global_UHSI_Supp = HOST_SDR_UNSUPP;

    /* for legacy/25MHz/50MHz bus speeds, no checks done here */
    if ((sd3_requested_clkmode == SD3CLKMODE_0_SDR12) ||
        (sd3_requested_clkmode == SD3CLKMODE_1_SDR25)) {
        sd->global_UHSI_Supp = HOST_SDR_12_25;
        return sd3_requested_clkmode;
    }
    /* get cap of card */
    if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_UHSI_SUPPORT,
                                     1, &card_val_uhsisupp)) != BCME_OK) {
        sd_err(("%s:SDIOD_CCCR_UHSI_SUPPORT query failed!\n", __FUNCTION__));
        return -1;
    }
    sd_info(("%s:Read %08x from Card at %08lx\n", __FUNCTION__,
        card_val_uhsisupp, SDIOD_CCCR_UHSI_SUPPORT));

    if (sd3_requested_clkmode != SD3CLKMODE_AUTO) {
        /* Note: it is assumed that, following are executed when (sd3ClkMode >= 2) */
        speedmask <<= (sd3_requested_clkmode - SD3CLKMODE_2_SDR50);

        /* check first about 3.0 HS CLK modes */
        if (!(GFIELD(sd->caps3, CAP3_30CLKCAP) & speedmask)) {
            sd_err(("%s:HC does not support req 3.0 UHSI mode."
                "requested:%d; capable:0x%x\n", __FUNCTION__,
                sd3_requested_clkmode, GFIELD(sd->caps3, CAP3_30CLKCAP)));
            return -1;
        }

        /* check first about 3.0 CARD CLK modes */
        if (!(GFIELD(card_val_uhsisupp, SDIO_BUS_SPEED_UHSICAP) & speedmask)) {
            sd_err(("%s:Card does not support req 3.0 UHSI mode. requested:%d;"
                "capable:0x%x\n", __FUNCTION__, sd3_requested_clkmode,
                GFIELD(card_val_uhsisupp, SDIO_BUS_SPEED_UHSICAP)));
            return -1;
        }

        /* check, to see if the card supports driver_type corr to the
            driver_type in preset value, which will be selected by
            requested UHSI mode
        */
        if (!sdstd_3_get_matching_drvstrn(sd, sd3_requested_clkmode,
            &drvstrn, &presetval)) {
            sd_err(("%s:DRVStrn mismatch!: card strn:0x%x; HC preset"
                "val:0x%x\n", __FUNCTION__, drvstrn, presetval));
            return -1;
        }
        /* success path. change the support variable accordingly */
        sd->global_UHSI_Supp = HOST_SDR_50_104_DDR;
        return sd3_requested_clkmode;
    } else {
        /* auto clk selection: get the highest clock capable by both card and HC */
/* TBD        TOBE DONE */
/*         sd->global_UHSI_Supp = TRUE; on success */
        return -1;
    }
}

static int
sdstd_3_sigvoltswitch_proc(sdioh_info_t *sd)
{
    int status;
    uint32 cmd_rsp = 0, presst;
    uint16 val1 = 0;

    sd3_trace(("sd3: %s:\n", __FUNCTION__));

    /* Issue cmd11 */
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_11, 0))
        != SUCCESS) {
        sd_err(("%s: CMD11 failed\n", __FUNCTION__));
        return status;
    }

    /* check response */
    sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
    if (
        GFIELD(cmd_rsp, RSP1_ERROR) ||    /*  bit 19 */
        GFIELD(cmd_rsp, RSP1_ILLEGAL_CMD) ||    /*  bit 22 */
        GFIELD(cmd_rsp, RSP1_COM_CRC_ERROR) ||    /*  bit 23 */
        GFIELD(cmd_rsp, RSP1_CARD_LOCKED)    /*  bit 25 */    ) {
        sd_err(("%s: FAIL:CMD11: cmd_resp:0x%x\n", __FUNCTION__, cmd_rsp));
        return ERROR;
    }

    /* SD Clock Enable = 0 */
    sdstd_wreg16(sd, SD_ClockCntrl,
                 sdstd_rreg16(sd, SD_ClockCntrl) & ~((uint16)0x4));

    /* check DAT[3..0] using Present State Reg. If not 0, error */
    presst = sdstd_rreg(sd, SD_PresentState);
    if (0 != GFIELD(presst, PRES_DAT_SIGNAL)) {
        sd_err(("%s: FAIL: PRESTT:0x%x\n", __FUNCTION__, presst));
        return ERROR;
    }

    /* turn 1.8V sig enable in HC2 */
    val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
    val1 = SFIELD(val1, HOSTCtrl2_1_8SIG_EN, 1);
    sdstd_wreg16(sd, SD3_HostCntrl2, val1);

    /* wait 5ms */
    OSL_DELAY(5000);

    /* check 1.8V sig enable in HC2. if cleared, error */
    val1 = sdstd_rreg16(sd, SD3_HostCntrl2);

    if (!GFIELD(val1, HOSTCtrl2_1_8SIG_EN)) {
        sd_err(("%s: FAIL: HC2:1.8V_En:0x%x\n", __FUNCTION__, val1));
        return ERROR;
    }

    /* SD Clock Enable = 1 */
    val1 = sdstd_rreg16(sd, SD_ClockCntrl);
    sdstd_wreg16(sd, SD_ClockCntrl, val1 | 0x4);

    /* wait 1ms */
    OSL_DELAY(1000);

    /* check DAT[3..0] using Present State Reg. If not 0b1111, error */
    presst = sdstd_rreg(sd, SD_PresentState);
    if (0xf != GFIELD(presst, PRES_DAT_SIGNAL)) {
        sd_err(("%s: FAIL: PRESTT_FINAL:0x%x\n", __FUNCTION__, presst));
        return ERROR;
    }

    return (SUCCESS);
}


static int
sdstd_3_power_override(sdioh_info_t *sd)
{
    int       status;
    uint32    data;

    status = sdstd_card_regread(sd, SDIO_FUNC_1, CHIP_CONTROL_BASE, 4, &data);
    if (BCME_OK != status) {
        sd_err(("%s:Accessing Chip ID failed - status: %d\n", __FUNCTION__, status));
        return    BCME_SDIO_ERROR;
    }

    /* Now it is ONLY for BCM43342 */
    /* XXX Use and can only use CMD52 to write at this stage -- size is set to 1 */
    if (BCM43342_CHIP_ID == (data & CHIP_CONTROL_CHIPID_MASK)) {
        status = sdstd_card_regwrite(sd, SDIO_FUNC_1, CHIP_CONTROL_BASE + CHIP_CONTROL_ADDR,
                                     1, CHIP_CONTROL_REG3);
        if (BCME_OK != status) {
            sd_err(("%s:Writing PMU Chip Control Register 3 failed - status: %d\n",
                __FUNCTION__, status));
            return    BCME_SDIO_ERROR;
        }

        status = sdstd_card_regread(sd, SDIO_FUNC_1, CHIP_CONTROL_BASE + CHIP_CONTROL_DATA,
                                    1, &data);
        if (BCME_OK != status) {
            sd_err(("%s:Accessing PMU Chip Control Register 3 failed - status: %d\n",
                __FUNCTION__, status));
            return    BCME_SDIO_ERROR;
        }

        data |= (CC3_VDDIO_SELECT_OVERRIDE | CC3_VDDIO_OVERRIDE_1_8V);
        status = sdstd_card_regwrite(sd, SDIO_FUNC_1, CHIP_CONTROL_BASE + CHIP_CONTROL_DATA,
                                     1, data);
        if (BCME_OK != status) {
            sd_err(("%s:Writing 0x%X to PMU Chip Control Reg 3 failed - status: %d\n",
                __FUNCTION__, data, status));
            return    BCME_SDIO_ERROR;
        }

        status = sdstd_card_regread(sd, SDIO_FUNC_1, CHIP_CONTROL_BASE + CHIP_CONTROL_DATA,
                                    1, &data);
        if (BCME_OK != status) {
            sd_err(("%s:Accessing PMU Chip Control Register 3 failed - status: %d\n",
                __FUNCTION__, status));
            return    BCME_SDIO_ERROR;
        }

        sd_err(("%s:PMU Chip Control Register 3: 0x%X\n", __FUNCTION__, data));
    }

    return    BCME_OK;
}


static int
sdstd_set_highspeed_mode(sdioh_info_t *sd, bool HSMode)
{
    uint32 regdata;
    int status;
    uint8 reg8;

    uint32 drvstrn;

    reg8 = sdstd_rreg8(sd, SD_HostCntrl);

#ifdef BCMINTERNAL
    /* The Jinvani SD Gold Host forces the highest clock rate in high-speed mode */
    /* Only enable high-speed mode if the SD clock divisor is 1. */
    if (sd->controller_type == SDIOH_TYPE_JINVANI_GOLD) {
        if (sd_divisor != 1) {
            HSMode = FALSE;
        }
    }
#endif /* BCMINTERNAL */

    if (HSMode == TRUE) {
        if (sd_hiok && (GFIELD(sd->caps, CAP_HIGHSPEED)) == 0) {
            sd_err(("Host Controller does not support hi-speed mode.\n"));
            return BCME_ERROR;
        }

        sd_info(("Attempting to enable High-Speed mode.\n"));

        if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                         1, &regdata)) != SUCCESS) {
            return BCME_SDIO_ERROR;
        }
        if (regdata & SDIO_SPEED_SHS) {
            sd_info(("Device supports High-Speed mode.\n"));

            regdata |= SDIO_SPEED_EHS;

            sd_info(("Writing %08x to Card at %08lx\n",
                     regdata, SDIOD_CCCR_SPEED_CONTROL));
            if ((status = sdstd_card_regwrite(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                              1, regdata)) != BCME_OK) {
                return BCME_SDIO_ERROR;
            }

            if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                             1, &regdata)) != BCME_OK) {
                return BCME_SDIO_ERROR;
            }

            sd_info(("Read %08x to Card at %08lx\n", regdata, SDIOD_CCCR_SPEED_CONTROL));

            reg8 = SFIELD(reg8, HOST_HI_SPEED_EN, 1);

            sd_err(("High-speed clocking mode enabled.\n"));
        }
        else {
            sd_err(("Device does not support High-Speed Mode.\n"));
            reg8 = SFIELD(reg8, HOST_HI_SPEED_EN, 0);
        }
    } else {
        /* Force off device bit */
        if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                         1, &regdata)) != BCME_OK) {
            return status;
        }
        if (regdata & SDIO_SPEED_EHS) {
            regdata &= ~SDIO_SPEED_EHS;
            if ((status = sdstd_card_regwrite(sd, 0, SDIOD_CCCR_SPEED_CONTROL,
                                              1, regdata)) != BCME_OK) {
                return status;
            }
        }

        sd_err(("High-speed clocking mode disabled.\n"));
        reg8 = SFIELD(reg8, HOST_HI_SPEED_EN, 0);
    }

    if ((sd->host_UHSISupported) && (sd->card_UHSI_voltage_Supported)) {
        /* also set the default driver strength in the card/HC [this is reqd because,
            if earlier we selected any other drv_strn, we need to reset it]
        */
        /* get the card driver strength from cccr */
        if ((status = sdstd_card_regread(sd, 0, SDIOD_CCCR_DRIVER_STRENGTH,
            1, &drvstrn)) != BCME_OK) {
            sd_err(("%s:Reading SDIOD_CCCR_DRIVER_STRENGTH from card"
                "Failed!\n", __FUNCTION__));
            return BCME_SDIO_ERROR;
        }

        /* reset card drv strn */
        drvstrn = SFIELD(drvstrn, SDIO_BUS_DRVR_TYPE_SEL, 0);

        /* set card drv strn */
        if ((status = sdstd_card_regwrite(sd, 0, SDIOD_CCCR_DRIVER_STRENGTH,
            1, drvstrn)) != BCME_OK) {
            sd_err(("%s:Setting SDIOD_CCCR_DRIVER_STRENGTH in"
                "card Failed!\n", __FUNCTION__));
            return BCME_SDIO_ERROR;
        }
    }

    sdstd_wreg8(sd, SD_HostCntrl, reg8);

    return BCME_OK;
}

/* Select DMA Mode:
 * If dma_mode == DMA_MODE_AUTO, pick the "best" mode.
 * Otherwise, pick the selected mode if supported.
 * If not supported, use PIO mode.
 */
static int
sdstd_set_dma_mode(sdioh_info_t *sd, int8 dma_mode)
{
    uint8 reg8, dma_sel_bits = SDIOH_SDMA_MODE;
    int8 prev_dma_mode = sd->sd_dma_mode;

    /* To pass unused-variable warning when debug macro sd_err() is empty */
    UNUSED_VARIABLE(dma_mode_description);

    switch (prev_dma_mode) {
        case DMA_MODE_AUTO:
            sd_dma(("%s: Selecting best DMA mode supported by controller.\n",
                      __FUNCTION__));
            if (GFIELD(sd->caps, CAP_ADMA2)) {
                sd->sd_dma_mode = DMA_MODE_ADMA2;
                dma_sel_bits = SDIOH_ADMA2_MODE;
            } else if (GFIELD(sd->caps, CAP_ADMA1)) {
                sd->sd_dma_mode = DMA_MODE_ADMA1;
                dma_sel_bits = SDIOH_ADMA1_MODE;
            } else if (GFIELD(sd->caps, CAP_DMA)) {
                sd->sd_dma_mode = DMA_MODE_SDMA;
            } else {
                sd->sd_dma_mode = DMA_MODE_NONE;
            }
            break;
        case DMA_MODE_NONE:
            sd->sd_dma_mode = DMA_MODE_NONE;
            break;
        case DMA_MODE_SDMA:
            if (GFIELD(sd->caps, CAP_DMA)) {
                sd->sd_dma_mode = DMA_MODE_SDMA;
            } else {
                sd_err(("%s: SDMA not supported by controller.\n", __FUNCTION__));
                sd->sd_dma_mode = DMA_MODE_NONE;
            }
            break;
        case DMA_MODE_ADMA1:
            if (GFIELD(sd->caps, CAP_ADMA1)) {
                sd->sd_dma_mode = DMA_MODE_ADMA1;
                dma_sel_bits = SDIOH_ADMA1_MODE;
            } else {
                sd_err(("%s: ADMA1 not supported by controller.\n", __FUNCTION__));
                sd->sd_dma_mode = DMA_MODE_NONE;
            }
            break;
        case DMA_MODE_ADMA2:
            if (GFIELD(sd->caps, CAP_ADMA2)) {
                sd->sd_dma_mode = DMA_MODE_ADMA2;
                dma_sel_bits = SDIOH_ADMA2_MODE;
            } else {
                sd_err(("%s: ADMA2 not supported by controller.\n", __FUNCTION__));
                sd->sd_dma_mode = DMA_MODE_NONE;
            }
            break;
        case DMA_MODE_ADMA2_64:
            sd_err(("%s: 64b ADMA2 not supported by driver.\n", __FUNCTION__));
            sd->sd_dma_mode = DMA_MODE_NONE;
            break;
        default:
            sd_err(("%s: Unsupported DMA Mode %d requested.\n", __FUNCTION__,
                    prev_dma_mode));
            sd->sd_dma_mode = DMA_MODE_NONE;
            break;
    }

    /* clear SysAddr, only used for SDMA */
    sdstd_wreg(sd, SD_SysAddr, 0);

    sd_err(("%s: %s mode selected.\n", __FUNCTION__, dma_mode_description[sd->sd_dma_mode]));

    reg8 = sdstd_rreg8(sd, SD_HostCntrl);
    reg8 = SFIELD(reg8, HOST_DMA_SEL, dma_sel_bits);
    sdstd_wreg8(sd, SD_HostCntrl, reg8);
    sd_dma(("%s: SD_HostCntrl=0x%02x\n", __FUNCTION__, reg8));

    return BCME_OK;
}

#ifdef BCMDBG
void
print_regs(sdioh_info_t *sd)
{
    uint8 reg8 = 0;
    uint16 reg16 = 0;
    uint32 reg32 = 0;
    uint8 presetreg;
    int i;

    reg8 = sdstd_rreg8(sd, SD_BlockSize);
    printf("REGS: SD_BlockSize [004h]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_BlockCount);
    printf("REGS: SD_BlockCount [006h]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_BlockSize);
    printf("REGS: SD_BlockSize [004h]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_TransferMode);
    printf("REGS: SD_TransferMode [00Ch]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_HostCntrl);
    printf("REGS: SD_HostCntrl [028h]:0x%x\n", reg8);

    reg32 = sdstd_rreg(sd, SD_PresentState);
    printf("REGS: SD_PresentState [024h]:0x%x\n", reg32);

    reg8 = sdstd_rreg8(sd, SD_PwrCntrl);
    printf("REGS: SD_PwrCntrl [029h]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_BlockGapCntrl);
    printf("REGS: SD_BlockGapCntrl [02Ah]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_WakeupCntrl);
    printf("REGS: SD_WakeupCntrl [02Bh]:0x%x\n", reg8);

    reg16 = sdstd_rreg16(sd, SD_ClockCntrl);
    printf("REGS: SD_ClockCntrl [02Ch]:0x%x\n", reg16);

    reg8 = sdstd_rreg8(sd, SD_TimeoutCntrl);
    printf("REGS: SD_TimeoutCntrl [02Eh]:0x%x\n", reg8);

    reg8 = sdstd_rreg8(sd, SD_SoftwareReset);
    printf("REGS: SD_SoftwareReset [02Fh]:0x%x\n", reg8);

    reg16 = sdstd_rreg16(sd, SD_IntrStatus);
    printf("REGS: SD_IntrStatus [030h]:0x%x\n", reg16);

    reg16 = sdstd_rreg16(sd, SD_ErrorIntrStatus);
    printf("REGS: SD_ErrorIntrStatus [032h]:0x%x\n", reg16);

    reg16 = sdstd_rreg16(sd, SD_IntrStatusEnable);
    printf("REGS: SD_IntrStatusEnable [034h]:0x%x\n", reg16);

    reg16 = sdstd_rreg16(sd, SD_ErrorIntrStatusEnable);
    printf("REGS: SD_ErrorIntrStatusEnable [036h]:0x%x\n", reg16);

    reg16 = sdstd_rreg16(sd, SD_IntrSignalEnable);
    printf("REGS: SD_IntrSignalEnable [038h]:0x%x\n", reg16);

    reg16 = sdstd_rreg16(sd, SD_ErrorIntrSignalEnable);
    printf("REGS: SD_ErrorIntrSignalEnable [03Ah]:0x%x\n", reg16);

    reg32 = sdstd_rreg(sd, SD_Capabilities);
    printf("REGS: SD_Capabilities [040h]:0x%x\n", reg32);

    reg32 = sdstd_rreg(sd, SD_MaxCurCap);
    printf("REGS: SD_MaxCurCap [04Ah]:0x%x\n", reg32);

    reg32 = sdstd_rreg(sd, SD_Capabilities3);
    printf("REGS: SD_Capabilities3 [044h]:0x%x\n", reg32);

    reg16 = sdstd_rreg16(sd, SD3_HostCntrl2);
    printf("REGS: SD3_HostCntrl2 [03Eh]:0x%x\n", reg16);

    for (i = 0; i < 8; i++) {
        presetreg = SD3_PresetValStart + i*2;
        printf("REGS: Presetvalreg:ix[%d]:0x%x, val=0x%x\n", i,
               presetreg, sdstd_rreg16(sd, presetreg));
    }

    reg16 = sdstd_rreg16(sd, SD_SlotInterruptStatus);
    printf("REGS: SD_SlotInterruptStatus [0FCh]:0x%x\n", reg16);

    reg16 = sdstd_rreg16(sd, SD_HostControllerVersion);
    printf("REGS: SD_HostControllerVersion [0FEh]:0x%x\n", reg16);
}
#endif /* BCMDBG */

#ifdef BCMINTERNAL
#ifdef NOTUSED
static int
parse_state(uint32 state, char *buf, int len)
{
    char *data = buf;

    sd_err(("Parsing state 0x%x\n", state));
    if (!len) {
        return (0);
    }

    data += sprintf(data, "cmd_inhibit      %d\n", GFIELD(state, PRES_CMD_INHIBIT));
    data += sprintf(data, "dat_inhibit      %d\n", GFIELD(state, PRES_DAT_INHIBIT));
    data += sprintf(data, "dat_busy            %d\n", GFIELD(state, PRES_DAT_BUSY));
    data += sprintf(data, "write_active     %d\n", GFIELD(state, PRES_WRITE_ACTIVE));
    data += sprintf(data, "read_active      %d\n", GFIELD(state, PRES_READ_ACTIVE));
    data += sprintf(data, "write_data_rdy   %d\n", GFIELD(state, PRES_WRITE_DATA_RDY));
    data += sprintf(data, "read_data_rdy    %d\n", GFIELD(state, PRES_READ_DATA_RDY));
    data += sprintf(data, "card_present     %d\n", GFIELD(state, PRES_CARD_PRESENT));
    data += sprintf(data, "card_stable      %d\n", GFIELD(state, PRES_CARD_STABLE));
    data += sprintf(data, "card_present_raw %d\n", GFIELD(state, PRES_CARD_PRESENT_RAW));
    data += sprintf(data, "write_enabled    %d\n", GFIELD(state, PRES_WRITE_ENABLED));
    data += sprintf(data, "cmd_signal       %d\n", GFIELD(state, PRES_CMD_SIGNAL));

    return (data - buf);
}

static int
parse_caps(uint32 cap, char *buf, int len)
{
    int block = 0xbeef;
    char *data = buf;

    data += sprintf(data, "TimeOut Clock Freq:\t%d\n", GFIELD(cap, CAP_TO_CLKFREQ));
    data += sprintf(data, "TimeOut Clock Unit:\t%d\n", GFIELD(cap, CAP_TO_CLKUNIT));
    data += sprintf(data, "Base Clock:\t\t%d\n", GFIELD(cap, CAP_BASECLK));
    switch (GFIELD(cap, CAP_MAXBLOCK)) {
    case 0: block = 512; break;
    case 1: block = 1024; break;
    case 2: block = 2048; break;
    case 3: block = 0; break;
    }
    data += sprintf(data, "Max Block Size:\t\t%d\n", block);
    data += sprintf(data, "Support High Speed:\t%d\n", GFIELD(cap, CAP_HIGHSPEED));
    data += sprintf(data, "Support DMA:\t\t%d\n", GFIELD(cap, CAP_DMA));
    data += sprintf(data, "Support Suspend:\t%d\n", GFIELD(cap, CAP_SUSPEND));
    data += sprintf(data, "Support 3.3 Volts:\t%d\n", GFIELD(cap, CAP_VOLT_3_3));
    data += sprintf(data, "Support 3.0 Volts:\t%d\n", GFIELD(cap, CAP_VOLT_3_0));
    data += sprintf(data, "Support 1.8 Volts:\t%d\n", GFIELD(cap, CAP_VOLT_1_8));
    return (data - buf);
}
#endif /* NOTUSED */
#endif /* BCMINTERNAL */

/* XXX Per SDIO Host Controller Spec section 3.2.1
    Note: for 2.x HC, new_sd_divisor should be a power of 2, but for 3.0
    HC, new_sd_divisor should be a multiple of 2.
*/
bool
sdstd_start_clock(sdioh_info_t *sd, uint16 new_sd_divisor)
{
    uint rc, count;
    uint16 divisor;
#ifdef __IOPOS__
    uint16 tmp;
#endif
    uint16 regdata;
#ifndef __IOPOS__
    uint16 val1;
#endif
    sd3_trace(("%s: starting clk\n", __FUNCTION__));
    /* turn off HC clock */
    sdstd_wreg16(sd, SD_ClockCntrl,
                 sdstd_rreg16(sd, SD_ClockCntrl) & ~((uint16)0x4)); /*  Disable the HC clock */

    /* Set divisor */
    if (sd->version == HOST_CONTR_VER_3) {
#ifdef BCMDBG
        if ((new_sd_divisor != 1) &&        /* 1 is a valid value */
            ((new_sd_divisor & (0x1)) ||    /* check for multiple of 2 */
             (new_sd_divisor == 0) ||
             (new_sd_divisor > 0x3ff))) {
            sd_err(("3.0: Invalid clock divisor target: %d\n", new_sd_divisor));
            return FALSE;
        }
#endif
        divisor = new_sd_divisor - 1;
    } else
    {
#ifdef BCMDBG
        if ((new_sd_divisor & (new_sd_divisor-1)) ||
            (new_sd_divisor == 0)) {
            sd_err(("Invalid clock divisor target: %d\n", new_sd_divisor));
            return FALSE;
        }
#endif
        /* new logic: if divisor > 256, restrict to 256 */
        if (new_sd_divisor > 256)
            new_sd_divisor = 256;
        divisor = (new_sd_divisor >> 1) << 8;
    }
#ifdef BCMINTERNAL
    if (sd->controller_type == SDIOH_TYPE_JINVANI_GOLD) {
        divisor = (new_sd_divisor >> 2) << 8;
    }
#endif /* BCMINTERNAL */

    sd_info(("Clock control is 0x%x\n", sdstd_rreg16(sd, SD_ClockCntrl)));
#ifdef __IOPOS__
    tmp = sdstd_rreg16(sd, SD_ClockCntrl);
    tmp &= 0x00ff;
    tmp |= divisor;
    sdstd_wreg16(sd, SD_ClockCntrl, divisor);
#else
    if (sd->version == HOST_CONTR_VER_3) {
        /* *get preset value and shift so that.
        *    bits 0-7 are in 15-8 and 9-8 are in 7-6 of clkctrl
        */
        val1 = divisor << 2;
        val1 &= 0x0ffc;
        val1 |= divisor >> 8;
        val1 <<= 6;
        sdstd_mod_reg16(sd, SD_ClockCntrl, 0xffC0, val1);
    } else
    {
        sdstd_mod_reg16(sd, SD_ClockCntrl, 0xff00, divisor);
    }

#endif /* __IOPOS__ */

    /* Add delay before starting clock */
    OSL_DELAY(100);

    sd_err(("%s: Using clock divisor of %d (regval 0x%04x)\n", __FUNCTION__,
             new_sd_divisor, divisor));
    if (new_sd_divisor > 0)
        sd_err(("%s:now, divided clk is: %d Hz\n",
          __FUNCTION__, GFIELD(sd->caps, CAP_BASECLK)*1000000/new_sd_divisor));
    else
        sd_err(("Using Primary Clock Freq of %d MHz\n", GFIELD(sd->caps, CAP_BASECLK)));
    sd_info(("Primary Clock Freq = %d MHz\n", GFIELD(sd->caps, CAP_BASECLK)));
    if (GFIELD(sd->caps, CAP_TO_CLKFREQ) == 50) {
        sd_info(("%s: Resulting SDIO clock is %d %s\n", __FUNCTION__,
                ((50 % new_sd_divisor) ? (50000 / new_sd_divisor) : (50 / new_sd_divisor)),
                ((50 % new_sd_divisor) ? "KHz" : "MHz")));
    } else if (GFIELD(sd->caps, CAP_TO_CLKFREQ) == 48) {
        sd_info(("%s: Resulting SDIO clock is %d %s\n", __FUNCTION__,
                ((48 % new_sd_divisor) ? (48000 / new_sd_divisor) : (48 / new_sd_divisor)),
                ((48 % new_sd_divisor) ? "KHz" : "MHz")));
    } else if (GFIELD(sd->caps, CAP_TO_CLKFREQ) == 33) {
        sd_info(("%s: Resulting SDIO clock is %d %s\n", __FUNCTION__,
                ((33 % new_sd_divisor) ? (33000 / new_sd_divisor) : (33 / new_sd_divisor)),
                ((33 % new_sd_divisor) ? "KHz" : "MHz")));
    } else if (GFIELD(sd->caps, CAP_TO_CLKFREQ) == 31) {
        sd_info(("%s: Resulting SDIO clock is %d %s\n", __FUNCTION__,
                ((31 % new_sd_divisor) ? (31000 / new_sd_divisor) : (31 / new_sd_divisor)),
                ((31 % new_sd_divisor) ? "KHz" : "MHz")));
    } else if (GFIELD(sd->caps, CAP_TO_CLKFREQ) == 8) {
        sd_info(("%s: Resulting SDIO clock is %d %s\n", __FUNCTION__,
                ((8 % new_sd_divisor) ? (8000 / new_sd_divisor) : (8 / new_sd_divisor)),
                ((8 % new_sd_divisor) ? "KHz" : "MHz")));
    } else if (sd->controller_type == SDIOH_TYPE_BCM27XX || sd->controller_type == SDIOH_TYPE_BCM4390X) {
        /* XXX - BCM 27XX Standard Host Controller returns 0 for CLKFREQ */
    } else {
        sd_err(("Need to determine divisor for %d MHz clocks\n",
                GFIELD(sd->caps, CAP_BASECLK)));
        sd_err(("Consult SD Host Controller Spec: Clock Control Register\n"));
        return (FALSE);
    }

#ifdef __IOPOS__
    /* XXX Need to implement or_reg for IOPOS */
    tmp = sdstd_rreg16(sd, SD_ClockCntrl);
    sdstd_wreg16(sd, SD_ClockCntrl, tmp | 0x1); /*  Enable the clock */
#else
    sdstd_or_reg16(sd, SD_ClockCntrl, 0x1); /*  Enable the clock */
#endif

    /* Wait for clock to stabilize */
    rc = (sdstd_rreg16(sd, SD_ClockCntrl) & 2);
    count = 0;
    while (!rc) {
        OSL_DELAY(1);
        sd_info(("Waiting for clock to become stable 0x%x\n", rc));
        rc = (sdstd_rreg16(sd, SD_ClockCntrl) & 2);
        count++;
        if (count > 10000) {
            sd_err(("%s:Clocks failed to stabilize after %u attempts\n",
                    __FUNCTION__, count));
            return (FALSE);
        }
    }
    /* Turn on clock */
#ifdef __IOPOS__
    /* XXX Need to implement or_reg for IOPOS */
    tmp = sdstd_rreg16(sd, SD_ClockCntrl);
    sdstd_wreg16(sd, SD_ClockCntrl, tmp | 0x4);
#else
    sdstd_or_reg16(sd, SD_ClockCntrl, 0x4);
#endif

    OSL_DELAY(20);

    /* Set timeout control (adjust default value based on divisor).
     * Disabling timeout interrupts during setting is advised by host spec.
     */
#ifdef BCMQT
    if (GFIELD(sd->caps, CAP_BASECLK) < 50)
#endif
    {
        uint toval;

        toval = sd_toctl;
        divisor = new_sd_divisor;

        while (toval && !(divisor & 1)) {
            toval -= 1;
            divisor >>= 1;
        }

        regdata = sdstd_rreg16(sd, SD_ErrorIntrStatusEnable);
        sdstd_wreg16(sd, SD_ErrorIntrStatusEnable, (regdata & ~ERRINT_DATA_TIMEOUT_BIT));
        sdstd_wreg8(sd, SD_TimeoutCntrl, (uint8)toval);
        sdstd_wreg16(sd, SD_ErrorIntrStatusEnable, regdata);
    }
#ifdef BCMQT
    else {
        sd_info(("%s: REsetting err int control\n", __FUNCTION__));
        /* XXX: turn off timeout INT, it resets clk ctrl bit */
        regdata = sdstd_rreg16(sd, SD_ErrorIntrStatusEnable);
        sdstd_wreg16(sd, SD_ErrorIntrStatusEnable, (regdata & ~ERRINT_DATA_TIMEOUT_BIT));
    }
#endif

    /* Add delay after starting clock. the board takes a while to become ready */
    OSL_DELAY(100);

    sd_info(("Final Clock control is 0x%x\n", sdstd_rreg16(sd, SD_ClockCntrl)));

    return TRUE;
}

/* XXX Per SDIO Host Controller Spec 3.3
 * volts_req:
 *        0 means default: select highest voltage.
 *        1 means 1.8V
 *        2 means 3.0V
 *        3 means 3.3V
 * returns
 *    TRUE: no error
 *    FALSE: general error
 *    SDIO_OCR_READ_FAIL: ocr reading failure. Now the HC has to try in other available voltages.
*/
uint16
sdstd_start_power(sdioh_info_t *sd, int volts_req)
{
    char *s;
    uint32 cmd_arg;
    uint32 cmd_rsp;
    uint8 pwr = 0;
    int volts = 0;
    uint16 val1;
    uint16 init_divider = 0;
    uint8 baseclk = 0;
    bool selhighest = (volts_req == 0) ? TRUE : FALSE;

    /* reset the card uhsi volt support to false */
    sd->card_UHSI_voltage_Supported = FALSE;

    /* Ensure a power on reset by turning off bus power in case it happened to
     * be on already. (This might happen if driver doesn't unload/clean up correctly,
     * crash, etc.)  Leave off for 100ms to make sure the power off isn't
     * ignored/filtered by the device.  Note we can't skip this step if the power is
     * off already since we don't know how long it has been off before starting
     * the driver.
     */
    sdstd_wreg8(sd, SD_PwrCntrl, 0);
    sd_info(("Turning off VDD/bus power briefly (100ms) to ensure reset\n"));
    OSL_DELAY(100000);

    /* For selecting highest available voltage, start from lowest and iterate */
    if (!volts_req)
        volts_req = 1;

    s = NULL;

    if (volts_req == 1) {
        if (GFIELD(sd->caps, CAP_VOLT_1_8)) {
            volts = 5;
            s = "1.8";
            if (FALSE == selhighest)
                goto voltsel;
            else
                volts_req++;
        } else {
            sd_err(("HC doesn't support voltage! trying higher voltage: %d\n", volts));
            volts_req++;
        }
    }

    if (volts_req == 2) {
        if (GFIELD(sd->caps, CAP_VOLT_3_0)) {
            volts = 6;
            s = "3.0";
            if (FALSE == selhighest)
                goto voltsel;
            else volts_req++;
        } else {
            sd_err(("HC doesn't support voltage! trying higher voltage: %d\n", volts));
            volts_req++;
        }
    }

    if (volts_req == 3) {
        if (GFIELD(sd->caps, CAP_VOLT_3_3)) {
            volts = 7;
            s = "3.3";
        } else {
            if ((FALSE == selhighest) || (volts == 0)) {
                sd_err(("HC doesn't support any voltage! error!\n"));
                return FALSE;
            }
        }
    }

    /* XXX
     * if UHSI is NOT supported, check for other voltages also. This is a safety measure
     * for embedded devices also, so that HC starts in lower power first. If this
     * function fails, the caller may disable UHSISupported
     * and call start power again to check support in higher voltages.
     */

voltsel:
    pwr = SFIELD(pwr, PWR_VOLTS, volts);
    pwr = SFIELD(pwr, PWR_BUS_EN, 1);
    sdstd_wreg8(sd, SD_PwrCntrl, pwr); /* Set Voltage level */
    sd_info(("Setting Bus Power to %s Volts\n", s));
    BCM_REFERENCE(s);

    /*
     * PR101766 : BRCM SDIO3.0 card is an embedded SD device. It is not a SD card.
     * VDDIO signalling will be tied to 1.8v level on all SDIO3.0 based boards.
     * So program the HC to drive VDDIO at 1.8v level.
     */
    if ((sd->version == HOST_CONTR_VER_3) && (volts == 5)) {
        val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
        val1 = SFIELD(val1, HOSTCtrl2_1_8SIG_EN, 1);
        sdstd_wreg16(sd, SD3_HostCntrl2, val1);
    }

    /* Wait for 500ms for power to stabilize.  Some designs have reset IC's
     * which can hold reset low for close to 300ms.  In addition there can
     * be ramp time for VDD and/or VDDIO which might be provided from a LDO.
     * For these reasons we need a pretty conservative delay here to have
     * predictable reset behavior in the face of an unknown design.
     */
    OSL_DELAY(500000);

    baseclk = GFIELD(sd->caps, CAP_BASECLK);
    sd_info(("%s:baseclk: %d MHz\n",    __FUNCTION__, baseclk));
    /* for 3.0, find divisor */
    if (sd->version == HOST_CONTR_VER_3) {
        /* ToDo : Dynamic modification of preset value table based on base clk */
        sd3_trace(("sd3: %s: checking divisor\n", __FUNCTION__));
        if (GFIELD(sd->caps3, CAP3_CLK_MULT) != 0)    {
            sd_err(("%s:Possible error: CLK Mul 1 CLOCKING NOT supported!\n",
                __FUNCTION__));
            return FALSE;
        } else {
            /*  calculate dividor  */
            init_divider = baseclk*1000/SDIO_CLK_FREQ;
            /* make it a multiple of 2. */
            init_divider += (init_divider & 0x1);
            sd_err(("%s:divider used for init:%d\n",
                __FUNCTION__, init_divider));
        }
    } else {
        /* Note: sd_divisor assumes that SDIO Base CLK is 50MHz. */
        int final_freq_based_on_div = 50/sd_divisor;
        if (baseclk > 50)
            sd_divisor = baseclk/final_freq_based_on_div;
        /* TBD: merge both SDIO 2.0 and 3.0 to share same divider logic */
            init_divider = baseclk*1000/SDIO_CLK_FREQ;
        /* find next power of 2 */
        NEXT_POW2(init_divider);
        sd_err(("%s:NONUHSI: divider used for init:%d\n",
            __FUNCTION__, init_divider));
    }

    /* Start at ~400KHz clock rate for initialization */
#ifndef __IOPOS__
    if (!sdstd_start_clock(sd, init_divider)) {
#else
    /* XXX I wonder if this special case of a 2KHz clock is still necessary
     * after cleaning up some issues in the initialization logic.  See PR's 87546,
     * 87548 and 87550.  It would be great if somebody with the IOPOS hardware
     * could investigate this a bit and remove the special case if possible.  johnvb
     */
    if (!sdstd_start_clock(sd, 2)) {
#endif
        sd_err(("%s: sdstd_start_clock failed\n", __FUNCTION__));
        return FALSE;
    }

    /* Get the Card's Operation Condition.  Occasionally the board
     * takes a while to become ready
     */
    cmd_arg = 0;
    cmd_rsp = 0;
    if (get_ocr(sd, &cmd_arg, &cmd_rsp) != SUCCESS) {
        sd_err(("%s: Failed to get OCR bailing\n", __FUNCTION__));
        /* No need to reset as not sure in what state the card is. */
        return SDIO_OCR_READ_FAIL;
    }

    sd_info(("cmd_rsp = 0x%x\n", cmd_rsp));
    sd_info(("mem_present = %d\n", GFIELD(cmd_rsp, RSP4_MEM_PRESENT)));
    sd_info(("num_funcs = %d\n", GFIELD(cmd_rsp, RSP4_NUM_FUNCS)));
    sd_info(("card_ready = %d\n", GFIELD(cmd_rsp, RSP4_CARD_READY)));
    sd_info(("OCR = 0x%x\n", GFIELD(cmd_rsp, RSP4_IO_OCR)));

    /* Verify that the card supports I/O mode */
    if (GFIELD(cmd_rsp, RSP4_NUM_FUNCS) == 0) {
        sd_err(("%s: Card does not support I/O\n", __FUNCTION__));
        return ERROR;
    }
    sd->num_funcs = GFIELD(cmd_rsp, RSP4_NUM_FUNCS);

    /* Examine voltage: Arasan only supports 3.3 volts,
     * so look for 3.2-3.3 Volts and also 3.3-3.4 volts.
     */

    /* XXX Pg 10 SDIO spec v1.10 */
    if ((GFIELD(cmd_rsp, RSP4_IO_OCR) & (0x3 << 20)) == 0) {
        sd_err(("This client does not support 3.3 volts!\n"));
        return ERROR;
    }
    sd_info(("Leaving bus power at 3.3 Volts\n"));

    cmd_arg = SFIELD(0, CMD5_OCR, 0xfff000);
    /* if HC uhsi supported and card voltage set is 3.3V then switch to 1.8V */
    if ((sd->host_UHSISupported) && (volts == 5)) {
        /* set S18R also */
        cmd_arg = SFIELD(cmd_arg, CMD5_S18R, 1);
    }
    cmd_rsp = 0;
    get_ocr(sd, &cmd_arg, &cmd_rsp);
    sd_info(("OCR = 0x%x\n", GFIELD(cmd_rsp, RSP4_IO_OCR)));

    if ((sd->host_UHSISupported)) {
        /* card responded with s18A => card supports sdio3.0,do tuning proc */
        if (GFIELD(cmd_rsp, RSP4_S18A) == 1) {
            if (sdstd_3_sigvoltswitch_proc(sd)) {
                /* continue with legacy way of working */
                sd_err(("%s: voltage switch not done. error, stopping\n",
                    __FUNCTION__));
                /* How to gracefully proceced here? */
                return FALSE;
            } else {
                sd->card_UHSI_voltage_Supported = TRUE;
                sd_err(("%s: voltage switch SUCCESS!\n", __FUNCTION__));
            }
        } else {
                /* This could happen for 2 cases.
                 * 1) means card is NOT sdio3.0 . Note that
                 *    card_UHSI_voltage_Supported is already false.
                 * 2) card is sdio3.0 but it is already in 1.8V.
                 *    But now, how to change host controller's voltage?
                 * In this case we need to do the following.
                 * sd->card_UHSI_voltage_Supported = TRUE;
                 * turn 1.8V sig enable in HC2
                 * val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
                 * val1 = SFIELD(val1, HOSTCtrl2_1_8SIG_EN, 1);
                 * sdstd_wreg16(sd, SD3_HostCntrl2, val1);
                 */
                sd_info(("%s: Not sdio3.0: host_UHSISupported: %d; HC volts=%d\n",
                    __FUNCTION__, sd->host_UHSISupported, volts));
        }
    } else {
        sd_info(("%s: Legacy [non sdio3.0] HC\n", __FUNCTION__));
    }

    return TRUE;
}

bool
sdstd_bus_width(sdioh_info_t *sd, int new_mode)
{
    uint32 regdata;
    int status;
    uint8 reg8;

    sd_trace(("%s\n", __FUNCTION__));
    if (sd->sd_mode == new_mode) {
        sd_info(("%s: Already at width %d\n", __FUNCTION__, new_mode));
        /* Could exit, but continue just in case... */
    }

    /* Set client side via reg 0x7 in CCCR */
    if ((status = sdstd_card_regread (sd, 0, SDIOD_CCCR_BICTRL, 1, &regdata)) != SUCCESS)
        return (bool)status;
    regdata &= ~BUS_SD_DATA_WIDTH_MASK;
    if (new_mode == SDIOH_MODE_SD4) {
        sd_info(("Changing to SD4 Mode\n"));
        regdata |= SD4_MODE;
    } else if (new_mode == SDIOH_MODE_SD1) {
        sd_info(("Changing to SD1 Mode\n"));
    } else {
        sd_err(("SPI Mode not supported by Standard Host Controller\n"));
    }

    if ((status = sdstd_card_regwrite (sd, 0, SDIOD_CCCR_BICTRL, 1, regdata)) != SUCCESS)
        return (bool)status;

    if (sd->host_UHSISupported) {
        uint32 card_asyncint = 0;
        uint16 host_asyncint = 0;

        if ((status = sdstd_card_regread (sd, 0, SDIOD_CCCR_INTR_EXTN, 1,
            &card_asyncint)) != SUCCESS) {
            sd_err(("%s:INTR EXT getting failed!, ignoring\n", __FUNCTION__));
        } else {
            host_asyncint = sdstd_rreg16(sd, SD3_HostCntrl2);

            /* check if supported by host and card */
            if ((regdata & SD4_MODE) &&
                (GFIELD(card_asyncint, SDIO_BUS_ASYNCINT_CAP)) &&
                (GFIELD(sd->caps, CAP_ASYNCINT_SUP))) {
                /* set enable async int in card */
                card_asyncint = SFIELD(card_asyncint, SDIO_BUS_ASYNCINT_SEL, 1);

                if ((status = sdstd_card_regwrite (sd, 0,
                    SDIOD_CCCR_INTR_EXTN, 1, card_asyncint)) != SUCCESS)
                    sd_err(("%s:INTR EXT setting failed!, ignoring\n",
                    __FUNCTION__));
                else {
                    /* set enable async int in host */
                    host_asyncint = SFIELD(host_asyncint,
                        HOSTCtrl2_ASYINT_EN, 1);
                    sdstd_wreg16(sd, SD3_HostCntrl2, host_asyncint);
                }
            } else {
                sd_err(("%s:INTR EXT NOT supported by either host or"
                    "card!, ignoring\n", __FUNCTION__));
            }
        }
    }

    /* Set host side via Host reg */
    reg8 = sdstd_rreg8(sd, SD_HostCntrl) & ~SD4_MODE;
    if (new_mode == SDIOH_MODE_SD4)
        reg8 |= SD4_MODE;
    sdstd_wreg8(sd, SD_HostCntrl, reg8);

    sd->sd_mode = new_mode;

    return TRUE;
}

static int
sdstd_driver_init(sdioh_info_t *sd)
{
    sd_trace(("%s\n", __FUNCTION__));
    sd->sd3_tuning_reqd = FALSE;
    if ((sdstd_host_init(sd)) != SUCCESS) {
        return ERROR;
    }

    /* Give WL_reset before sending CMD5 to dongle for Revx SDIO3 HC's */
    if ((sd->controller_type == SDIOH_TYPE_RICOH_R5C822) && (sd->version == HOST_CONTR_VER_3))
    {
        sdstd_wreg16(sd, SD3_WL_BT_reset_register, 0x8);
        OSL_DELAY(sd_delay_value);
        sdstd_wreg16(sd, SD3_WL_BT_reset_register, 0x0);
        OSL_DELAY(500000);
    }

    if (sd->client_init(sd) != SUCCESS) {
        return ERROR;
    }

    /* if the global cap matched and is SDR 104/50 [if 50 it is reqd] enable tuning. */
    if ((TRUE != sd3_sw_override1) && SD3_TUNING_REQD(sd, sd_uhsimode)) {
            sd->sd3_tuning_reqd = TRUE;

            /* init OS structs for tuning */
            sdstd_3_osinit_tuning(sd);

            /* enable HC tuning interrupt OR timer based on tuning method */
            if (GFIELD(sd->caps3, CAP3_RETUNING_MODES)) {
                /* enable both RTReq and timer */
                sd->intmask |= HC_INTR_RETUNING;
                sdstd_wreg16(sd, SD_IntrSignalEnable, sd->intmask);
#ifdef BCMSDYIELD
                if (sd_forcerb)
                    sdstd_rreg16(sd, SD_IntrSignalEnable); /* Sync readback */
#endif /* BCMSDYIELD */
            }
        }

    return SUCCESS;
}

static int
sdstd_get_cisaddr(sdioh_info_t *sd, uint32 regaddr)
{
    /* read 24 bits and return valid 17 bit addr */
    int i;
    uint32 scratch, regdata = 0;
    uint8 *ptr = (uint8 *)&scratch;
    for (i = 0; i < 3; i++) {
        if ((sdstd_card_regread (sd, 0, regaddr, 1, &regdata)) != SUCCESS)
            sd_err(("%s: Can't read!\n", __FUNCTION__));

        *ptr++ = (uint8) regdata;
        regaddr++;
    }
    /* Only the lower 17-bits are valid */
    scratch = ltoh32(scratch);
    scratch &= 0x0001FFFF;
    return (scratch);
}

static int
sdstd_card_enablefuncs(sdioh_info_t *sd)
{
    int status;
    uint32 regdata;
    uint32 fbraddr;
    uint8 func;

    sd_trace(("%s\n", __FUNCTION__));

    /* Get the Card's common CIS address */
    sd->com_cis_ptr = sdstd_get_cisaddr(sd, SDIOD_CCCR_CISPTR_0);
    sd->func_cis_ptr[0] = sd->com_cis_ptr;
    sd_info(("%s: Card's Common CIS Ptr = 0x%x\n", __FUNCTION__, sd->com_cis_ptr));

    /* Get the Card's function CIS (for each function) */
    for (fbraddr = SDIOD_FBR_STARTADDR, func = 1;
         func <= sd->num_funcs; func++, fbraddr += SDIOD_FBR_SIZE) {
        sd->func_cis_ptr[func] = sdstd_get_cisaddr(sd, SDIOD_FBR_CISPTR_0 + fbraddr);
        sd_info(("%s: Function %d CIS Ptr = 0x%x\n",
                 __FUNCTION__, func, sd->func_cis_ptr[func]));
    }

    /* Enable function 1 on the card */
    regdata = SDIO_FUNC_ENABLE_1;
    if ((status = sdstd_card_regwrite(sd, 0, SDIOD_CCCR_IOEN, 1, regdata)) != SUCCESS)
        return status;

    return SUCCESS;
}

/* Read client card reg */
static int
sdstd_card_regread(sdioh_info_t *sd, int func, uint32 regaddr, int regsize, uint32 *data)
{
    int status;
    uint32 cmd_arg;
    uint32 rsp5;

#ifdef BCMDBG
    if (sdstd_rreg16 (sd, SD_ErrorIntrStatus) != 0) {
        sd_err(("%s: Entering: ErrorintrStatus 0x%x, intstat = 0x%x\n",
                __FUNCTION__, sdstd_rreg16(sd, SD_ErrorIntrStatus),
                sdstd_rreg16(sd, SD_IntrStatus)));
    }
#endif

    cmd_arg = 0;

    if ((func == 0) || (regsize == 1)) {
        cmd_arg = SFIELD(cmd_arg, CMD52_FUNCTION, func);
        cmd_arg = SFIELD(cmd_arg, CMD52_REG_ADDR, regaddr);
        cmd_arg = SFIELD(cmd_arg, CMD52_RW_FLAG, SDIOH_XFER_TYPE_READ);
        cmd_arg = SFIELD(cmd_arg, CMD52_RAW, 0);
        cmd_arg = SFIELD(cmd_arg, CMD52_DATA, 0);

        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_52, cmd_arg))
            != SUCCESS)
            return status;

        sdstd_cmd_getrsp(sd, &rsp5, 1);
        if (sdstd_rreg16(sd, SD_ErrorIntrStatus) != 0) {
            sd_err(("%s: 1: ErrorintrStatus 0x%x\n",
                    __FUNCTION__, sdstd_rreg16(sd, SD_ErrorIntrStatus)));
        }

        if (GFIELD(rsp5, RSP5_FLAGS) != 0x10)
            sd_err(("%s: rsp5 flags is 0x%x\t %d\n",
                    __FUNCTION__, GFIELD(rsp5, RSP5_FLAGS), func));

        if (GFIELD(rsp5, RSP5_STUFF))
            sd_err(("%s: rsp5 stuff is 0x%x: should be 0\n",
                    __FUNCTION__, GFIELD(rsp5, RSP5_STUFF)));
        *data = GFIELD(rsp5, RSP5_DATA);

        sd_data(("%s: Resp data(0x%x)\n", __FUNCTION__, *data));
    } else {
        cmd_arg = SFIELD(cmd_arg, CMD53_BYTE_BLK_CNT, regsize);
        cmd_arg = SFIELD(cmd_arg, CMD53_OP_CODE, 1);    /* XXX SDIO spec v 1.10, Sec 5.3 */
        cmd_arg = SFIELD(cmd_arg, CMD53_BLK_MODE, 0);
        cmd_arg = SFIELD(cmd_arg, CMD53_FUNCTION, func);
        cmd_arg = SFIELD(cmd_arg, CMD53_REG_ADDR, regaddr);
        cmd_arg = SFIELD(cmd_arg, CMD53_RW_FLAG, SDIOH_XFER_TYPE_READ);

        sd->data_xfer_count = regsize;

        /* sdstd_cmd_issue() returns with the command complete bit
         * in the ISR already cleared
         */
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_53, cmd_arg))
            != SUCCESS)
            return status;

        sdstd_cmd_getrsp(sd, &rsp5, 1);

        if (GFIELD(rsp5, RSP5_FLAGS) != 0x10)
            sd_err(("%s: rsp5 flags is 0x%x\t %d\n",
                    __FUNCTION__, GFIELD(rsp5, RSP5_FLAGS), func));

        if (GFIELD(rsp5, RSP5_STUFF))
            sd_err(("%s: rsp5 stuff is 0x%x: should be 0\n",
                    __FUNCTION__, GFIELD(rsp5, RSP5_STUFF)));

        if (sd->polled_mode) {
            volatile uint16 int_reg;
            int retries = RETRIES_LARGE;

            /* Wait for Read Buffer to become ready */
            do {
                sdstd_os_yield(sd);
                int_reg = sdstd_rreg16(sd, SD_IntrStatus);
            } while (--retries && (GFIELD(int_reg, INTSTAT_BUF_READ_READY) == 0));

            if (!retries) {
                sd_err(("%s: Timeout on Buf_Read_Ready: "
                        "intStat: 0x%x errint: 0x%x PresentState 0x%x\n",
                        __FUNCTION__, int_reg,
                        sdstd_rreg16(sd, SD_ErrorIntrStatus),
                        sdstd_rreg(sd, SD_PresentState)));
                sdstd_check_errs(sd, SDIOH_CMD_53, cmd_arg);
                return (ERROR);
            }

            /* Have Buffer Ready, so clear it and read the data */
            sdstd_wreg16(sd, SD_IntrStatus, SFIELD(0, INTSTAT_BUF_READ_READY, 1));
            if (regsize == 2)
                *data = sdstd_rreg16(sd, SD_BufferDataPort0);
            else
                *data = sdstd_rreg(sd, SD_BufferDataPort0);

            sd_data(("%s: Resp data(0x%x)\n", __FUNCTION__, *data));
            /* Check Status.
             * After the data is read, the Transfer Complete bit should be on
             */
            retries = RETRIES_LARGE;
            do {
                int_reg = sdstd_rreg16(sd, SD_IntrStatus);
            } while (--retries && (GFIELD(int_reg, INTSTAT_XFER_COMPLETE) == 0));

            /* Check for any errors from the data phase */
            if (sdstd_check_errs(sd, SDIOH_CMD_53, cmd_arg))
                return ERROR;

            if (!retries) {
                sd_err(("%s: Timeout on xfer complete: "
                        "intr 0x%04x err 0x%04x state 0x%08x\n",
                        __FUNCTION__, int_reg,
                        sdstd_rreg16(sd, SD_ErrorIntrStatus),
                        sdstd_rreg(sd, SD_PresentState)));
                return (ERROR);
            }

            sdstd_wreg16(sd, SD_IntrStatus, SFIELD(0, INTSTAT_XFER_COMPLETE, 1));
        }
    }
    if (sd->polled_mode) {
        if (regsize == 2)
            *data &= 0xffff;
    }
    return SUCCESS;
}

bool
check_client_intr(sdioh_info_t *sd)
{
    uint16 raw_int, cur_int, old_int;

    raw_int = sdstd_rreg16(sd, SD_IntrStatus);
    cur_int = raw_int & sd->intmask;

    if (!cur_int) {
        /* Not an error -- might share interrupts... */
        return FALSE;
    }

    if (GFIELD(cur_int, INTSTAT_CARD_INT)) {
        unsigned long flags;

        sdstd_os_lock_irqsave(sd, &flags);
        old_int = sdstd_rreg16(sd, SD_IntrStatusEnable);
        sdstd_wreg16(sd, SD_IntrStatusEnable, SFIELD(old_int, INTSTAT_CARD_INT, 0));
        sdstd_os_unlock_irqrestore(sd, &flags);

        if (sd->client_intr_enabled && sd->use_client_ints) {
            sd->intrcount++;
            ASSERT(sd->intr_handler);
            ASSERT(sd->intr_handler_arg);
            (sd->intr_handler)(sd->intr_handler_arg);
        } else {
            sd_err(("%s: Not ready for intr: enabled %d, handler %p\n",
                    __FUNCTION__, sd->client_intr_enabled, sd->intr_handler));
        }
        sdstd_os_lock_irqsave(sd, &flags);
        old_int = sdstd_rreg16(sd, SD_IntrStatusEnable);
        sdstd_wreg16(sd, SD_IntrStatusEnable, SFIELD(old_int, INTSTAT_CARD_INT, 1));
        sdstd_os_unlock_irqrestore(sd, &flags);
    } else {
        /* Local interrupt: disable, set flag, and save intrstatus */
        sdstd_wreg16(sd, SD_IntrSignalEnable, 0);
        sdstd_wreg16(sd, SD_ErrorIntrSignalEnable, 0);
        sd->local_intrcount++;
        sd->got_hcint = TRUE;
        sd->last_intrstatus = cur_int;
    }

    return TRUE;
}

void
sdstd_spinbits(sdioh_info_t *sd, uint16 norm, uint16 err)
{
    uint16 int_reg, err_reg;
    int retries = RETRIES_LARGE;

    do {
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
        err_reg = sdstd_rreg16(sd, SD_ErrorIntrStatus);
    } while (--retries && !(int_reg & norm) && !(err_reg & err));

    norm |= sd->intmask;
    if (err_reg & err)
        norm = SFIELD(norm, INTSTAT_ERROR_INT, 1);
    sd->last_intrstatus = int_reg & norm;
}

/* write a client register */
static int
sdstd_card_regwrite(sdioh_info_t *sd, int func, uint32 regaddr, int regsize, uint32 data)
{
    int status;
    uint32 cmd_arg, rsp5, flags;

    cmd_arg = 0;

    if ((func == 0) || (regsize == 1)) {
        cmd_arg = SFIELD(cmd_arg, CMD52_FUNCTION, func);
        cmd_arg = SFIELD(cmd_arg, CMD52_REG_ADDR, regaddr);
        cmd_arg = SFIELD(cmd_arg, CMD52_RW_FLAG, SDIOH_XFER_TYPE_WRITE);
        cmd_arg = SFIELD(cmd_arg, CMD52_RAW, 0);
        cmd_arg = SFIELD(cmd_arg, CMD52_DATA, data & 0xff);
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_52, cmd_arg))
            != SUCCESS)
            return status;

        sdstd_cmd_getrsp(sd, &rsp5, 1);
        flags = GFIELD(rsp5, RSP5_FLAGS);
        if (flags && (flags != 0x10))
            sd_err(("%s: rsp5.rsp5.flags = 0x%x, expecting 0x10\n",
                    __FUNCTION__,  flags));
    }
    else {
        cmd_arg = SFIELD(cmd_arg, CMD53_BYTE_BLK_CNT, regsize);
        /* XXX SDIO spec v 1.10, Sec 5.3  Not FIFO */
        cmd_arg = SFIELD(cmd_arg, CMD53_OP_CODE, 1);
        cmd_arg = SFIELD(cmd_arg, CMD53_BLK_MODE, 0);
        cmd_arg = SFIELD(cmd_arg, CMD53_FUNCTION, func);
        cmd_arg = SFIELD(cmd_arg, CMD53_REG_ADDR, regaddr);
        cmd_arg = SFIELD(cmd_arg, CMD53_RW_FLAG, SDIOH_XFER_TYPE_WRITE);

        sd->data_xfer_count = regsize;

        /* sdstd_cmd_issue() returns with the command complete bit
         * in the ISR already cleared
         */
        if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_53, cmd_arg))
            != SUCCESS)
            return status;

        sdstd_cmd_getrsp(sd, &rsp5, 1);

        if (GFIELD(rsp5, RSP5_FLAGS) != 0x10)
            sd_err(("%s: rsp5 flags = 0x%x, expecting 0x10\n",
                    __FUNCTION__,  GFIELD(rsp5, RSP5_FLAGS)));
        if (GFIELD(rsp5, RSP5_STUFF))
            sd_err(("%s: rsp5 stuff is 0x%x: expecting 0\n",
                    __FUNCTION__, GFIELD(rsp5, RSP5_STUFF)));

        if (sd->polled_mode) {
            uint16 int_reg;
            int retries = RETRIES_LARGE;

            /* Wait for Write Buffer to become ready */
            do {
                int_reg = sdstd_rreg16(sd, SD_IntrStatus);
            } while (--retries && (GFIELD(int_reg, INTSTAT_BUF_WRITE_READY) == 0));

            if (!retries) {
                sd_err(("%s: Timeout on Buf_Write_Ready: intStat: 0x%x "
                        "errint: 0x%x PresentState 0x%x\n",
                        __FUNCTION__, int_reg,
                        sdstd_rreg16(sd, SD_ErrorIntrStatus),
                        sdstd_rreg(sd, SD_PresentState)));
                sdstd_check_errs(sd, SDIOH_CMD_53, cmd_arg);
                return (ERROR);
            }
            /* Clear Write Buf Ready bit */
            int_reg = 0;
            int_reg = SFIELD(int_reg, INTSTAT_BUF_WRITE_READY, 1);
            sdstd_wreg16(sd, SD_IntrStatus, int_reg);

            /* At this point we have Buffer Ready, so write the data */
            if (regsize == 2)
                sdstd_wreg16(sd, SD_BufferDataPort0, (uint16) data);
            else
                sdstd_wreg(sd, SD_BufferDataPort0, data);

            /* Wait for Transfer Complete */
            retries = RETRIES_LARGE;
            do {
                int_reg = sdstd_rreg16(sd, SD_IntrStatus);
            } while (--retries && (GFIELD(int_reg, INTSTAT_XFER_COMPLETE) == 0));

            /* Check for any errors from the data phase */
            if (sdstd_check_errs(sd, SDIOH_CMD_53, cmd_arg))
                return ERROR;

            if (retries == 0) {
                sd_err(("%s: Timeout for xfer complete; State = 0x%x, "
                        "intr state=0x%x, Errintstatus 0x%x rcnt %d, tcnt %d\n",
                        __FUNCTION__, sdstd_rreg(sd, SD_PresentState),
                        int_reg, sdstd_rreg16(sd, SD_ErrorIntrStatus),
                        sd->r_cnt, sd->t_cnt));
            }
            /* Clear the status bits */
            sdstd_wreg16(sd, SD_IntrStatus, SFIELD(int_reg, INTSTAT_CARD_INT, 0));
        }
    }
    return SUCCESS;
}

void
sdstd_cmd_getrsp(sdioh_info_t *sd, uint32 *rsp_buffer, int count /* num 32 bit words */)
{
    int rsp_count;
    int respaddr = SD_Response0;

    if (count > 4)
        count = 4;

    for (rsp_count = 0; rsp_count < count; rsp_count++) {
        *rsp_buffer++ = sdstd_rreg(sd, respaddr);
        respaddr += 4;
    }
}

/*
    Note: options: 0 - default
                 1 - tuning option: Means that, this cmd issue is as a part
                    of tuning. So no need to check the start tuning function.
*/
static int
sdstd_cmd_issue(sdioh_info_t *sdioh_info, bool use_dma, uint32 cmd, uint32 arg)
{
    uint16 cmd_reg;
    int retries;
    uint32 cmd_arg;
    uint16 xfer_reg = 0;

#ifdef BCMDBG
    if (sdstd_rreg16(sdioh_info, SD_ErrorIntrStatus) != 0) {
        sd_err(("%s: Entering: ErrorIntrStatus 0x%x, Expecting 0\n",
                __FUNCTION__, sdstd_rreg16(sdioh_info, SD_ErrorIntrStatus)));
    }
#endif

    if ((sdioh_info->sd_mode == SDIOH_MODE_SPI) &&
        ((cmd == SDIOH_CMD_3) || (cmd == SDIOH_CMD_7) || (cmd == SDIOH_CMD_15))) {
        sd_err(("%s: Cmd %d is not for SPI\n", __FUNCTION__, cmd));
        return ERROR;
    }

    retries = RETRIES_SMALL;
    while ((GFIELD(sdstd_rreg(sdioh_info, SD_PresentState), PRES_CMD_INHIBIT)) && --retries) {
        if (retries == RETRIES_SMALL)
            sd_err(("%s: Waiting for Command Inhibit cmd = %d 0x%x\n",
                    __FUNCTION__, cmd, sdstd_rreg(sdioh_info, SD_PresentState)));
    }
    if (!retries) {
        sd_err(("%s: Command Inhibit timeout\n", __FUNCTION__));
        if (trap_errs)
            ASSERT(0);
        return ERROR;
    }


    cmd_reg = 0;
    switch (cmd) {
    case SDIOH_CMD_0:     /* Set Card to Idle State - No Response */
    case SDIOH_CMD_15:    /* Set card to inactive state - Response None */
        sd_data(("%s: CMD%d\n", __FUNCTION__, cmd));
        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_NONE);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        break;

    /* R2 */
    case SDIOH_CMD_2:
    case SDIOH_CMD_9:
        sd_data(("%s: CMD%d\n", __FUNCTION__, cmd));
        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_136);
        // Workaround!!!
        // Similar CRC probelm described in http://www.yqcomputer.com/1023_20153_1.htm happens
        // So CRC checking disabled for R2 response
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        break;

    case SDIOH_ACMD_41:
        cmd &= ~SDIHO_ACMD_PREFIX;
    case SDIOH_CMD_1:
    case SDIOH_CMD_3:     /* Ask card to send RCA - Response R6 */
    case SDIOH_CMD_5:     /* Send Operation condition - Response R4 */
        sd_data(("%s: CMD%d\n", __FUNCTION__, cmd));
        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        break;

    case SDIOH_ACMD_6:
        cmd &= ~SDIHO_ACMD_PREFIX;
    case SDIOH_CMD_6:
    case SDIOH_CMD_7:     /* Select card - Response R1 */
    case SDIOH_CMD_8:
    case SDIOH_CMD_11:    /* Select card - Response R1 */
    case SDIOH_CMD_14:    /* eSD Sleep - Response R1 */
    case SDIOH_CMD_16:
    case SDIOH_CMD_55:
        sd_data(("%s: CMD%d\n", __FUNCTION__, cmd));
        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        break;

    case SDIOH_CMD_12:
        sd_data(("%s: CMD%d\n", __FUNCTION__, cmd));
        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48_BUSY);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        break;

    case MMC_CMD_8:
    case SDIOH_CMD_17:
    case SDIOH_CMD_18:
    case SDIOH_CMD_19:    /* clock tuning - Response R1 */
    case SDIOH_CMD_24:
    case SDIOH_CMD_25:
        sd_data(("%s: CMD%d\n", __FUNCTION__, cmd));
        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        /* Host controller reads 64 byte magic pattern from card
        *  Hence Direction = 1 ( READ )
        */
        xfer_reg = SFIELD(xfer_reg, XFER_DATA_DIRECTION, 1);
        break;

    case SDIOH_CMD_52:    /* IO R/W Direct (single byte) - Response R5 */

        sd_data(("%s: CMD52 func(%d) addr(0x%x) %s data(0x%x)\n",
            __FUNCTION__,
            GFIELD(arg, CMD52_FUNCTION),
            GFIELD(arg, CMD52_REG_ADDR),
            GFIELD(arg, CMD52_RW_FLAG) ? "W" : "R",
            GFIELD(arg, CMD52_DATA)));

        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);
        break;

    case SDIOH_CMD_53:    /* IO R/W Extended (multiple bytes/blocks) */

        sd_data(("%s: CMD53 func(%d) addr(0x%x) %s mode(%s) cnt(%d), %s\n",
            __FUNCTION__,
            GFIELD(arg, CMD53_FUNCTION),
            GFIELD(arg, CMD53_REG_ADDR),
            GFIELD(arg, CMD53_RW_FLAG) ? "W" : "R",
            GFIELD(arg, CMD53_BLK_MODE) ? "Block" : "Byte",
            GFIELD(arg, CMD53_BYTE_BLK_CNT),
            GFIELD(arg, CMD53_OP_CODE) ? "Incrementing addr" : "Single addr"));

        cmd_arg = arg;
        xfer_reg = 0;

        cmd_reg = SFIELD(cmd_reg, CMD_RESP_TYPE, RESP_TYPE_48);
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_DATA_EN, 1);
        cmd_reg = SFIELD(cmd_reg, CMD_TYPE, CMD_TYPE_NORMAL);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX, cmd);

        use_dma = USE_DMA(sdioh_info) && GFIELD(cmd_arg, CMD53_BLK_MODE);

        if (GFIELD(cmd_arg, CMD53_BLK_MODE)) {
            uint16 blocksize;
            uint16 blockcount;
            int func;

            ASSERT(sdioh_info->sd_blockmode);

            func = GFIELD(cmd_arg, CMD53_FUNCTION);
            blocksize = MIN((int)sdioh_info->data_xfer_count,
                            sdioh_info->client_block_size[func]);
            blockcount = GFIELD(cmd_arg, CMD53_BYTE_BLK_CNT);

            /* data_xfer_cnt is already setup so that for multiblock mode,
             * it is the entire buffer length.  For non-block or single block,
             * it is < 64 bytes
             */
            if (use_dma) {
                switch (sdioh_info->sd_dma_mode) {
                case DMA_MODE_SDMA:
                    sd_dma(("%s: SDMA: SysAddr reg was 0x%x now 0x%x\n",
                          __FUNCTION__, sdstd_rreg(sdioh_info, SD_SysAddr),
                         (uint32)sdioh_info->dma_phys));
                sdstd_wreg(sdioh_info, SD_SysAddr, sdioh_info->dma_phys);
                    break;
                case DMA_MODE_ADMA1:
                case DMA_MODE_ADMA2:
                    sd_dma(("%s: ADMA: Using ADMA\n", __FUNCTION__));
#ifdef BCMSDIOH_TXGLOM
                    /* multi-descriptor is currently used only for hc3 */
                    if ((sdioh_info->glom_info.count != 0) &&
                        (sdioh_info->txglom_mode == SDPCM_TXGLOM_MDESC)) {
                        uint32 i = 0;
                        for (i = 0;
                            i < sdioh_info->glom_info.count-1;
                            i++) {
                            glom_buf_t *glom_info;
                            glom_info = &(sdioh_info->glom_info);
                            sd_create_adma_descriptor(sdioh_info,
                                i,
                                glom_info->dma_phys_arr[i],
                                glom_info->nbytes[i],
                                ADMA2_ATTRIBUTE_VALID |
                                ADMA2_ATTRIBUTE_ACT_TRAN);
                        }

                        sd_create_adma_descriptor(sdioh_info,
                            i,
                            sdioh_info->glom_info.dma_phys_arr[i],
                            sdioh_info->glom_info.nbytes[i],
                            ADMA2_ATTRIBUTE_VALID |
                            ADMA2_ATTRIBUTE_END |
                            ADMA2_ATTRIBUTE_INT |
                            ADMA2_ATTRIBUTE_ACT_TRAN);
                    } else
#endif /* BCMSDIOH_TXGLOM */
                    {
                        sd_create_adma_descriptor(sdioh_info, 0,
                        sdioh_info->dma_phys, blockcount*blocksize,
                        ADMA2_ATTRIBUTE_VALID | ADMA2_ATTRIBUTE_END |
                        ADMA2_ATTRIBUTE_INT | ADMA2_ATTRIBUTE_ACT_TRAN);
                    }
                    /* Dump descriptor if DMA debugging is enabled. */
                    if (sd_msglevel & SDH_DMA_VAL) {
                        sd_dump_adma_dscr(sdioh_info);
                    }

                    sdstd_wreg(sdioh_info, SD_ADMA_SysAddr,
                               sdioh_info->adma2_dscr_phys);
                    break;
                default:
                    sd_err(("%s: unsupported DMA mode %d.\n",
                        __FUNCTION__, sdioh_info->sd_dma_mode));
                    break;
                }
            }

            sd_trace(("%s: Setting block count %d, block size %d bytes\n",
                      __FUNCTION__, blockcount, blocksize));
            sdstd_wreg16(sdioh_info, SD_BlockSize, blocksize);
            sdstd_wreg16(sdioh_info, SD_BlockCount, blockcount);

            xfer_reg = SFIELD(xfer_reg, XFER_DMA_ENABLE, use_dma);

            if (sdioh_info->client_block_size[func] != blocksize)
                set_client_block_size(sdioh_info, 1, blocksize);

            if (blockcount > 1) {
                xfer_reg = SFIELD(xfer_reg, XFER_MULTI_BLOCK, 1);
                xfer_reg = SFIELD(xfer_reg, XFER_BLK_COUNT_EN, 1);
                xfer_reg = SFIELD(xfer_reg, XFER_CMD_12_EN, 0);
            } else {
                xfer_reg = SFIELD(xfer_reg, XFER_MULTI_BLOCK, 0);
                xfer_reg = SFIELD(xfer_reg, XFER_BLK_COUNT_EN, 0);
                xfer_reg = SFIELD(xfer_reg, XFER_CMD_12_EN, 0);
            }

            if (GFIELD(cmd_arg, CMD53_RW_FLAG) == SDIOH_XFER_TYPE_READ)
                xfer_reg = SFIELD(xfer_reg, XFER_DATA_DIRECTION, 1);
            else
                xfer_reg = SFIELD(xfer_reg, XFER_DATA_DIRECTION, 0);

            retries = RETRIES_SMALL;
            while (GFIELD(sdstd_rreg(sdioh_info, SD_PresentState),
                          PRES_DAT_INHIBIT) && --retries)
                sd_err(("%s: Waiting for Data Inhibit cmd = %d\n",
                        __FUNCTION__, cmd));
            if (!retries) {
                sd_err(("%s: Data Inhibit timeout\n", __FUNCTION__));
                if (trap_errs)
                    ASSERT(0);
                return ERROR;
            }
#ifndef __IOPOS__
            sdstd_wreg16(sdioh_info, SD_TransferMode, xfer_reg);
#endif

        } else {    /* Non block mode */
            uint16 bytes = GFIELD(cmd_arg, CMD53_BYTE_BLK_CNT);
            /* The byte/block count field only has 9 bits,
             * so, to do a 512-byte bytemode transfer, this
             * field will contain 0, but we need to tell the
             * controller we're transferring 512 bytes.
             */
            if (bytes == 0) bytes = 512;

            if (use_dma)
                sdstd_wreg(sdioh_info, SD_SysAddr, sdioh_info->dma_phys);

            /* PCI: Transfer Mode register 0x0c */
            xfer_reg = SFIELD(xfer_reg, XFER_DMA_ENABLE, bytes <= 4 ? 0 : use_dma);
            xfer_reg = SFIELD(xfer_reg, XFER_CMD_12_EN, 0);
            if (GFIELD(cmd_arg, CMD53_RW_FLAG) == SDIOH_XFER_TYPE_READ)
                xfer_reg = SFIELD(xfer_reg, XFER_DATA_DIRECTION, 1);
            else
                xfer_reg = SFIELD(xfer_reg, XFER_DATA_DIRECTION, 0);
            /* See table 2-8 Host Controller spec ver 1.00 */
            xfer_reg = SFIELD(xfer_reg, XFER_BLK_COUNT_EN, 0); /* Dont care */
            xfer_reg = SFIELD(xfer_reg, XFER_MULTI_BLOCK, 0);

            sdstd_wreg16(sdioh_info, SD_BlockSize,  bytes);

            /* XXX This should be a don't care but Arasan needs it
             * to be one.  Its fixed in later versions (but they
             * don't have version numbers, sigh).
             */
            sdstd_wreg16(sdioh_info, SD_BlockCount, 1);

            retries = RETRIES_SMALL;
            while (GFIELD(sdstd_rreg(sdioh_info, SD_PresentState),
                          PRES_DAT_INHIBIT) && --retries)
                sd_err(("%s: Waiting for Data Inhibit cmd = %d\n",
                        __FUNCTION__, cmd));
            if (!retries) {
                sd_err(("%s: Data Inhibit timeout\n", __FUNCTION__));
                if (trap_errs)
                    ASSERT(0);
                return ERROR;
            }
#ifndef __IOPOS__
            sdstd_wreg16(sdioh_info, SD_TransferMode, xfer_reg);
#endif
        }
        break;

    default:
        sd_err(("%s: Unknown command\n", __FUNCTION__));
        return ERROR;
    }

    if (sdioh_info->sd_mode == SDIOH_MODE_SPI) {
        cmd_reg = SFIELD(cmd_reg, CMD_CRC_EN, 0);
        cmd_reg = SFIELD(cmd_reg, CMD_INDEX_EN, 0);
    }

    /* Setup and issue the SDIO command */
    sdstd_wreg(sdioh_info, SD_Arg0, arg);
#ifdef __IOPOS__
    /* XXX Fix me.  Adjust so all OS's act like IOPOS and delay writing
       all 32 bits until here.
     */
    {
        uint32 tmp32 = cmd_reg << 16;
        tmp32 |= xfer_reg;
        sdstd_wreg(sdioh_info, SD_TransferMode, tmp32);
    }
#else
    /* In response to CMD19 card sends 64 byte magic pattern.
     * So SD_BlockSize = 64 & SD_BlockCount = 1
    */
    if (GFIELD(cmd_reg, CMD_INDEX) == SDIOH_CMD_19) {
        sdstd_wreg16(sdioh_info, SD_TransferMode, xfer_reg);
        sdstd_wreg16(sdioh_info, SD_BlockSize, 64);
        sdstd_wreg16(sdioh_info, SD_BlockCount, 1);
    }
    sdstd_wreg16(sdioh_info, SD_Command, cmd_reg);
#endif    /* __IOPOS__ */

    /* If we are in polled mode, wait for the command to complete.
     * In interrupt mode, return immediately. The calling function will
     * know that the command has completed when the CMDATDONE interrupt
     * is asserted
     */
    if (sdioh_info->polled_mode) {
        uint16 int_reg = 0;
        retries = RETRIES_LARGE;

        /* For CMD19 no need to wait for cmd completion */
        if (GFIELD(cmd_reg, CMD_INDEX) == SDIOH_CMD_19)
            return SUCCESS;

        do {
            int_reg = sdstd_rreg16(sdioh_info, SD_IntrStatus);
            sdstd_os_yield(sdioh_info);
        } while (--retries &&
                 (GFIELD(int_reg, INTSTAT_ERROR_INT) == 0) &&
                 (GFIELD(int_reg, INTSTAT_CMD_COMPLETE) == 0));

        if (!retries) {
            sd_err(("%s: CMD_COMPLETE timeout: intrStatus: 0x%x "
                    "error stat 0x%x state 0x%x\n",
                    __FUNCTION__, int_reg,
                    sdstd_rreg16(sdioh_info, SD_ErrorIntrStatus),
                    sdstd_rreg(sdioh_info, SD_PresentState)));

            /* Attempt to reset CMD line when we get a CMD timeout */
            sdstd_wreg8(sdioh_info, SD_SoftwareReset, SFIELD(0, SW_RESET_CMD, 1));
            retries = RETRIES_LARGE;
            do {
                sd_trace(("%s: waiting for CMD line reset\n", __FUNCTION__));
            } while ((GFIELD(sdstd_rreg8(sdioh_info, SD_SoftwareReset),
                             SW_RESET_CMD)) && retries--);

            if (!retries) {
                sd_err(("%s: Timeout waiting for CMD line reset\n", __FUNCTION__));
            }

            if (trap_errs)
                ASSERT(0);
            return (ERROR);
        }

        /* Clear Command Complete interrupt */
        int_reg = SFIELD(0, INTSTAT_CMD_COMPLETE, 1);
        sdstd_wreg16(sdioh_info, SD_IntrStatus, int_reg);

        /* Check for Errors */
        if (sdstd_check_errs(sdioh_info, cmd, arg)) {
            if (trap_errs)
                ASSERT(0);
            return ERROR;
        }
    }
    return SUCCESS;
}

/*
 * XXX On entry: If single block or non-block, buffersize <= blocksize.
 * If Mulitblock, buffersize is unlimited.
 * Question is how to handle the leftovers in either single or multiblock.
 * I think the caller should break the buffer up so this routine will always
 * use blocksize == buffersize to handle the end piece of the buffer
 */

static int
sdstd_card_buf(sdioh_info_t *sd, int rw, int func, bool fifo, uint32 addr, int nbytes, uint32 *data)
{
    int status;
    uint32 cmd_arg;
    uint32 rsp5;
    uint16 int_reg, int_bit;
    uint flags;
    int num_blocks, blocksize;
    bool local_blockmode, local_dma;
    bool read = rw == SDIOH_READ ? 1 : 0;
    bool yield = FALSE;
#ifdef BCMSDIOH_TXGLOM
    uint32 i;
    uint8 *localbuf = NULL;
#endif

    ASSERT(nbytes);

    cmd_arg = 0;

    sd_data(("%s: %s 53 addr 0x%x, len %d bytes, r_cnt %d t_cnt %d\n",
             __FUNCTION__, read ? "Rd" : "Wr", addr, nbytes, sd->r_cnt, sd->t_cnt));

    if (read) sd->r_cnt++; else sd->t_cnt++;

    local_blockmode = sd->sd_blockmode;
    local_dma = USE_DMA(sd);

#ifdef BCMSDIOH_TXGLOM
    /* If multiple buffers are there, then calculate the nbytes from that */
    if (!read && (func == SDIO_FUNC_2) && (sd->glom_info.count != 0)) {
        uint32 i;
        nbytes = 0;
        for (i = 0; i < sd->glom_info.count; i++) {
            nbytes += sd->glom_info.nbytes[i];
        }
        ASSERT(nbytes <= sd->alloced_dma_size);
    }
#endif

    /* Don't bother with block mode on small xfers */
    if (nbytes < sd->client_block_size[func]) {
        sd_data(("setting local blockmode to false: nbytes (%d) != block_size (%d)\n",
                 nbytes, sd->client_block_size[func]));
        local_blockmode = FALSE;
        local_dma = FALSE;
#ifdef BCMSDIOH_TXGLOM
        /* In glommed case, create a single pkt from multiple pkts */
        if (!read && (func == SDIO_FUNC_2) && (sd->glom_info.count != 0)) {
            uint32 offset = 0;
            localbuf = (uint8 *)MALLOC(sd->osh, nbytes);
            data = (uint32 *)localbuf;
            for (i = 0; i < sd->glom_info.count; i++) {
                bcopy(sd->glom_info.dma_buf_arr[i],
                    ((uint8 *)data + offset),
                    sd->glom_info.nbytes[i]);
                offset += sd->glom_info.nbytes[i];
            }
        }
#endif
    }

    if (local_blockmode) {
        blocksize = MIN(sd->client_block_size[func], nbytes);
        num_blocks = nbytes/blocksize;
        cmd_arg = SFIELD(cmd_arg, CMD53_BYTE_BLK_CNT, num_blocks);
        cmd_arg = SFIELD(cmd_arg, CMD53_BLK_MODE, 1);
    } else {
        num_blocks =  1;
        blocksize = nbytes;
        cmd_arg = SFIELD(cmd_arg, CMD53_BYTE_BLK_CNT, nbytes);
        cmd_arg = SFIELD(cmd_arg, CMD53_BLK_MODE, 0);
    }

    if (local_dma && !read) {
#ifdef BCMSDIOH_TXGLOM
        if ((func == SDIO_FUNC_2) && (sd->glom_info.count != 0)) {
            /* In case of hc ver 2 DMA_MAP may not work properly due to 4K alignment
             * requirements. So copying pkt to 4K aligned pre-allocated pkt.
             * Total length should not cross the pre-alloced memory size
             */
            if (sd->txglom_mode == SDPCM_TXGLOM_CPY) {
                uint32 total_bytes = 0;
                for (i = 0; i < sd->glom_info.count; i++) {
                    bcopy(sd->glom_info.dma_buf_arr[i],
                        (uint8 *)sd->dma_buf + total_bytes,
                        sd->glom_info.nbytes[i]);
                    total_bytes += sd->glom_info.nbytes[i];
                }
                sd_sync_dma(sd, read, total_bytes);
            }
        } else
#endif /* BCMSDIOH_TXGLOM */
        {
            bcopy(data, sd->dma_buf, nbytes);
            sd_sync_dma(sd, read, nbytes);
        }
    }

    if (fifo)
        cmd_arg = SFIELD(cmd_arg, CMD53_OP_CODE, 0);    /* XXX SDIO spec v 1.10, Sec 5.3  */
    else
        cmd_arg = SFIELD(cmd_arg, CMD53_OP_CODE, 1);    /* XXX SDIO spec v 1.10, Sec 5.3  */

    cmd_arg = SFIELD(cmd_arg, CMD53_FUNCTION, func);
    cmd_arg = SFIELD(cmd_arg, CMD53_REG_ADDR, addr);
    if (read)
        cmd_arg = SFIELD(cmd_arg, CMD53_RW_FLAG, SDIOH_XFER_TYPE_READ);
    else
        cmd_arg = SFIELD(cmd_arg, CMD53_RW_FLAG, SDIOH_XFER_TYPE_WRITE);

    sd->data_xfer_count = nbytes;

    /* sdstd_cmd_issue() returns with the command complete bit
     * in the ISR already cleared
     */
    if ((status = sdstd_cmd_issue(sd, local_dma, SDIOH_CMD_53, cmd_arg)) != SUCCESS) {
        sd_err(("%s: cmd_issue failed for %s\n", __FUNCTION__, (read ? "read" : "write")));
        return status;
    }

    sdstd_cmd_getrsp(sd, &rsp5, 1);

    if ((flags = GFIELD(rsp5, RSP5_FLAGS)) != 0x10) {
        sd_err(("%s: Rsp5: nbytes %d, dma %d blockmode %d, read %d "
                "numblocks %d, blocksize %d\n",
                __FUNCTION__, nbytes, local_dma, local_dma, read, num_blocks, blocksize));

        if (flags & 1)
            sd_err(("%s: rsp5: Command not accepted: arg out of range 0x%x, "
                    "bytes %d dma %d\n",
                    __FUNCTION__, flags, GFIELD(cmd_arg, CMD53_BYTE_BLK_CNT),
                    GFIELD(cmd_arg, CMD53_BLK_MODE)));
        if (flags & 0x8)
            sd_err(("%s: Rsp5: General Error\n", __FUNCTION__));

        sd_err(("%s: rsp5 flags = 0x%x, expecting 0x10 returning error\n",
                __FUNCTION__,  flags));
        if (trap_errs)
            ASSERT(0);
        return ERROR;
    }

    if (GFIELD(rsp5, RSP5_STUFF))
        sd_err(("%s: rsp5 stuff is 0x%x: expecting 0\n",
                __FUNCTION__, GFIELD(rsp5, RSP5_STUFF)));

#ifdef BCMSDYIELD
    yield = sd_yieldcpu && ((uint)nbytes >= sd_minyield);
#endif

    if (!local_dma) {
        int bytes, i;
        uint32 tmp;
#ifdef __IOPOS__
        uint32 *tmp_swap = data;
#endif
        for (i = 0; i < num_blocks; i++) {
            int words;

            /* Decide which status bit we're waiting for */
            if (read)
                int_bit = SFIELD(0, INTSTAT_BUF_READ_READY, 1);
            else
                int_bit = SFIELD(0, INTSTAT_BUF_WRITE_READY, 1);

            /* If not on, wait for it (or for xfer error) */
            int_reg = sdstd_rreg16(sd, SD_IntrStatus);
            if (!(int_reg & int_bit)) {
                status = sdstd_waitbits(sd, int_bit, ERRINT_TRANSFER_ERRS,
                                        yield, &int_reg);
                switch (status) {
                case -1:
                    sd_err(("%s: pio interrupted\n", __FUNCTION__));
                    return ERROR;
                case -2:
                    sd_err(("%s: pio timeout waiting for interrupt\n",
                            __FUNCTION__));
                    return ERROR;
                }
            }
#ifdef BCMSLTGT
        /*    int_reg = sdstd_rreg16(sd, SD_IntrStatus); */
#endif
            /* Confirm we got the bit w/o error */
            if (!(int_reg & int_bit) || GFIELD(int_reg, INTSTAT_ERROR_INT)) {
                sd_err(("%s: Error or timeout for Buf_%s_Ready: intStat: 0x%x "
                        "errint: 0x%x PresentState 0x%x\n",
                        __FUNCTION__, read ? "Read" : "Write", int_reg,
                        sdstd_rreg16(sd, SD_ErrorIntrStatus),
                        sdstd_rreg(sd, SD_PresentState)));
                sdstd_dumpregs(sd);
                sdstd_check_errs(sd, SDIOH_CMD_53, cmd_arg);
                return (ERROR);
            }

            /* Clear Buf Ready bit */
            sdstd_wreg16(sd, SD_IntrStatus, int_bit);

#ifdef __IOPOS__
            /* PIO needs to be swapped on Nintendo platform */
            ASSERT(((blocksize/4) * 4) == blocksize);
            if (!read) {
                for (words = blocksize >> 2; words; words--) {
                    *tmp_swap = BCMSWAP32(*tmp_swap);
                    tmp_swap++;
                }
            }
#endif
            /* At this point we have Buffer Ready, write the data 4 bytes at a time */
            for (words = blocksize/4; words; words--) {
                if (read)
                    *data = sdstd_rreg(sd, SD_BufferDataPort0);
                else
                    sdstd_wreg(sd, SD_BufferDataPort0, *data);
                data++;
            }
#ifdef __IOPOS__
            if (read) {
                /* PIO needs to be swapped on Nintendo platform */
                for (words = blocksize/4; words; words--) {
                    *tmp_swap = BCMSWAP32(*tmp_swap);
                    tmp_swap++;
                }
            }
#endif

            /* XXX
             * Handle < 4 bytes.  wlc_pio.c currently (as of 12/20/05) truncates buflen
             * to be evenly divisible by 4.  However dongle passes arbitrary lengths,
             * so handle it here
             */
            bytes = blocksize % 4;

            /* If no leftover bytes, go to next block */
            if (!bytes)
                continue;

            switch (bytes) {
            case 1:
                /* R/W 8 bits */
                if (read)
                    *(data++) = (uint32)(sdstd_rreg8(sd, SD_BufferDataPort0));
                else
                    sdstd_wreg8(sd, SD_BufferDataPort0,
                                (uint8)(*(data++) & 0xff));
                break;
            case 2:
                /* R/W 16 bits */
                if (read)
                    *(data++) = (uint32)sdstd_rreg16(sd, SD_BufferDataPort0);
                else
                    sdstd_wreg16(sd, SD_BufferDataPort0, (uint16)(*(data++)));
                break;
            case 3:
                /* R/W 24 bits:
                 * SD_BufferDataPort0[0-15] | SD_BufferDataPort1[16-23]
                 */
                if (read) {
                    tmp = (uint32)sdstd_rreg16(sd, SD_BufferDataPort0);
                    tmp |= ((uint32)(sdstd_rreg8(sd,
                                                 SD_BufferDataPort1)) << 16);
                    *(data++) = tmp;
                } else {
                    tmp = *(data++);
                    sdstd_wreg16(sd, SD_BufferDataPort0, (uint16)tmp & 0xffff);
                    sdstd_wreg8(sd, SD_BufferDataPort1,
                                (uint8)((tmp >> 16) & 0xff));
                }
                break;
            default:
                sd_err(("%s: Unexpected bytes leftover %d\n",
                        __FUNCTION__, bytes));
                ASSERT(0);
                break;
            }
        }
    }    /* End PIO processing */

    /* Wait for Transfer Complete or Transfer Error */
    int_bit = SFIELD(0, INTSTAT_XFER_COMPLETE, 1);

    /* If not on, wait for it (or for xfer error) */
    int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    if (!(int_reg & int_bit)) {
        status = sdstd_waitbits(sd, int_bit, ERRINT_TRANSFER_ERRS, yield, &int_reg);
        switch (status) {
        case -1:
            sd_err(("%s: interrupted\n", __FUNCTION__));
            return ERROR;
        case -2:
            sd_err(("%s: timeout waiting for interrupt\n", __FUNCTION__));
            return ERROR;
        }
    }

    /* Check for any errors from the data phase */
    if (sdstd_check_errs(sd, SDIOH_CMD_53, cmd_arg))
        return ERROR;

    /* May have gotten a software timeout if not blocking? */
    int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    if (!(int_reg & int_bit)) {
        sd_err(("%s: Error or Timeout for xfer complete; %s, dma %d, State 0x%08x, "
                "intr 0x%04x, Err 0x%04x, len = %d, rcnt %d, tcnt %d\n",
                __FUNCTION__, read ? "R" : "W", local_dma,
                sdstd_rreg(sd, SD_PresentState), int_reg,
                sdstd_rreg16(sd, SD_ErrorIntrStatus), nbytes,
                sd->r_cnt, sd->t_cnt));
        sdstd_dumpregs(sd);
        return ERROR;
    }

    /* Clear the status bits */
    int_reg = int_bit;
    if (local_dma) {
        /* DMA Complete */
        /* XXX Step 14, Section 3.6.2.2 Stnd Cntrlr Spec */
        /* Reads in particular don't have DMA_COMPLETE set */
        int_reg = SFIELD(int_reg, INTSTAT_DMA_INT, 1);
    }
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    /* Fetch data */
    if (local_dma && read) {
        sd_sync_dma(sd, read, nbytes);
        bcopy(sd->dma_buf, data, nbytes);
    }
#ifdef BCMSDIOH_TXGLOM
    if (localbuf)
        MFREE(sd->osh, localbuf, nbytes);
#endif
    return SUCCESS;
}

static int
set_client_block_size(sdioh_info_t *sd, int func, int block_size)
{
    int base;
    int err = 0;


    sd_err(("%s: Setting block size %d, func %d\n", __FUNCTION__, block_size, func));
    sd->client_block_size[func] = block_size;

    /* Set the block size in the SDIO Card register */
    base = func * SDIOD_FBR_SIZE;
    err = sdstd_card_regwrite(sd, 0, base+SDIOD_CCCR_BLKSIZE_0, 1, block_size & 0xff);
    if (!err) {
        err = sdstd_card_regwrite(sd, 0, base+SDIOD_CCCR_BLKSIZE_1, 1,
                                  (block_size >> 8) & 0xff);
    }

    /* Do not set the block size in the SDIO Host register, that
     * is func dependent and will get done on an individual
     * transaction basis
     */

    return (err ? BCME_SDIO_ERROR : 0);
}

/* Reset and re-initialize the device */
int
sdioh_sdio_reset(sdioh_info_t *si)
{
    uint8 hreg;

    /* Reset the attached device (use slower clock for safety) */
    if (!sdstd_start_clock(si, 128)) {
            sd_err(("set clock failed!\n"));
            return ERROR;
            }
    sdstd_reset(si, 0, 1);

    /* Reset portions of the host state accordingly */
    hreg = sdstd_rreg8(si, SD_HostCntrl);
    hreg = SFIELD(hreg, HOST_HI_SPEED_EN, 0);
    hreg = SFIELD(hreg, HOST_DATA_WIDTH, 0);
    si->sd_mode = SDIOH_MODE_SD1;

    /* Reinitialize the card */
    si->card_init_done = FALSE;
    return si->client_init(si);
}

#ifdef BCMINTERNAL
#ifdef NOTUSED
static void
cis_fetch(sdioh_info_t *sd, int func, char *data, int len)
{
    int count;
    int offset;
    char *end = data + len;
    uint32 foo;

    for (count = 0; count < 512 && data < end; count++) {
        offset =  sd->func_cis_ptr[func] + count;
        if (sdstd_card_regread (sd, func, offset, 1, &foo) < 0) {
            sd_err(("%s: regread failed\n", __FUNCTION__));
            return;
        }
        data += sprintf(data, "%.2x ", foo & 0xff);
        if (((count+1) % 16) == 0)
            data += sprintf(data, "\n");
    }
}
#endif /* NOTUSED */
#endif /* BCMINTERNAL */

static void
sd_map_dma(sdioh_info_t * sd)
{

    uint alloced;
    void *va;
    uint dma_buf_size = SD_PAGE;

#ifdef BCMSDIOH_TXGLOM
    /* There is no alignment requirement for HC3 */
    if ((sd->version == HOST_CONTR_VER_3) && sd_txglom) {
        /* Max glom packet length is 64KB */
        dma_buf_size = SD_PAGE * 16;
    }
#endif

    alloced = 0;
    if ((va = DMA_ALLOC_CONSISTENT(sd->osh, dma_buf_size, SD_PAGE_BITS, &alloced,
        &sd->dma_start_phys, 0x12)) == NULL) {
        sd->sd_dma_mode = DMA_MODE_NONE;
        sd->dma_start_buf = 0;
        sd->dma_buf = (void *)0;
        sd->dma_phys = 0;
        sd->alloced_dma_size = 0;
        sd_err(("%s: DMA_ALLOC failed. Disabling DMA support.\n", __FUNCTION__));
    } else {
        sd->dma_start_buf = va;
        sd->dma_buf = (void *)ROUNDUP((uintptr)va, SD_PAGE);
        sd->dma_phys = ROUNDUP((sd->dma_start_phys), SD_PAGE);
        sd->alloced_dma_size = alloced;
        sd_err(("%s: Mapped DMA Buffer %dbytes @virt/phys: %p/0x%lx\n",
                __FUNCTION__, sd->alloced_dma_size, sd->dma_buf, sd->dma_phys));
        sd_fill_dma_data_buf(sd, 0xA5);
    }

    if ((va = DMA_ALLOC_CONSISTENT(sd->osh, SD_PAGE, SD_PAGE_BITS, &alloced,
        &sd->adma2_dscr_start_phys, 0x12)) == NULL) {
        sd->sd_dma_mode = DMA_MODE_NONE;
        sd->adma2_dscr_start_buf = 0;
        sd->adma2_dscr_buf = (void *)0;
        sd->adma2_dscr_phys = 0;
        sd->alloced_adma2_dscr_size = 0;
        sd_err(("%s: DMA_ALLOC failed for descriptor buffer. "
                "Disabling DMA support.\n", __FUNCTION__));
    } else {
        sd->adma2_dscr_start_buf = va;
        sd->adma2_dscr_buf = (void *)ROUNDUP((uintptr)va, SD_PAGE);
        sd->adma2_dscr_phys = ROUNDUP((sd->adma2_dscr_start_phys), SD_PAGE);
        sd->alloced_adma2_dscr_size = alloced;
        sd_err(("%s: Mapped ADMA2 Descriptor Buffer %dbytes @virt/phys: %p/0x%lx\n",
                __FUNCTION__, sd->alloced_adma2_dscr_size, sd->adma2_dscr_buf,
                sd->adma2_dscr_phys));
        sd_clear_adma_dscr_buf(sd);
    }
}

static void
sd_unmap_dma(sdioh_info_t * sd)
{
    if (sd->dma_start_buf) {
        DMA_FREE_CONSISTENT(sd->osh, sd->dma_start_buf, sd->alloced_dma_size,
            sd->dma_start_phys, 0x12);
    }

    if (sd->adma2_dscr_start_buf) {
        DMA_FREE_CONSISTENT(sd->osh, sd->adma2_dscr_start_buf, sd->alloced_adma2_dscr_size,
                            sd->adma2_dscr_start_phys, 0x12);
    }
}

static void
sd_clear_adma_dscr_buf(sdioh_info_t *sd)
{
    bzero((char *)sd->adma2_dscr_buf, SD_PAGE);
    sd_dump_adma_dscr(sd);
}

static void
sd_fill_dma_data_buf(sdioh_info_t *sd, uint8 data)
{
    memset((char *)sd->dma_buf, data, SD_PAGE);
}

static void
sd_create_adma_descriptor(sdioh_info_t *sd, uint32 index,
                          uint32 addr_phys, uint16 length, uint16 flags)
{
    adma2_dscr_32b_t *adma2_dscr_table;
    adma1_dscr_t *adma1_dscr_table;

    adma2_dscr_table = sd->adma2_dscr_buf;
    adma1_dscr_table = sd->adma2_dscr_buf;

    switch (sd->sd_dma_mode) {
        case DMA_MODE_ADMA2:
            sd_dma(("%s: creating ADMA2 descriptor for index %d\n",
                __FUNCTION__, index));

            adma2_dscr_table[index].phys_addr = addr_phys;
            adma2_dscr_table[index].len_attr = length << 16;
            adma2_dscr_table[index].len_attr |= flags;
            break;
        case DMA_MODE_ADMA1:
            /* ADMA1 requires two descriptors, one for len
             * and the other for data transfer
             */
            index <<= 1;

            sd_dma(("%s: creating ADMA1 descriptor for index %d\n",
                __FUNCTION__, index));

            adma1_dscr_table[index].phys_addr_attr = length << 12;
            adma1_dscr_table[index].phys_addr_attr |= (ADMA1_ATTRIBUTE_ACT_SET |
                                                       ADMA2_ATTRIBUTE_VALID);
            adma1_dscr_table[index+1].phys_addr_attr = addr_phys & 0xFFFFF000;
            adma1_dscr_table[index+1].phys_addr_attr |= (flags & 0x3f);
            break;
        default:
            sd_err(("%s: cannot create ADMA descriptor for DMA mode %d\n",
                __FUNCTION__, sd->sd_dma_mode));
            break;
    }
}

static void
sd_dump_adma_dscr(sdioh_info_t *sd)
{
    adma2_dscr_32b_t *adma2_dscr_table;
    adma1_dscr_t *adma1_dscr_table;
    uint32 i = 0;
    uint16 flags;
    char flags_str[32];

    ASSERT(sd->adma2_dscr_buf != NULL);

    adma2_dscr_table = sd->adma2_dscr_buf;
    adma1_dscr_table = sd->adma2_dscr_buf;

    switch (sd->sd_dma_mode) {
        case DMA_MODE_ADMA2:
            sd_err(("ADMA2 Descriptor Table (%dbytes) @virt/phys: %p/0x%lx\n",
                SD_PAGE, sd->adma2_dscr_buf, sd->adma2_dscr_phys));
            sd_err((" #[Descr VA  ]  Buffer PA  | Len    | Flags  (5:4  2   1   0)"
                    "     |\n"));
            while (adma2_dscr_table->len_attr & ADMA2_ATTRIBUTE_VALID) {
                flags = adma2_dscr_table->len_attr & 0xFFFF;
                sprintf(flags_str, "%s%s%s%s",
                    ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                    ADMA2_ATTRIBUTE_ACT_LINK) ? "LINK " :
                    ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                    ADMA2_ATTRIBUTE_ACT_TRAN) ? "TRAN " :
                    ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                    ADMA2_ATTRIBUTE_ACT_NOP) ? "NOP  " : "RSV  ",
                    (flags & ADMA2_ATTRIBUTE_INT ? "INT " : "    "),
                    (flags & ADMA2_ATTRIBUTE_END ? "END " : "    "),
                    (flags & ADMA2_ATTRIBUTE_VALID ? "VALID" : ""));
                sd_err(("%2d[0x%p]: 0x%08x | 0x%04x | 0x%04x (%s) |\n",
                        i, adma2_dscr_table, adma2_dscr_table->phys_addr,
                        adma2_dscr_table->len_attr >> 16, flags, flags_str));
                i++;

#ifdef linux
                /* Follow LINK descriptors or skip to next. */
                if ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                     ADMA2_ATTRIBUTE_ACT_LINK) {
                    adma2_dscr_table = phys_to_virt(
                        adma2_dscr_table->phys_addr);
                } else {
                    adma2_dscr_table++;
                }
#else
                adma2_dscr_table++;
#endif /* linux */

            }
            break;
        case DMA_MODE_ADMA1:
            sd_err(("ADMA1 Descriptor Table (%dbytes) @virt/phys: %p/0x%lx\n",
                     SD_PAGE, sd->adma2_dscr_buf, sd->adma2_dscr_phys));
            sd_err((" #[Descr VA  ]  Buffer PA  | Flags  (5:4  2   1   0)     |\n"));

            for (i = 0; adma1_dscr_table->phys_addr_attr & ADMA2_ATTRIBUTE_VALID; i++) {
                flags = adma1_dscr_table->phys_addr_attr & 0x3F;
                sprintf(flags_str, "%s%s%s%s",
                    ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                    ADMA2_ATTRIBUTE_ACT_LINK) ? "LINK " :
                    ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                    ADMA2_ATTRIBUTE_ACT_TRAN) ? "TRAN " :
                    ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                    ADMA2_ATTRIBUTE_ACT_NOP) ? "NOP  " : "SET  ",
                    (flags & ADMA2_ATTRIBUTE_INT ? "INT " : "    "),
                    (flags & ADMA2_ATTRIBUTE_END ? "END " : "    "),
                    (flags & ADMA2_ATTRIBUTE_VALID ? "VALID" : ""));
                sd_err(("%2d[0x%p]: 0x%08x | 0x%04x | (%s) |\n",
                        i, adma1_dscr_table,
                        adma1_dscr_table->phys_addr_attr & 0xFFFFF000,
                        flags, flags_str));

#ifdef linux
                /* Follow LINK descriptors or skip to next. */
                if ((flags & ADMA2_ATTRIBUTE_ACT_LINK) ==
                     ADMA2_ATTRIBUTE_ACT_LINK) {
                    adma1_dscr_table = phys_to_virt(
                        adma1_dscr_table->phys_addr_attr & 0xFFFFF000);
                } else {
                    adma1_dscr_table++;
                }
#else
                adma2_dscr_table++;
#endif /* linux */
            }
            break;
        default:
            sd_err(("Unknown DMA Descriptor Table Format.\n"));
            break;
    }
}

static void
sdstd_dumpregs(sdioh_info_t *sd)
{
    sd_err(("IntrStatus:       0x%04x ErrorIntrStatus       0x%04x\n",
                sdstd_rreg16(sd, SD_IntrStatus),
                sdstd_rreg16(sd, SD_ErrorIntrStatus)));
    sd_err(("IntrStatusEnable: 0x%04x ErrorIntrStatusEnable 0x%04x\n",
                sdstd_rreg16(sd, SD_IntrStatusEnable),
                sdstd_rreg16(sd, SD_ErrorIntrStatusEnable)));
    sd_err(("IntrSignalEnable: 0x%04x ErrorIntrSignalEnable 0x%04x\n",
                sdstd_rreg16(sd, SD_IntrSignalEnable),
                sdstd_rreg16(sd, SD_ErrorIntrSignalEnable)));
}

/* iovar table lookup */
/* XXX could mandate sorted tables and do a binary search */
const bcm_iovar_t*
bcm_iovar_lookup(const bcm_iovar_t *table, const char *name)
{
    const bcm_iovar_t *vi;
    const char *lookup_name;

    /* skip any ':' delimited option prefixes */
    lookup_name = strrchr(name, ':');
    if (lookup_name != NULL)
        lookup_name++;
    else
        lookup_name = name;

    ASSERT(table != NULL);

    for (vi = table; vi->name; vi++) {
        if (!strcmp(vi->name, lookup_name))
            return vi;
    }
    /* ran to end of table */

    return NULL; /* var name not found */
}

int
bcm_iovar_lencheck(const bcm_iovar_t *vi, void *arg, int len, bool set)
{
    int bcmerror = 0;

    /* length check on io buf */
    switch (vi->type) {
    case IOVT_BOOL:
    case IOVT_INT8:
    case IOVT_INT16:
    case IOVT_INT32:
    case IOVT_UINT8:
    case IOVT_UINT16:
    case IOVT_UINT32:
        /* all integers are int32 sized args at the ioctl interface */
        if (len < (int)sizeof(int)) {
            bcmerror = BCME_BUFTOOSHORT;
        }
        break;

    case IOVT_BUFFER:
        /* buffer must meet minimum length requirement */
        if (len < vi->minlen) {
            bcmerror = BCME_BUFTOOSHORT;
        }
        break;

    case IOVT_VOID:
        if (!set) {
            /* Cannot return nil... */
            bcmerror = BCME_UNSUPPORTED;
        } else if (len) {
            /* Set is an action w/o parameters */
            bcmerror = BCME_BUFTOOLONG;
        }
        break;

    default:
        /* unknown type for length check in iovar info */
        ASSERT(0);
        bcmerror = BCME_UNSUPPORTED;
    }

    return bcmerror;
}

/*
    issue acmd
*/
static int
sdstd_acmd_issue(sdioh_info_t *sd, bool use_dma, uint32 cmd, uint32 arg)
{
    uint32 cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);

    if (sdstd_cmd_issue(sd, use_dma, SDIOH_CMD_55, cmd_arg)
            != SUCCESS)
    {
        return ERROR;
    }

    return (sdstd_cmd_issue(sd, use_dma, cmd, arg));
}

static int
sdstd_parse_csd(sdioh_info_t *sd)
{
    uint32 CSD_STRUCTURE = sd->CSD[3] >> 30;
    uint32 READ_BL_LEN, C_SIZE, C_SIZE_MULT, BLOCKNR, MULT, BLOCK_LEN;

    if (CSD_STRUCTURE == 0)
    {
        READ_BL_LEN = (sd->CSD[2] >> 16) & 0xf;
        C_SIZE = ((sd->CSD[1] >> 30) & 0x2) | ((sd->CSD[2] & 0x3ff) << 2);
        C_SIZE_MULT = (sd->CSD[1] >> 15) & 0x7;
        BLOCK_LEN = 1<<READ_BL_LEN;
        MULT = 1<<(C_SIZE_MULT+2);
        BLOCKNR = (C_SIZE + 1) * MULT;
        sd->csd_blocknr = BLOCKNR;
        sd->csd_block_len = BLOCK_LEN;
        sd->csd_blocknr_512 = BLOCKNR * (BLOCK_LEN / 512);
        sd->csd_sdsc = 1;
    }
    else if (CSD_STRUCTURE == 1)
    {
        BLOCK_LEN = 512;
        MULT = 1024;
        C_SIZE = ((sd->CSD[1] & 0xffff0000) >> 16) | ((sd->CSD[2] & 0xff) << 16);
        BLOCKNR = (C_SIZE + 1) * MULT;
        sd->csd_blocknr = sd->csd_blocknr_512 = BLOCKNR;
        sd->csd_block_len = BLOCK_LEN;
        sd->csd_sdsc = 0;
    }
    else
    {
        sd_err(("%s: CSD_STRUCTURE is invalid\n", __FUNCTION__));
        return ERROR;
    }

    return SUCCESS;
}

#define ACMD41_RETRIES 20000
static int
get_sdmmc_ocr(sdioh_info_t *sd, uint32 *cmd_arg, uint32 *cmd_rsp, uint32 acmd41_hcs)
{
    int retries, status;

    /* Get the Card's Operation Condition.  Occasionally the board
     * takes a while to become ready
     */
    retries = ACMD41_RETRIES;
    do {
        *cmd_rsp = 0;
        OSL_DELAY(100);
        if ((status = sdstd_acmd_issue(sd, USE_DMA(sd), SDIOH_ACMD_41, *cmd_arg))
            != SUCCESS) {
            sd_err(("%s: ACMD41 failed\n", __FUNCTION__));
            return status;
        }
        sdstd_cmd_getrsp(sd, cmd_rsp, 1);
        *cmd_arg = *cmd_rsp | acmd41_hcs;
        if (!GFIELD(*cmd_rsp, RSP4_CARD_READY))
            sd_trace(("%s: Waiting for card to become ready\n", __FUNCTION__));
    } while ((!GFIELD(*cmd_rsp, RSP4_CARD_READY)) && --retries);
    if (!retries)
        return ERROR;

    return (SUCCESS);
}

static int
get_mmc_ocr(sdioh_info_t *sd, uint32 *cmd_arg, uint32 *cmd_rsp)
{
    int retries;

    /*
     * perform CMD0 first before issuing any commands
     * GO_IDLE_STATE
     */
    if (sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_0, 0)
        != SUCCESS) {
        sd_err(("BCMSDIOH: cardinit: CMD0 failed!\n"));
        return FALSE;
    }

    /* Get the Card's Operation Condition.  Occasionally the board
     * takes a while to become ready
     */
    retries = ACMD41_RETRIES;
    do {
        *cmd_rsp = 0;
        OSL_DELAY(100);
        if (sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_1, *cmd_arg)
            != SUCCESS) {
            sd_err(("%s: CMD1 failed\n", __FUNCTION__));
            return FALSE;
        }
        sdstd_cmd_getrsp(sd, cmd_rsp, 1);
        if (!GFIELD(*cmd_rsp, RSP4_CARD_READY))
            sd_trace(("%s: Waiting for card to become ready\n", __FUNCTION__));
    } while ((!GFIELD(*cmd_rsp, RSP4_CARD_READY)) && --retries);
    if (!retries)
        return FALSE;

    sd_info(("%s: cmd_rsp: 0x%08x\n", __FUNCTION__, *cmd_rsp));

    return (TRUE);
}

static uint
detect_card_type(sdioh_info_t *sd)
{
    /*
     * perform CMD0 first before issuing any commands
     * GO_IDLE_STATE
     */
    if (sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_0, 0)
        != SUCCESS) {
        sd_err(("BCMSDIOH: cardinit: CMD0 failed!\n"));
        return FALSE;
    }

    OSL_DELAY(100);
    if (sdstd_acmd_issue(sd, USE_DMA(sd), SDIOH_ACMD_41, 0)
        != SUCCESS) {
        sd_err(("%s: ACMD41 failed\n", __FUNCTION__));
        sd_info(("MMC Card detected.\n"));
        return MMC_CARD;
    }
    sd_info(("SD Card detected.\n"));
    return SD_CARD;
}

static uint16
sdstd_sd_start_power(sdioh_info_t *sd)
{
    uint32 cmd_arg;
    uint32 cmd_rsp;
    uint32 acmd41_hcs = (1<<30);

    /*
     * perform CMD0 first before issuing any commands
     * GO_IDLE_STATE
     */
    cmd_arg = 0;
    if (sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_0, cmd_arg)
        != SUCCESS) {
        sd_err(("BCMSDIOH: cardinit: CMD0 failed!\n"));
        return FALSE;
    }

    /*
     * SEND_IF_COND
     */

    cmd_arg = 0x1AA;
    if (sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_8, cmd_arg)
        != SUCCESS) {
        sd_err(("BCMSDIOH: cardinit: CMD8 failed!\n"));
        acmd41_hcs = 0;
    }

    /* Get the Card's Operation Condition.  Occasionally the board
     * takes a while to become ready
     */
    cmd_arg = 0;
    cmd_rsp = 0;
    if (get_sdmmc_ocr(sd, &cmd_arg, &cmd_rsp, acmd41_hcs) != SUCCESS) {
        sd_err(("%s: Failed to get OCR bailing\n", __FUNCTION__));
        /* No need to reset as not sure in what state the card is. */
        return SDIO_OCR_READ_FAIL;
    }

    sd_info(("cmd_rsp = 0x%x\n", cmd_rsp));
    sd_info(("mem_present = %d\n", GFIELD(cmd_rsp, RSP4_MEM_PRESENT)));
    sd_info(("num_funcs = %d\n", GFIELD(cmd_rsp, RSP4_NUM_FUNCS)));
    sd_info(("card_ready = %d\n", GFIELD(cmd_rsp, RSP4_CARD_READY)));
    sd_info(("OCR = 0x%x\n", GFIELD(cmd_rsp, RSP4_IO_OCR)));

    sd->num_funcs = GFIELD(cmd_rsp, RSP4_NUM_FUNCS);

    /* Examine voltage: Arasan only supports 3.3 volts,
     * so look for 3.2-3.3 Volts and also 3.3-3.4 volts.
     */

    /* XXX Pg 10 SDIO spec v1.10 */
    if ((GFIELD(cmd_rsp, RSP4_IO_OCR) & (0x3 << 20)) == 0) {
        sd_err(("This client does not support 3.3 volts!\n"));
        return ERROR;
    }
    sd_info(("Leaving bus power at 3.3 Volts\n"));

    return TRUE;
}

static uint16
sdstd_mmc_start_power(sdioh_info_t *sd)
{
    uint32 cmd_arg;
    uint32 cmd_rsp;

    cmd_arg = 0x40FF8000;
    cmd_rsp = 0;

    return get_mmc_ocr(sd, &cmd_arg, &cmd_rsp);
}

static uint16
sdstd_sdmmc_set_clock(sdioh_info_t *sd, uint32 clock)
{
    uint8 baseclk = 0;
    uint16 init_divider = 0;

    baseclk = GFIELD(sd->caps, CAP_BASECLK);
    sd_info(("%s:baseclk: %d MHz\n",    __FUNCTION__, baseclk));
    /* for 3.0, find divisor */
    if (sd->version == HOST_CONTR_VER_3) {
        /* ToDo : Dynamic modification of preset value table based on base clk */
        sd3_trace(("sd3: %s: checking divisor\n", __FUNCTION__));
        if (GFIELD(sd->caps3, CAP3_CLK_MULT) != 0)    {
            sd_err(("%s:Possible error: CLK Mul 1 CLOCKING NOT supported!\n",
                __FUNCTION__));
            return FALSE;
        } else {
            /*  calculate dividor */
            init_divider = baseclk*1000/clock;
            /* make it a multiple of 2. */
            init_divider += (init_divider & 0x1);
            sd_err(("%s:divider used for init:%d\n",
                __FUNCTION__, init_divider));
        }
    } else {
        /* Note: sd_divisor assumes that SDIO Base CLK is 50MHz. */
        int final_freq_based_on_div = 50/sd_divisor;
        if (baseclk > 50)
            sd_divisor = baseclk/final_freq_based_on_div;
        /* TBD: merge both SDIO 2.0 and 3.0 to share same divider logic */
            init_divider = baseclk*1000/clock;
        /* find next power of 2 */
        NEXT_POW2(init_divider);
        sd_err(("%s:NONUHSI: divider used for init:%d\n",
            __FUNCTION__, init_divider));
    }

    /* Start at ~400KHz clock rate for initialization */
#ifndef __IOPOS__
    if (!sdstd_start_clock(sd, init_divider)) {
#else
    /* XXX I wonder if this special case of a 2KHz clock is still necessary
     * after cleaning up some issues in the initialization logic.  See PR's 87546,
     * 87548 and 87550.  It would be great if somebody with the IOPOS hardware
     * could investigate this a bit and remove the special case if possible.  johnvb
     */
    if (!sdstd_start_clock(sd, 2)) {
#endif
        sd_err(("%s: sdstd_start_clock failed\n", __FUNCTION__));
        return FALSE;
    }

    return TRUE;
}

static uint16
sdstd_sdmmc_start_power(sdioh_info_t *sd, int volts_req)
{
    char *s;
    uint8 pwr = 0;
    int volts = 0;
    uint16 val1;

    bool selhighest = (volts_req == 0) ? TRUE : FALSE;

    /* reset the card uhsi volt support to false */
    sd->card_UHSI_voltage_Supported = FALSE;

    /* Ensure a power on reset by turning off bus power in case it happened to
     * be on already. (This might happen if driver doesn't unload/clean up correctly,
     * crash, etc.)  Leave off for 100ms to make sure the power off isn't
     * ignored/filtered by the device.  Note we can't skip this step if the power is
     * off already since we don't know how long it has been off before starting
     * the driver.
     */
    sdstd_wreg8(sd, SD_PwrCntrl, 0);
    sd_info(("Turning off VDD/bus power briefly (100ms) to ensure reset\n"));
    OSL_DELAY(100000);

    /* For selecting highest available voltage, start from lowest and iterate */
    if (!volts_req)
        volts_req = 1;

    s = NULL;

    if (volts_req == 1) {
        if (GFIELD(sd->caps, CAP_VOLT_1_8)) {
            volts = 5;
            s = "1.8";
            if (FALSE == selhighest)
                goto voltsel;
            else
                volts_req++;
        } else {
            sd_err(("HC doesn't support voltage! trying higher voltage: %d\n", volts));
            volts_req++;
        }
    }

    if (volts_req == 2) {
        if (GFIELD(sd->caps, CAP_VOLT_3_0)) {
            volts = 6;
            s = "3.0";
            if (FALSE == selhighest)
                goto voltsel;
            else volts_req++;
        } else {
            sd_err(("HC doesn't support voltage! trying higher voltage: %d\n", volts));
            volts_req++;
        }
    }

    if (volts_req == 3) {
        if (GFIELD(sd->caps, CAP_VOLT_3_3)) {
            volts = 7;
            s = "3.3";
        } else {
            if ((FALSE == selhighest) || (volts == 0)) {
                sd_err(("HC doesn't support any voltage! error!\n"));
                return FALSE;
            }
        }
    }

    /* XXX
     * if UHSI is NOT supported, check for other voltages also. This is a safety measure
     * for embedded devices also, so that HC starts in lower power first. If this
     * function fails, the caller may disable UHSISupported
     * and call start power again to check support in higher voltages.
     */

voltsel:
    pwr = SFIELD(pwr, PWR_VOLTS, volts);
    pwr = SFIELD(pwr, PWR_BUS_EN, 1);
    sdstd_wreg8(sd, SD_PwrCntrl, pwr); /* Set Voltage level */
    sd_info(("Setting Bus Power to %s Volts\n", s));
    BCM_REFERENCE(s);

    /*
     * PR101766 : BRCM SDIO3.0 card is an embedded SD device. It is not a SD card.
     * VDDIO signalling will be tied to 1.8v level on all SDIO3.0 based boards.
     * So program the HC to drive VDDIO at 1.8v level.
     */
    if ((sd->version == HOST_CONTR_VER_3) && (volts == 5)) {
        val1 = sdstd_rreg16(sd, SD3_HostCntrl2);
        val1 = SFIELD(val1, HOSTCtrl2_1_8SIG_EN, 1);
        sdstd_wreg16(sd, SD3_HostCntrl2, val1);
    }

    /* Wait for 500ms for power to stabilize.  Some designs have reset IC's
     * which can hold reset low for close to 300ms.  In addition there can
     * be ramp time for VDD and/or VDDIO which might be provided from a LDO.
     * For these reasons we need a pretty conservative delay here to have
     * predictable reset behavior in the face of an unknown design.
     */
    OSL_DELAY(500000);

    if (sdstd_sdmmc_set_clock(sd, SDIO_CLK_FREQ) == FALSE) {
        return FALSE;
    }

    // OSL_DELAY(10000);

    sd_card_type = detect_card_type(sd);

    if (sd_card_type == SD_CARD) {
        return sdstd_sd_start_power(sd);
    }
    else {
        return sdstd_mmc_start_power(sd);
    }

}

int
sdstd_sdmmc_init(sdioh_info_t *sd)
{
    uint16 powerstat = 0;
    uint16 result;

#ifdef BCMINTERNAL
#ifdef NOTUSED
    /* Handy routine to dump capabilities. */
    static char caps_buf[500];
    parse_caps(sd->caps, caps_buf, 500);
    sd_err((caps_buf));
#endif /* NOTUSED */
#endif /* BCMINTERNAL */

    sd_trace(("%s: Powering up slot %d\n", __FUNCTION__, sd->adapter_slot));

    /* Clear any pending ints */
    sdstd_wreg16(sd, SD_IntrStatus, 0x1fff);
    sdstd_wreg16(sd, SD_ErrorIntrStatus, 0x0fff);

    /* Enable both Normal and Error Status.  This does not enable
     * interrupts, it only enables the status bits to
     * become 'live'
     */

    if (!sd->host_UHSISupported)
        sdstd_wreg16(sd, SD_IntrStatusEnable, 0x1ff);
    else
    {
        /* INT_x interrupts, but DO NOT enable signalling [enable retuning
         * will happen later]
         */
        sdstd_wreg16(sd, SD_IntrStatusEnable, 0x0fff);
    }
    sdstd_wreg16(sd, SD_ErrorIntrStatusEnable, 0xffff /*& ~ERRINT_DATA_CRC_BIT*/);

    sdstd_wreg16(sd, SD_IntrSignalEnable, CLIENT_INTR);      /* Enable card interrupt. */

    if (sd->host_UHSISupported) {
        /* when HC is started for SDIO 3.0 mode, start in lowest voltage mode first. */
        powerstat = sdstd_sdmmc_start_power(sd, 1);
        if (SDIO_OCR_READ_FAIL == powerstat) {
            /* This could be because the device is 3.3V, and possible does
             * not have sdio3.0 support. So, try in highest voltage
             */
            sd_err(("sdstd_sdmmc_start_power: legacy device: trying highest voltage\n"));
            sd_err(("%s failed\n", __FUNCTION__));
            return ERROR;
        } else if (TRUE != powerstat) {
            sd_err(("sdstd_sdmmc_start_power failed\n"));
            return ERROR;
        }
    } else
    /* XXX legacy driver: start in highest voltage mode first.
     * CAUTION: trying to start a legacy dhd with sdio3.0HC and sdio3.0 device could
     * burn the sdio3.0device if the device has started in 1.8V.
     */
    if (TRUE != sdstd_sdmmc_start_power(sd, 0)) {
        sd_err(("sdstd_sdmmc_start_power failed\n"));
        return ERROR;
    }

    if (sd_card_type == SD_CARD) {
        result = sdstd_sd_init(sd);
    }
    else {
        result = sdstd_mmc_init(sd);
    }

    return (result);
}

static int
sdstd_sd_init(sdioh_info_t *sd)
{
    uint32 cmd_arg, cmd_rsp;
    int status;
    uint16 rsp6_status;
    uint32 rsp2_csd[4];

    /*
     * ALL_SEND_CID
     */

    cmd_arg = 0;
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_2, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD2 failed!\n", __FUNCTION__));
        return status;
    }

    /*
     * Ask it to send an RCA
     * SEND_RELATIVE_ADDR
     */

    OSL_DELAY(1000);

    cmd_arg = 0;
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_3, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD3 failed!\n", __FUNCTION__));
        return status;
    }

    sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
    rsp6_status = GFIELD(cmd_rsp, RSP6_STATUS);
    rsp6_status = GFIELD(cmd_rsp, RSP6_STATUS);
    if (GFIELD(rsp6_status, RSP6STAT_COM_CRC_ERROR) ||
        GFIELD(rsp6_status, RSP6STAT_ILLEGAL_CMD) ||
        GFIELD(rsp6_status, RSP6STAT_ERROR)) {
        sd_err(("%s: CMD3 response error. Response = 0x%x!\n",
                __FUNCTION__, rsp6_status));
        return ERROR;
    }

    /* Save the Card's RCA */
    sd->card_rca = GFIELD(cmd_rsp, RSP6_IO_RCA);
    sd_info(("RCA is 0x%x\n", sd->card_rca));

    if (rsp6_status)
        sd_err(("raw status is 0x%x\n", rsp6_status));

    /*
     * SEND_CSD
     */
    cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_9, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD9 failed!\n", __FUNCTION__));
        return status;
    }

    sdstd_cmd_getrsp(sd, rsp2_csd, 4);

    /* Workaround: fix of 8 bit right shifted CSD value */
    rsp2_csd[3] = (rsp2_csd[3] << 8) | ((rsp2_csd[2] >> 24) & 0xff);
    rsp2_csd[2] = (rsp2_csd[2] << 8) | ((rsp2_csd[1] >> 24) & 0xff);
    rsp2_csd[1] = (rsp2_csd[1] << 8) | ((rsp2_csd[0] >> 24) & 0xff);
    rsp2_csd[0] = rsp2_csd[0] << 8;

    /* copy CSD value */
    bcopy(rsp2_csd, sd->CSD, sizeof(rsp2_csd));

    if (sdstd_parse_csd(sd) != SUCCESS)
    {
        return ERROR;
    }

    /* sd_info(("rsp2_csd[0] = %08x, rsp2_csd[1] = %08x, rsp2_csd[2] = %08x, rsp2_csd[3] = %08x\n",
        (int) rsp2_csd[0], (int) rsp2_csd[1], (int) rsp2_csd[2], (int) rsp2_csd[3])); */


    sd_info(("BLOCKNR = %u, BLOCKNR(NORM) = %u, BLOCK_LEN = %u\n",
        sd->csd_blocknr, sd->csd_blocknr_512, sd->csd_block_len));

    /*
     * Select the card
     * SELECT/DESELECT_CARD
     */

    cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_7, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD7 failed!\n", __FUNCTION__));
        return status;
    }
    sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
    if (cmd_rsp != SDIOH_CMD7_EXP_STATUS) {
        sd_err(("%s: CMD7 response error. Response = 0x%x!\n",
                __FUNCTION__, cmd_rsp));
        return ERROR;
    }

    /* block length == 512 */
    if (set_sdmmc_block_size(sd, 1, BLOCK_SIZE_4328) != SUCCESS)
    {
        return ERROR;
    }

    if (!sdstd_sdmmc_bus_width(sd, sd_sdmode)) {
        sd_err(("sdstd_sdmmc_bus_width failed\n"));
        return ERROR;
    }

    /* inplement set high speed CMD6 */
    cmd_arg = (1 << 31) | 0xFFF0;
    cmd_arg |= 1 &0xF;
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_6, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD6 failed!\n", __FUNCTION__));
        return ERROR;
    }

    OSL_DELAY(10000);

    if ((status = sdstd_sdmmc_set_clock(sd, 50000)) == FALSE) {
        sd_err(("%s: sdstd_sdmmc_set_clock failed!\n", __FUNCTION__));
        return ERROR;
    }

    OSL_DELAY(10000);

    sd->card_init_done = TRUE;

    return SUCCESS;
}

#define EXTCSD_RETRIES (1000000)
#define EXTCSD_DELAY (10)
static int
sdstd_mmc_init(sdioh_info_t *sd)
{
    uint32 cmd_arg, cmd_rsp;
    int status;
    uint16 arg16 = 0;
    // uint32 rsp2_csd[BLOCK_SIZE_4328/sizeof(int)];
    uint8 extcsd[BLOCK_SIZE_4328];
    uint16 rsp6_status;
    int retries = 0;
    int i;
    int max = BLOCK_SIZE_4328/sizeof(int);
    uint16 int_reg = 0;
    uint32* p = (uint32*) extcsd;
    // uint32 rsp2_csd[4];

    /*
     * ALL_SEND_CID
     */

    cmd_arg = 0;
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_2, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD2 failed!\n", __FUNCTION__));
        return status;
    }

    /*
     * Ask it to send an RCA
     * SEND_RELATIVE_ADDR
     */

    OSL_DELAY(1000);

    cmd_arg = 0;
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_3, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD3 failed!\n", __FUNCTION__));
        return status;
    }

    sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
    rsp6_status = GFIELD(cmd_rsp, RSP6_STATUS);
    rsp6_status = GFIELD(cmd_rsp, RSP6_STATUS);
    if (GFIELD(rsp6_status, RSP6STAT_COM_CRC_ERROR) ||
        GFIELD(rsp6_status, RSP6STAT_ILLEGAL_CMD) ||
        GFIELD(rsp6_status, RSP6STAT_ERROR)) {
        sd_err(("%s: CMD3 response error. Response = 0x%x!\n",
                __FUNCTION__, rsp6_status));
        return ERROR;
    }

    /* Save the Card's RCA */
    sd->card_rca = GFIELD(cmd_rsp, RSP6_IO_RCA);
    sd_info(("RCA is 0x%x\n", sd->card_rca));

    if (rsp6_status)
        sd_err(("raw status is 0x%x\n", rsp6_status));

    /*
     * Select the card
     * SELECT/DESELECT_CARD
     */

    cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_7, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD7 failed!\n", __FUNCTION__));
        return status;
    }
    sdstd_cmd_getrsp(sd, &cmd_rsp, 1);
    if (cmd_rsp != SDIOH_CMD7_EXP_STATUS) {
        sd_err(("%s: CMD7 response error. Response = 0x%x!\n",
                __FUNCTION__, cmd_rsp));
        return ERROR;
    }

    /*
     * SEND_CSD
     */
    arg16 = SFIELD(arg16, XFER_MULTI_BLOCK, 0);
    arg16 = SFIELD(arg16, XFER_BLK_COUNT_EN, 0);
    arg16 = SFIELD(arg16, XFER_CMD_12_EN, 0);
    arg16 = SFIELD(arg16, XFER_DATA_DIRECTION, 1);

    sdstd_wreg16(sd, SD_TransferMode, arg16);
    sdstd_wreg16(sd, SD_BlockSize, BLOCK_SIZE_4328);
    sdstd_wreg16(sd, SD_BlockCount, 1);

    cmd_arg = SFIELD(0, CMD7_RCA, sd->card_rca);
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), MMC_CMD_8, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: MMC CMD8 failed!\n", __FUNCTION__));
        return status;
    }

    retries = EXTCSD_RETRIES;

    do
    {
        OSL_DELAY(EXTCSD_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_BUF_READ_READY) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: MMC CMD8 failed. INTSTAT_BUF_READ_READY not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return ERROR;
    }

    int_reg = SFIELD(0, INTSTAT_BUF_READ_READY, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    retries = EXTCSD_RETRIES;

    while (!GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_READ_DATA_RDY) && --retries)
    {
        OSL_DELAY(EXTCSD_DELAY);
    }

    sd_trace(("%s: retries == %d\n", __FUNCTION__, retries));

    if (retries == 0)
    {
        sd_err(("%s: MMC CMD8 failed. PRES_READ_DATA_RDY not ready\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return ERROR;
    }

    for ( i = 0; i < max; i++ )
    {
        uint32 val = sdstd_rreg(sd, SD_BufferDataPort0);
        // *(p++) = sdstd_rreg(sd, SD_BufferDataPort0);
        sd_info(("%03d: %08x\n", i * 4, val));
        *(p++) = val;
    }

    retries = EXTCSD_RETRIES;

    do
    {
        OSL_DELAY(EXTCSD_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_XFER_COMPLETE) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: MMC CMD8 failed. INTSTAT_XFER_COMPLETE not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return ERROR;
    }

    int_reg = SFIELD(0, INTSTAT_XFER_COMPLETE, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    sd_info(("sector_num = %08x\n",
        *((int *) &extcsd[212])));

    /* block length == 512 */
    if (set_sdmmc_block_size(sd, 1, BLOCK_SIZE_4328) != SUCCESS)
    {
        return ERROR;
    }

    if (!sdstd_mmc_bus_width(sd, sd_sdmode)) {
        sd_err(("sdstd_mmc_bus_width failed\n"));
        return ERROR;
    }

    OSL_DELAY(5000);

    /* inplement set high speed CMD6 */
    cmd_arg = (0x3 << 24) | (0xB9 << 16) | (0x0 << 8) | 0x0 << 0;
    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_6, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD6 failed!\n", __FUNCTION__));
        return ERROR;
    }

    OSL_DELAY(10000);

    if ((status = sdstd_sdmmc_set_clock(sd, 50000)) == FALSE) {
        sd_err(("%s: sdstd_sdmmc_set_clock failed!\n", __FUNCTION__));
        return ERROR;
    }

    OSL_DELAY(10000);

    sd->card_init_done = TRUE;

    return SUCCESS;
}

static int
set_sdmmc_block_size(sdioh_info_t *sd, int func, int block_size)
{
    UNUSED_VARIABLE(func);
    int status;

    sd_info(("%s: Setting block size %d, func %d\n", __FUNCTION__, block_size, func));

    /*
     * SET_BLOCKLEN
     */

    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_16, block_size))
        != SUCCESS) {
        sd_err(("%s: CMD16 failed!\n", __FUNCTION__));
        return status;
    }

    return SUCCESS;
}

static bool
sdstd_sdmmc_bus_width(sdioh_info_t *sd, int new_mode)
{
    int status;
    uint8 reg8;

    sd_trace(("%s\n", __FUNCTION__));
    if (sd->sd_mode == new_mode) {
        sd_info(("%s: Already at width %d\n", __FUNCTION__, new_mode));
        /* Could exit, but continue just in case... */
    }

    if ((status = sdstd_acmd_issue(sd, USE_DMA(sd), SDIOH_ACMD_6, new_mode))
        != SUCCESS) {
        sd_err(("%s: ACMD6 failed\n", __FUNCTION__));
        return status;
    }

    /* Set host side via Host reg */
    reg8 = sdstd_rreg8(sd, SD_HostCntrl) & ~(SD1_MODE | SD4_MODE);
    reg8 |= new_mode;
    sdstd_wreg8(sd, SD_HostCntrl, reg8);

    sd->sd_mode = new_mode;

    return TRUE;
}

static bool
sdstd_mmc_bus_width(sdioh_info_t *sd, int new_mode)
{
    int status;
    uint8 reg8;
    uint32 cmd_arg = (0x3 << 24) | (0xB7 << 16) | (0x0 << 8) | 0x0 << 0;;

    sd_trace(("%s\n", __FUNCTION__));
    if (sd->sd_mode == new_mode) {
        sd_info(("%s: Already at width %d\n", __FUNCTION__, new_mode));
        /* Could exit, but continue just in case... */
    }

    if (new_mode == SDIOH_MODE_SD4) {
        cmd_arg = (0x3 << 24) | (0xB7 << 16) | (0x1 << 8) | 0x0 << 0;
    }

    if ((status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_6, cmd_arg))
        != SUCCESS) {
        sd_err(("%s: CMD6 failed\n", __FUNCTION__));
        return status;
    }

    OSL_DELAY(10000);

    /* Set host side via Host reg */
    reg8 = sdstd_rreg8(sd, SD_HostCntrl) & ~(SD1_MODE | SD4_MODE);
    reg8 |= new_mode;
    sdstd_wreg8(sd, SD_HostCntrl, reg8);

    sd->sd_mode = new_mode;

    return TRUE;
}

#define READ_RETRIES (1000000)
#define READ_DELAY (10)
extern SDIOH_API_RC
sdioh_sdmmc_read_block(sdioh_info_t *sd, uint32_t address, uint8_t* data)
{
    SDIOH_API_RC result = SDIOH_API_RC_FAIL;
    int status;
    int retries = 0;
    int i;
    int max = BLOCK_SIZE_4328/sizeof(int);
    uint16 arg16 = 0;
    uint16 int_reg = 0;
    uint32* p = (uint32*) data;

    prev = host_rtos_get_time();

    arg16 = SFIELD(arg16, XFER_MULTI_BLOCK, 0);
    arg16 = SFIELD(arg16, XFER_BLK_COUNT_EN, 0);
    arg16 = SFIELD(arg16, XFER_CMD_12_EN, 0);
    arg16 = SFIELD(arg16, XFER_DATA_DIRECTION, 1);

    sdstd_wreg16(sd, SD_TransferMode, arg16);
    sdstd_wreg16(sd, SD_BlockSize, BLOCK_SIZE_4328);
    sdstd_wreg16(sd, SD_BlockCount, 1);

    status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_17, address);

    if (status != SUCCESS)
    {
        sd_err(("%s: CMD17 failed\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    retries = READ_RETRIES;

    do
    {
        OSL_DELAY(READ_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_BUF_READ_READY) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD17 failed. INTSTAT_BUF_READ_READY not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_BUF_READ_READY, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    retries = READ_RETRIES;

    while (!GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_READ_DATA_RDY) && --retries)
    {
        OSL_DELAY(READ_DELAY);
    }

    sd_trace(("%s: retries == %d\n", __FUNCTION__, retries));

    if (retries == 0)
    {
        sd_err(("%s: CMD17 failed. PRES_READ_DATA_RDY not ready\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    for ( i = 0; i < max; i++ )
    {
        *(p++) = sdstd_rreg(sd, SD_BufferDataPort0);
    }

    retries = READ_RETRIES;

    do
    {
        OSL_DELAY(READ_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_XFER_COMPLETE) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD17 failed. INTSTAT_XFER_COMPLETE not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_XFER_COMPLETE, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    result = SDIOH_API_RC_SUCCESS;

    sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    now = host_rtos_get_time();

    sd_trace(("%s: processed time = %u ms\n", __FUNCTION__, (unsigned int) (now - prev)));

    return result;
}

extern SDIOH_API_RC
sdioh_sdmmc_read_blocks(sdioh_info_t *sd, uint32_t address, uint32_t count, uint8_t* data)
{
    SDIOH_API_RC result = SDIOH_API_RC_FAIL;
    int status;
    int retries = 0;
    int i, j;
    int max = (BLOCK_SIZE_4328/sizeof(int));
    uint16 arg16 = 0;
    uint16 int_reg = 0;
    uint32* p = (uint32*) data;

    prev = host_rtos_get_time();

    arg16 = SFIELD(arg16, XFER_MULTI_BLOCK, 1);
    arg16 = SFIELD(arg16, XFER_BLK_COUNT_EN, 1);
    arg16 = SFIELD(arg16, XFER_CMD_12_EN, 0);
    arg16 = SFIELD(arg16, XFER_DATA_DIRECTION, 1);

    sdstd_wreg16(sd, SD_TransferMode, arg16);
    sdstd_wreg16(sd, SD_BlockSize, BLOCK_SIZE_4328);
    sdstd_wreg16(sd, SD_BlockCount, count);

    status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_18, address);

    if (status != SUCCESS)
    {
        sd_err(("%s: CMD18 failed\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    retries = READ_RETRIES;

    do
    {
        OSL_DELAY(READ_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_BUF_READ_READY) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD18 failed. INTSTAT_BUF_READ_READY not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_BUF_READ_READY, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    for ( j = 0; j < count; j++ )
    {
        retries = READ_RETRIES;

        while (!GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_READ_DATA_RDY) && --retries)
        {
            OSL_DELAY(READ_DELAY);
        }

        sd_trace(("%s: retries = %d\n", __FUNCTION__, retries));

        if (retries == 0)
        {
            sd_err(("%s: CMD18 failed. PRES_READ_DATA_RDY not ready\n", __FUNCTION__));
            // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
            return SDIOH_API_RC_FAIL;
        }

        for ( i = 0; i < max; i++ )
        {
            *(p++) = sdstd_rreg(sd, SD_BufferDataPort0);
        }

    }

    retries = READ_RETRIES;

    do
    {
        OSL_DELAY(READ_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_XFER_COMPLETE) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD18 failed. INTSTAT_XFER_COMPLETE not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_XFER_COMPLETE, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    result = SDIOH_API_RC_SUCCESS;

    sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    if (SUCCESS != sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_12, 0))
    {
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    retries = READ_RETRIES;

    while (GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_CMD_INHIBIT) && --retries)
    {
        OSL_DELAY(READ_DELAY);
    }

    if (retries == 0)
    {
        sd_err(("%s: CMD18 failed. PRES_CMD_INHIBIT is on\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    now = host_rtos_get_time();

    sd_trace(("%s: processed time = %u ms\n", __FUNCTION__, (unsigned int) (now - prev)));

    return result;
}

#define WRITE_RETRIES (1000000)
#define WRITE_DELAY   (30)
extern SDIOH_API_RC
sdioh_sdmmc_write_block(sdioh_info_t *sd, uint32_t address, const uint8_t* data)
{
    SDIOH_API_RC result = SDIOH_API_RC_FAIL;
    int status;
    int retries = 0;
    int i;
    int max = BLOCK_SIZE_4328/sizeof(int);
    uint16 arg16 = 0;
    uint16 int_reg = 0;
    uint32* p = (uint32*) data;

    prev = host_rtos_get_time();

    arg16 = SFIELD(arg16, XFER_MULTI_BLOCK, 0);
    arg16 = SFIELD(arg16, XFER_BLK_COUNT_EN, 0);
    arg16 = SFIELD(arg16, XFER_CMD_12_EN, 0);
    arg16 = SFIELD(arg16, XFER_DATA_DIRECTION, 0);

    sdstd_wreg16(sd, SD_TransferMode, arg16);
    sdstd_wreg16(sd, SD_BlockSize, BLOCK_SIZE_4328);
    sdstd_wreg16(sd, SD_BlockCount, 1);

    status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_24, address);

    if (status != SUCCESS)
    {
        sd_err(("%s: CMD24 failed\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    retries = WRITE_RETRIES;

    do
    {
        OSL_DELAY(WRITE_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_BUF_WRITE_READY) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD24 failed. INTSTAT_BUF_WRITE_READY not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_BUF_WRITE_READY, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    retries = WRITE_RETRIES;

    while (!GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_WRITE_DATA_RDY) && --retries)
    {
        OSL_DELAY(WRITE_DELAY);
    }

    sd_trace(("%s: retries == %d\n", __FUNCTION__, retries));

    if (retries == 0)
    {
        sd_err(("%s: CMD24 failed. PRES_WRITE_DATA_RDY not ready\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    for ( i = 0; i < max; i++ )
    {
        sdstd_wreg(sd, SD_BufferDataPort0, *(p++) );
    }

    retries = WRITE_RETRIES;

    do
    {
        OSL_DELAY(WRITE_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_XFER_COMPLETE) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD24 failed. INTSTAT_XFER_COMPLETE not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_XFER_COMPLETE, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    result = SDIOH_API_RC_SUCCESS;

    sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    now = host_rtos_get_time();

    sd_trace(("%s: processed time = %u ms\n", __FUNCTION__, (unsigned int) (now - prev)));

    return result;
}

extern SDIOH_API_RC
sdioh_sdmmc_write_blocks(sdioh_info_t *sd, uint32_t address, uint32_t count, const uint8_t* data)
{
    SDIOH_API_RC result = SDIOH_API_RC_FAIL;
    int status;
    int retries = 0;
    int i, j;
    int max = (BLOCK_SIZE_4328/sizeof(int));
    uint16 arg16 = 0;
    uint16 int_reg = 0;
    uint32* p = (uint32*) data;

    prev = host_rtos_get_time();

    arg16 = SFIELD(arg16, XFER_MULTI_BLOCK, 1);
    arg16 = SFIELD(arg16, XFER_BLK_COUNT_EN, 1);
    arg16 = SFIELD(arg16, XFER_CMD_12_EN, 0);
    arg16 = SFIELD(arg16, XFER_DATA_DIRECTION, 0);

    sdstd_wreg16(sd, SD_TransferMode, arg16);
    sdstd_wreg16(sd, SD_BlockSize, BLOCK_SIZE_4328);
    sdstd_wreg16(sd, SD_BlockCount, count);

    status = sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_25, address);

    if (status != SUCCESS)
    {
        sd_err(("%s: CMD25 failed\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    retries = WRITE_RETRIES;

    do
    {
        OSL_DELAY(WRITE_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_BUF_WRITE_READY) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD25 failed. INTSTAT_BUF_WRITE_READY not ready\n", __FUNCTION__));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_BUF_WRITE_READY, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    for ( j = 0; j < count; j++ )
    {
        retries = WRITE_RETRIES;

        while (!GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_WRITE_DATA_RDY) && --retries)
        {
            OSL_DELAY(WRITE_DELAY);
        }

        sd_trace(("%s: %d of count = %d, retries = %d\n", __FUNCTION__, j, (int) count, retries));

        if (retries == 0)
        {
            sd_err(("%s: CMD25 failed. PRES_WRITE_DATA_RDY not ready\n", __FUNCTION__));
            // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
            return SDIOH_API_RC_FAIL;
        }

        for ( i = 0; i < max; i++ )
        {
            sdstd_wreg(sd, SD_BufferDataPort0, *(p++) );
        }

        while (0xf != (0xf & GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_DAT_SIGNAL)) && --retries)
        {
            OSL_DELAY(WRITE_DELAY);
        }

        if (retries == 0)
        {
            sd_err(("%s: CMD25 failed. PRES_DAT_SIGNAL not clear\n", __FUNCTION__));
            // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
            return SDIOH_API_RC_FAIL;
        }

    }

    retries = WRITE_RETRIES;

    do
    {
        OSL_DELAY(WRITE_DELAY);
        int_reg = sdstd_rreg16(sd, SD_IntrStatus);
    } while (!GFIELD(int_reg, INTSTAT_XFER_COMPLETE) && --retries);

    sd_trace(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));

    if (retries == 0)
    {
        sd_err(("%s: int_reg = 0x%x, retries = %d\n", __FUNCTION__, int_reg, retries));
        sd_err(("%s: CMD25 failed. INTSTAT_XFER_COMPLETE not ready\n", __FUNCTION__));
        sd_err(("%s: DAT = %d\n", __FUNCTION__, GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_DAT_SIGNAL)));
        sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    int_reg = SFIELD(0, INTSTAT_XFER_COMPLETE, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);

    result = SDIOH_API_RC_SUCCESS;

    sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    if (SUCCESS != sdstd_cmd_issue(sd, USE_DMA(sd), SDIOH_CMD_12, 0))
    {
        sd_err(("%s: CMD25 failed. Transmission Stop (CMD12) failed.\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    retries = WRITE_RETRIES;

    while (GFIELD(sdstd_rreg(sd, SD_PresentState), PRES_CMD_INHIBIT) && --retries)
    {
        OSL_DELAY(WRITE_DELAY);
    }

    if (retries == 0)
    {
        sd_err(("%s: CMD25 failed. PRES_CMD_INHIBIT is on\n", __FUNCTION__));
        // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);
        return SDIOH_API_RC_FAIL;
    }

    // sdioh_sdmmc_transfer_error_check(sd, __FUNCTION__);

    now = host_rtos_get_time();

    sd_trace(("%s: processed time = %u ms\n", __FUNCTION__, (unsigned int) (now - prev)));

    return result;
}

extern SDIOH_API_RC
sdioh_sdmmc_transfer_error_check(sdioh_info_t *sd, const char* func_name)
{
    uint16 int_reg, interr_reg;

    int_reg = sdstd_rreg16(sd, SD_IntrStatus);

    if (!GFIELD(int_reg, INTSTAT_ERROR_INT))
    {
        return SDIOH_API_RC_SUCCESS;
    }

    interr_reg = sdstd_rreg16(sd, SD_ErrorIntrStatus);
    sd_err(("%s: SD_IntrStatus = 0x%x\n", func_name, int_reg));
    sd_err(("%s: SD_ErrorIntrStatus = 0x%x\n", func_name, interr_reg));
    int_reg = SFIELD(0, INTSTAT_ERROR_INT, 1);
    sdstd_wreg16(sd, SD_IntrStatus, int_reg);
    sdstd_wreg16(sd, SD_ErrorIntrStatus, interr_reg);

    return SDIOH_API_RC_FAIL;
}
