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
 *  Broadcom WLAN M2M Protocol interface
 *
 *  Implements the WICED Bus Protocol Interface for M2M
 *  Provides functions for initialising, de-intitialising 802.11 device,
 *  sending/receiving raw packets etc
 */

#include "m2m_hnddma.h"
#include "m2mdma_core.h"

#include "cr4.h"
#include "platform_cache.h"
#include "wiced_osl.h"

#include "network/wwd_network_constants.h"
#include "platform/wwd_bus_interface.h"
#include "wwd_assert.h"
#include "wwd_rtos_isr.h"

#include "platform_appscr4.h"
#include "platform_mcu_peripheral.h"
#include "platform_m2m.h"
#include "platform_config.h"
#include "m2m_hnddma.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define VERIFY_RESULT( x )     { wiced_result_t verify_result; verify_result = (x); if ( verify_result != WICED_SUCCESS ) return verify_result; }

#define M2M_DESCRIPTOR_ALIGNMENT            16

#if !defined( WICED_DCACHE_WTHROUGH ) && defined( PLATFORM_L1_CACHE_SHIFT )
#define M2M_CACHE_WRITE_BACK                1
#define M2M_OPTIMIZED_DESCRIPTOR_ALIGNMENT  MAX(M2M_DESCRIPTOR_ALIGNMENT, PLATFORM_L1_CACHE_BYTES)
#define M2M_MEMCPY_DCACHE_CLEAN( ptr, size) platform_dcache_clean_range( ptr, size )
#else
#define M2M_CACHE_WRITE_BACK                0
#define M2M_OPTIMIZED_DESCRIPTOR_ALIGNMENT  M2M_DESCRIPTOR_ALIGNMENT
#define M2M_MEMCPY_DCACHE_CLEAN( ptr, size) do { UNUSED_PARAMETER( ptr ); UNUSED_PARAMETER( size ); cpu_data_synchronisation_barrier( ); } while(0)
#endif /* !WICED_DCACHE_WTHROUGH && PLATFORM_L1_CACHE_SHIFT */

#if defined( WICED_DCACHE_WTHROUGH ) && defined( PLATFORM_L1_CACHE_SHIFT )
#define M2M_CACHE_WRITE_THROUGH             1
#else
#define M2M_CACHE_WRITE_THROUGH             0
#endif /* WICED_DCACHE_WTHROUGH && PLATFORM_L1_CACHE_SHIFT */

/******************************************************
 *             Constants
 ******************************************************/

#define IRL_FC_SHIFT                    24 /* frame count */
#define DEF_IRL                         (1 << IRL_FC_SHIFT)

#define ACPU_DMA_TX_CHANNEL             (1)
#define ACPU_DMA_RX_CHANNEL             (0)
#define WCPU_DMA_TX_CHANNEL             (0)
#define WCPU_DMA_RX_CHANNEL             (1)
#define MEMCPY_M2M_DMA_CHANNEL          (2)

#define ACPU_DMA_RX_CHANNEL_IRQ_NUM     (1)

/* DMA tunable parameters */
#ifndef M2M_DMA_RX_BUFFER_COUNT
#define M2M_DMA_RX_BUFFER_COUNT                (20)  /*(3)*/    /* Number of rx buffers */
#endif

#ifndef M2M_DMA_TX_DESCRIPTOR_COUNT
#define M2M_DMA_TX_DESCRIPTOR_COUNT            (32)  /*(3)*/
#endif

#ifndef M2M_DMA_RX_DESCRIPTOR_COUNT
#define M2M_DMA_RX_DESCRIPTOR_COUNT            (32)
#endif

#define SDPCMD_RXOFFSET                        (8)

#define M2M_DMA_RX_BUFFER_SIZE                 WICED_LINK_MTU
#define M2M_DMA_RX_BUFFER_HEADROOM             (0)

/* M2M register mapping */
#define M2M_REG_ADR              PLATFORM_M2M_REGBASE(0)
#define M2M_DMA_ADR              PLATFORM_M2M_REGBASE(0x200)

#define ARMCR4_SW_INT0           (0x1 << 0)
#define ARMCR4_SW_INT0_STATUS    (0x1 << SW0_ExtIRQn)
#define PLATFORM_WLANCR4         ((volatile wlancr4_regs_t*)PLATFORM_WLANCR4_REGBASE(0x0))

#define M2M_WLAN_WAIT_ASSERT_SEC 3

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

typedef appscr4_regs_t wlancr4_regs_t;

/******************************************************
 *             Variables
 ******************************************************/

#ifndef WWD_BUS_TYPE_SDIO
static m2m_hnddma_t*         dma_handle = NULL;
static osl_t*                osh        = NULL;
static int                   deiniting  = 0;
static int                   initing    = 0;
#endif /* !WWD_BUS_TYPE_SDIO */
static volatile sbm2mregs_t* m2mreg     = (sbm2mregs_t *)M2M_REG_ADR;

/******************************************************
 *             Function declarations
 ******************************************************/

static inline void device_memcpy(void* dest, const void* src, size_t n);

/******************************************************
 *             Function definitions
 ******************************************************/

static void m2m_core_enable( void )
{
    osl_wrapper_enable( (void*)PLATFORM_M2M_MASTER_WRAPPER_REGBASE(0x0) );
}

#ifndef WWD_BUS_TYPE_SDIO
static void m2m_dma_enable_interrupts( wiced_bool_t enable )
{
    uint32_t rx_mask = enable ? I_DMA : 0x0;
    W_REG( osh, &m2mreg->intregs[ ACPU_DMA_RX_CHANNEL ].intmask, rx_mask );
}

void m2m_signal_txdone( void )
{
    PLATFORM_WLANCR4->sw_int = ARMCR4_SW_INT0; /* Signal WLAN core there is a TX done by setting wlancr4 SW interrupt 0 */
}

#if PLATFORM_WLAN_POWERSAVE

static wiced_bool_t m2m_wlan_is_ready( void )
{
    return ( ( m2mreg->dmaregs[ACPU_DMA_RX_CHANNEL].tx.control & D64_XC_XE ) != 0 ) ? WICED_TRUE : WICED_FALSE;
}

static wiced_bool_t m2m_tx_is_idle( void )
{
    if ( m2m_dma_txactive( dma_handle ) != 0 )
    {
        /* Still have packets in transit towards WLAN. */
        return WICED_FALSE;
    }
    if ( ( m2mreg->intregs[ACPU_DMA_TX_CHANNEL].intstatus & I_RI ) != 0 )
    {
        /*
         * WLAN still not see transmitted packet.
         * WLAN ISR pulls APPS resources up, then ack interrupt, then handle packet.
         * After pulling it is WLAN responsibility to keep resources up until handling is completed.
         * APPS must wait till RX interrupt is acknowledged to make sure APPS not went to deep-sleep and WLAN lost received packet.
         */
        return WICED_FALSE;
    }
    if ( ( PLATFORM_WLANCR4->fiqirq_status & ARMCR4_SW_INT0_STATUS ) != 0 )
    {
        /*
         * WLAN still has pending SWINT interrupt.
         * To handle it WLAN will pull resources up before.
         * To reduce chances (optimization) of APPS going down and immediately going up let's wait WLAN complete SWINT handling.
         */
        return WICED_FALSE;
    }
    return WICED_TRUE;
}

void m2m_powersave_comm_begin( void )
{
    PLATFORM_WLAN_POWERSAVE_RES_UP();
    M2M_INIT_DMA_ASYNC();
}

wiced_bool_t m2m_powersave_comm_end( wiced_bool_t tx )
{
    wiced_bool_t result = !tx || m2m_tx_is_idle( );

    if ( result )
    {
        PLATFORM_WLAN_POWERSAVE_RES_DOWN( NULL, WICED_FALSE );
    }

    return result;
}

static void m2m_powersave_comm_signal_txdone( void )
{
    m2m_powersave_comm_begin();
    m2m_signal_txdone();
    m2m_powersave_comm_end( WICED_FALSE );
}

static void m2m_powersave_comm_init_begin( void )
{
    if ( !m2m_wlan_is_ready() )
    {
        m2m_powersave_comm_signal_txdone();
    }
}

static void m2m_powersave_comm_init_done( void )
{
    uint32_t last_stamp = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
    PLATFORM_TIMEOUT_BEGIN( start_stamp );

    while ( !m2m_wlan_is_ready() )
    {
        uint32_t current_stamp = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );

        if ( ( current_stamp - last_stamp ) / 1024 > 0 )
        {
            m2m_powersave_comm_signal_txdone();
            last_stamp = current_stamp;
        }

        PLATFORM_TIMEOUT_SEC_ASSERT( "wait wlan for too long", start_stamp, m2m_wlan_is_ready(), M2M_WLAN_WAIT_ASSERT_SEC );
    }
}

#else

static inline void m2m_powersave_comm_begin( void )
{
}

static inline wiced_bool_t m2m_powersave_comm_end( wiced_bool_t tx )
{
    UNUSED_PARAMETER( tx );
    return WICED_TRUE;
}

static inline void m2m_powersave_comm_init_begin( void )
{
}

static inline void m2m_powersave_comm_init_done( void )
{
}

#endif /* PLATFORM_WLAN_POWERSAVE */

uint32_t m2m_read_intstatus( wiced_bool_t* signal_txdone )
{
    uint32_t intstatus = 0;
    uint32_t txint;
    uint32_t rxint;

    txint = R_REG( osh, &m2mreg->intregs[ ACPU_DMA_TX_CHANNEL ].intstatus );
    wiced_assert( "TX intstatus error", ( txint & I_ERRORS ) == 0 );
    if ( txint & I_XI )
    {
        W_REG( osh, &m2mreg->intregs[ ACPU_DMA_TX_CHANNEL ].intstatus, I_XI );
        intstatus |= I_XI;
    }

    rxint = R_REG( osh, &m2mreg->intregs[ ACPU_DMA_RX_CHANNEL ].intstatus );
    wiced_assert( "RX intstatus error", ( rxint & I_ERRORS ) == 0 );
    if ( rxint & I_RI )
    {
        W_REG( osh, &m2mreg->intregs[ ACPU_DMA_RX_CHANNEL ].intstatus, I_RI );
        intstatus |= I_RI;
    }

    if ( signal_txdone )
    {
        *signal_txdone = ( rxint & I_XI ) ? WICED_TRUE : WICED_FALSE;
    }

    return intstatus;
}

static void m2m_dma_set_irq_line_number( uint8_t channel, uint8_t irq_number )
{
    const uint32_t shift = channel * 2;
    const uint32_t mask  = 0x3 << shift;
    uint32_t intcontrol  = m2mreg->intcontrol;

    intcontrol &= ~mask;
    intcontrol |= ( (uint32_t)irq_number << shift ) & mask;

    m2mreg->intcontrol = intcontrol;
}


void m2m_init_dma( void )
{
    int i;
    int channel_count;

    if ( ( dma_handle != NULL ) || ( initing != 0 ) )
    {
        return;
    }

    initing = 1;

    WPRINT_PLATFORM_DEBUG(("init_m2m_dma.. txreg: %p, rxreg: %p, txchan: %d, rxchannl: %d\n",
        &(m2mreg->dmaregs[ACPU_DMA_TX_CHANNEL].tx), &(m2mreg->dmaregs[ACPU_DMA_RX_CHANNEL].rx), ACPU_DMA_TX_CHANNEL, ACPU_DMA_RX_CHANNEL));

    m2m_powersave_comm_begin();

    m2m_core_enable();

    m2m_powersave_comm_init_begin();

    osh = MALLOC(NULL, sizeof(osl_t));
    wiced_assert ("m2m_init_dma failed", osh != 0);

    /* Need to reserve 4 bytes header room so that the manipulation in wwd_bus_read_frame
     * which moves the data offset back 12 bytes would not break the data pointer */
    osh->rx_pktget_add_remove  = sizeof(wwd_buffer_header_t) - sizeof( wwd_bus_header_t );
    /* No need to do anything during freeing, host_buffer_release() works fine */
    osh->tx_pktfree_add_remove = 0;

    dma_handle = m2m_dma_attach( osh, "M2MDEV", NULL,
                                 (volatile void *)&(m2mreg->dmaregs[ACPU_DMA_TX_CHANNEL].tx),
                                 (volatile void *)&(m2mreg->dmaregs[ACPU_DMA_RX_CHANNEL].rx),
                                 M2M_DMA_TX_DESCRIPTOR_COUNT,
                                 M2M_DMA_RX_DESCRIPTOR_COUNT,
                                 M2M_DMA_RX_BUFFER_SIZE,
                                 M2M_DMA_RX_BUFFER_HEADROOM,
                                 M2M_DMA_RX_BUFFER_COUNT,
                                 SDPCMD_RXOFFSET,
                                 NULL );

    wiced_assert ("m2m_dma_attach failed", dma_handle != 0);

    m2m_dma_rxinit( dma_handle );
    m2m_dma_rxfill( dma_handle );
    W_REG( osh, &m2mreg->intrxlazy[ACPU_DMA_RX_CHANNEL], DEF_IRL );

    /* Configure all m2m channels except ACPU_DMA_TX_CHANNEL to use irq1 and remap the m2m irq */
    channel_count = (m2mreg->capabilities & M2M_CAPABILITIES_CHANNEL_COUNT_MASK) + 1;
    for ( i = 0; i < channel_count ; i++ )
    {
        if ( i != ACPU_DMA_TX_CHANNEL)
        {
            m2m_dma_set_irq_line_number( i, ACPU_DMA_RX_CHANNEL_IRQ_NUM );
        }
    }
    platform_irq_remap_sink    ( OOB_AOUT_M2M_INTR1, M2M_ExtIRQn );
#ifndef M2M_RX_POLL_MODE
    m2m_dma_enable_interrupts  ( WICED_TRUE );
#else
    m2m_dma_enable_interrupts  ( WICED_FALSE );
#endif /* M2M_RX_POLL_MODE */

    m2m_dma_txinit( dma_handle ); /* Last step of initialization. Register settings inside tells WLAN that RX is ready. */

    m2m_powersave_comm_init_done();

    m2m_powersave_comm_end( WICED_FALSE );

    initing = 0;
}

int m2m_is_dma_inited( void )
{
    return ( dma_handle != NULL );
}

void m2m_dma_tx_reclaim( void )
{
    void* txd;

    while ( ( txd = m2m_dma_getnexttxp( dma_handle, FALSE ) ) != NULL )
    {
        PKTFREE( osh, txd, TRUE );
    }
}

void* m2m_read_dma_packet( uint16_t** hwtag )
{
    void* packet = NULL;

retry:
    packet = m2m_dma_rx( dma_handle );
    if ( packet == NULL )
    {
        return NULL;
    }

    PKTPULL(osh, packet, (uint16)SDPCMD_RXOFFSET);

    *hwtag = (uint16_t*) PKTDATA(osh, packet);

    if ( *hwtag == NULL )
    {
        PKTFREE(osh, packet, FALSE);
        goto retry;
    }

    if ( ( (*hwtag)[0] == 0 ) &&
         ( (*hwtag)[1] == 0 ) &&
         ( PKTLEN(osh, packet) == 504 ) )
    {
        // Dummy frame to keep M2M happy
        PKTFREE( osh, packet, FALSE );
        goto retry;
    }

    if ( ( ( (*hwtag)[0] | (*hwtag)[1] ) == 0                 ) ||
         ( ( (*hwtag)[0] ^ (*hwtag)[1] ) != (uint16_t) 0xFFFF ) )
    {
        WPRINT_PLATFORM_DEBUG(("Hardware Tag mismatch..\n"));

        PKTFREE(osh, packet, FALSE);

        goto retry;
    }
    return packet;
}

void m2m_refill_dma( void )
{
    if ( deiniting == 0 )
    {
        if ( m2m_dma_rxfill( dma_handle ) )
        {
#ifdef WPRINT_ENABLE_PLATFORM_DEBUG
            static int out_of_memory_count = 0;
#endif /* WPRINT_ENABLE_PLATFORM_DEBUG */
            /* ran out of buffers to receive from the WLAN module; log it */
            WPRINT_PLATFORM_DEBUG(("out of memory: DMA RX fill left empty ring!"));
            WPRINT_PLATFORM_DEBUG(("(%d times)\n", ++out_of_memory_count));
        }
    }
}

int m2m_rxactive_dma( void )
{
    return m2m_dma_rxactive( dma_handle );
}

int m2m_dma_tx_data( void* buffer )
{
    return m2m_dma_txfast( dma_handle, buffer, TRUE ); /* function release packet inside if failed */
}

void m2m_deinit_dma( void )
{
    deiniting = 1;
    m2m_dma_enable_interrupts( WICED_FALSE );
    m2m_dma_txreset  ( dma_handle );
    m2m_dma_rxreset  ( dma_handle );
    m2m_dma_rxreclaim( dma_handle );
    m2m_dma_txreclaim( dma_handle, HNDDMA_RANGE_ALL );
    m2m_dma_detach   ( dma_handle );
    MFREE(NULL, osh, sizeof(osl_t));
    osh = NULL;
    dma_handle = NULL;
    deiniting = 0;
}
#endif /* !WWD_BUS_TYPE_SDIO */

void m2m_switch_off_dma_post_completion( void )
{
    int      is_irq_enabled;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS( flags );

    is_irq_enabled = platform_irq_is_irq_enabled( M2M_ExtIRQn );

    /* Wait till DMA has finished */
    while ( 1 )
    {
         m2mreg->intregs[MEMCPY_M2M_DMA_CHANNEL].intmask = I_RI | I_ERRORS;
         platform_irq_enable_irq( M2M_ExtIRQn );

         cpu_wait_for_interrupt( );

         m2mreg->intregs[MEMCPY_M2M_DMA_CHANNEL].intmask = 0x0;
         (void)m2mreg->intregs[MEMCPY_M2M_DMA_CHANNEL].intmask; /* read back */
         if ( !is_irq_enabled )
         {
             platform_irq_disable_irq( M2M_ExtIRQn );
         }

         WICED_RESTORE_INTERRUPTS( flags );

         wiced_assert( "TX failed", (m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.status0 & D64_XS0_XS_MASK) != D64_XS0_XS_STOPPED/* error*/ );
         wiced_assert( "RX failed", (m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.status0 & D64_XS0_XS_MASK) != D64_XS0_XS_STOPPED/* error*/ );

         if ( ( (m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.status0 & D64_XS0_XS_MASK) != D64_XS0_XS_ACTIVE ) &&
              ( (m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.status0 & D64_XS0_XS_MASK) != D64_XS0_XS_ACTIVE ) )
         {
             break;
         }

         WICED_SAVE_INTERRUPTS( flags );
    }

    /* Switch off the DMA */
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.control &= (~D64_XC_XE);
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.control &= (~D64_XC_XE);
    m2mreg->intregs[MEMCPY_M2M_DMA_CHANNEL].intstatus = ~0x0;
}

void m2m_post_dma_completion_operations( void* destination, uint32_t byte_count )
{
#if M2M_CACHE_WRITE_THROUGH
    /* Prepares the cache for reading the finished DMA data. */
    platform_dcache_inv_range( destination, byte_count );
#else
    UNUSED_PARAMETER( destination );
    UNUSED_PARAMETER( byte_count );
#endif /* M2M_CACHE_WRITE_THROUGH */
    /* Indicate empty table. Otherwise core is not freeing PMU resources. */
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.ptr = m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.addrlow & 0xFFFF;
}

static inline void device_memcpy(void* dest, const void* src, size_t n)
{
    char *d = (char*)dest;
    char *s = (char*)src;

    while(n--)
    {
    *d++ = *s++;
    }
}

int m2m_unprotected_dma_memcpy( void* destination, const void* source, uint32_t byte_count, wiced_bool_t blocking )
{
#if M2M_CACHE_WRITE_BACK

    /* It is unsafe to perform DMA to partial cache lines, since it is impossible to predict when a
     * cache line will be evicted back to main memory, and such an event would overwrite the DMA data.
     * Hence it is required that any cache lines which contain other data must be manually copied.
     * These will be the first & last cache lines.
     */
    uint32_t start_offset = PLATFORM_L1_CACHE_LINE_OFFSET( destination );
    if ( start_offset != 0 )
    {
        /* Start of destination not at start of cache line
         * memcpy the first part of source data to destination
         */
        uint32_t start_copy_byte_count = MIN( byte_count, PLATFORM_L1_CACHE_BYTES - start_offset );
        device_memcpy( destination, source, start_copy_byte_count );
        source      += start_copy_byte_count;
        destination += start_copy_byte_count;
        byte_count  -= start_copy_byte_count;
    }

    uint32_t end_offset   = ((uint32_t) destination + byte_count) & PLATFORM_L1_CACHE_LINE_MASK;
    if ( ( byte_count > 0 ) && ( end_offset != 0 ))
    {
        /* End of destination not at end of cache line
         * memcpy the last part of source data to destination
         */
        uint32_t end_copy_byte_count = MIN( byte_count, end_offset );
        uint32_t offset_from_start = byte_count - end_copy_byte_count;

        device_memcpy( (char*)destination + offset_from_start, (char*)source + offset_from_start, end_copy_byte_count );
        byte_count  -= end_copy_byte_count;
    }

    /* Remaining data should be fully aligned to cache lines */
    wiced_assert( "check alignment achieved by copy", ( byte_count == 0 ) || ( ( (uint32_t)destination & PLATFORM_L1_CACHE_LINE_MASK ) == 0 ) );
    wiced_assert( "check alignment achieved by copy", ( byte_count == 0 ) || ( ( ( (uint32_t)destination + byte_count ) & PLATFORM_L1_CACHE_LINE_MASK ) == 0 ) );

#endif /* M2M_CACHE_WRITE_BACK */

    /* Check if there are any remaining cache lines (that are entirely part of destination buffer) */
    if ( byte_count <= 0 )
    {
        return 0;
    }

    /* Allocate descriptors */
    static volatile dma64dd_t PLATFORM_DMA_DESCRIPTORS_SECTION(m2m_descriptors)[2] ALIGNED(M2M_OPTIMIZED_DESCRIPTOR_ALIGNMENT);

    /* Prepare M2M engine */
    m2m_core_enable( );

    /* Enable interrupts generation after each frame */
    m2mreg->intrxlazy[MEMCPY_M2M_DMA_CHANNEL] = DEF_IRL;

    /* Setup descriptors */
    m2m_descriptors[0].ctrl1 = D64_CTRL1_EOF | D64_CTRL1_SOF;
    m2m_descriptors[1].ctrl1 = D64_CTRL1_EOF | D64_CTRL1_SOF | D64_CTRL1_IOC;

    /* Setup DMA channel transmitter */
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.addrhigh = 0;
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.addrlow  = ((uint32_t)&m2m_descriptors[0]);  // Transmit descriptor table address
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.ptr      = ((uint32_t)&m2m_descriptors[0] + sizeof(dma64dd_t)) & 0xffff; // needs to be lower 16 bits of descriptor address
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.control  = D64_XC_PD | ((2 << D64_XC_BL_SHIFT) & D64_XC_BL_MASK) | ((1 << D64_XC_PC_SHIFT) & D64_XC_PC_MASK);

    /* Setup DMA channel receiver */
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.addrhigh = 0;
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.addrlow  = ((uint32_t)&m2m_descriptors[1]);  // Transmit descriptor table address
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.ptr      = ((uint32_t)&m2m_descriptors[1] + sizeof(dma64dd_t)) & 0xffff; // needs to be lower 16 bits of descriptor address
    m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.control  = D64_XC_PD | ((2 << D64_XC_BL_SHIFT) & D64_XC_BL_MASK) | ((1 << D64_XC_PC_SHIFT) & D64_XC_PC_MASK);

    /* Flush pending writes in source buffer range to main memory, so they are accessable by the DMA */
    M2M_MEMCPY_DCACHE_CLEAN( source, byte_count );

    void*    original_destination = destination;
    uint32_t original_byte_count  = byte_count;

#if M2M_CACHE_WRITE_BACK
    /* Invalidate destination buffer - required so that any pending write data is not
     * written back (evicted) over the top of the DMA data.
     * This also prepares the cache for reading the finished DMA data.  However if other functions are silly
     * enough to read the DMA data before it is finished, the cache may get out of sync.
     */
    platform_dcache_inv_range( destination, byte_count );
#endif /* M2M_CACHE_WRITE_BACK */

    wiced_assert( "nonblocking m2m dma memcpy supports max payload of D64_CTRL2_BC_USABLE_MASK",
     ( ( blocking != WICED_FALSE ) ||  ( byte_count <=  D64_CTRL2_BC_USABLE_MASK ) ) );

    uint32_t dma_source = platform_addr_cpu_to_dma( source );
    uint32_t dma_destination = platform_addr_cpu_to_dma( destination );
    while ( byte_count > 0 )
    {
        uint32_t write_size = MIN( byte_count, D64_CTRL2_BC_USABLE_MASK );

        m2m_descriptors[0].addrlow = dma_source;
        m2m_descriptors[1].addrlow = dma_destination;
        m2m_descriptors[0].ctrl2   = D64_CTRL2_BC_USABLE_MASK & write_size;
        m2m_descriptors[1].ctrl2   = D64_CTRL2_BC_USABLE_MASK & write_size;

        /* Flush the DMA descriptors to main memory to ensure DMA picks them up */
        M2M_MEMCPY_DCACHE_CLEAN( (void*)&m2m_descriptors[0], sizeof(m2m_descriptors) );

        /* Fire off the DMA */
        m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].rx.control |= D64_XC_XE;
        m2mreg->dmaregs[MEMCPY_M2M_DMA_CHANNEL].tx.control |= D64_XC_XE;

        /* Update variables */
        byte_count      -= write_size;
        dma_destination += write_size;
        dma_source      += write_size;

        if ( blocking == WICED_TRUE )
        {
            m2m_switch_off_dma_post_completion( );
        }
    }

    if ( blocking == WICED_TRUE )
    {
        m2m_post_dma_completion_operations( original_destination, original_byte_count );
    }

    return 0;
}

#ifndef WWD_BUS_TYPE_SDIO
void m2m_wlan_dma_deinit( void )
{
    /* Stop WLAN side M2M dma */
    m2m_dma_txreset_inner(osh, (void *)&m2mreg->dmaregs[ACPU_DMA_RX_CHANNEL].tx);
    m2m_dma_rxreset_inner(osh, (void *)&m2mreg->dmaregs[ACPU_DMA_TX_CHANNEL].rx);

    /* Reset wlan m2m tx/rx pointers */
    W_REG(osh, &m2mreg->dmaregs[ACPU_DMA_RX_CHANNEL].tx.ptr, 0);
    W_REG(osh, &m2mreg->dmaregs[ACPU_DMA_TX_CHANNEL].rx.ptr, 0);

    /* Mask interrupt & clear all interrupt status */
    W_REG(osh, &m2mreg->intregs[ACPU_DMA_TX_CHANNEL].intmask, 0);
    W_REG(osh, &m2mreg->intregs[ACPU_DMA_RX_CHANNEL].intstatus, 0xffffffff);
    W_REG(osh, &m2mreg->intregs[ACPU_DMA_TX_CHANNEL].intstatus, 0xffffffff);
}

#ifndef M2M_RX_POLL_MODE

/* M2M DMA:
 * APPS: TX/RX : M2M channel 1/channel 0
 * WLAN: TX/RX : M2M channel 0/channel 1
 * APPS and WLAN core only enable RX interrupt. Regarding TX DONE interrupt,
 * APPS core uses SW0 interrupt to signal WLAN core once there is TX DONE interrupt happening.
 * WLAN core also uses SW0 interrupt to signal APPS core the TX DONE interrupt.
 * */
WWD_RTOS_DEFINE_ISR( platform_m2mdma_isr )
{
    host_platform_bus_disable_interrupt();
    wwd_thread_notify_irq();
}
WWD_RTOS_MAP_ISR( platform_m2mdma_isr, M2M_ISR)
WWD_RTOS_MAP_ISR( platform_m2mdma_isr, Sw0_ISR)

#endif /* !M2M_RX_POLL_MODE */
#endif /* !WWD_BUS_TYPE_SDIO */
