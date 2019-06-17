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
#pragma once

#include "wwd_rtos.h"
#include "platform_isr.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/* Use this macro to define an RTOS-aware interrupt handler where RTOS
 * primitives can be safely accessed.
 * Current port has no vectored interrupt controller, so single entry point
 * to all interrupts is used. And this entry is wrapped with RTOS-aware code,
 * so all interrupts are actually RTOS-aware.
 *
 * @usage:
 * WWD_RTOS_DEFINE_ISR( my_irq_handler )
 * {
 *     // Do something here
 * }
 */
#define WWD_RTOS_DEFINE_ISR( function )          PLATFORM_DEFINE_ISR( function )

/* Macro for mapping a function defined using WWD_RTOS_DEFINE_ISR
 * to an interrupt handler declared in
 * <Wiced-SDK>/WICED/platform/MCU/<Family>/platform_isr_interface.h
 *
 * @usage:
 * WWD_RTOS_MAP_ISR( my_irq, USART1_irq )
 */
#define WWD_RTOS_MAP_ISR( function, isr )        PLATFORM_MAP_ISR( function, isr )


/* Use this macro to define function which serves as ISR demuxer.
 * It is used when no vectored interrupt controller, and single
 * vector triggered for all interrupts.
 *
 * @usage:
 * WWD_RTOS_DEFINE_ISR_DEMUXER( my_irq_handler )
 * {
 *     // Do something here
 * }
 */
#define WWD_RTOS_DEFINE_ISR_DEMUXER( function )  PLATFORM_DEFINE_ISR( function )

/* Macro to declare that function is ISR demuxer.
 * Function has to be defined via WWD_RTOS_DEFINE_ISR_DEMUXER
 *
 * @usage:
 * WWD_RTOS_MAP_ISR_DEMUXER( my_irq_demuxer )
 */
#define WWD_RTOS_MAP_ISR_DEMUXER( function )     PLATFORM_MAP_ISR( function, vApplicationIRQHandler )


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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/


#ifdef __cplusplus
} /* extern "C" */
#endif
