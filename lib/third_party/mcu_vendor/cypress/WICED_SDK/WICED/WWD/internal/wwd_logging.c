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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "wwd_logging.h"

#ifdef WWD_LOGGING_STDOUT_ENABLE
int                  wwd_logging_enabled = 0;

#elif defined WWD_LOGGING_BUFFER_ENABLE

#ifndef LOGGING_BUFFER_SIZE
#error LOGGING_BUFFER_SIZE is not defined
#endif /* LOGGING_BUFFER_SIZE */

static unsigned long logging_buffer_position = 0;
static char          logging_buffer[LOGGING_BUFFER_SIZE];
int                  wwd_logging_enabled = 0;

int wwd_logging_printf(const char *format, ...)
{
    int potential_num_written = 0;
    va_list args;
    va_start (args, format);
    potential_num_written = vsnprintf (&logging_buffer[logging_buffer_position], (size_t)(LOGGING_BUFFER_SIZE - logging_buffer_position), format, args);
    if ( potential_num_written > (int)(LOGGING_BUFFER_SIZE - logging_buffer_position) )
    {
        /* full print did not fit in buffer - wipe what was just written and reprint at start of buffer */
        memset( &logging_buffer[logging_buffer_position], (int)'\n', (size_t)(LOGGING_BUFFER_SIZE - logging_buffer_position ) );
        logging_buffer_position = 0;
        potential_num_written = vsnprintf (&logging_buffer[logging_buffer_position], (size_t)(LOGGING_BUFFER_SIZE - logging_buffer_position), format, args);
    }
    logging_buffer_position += (unsigned)potential_num_written;
    if ( logging_buffer_position >= LOGGING_BUFFER_SIZE )
    {
        logging_buffer_position = 0;
    }

    va_end (args);
    return potential_num_written;
}

void wwd_get_logbuffer(uint8_t **bufptr)
{
    *bufptr = (uint8_t *)logging_buffer;
}

#endif /* ifdef WWD_LOGGING_BUFFER_ENABLE */

