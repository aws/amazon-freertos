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
 *  Platform ChipCommon Functions
 */

#include <stdint.h>
#include "platform_appscr4.h"
#include "wiced_platform.h"
#include "wwd_assert.h"
#include "wwd_rtos_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* ChipCommon IntStatus and IntMask register bits */
#define CC_INT_STATUS_MASK_GPIOINT             (1 << 0)
#define CC_INT_STATUS_MASK_EXTINT              (1 << 1)
#define CC_INT_STATUS_MASK_ECIGCIINT           (1 << 4)
#define CC_INT_STATUS_MASK_PMUINT              (1 << 5)
#define CC_INT_STATUS_MASK_UARTINT             (1 << 6)
#define CC_INT_STATUS_MASK_SECIGCIWAKEUPINT    (1 << 7)
#define CC_INT_STATUS_MASK_SPMINT              (1 << 8)
#define CC_INT_STATUS_MASK_ASCURXINT           (1 << 9)
#define CC_INT_STATUS_MASK_ASCUTXINT           (1 << 10)
#define CC_INT_STATUS_MASK_ASCUASTPINT         (1 << 11)

/* ChipCommon IntStatus register bit only */
#define CC_INT_STATUS_WDRESET                  (1 << 31)

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

/* Extern'ed from platforms/<43909_Platform>/platform.c */
extern platform_uart_driver_t platform_uart_drivers[];

/******************************************************
 *               Function Definitions
 ******************************************************/

/* Disable external interrupts from ChipCommon to APPS Core */
void platform_chipcommon_disable_irq( void )
{
    platform_irq_disable_irq( ChipCommon_ExtIRQn );
}

/* Enable external interrupts from ChipCommon to APPS Core */
void platform_chipcommon_enable_irq( void )
{
    platform_irq_enable_irq(ChipCommon_ExtIRQn);
}

WWD_RTOS_DEFINE_ISR( platform_chipcommon_isr )
{
    uint32_t int_mask;
    uint32_t int_status;

    int_mask   = PLATFORM_CHIPCOMMON->interrupt.mask.raw;
    int_status = PLATFORM_CHIPCOMMON->interrupt.status.raw;

    /* DeMultiplex the ChipCommon interrupt */
    if ( ( ( int_status & CC_INT_STATUS_MASK_GPIOINT ) != 0 ) && ( ( int_mask & CC_INT_STATUS_MASK_GPIOINT ) != 0 ) )
    {
        /* GPIO interrupt */
        platform_gpio_irq();
    }

    if ( ( ( int_status & CC_INT_STATUS_MASK_UARTINT ) != 0 ) && ( ( int_mask & CC_INT_STATUS_MASK_UARTINT ) != 0 ) )
    {
#ifndef BCM4390X_UART_SLOW_POLL_MODE
        /* Slow UART interrupt */
        platform_uart_irq(&platform_uart_drivers[WICED_UART_1]);
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */
    }

    if ( ( ( int_status & CC_INT_STATUS_MASK_ECIGCIINT ) != 0 ) && ( ( int_mask & CC_INT_STATUS_MASK_ECIGCIINT ) != 0 ) )
    {
#ifndef BCM4390X_UART_FAST_POLL_MODE
        /* Fast UART interrupt */
        platform_uart_irq(&platform_uart_drivers[WICED_UART_2]);
#endif /* !BCM4390X_UART_FAST_POLL_MODE */

#ifndef BCM4390X_UART_GCI_POLL_MODE
        /* GCI UART interrupt */
        platform_uart_irq(&platform_uart_drivers[WICED_UART_3]);
#endif /* !BCM4390X_UART_GCI_POLL_MODE */
    }

    if ( ( ( int_status & CC_INT_STATUS_MASK_ASCURXINT ) != 0 ) && ( ( int_mask & CC_INT_STATUS_MASK_ASCURXINT ) != 0 ) )
    {
        platform_ascu_irq(int_status);
    }

    if ( ( ( int_status & CC_INT_STATUS_MASK_ASCUTXINT ) != 0 ) && ( ( int_mask & CC_INT_STATUS_MASK_ASCUTXINT ) != 0 ) )
    {
        platform_ascu_irq(int_status);
    }

    if ( ( ( int_status & CC_INT_STATUS_MASK_ASCUASTPINT ) != 0 ) && ( ( int_mask & CC_INT_STATUS_MASK_ASCUASTPINT ) != 0 ) )
    {
        platform_ascu_irq(int_status);
    }
}

WWD_RTOS_MAP_ISR( platform_chipcommon_isr, ChipCommon_ISR )
