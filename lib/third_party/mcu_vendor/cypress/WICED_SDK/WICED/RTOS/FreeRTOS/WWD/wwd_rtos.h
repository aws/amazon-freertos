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
 *  Definitions for the FreeRTOS implementation of the Wiced RTOS
 *  abstraction layer.
 *
 */

#ifndef INCLUDED_WWD_RTOS_H_
#define INCLUDED_WWD_RTOS_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "wwd_FreeRTOS_systick.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void vPortSVCHandler    ( void );
extern void xPortPendSVHandler ( void );
extern void xPortSysTickHandler( void );

/* Define interrupt handlers needed by FreeRTOS. These defines are used by the
 * vector table.
 */
#define SVC_irq     vPortSVCHandler
#define PENDSV_irq  xPortPendSVHandler
#define SYSTICK_irq xPortSysTickHandler


#define RTOS_HIGHER_PRIORTIY_THAN(x)     (x < RTOS_HIGHEST_PRIORITY ? x+1 : RTOS_HIGHEST_PRIORITY)
#define RTOS_LOWER_PRIORTIY_THAN(x)      (x > RTOS_LOWEST_PRIORITY ? x-1 : RTOS_LOWEST_PRIORITY)
#define RTOS_LOWEST_PRIORITY             (0)
#define RTOS_HIGHEST_PRIORITY            (configMAX_PRIORITIES-1)
#define RTOS_DEFAULT_THREAD_PRIORITY     (1)

#define RTOS_USE_DYNAMIC_THREAD_STACK

#ifndef WWD_LOGGING_STDOUT_ENABLE
#ifdef DEBUG
#define WWD_THREAD_STACK_SIZE        (1600 + 1400)   /* Stack checking requires a larger stack */
#else /* ifdef DEBUG */
#define WWD_THREAD_STACK_SIZE        (1400 + 1400)
#endif /* ifdef DEBUG */
#else /* if WWD_LOGGING_STDOUT_ENABLE */
#define WWD_THREAD_STACK_SIZE        (1400 + 4096 + 1400) /* WWD_LOG uses printf and requires a minimum of 4K stack */
#endif /* WWD_LOGGING_STDOUT_ENABLE */

/******************************************************
 *             Structures
 ******************************************************/

typedef SemaphoreHandle_t   /*@abstract@*/ /*@only@*/ host_semaphore_type_t;  /** FreeRTOS definition of a semaphore */
typedef SemaphoreHandle_t   /*@abstract@*/ /*@only@*/ host_mutex_type_t;  /** FreeRTOS definition of a mutex */
typedef TaskHandle_t        /*@abstract@*/ /*@only@*/ host_thread_type_t;     /** FreeRTOS definition of a thread handle */
typedef QueueHandle_t       /*@abstract@*/ /*@only@*/ host_queue_type_t;      /** FreeRTOS definition of a message queue */

/*@external@*/ extern void vApplicationMallocFailedHook( void );
/*@external@*/ extern void vApplicationIdleSleepHook( void );

typedef struct
{
    uint8_t info;    /* not supported yet */
} host_rtos_thread_config_type_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_RTOS_H_ */
