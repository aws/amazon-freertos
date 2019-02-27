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
 *  The Wiced Thread allows thread safe access to the Wiced hardware bus
 *  This is an Wiced internal file and should not be used by functions outside Wiced.
 *
 *  This file provides prototypes for functions which allow multiple threads to use the Wiced hardware bus (SDIO or SPI)
 *  This is achieved by having a single thread (the "Wiced Thread") which queues messages to be sent, sending
 *  them sequentially, as well as receiving messages as they arrive.
 *
 *  Messages to be sent come from the @ref wwd_sdpcm_send_common function in wwd_sdpcm.c .  The messages already
 *  contain SDPCM headers, but not any bus headers (GSPI), and are passed via a queue
 *  This function can be called from any thread.
 *
 *  Messages are received by way of a callback supplied by in wwd_sdpcm.c - wwd_sdpcm_process_rx_packet
 *  Received messages are delivered in the context of the Wiced Thread, so the callback function needs to avoid blocking.
 *
 */

#ifndef INCLUDED_WWD_THREAD_H_
#define INCLUDED_WWD_THREAD_H_

#include "wwd_buffer.h"
#include "wwd_constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define WWD_THREAD_PRIORITY   RTOS_HIGHEST_PRIORITY

/** Initialises the Wiced Thread
 *
 * Initialises the Wiced thread, and its flags/semaphores,
 * then starts it running
 *
 * @return    wiced result code
 */
extern wwd_result_t wwd_thread_init( void )  /*@modifies internalState@*/;


/** Terminates the Wiced Thread
 *
 * Sets a flag then wakes the Wiced Thread to force it to terminate.
 *
 */
extern void wwd_thread_quit( void );


extern void wwd_thread_notify( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_THREAD_H_ */
