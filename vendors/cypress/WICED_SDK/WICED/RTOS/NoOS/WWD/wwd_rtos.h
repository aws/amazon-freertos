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
 *  Definitions of the Wiced RTOS abstraction layer for the special case
 *  of having no RTOS
 *
 */

#ifndef INCLUDED_WWD_RTOS_H_
#define INCLUDED_WWD_RTOS_H_

#include "platform_isr.h"
#include "noos.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RTOS_HIGHER_PRIORTIY_THAN(x)     (x)
#define RTOS_LOWER_PRIORTIY_THAN(x)      (x)
#define RTOS_LOWEST_PRIORITY             (0)
#define RTOS_HIGHEST_PRIORITY            (0)
#define RTOS_DEFAULT_THREAD_PRIORITY     (0)

#define RTOS_USE_DYNAMIC_THREAD_STACK
#define WWD_THREAD_STACK_SIZE            (544 + 1400)

/*
 * The number of system ticks per second
 */
#define SYSTICK_FREQUENCY  (1000)

/******************************************************
 *             Structures
 ******************************************************/

typedef volatile unsigned char   host_semaphore_type_t;  /** NoOS definition of a semaphore */
typedef volatile unsigned char   host_mutex_type_t;      /** NoOS definition of a mutex */
typedef volatile unsigned char   host_thread_type_t;     /** NoOS definition of a thread handle - Would be declared void but that is not allowed. */
typedef volatile unsigned char   host_queue_type_t;      /** NoOS definition of a message queue */

typedef struct
{
    uint8_t info;    /* not supported yet */
} host_rtos_thread_config_type_t;

/*@external@*/ void NoOS_setup_timing( void );
/*@external@*/ void NoOS_stop_timing( void );
/*@external@*/ void NoOS_systick_irq( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_RTOS_H_ */
