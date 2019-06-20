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
 *
 */

#include "platform_i2s.h"
#include "wwd_assert.h"
#include "platform_audio.h"
#include "platform_config.h"
#include "string.h"
#include "wwd_rtos_isr.h"
#include <typedefs.h>
#include <bcmutils.h>
#include <siutils.h>
#include <sbhnddma.h>
#include <sbchipc.h>
#include <hnddma.h>
#include <hndpmu.h>
#include <hndsoc.h>

#include "platform_appscr4.h"

#include <i2s_core.h>

/******************************************************
 *                      Macros
 ******************************************************/

#define XTAL_FREQUENCY              37400000
#define TWO_POW24                   16777216
/* FIXME This belongs in i2s_core.h. */
#define I2S_INTRECE_LAZY_FC_MASK    0xFF000000
#define I2S_INTRECE_LAZY_FC_SHIFT   24

#define SPDIF_INT_MASK              (0)
#define SPDIF_INT_ERROR_MASK        (0)

#define I2S_INT_ERROR_MASK          (I2S_INT_DESCERR | I2S_INT_DATAERR | I2S_INT_DESC_PROTO_ERR | SPDIF_INT_ERROR_MASK)

/* FIXME What device ID should be passed to si_attach? */
#define I2S_DEV_ID                  0

#define REG32(ADDR)                 *((volatile uint32_t*)(ADDR))

#define CHECK_RETURN(expr, err_ret) do {if (!(expr)) {wiced_assert("failed\n", 0); return err_ret;}} while(0)

/******************************************************
 *                    Constants
 ******************************************************/

/* Double buffer (need only two). */
/* HNDDMA requires one slot for counting, so round-up to nearest power-of-2. */
#define I2S_NR_DMA_TXDS_MAX         (4)
#define I2S_NR_DMA_RXDS_MAX         (4)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct i2s_stream           i2s_stream_t;
typedef struct i2s_control          i2s_control_t;
typedef struct i2s_ops              i2s_ops_t;

typedef int (i2s_port_op_void_t)(platform_i2s_port_t);
typedef wiced_result_t (i2s_port_op_bool_t)(platform_i2s_port_t, wiced_bool_t);
typedef wiced_result_t (i2s_port_op_dma_attach_t)(platform_i2s_port_t, uint16_t);

/******************************************************
 *                    Structures
 ******************************************************/

/* TX/RX stream. */
struct i2s_stream
{
    uint8_t                         is_xrun     : 1;

    uint16_t                        period_size;
    wiced_audio_buffer_header_t     *position;

    uint16_t                        periods_count;
    wiced_audio_session_ref         sh;

    /* Metrics. */
    uint32_t                        periods_transferred;
    uint32_t                        sw_xrun, hw_xrun;
};

struct i2s_control
{
    /* Configuration from initialize. */
    uint8_t                         sample_bits;
    uint8_t                         channels;
    uint32_t                        sample_rate;
    wiced_i2s_spdif_mode_t          i2s_spdif_mode;

    /* Standard wrappers. */
    osl_t                           *osh;
    hnddma_t                        *di[2];
    i2sregs_t                       *regs;
    si_t                            *sih;

    /* Global interrupt mask. */
    uint32_t                        intmask;

    /* Indexed by WICED_I2S_READ/WRITE. */
    i2s_stream_t                    streams[2];

    /* Bit-fields where bit positions are PLATFORM_I2S_READ/WRITE. */
    uint8_t                         in_use          : 2;

    /* Other states. */
    uint8_t                         pll_configured  : 1;
};

struct i2s_ops
{
    /* Attach DMA. */
    i2s_port_op_dma_attach_t        *dma_attach;

    /* Initialize DMA. */
    i2s_port_op_bool_t              *dma_init;

    /* DevCtrl value for enabling the peripheral transfer. */
    uint32_t                        peripheral_transfer_enable_val;

    /* Enable interrupts. */
    i2s_port_op_bool_t              *interrupts_on;

    /* Post DMA descriptor of fixed size at current position. */
    i2s_port_op_void_t              *post_dma_descriptor;

    /* Reclaim a single DMA descriptor. */
    i2s_port_op_void_t              *reclaim_dma_descriptor;

    /* Interrupt types. */
    uint32_t                        xrun_interrupt;
    uint32_t                        xfer_interrupt;
};

/******************************************************
 *               Function Declarations
 ******************************************************/

static void           i2s_cleanup( platform_i2s_port_t port );
static wiced_result_t i2s_set_clkdiv( platform_i2s_port_t port, const wiced_i2s_params_t *config, platform_audio_pll_channels_t audio_pll_ch, uint32_t *mclk );
static wiced_result_t i2s_configure_peripheral( wiced_i2s_t i2s, const wiced_i2s_params_t *config, uint32_t *mclk);
static wiced_result_t i2s_peripheral_transfer_enable( platform_i2s_port_t port, platform_i2s_direction_t dir, uint32_t bits, wiced_bool_t on);
static wiced_result_t i2s_dma_attach( platform_i2s_port_t port, platform_i2s_direction_t dir, uint16_t period_size );
static wiced_result_t i2s_stop_stream( platform_i2s_port_t port, platform_i2s_direction_t dir );

static wiced_result_t i2s_configure_pll( osl_t *osh, si_t *sih, const platform_audio_pll_channels_t ch, uint32_t mclk );
static wiced_result_t i2s_disable_pll( si_t *sih );

/* Port ops. */
static i2s_port_op_dma_attach_t     i2s_op_rx_dma_attach;
static i2s_port_op_dma_attach_t     i2s_op_tx_dma_attach;
static i2s_port_op_bool_t           i2s_op_rx_dma_init;
static i2s_port_op_bool_t           i2s_op_tx_dma_init;
static i2s_port_op_bool_t           i2s_op_rx_interrupts_on;
static i2s_port_op_bool_t           i2s_op_tx_interrupts_on;
static i2s_port_op_void_t           i2s_op_rxfill_unframed;
static i2s_port_op_void_t           i2s_op_txunframed;
static i2s_port_op_void_t           i2s_op_getnextrxp;
static i2s_port_op_void_t           i2s_op_getnexttxp;

/* Inline helpers. */

static inline i2s_control_t *I2S_CONTROL( platform_i2s_port_t port ) {
    static i2s_control_t i2s_control[BCM4390X_I2S_MAX];
    return &i2s_control[port];
}

static inline const platform_i2s_t *I2S_INTERFACE(wiced_i2s_t i2s) {
    extern const platform_i2s_t i2s_interfaces[WICED_I2S_MAX];
    return &i2s_interfaces[i2s];
}

/*
 * IMPORTANT: DO NOT use the I2S_PORT_INFO macro to get to the is_master or audio_pll_ch fields !
 * Please access these fields DIRECTLY from the platform_i2s_port_info_t* table pointer
 * in the platform_i2s_t struct.
 */
static inline const platform_i2s_port_info_t *I2S_PORT_INFO( platform_i2s_port_t port ) {
    extern const platform_i2s_port_info_t i2s_port_info[BCM4390X_I2S_MAX];
    return &i2s_port_info[port];
}

static inline i2s_stream_t *I2S_RX_STREAM( platform_i2s_port_t port ) {
    return &I2S_CONTROL(port)->streams[PLATFORM_I2S_READ];
}

static inline i2s_stream_t *I2S_TX_STREAM( platform_i2s_port_t port ) {
    return &I2S_CONTROL(port)->streams[PLATFORM_I2S_WRITE];
}

static inline i2s_stream_t *I2S_STREAM( wiced_i2s_t i2s ) {
    return &I2S_CONTROL(I2S_INTERFACE(i2s)->port_info->port)->streams[I2S_INTERFACE(i2s)->stream_direction];
}

/* Increment position by one period. */
static inline void I2S_INC_POSITION( i2s_stream_t *stream ) {
    stream->position = stream->position->next;
}

static inline i2sregs_t *I2S_REGS( platform_i2s_port_t port ) {
    return I2S_CONTROL(port)->regs;
}

/* FIXME Revisit when full-duplex is fully implemented! */
static inline void I2S_LOCK_PORT( platform_i2s_port_t port ) {
    (void)0; /* TODO */
}

/* FIXME Revisit when full-duplex is fully implemented! */
static inline void I2S_UNLOCK_PORT( platform_i2s_port_t port ) {
    (void)0; /* TODO */
}

/* Disable all I2S interrupts. */
static inline void i2s_intrsoff( platform_i2s_port_t port ) {
    W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intmask, 0);
}

/* Enable I2S interrupts. */
static inline void i2s_intrson( platform_i2s_port_t port ) {
    W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intmask, I2S_CONTROL(port)->intmask);
}

static inline const i2s_ops_t *I2S_OPS( platform_i2s_direction_t dir ) {
    static const i2s_ops_t i2s_ops[2] =
    {
        [PLATFORM_I2S_READ] =
        {
            .dma_attach                 = i2s_op_rx_dma_attach,
            .dma_init                   = i2s_op_rx_dma_init,
            .peripheral_transfer_enable_val = I2S_CTRL_RECEN,
            .interrupts_on              = i2s_op_rx_interrupts_on,
            .post_dma_descriptor        = i2s_op_rxfill_unframed,
            .reclaim_dma_descriptor     = i2s_op_getnextrxp,
            .xrun_interrupt             = I2S_INT_RCVFIFO_OFLOW,
            .xfer_interrupt             = I2S_INT_RCV_INT,
        },

        [PLATFORM_I2S_WRITE] =
        {
            .dma_attach                 = i2s_op_tx_dma_attach,
            .dma_init                   = i2s_op_tx_dma_init,
            .peripheral_transfer_enable_val = I2S_CTRL_PLAYEN,
            .interrupts_on              = i2s_op_tx_interrupts_on,
            .post_dma_descriptor        = i2s_op_txunframed,
            .reclaim_dma_descriptor     = i2s_op_getnexttxp,
            .xrun_interrupt             = I2S_INT_XMTFIFO_UFLOW,
            .xfer_interrupt             = I2S_INT_XMT_INT,
        },
    };

    return &i2s_ops[dir];
}

/******************************************************
 *               Variables Definitions
 ******************************************************/

static uint             msg_level = 1;

/******************************************************
 *               Function Definitions
 ******************************************************/

#if defined(WICED_AUDIO_BUFFER_RESERVED_BYTES)
#if (I2S_DMA_OFS_BYTES > WICED_AUDIO_BUFFER_RESERVED_BYTES)
#error "Not enough headroom allocated for DMA buffer!"
#endif
#endif

wiced_result_t wiced_i2s_init( wiced_audio_session_ref sh, wiced_i2s_t i2s, wiced_i2s_params_t *config, uint32_t *mclk)
{
    wiced_result_t              result      = WICED_SUCCESS;
    platform_i2s_direction_t    dir         = I2S_INTERFACE(i2s)->stream_direction;
    platform_i2s_port_t         port        = I2S_INTERFACE(i2s)->port_info->port;
    wiced_bool_t                is_locked   = WICED_FALSE;
    i2s_control_t               *control    = I2S_CONTROL(port);

    if ( !PLATFORM_FEATURE_ENAB( I2S ) )
    {
        return WICED_UNSUPPORTED;
    }

    /* Check arguments. */
    if (((dir == PLATFORM_I2S_WRITE) && (config->period_size > I2S_TX_PERIOD_BYTES_MAX)) ||
        ((dir == PLATFORM_I2S_READ)  && (config->period_size > I2S_RX_PERIOD_BYTES_MAX)) )
    {
        result = WICED_BADARG;
    }

    if (result == WICED_SUCCESS)
    {
        /* LOCK */
        I2S_LOCK_PORT(port);
        is_locked = WICED_TRUE;

        if (control->in_use & (1 << dir))
        {
            /* Already initialized. */
            result = WICED_ERROR;
            goto DONE_STREAM_IN_USE;
        }

        if (control->in_use & ~(1 << dir))
        {
            /* The other stream is in-use. */
            /* Verify that the configuration is compatible with the other stream. */
            result = (control->channels       == config->channels         &&
                      control->sample_rate    == config->sample_rate      &&
                      control->sample_bits    == config->bits_per_sample  &&
                      control->i2s_spdif_mode == config->i2s_spdif_mode) ? WICED_SUCCESS : WICED_BADARG;

            /* Don't do a full configuration since the settings are shared. */
            goto DONE_OTHER_STREAM_IN_USE;
        }
    }

    /* FIXME osh handle needs an attach. */

    if (result == WICED_SUCCESS)
    {
        /* Get si handle. */
        control->sih = si_attach(I2S_DEV_ID, control->osh, NULL /*regsva*/, PCI_BUS, NULL, NULL, NULL);
        if (control->sih == NULL)
        {
            result = WICED_ERROR;
        }
    }

    if (result == WICED_SUCCESS)
    {
        /* Get I2S registers. */
        control->regs = (i2sregs_t *)si_setcore(control->sih, I2S_CORE_ID, port);
        if (control->regs == NULL)
        {
            result = WICED_ERROR;
        }
    }

    if (result == WICED_SUCCESS)
    {
        /* Take I2S core out of reset. */
        si_core_reset(control->sih, 0, 0);
    }

    if (result == WICED_SUCCESS)
    {
        platform_irq_enable_irq( I2S_PORT_INFO(port)->irqn );

        result = i2s_configure_peripheral(i2s, config, mclk);
    }

    if (result == WICED_SUCCESS)
    {
        control->sample_bits    = config->bits_per_sample;
        control->channels       = config->channels;
        control->sample_rate    = config->sample_rate;
        control->i2s_spdif_mode = config->i2s_spdif_mode;
    }

    if (result != WICED_SUCCESS)
    {
        i2s_cleanup(port);
    }

DONE_OTHER_STREAM_IN_USE:
    if (result == WICED_SUCCESS)
    {
        i2s_stream_t *stream = I2S_STREAM(i2s);

        stream->period_size = config->period_size;
        stream->sh          = sh;

        /* Direction specific DMA attach.
         * Note that if a single attach is used, then the RX payload
         * size must be known (e.g. the max size needs to be specified).
         * The underlying hnddma code will invalidate based on the
         * max RX size.  Instead, use two separate attaches to save payload
         * space.
         */
        result = i2s_dma_attach(port, dir, config->period_size);
    }
    if (result == WICED_SUCCESS)
    {
        /* Mark this stream as in-use. */
        control->in_use |= (1 << dir);
    }

DONE_STREAM_IN_USE:
    if (is_locked == WICED_TRUE)
        I2S_UNLOCK_PORT(port);

    return result;
}

wiced_result_t wiced_i2s_deinit( wiced_i2s_t i2s )
{
    platform_i2s_direction_t    dir         = I2S_INTERFACE(i2s)->stream_direction;
    platform_i2s_port_t         port        = I2S_INTERFACE(i2s)->port_info->port;
    i2s_control_t               *control    = I2S_CONTROL(port);
    i2s_stream_t                *stream     = I2S_STREAM(i2s);

    I2S_LOCK_PORT(port);

    if (!(control->in_use & (1 << dir)))
    {
        /*
         * Not in use so there's nothing to do.
         */

        I2S_UNLOCK_PORT(port);

        return WICED_SUCCESS;
    }

    /* Clean-up stream. */
    if (control->in_use & (1 << dir))
    {
        wiced_assert("bad handle", control->di[dir] != NULL);
        if (control->di[dir] != NULL)
        {
            dma_detach(control->di[dir]);
            control->di[dir] = NULL;
        }

        memset(stream, 0, sizeof *stream);

        /* This stream is no longer in-use. */
        control->in_use &= ~(1 << dir);
    }

    /* Check whether to perform shared clean-up. */
    if (!(control->in_use & ~(1 << dir)))
    {
        /* No other in-use streams; clean-up. */
        i2s_cleanup(port);
    }

    I2S_UNLOCK_PORT(port);

    return WICED_SUCCESS;
}

wiced_result_t wiced_i2s_start( wiced_i2s_t i2s )
{
    wiced_result_t              result  = WICED_SUCCESS;
    platform_i2s_direction_t    dir     = I2S_INTERFACE(i2s)->stream_direction;
    platform_i2s_port_t         port    = I2S_INTERFACE(i2s)->port_info->port;
    i2s_stream_t                *stream = I2S_STREAM(i2s);


    I2S_LOCK_PORT(port);

    /* Double buffering requires 2 periods worth of data. */
    if (wiced_audio_buffer_platform_get_periods(stream->sh) < 2)
    {
        result = WICED_ERROR;
        goto ERROR_INSUFFICIENT_DATA;
    }

    wiced_assert("Stop not called on xrun?", stream->is_xrun == 0);
    stream->is_xrun = 0;

    (*I2S_OPS(dir)->dma_init)(port, WICED_TRUE);

    (*I2S_OPS(dir)->post_dma_descriptor)(port);
    I2S_INC_POSITION(stream);
    (*I2S_OPS(dir)->post_dma_descriptor)(port);

    (*I2S_OPS(dir)->interrupts_on)(port, WICED_TRUE);

    i2s_peripheral_transfer_enable(port, dir, I2S_OPS(dir)->peripheral_transfer_enable_val, WICED_TRUE);

ERROR_INSUFFICIENT_DATA:
    I2S_UNLOCK_PORT(port);

    return result;
}

wiced_result_t wiced_i2s_stop( wiced_i2s_t i2s )
{
    wiced_result_t              result;
    platform_i2s_direction_t    dir     = I2S_INTERFACE(i2s)->stream_direction;
    platform_i2s_port_t         port    = I2S_INTERFACE(i2s)->port_info->port;

    I2S_LOCK_PORT(port);

    result = i2s_stop_stream(port, dir);
    if (result == WICED_SUCCESS)
    {
        I2S_STREAM(i2s)->is_xrun = 0;
    }
    I2S_UNLOCK_PORT(port);

    return result;
}

wiced_result_t wiced_i2s_set_audio_buffer_details(wiced_i2s_t i2s, wiced_audio_buffer_header_t *audio_buffer_ptr)
{
    if( audio_buffer_ptr != NULL )
    {
        platform_i2s_port_t         port    = I2S_INTERFACE(i2s)->port_info->port;
        i2s_stream_t                *stream = I2S_STREAM(i2s);

        I2S_LOCK_PORT(port);
        stream->position = audio_buffer_ptr;
        I2S_UNLOCK_PORT(port);
        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t wiced_i2s_get_current_hw_pointer(wiced_i2s_t i2s, uint32_t *hw_pointer)
{
    UNUSED_PARAMETER(i2s);
    *hw_pointer = 0;
    return WICED_UNSUPPORTED;
}

wiced_result_t wiced_i2s_get_current_hw_buffer_weight( wiced_i2s_t i2s, uint32_t* weight )
{
    wiced_result_t                  result      = WICED_SUCCESS;
    platform_i2s_direction_t        dir         = I2S_INTERFACE(i2s)->stream_direction;
    const platform_i2s_port_info_t* port_info   = I2S_INTERFACE(i2s)->port_info;
    platform_i2s_port_t             port        = port_info->port;
    i2s_control_t*                  control     = I2S_CONTROL(port);
    uint32_t                        fifocounter = R_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->fifocounter);
    uint32_t                        cnt_mask;
    uint32_t                        cnt_shift;

    if (!(control->in_use & (1 << dir)))
    {
        *weight = 0;
    }
    else
    {
        if (dir == PLATFORM_I2S_READ)
        {
            cnt_mask  = I2S_FC_RX_CNT_MASK;
            cnt_shift = I2S_FC_RX_CNT_SHIFT;
        }
        else
        {
            cnt_mask  = I2S_FC_TX_CNT_MASK;
            cnt_shift = I2S_FC_TX_CNT_SHIFT;
        }
        fifocounter = (fifocounter & cnt_mask) >> cnt_shift;
        *weight = (32 / control->sample_bits) * ((fifocounter * control->sample_bits) / 8);
    }

    return result;
}

wiced_result_t wiced_i2s_get_clocking_details(const wiced_i2s_params_t *config, uint32_t *mclk )
{
    wiced_result_t      result = WICED_SUCCESS;
    size_t              ii;
    uint32_t            rate   = config->sample_rate;

    /* Set up our ClockDivider register with audio sample rate. */
    for (ii = 0; ii < ARRAYSIZE(i2s_clkdiv_coeffs); ii++)
    {
        if (i2s_clkdiv_coeffs[ii].rate == rate)
        {
            break;
        }
    }

    if ((mclk != NULL) && (ii < ARRAYSIZE(i2s_clkdiv_coeffs)))
    {
        *mclk = i2s_clkdiv_coeffs[ii].mclk;
    }
    else
    {
        result = WICED_BADARG;
    }

    return result;
}

/******************************************************
 *          Static Function Definitions
 ******************************************************/

static wiced_result_t i2s_stop_stream( platform_i2s_port_t port, platform_i2s_direction_t dir )
{
    i2s_peripheral_transfer_enable(port, dir, I2S_OPS(dir)->peripheral_transfer_enable_val, WICED_FALSE);
    (*I2S_OPS(dir)->interrupts_on)(port, WICED_FALSE);
    (*I2S_OPS(dir)->dma_init)(port, WICED_FALSE);

    return WICED_SUCCESS;
}

/* Cleanup peripheral port. */
static void i2s_cleanup( platform_i2s_port_t port )
{
    platform_irq_disable_irq( I2S_PORT_INFO(port)->irqn );

    if (I2S_CONTROL(port)->sih != NULL)
    {
        if (I2S_CONTROL(port)->pll_configured)
        {
            /* FIXME There is coupling between the I2S peripherals w.r.t. the audio PLL that
             * isn't captured here.  For now, use a big-hammer.
             */
            i2s_disable_pll(I2S_CONTROL(port)->sih);
            I2S_CONTROL(port)->pll_configured = 0;
        }

        si_core_disable(I2S_CONTROL(port)->sih, 0);

        si_detach(I2S_CONTROL(port)->sih);

        I2S_CONTROL(port)->sih = NULL;
        I2S_CONTROL(port)->regs = NULL;
    }
}

/* XXX   These macros attempt to follow the format in sbchipc.h.
 * FIXME Figure out how to integrate.
 */

#define PMU_CC6_AUDIOPLL_EN_PWRDN_MASK          (1 << PMU_CC6_AUDIOPLL_EN_PWRDN_SHIFT)
#define PMU_CC6_AUDIOPLL_EN_PWRDN_SHIFT         (16)
#define PMU_CC6_AUDIOPLL_EN_PWRDN               (PMU_CC6_AUDIOPLL_EN_PWRDN_MASK)

#define PMU_CC6_AUDIOPLL_FORCE_ARESET_MASK      (1 << PMU_CC6_AUDIOPLL_FORCE_ARESET_SHIFT)
#define PMU_CC6_AUDIOPLL_FORCE_ARESET_SHIFT     (17)
#define PMU_CC6_AUDIOPLL_FORCE_ARESET           (PMU_CC6_AUDIOPLL_FORCE_ARESET_MASK)

#define PMU_CC6_AUDIOPLL_FORCE_DRESET_MASK      (1 << PMU_CC6_AUDIOPLL_FORCE_DRESET_SHIFT)
#define PMU_CC6_AUDIOPLL_FORCE_DRESET_SHIFT     (19)
#define PMU_CC6_AUDIOPLL_FORCE_DRESET           (PMU_CC6_AUDIOPLL_FORCE_DRESET_MASK)

#define PMU_CC6_AUDIOPLL_EN_ASCU_AVBCLK_MASK    (1 << PMU_CC6_AUDIOPLL_EN_ASCU_AVBCLK_SHIFT)
#define PMU_CC6_AUDIOPLL_EN_ASCU_AVBCLK_SHIFT   (21)
#define PMU_CC6_AUDIOPLL_EN_ASCU_AVBCLK         (PMU_CC6_AUDIOPLL_EN_ASCU_AVBCLK_MASK)

#define PMU_CC6_AUDIOPLL_EN_APLLCLK_MASK        (1 << PMU_CC6_AUDIOPLL_EN_APLLCLK_SHIFT)
#define PMU_CC6_AUDIOPLL_EN_APLLCLK_SHIFT       (22)
#define PMU_CC6_AUDIOPLL_EN_APLLCLK             (PMU_CC6_AUDIOPLL_EN_APLLCLK_MASK)

#define PMU_CC6_AUDIOPLL_EN_M1CLK_MASK          (1 << PMU_CC6_AUDIOPLL_EN_M1CLK_SHIFT)
#define PMU_CC6_AUDIOPLL_EN_M1CLK_SHIFT         (24)
#define PMU_CC6_AUDIOPLL_EN_M1CLK               (PMU_CC6_AUDIOPLL_EN_M1CLK_MASK)

#define PMU_CC6_AUDIOPLL_EN_M2CLK_MASK          (1 << PMU_CC6_AUDIOPLL_EN_M2CLK_SHIFT)
#define PMU_CC6_AUDIOPLL_EN_M2CLK_SHIFT         (25)
#define PMU_CC6_AUDIOPLL_EN_M2CLK               (PMU_CC6_AUDIOPLL_EN_M2CLK_MASK)

#define PMU0_PLL0_PLLCTL8                       8
#define PMU0_PLL0_PLLCTL9                       9
#define PMU0_PLL0_PLLCTL10                      10
#define PMU0_PLL0_PLLCTL11                      11

#define AUDIOPLL_PLLCTL0                        PMU0_PLL0_PLLCTL8
#define AUDIOPLL_PLLCTL1                        PMU0_PLL0_PLLCTL9
#define AUDIOPLL_PLLCTL2                        PMU0_PLL0_PLLCTL10
#define AUDIOPLL_PLLCTL3                        PMU0_PLL0_PLLCTL11

#define AUDIOPLL_PC0_I_ENB_CLKOUT_MASK          0x00000001
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SHIFT         0
#define AUDIOPLL_PC0_I_ENB_CLKOUT_BITS          1

/* Post-divider channel enable. */
#define AUDIOPLL_PC0_I_HOLD_CH_MASK             0x0000000E
#define AUDIOPLL_PC0_I_HOLD_CH_SHIFT            1
#define AUDIOPLL_PC0_I_HOLD_CH_BITS             3

#define AUDIOPLL_PC0_I_CH1_DEL_MASK             0x00000030
#define AUDIOPLL_PC0_I_CH1_DEL_SHIFT            4
#define AUDIOPLL_PC0_I_CH1_DEL_BITS             2
#define AUDIOPLL_PC0_I_CH2_DEL_MASK             0x000000C0
#define AUDIOPLL_PC0_I_CH2_DEL_SHIFT            6
#define AUDIOPLL_PC0_I_CH1_DEL_BITS             2
#define AUDIOPLL_PC0_I_CH3_DEL_MASK             0x00000300
#define AUDIOPLL_PC0_I_CH3_DEL_SHIFT            8
#define AUDIOPLL_PC0_I_CH3_DEL_BITS             2

#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_MASK   0x00001C00
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_SHIFT  10
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_BITS   3

#define AUDIOPLL_PC0_I_NDIV_MODE_MASK           0x0000E000
#define AUDIOPLL_PC0_I_NDIV_MODE_SHIFT          13
#define AUDIOPLL_PC0_I_NDIV_MODE_BITS           3
#define AUDIOPLL_PC0_I_NDIV_MODE_UNIT           (0 << AUDIOPLL_PC0_I_NDIV_MODE_SHIFT)
#define AUDIOPLL_PC0_I_NDIV_MODE_EIGHTH         (1 << AUDIOPLL_PC0_I_NDIV_MODE_SHIFT)
#define AUDIOPLL_PC0_I_NDIV_MODE_MASH11         (2 << AUDIOPLL_PC0_I_NDIV_MODE_SHIFT)
#define AUDIOPLL_PC0_I_NDIV_MODE_MASH111        (3 << AUDIOPLL_PC0_I_NDIV_MODE_SHIFT)

#define AUDIOPLL_PC0_I_LOAD_EN_CH_MASK          0x00070000
#define AUDIOPLL_PC0_I_LOAD_EN_CH_SHIFT         16
#define AUDIOPLL_PC0_I_LOAD_EN_CH_BITS          3

#define AUDIOPLL_PC0_I_P1DIV_MASK               0x00780000
#define AUDIOPLL_PC0_I_P1DIV_SHIFT              19
#define AUDIOPLL_PC0_I_P1DIV_BITS               4

#define AUDIOPLL_PC0_I_NDIV_INT_MASK            0xFF800000
#define AUDIOPLL_PC0_I_NDIV_INT_SHIFT           23
#define AUDIOPLL_PC0_I_NDIV_INT_BITS            9

#define AUDIOPLL_PC1_I_M1DIV_CH1_MASK           0x000000FF
#define AUDIOPLL_PC1_I_M1DIV_CH1_SHIFT          0
#define AUDIOPLL_PC1_I_M1DIV_CH1_BITS           8

#define AUDIOPLL_PC1_I_M1DIV_CH2_MASK           0x0000FF00
#define AUDIOPLL_PC1_I_M1DIV_CH2_SHIFT          8
#define AUDIOPLL_PC1_I_M1DIV_CH2_BITS           8

#define AUDIOPLL_PC1_I_M1DIV_CH3_MASK           0x00FF0000
#define AUDIOPLL_PC1_I_M1DIV_CH3_SHIFT          16
#define AUDIOPLL_PC1_I_M1DIV_CH3_BITS           8

#define AUDIOPLL_PC1_PLLCTL_MASK                0xFF000000
#define AUDIOPLL_PC1_PLLCTL_SHIFT               24
#define AUDIOPLL_PC1_PLLCTL_BITS                8
#define AUDIOPLL_PC1_PLLCTL_DEFAULT_VALUE       (0xE1 << AUDIOPLL_PC1_PLLCTL_SHIFT)

#define AUDIOPLL_PC2_I_NDIV_FRAC_MASK           0x00FFFFFF
#define AUDIOPLL_PC2_I_NDIV_FRAC_SHIFT          0
#define AUDIOPLL_PC2_I_NDIV_FRAC_BITS           24

#define CST4390X_AUDIOPLL_LOCKED                0x00020000

/* Helpers */

#define AUDIOPLL_PC0_I_HOLD_CHX(_X_, _V_)       \
    (((_V_) != AUDIOPLL_PC0_I_HOLD_CHX_ENABLED) ? (((1 << (_X_)) << AUDIOPLL_PC0_I_HOLD_CH_SHIFT) & AUDIOPLL_PC0_I_HOLD_CH_MASK) : 0)
#define AUDIOPLL_PC0_I_HOLD_CHX_MASK(_X_)       AUDIOPLL_PC0_I_HOLD_CHX(_X_, ~0)
#define AUDIOPLL_PC0_I_HOLD_CHX_ENABLED         0
#define AUDIOPLL_PC0_I_HOLD_CHX_DISABLED        1

#define AUDIOPLL_PC0_I_CHX_DEL(_X_, _V_)        (((_V_) & 0x3) << (((_X_) * 2) + 4))
#define AUDIOPLL_PC0_I_CHX_DEL_MASK(_X_)        AUDIOPLL_PC0_I_CHX_DEL(_X_, ~0)
#define AUDIOPLL_PC0_I_CHX_NO_DELAY             0
#define AUDIOPLL_PC0_I_CHX_0P5_PERIOD_DELAY     1
#define AUDIOPLL_PC0_I_CHX_1P0_PERIOD_DELAY     2
#define AUDIOPLL_PC0_I_CHX_1P5_PERIOD_DELAY     3

#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(_X_, _V_) \
    (((_V_) != AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_ENABLED) ? (((1 << (_X_)) << AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_SHIFT) & AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_MASK) : 0 )
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_MASK(_X_) \
                                                AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(_X_, ~0)
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_ENABLED    0
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_DISABLED   1

#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CH1    AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(0, AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_DISABLED)
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CH2    AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(1, AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_DISABLED)
#define AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CH3    AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(2, AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_DISABLED)

#define AUDIOPLL_PC0_I_LOAD_EN_CH_CHX(_X_, _V_) \
    (((_V_) != AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_HOLD) ? (((1 << (_X_)) << AUDIOPLL_PC0_I_LOAD_EN_CH_SHIFT) & AUDIOPLL_PC0_I_LOAD_EN_CH_MASK) : 0 )
#define AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_MASK(_X_) AUDIOPLL_PC0_I_LOAD_EN_CH_CHX(_X_, ~0)
#define AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_LOAD      1
#define AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_HOLD      0
#define AUDIOPLL_PC0_I_LOAD_EN_CH_CH1           AUDIOPLL_PC0_I_LOAD_EN_CH_CHX(0, AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_LOAD)
#define AUDIOPLL_PC0_I_LOAD_EN_CH_CH2           AUDIOPLL_PC0_I_LOAD_EN_CH_CHX(1, AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_LOAD)
#define AUDIOPLL_PC0_I_LOAD_EN_CH_CH3           AUDIOPLL_PC0_I_LOAD_EN_CH_CHX(2, AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_LOAD)

#define AUDIOPLL_PC0_I_P1DIV(_V_)               (((_V_) << AUDIOPLL_PC0_I_P1DIV_SHIFT) & AUDIOPLL_PC0_I_P1DIV_MASK)

#define AUDIOPLL_PC0_I_NDIV_INT(_V_)            (((_V_) << AUDIOPLL_PC0_I_NDIV_INT_SHIFT) & AUDIOPLL_PC0_I_NDIV_INT_MASK)

#define AUDIOPLL_PC1_I_M1DIV_CHX(_X_, _V_)      (((_V_) & 0xFF) << ((_X_) * 8))
#define AUDIOPLL_PC1_I_M1DIV_CHX_MASK(_X_)      AUDIOPLL_PC1_I_M1DIV_CHX(_X_, ~0)

#define AUDIOPLL_PC2_I_NDIV_FRAC(_V_)           (((_V_) << AUDIOPLL_PC2_I_NDIV_FRAC_SHIFT) & AUDIOPLL_PC2_I_NDIV_FRAC_MASK)


static inline void i2s_pll_configure_channel(si_t *sih, const platform_audio_pll_channels_t ch, uint8_t i_mdiv)
{
    si_pmu_pllcontrol(sih,
            AUDIOPLL_PLLCTL0,
            AUDIOPLL_PC0_I_HOLD_CHX_MASK(ch)        |
            AUDIOPLL_PC0_I_CHX_DEL_MASK(ch)         |
            AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_MASK(ch) |
            AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_MASK(ch)
            ,
            AUDIOPLL_PC0_I_HOLD_CHX(ch, AUDIOPLL_PC0_I_HOLD_CHX_ENABLED) |
            AUDIOPLL_PC0_I_CHX_DEL(ch, AUDIOPLL_PC0_I_CHX_NO_DELAY) |
            AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(ch, AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_DISABLED) |
            AUDIOPLL_PC0_I_LOAD_EN_CH_CHX(ch, AUDIOPLL_PC0_I_LOAD_EN_CH_CHX_LOAD)
            );
    si_pmu_pllcontrol(sih,
            AUDIOPLL_PLLCTL1,
            AUDIOPLL_PC1_I_M1DIV_CHX_MASK(ch),
            AUDIOPLL_PC1_I_M1DIV_CHX(ch, i_mdiv));
}
inline static void i2s_pll_enable_post_dividers(si_t *sih, const platform_audio_pll_channels_t ch)
{
    si_pmu_pllcontrol(sih,
            AUDIOPLL_PLLCTL0,
            AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_MASK(ch),
            AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX(ch, AUDIOPLL_PC0_I_ENB_CLKOUT_SINGLE_CHX_ENABLED));
}



inline static void i2s_pll_enable_clk(si_t *sih, const platform_audio_pll_channels_t ch)
{
    static const uint32_t cc6_clk_enab[BCM4390X_AUDIO_PLL_MAX] =
    {
        [BCM4390X_AUDIO_PLL_AUDIO_SYNC] = PMU_CC6_AUDIOPLL_EN_APLLCLK,
        [BCM4390X_AUDIO_PLL_MCLK1]      = PMU_CC6_AUDIOPLL_EN_M1CLK,
        [BCM4390X_AUDIO_PLL_MCLK2]      = PMU_CC6_AUDIOPLL_EN_M2CLK,
    };
    si_pmu_chipcontrol(sih,
            PMU_CHIPCTL6,
            cc6_clk_enab[ch],
            cc6_clk_enab[ch]);
}

wiced_result_t wiced_i2s_pll_set_fractional_divider(wiced_i2s_t i2s, float value)
{
    wiced_result_t        result = WICED_ERROR;
    platform_i2s_direction_t dir = I2S_INTERFACE(i2s)->stream_direction;
    platform_i2s_port_t     port = I2S_INTERFACE(i2s)->port_info->port;
    i2s_control_t       *control = I2S_CONTROL(port);
    si_t                    *sih = I2S_CONTROL(port)->sih;
    uint32_t        new_frac_div = 0;
    double                   VCO = -1.0f;
    double               new_VCO = 0.0f;
    double    new_frac_div_float = 0.0f;
    double            ndiv_float = -1.0f;
    wiced_bool_t         b_apply = WICED_FALSE;
    int                       ch = 2;

    enum  {
        FREQUENCY_44p1K=0,
        FREQUENCY_48K,
        FREQUENCY_88p2K,
        FREQUENCY_96K,
        FREQUENCY_192K,
        FREQUENCY_384K
    };

    static const struct i2s_pll_config
    {
            uint8_t     i_mdiv;
            uint32_t    frequency;
            uint32_t    mclk;
            uint32_t    i_ndiv_frac;
            uint16_t    i_ndiv_int;
            double      vco;
    } i2s_pll_config[] =
    {
            /* 11.2896 MHz */
            {.frequency =  44100, .mclk = 11289600,  .i_ndiv_int = 38,  .i_ndiv_frac = 10707273, .i_mdiv = 64, .vco = 722534399.91 },
            /* 12.288  MHz  */
            {.frequency =  48000, .mclk = 12288000,  .i_ndiv_int = 42,  .i_ndiv_frac =   925887, .i_mdiv = 64, .vco = 786432000.00 },
            /* 22.5792 MHz */
            {.frequency =  88200, .mclk = 22579200,  .i_ndiv_int = 38,  .i_ndiv_frac = 10707273, .i_mdiv = 32, .vco = 722534399.91 },
            /* 24.576  MHz  */
            {.frequency =  96000, .mclk = 24576000,  .i_ndiv_int = 42,  .i_ndiv_frac =   925887, .i_mdiv = 32, .vco = 786432000.00 },
            /* 24.576  MHz  */
            {.frequency = 192000, .mclk = 24576000,  .i_ndiv_int = 42,  .i_ndiv_frac =   925887, .i_mdiv = 32, .vco = 786432000.00 },
            /* 49.152  MHz  */
            {.frequency = 384000, .mclk = 49152000,  .i_ndiv_int = 42,  .i_ndiv_frac =   925887, .i_mdiv = 16, .vco = 786432000.00 },
    };

    if (!(control->in_use & (1 << dir)))
    {
        goto ERROR_STREAM_NOT_IN_USE;
    }

    /**
     * Value contains the change in PPM (applies to either VCO or MCLK)
     * This needs to be converted to the fractional value.
     */

    switch( control->sample_rate )
    {
        case 44100:
            VCO        = i2s_pll_config[FREQUENCY_44p1K].vco;
            ndiv_float = (double)i2s_pll_config[FREQUENCY_44p1K].i_ndiv_int;
            break;

        case 48000:
            VCO        = i2s_pll_config[FREQUENCY_48K].vco;
            ndiv_float = (double)i2s_pll_config[FREQUENCY_48K].i_ndiv_int;
            break;

        case 88200:
            VCO        = i2s_pll_config[FREQUENCY_88p2K].vco;
            ndiv_float = (double)i2s_pll_config[FREQUENCY_88p2K].i_ndiv_int;
            break;

        case 96000:
            VCO        = i2s_pll_config[FREQUENCY_96K].vco;
            ndiv_float = (double)i2s_pll_config[FREQUENCY_96K].i_ndiv_int;
            break;

        case 192000:
            VCO        = i2s_pll_config[FREQUENCY_192K].vco;
            ndiv_float = (double)i2s_pll_config[FREQUENCY_192K].i_ndiv_int;
            break;

        case 384000:
            VCO        = i2s_pll_config[FREQUENCY_384K].vco;
            ndiv_float = (double)i2s_pll_config[FREQUENCY_384K].i_ndiv_int;
            break;

        default:
            break;
    }

    if ( (VCO >= 0) && (ndiv_float >=0) )
    {
        new_VCO            = ( VCO * ( (double)1.0f + ( value / (double)1000000.0f) ) );
        new_frac_div_float = ( ( ( 2 * new_VCO ) / (double)XTAL_FREQUENCY )  - ndiv_float ) * TWO_POW24;
        b_apply            = ((new_frac_div_float >= 0) && (new_frac_div_float < TWO_POW24)) ? WICED_TRUE : WICED_FALSE;
        new_frac_div       = (uint32_t) new_frac_div_float;
    }

    if ( b_apply == WICED_TRUE )
    {
        si_pmu_pllcontrol(sih,
                          AUDIOPLL_PLLCTL2,
                          AUDIOPLL_PC2_I_NDIV_FRAC_MASK,
                          AUDIOPLL_PC2_I_NDIV_FRAC(new_frac_div));

        i2s_pll_configure_channel(sih, BCM4390X_AUDIO_PLL_AUDIO_SYNC, 0x4 /* recommended */);

        si_pmu_pllupd(sih);

        /* Enable post divider outputs. */
        i2s_pll_enable_post_dividers(sih, ch);
        i2s_pll_enable_post_dividers(sih, BCM4390X_AUDIO_PLL_AUDIO_SYNC);
        si_pmu_pllupd(sih);

        /* Clk_ht enables for apll_clk m1_clk and m2_clk. */
        i2s_pll_enable_clk(sih, ch);
        i2s_pll_enable_clk(sih, BCM4390X_AUDIO_PLL_AUDIO_SYNC);

        result = WICED_SUCCESS;
    }

ERROR_STREAM_NOT_IN_USE:
    return result;
}



static wiced_result_t i2s_disable_pll( si_t *sih )
{
    /* XXX Is this the correct power-down sequence? */
    si_pmu_chipcontrol(sih,
            PMU_CHIPCTL6,
            PMU_CC6_AUDIOPLL_EN_PWRDN_MASK,
            PMU_CC6_AUDIOPLL_EN_PWRDN);

    return WICED_SUCCESS;
}

/* FIXME This needs to be shared correctly between the I2S peripherals! */
static wiced_result_t i2s_configure_pll( osl_t *osh, si_t *sih, const platform_audio_pll_channels_t ch, uint32_t mclk )
{
    /* FIXME: Assumes 37.4MHz xtal and variable Fvco (n-divider is not constant across mclks). */
    static const struct i2s_pll_config
    {
        uint32_t    mclk;
        uint32_t    i_ndiv_frac;
        uint16_t    i_ndiv_int;
        uint8_t     i_mdiv;
    } i2s_pll_config[] =
    {
        /* 11.2896MHz */
        {.mclk = 11289600,  .i_ndiv_int = 38,  .i_ndiv_frac = 10707273, .i_mdiv = 64 },
        /* 12.288MHz */
        {.mclk = 12288000,  .i_ndiv_int = 42,  .i_ndiv_frac = 925887,   .i_mdiv = 64 },
        /* 22.5792MHz */
        {.mclk = 22579200,  .i_ndiv_int = 38,  .i_ndiv_frac = 10707273, .i_mdiv = 32 },
        /* 24.576MHz */
        {.mclk = 24576000,  .i_ndiv_int = 42,  .i_ndiv_frac = 925887,   .i_mdiv = 32 },
    };
    size_t          i;

    for (i = 0; i < ARRAYSIZE(i2s_pll_config); i++)
    {
        if (i2s_pll_config[i].mclk == mclk)
            break;
    }
    if (i == ARRAYSIZE(i2s_pll_config))
    {
        return WICED_BADARG;
    }

    /* XXX Note that apll_clk needs to be enabled for the ASCU, otherwise I2S won't clock-out
     * data in master mode.
    */

    /* Release pwrdn. */
    si_pmu_chipcontrol(sih,
            PMU_CHIPCTL6,
            PMU_CC6_AUDIOPLL_EN_PWRDN_MASK | PMU_CC6_AUDIOPLL_FORCE_ARESET_MASK | PMU_CC6_AUDIOPLL_FORCE_DRESET_MASK,
            PMU_CC6_AUDIOPLL_FORCE_ARESET | PMU_CC6_AUDIOPLL_FORCE_DRESET);

    /* Release areset. */
    si_pmu_chipcontrol(sih,
            PMU_CHIPCTL6,
            PMU_CC6_AUDIOPLL_FORCE_ARESET_MASK,
            0);

    /* Analog circuit should be up and running so you can program the frequency values and clock
     * output enables and other register settings.
     * Do not enable the individual channel outputs until the APLL is locked and dreset is
     * released (in that order).
     */

    /* XXX Can we support fixed n-dividers so this can be global? */
    si_pmu_pllcontrol(sih,
            AUDIOPLL_PLLCTL0,
            AUDIOPLL_PC0_I_ENB_CLKOUT_MASK          |
            AUDIOPLL_PC0_I_NDIV_MODE_MASK           |
            AUDIOPLL_PC0_I_P1DIV_MASK               |
            AUDIOPLL_PC0_I_NDIV_INT_MASK,
            AUDIOPLL_PC0_I_NDIV_MODE_MASH111        |
            AUDIOPLL_PC0_I_P1DIV(2)                 |
            AUDIOPLL_PC0_I_NDIV_INT(i2s_pll_config[i].i_ndiv_int));
    si_pmu_pllcontrol(sih,
            AUDIOPLL_PLLCTL2,
            AUDIOPLL_PC2_I_NDIV_FRAC_MASK,
            AUDIOPLL_PC2_I_NDIV_FRAC(i2s_pll_config[i].i_ndiv_frac));
#if 0
    si_pmu_pllcontrol(sih,
            AUDIOPLL_PLLCTL3,
            ~0,
            0x02600004);
#endif
    i2s_pll_configure_channel(sih, ch, i2s_pll_config[i].i_mdiv);
    i2s_pll_configure_channel(sih, BCM4390X_AUDIO_PLL_AUDIO_SYNC, 0x4 /* recommended */);

    si_pmu_pllupd(sih);

    /* Poll the audio pll lock status register - once this is high we can release dreset and the enable
     * the individual pll post divider output channels.
     */
    {
        /* XXX Use si_setcoreidx instead? */
        chipcregs_t     *cc = (void *)PLATFORM_CHIPCOMMON_REGBASE( 0x0 );
        while ((R_REG(osh, &cc->chipstatus) & CST4390X_AUDIOPLL_LOCKED) == 0);
    }

    /* Release dreset. */
    si_pmu_chipcontrol(sih,
            PMU_CHIPCTL6,
            PMU_CC6_AUDIOPLL_FORCE_DRESET_MASK,
            0);

    /* Enable post divider outputs. */
    i2s_pll_enable_post_dividers(sih, ch);
    i2s_pll_enable_post_dividers(sih, BCM4390X_AUDIO_PLL_AUDIO_SYNC);
    si_pmu_pllupd(sih);

    /* Clk_ht enables for apll_clk m1_clk and m2_clk. */
    i2s_pll_enable_clk(sih, ch);
    i2s_pll_enable_clk(sih, BCM4390X_AUDIO_PLL_AUDIO_SYNC);

    return WICED_SUCCESS;
}

/* ClockDivider.SRate */
static wiced_result_t i2s_set_clkdiv( platform_i2s_port_t port, const wiced_i2s_params_t *config, platform_audio_pll_channels_t audio_pll_ch, uint32_t *mclk )
{
    wiced_result_t      result;
    size_t              ii;
    uint32_t            rate        = config->sample_rate;

    /* Set up our ClockDivider register with audio sample rate. */
    for (ii = 0; ii < ARRAYSIZE(i2s_clkdiv_coeffs); ii++)
    {
        if (i2s_clkdiv_coeffs[ii].rate == rate)
        {
            break;
        }
    }

    if (ii < ARRAYSIZE(i2s_clkdiv_coeffs))
    {
        /* Write the new SRATE into the clock divider register */
        uint32_t srate  = (i2s_clkdiv_coeffs[ii].srate << I2S_CLKDIV_SRATE_SHIFT);
        uint32_t clkdiv = R_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->clkdivider);

        clkdiv &= ~I2S_CLKDIV_SRATE_MASK;
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->clkdivider, clkdiv | srate);

        *mclk = i2s_clkdiv_coeffs[ii].mclk;

        /* FIXME: Move this to the data structure which describes the I2S/codec binding. */
        result = i2s_configure_pll(I2S_CONTROL(port)->osh, I2S_CONTROL(port)->sih, audio_pll_ch, *mclk);
        if (result == WICED_SUCCESS)
        {
            I2S_CONTROL(port)->pll_configured = 1;
        }

        if (config->i2s_spdif_mode == WICED_I2S_SPDIF_MODE_ON)
        {
            uint32_t period_cnt = i2s_clkdiv_coeffs[ii].spdif_period_cnt;

            W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->srxpcnt, period_cnt);
        }
    }
    else
    {
        result = WICED_BADARG;
    }

    return result;
}

static wiced_result_t i2s_configure_peripheral( wiced_i2s_t i2s, const wiced_i2s_params_t *config, uint32_t *mclk )
{
    wiced_result_t                  result        = WICED_SUCCESS;
    platform_i2s_direction_t        dir           = I2S_INTERFACE(i2s)->stream_direction;
    const platform_i2s_port_info_t* port_info     = I2S_INTERFACE(i2s)->port_info;
    platform_i2s_port_t             port          = port_info->port;
    platform_audio_pll_channels_t   audio_pll_ch  = port_info->audio_pll_ch;
    unsigned int                    is_master     = port_info->is_master;
    uint32_t                        devctrl       = R_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->devcontrol);
    uint32_t                        srxctrl       = R_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->srxctrl);
    uint32_t                        i2sctrl       = 0;

    /* Touching DevControl when in_use will cause the I2S FIFO to be
     * reset thus introducing an audible "pop" on the in-use stream.
     * Don't call if in-use.
     */
    wiced_assert("Port already configured!", 0 == (I2S_CONTROL(port)->in_use & ~(1 << dir)));

    /* I2S only (for now). */
    devctrl &= ~I2S_DC_MODE_TDM;

    /* Setup I2S frame master. */
    if (is_master)
    {
        devctrl &= ~I2S_DC_BCLKD_IN;

        /* ClockDivider.SRate */
        result = i2s_set_clkdiv(port, config, audio_pll_ch, mclk);
        if (result != WICED_SUCCESS)
            goto DONE;

        /* Force I2S0/1 sync start. */
        /* XXX Required for DMA not to occasional throw a protocol error.
         * Why do we need this?!
        */
        REG32(0x18000200) = 0x60;
    }
    else
    {
        devctrl |= I2S_DC_BCLKD_IN;
        *mclk = 0;
    }

    /* DevControl.OPCHSEL */
    {
        /* Set up for the # of channels in this stream */
        /* For I2S/SPDIF we support 2 channel -OR- 6 (5.1) channels */
        switch ( config->channels )
        {
        case 2:
            devctrl &= ~I2S_DC_OPCHSEL_6;
            break;
        case 6:
            devctrl |= I2S_DC_OPCHSEL_6;
            break;
        default:
            result = WICED_BADARG;
            goto DONE;
        }
    }

    /* Clear TX/RX word length bits then set the # of bits per sample in this stream.
     * This could be configured independently of the in-use substream (see in_use
     * comment above).  For simplicity, set both RX and TX substreams.
    */
    {
        /* DevControl.WL_RX */
        {
            devctrl &= ~I2S_DC_WL_RX_MASK;
            srxctrl &= ~I2S_SRXC_WL_MASK;

            switch ( config->bits_per_sample )
            {
                /* Available in I2S core rev > 2. */
                case 8:
                    devctrl |= 0x8000;
                    srxctrl |= 0x1000;
                    break;
                case 16:
                    devctrl |= 0x0;
                    srxctrl |= 0x0;
                    /* LCH data in least-significant word. */
                    i2sctrl |= I2S_CTRL_RXLR_CHSEL;
                    break;
                case 20:
                    devctrl |= 0x1000;
                    srxctrl |= 0x1;
                    break;
                case 24:
                    devctrl |= 0x2000;
                    srxctrl |= 0x2;
                    break;
                case 32:
                    devctrl |= 0x3000;
                    break;
                    /* XXX: no 16bextended! */
                default:
                    result = WICED_BADARG;
                    goto DONE;
            }
        }

        /* DevControl.WL_TX */
        {
            devctrl &= ~I2S_DC_WL_TX_MASK;
            switch ( config->bits_per_sample )
            {
                /* Available in I2S core rev > 2. */
                case 8:
                    devctrl |= 0x4000;
                    break;
                case 16:
                    devctrl |= 0x0;
                    /* LCH data in least-significant word. */
                    i2sctrl |= I2S_CTRL_TXLR_CHSEL;
                    break;
                case 20:
                    devctrl |= 0x400;
                    break;
                case 24:
                    devctrl |= 0x800;
                    break;
                case 32:
                    devctrl |= 0xC00;
                    break;
                    /* XXX: no 16bextended! */
                default:
                    result = WICED_BADARG;
                    goto DONE;
            }
        }
    }

    /* Only enable full duplex when not in SPDIF mode */
    devctrl &= (~I2S_DC_DPX_MASK);
    if (config->i2s_spdif_mode != WICED_I2S_SPDIF_MODE_ON)
    {
        /* Full duplex. */
        devctrl |= I2S_DC_DPX_MASK;
    }
    else if (dir == PLATFORM_I2S_READ)
    {
        /* SPDIF mode is on, and we're RX'ing */

        /* Set half-duplex RX */
        devctrl |= (0x1<<I2S_DC_DPX_SHIFT);

        srxctrl |= (I2S_SRXC_RXEN | I2S_SRXC_RXSTEN | I2S_SRXC_HW_PCNTEN);
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->srxctrl, srxctrl);
    }
    else
    {
        /* SPDIF mode is on, and we're TX'ing */
        result = WICED_UNSUPPORTED;
        goto DONE;
    }

    /* Write I2S devcontrol reg. */
    W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->devcontrol, devctrl);

    /* DevControl.I2SCFG */
    {
        /* Latch settings. */
        devctrl |= I2S_DC_I2SCFG;
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->devcontrol, devctrl);
    }

    /* Write I2S i2scontrol reg. */
    W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->i2scontrol, i2sctrl);

DONE:
    return result;
}

/* Enable/disable stream transfer. */
static wiced_result_t i2s_peripheral_transfer_enable( platform_i2s_port_t port, platform_i2s_direction_t dir, uint32_t bits, wiced_bool_t on)
{
    if (on == WICED_TRUE)
    {
        OR_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->i2scontrol, bits);

        if (I2S_CONTROL(port)->i2s_spdif_mode == WICED_I2S_SPDIF_MODE_ON)
        {
            if (dir == PLATFORM_I2S_READ)
            {
                OR_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->srxctrl, I2S_SRXC_RXEN);
            }
            else
            {
                OR_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->stxctrl, I2S_STXC_TXEN);
            }
        }
    }
    else
    {
        AND_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->i2scontrol, ~bits);

        if (I2S_CONTROL(port)->i2s_spdif_mode == WICED_I2S_SPDIF_MODE_ON)
        {
            if (dir == PLATFORM_I2S_READ)
            {
                AND_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->srxctrl, ~I2S_SRXC_RXEN);
            }
            else
            {
                AND_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->stxctrl, ~I2S_STXC_TXEN);
            }
        }
    }

    return WICED_SUCCESS;
}

static wiced_result_t i2s_dma_attach( platform_i2s_port_t port, platform_i2s_direction_t dir, uint16_t period_size )
{
    i2s_control_t   *control    = I2S_CONTROL(port);
    wiced_result_t  result;

    result = (*I2S_OPS(dir)->dma_attach)(port, period_size);
    if (result == WICED_SUCCESS)
    {
        dma_ctrlflags(control->di[dir], DMA_CTRL_UNFRAMED /* mask */, DMA_CTRL_UNFRAMED /* value */);
    }

    return result;
}

static wiced_result_t i2s_op_rx_dma_attach( platform_i2s_port_t port, uint16_t period_size )
{
    i2s_control_t   *control            = I2S_CONTROL(port);
    const platform_i2s_direction_t dir  = PLATFORM_I2S_READ;
    hnddma_t        *di;
    wiced_result_t  result;

    //wiced_assert("bad handle", control->osh != NULL);
    wiced_assert("bad handle", control->sih != NULL);

    result = WICED_SUCCESS;

    di = dma_attach(
            control->osh,
            "i2s_dma_rx",
            control->sih,
            NULL,   /* dmaregstx */
            &I2S_REGS(port)->dmaregs[0].dmarcv,
            0,      /* ntxd */
            I2S_NR_DMA_RXDS_MAX,
            period_size + I2S_DMA_RXOFS_BYTES,
            0,      /* rxextraheadroom */
            0,      /* nrxpost */
            I2S_DMA_RXOFS_BYTES,
            &msg_level);
    if (di == NULL)
    {
        result = WICED_ERROR;
    }

    /* Copy out. */
    if (result == WICED_SUCCESS)
    {
        control->di[dir] = di;
    }

    return result;
}

static wiced_result_t i2s_op_tx_dma_attach( platform_i2s_port_t port, uint16_t period_size )
{
    i2s_control_t   *control            = I2S_CONTROL(port);
    const platform_i2s_direction_t dir  = PLATFORM_I2S_WRITE;
    hnddma_t        *di;
    wiced_result_t  result;

    UNUSED_PARAMETER(period_size);

    result = WICED_SUCCESS;

    //wiced_assert("bad handle", control->osh != NULL);
    wiced_assert("bad handle", control->sih != NULL);

    di = dma_attach(
            control->osh,
            "i2s_dma_tx",
            control->sih,
            &I2S_REGS(port)->dmaregs[0].dmaxmt,
            NULL,   /* dmaregsrx */
            I2S_NR_DMA_TXDS_MAX,
            0,      /* nrxd */
            0,      /* rxbufsize */
            0,      /* rxextraheadroom */
            0,      /* nrxpost */
            0,      /* rxoffset */
            &msg_level);
    if (di == NULL)
    {
        result = WICED_ERROR;
    }

    /* Copy out. */
    if (result == WICED_SUCCESS)
    {
        control->di[dir] = di;
    }

    return result;
}

static wiced_result_t i2s_op_rx_dma_init( platform_i2s_port_t port, wiced_bool_t do_init )
{
    hnddma_t    *di     = I2S_CONTROL(port)->di[PLATFORM_I2S_READ];

    dma_rxreset(di);

    if (do_init == WICED_TRUE)
    {
        dma_rxinit(di);
    }
    else
    {
        dma_rxreclaim(di);
    }

    return WICED_SUCCESS;
}

static wiced_result_t i2s_op_tx_dma_init( platform_i2s_port_t port, wiced_bool_t do_init )
{
    hnddma_t    *di     = I2S_CONTROL(port)->di[PLATFORM_I2S_WRITE];

    dma_txreset(di);

    if (do_init == WICED_TRUE)
    {
        dma_txinit(di);
    }
    else
    {
        dma_txreclaim(di, HNDDMA_RANGE_ALL);
    }

    return WICED_SUCCESS;
}

static wiced_result_t i2s_op_rx_interrupts_on( platform_i2s_port_t port, wiced_bool_t on )
{
    uint32 intmask_capture = I2S_INT_RCV_INT | I2S_INT_RCVFIFO_OFLOW;

    if (I2S_CONTROL(port)->i2s_spdif_mode == WICED_I2S_SPDIF_MODE_ON)
    {
        intmask_capture |= SPDIF_INT_MASK;
    }

    if (on == WICED_TRUE)
    {
        uint32_t i2sintrecelazy = R_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intrecvlazy);

        /* Clear any pending interrupts. */
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus, intmask_capture);
        /* Enable interrupts. */
        I2S_CONTROL(port)->intmask |= intmask_capture;
        OR_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intmask, intmask_capture);

        /* Enable I2S lazy interrupt.  The lazy interrupt is required
         * to generate reception interrupts in contrast to using the
         * IOC mechanism for TX.  Interrupt when 1 frame is
         * transferred, corresponding to one descriptor.
        */
        i2sintrecelazy  &= ~I2S_INTRECE_LAZY_FC_MASK;
        i2sintrecelazy  |= 1 << I2S_INTRECE_LAZY_FC_SHIFT;
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intrecvlazy, i2sintrecelazy);
    }
    else
    {
        AND_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intmask, ~intmask_capture);
        I2S_CONTROL(port)->intmask &= ~intmask_capture;
    }

    return WICED_SUCCESS;
}

static wiced_result_t i2s_op_tx_interrupts_on( platform_i2s_port_t port, wiced_bool_t on )
{
    static const uint32 intmask_playback   = (I2S_INT_XMT_INT | I2S_INT_XMTFIFO_UFLOW);

    if (on == WICED_TRUE)
    {
        /* Clear any pending interrupts. */
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus, intmask_playback);
        /* Enable interrupts. */
        OR_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intmask, intmask_playback);

        I2S_CONTROL(port)->intmask |= (intmask_playback);
    }
    else
    {
        AND_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intmask, ~intmask_playback);
        I2S_CONTROL(port)->intmask &= ~intmask_playback;
    }

    return WICED_SUCCESS;
}

static int i2s_op_getnextrxp( platform_i2s_port_t port )
{
    hnddma_t    *di     = I2S_CONTROL(port)->di[PLATFORM_I2S_READ];
    return dma_getnextrxp(di, FALSE) != NULL ? 0 : 1;
}

static int i2s_op_getnexttxp( platform_i2s_port_t port )
{
    hnddma_t    *di     = I2S_CONTROL(port)->di[PLATFORM_I2S_WRITE];
    return dma_getnexttxp(di, HNDDMA_RANGE_TRANSMITTED) != NULL ? 0 : 1;
}

static int i2s_op_rxfill_unframed( platform_i2s_port_t port )
{
    i2s_stream_t    *stream     = I2S_RX_STREAM(port);
    hnddma_t        *di         = I2S_CONTROL(port)->di[PLATFORM_I2S_READ];

    return dma_rxfill_unframed(di, (void *)(stream->position->data_start - I2S_DMA_RXOFS_BYTES), stream->period_size + I2S_DMA_RXOFS_BYTES, TRUE);
}

static int i2s_op_txunframed( platform_i2s_port_t port )
{
    i2s_stream_t    *stream     = I2S_TX_STREAM(port);
    hnddma_t        *di         = I2S_CONTROL(port)->di[PLATFORM_I2S_WRITE];
    return dma_txunframed(di, (void *)(stream->position->data_start - I2S_DMA_TXOFS_BYTES), stream->period_size + I2S_DMA_TXOFS_BYTES, TRUE);
}

#ifdef WICED_USE_AUDIO
static void service_transfer_complete( platform_i2s_port_t port, platform_i2s_direction_t dir )
{
    i2s_stream_t    *stream     = &I2S_CONTROL(port)->streams[dir];

    stream->periods_transferred++;

    /* Reclaim descriptor. */
    (*I2S_OPS(dir)->reclaim_dma_descriptor)(port);

    /* Acknowledge interrupt. */
    W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus, I2S_OPS(dir)->xfer_interrupt);

    /* update our position in the audio buffer, so the framework knows how much real estate in the buffer is available for a user */
    wiced_audio_buffer_platform_event(stream->sh, WICED_AUDIO_PERIOD_ELAPSED);

    /* We have transferred all requested periods; see if we have got more. */
    stream->periods_count = wiced_audio_buffer_platform_get_periods(stream->sh);

    /* We have got more to transfer; schedule transfer. */
    if (stream->periods_count > 1) //should be >1 since two periods are transferred in wiced_i2s_start().
    {
        I2S_INC_POSITION(stream);
        (*I2S_OPS(dir)->post_dma_descriptor)(port);
    }
    else if (dir == PLATFORM_I2S_READ)
    {
        i2s_stop_stream(port, dir);
    }
}

static void service_xrun( platform_i2s_port_t port, platform_i2s_direction_t dir )
{
    i2s_stream_t    *stream     = &I2S_CONTROL(port)->streams[dir];

    stream->hw_xrun++;

    /* Acknowledge interrupt. */
    W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus, I2S_OPS(dir)->xrun_interrupt);

    if (!stream->is_xrun)
    {
        //WICED_TRIGGER_BREAKPOINT();
        stream->is_xrun = 1;
        wiced_audio_buffer_platform_event(stream->sh, WICED_AUDIO_UNDERRUN);
    }
}

/* XXX Revisit sticking this in an I/O worker thread instead. */
void platform_i2s_isr( platform_i2s_port_t port )
{
    uint32_t intstatus;
    wiced_i2s_spdif_mode_t spdif_mode;

    platform_irq_disable_irq( I2S_PORT_INFO(port)->irqn );
    i2s_intrsoff(port);

    intstatus  = R_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus);
    spdif_mode = I2S_CONTROL(port)->i2s_spdif_mode;

    /* Check for unsolicited interrupts.
     * In SPDIF mode, the SPDIF status bits may be set even if the interrupt is not enabled.
     */
    if ((spdif_mode != WICED_I2S_SPDIF_MODE_ON) && (intstatus & ~I2S_CONTROL(port)->intmask))
    {
        /* Unsolicited interrupt! */
        wiced_assert("Invalid interrupt source", 0);
    }

    /* TX handling. */
    if (intstatus & I2S_INT_XMT_INT)
    {
        service_transfer_complete(port, PLATFORM_I2S_WRITE);
    }
    if (intstatus & I2S_INT_XMTFIFO_UFLOW)
    {
        service_xrun(port, PLATFORM_I2S_WRITE);
    }

    /* RX handling. */
    if (intstatus & I2S_INT_RCV_INT)
    {
        service_transfer_complete(port, PLATFORM_I2S_READ);
    }
    if (intstatus & I2S_INT_RCVFIFO_OFLOW)
    {
        service_xrun(port, PLATFORM_I2S_READ);
    }

    /* Error handling. */
    if (intstatus & I2S_INT_ERROR_MASK)
    {
        /* Stop both streams. */
        /* If already stopped above then don't stop again. */
        if (I2S_CONTROL(port)->intmask & I2S_INT_XMT_INT)
        {
            wiced_audio_buffer_platform_event(I2S_TX_STREAM(port)->sh, WICED_AUDIO_HW_ERROR);
            i2s_stop_stream(port, PLATFORM_I2S_WRITE);
        }
        if (I2S_CONTROL(port)->intmask & I2S_INT_RCV_INT)
        {
            wiced_audio_buffer_platform_event(I2S_RX_STREAM(port)->sh, WICED_AUDIO_HW_ERROR);
            i2s_stop_stream(port, PLATFORM_I2S_READ);
        }

        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus, I2S_INT_ERROR_MASK);

        /* Big-hammer. */
        /* Keep interrupts disabled; bail. */
        return;
    }

    if (spdif_mode == WICED_I2S_SPDIF_MODE_ON)
    {
        /* Clear any remaining interrupts. */
        W_REG(I2S_CONTROL(port)->osh, &I2S_REGS(port)->intstatus, intstatus);
    }

    i2s_intrson(port);
    platform_irq_enable_irq( I2S_PORT_INFO(port)->irqn );
}

/******************************************************
 *           Interrupt Handler Definitions
 ******************************************************/

/* XXX IMHO This would reside in platform.c but it's here because
 * building wwd.ping barfed.
 */

/* FIXME move this to platform_i2s.h? */
extern void platform_i2s_isr( platform_i2s_port_t port );

WWD_RTOS_DEFINE_ISR( platform_i2s0_isr )
{
    platform_i2s_isr(BCM4390X_I2S_0);
}

WWD_RTOS_DEFINE_ISR( platform_i2s1_isr )
{
    platform_i2s_isr(BCM4390X_I2S_1);
}

/******************************************************
 *            Interrupt Handlers Mapping
 ******************************************************/

WWD_RTOS_MAP_ISR( platform_i2s0_isr, I2S0_ISR )
WWD_RTOS_MAP_ISR( platform_i2s1_isr, I2S1_ISR )
#endif
