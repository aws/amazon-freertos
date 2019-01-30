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
 * BCM43909 vector table
 */

#include "cr4.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "platform_appscr4.h"
#include "platform_mcu_peripheral.h"
#include "wwd_rtos_isr.h"

#include "typedefs.h"
#include "sbchipc.h"
#include "aidmp.h"

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

typedef struct
{
    ExtIRQn_Type irqn;
    void (*isr)( void );
} interrupt_vector_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

interrupt_vector_t interrupt_vector_table[] =
{
#ifdef BOOTLOADER
    {
        .irqn  = Timer_ExtIRQn,
        .isr   = Timer_ISR,
    },
#else
    {
        .irqn  = SW0_ExtIRQn,
        .isr   = Sw0_ISR,
    },
    {
        .irqn  = SW1_ExtIRQn,
        .isr   = Sw1_ISR,
    },
    {
        .irqn  = Timer_ExtIRQn,
        .isr   = Timer_ISR,
    },
    {
        .irqn  = SDIO_REMAPPED_ExtIRQn,
#ifdef GSPI_SLAVE_ENABLE
        .isr   = GSPI_SLAVE_ISR,
#else
        .isr   = SDIO_HOST_ISR,
#endif
    },
    {
        .irqn  = USB_REMAPPED_ExtIRQn,
        .isr   = USB_HOST_ISR,
    },
    {
        .irqn  = GMAC_ExtIRQn,
        .isr   = GMAC_ISR,
    },
    {
        .irqn  = Serror_ExtIRQn,
        .isr   = Serror_ISR,
    },
#ifndef M2M_RX_POLL_MODE
    {
        .irqn  = M2M_ExtIRQn,
        .isr   = M2M_ISR,
    },
#endif
    {
        .irqn  = I2S0_ExtIRQn,
        .isr   = I2S0_ISR,
    },
    {
        .irqn  = I2S1_ExtIRQn,
        .isr   = I2S1_ISR,
    },
    {
        .irqn  = ChipCommon_ExtIRQn,
        .isr   = ChipCommon_ISR,
    },
#endif
};

/******************************************************
 *               Function Definitions
 ******************************************************/

uint32_t WEAK NEVER_INLINE platform_irq_demuxer_hook( uint32_t irq_status )
{
    return irq_status;
}

WWD_RTOS_DEFINE_ISR_DEMUXER( platform_irq_demuxer )
{
    uint32_t mask = PLATFORM_APPSCR4->irq_mask;
    uint32_t status = mask & PLATFORM_APPSCR4->fiqirq_status;

    if ( status )
    {
        unsigned i;

        PLATFORM_APPSCR4->fiqirq_status = status;

#if PLATFORM_IRQ_DEMUXER_HOOK
        status = platform_irq_demuxer_hook( status );
#endif

        for ( i = 0; status && (i < ARRAYSIZE(interrupt_vector_table)); ++i )
        {
            uint32_t irqn_mask = IRQN2MASK(interrupt_vector_table[i].irqn);

            if ( status & irqn_mask )
            {
                interrupt_vector_table[i].isr();
                status &= ~irqn_mask;
            }
        }
    }

    if ( status )
    {
        Unhandled_ISR();
    }

    cpu_data_synchronisation_barrier( );
}

WWD_RTOS_MAP_ISR_DEMUXER( platform_irq_demuxer )

void platform_irq_init( void )
{
    PLATFORM_APPSCR4->irq_mask = 0;
    PLATFORM_APPSCR4->int_mask = 0;

    platform_tick_irq_init( );
}

platform_result_t platform_irq_remap_sink( uint8_t bus_line_num, uint8_t sink_num )
{
    volatile uint32_t* oobselina = (volatile uint32_t*)( PLATFORM_APPSCR4_MASTER_WRAPPER_REGBASE(0x0) + ( ( sink_num < 4) ? AI_OOBSELINA30 : AI_OOBSELINA74 ) );
    int shift = ( sink_num % 4 ) * 8;

    if ( ( bus_line_num >= AI_OOBSEL_BUSLINE_COUNT ) || ( sink_num >= AI_OOBSEL_SINK_COUNT ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    *oobselina = ( *oobselina & ~( (uint32_t)AI_OOBSEL_MASK << shift ) ) | ( (uint32_t)bus_line_num << shift );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_irq_remap_source( uint32_t wrapper_addr, uint8_t source_num, uint8_t bus_line_num )
{
    volatile uint32_t* oobselouta = (volatile uint32_t*)( wrapper_addr + ( ( source_num < 4) ? AI_OOBSELOUTA30 : AI_OOBSELOUTA74 ) );
    int shift = ( source_num % 4 ) * 8;

    if ( ( bus_line_num >= AI_OOBSEL_BUSLINE_COUNT ) || ( source_num >= AI_OOBSEL_SOURCE_COUNT ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    *oobselouta = ( *oobselouta & ~( (uint32_t)AI_OOBSEL_MASK << shift ) ) | ( (uint32_t)bus_line_num << shift );

    return PLATFORM_SUCCESS;
}
